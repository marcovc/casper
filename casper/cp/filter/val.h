 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_CP_FILTER_VAL
#define _H_CASPER_CP_FILTER_VAL

#include <casper/cp/filter/common.h>
#include <casper/cp/filter/common.h>
#include <casper/cp/filter/bnd.h>	//tmp
#include <casper/cp/view/val.h>
#include <casper/cp/view/bnd.h>

namespace Casper {
namespace CP {


template<class,class,class> // TODO: default is to use FilterExpr1
struct ValFilterView1;

template<class,class,class,class,class>
struct ValFilterView2; // TODO: default is to use FilterExpr2

template<class,class,class,class,class,class,class>
struct ValFilterView3; // TODO: default is to use FilterExpr3

template<class Rel>
struct ValFilterView; // default is to complain at compile time

// happens for example when the negation for a relation is not defined
template<>
struct ValFilterView<Rel0<UnknownRel> > : NoFilter
{
	ValFilterView(Store& s,const Rel0<UnknownRel>&) : NoFilter(s,*this) {}
};

template<class F,class T1>
struct ValFilterView<Rel1<F,T1> > :
	public ValFilterView1<F,typename Casper::Traits::GetEval<T1>::Type,T1>
{
	ValFilterView(Store& s,const Rel1<F,T1>& r) :
		ValFilterView1<F,typename Casper::Traits::GetEval<T1>::Type,T1>(s,r.p1) {}
};

template<class F,class T1,class T2>
struct ValFilterView<Rel2<F,T1,T2> > :
	public ValFilterView2<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 typename Casper::Traits::GetEval<T2>::Type,T2>
{
	ValFilterView(Store& s,const Rel2<F,T1,T2>& r) :
		ValFilterView2<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  typename Casper::Traits::GetEval<T2>::Type,T2>(s,r.p1,r.p2) {}
};

template<class F,class T1,class T2,class T3>
struct ValFilterView<Rel3<F,T1,T2,T3> > :
	public ValFilterView3<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 typename Casper::Traits::GetEval<T2>::Type,T2,
						 typename Casper::Traits::GetEval<T3>::Type,T3>
{
	ValFilterView(Store& s,const Rel3<F,T1,T2,T3>& r) :
		ValFilterView3<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  typename Casper::Traits::GetEval<T2>::Type,T2,
					  typename Casper::Traits::GetEval<T3>::Type,T3>(s,r.p1,r.p2,r.p3) {}
};

/**
 * FIXME: see same relation for bnd and dom
 * Enforces the conjunction of two constraints. It succeeds if both
 * constraints are satisfiable, fails otherwise.
 */
template<class Eval1,class Expr1,class Eval2,class Expr2>
struct ValFilterView2<And,Eval1,Expr1,Eval2,Expr2> : IFilter
{
	ValFilterView2(Store& s,const Expr1& p1, const Expr2& p2) :
		IFilter(s),store(s),v1(p1),v2(p2) {}

	bool execute()
	{	return postValFilter(store,v1) and postValFilter(store,v2);	}
/*
	bool entailed() const
	{	return p1.min()==p2.min() && p1.min()==true; }
	Filter operator!()
	{	return Val(!v1 or !v2); }
*/
	void attach(INotifiable* s) {}
	void detach(INotifiable* s) {}

