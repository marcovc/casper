/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2009 - Marco Correia <marco.v.correia@gmail.com>           	   *
 *                                                                         *
 *   Licensed under the Apache License, Version 2.0 (the "License");       *
 *   you may not use this file except in compliance with the License.      *
 *   You may obtain a copy of the License at                               *
 *            http://www.apache.org/licenses/LICENSE-2.0                   *
 *   Unless required by applicable law or agreed to in writing, software   *
 *   distributed under the License is distributed on an                    *
 *   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,          *
 *   either express or implied.                                            *
 *   See the License for the specific language governing permissions and   *
 *   limitations under the License.                                        *
 \*************************************************************************/

#ifndef CASPER_SET_CHKVIEW_H_
#define CASPER_SET_CHKVIEW_H_

#include <casper/kernel/view/chkview.h>

namespace casper {


template<class Elem,class View1,class View2>
struct ChkViewRel2<Member,Elem,View1,Set<Elem>,View2>
{
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
			elem(solver,p1),set(solver,p2) {}
	Bool isTrue() const	// is it true?
	{	return elem.ground() and
			   set->findInIn(elem.value()) != set->endIn();	}
	Bool canBeTrue() const 	// can it still be true?
	{	return !elem.ground() or
			   set->findInIn(elem.value()) != set->endIn() or
			   set->findInPoss(elem.value()) != set->endPoss();
	}
	Bool setToTrue()
	{
		detach(pOwner);
		return set.solver().post(member(elem.getView(),set.getView()));
	}
	Bool setToFalse()
	{
		detach(pOwner);
		return set.solver().post(notMember(elem.getView(),set.getView()));
	}
	ICPSolver& solver() const {	return set.solver();	}

	void attach(INotifiable* f) { 	pOwner=f; elem.attach(f); set->attachOnDomain(f);}
	void detach(INotifiable* f) {	elem.detach(f); set->detachOnDomain(f);}

	Rel2<Member,View1,View2> getView()  const
	{ 	return Rel2<Member,View1,View2>(elem.getView(),set.getView());	}

	ValView<Elem,View1>	elem;
	DomView<Set<Elem>,View2>	set;
	INotifiable*	pOwner;
};



template<class Elem,class View1,class View2>
struct ChkViewRel2<NotMember,Elem,View1,Set<Elem>,View2>
{
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
			elem(solver,p1),set(solver,p2) {}
	Bool isTrue() const	// is it true?
	{	return elem.ground() and
			   set->findInPoss(elem.value()) == set->endPoss() and
			   set->findInIn(elem.value()) == set->endIn();
	}
	Bool canBeTrue() const 	// can it still be true?
	{	return !elem.ground() or
			   set->findInIn(elem.value()) == set->endIn();
	}
	Bool setToTrue()
	{
		detach(pOwner);
		return set.solver().post(notMember(elem.getView(),set.getView()));
	}
	Bool setToFalse()
	{
		detach(pOwner);
		return set.solver().post(member(elem.getView(),set.getView()));
	}
	ICPSolver& solver() const {	return set.solver();	}

	void attach(INotifiable* f) { 	pOwner=f; elem.attach(f); set->attachOnDomain(f);}
	void detach(INotifiable* f) {	elem.detach(f); set->detachOnDomain(f);}

	Rel2<NotMember,View1,View2> getView()  const
	{ 	return Rel2<NotMember,View1,View2>(elem.getView(),set.getView());	}

	ValView<Elem,View1>	elem;
	DomView<Set<Elem>,View2>	set;
	INotifiable*	pOwner;
};


// FIXME: not tested at all
template<class Elem,class View1,class View2>
struct ChkViewRel2<Contained,Set<Elem>,View1,Set<Elem>,View2>
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;

	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
			x(solver,p1),y(solver,p2) {}
	Bool isTrue() const	// when lub(x) C glb(y)
	{
		return x->cardMin() <= y->cardMax() and
			   detail::contained(x->beginIn(),x->endIn(),
								 y->beginIn(),y->endIn()) and
			   detail::contained(x->beginPoss(),x->endPoss(),
								 y->beginIn(),y->endIn());
	}
	Bool canBeTrue() const 	// when glb(x) C lub(y)
	{
		return x->cardMin() <= y->cardMax() and
				!makeDiffIt(makeInIt(*x),makeLUBIt(*y)).valid();
	}
	Bool setToTrue()
	{
		detach(pOwner);
		return solver().post(Bnd(contained(x.getView(),y.getView())));
	}
	Bool setToFalse()
	{
		detach(pOwner);
		throw ENoFilter("NotContained(set,set)");
	}
	ICPSolver& solver() const {	return x->solver();	}

