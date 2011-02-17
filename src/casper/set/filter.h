/***************************************************************************
 *   Copyright (C) 2003/2007 by Marco Correia                              *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _H_CASPER_SET_FILTER
#define _H_CASPER_SET_FILTER

#include <casper/set/relation.h>
#include <casper/set/idxsetfd.h>
#include <casper/set/util.h>
#include <casper/set/chkview.h>
#include <casper/kernel/filter/common.h>

#include <casper/int/view.h>

#include <algorithm>

namespace casper {

/**
 * \defgroup SetFilters Set filtering
 * Filters specific to set domain variables.
 * \ingroup Set
 * \ingroup Filtering
 */
/*@{*/

template<class View>
struct ValView<Int,Rel1<Cardinal,View> >
{
	typedef typename EvalTraits<View>::Eval::Elem	Elem;
	ValView(ICPSolver& solver, const Rel1<Cardinal,View>& v) :
		v(solver,v.p1) {}
	Bool ground() const { return v->card().ground(); 	}
	Int value() const { return v->card().value();	}
	Bool setValue(const Int& val) { return v->card()=val;	}
	Void attach(INotifiable* n)
	{	v->card().attachOnGround(n);	}
	Void detach(INotifiable* n)
	{	v->card().detachOnGround(n);	}
	Rel1<Cardinal,View>	getView() const
	{	return cardinal(v.getView());	}
	ICPSolver& solver() const	{	return v.solver();	}

	DomView<Set<Elem>,View> v;
};

template<class View>
struct BndView<Int,Rel1<Cardinal,View> >
{
	typedef typename EvalTraits<View>::Eval::Elem	Elem;
	BndView(ICPSolver& solver, const Rel1<Cardinal,View>& v) :
		v(solver,v.p1) {}
	Int min() const { return v->card().min(); 	}
	Int max() const { return v->card().max();	}
	Bool updateMin(const Int& val)
	{ 	return v->card().updateMin(val); }
	Bool updateMax(const Int& val)
	{ 	return v->card().updateMax(val); }
	Bool updateRange(const Int& lb, const Int& ub)
	{	return v->card().updateRange(lb,ub);	}
	Void range(Int& lb, Int& ub) const
	{	lb = min(); ub = max();	}
	Void attach(INotifiable* n)
	{	v->card().attachOnBounds(n);	}
	Void detach(INotifiable* n)
	{	v->card().detachOnBounds(n);	}
	ICPSolver& solver() const	{	return v.solver();	}
	Rel1<Cardinal,View>	getView() const
	{	return cardinal(v.getView());	}

	DomView<Set<Elem>,View> v;
};

template<class View,class Dom>
struct DomView<Int,Rel1<Cardinal,View>,Dom>
{
	typedef typename EvalTraits<View>::Eval::Elem	Elem;
	DomView(ICPSolver& solver, const Rel1<Cardinal,View>& v) :
		v(solver,v.p1) {}
	Dom*	operator->()		{	return &v->card();	}
	Dom*	operator->() const	{	return const_cast<Dom*>(&v->card());}
	Dom&	operator*()			{	return v->card();	}
	Dom&	operator*() const	{	return const_cast<Dom&>(v->card());}

	Void attach(INotifiable* n)
	{	v->card().attachOnDomain(n);	}
	Void detach(INotifiable* n)
	{	v->card().detachOnDomain(n);	}

	Rel1<Cardinal,View>	getView() const
	{	return cardinal(v.getView());	}
	ICPSolver& solver() const	{	return v.solver();	}

	DomView<Set<Elem>,View> v;
};

/**
 * 	Enforces Bnd on the member(elem,set) set constraint.
 *	Implements cardinal(6,7).
 *  TODO: Propagation in the Set->FD direction is missing.
 */
template<class View1,class Elem,class View2>
struct BndFilterView2<Member,Elem,View1,Set<Elem>,View2> : IFilter
{
	typedef typename DomView<Set<Elem>,View2>::Dom::IIterator IIterator;
	typedef typename DomView<Set<Elem>,View2>::Dom::PIterator PIterator;

	BndFilterView2(ICPSolver& s,const View1& elem, const View2& set) :
		IFilter(s),elem(s,elem),set(s,set) {}

	bool execute()
	{
		if (!elem.ground())	// first time or never
			return true;
		detach(pOwner);
		IIterator iIt = set->findInIn(elem.value());
		if (iIt != set->endIn())
			return true;	// already in 'in'
		PIterator pIt = set->findInPoss(elem.value());
		if (pIt == set->endPoss())
			return false;	// not in 'poss'
		return set->insert(pIt);
	}

	void attach(INotifiable* s)
	{	pOwner = s; elem.attach(s);	}
	void detach(INotifiable* s)
	{	elem.detach(s); }

	ValView<Elem,View1>	elem;
	DomView<Set<Elem>,View2>	set;
	INotifiable*				pOwner;
};

/**
 * 	Enforces bound consistency on the notMember(elem,set) set constraint.
 *	Implements cardinal(8,9).
 *  TODO: Propagation in the Set->FD direction is missing.
 */
template<class View1,class Elem,class View2>
struct BndFilterView2<NotMember,Elem,View1,Set<Elem>,View2> : IFilter
{
	typedef typename DomView<Set<Elem>,View2>::Dom::IIterator IIterator;
	typedef typename DomView<Set<Elem>,View2>::Dom::PIterator PIterator;

	BndFilterView2(ICPSolver& s,const View1& elem, const View2& set) :
		IFilter(s),elem(s,elem),set(s,set) {}

	bool execute()
	{
		if (!elem.ground())	// first time or never
			return true;
		IIterator iIt = set->findInIn(elem.value());
		if (iIt != set->endIn())
			return false;	// already in 'in'
		detach(pOwner);
		PIterator pIt = set->findInPoss(elem.value());
		if (pIt != set->endPoss())	// in 'poss'
			return set->erase(pIt);
		return true;
	}

	void attach(INotifiable* s)
	{	pOwner = s; elem.attach(s);	 }
	void detach(INotifiable* s)
	{	elem.detach(s); }

	ValView<Elem,View1>			elem;
	DomView<Set<Elem>,View2>	set;
	INotifiable*				pOwner;
};