	Store&				store;
	Expr1	v1;
	Expr2	v2;
};

#if 0
/**
	Value expression from filter (reification)
	TODO: this should be like BndReify
*/
template<class View>
struct ValReify
{
	ValReify(CPSolver& solver, const View& p1) :
						pos(p1),
						neg(!pos) {}
	bool value() const { assert(ground()); return pos.entailed(); }
	bool ground() const { return pos.entailed() || neg.entailed(); }
	void attach(INotifiable* f) { pos.attach(f); neg.attach(f); 	}
	void detach(INotifiable* f) { pos.detach(f); neg.detach(f); 	}
	Filter	pos;
	Filter	neg;
};

template<class F,class Expr1,class Eval>
struct ValViewRel1 : ValReify<Rel1<F,Expr1> >
{
	typedef Rel1<F,Expr1> Rel;
	typedef ValReify<Rel> Super;
	ValViewRel1(CPSolver& solver, const Expr1& p1) :
		Super(solver,Rel(p1)) {}
};

template<class F, class Expr1,class Expr2,class Eval>
struct ValViewRel2 : ValReify<Rel2<F,Expr1,Expr2> >
{
	typedef Rel2<F,Expr1,Expr2> Rel;
	typedef ValReify<Rel> Super;
	ValViewRel2(CPSolver& solver, const Expr1& p1, const Expr2& p2) :
		Super(solver,Rel(p1,p2)) {}
};
#endif

///	Enforces equality
// FIXME: must use setVal when ready
template<class Eval,class Expr1,class Expr2>
struct ValFilterView2<Equal,Eval,Expr1,Eval,Expr2> : IFilter
{
	ValFilterView2(Store& store,const Expr1& p1,const Expr2& p2)
			: IFilter(store),p1(store,p1), p2(store,p2) {}

	bool execute()
	{
		if (p1.ground())
			return p2.setValue(p1.value());
		else
		if (p2.ground())
			return p1.setValue(p2.value());
		return true;
	}
//	bool entailed() const
//	{	return p1.ground() and p2.ground() and p1.value() == p2.value();	}
//	Filter	operator!() const
//	{	return Val(v1 != v2);	}
	void attach(INotifiable* s)
	{	p1.attach(s); p2.attach(s);	}
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	ValView<Eval,Expr1>	p1;
	ValView<Eval,Expr2>	p2;
};

///	Enforces disequality
// FIXME: must use setVal when ready
template<class Eval,class Expr1,class Expr2>
struct ValFilterView2<Distinct,Eval,Expr1,Eval,Expr2> : IFilter
{
	ValFilterView2(Store& store,const Expr1& p1,const Expr2& p2)
		: IFilter(store),p1(store,p1), p2(store,p2) {}

	bool execute()
	{
		return !p1.ground() or !p2.ground() or p1.value()!=p2.value();
	}
//	bool entailed() const
//	{	return p1.ground() and p2.ground() and p1.value() != p2.value();	}
//	Filter	operator!() const
//	{	return Val(v1 == v2);	}
	void attach(INotifiable* s)
	{	p1.attach(s); p2.attach(s);	}
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	ValView<Eval,Expr1>	p1;
	ValView<Eval,Expr2>	p2;
};

// TODO: Dom expression from filter (reification)

/**
	Value expression from filter (reification)
*/
/*
template<>
struct ValView<bool,Filter> : ValReify<Filter>
{
	typedef ValReify<Filter> Super;

	ValView(CPSolver& solver, const Filter& v) :
						Super(solver,v) {}
};
*/
struct PostValFilter
{
	template<class View>
	bool operator()(Store& s,const View& v) const
	{	return s.post(new (s) ValFilterView<View>(s,v));	}
};

extern PostValFilter postValFilter;


// AtMost constraint
template<class T,int Dim,class Dom>
struct AtMost : IFilter
{
	AtMost(Store& s, uint occ, VarArray<T,Dim,Dom> vars, const T& val) :
		IFilter(s),store(s),occ(s,occ),vars(vars),val(val) {}

	struct VarDemon : INotifiable
	{
		VarDemon(AtMost* pOwner, uint index) :
			rOwner(*pOwner),index(index) {}
		bool notify()
		{	return rOwner.notifyIndex(index);	}
		AtMost& rOwner;
		const uint index;
	};
	bool execute()	// this filter does everything in notification phase
	{	return true; }
	bool notifyIndex(uint index)
	{
		assert(vars[index].ground());
		if (vars[index].domain().value() == val)
		{
			if (occ == 0)
				return false;
			occ = occ-1;
		}
		return true;
	}
	void attach(INotifiable* s)
	{
		uint ss = vars.count();
		for (uint i = 0; i < ss; i++)
			vars(i).domain().attachOnGround(new (store) VarDemon(this,i));
	}
	void detach(INotifiable* s)
	{
		/* TODO: should detach all created demons */
	}
	Store&					store;
	Reversible<uint> 		occ;
	VarArray<T,Dim,Dom> 	vars;
	const T 		val;
};

template<class T,int Dim,class Dom>
Filter atMost(Store& store,uint occ, VarArray<T,Dim,Dom> vars, const T& val)
{	return new (store) AtMost<T,Dim,Dom>(store,occ,vars,val); }


// AtLeast constraint
template<class T,int Dim,class Dom>
struct AtLeast : IFilter
{
	AtLeast(Store& s, uint occ, VarArray<T,Dim,Dom> vars, const T& val) :
		IFilter(s),store(store),occ(s,vars.size()-occ),vars(vars),val(val) {}