	void attach(INotifiable* f)
	{ 	pOwner=f; x->attachOnDomain(f);	y->attachOnDomain(f);}
	void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f);}

	Rel2<Contained,View1,View2> getView()  const
	{ 	return Rel2<Contained,View1,View2>(x.getView(),y.getView());	}

	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	INotifiable*	pOwner;
};


// FIXME: not tested at all
template<class Elem,class View1,class View2>
struct ChkViewRel2<Equal,Set<Elem>,View1,Set<Elem>,View2>
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;

	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
			x(solver,p1),y(solver,p2),xCy(solver,p1,p2),yCx(solver,p2,p1) {}
	Bool isTrue() const	// when lub(x) = lub(y)
	{
		return x->ground() and y->ground() and
			   detail::equal(x->beginIn(),x->endIn(),
							 y->beginIn(),y->endIn());
	}
	Bool canBeTrue() const 	// when glb(x) C lub(y) and glb(y) C lub(x)
	{	return xCy.canBeTrue() and yCx.canBeTrue();	}
	Bool setToTrue()
	{
		detach(pOwner);
		return solver().post(Bnd(equal(x.getView(),y.getView())));
	}
	Bool setToFalse()
	{
		detach(pOwner);
		return solver().post(Bnd(distinct(x.getView(),y.getView())));
	}
	ICPSolver& solver() const {	return x->solver();	}

	void attach(INotifiable* f)
	{ 	pOwner=f; x->attachOnDomain(f);	y->attachOnDomain(f);}
	void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f);}

	Rel2<Equal,View1,View2> getView()  const
	{ 	return Rel2<Equal,View1,View2>(x.getView(),y.getView());	}

	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	INotifiable*	pOwner;
	ChkViewRel2<Contained,Set<Elem>,View1,Set<Elem>,View2>	xCy;
	ChkViewRel2<Contained,Set<Elem>,View2,Set<Elem>,View1>	yCx;
};


template<class Elem,class View1,class View2>
struct ChkViewRel2<Disjoint,Set<Elem>,View1,Set<Elem>,View2>
{
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
			x(solver,p1),y(solver,p2) {}
	Bool isTrue() const	// is it true?
	{
		return !makeInterIt(makeLUBIt(*x),makeLUBIt(*y)).valid();
	}
	Bool canBeTrue() const 	// can it still be true?
	{
		return !detail::intersect(x->beginIn(),x->endIn(),
								  y->beginIn(),y->endIn());
	}
	Bool setToTrue()
	{
		detach(pOwner);
		return solver().post(disjoint(x.getView(),y.getView()));
	}
	Bool setToFalse()		// FIXME: using views instead of auxvar
	{
		detach(pOwner);
		detail::Vector<Elem> lub1(x->inSize()+x->possSize());
		detail::setLUB(*x,lub1);

		detail::Vector<Elem> lub2(y->inSize()+y->possSize());
		detail::setLUB(*y,lub2);

		detail::QuickList<Elem> lub3(lub1.size()+lub2.size());
		detail::setUnion(lub1.begin(),lub1.end(),
						 lub2.begin(),lub2.end(),
						 detail::ListInserter<Elem>(lub3));
		DomVar<IntSet> aux(solver(),lub3);
		return solver().post(intersect(x.getView(),y.getView(),aux) and
							 cardinal(aux)>0);
	}
	ICPSolver& solver() const {	return x.solver();	}

	void attach(INotifiable* f) { 	pOwner=f; x->attachOnDomain(f); y->attachOnDomain(f);}
	void detach(INotifiable* f) {	x->detachOnDomain(f); y->detachOnDomain(f);}

	Rel2<Disjoint,View1,View2> getView()  const
	{ 	return Rel2<Disjoint,View1,View2>(x.getView(),y.getView());	}

	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	INotifiable*	pOwner;
};