/**
 * 	Enforces bound consistency on the contained(set,set) set constraint.
 *	Implements cardinal(25-28).
 */
template<class Elem,class View1,class View2>
struct BndFilterView2<Contained,Set<Elem>,View1,Set<Elem>,View2> : IFilter
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;

	BndFilterView2(ICPSolver& s,const View1& v1, const View2& v2) :
		IFilter(s),x(s,v1),y(s,v2),
		xGLBDeltasIt(x->glbDeltas().begin()),
		yLUBDeltasIt(y->lubDeltas().begin()),
		first(s,true)
		{}

	bool execute()
	{
		//Debug::log(this,"Entering execute");
		if (first)
		{
			first = false;

			if (!solver().post(	cardinal(x.getView()) <= cardinal(y.getView())	))
				return false;

			if (!detail::setSafeInsertRange(*y,makeInIt(*x)))
				return false;

			if (!detail::setEraseRange(*x,makeDiffIt(makePossIt(*x),makeLUBIt(*y))))
				return false;

			xGLBDeltasIt = x->glbDeltas().end();
			yLUBDeltasIt = y->lubDeltas().end();
			return true;
		}

		//countDelta[curFilter]+= x->glbDeltas().beginFrom(xGLBDeltasIt) != x->glbDeltas().end();
		for (xGLBDeltasIt = x->glbDeltas().beginFrom(xGLBDeltasIt);
				xGLBDeltasIt != x->glbDeltas().end(); ++xGLBDeltasIt)
		{
		//	++sumDelta[curFilter];
			if (!setSafeInsertRange(*y,makeIt(*xGLBDeltasIt)))
				return false;
		}
		//countDelta[curFilter]+= y->lubDeltas().beginFrom(yLUBDeltasIt) != y->lubDeltas().end();
		for (yLUBDeltasIt = y->lubDeltas().beginFrom(yLUBDeltasIt);
				yLUBDeltasIt != y->lubDeltas().end(); ++yLUBDeltasIt)
		{
		//	++sumDelta[curFilter];
			if (!setEraseRange(*x,makeIt(*yLUBDeltasIt)))
				return false;
		}

		if (y->ground() or x->ground())
			detach(pOwner);

		return true;
	}

	Cost cost() const
	{	return linearLo;	}

	void attach(INotifiable* s)
	{
		pOwner = s;
		x->attachOnGLB(s);
		y->attachOnLUB(s);
	}
	void detach(INotifiable* s)
	{
		x->detachOnGLB(s);
		y->detachOnLUB(s);
	}

	DomView<Set<Elem>,View1>			x;
	DomView<Set<Elem>,View2>			y;
	typename DomX::DeltasIterator 		xGLBDeltasIt;
	typename DomY::DeltasIterator 		yLUBDeltasIt;
	Reversible<Bool>					first;
	INotifiable*	pOwner;
};

/**
 * 	Enforces bound consistency on the equal(set,set) set constraint. This constraint
 *  is rewritten as two \p Contained constraints, one in each direction.
 *	Implements cardinal(15-17).
 */
template<class Elem,class View1,class View2>
struct BndFilterFact2<Equal,Set<Elem>,View1,Set<Elem>,View2>
{
	static Filter create(ICPSolver& s, const View1& v1, const View2& v2)
	{
		//DomView<Set<Elem>,View1> vv1(s,v1);
		//DomView<Set<Elem>,View2> vv2(s,v2);
		return Bnd(contained(v1,v2)) and Bnd(contained(v2,v1));
	}
};

/**
 * 	Enforces BndD1 on the distinct(set,set) set constraint.
 *	Implements cardinal(18,19).
 */
template<class Elem,class View1,class View2>
struct BndD1FilterView2<Distinct,Set<Elem>,View1,Set<Elem>,View2> : IFilter
{
	typedef typename DomView<Set<Elem>,View1>::Dom	Dom1;
	typedef typename DomView<Set<Elem>,View2>::Dom	Dom2;

	BndD1FilterView2(ICPSolver& s,const View1& v1, const View2& v2) :
		IFilter(s),x(s,v1),y(s,v2) {}

	bool execute()
	{
		if (!y->ground())
			return true;

		if (detail::equal(y->beginIn(),y->endIn(),x->beginIn(),x->endIn()))
		{
			if (x->ground())
				return false;
			detach(pOwner);
			return solver().post( cardinal(x.getView()) > cardinal(y.getView()) );
		}

		if (detail::setUnion(x->beginIn(),x->endIn(),
			 				 x->beginPoss(),x->endPoss(),
			 detail::RangeEqual<typename Dom2::IIterator>(y->beginIn(),y->endIn())))
		{
			detach(pOwner);
			return solver().post( cardinal(x.getView()) < cardinal(y.getView()) );
		}

		return true;
	}

	void attach(INotifiable* s)
	{
		pOwner = s;
		x->attachOnLUB(s);
		y->attachOnGround(s);
	}

	void detach(INotifiable* s)
	{
		x->detachOnLUB(s);
		y->detachOnGround(s);
	}

	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	INotifiable*				pOwner;
};

/**
 * 	Enforces bound consistency on the distinct(set,set) set constraint.
 *	Implements cardinal(18-19) with extra propagation in set cardinality.
 */
template<class Elem,class View1,class View2>
struct BndFilterFact2<Distinct,Set<Elem>,View1,Set<Elem>,View2>
{
	static Filter create(ICPSolver& s, const View1& v1, const View2& v2)
	{
		return BndD1(distinct(v1,v2)) and BndD1(distinct(v2,v1));
	}
};

template<class Elem,class View1,class View2>
struct BACDisjointCard : IFilter
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;
	typedef typename DomX::Value	ValX;
	typedef typename DomY::Value	ValY;

	BACDisjointCard(ICPSolver& solver,
					const View1& v1,const View2& v2) :
		IFilter(solver),
		x(solver,v1),
		y(solver,v2)
	//	,
	//	xLUBDeltasIt(x->lubDeltas().begin()),
	//	yLUBDeltasIt(y->lubDeltas().begin())
		{}

	Bool execute()
	{
		UInt aux = detail::distance(makeInterIt(makePossIt(*x),makePossIt(*y)));

		Int n3 = x->inSize()+y->inSize()+
				 x->possSize()+y->possSize()-
				 aux;

		return x->card().updateMax(n3 - y->minCard()) and
			   y->card().updateMax(n3 - x->minCard());
	}

	Cost cost() const
	{	return linearHi;	}

	void attach(INotifiable* s)
	{
		x->attachOnLUB(s);
		y->attachOnLUB(s);
	}
	void detach(INotifiable* s)
	{
		x->detachOnLUB(s);
		y->detachOnLUB(s);
	}

	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	//typename DomX::DeltasIterator 		xLUBDeltasIt;
	//typename DomY::DeltasIterator 		yLUBDeltasIt;
};