	struct VarDemon : INotifiable
	{
		VarDemon(AtLeast* pOwner, uint index) :
			rOwner(*pOwner),index(index) {}
		bool notify()
		{	return rOwner.notifyIndex(index);	}
		AtLeast& rOwner;
		const uint index;
	};
	bool execute()	// this filter does everything in notification phase
	{	return true; }
	bool notifyIndex(uint index)
	{
		assert(vars[index].ground());
		if (vars[index].domain().value() != val)
		{
			if (occ == 0)
				return false;
			occ = occ-1;
		}
		return true;
	}
	void attach(INotifiable* s)
	{
		uint ss = vars.count();
		for (uint i = 0; i < ss; i++)
			vars(i).domain().attachOnGround(new (store) VarDemon(this,i));
	}
	void detach(INotifiable* s)
	{
		/* TODO: should detach all created demons */
	}
	Store&					store;
	Reversible<uint> 		occ;
	VarArray<T,Dim,Dom> 	vars;
	const T 		val;
};

template<class T,int Dim,class Dom>
Filter atLeast(Store& store,uint occ, VarArray<T,Dim,Dom> vars, const T& val)
{	return new (store) AtLeast<T,Dim,Dom>(store,occ,vars,val); }

// Exactly constraint
template<class T,int Dim,class Dom>
struct Exactly : IFilter
{
	Exactly(Store& s, uint occ, VarArray<T,Dim,Dom> vars, const T& val) :
		IFilter(s),store(s),eqocc(s,occ),neqocc(s,vars.size()-occ),
		vars(vars),
		val(val) {}

	struct VarDemon : INotifiable
	{
		VarDemon(Exactly* pOwner, uint index) :
			rOwner(*pOwner),index(index) {}
		bool notify()
		{	return rOwner.notifyIndex(index);	}
		Exactly& rOwner;
		const uint index;
	};
	bool execute()	// this filter does everything in notification phase
	{	return true; }
	bool notifyIndex(uint index)
	{
		assert(vars[index].ground());
		if (vars[index].domain().value() == val)
		{
			if (eqocc == 0)
				return false;
			eqocc = eqocc-1;
		}
		else
		{
			if (neqocc == 0)
				return false;
			neqocc = neqocc-1;
		}

		return true;
	}
	void attach(INotifiable* s)
	{
		uint ss = vars.count();
		for (uint i = 0; i < ss; i++)
			vars(i).domain().attachOnGround(new (store) VarDemon(this,i));
	}
	void detach(INotifiable* s)
	{
		/* TODO: should detach all created demons */
	}
	Store&					store;
	Reversible<uint> 		eqocc;
	Reversible<uint> 		neqocc;
	VarArray<T,Dim,Dom> 	vars;
	const T 		val;
};

template<class T,int Dim,class Dom>
Filter exactly(Store& store,uint occ, VarArray<T,Dim,Dom> vars, const T& val)
{	return new (store) Exactly<T,Dim,Dom>(store,occ,vars,val); }


template<>
struct ValFilterView<ValExpr<bool> > : IFilter
{
	ValFilterView(Store& s,const ValExpr<bool>& p) :
		IFilter(s),p(p) {}

	// TODO: use setVal when ready
	bool execute()
	{	return p.setValue(true); }
//	bool entailed() const
//	{	return p.ground() and p.value()==true;	}
//	Filter operator!() const
//	{	return Val(p==false); }
	void attach(INotifiable* s) {	p.attach(s);	}
	void detach(INotifiable* s) {	p.detach(s);	}

	ValExpr<bool>	p;
};

} // CP
} // Casper

#endif /*_H_CASPER_CP_FILTER_VAL */