// FIXME: not tested at all
template<class Elem,class View1,class View2,class View3>
struct ChkViewRel3<Intersect,Set<Elem>,View1,Set<Elem>,View2,
					Set<Elem>,View3>
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;
	typedef typename DomView<Set<Elem>,View3>::Dom	DomZ;

	ChkViewRel3(ICPSolver& solver, const View1& p1,const View2& p2,
					const View3& p3) :
			x(solver,p1),y(solver,p2),z(solver,p3) {}
	Bool isTrue() const	// is it true?
	{
		if (!z->ground() or
			z->minCard()>std::min(x->maxCard(),y->maxCard()))
			return false;

		return  !makeSymDiffIt(makeInterIt(makeInIt(*x),makeInIt(*y)),makeInIt(*z)).valid() and
				!makeDiffIt(makePossIt(*x),makeInIt(*z)).valid() and
				!makeDiffIt(makePossIt(*y),makeInIt(*z)).valid();
	}
	Bool canBeTrue() const 	// can it still be true?
	{
		// if in(z) C intersection(lub(x),lub(y)) and
		//  intersection(in(x),in(y)) C lub(z)

		return z->minCard()<=std::min(x->maxCard(),y->maxCard()) and
				!makeDiffIt(makeInIt(*z),makeInterIt(makeLUBIt(*x),makeLUBIt(*y))).valid() and
				!makeDiffIt(makeInterIt(makeInIt(*x),makeInIt(*y)),makeLUBIt(*z)).valid();
	}
	Bool setToTrue()
	{
		detach(pOwner);
		return solver().post(intersect(x.getView(),y.getView(),z.getView()));
	}
	Bool setToFalse()		// FIXME: using views instead of auxvar
	{	throw ENoFilter("NotIntersect(set,set,set)");	}
	ICPSolver& solver() const {	return x.solver();	}

	void attach(INotifiable* f)
	{ 	pOwner=f; x->attachOnDomain(f); y->attachOnDomain(f); z->attachOnDomain(f);}
	void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f); z->detachOnDomain(f);	}

	Rel3<Intersect,View1,View2,View3> getView()  const
	{ 	return Rel3<Intersect,View1,View2,View3>(x.getView(),y.getView(),z.getView());	}

	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	DomView<Set<Elem>,View3>	z;
	INotifiable*	pOwner;
};


// FIXME: not tested at all
template<class Elem,class View1,class View2,class View3>
struct ChkViewRel3<Union,Set<Elem>,View1,Set<Elem>,View2,Set<Elem>,View3>
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;
	typedef typename DomView<Set<Elem>,View3>::Dom	DomZ;

	ChkViewRel3(ICPSolver& solver, const View1& p1,const View2& p2,
					const View3& p3) :
			x(solver,p1),y(solver,p2),z(solver,p3) {}

	Bool isTrue() const	// is it true?
	{
		if (!z->ground() or z->card().value()>x->card().max()+y->card().max())
			return false;

		return !makeSymDiffIt(makeUnionIt(makeInIt(*x),makeInIt(*y)),makeInIt(*z)).valid() and
			   !makeDiffIt(makePossIt(*x),makeInIt(*z)).valid() and
			   !makeDiffIt(makePossIt(*y),makeInIt(*z)).valid();
	}

	Bool canBeTrue() const 	// can it still be true?
	{
		// if (in(x) U in(y)) C lub(z)  and
		//    in(z) C (lub(x) U lub(y))

		return z->card().min() <= x->card().max()+y->card().max() and
				!makeDiffIt(makeUnionIt(makeInIt(*x),makeInIt(*y)),makeLUBIt(*z)).valid() and
				!makeDiffIt(makeInIt(*z),makeUnionIt(makeLUBIt(*x),makeLUBIt(*y))).valid();
	}
	Bool setToTrue()
	{
		detach(pOwner);
		return solver().post(union_(x.getView(),y.getView(),z.getView()));
	}
	Bool setToFalse()
	{	throw ENoFilter("NotUnion(set,set,set)");	}
	ICPSolver& solver() const {	return x.solver();	}

	void attach(INotifiable* f)
	{ 	pOwner=f; x->attachOnDomain(f); y->attachOnDomain(f); z->attachOnDomain(f);}
	void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f); z->detachOnDomain(f);	}

	Rel3<Union,View1,View2,View3> getView()  const
	{ 	return Rel3<Union,View1,View2,View3>(x.getView(),y.getView(),z.getView());	}

	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	DomView<Set<Elem>,View3>	z;
	INotifiable*	pOwner;
};


}

#endif /* CASPER_SET_CHKVIEW_H_ */