template<class Elem,class View1,class View2>
Filter disjointCard(ICPSolver& solver,const View1& v1,const View2& v2)
{	return new (solver.heap()) BACDisjointCard<Elem,View1,View2>(solver,v1,v2);}

/**
 * 	Enforces bound consistency on the disjoint(set,set) set constraint.
 *	Implements cardinal(20-24).
 */
template<class Elem,class View1,class View2>
struct BndFilterView2<Disjoint,Set<Elem>,View1,Set<Elem>,View2> : IFilter
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;

	BndFilterView2(ICPSolver& s,const View1& v1, const View2& v2) :
		IFilter(s),x(s,v1),y(s,v2),
		xGLBDeltasIt(x->glbDeltas().begin()),
		yGLBDeltasIt(y->glbDeltas().begin()),
		chk(s,v1,v2),
		first(s,true) {}

	bool execute()
	{
		if (first)
		{
			if (chk.isTrue())
			{
				detach(pOwner);
				return true;
			}
			first = false;

			if (!solver().post(	disjointCard<Elem>(solver(),x.getView(),y.getView())))
				return false;

	/*		if (detail::intersect(x->beginIn(),x->endIn(),
								  y->beginIn(),y->endIn()))
				return false;
*/
			if (!detail::setSafeEraseRange(*x,makeInIt(*y)))
				return false;

			if (!detail::setEraseRange(*y,makeInIt(*x)))
				return false;

			xGLBDeltasIt = x->glbDeltas().end();
			yGLBDeltasIt = y->glbDeltas().end();

			return true;
		}

		for (xGLBDeltasIt = x->glbDeltas().beginFrom(xGLBDeltasIt);
				xGLBDeltasIt != x->glbDeltas().end(); ++xGLBDeltasIt)
			if (!setSafeEraseRange(*y,makeIt(*xGLBDeltasIt)))
				return false;

		for (yGLBDeltasIt = y->glbDeltas().beginFrom(yGLBDeltasIt);
				yGLBDeltasIt != y->glbDeltas().end(); ++yGLBDeltasIt)
			if (!setEraseRange(*x,makeIt(*yGLBDeltasIt)))
				return false;

		if (chk.isTrue())
			detach(pOwner);
		return true;
	}

	Cost cost() const
	{	return linearLo;	}

	void attach(INotifiable* s)
	{
		pOwner = s;
		x->attachOnGLB(s);
		y->attachOnGLB(s);
	}
	void detach(INotifiable* s)
	{
		x->detachOnGLB(s);
		y->detachOnGLB(s);
	}

	DomView<Set<Elem>,View1>			x;
	DomView<Set<Elem>,View2>			y;
	typename DomX::DeltasIterator 		xGLBDeltasIt;
	typename DomY::DeltasIterator 		yGLBDeltasIt;
	ChkViewRel2<Disjoint,Set<Elem>,View1,Set<Elem>,View2>	chk;
	Reversible<Bool>			first;
	INotifiable*	pOwner;
};

/**
 * 	Enforces bound consistency on the disjoint(seq<set>) set constraint.
 *	Rewritten as a set of disjoint pairwise constraints.
 */
template<class Elem,class View>
struct BndFilterFact1<Disjoint,Seq<Set<Elem> >,View>
{
	static Filter create(ICPSolver& solver,const View& v)
	{
		DomArrayView<Set<Elem>,View> s(solver,v);
		BndExpr<Bool> e(solver,true);
		for (UInt i = 0; i < s.size(); i++)
			for (UInt j = i+1; j < s.size(); j++)
				e = e and disjoint(s[i].getView(),s[j].getView());
		return e;
	}
};

template<class Elem,class View1,class View2,class View3>
struct BACIntersectCard1 : IFilter
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;
	typedef typename DomView<Set<Elem>,View3>::Dom	DomZ;

	BACIntersectCard1(ICPSolver& solver,
					  const View1& v1,const View2& v2,const View3& v3) :
		IFilter(solver),
		x(solver,v1),
		y(solver,v2),
		z(solver,v3)
	//	,
	//	xLUBDeltasIt(x->lubDeltas().begin()),
	//	yLUBDeltasIt(y->lubDeltas().begin()),
	//	zLUBDeltasIt(z->lubDeltas().begin())
		{}

	Bool execute()
	{
		Int n3 = detail::distance(makeUnionIt(makeLUBIt(*x),makeLUBIt(*y)));

		return z->card().updateMin(x->minCard()+y->minCard()-n3) and
			   x->card().updateMax(z->maxCard()-y->minCard()+n3) and
			   y->card().updateMax(z->maxCard()-x->minCard()+n3);
	}

	Cost cost() const
	{	return linearHi;	}

	void attach(INotifiable* s)
	{
		x->attachOnLUB(s);
		y->attachOnLUB(s);
		z->attachOnLUB(s);
	}
	void detach(INotifiable* s)
	{
		x->detachOnLUB(s);
		y->detachOnLUB(s);
		z->detachOnLUB(s);
	}

	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	DomView<Set<Elem>,View3>	z;
//	typename DomX::DeltasIterator 		xLUBDeltasIt;
//	typename DomY::DeltasIterator 		yLUBDeltasIt;
//	typename DomZ::DeltasIterator 		zLUBDeltasIt;
};

template<class Elem,class View1,class View2,class View3>
Filter intersectCard1(ICPSolver& solver,const View1& v1,const View2& v2,const View3& v3)
{	return new (solver.heap()) BACIntersectCard1<Elem,View1,View2,View3>(solver,v1,v2,v3);}


