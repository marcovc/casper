 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_VALFILTER
#define _H_CASPER_KERNEL_VALFILTER

#include <casper/kernel/filter/common.h>
#include <casper/kernel/filter/common.h>
#include <casper/kernel/filter/bacfilter.h>	//tmp
#include <casper/kernel/view/valview.h>
#include <casper/kernel/view/bndview.h>

namespace casper {

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
	ValFilterView(ICPSolver& s,const Rel0<UnknownRel>&) : NoFilter(s,*this) {}
};

template<class F,class T1>
struct ValFilterView<Rel1<F,T1> > :
	public ValFilterView1<F,typename EvalTraits<T1>::Eval,T1>
{
	ValFilterView(ICPSolver& s,const Rel1<F,T1>& r) :
		ValFilterView1<F,typename EvalTraits<T1>::Eval,T1>(s,r.p1) {}
};

template<class F,class T1,class T2>
struct ValFilterView<Rel2<F,T1,T2> > :
	public ValFilterView2<F,typename EvalTraits<T1>::Eval,T1,
						 typename EvalTraits<T2>::Eval,T2>
{
	ValFilterView(ICPSolver& s,const Rel2<F,T1,T2>& r) :
		ValFilterView2<F,typename EvalTraits<T1>::Eval,T1,
					  typename EvalTraits<T2>::Eval,T2>(s,r.p1,r.p2) {}
};

template<class F,class T1,class T2,class T3>
struct ValFilterView<Rel3<F,T1,T2,T3> > :
	public ValFilterView3<F,typename EvalTraits<T1>::Eval,T1,
						 typename EvalTraits<T2>::Eval,T2,
						 typename EvalTraits<T3>::Eval,T3>
{
	ValFilterView(ICPSolver& s,const Rel3<F,T1,T2,T3>& r) :
		ValFilterView3<F,typename EvalTraits<T1>::Eval,T1,
					  typename EvalTraits<T2>::Eval,T2,
					  typename EvalTraits<T3>::Eval,T3>(s,r.p1,r.p2,r.p3) {}
};

/**
 * TODO: This should use 'ValView::setValue' method
 * Enforces the conjunction of two constraints. It succeeds if both
 * constraints are satisfiable, fails otherwise.
 */
template<class Eval1,class View1,class Eval2,class View2>
struct ValFilterView2<And,Eval1,View1,Eval2,View2> : IFilter
{
	ValFilterView2(ICPSolver& s,const View1& p1, const View2& p2) :
		IFilter(s),p1(s,p1),p2(s,p2),v1(p1),v2(p2) {}

	bool execute()
	{	return p1.updateMin(true) && p2.updateMin(true); }
	bool entailed() const
	{	return p1.min()==p2.min() && p1.min()==true; }
	Filter operator!()
	{	return Val(!v1 or !v2); }
	void attach(INotifiable* s) {}
	void detach(INotifiable* s) {}

	BndView<Bool,View1>	p1;
	BndView<Bool,View2>	p2;
	View1	v1;
	View2	v2;
};

#if 0
/**
	Value expression from filter (reification)
	TODO: this should be like BndReify
*/
template<class View>
struct ValReify
{
	ValReify(ICPSolver& solver, const View& p1) :
						pos(p1),
						neg(!pos) {}
	Bool value() const { assert(ground()); return pos.entailed(); }
	Bool ground() const { return pos.entailed() || neg.entailed(); }
	void attach(INotifiable* f) { pos.attach(f); neg.attach(f); 	}
	void detach(INotifiable* f) { pos.detach(f); neg.detach(f); 	}
	Filter	pos;
	Filter	neg;
};

template<class F,class View1,class Eval>
struct ValViewRel1 : ValReify<Rel1<F,View1> >
{
	typedef Rel1<F,View1> Rel;
	typedef ValReify<Rel> Super;
	ValViewRel1(ICPSolver& solver, const View1& p1) :
		Super(solver,Rel(p1)) {}
};

template<class F, class View1,class View2,class Eval>
struct ValViewRel2 : ValReify<Rel2<F,View1,View2> >
{
	typedef Rel2<F,View1,View2> Rel;
	typedef ValReify<Rel> Super;
	ValViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		Super(solver,Rel(p1,p2)) {}
};
#endif

///	Enforces equality
// FIXME: must use setVal when ready
template<class Eval,class View1,class View2>
struct ValFilterView2<Equal,Eval,View1,Eval,View2> : IFilter
{
	ValFilterView2(ICPSolver& solver,const View1& p1,const View2& p2)
		: IFilter(solver),p1(solver,p1), p2(solver,p2),
			v1(p1),v2(p2) {}

	bool execute()
	{
		return true;
	}
	bool entailed() const
	{	return p1.ground() and p2.ground() and p1.value() == p2.value();	}
	Filter	operator!() const
	{	return Val(v1 != v2);	}
	void attach(INotifiable* s)
	{	p1.attach(s); p2.attach(s);	}
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	ValView<Eval,View1>	p1;
	ValView<Eval,View2>	p2;
	View1		v1;
	View2		v2;
};

///	Enforces disequality
// FIXME: must use setVal when ready
template<class Eval,class View1,class View2>
struct ValFilterView2<Distinct,Eval,View1,Eval,View2> : IFilter
{
	ValFilterView2(ICPSolver& solver,const View1& p1,const View2& p2)
		: IFilter(solver),p1(solver,p1), p2(solver,p2),
			v1(p1),v2(p2) {}

	bool execute()
	{
		return true;
	}
	bool entailed() const
	{	return p1.ground() and p2.ground() and p1.value() != p2.value();	}
	Filter	operator!() const
	{	return Val(v1 == v2);	}
	void attach(INotifiable* s)
	{	p1.attach(s); p2.attach(s);	}
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	ValView<Eval,View1>	p1;
	ValView<Eval,View2>	p2;
	View1		v1;
	View2		v2;
};

// TODO: Dom expression from filter (reification)

/**
	Value expression from filter (reification)
*/
/*
template<>
struct ValView<Bool,Filter> : ValReify<Filter>
{
	typedef ValReify<Filter> Super;

	ValView(ICPSolver& solver, const Filter& v) :
						Super(solver,v) {}
};
*/
struct ValFilterFact
{
	template<class View>
	Filter operator()(const View& v) const
	{	return new (getCPSolver(v).heap()) ValFilterView<View>(getCPSolver(v),v);	}
};

extern ValFilterFact Val;


// AtMost constraint
template<class T,int Dim,class Dom>
struct AtMost : IFilter
{
	AtMost(ICPSolver& s, UInt occ, DomVarArray<T,Dim,Dom> vars, const T& val) :
		IFilter(s),occ(s,occ),vars(vars),val(val) {}

	struct VarDemon : INotifiable
	{
		VarDemon(AtMost* pOwner, UInt index) :
			rOwner(*pOwner),index(index) {}
		Bool notify()
		{	return rOwner.notifyIndex(index);	}
		AtMost& rOwner;
		const UInt index;
	};
	Bool execute()	// this filter does everything in notification phase
	{	return true; }
	Bool notifyIndex(UInt index)
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
		UInt ss = vars.count();
		for (UInt i = 0; i < ss; i++)
			vars(i).domain().attachOnGround(new (solver().heap()) VarDemon(this,i));
	}
	void detach(INotifiable* s)
	{
		/* TODO: should detach all created demons */
	}
	Reversible<UInt> 		occ;
	DomVarArray<T,Dim,Dom> 	vars;
	const T 		val;
};

template<class T,int Dim,class Dom>
Filter atMost(UInt occ, DomVarArray<T,Dim,Dom> vars, const T& val)
{	return new (vars.solver().heap())
		AtMost<T,Dim,Dom>(vars.solver(),occ,vars,val); }


// AtLeast constraint
template<class T,int Dim,class Dom>
struct AtLeast : IFilter
{
	AtLeast(ICPSolver& s, UInt occ, DomVarArray<T,Dim,Dom> vars, const T& val) :
		IFilter(s),occ(s,vars.size()-occ),vars(vars),val(val) {}

	struct VarDemon : INotifiable
	{
		VarDemon(AtLeast* pOwner, UInt index) :
			rOwner(*pOwner),index(index) {}
		Bool notify()
		{	return rOwner.notifyIndex(index);	}
		AtLeast& rOwner;
		const UInt index;
	};
	Bool execute()	// this filter does everything in notification phase
	{	return true; }
	Bool notifyIndex(UInt index)
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
		UInt ss = vars.count();
		for (UInt i = 0; i < ss; i++)
			vars(i).domain().attachOnGround(new (solver().heap()) VarDemon(this,i));
	}
	void detach(INotifiable* s)
	{
		/* TODO: should detach all created demons */
	}
	Reversible<UInt> 		occ;
	DomVarArray<T,Dim,Dom> 	vars;
	const T 		val;
};

template<class T,int Dim,class Dom>
Filter atLeast(UInt occ, DomVarArray<T,Dim,Dom> vars, const T& val)
{	return new (vars.solver().heap())
		AtLeast<T,Dim,Dom>(vars.solver(),occ,vars,val); }

// Exactly constraint
template<class T,int Dim,class Dom>
struct Exactly : IFilter
{
	Exactly(ICPSolver& s, UInt occ, DomVarArray<T,Dim,Dom> vars, const T& val) :
		IFilter(s),eqocc(s,occ),neqocc(s,vars.size()-occ),
		vars(vars),
		val(val) {}

	struct VarDemon : INotifiable
	{
		VarDemon(Exactly* pOwner, UInt index) :
			rOwner(*pOwner),index(index) {}
		Bool notify()
		{	return rOwner.notifyIndex(index);	}
		Exactly& rOwner;
		const UInt index;
	};
	Bool execute()	// this filter does everything in notification phase
	{	return true; }
	Bool notifyIndex(UInt index)
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
		UInt ss = vars.count();
		for (UInt i = 0; i < ss; i++)
			vars(i).domain().attachOnGround(new (solver().heap()) VarDemon(this,i));
	}
	void detach(INotifiable* s)
	{
		/* TODO: should detach all created demons */
	}
	Reversible<UInt> 		eqocc;
	Reversible<UInt> 		neqocc;
	DomVarArray<T,Dim,Dom> 	vars;
	const T 		val;
};

template<class T,int Dim,class Dom>
Filter exactly(UInt occ, DomVarArray<T,Dim,Dom> vars, const T& val)
{	return new (vars.solver().heap())
		Exactly<T,Dim,Dom>(vars.solver(),occ,vars,val); }


template<class Eval>
struct ValFilterView<ValExpr<Eval> > : IFilter
{
	ValFilterView(ICPSolver& s,const ValExpr<Eval>& p) :
		IFilter(s),p(p) {}

	// TODO: use setVal when ready
	bool execute()
	{	return true; }
	bool entailed() const
	{	return p.ground() and p.value()==true;	}
	Filter operator!() const
	{	return Val(p==false); }
	void attach(INotifiable* s) {	p.attach(s);	}
	void detach(INotifiable* s) {	p.detach(s);	}

	ValExpr<Eval>	p;
};

};

#endif /*_H_CASPER_KERNEL_VALFILTER */