template<class Elem,class View1,class View2,class View3>
struct BACIntersectCard2 : IFilter
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;
	typedef typename DomView<Set<Elem>,View3>::Dom	DomZ;
	typedef typename DomX::Value	ValX;
	typedef typename DomY::Value	ValY;
	typedef typename DomZ::Value	ValZ;

	BACIntersectCard2(ICPSolver& solver,
					  const View1& v1,const View2& v2,const View3& v3) :
		IFilter(solver),
		x(solver,v1),
		y(solver,v2),
		z(solver,v3)
	//	,
	//	xGLBDeltasIt(x->glbDeltas().begin()),
	//	yLUBDeltasIt(y->lubDeltas().begin()),
	//	zLUBDeltasIt(z->lubDeltas().begin())
		{}

	Bool execute()
	{
		Int n3 = detail::distance(makeDiffIt(makeInIt(*x),makeLUBIt(*y)));

		return z->card().updateMax(x->card().max()-
								   n3) and
			   x->card().updateMin(z->card().min()+
				   			   	   n3) and
			   y->card().updateMin(z->card().min()+
				   			   	   n3);
	}

	Cost cost() const
	{	return linearHi;	}

	void attach(INotifiable* s)
	{
		x->attachOnGLB(s);
		y->attachOnLUB(s);
		z->attachOnLUB(s);
	}
	void detach(INotifiable* s)
	{
		x->detachOnGLB(s);
		y->detachOnLUB(s);
		z->detachOnLUB(s);
	}

	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	DomView<Set<Elem>,View3>	z;
	//typename DomX::DeltasIterator 		xGLBDeltasIt;
	//typename DomY::DeltasIterator 		yLUBDeltasIt;
	//typename DomZ::DeltasIterator 		zLUBDeltasIt;
};

template<class Elem,class View1,class View2,class View3>
Filter intersectCard2(ICPSolver& solver,View1& v1,View2& v2,View3& v3)
{	return new (solver.heap()) BACIntersectCard2<Elem,View1,View2,View3>(solver,v1,v2,v3);}

/**
 * 	Enforces bound consistency on the intersect(set,set,set) set constraint.
 *	Implements cardinal...
 */
template<class Elem,class View1,class View2,class View3>
struct BndFilterView3<Intersect,Set<Elem>,View1,Set<Elem>,View2,Set<Elem>,View3> : IFilter
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;
	typedef typename DomView<Set<Elem>,View3>::Dom	DomZ;

	BndFilterView3(ICPSolver& s,const View1& v1,
				   const View2& v2, const View3& v3) :
		IFilter(s),x(s,v1),y(s,v2),z(s,v3),
		xGLBDeltasIt(x->glbDeltas().begin()),
		yGLBDeltasIt(y->glbDeltas().begin()),
		zLUBDeltasIt(z->lubDeltas().begin()),
		first(s,true),
		chk(s,v1,v2,v3) {}

	bool execute()
	{
		//Debug::log(this,"Entering execute");
		if (first)
		{
			if (!chk.canBeTrue())
				return false;
			if (chk.isTrue())
			{
				detach(pOwner);
				return true;
			}

			first = false;

			// 29
			if (!solver().post( contained(z.getView(),x.getView()) ))
				return false;

			// 29
			if (!solver().post( contained(z.getView(),y.getView()) ))
				return false;

			UInt n1 = distance(makeDiffIt(makeInIt(*x),makeLUBIt(*y)));

			// 31
			if (!solver().post( cardinal(z.getView()) <= cardinal(x.getView()) - (Int)n1 ))
				return false;

			UInt n2 = distance(makeDiffIt(makeInIt(*y),makeLUBIt(*x)));

			// 32
			if (!solver().post( cardinal(z.getView()) <= cardinal(y.getView()) - (Int)n2 ))
				return false;

			if (!solver().post( intersectCard1<Elem>(solver(),x.getView(),y.getView(),z.getView())))
				return false;
		}

		for (xGLBDeltasIt = x->glbDeltas().beginFrom(xGLBDeltasIt);
				xGLBDeltasIt != x->glbDeltas().end(); ++xGLBDeltasIt)
		{
			if (!detail::setSafeInsertRange(*z,makeInterIt(makeInIt(*y),
														   makeIt(*xGLBDeltasIt))))
				return false;
			if (!detail::setSafeEraseRange(*y,makeDiffIt(makeIt(*xGLBDeltasIt),
														 makeLUBIt(*z))))
				return false;
		}
		for (yGLBDeltasIt = y->glbDeltas().beginFrom(yGLBDeltasIt);
				yGLBDeltasIt != y->glbDeltas().end(); ++yGLBDeltasIt)
		{
			if (!detail::setSafeInsertRange(*z,makeInterIt(makeInIt(*x),
														   makeIt(*yGLBDeltasIt))))
				return false;

			if (!detail::setSafeEraseRange(*x,makeDiffIt(makeIt(*yGLBDeltasIt),
														 makeLUBIt(*z))))
				return false;
		}
		for (zLUBDeltasIt = z->lubDeltas().beginFrom(zLUBDeltasIt);
				zLUBDeltasIt != z->lubDeltas().end(); ++zLUBDeltasIt)
		{
			if (!detail::setSafeEraseRange(*y,makeInterIt(makeInIt(*x),
														  makeIt(*zLUBDeltasIt))))
				return false;

			if (!detail::setSafeEraseRange(*x,makeInterIt(makeInIt(*y),
														  makeIt(*zLUBDeltasIt))))
				return false;
		}

		if (chk.isTrue())
		{
			detach(pOwner);
			return true;
		}
		return true;
	}

	Cost cost() const
	{	return linearLo;	}

	void attach(INotifiable* s)
	{
		pOwner = s;
		x->attachOnGLB(s);
		y->attachOnGLB(s);
		z->attachOnLUB(s);
	}
	void detach(INotifiable* s)
	{
		x->detachOnGLB(s);
		y->detachOnGLB(s);
		z->detachOnLUB(s);
	}

	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	DomView<Set<Elem>,View3>	z;
	typename DomX::DeltasIterator 		xGLBDeltasIt;
	typename DomY::DeltasIterator 		yGLBDeltasIt;
	typename DomZ::DeltasIterator 		zLUBDeltasIt;
	Reversible<Bool>			first;
	INotifiable*	pOwner;
	ChkViewRel3<Intersect,Set<Elem>,View1,Set<Elem>,View2,Set<Elem>,View3> chk;
};

template<class Elem,class View1,class View2,class Dom>
struct DomView<Set<Elem>,Rel2<Intersect,View1,View2>,Dom> : IDomExpr<Dom>
{
	typedef typename DomView<Set<Elem>,View1>::Dom	Dom1;
	typedef typename DomView<Set<Elem>,View2>::Dom	Dom2;
	typedef Dom	Dom3;
	typedef typename Dom1::Value	Val1;
	typedef typename Dom2::Value	Val2;
	typedef typename Dom3::Value	Val3;

	DomView(ICPSolver& s, const Rel2<Intersect,View1,View2>& r) :
		IDomExpr<Dom>(s)
	{
		// FIXME: rewrite using new iteration views
		DomView<Set<Elem>,View1>	v1(s,r.p1);
		DomView<Set<Elem>,View2>	v2(s,r.p2);
		detail::Vector<Val1> lub1(v1->inSize()+v1->possSize());
		detail::setLUB(*v1,lub1);

		detail::Vector<Val2> lub2(v2->inSize()+v2->possSize());
		detail::setLUB(*v2,lub2);

		detail::QuickList<Val3> lub3(lub1.size()+lub2.size());
		detail::setUnion(lub1.begin(),lub1.end(),
						 lub2.begin(),lub2.end(),
						 detail::ListInserter<Val3>(lub3));

		auxD = new (s.heap()) Dom(s,lub3.begin(),lub3.end());
		s.post( intersect(v1.getView(),v2.getView(),DomVar<Set<Elem>,Dom>(s,auxD)) );
	}
	Dom*	operator->()		{	return auxD;	}
	Dom*	operator->() const	{	return const_cast<Dom*>(auxD);}
	Dom&	operator*()			{	return *auxD;	}
	Dom&	operator*() const	{	return const_cast<Dom&>(*auxD);}

	Void attach(INotifiable* n)
	{	auxD->attachOnDomain(n);	}
	Void detach(INotifiable* n)
	{	auxD->detachOnDomain(n);	}
	Dom*			   auxD;
};

/**
 * 	Enforces bound consistency on the unionEqual(set,set,set) set constraint.
 *	Implements cardinal...
 */
template<class Elem,class View1,class View2,class View3>
struct BndFilterView3<UnionEqual,Set<Elem>,View1,Set<Elem>,View2,Set<Elem>,View3> : IFilter
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;
	typedef typename DomView<Set<Elem>,View3>::Dom	DomZ;

	BndFilterView3(ICPSolver& s,const View1& v1,
				   const View2& v2, const View3& v3) :
		IFilter(s),x(s,v1),y(s,v2),z(s,v3),
		xLUBDeltasIt(x->lubDeltas().begin()),
		yLUBDeltasIt(y->lubDeltas().begin()),
		zGLBDeltasIt(z->glbDeltas().begin()),
		first(s,true) {}

	Cost cost() const
	{	return linearLo;	}

	bool execute()
	{
		if (first)
		{
			first = false;

			// 40
			if (!solver().post( contained(x.getView(),z.getView()) ))
				return false;

			// 40
			if (!solver().post( contained(y.getView(),z.getView()) ))
				return false;

			// 40
			if (!solver().post( cardinal(z.getView()) <=
								cardinal(x.getView()) + cardinal(y.getView()) ))
				return false;

			// 42
			UInt n1 = distance(makeDiffIt(makeInIt(*x),makeLUBIt(*y)));

			if (!solver().post( cardinal(z.getView()) >=
								cardinal(y.getView()) + (Int)n1 ))
				return false;

			// 43
			UInt n2 = distance(makeDiffIt(makeInIt(*y),makeLUBIt(*x)));

			if (!solver().post( cardinal(z.getView()) >=
								cardinal(x.getView()) + (Int)n2 ))
				return false;

			if (!detail::setSafeEraseRange(*z,makeDiffIt(makeLUBIt(*z),
										makeUnionIt(makeLUBIt(*x),makeLUBIt(*y)))))
				return false;

			if (!detail::setSafeInsertRange(*y,makeDiffIt(makeInIt(*z),makeLUBIt(*x))))
				return false;

			if (!detail::setSafeInsertRange(*x,makeDiffIt(makeInIt(*z),makeLUBIt(*y))))
				return false;

			xLUBDeltasIt = x->lubDeltas().end();
			yLUBDeltasIt = y->lubDeltas().end();
			zGLBDeltasIt = z->glbDeltas().end();

			return true;
		}

		for (xLUBDeltasIt = x->lubDeltas().beginFrom(xLUBDeltasIt);
				xLUBDeltasIt != x->lubDeltas().end(); ++xLUBDeltasIt)
		{
			if (!detail::setSafeEraseRange(*z,makeDiffIt(makeIt(*xLUBDeltasIt),
														 makeLUBIt(*y))))
				return false;

			if (!detail::setSafeInsertRange(*y,makeInterIt(makeInIt(*z),
														   makeIt(*xLUBDeltasIt))))
				return false;
		}

		for (yLUBDeltasIt = y->lubDeltas().beginFrom(yLUBDeltasIt);
				yLUBDeltasIt != y->lubDeltas().end(); ++yLUBDeltasIt)
		{
			if (!detail::setSafeEraseRange(*z,makeDiffIt(makeIt(*yLUBDeltasIt),
														 makeLUBIt(*x))))
				return false;

			if (!detail::setSafeInsertRange(*x,makeInterIt(makeInIt(*z),
														   makeIt(*yLUBDeltasIt))))
				return false;
		}

		for (zGLBDeltasIt = z->glbDeltas().beginFrom(zGLBDeltasIt);
				zGLBDeltasIt != z->glbDeltas().end(); ++zGLBDeltasIt)
		{
			if (!detail::setSafeInsertRange(*x,makeDiffIt(makeIt(*zGLBDeltasIt),
														  makeLUBIt(*y))))
				return false;

			if (!detail::setSafeInsertRange(*y,makeDiffIt(makeIt(*zGLBDeltasIt),
														  makeLUBIt(*x))))
				return false;
		}

		return true;
	}

	void attach(INotifiable* s)
	{
		x->attachOnLUB(s);
		y->attachOnLUB(s);
		z->attachOnDomain(s);
	}
	void detach(INotifiable* s)
	{
		x->detachOnLUB(s);
		y->detachOnLUB(s);
		z->detachOnDomain(s);
	}

	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	DomView<Set<Elem>,View3>	z;

	typename DomX::DeltasIterator 		xLUBDeltasIt;
	typename DomY::DeltasIterator 		yLUBDeltasIt;
	typename DomZ::DeltasIterator 		zGLBDeltasIt;

	Reversible<Bool>			first;
};

/**
 * 	Enforces bound consistency on the union(setArray,set) constraint.
 */
template<class Elem,class View1,class View2>
struct BndFilterFact2<Union,Seq<Set<Elem> >,View1,Set<Elem>,View2>
{
	static Filter create(ICPSolver& solver, const View1& a,const View2& r)
	{
		typedef typename DomArrayView<Set<Elem>,View1>::Dom	Dom;
		DomArrayView<Set<Elem>,View1> s(solver,a);
		assert(s.size()>1);
		std::set<Elem> elems;
		for (UInt i = 0; i < s.size(); ++i)
		{
			for (typename Dom::IIterator it = s[i]->beginIn();
					it != s[i]->endIn(); ++it)
				elems.insert(*it);
			for (typename Dom::PIterator it = s[i]->beginPoss();
					it != s[i]->endPoss(); ++it)
				elems.insert(*it);
		}
		DomVarArray<Set<Elem> > auxvars(solver,s.size()-1,elems.begin(),elems.end());

		BndExpr<Bool> e(solver,true);
		e = e and union_(s[0].getView(),s[1].getView(),auxvars[0]);
		for (UInt i = 1; i < s.size()-1; i++)
			e = e and union_(auxvars[i-1],s[i+1].getView(),auxvars[i]);
		e = e and auxvars[s.size()-2] == r;
		return e;
	}
};


/**
 * 	Enforces bound consistency on the partition(sets) constraint.
 *  The filter is highly incremental although the propagation could
 *  probably be stronger...
 */
template<class Elem,class View1>
struct BndFilterView1<Partition,Seq<Set<Elem> >,View1> : IFilter
{
	typedef typename ElemTraits<View1>::Elem	SeqElem;
	typedef typename DomTraits<SeqElem>::Dom		Dom;
	//typedef typename DomView<Set<Elem>,View1>::Dom	Dom;
	typedef typename Dom::Value	Val;
	typedef detail::Pair<UInt,typename Dom::PIterator>	P;

	struct LessFirst
	{
		Bool operator()(const P& p1, const P& p2) const
		{	return p1.first < p2.first;	}
	};

	typedef detail::RSUList<P,LessFirst>	PossList;
	typedef PossList* PPossList;
	typedef detail::HashMap<Elem,PPossList>	InPoss;

	BndFilterView1(ICPSolver& s,const View1& v1) :
		IFilter(s),x(s,v1),
		xGLBDeltasIt(x.size()),
		xLUBDeltasIt(x.size()),
		first(s,true)
		{}

	Cost cost() const
	{	return linearLo;	}

	bool execute()
	{
		if (first)
		{
			first = false;

			// calculate union of all values
			set<Elem> un;
			for (UInt i = 0; i < x.size(); ++i)
			{
				for (typename Dom::IIterator it = x[i]->beginIn();
						it != x[i]->endIn(); ++it)
					un.insert(*it);
				for (typename Dom::PIterator it = x[i]->beginPoss();
						it != x[i]->endPoss(); ++it)
					un.insert(*it);
			}
			maxElems = un.size();

			// fix cardinality
			BndExpr<Int> card(solver(),0);
			for (UInt i = 0; i < x.size(); ++i)
				card = card + cardinal(x[i].getView());
			if (!solver().post(card == (Int)un.size()))
				return false;

			// remove those elements already in In from other LUBs
			for (UInt i1 = 0; i1 < x.size(); ++i1)
				if (x[i1]->inSize()>0)
					for (UInt i2 = 0; i2 < x.size(); ++i2)
						if (i1 != i2 and
								!setSafeEraseRange(*x[i2],
												x[i1]->beginIn(),x[i1]->endIn()))
						return false;

			// create element->inPoss map
			for (UInt i = 0; i < x.size(); ++i)
				for (typename Dom::PIterator it = x[i]->beginPoss();
						it != x[i]->endPoss(); ++it)
				{
					if (inPoss.find(*it) == inPoss.end())
						inPoss[*it] = new (solver().heap()) PossList(solver());
					inPoss[*it]->insert(P(i,it));
				}

			// insert elements that may belong only to one set
			for (typename InPoss::Iterator it = inPoss.begin();
					it != inPoss.end(); ++it)
				if (it->second->size()==1)
				{
					if (!x[it->second->begin()->first]->safeInsert(it->first))
						return false;
					it->second->clear();
				}

			// reset deltas
			for (UInt i = 0; i < x.size(); ++i)
			{
				xGLBDeltasIt[i] = x[i]->glbDeltas().end();
				xLUBDeltasIt[i] = x[i]->lubDeltas().end();
			}

			return true;
		}
		//cout << "------ here --------\n";
		typedef detail::QuickSList<typename PossList::Iterator> OneLeft;
		OneLeft oneLeft(maxElems*x.size());

		// first update map for erased elements, and mark those which can
		// only be in one set
		for (UInt i = 0; i < x.size(); ++i)
			for (xLUBDeltasIt[i] = x[i]->lubDeltas().beginFrom(xLUBDeltasIt[i]);
					xLUBDeltasIt[i] != x[i]->lubDeltas().end(); ++xLUBDeltasIt[i])
				for (typename Dom::Delta::Iterator it = xLUBDeltasIt[i]->begin();
						it != xLUBDeltasIt[i]->end(); ++it)
				{
					//cout << "updating map for erased elem " << *it << " from set " << i << endl;
					if (inPoss[*it]->empty()) // element was inserted previously by this filter
						continue;
					typename PossList::Iterator pit = inPoss[*it]->begin();
					for ( ; pit != inPoss[*it]->end(); ++pit)
						if (pit->first == i)
							break;
					assert(pit != inPoss[*it]->end());
					inPoss[*it]->erase(pit);
					//cout << "erasing elem " << *it << " from InPoss " << pit->first << endl;
					if (inPoss[*it]->empty())
						return false;	// element cannot be assigned anywhere
					if (++inPoss[*it]->begin()==inPoss[*it]->end())
						oneLeft.pushFront(inPoss[*it]->begin());
				}

		// remove inserted elements from other poss
		for (UInt i = 0; i < x.size(); ++i)
			for (xGLBDeltasIt[i] = x[i]->glbDeltas().beginFrom(xGLBDeltasIt[i]);
					xGLBDeltasIt[i] != x[i]->glbDeltas().end(); ++xGLBDeltasIt[i])
				for (typename Dom::Delta::Iterator it = xGLBDeltasIt[i]->begin();
						it != xGLBDeltasIt[i]->end(); ++it)
				{
					//if (inPoss[*it]->empty())	// element was inserted elsewhere
					//	return false;
					for (typename PossList::Iterator pit = inPoss[*it]->begin();
							pit != inPoss[*it]->end(); ++pit)
						if (pit->first != i and
					// the following checks for the corner case of the insertion
					// into multiple variables since last execution of this filter
							(x[pit->first]->findInIn(*pit->second)!=
							 x[pit->first]->endIn() or
							!x[pit->first]->erase(pit->second)))
							return false;
					inPoss[*it]->clear();
					//cout << "inserted elem " << *it << " in set " << i << endl;
				}

		// check if elements that can only be in one set are still unassigned,
		// and assign them
		for (typename OneLeft::Iterator it = oneLeft.begin();
					it != oneLeft.end(); ++it)
			if (!inPoss[*(*it)->second]->empty())
			{
				assert(++inPoss[*(*it)->second]->begin()==
					   inPoss[*(*it)->second]->end());
				if (!x[(*it)->first]->insert((*it)->second))
					return false;
				inPoss[*(*it)->second]->clear();
			//	cout << "clearing2 inPoss of elem " << *(*it)->second << endl;
			}
		//cout << "returning true\n";
		return true;
	}

	void attach(INotifiable* s)
	{
		for (UInt i = 0; i < x.size(); ++i)
			x[i]->attachOnDomain(s);
	}
	void detach(INotifiable* s)
	{
		for (UInt i = 0; i < x.size(); ++i)
			x[i]->detachOnDomain(s);
	}

	DomArrayView<Set<Elem>,View1> x;
	Array<typename Dom::DeltasIterator> 	xGLBDeltasIt;
	Array<typename Dom::DeltasIterator> 	xLUBDeltasIt;
	Reversible<Bool>			first;
	InPoss						inPoss;
	UInt						maxElems;
};


/**
 * 	Enforces bound consistency on the union(sets) constraint.
 */
template<class Elem,class View1,class View2>
struct BndFilterView2<UnionEqual,Seq<Set<Elem> >,View1,Set<Elem>,View2> : IFilter
{
	typedef typename DomView<Set<Elem>,View1>::Dom	Dom1;
	typedef typename DomView<Set<Elem>,View2>::Dom	Dom2;
	typedef typename Dom1::Value	Val;

	typedef detail::RSUList<UInt>	PossList;
	typedef PossList* PPossList;
	typedef detail::HashMap<Elem,PPossList>	InLUB;

	BndFilterView2(ICPSolver& s,const View1& v1,const View2& v2) :
		IFilter(s),x(s,v1),z(s,v2),
		xGLBDeltasIt(x.size()),
		xLUBDeltasIt(x.size()),
		zGLBDeltasIt(z->glbDeltas().begin()),
		zLUBDeltasIt(z->lubDeltas().begin()),
		first(s,true)
		{}

	Cost cost() const
	{	return linearLo;	}

	bool execute()
	{
		if (first)
		{
			first = false;

			// remove from x all values not in z
			for (UInt i = 0; i < x.size(); ++i)
				if (!setSafeEraseRange(*x[i],makeDiffIt(makeLUBIt(*x[i]),makeLUBIt(*z))))
					return false;

			// insert in z all values in x
			for (UInt i = 0; i < x.size(); ++i)
				if (!setSafeInsertRange(*z,makeInIt(*x[i])))
					return false;


			// get union of all values in lub(x)
			set<Elem> un;
			for (UInt i = 0; i < x.size(); ++i)
			{
				for (typename Dom1::IIterator it = x[i]->beginIn();
						it != x[i]->endIn(); ++it)
					un.insert(*it);
				for (typename Dom1::PIterator it = x[i]->beginPoss();
						it != x[i]->endPoss(); ++it)
					un.insert(*it);
			}

			// remove from z all values not in union(lub(x))
			if (!setSafeEraseRange(*z,makeDiffIt(makeLUBIt(*z),makeSTLIt(un))))
				return false;

			// create element->inLUB map
			for (UInt i = 0; i < x.size(); ++i)
			{
				for (auto it = makeLUBIt(*x[i]); it.valid(); it.iterate())
				{
					if (inLUB.find(it.value()) == inLUB.end())
						inLUB[it.value()] = new (solver().heap()) PossList(solver());
					inLUB[it.value()]->insert(i);
				}
			}
			// if an element in z can be only in one x then insert it
			for (typename Dom2::IIterator it = z->beginIn(); it != z->endIn(); ++it)
			{
				typename InLUB::Iterator hit = inLUB.find(*it);
				assert(hit!=inLUB.end() and !hit->second->empty());
				if (++hit->second->begin()==hit->second->end())
					if (!x[*hit->second->begin()]->safeInsert(*it))
						return false;
			}

			// reset deltas
			for (UInt i = 0; i < x.size(); ++i)
			{
				xGLBDeltasIt[i] = x[i]->glbDeltas().end();
				xLUBDeltasIt[i] = x[i]->lubDeltas().end();
			}
			zLUBDeltasIt = z->lubDeltas().end();
			zGLBDeltasIt = z->glbDeltas().end();

			return true;
		}

		// for any remove from z, remove from all x
		for (zLUBDeltasIt = z->lubDeltas().beginFrom(zLUBDeltasIt);
					zLUBDeltasIt != z->lubDeltas().end(); ++zLUBDeltasIt)
			for (UInt i = 0; i < x.size(); ++i)
				if (!setSafeEraseRange(*x[i],makeIt(*zLUBDeltasIt)))
					return false;

		// update map for erased elements and erase from z if element is not in x
		for (UInt i = 0; i < x.size(); ++i)
			for (xLUBDeltasIt[i] = x[i]->lubDeltas().beginFrom(xLUBDeltasIt[i]);
					xLUBDeltasIt[i] != x[i]->lubDeltas().end(); ++xLUBDeltasIt[i])
				for (typename Dom1::Delta::Iterator it = xLUBDeltasIt[i]->begin();
						it != xLUBDeltasIt[i]->end(); ++it)
				{
					typename InLUB::Iterator hit = inLUB.find(*it);
					if (hit->second->empty()) // element was removed previously by this filter
						continue;
					typename PossList::Iterator pit = hit->second->begin();
					for ( ; pit != hit->second->end(); ++pit)
						if (*pit == i)
							break;
					assert(pit != hit->second->end());
					hit->second->erase(pit);
					if (hit->second->empty()) // element cannot be in z
					{
						if (!z->safeErase(*it))
							return false;
					}
					else	// if there is only one element in xi
					if (++hit->second->begin()==hit->second->end() and
						z->findInIn(*it)!=z->endIn())
						if (!x[*hit->second->begin()]->safeInsert(*it))
							return false;
				}

		// for any insertion in x insert also in z
		for (UInt i = 0; i < x.size(); ++i)
			for (xGLBDeltasIt[i] = x[i]->glbDeltas().beginFrom(xGLBDeltasIt[i]);
				xGLBDeltasIt[i] != x[i]->glbDeltas().end(); ++xGLBDeltasIt[i])
				if (!setSafeInsertRange(*z,makeIt(*xGLBDeltasIt[i])))
					return false;

		// for any insert in z insert in xi if there is only one xi
		for (zGLBDeltasIt = z->glbDeltas().beginFrom(zGLBDeltasIt);
					zGLBDeltasIt != z->glbDeltas().end(); ++zGLBDeltasIt)
			for (typename Dom2::Delta::Iterator it = zGLBDeltasIt->begin();
					it != zGLBDeltasIt->end(); ++it)
			{
				typename InLUB::Iterator hit = inLUB.find(*it);
				if (hit->second->empty())
					return false;
				if (++hit->second->begin()==hit->second->end())
					if (!x[*hit->second->begin()]->safeInsert(*it))
						return false;
			}

		return true;
	}

	void attach(INotifiable* s)
	{
		for (UInt i = 0; i < x.size(); ++i)
			x[i]->attachOnDomain(s);
		z->attachOnDomain(s);
	}
	void detach(INotifiable* s)
	{
		for (UInt i = 0; i < x.size(); ++i)
			x[i]->detachOnDomain(s);
		z->detachOnDomain(s);
	}

	DomArrayView<Set<Elem>,View1> x;
	DomView<Set<Elem>,View2> 	z;

	Array<typename Dom1::DeltasIterator> 	xGLBDeltasIt;
	Array<typename Dom1::DeltasIterator> 	xLUBDeltasIt;
	typename Dom2::DeltasIterator	zGLBDeltasIt;
	typename Dom2::DeltasIterator	zLUBDeltasIt;

	Reversible<Bool>			first;
	InLUB						inLUB;
	UInt						maxElems;
};


/*@}*/


/// TODO: test, test, test, ...
template<class Eval,class Dom>
struct BndViewRel1<Min,DomVar<Set<Eval>,Dom>,Eval>
{
	BndViewRel1(ICPSolver& solver, const DomVar<Set<Eval>,Dom>& v) :
		v(v)
	{
		if (v.domain().empty())
			throw EEmptySetVar("min");
		//solver.post(cardinal(v)>0);  // FIXME: not sure how to handle min over empty set
	}

	Eval min() const
	{
		if (v.domain().inSize()>0)
		{
			if (v.domain().ground())
				return v.domain().in().min();
			return std::min(v.domain().in().min(),v.domain().poss().min());
		}
		if (v.domain().ground())
			throw EEmptySetVar("min");
		return v.domain().poss().min();
	}
	Eval max() const
	{
		if (v.domain().inSize()>0)
			return v.domain().in().min();
		if (v.domain().ground())
			throw EEmptySetVar("min");
		return v.domain().poss().max();
	}
	Bool updateMin(const Eval& val)
	{
		if (v.domain().inSize()>0)
		{
			if (val>v.domain().in().min())
				return false;
		}
		else
			if (v.domain().ground())
				throw EEmptySetVar("min");

		return v.domain().ground() or
			   v.domain().poss().min()>=val or
			   v.domain().erase(v.domain().beginPoss(),
							    v.domain().poss().lowerBound(val));
	}
	Bool updateMax(const Eval& val)
	{
		if (v.domain().ground())
		{
			if (v.domain().inSize()==0)
				throw EEmptySetVar("min");
			else
				return val>=v.domain().in().min();
		}
		if (v.domain().inSize()==0) // we know domain can not be ground
			return val>=v.domain().poss().min();
		if (v.domain().in().min()<=val)
			return true;
		if (v.domain().poss().min()>val)
			return false;
		if (v.domain().possSize()==1 or
			*++v.domain().beginPoss()>val)
			return v.domain().insert(v.domain().beginPoss());
		return true;
	}
	Void range(Eval& v1,Eval& v2) const
	{ v1 = min(); v2=max(); }
	Bool updateRange(const Eval& v1, const Eval& v2)
	{	return updateMin(v1) and updateMax(v2); }

	Void attach(INotifiable* n) {	v.domain().attachOnDomain(n);	}
	Void detach(INotifiable* n) {	v.domain().detachOnDomain(n);	}
	Rel1<Min,DomVar<Set<Eval>,Dom> > getView()  const
	{ return Rel1<Min,DomVar<Set<Eval>,Dom> >(v); }

	DomVar<Set<Eval>,Dom>	v;
};
};


#endif /*_H_CASPER_SET_FILTER*/
