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

#ifndef _H_CASPER_KERNEL_GACFILTER
#define _H_CASPER_KERNEL_GACFILTER

#include <casper/kernel/filter/common.h>
#include <casper/kernel/filter/basic.h>
#include <casper/kernel/view/domview.h>
#include <casper/kernel/view/arrayview.h>
#include <casper/kernel/filter/bacfilter.h>

namespace casper {

template<class,class,class> // TODO: default is to use FilterExpr1
struct DomFilterView1;

template<class,class,class,class,class>
struct DomFilterView2; // TODO: default is to use FilterExpr2

template<class,class,class,class,class,class,class>
struct DomFilterView3; // TODO: default is to use FilterExpr3

template<class Rel>
struct DomFilterView; // default is to complain at compile time

// happens for example when the negation for a relation is not defined
template<>
struct DomFilterView<Rel0<UnknownRel> > : NoFilter
{
	DomFilterView(ICPSolver& s,const Rel0<UnknownRel>&) : NoFilter(s,*this) {}
};

template<class F,class T1>
struct DomFilterView<Rel1<F,T1> > :
	public DomFilterView1<F,typename EvalTraits<T1>::Eval,T1>
{
	DomFilterView(ICPSolver& s,const Rel1<F,T1>& r) :
		DomFilterView1<F,typename EvalTraits<T1>::Eval,T1>(s,r.p1) {}
};

template<class F,class T1,class T2>
struct DomFilterView<Rel2<F,T1,T2> > :
	public DomFilterView2<F,typename EvalTraits<T1>::Eval,T1,
						 typename EvalTraits<T2>::Eval,T2>
{
	DomFilterView(ICPSolver& s,const Rel2<F,T1,T2>& r) :
		DomFilterView2<F,typename EvalTraits<T1>::Eval,T1,
					  typename EvalTraits<T2>::Eval,T2>(s,r.p1,r.p2) {}
};

template<class F,class T1,class T2,class T3>
struct DomFilterView<Rel3<F,T1,T2,T3> > :
	public DomFilterView3<F,typename EvalTraits<T1>::Eval,T1,
						 typename EvalTraits<T2>::Eval,T2,
						 typename EvalTraits<T3>::Eval,T3>
{
	DomFilterView(ICPSolver& s,const Rel3<F,T1,T2,T3>& r) :
		DomFilterView3<F,typename EvalTraits<T1>::Eval,T1,
					  typename EvalTraits<T2>::Eval,T2,
					  typename EvalTraits<T3>::Eval,T3>(s,r.p1,r.p2,r.p3) {}
};

// FIXME: this disables all GAC filters (must specialize to fix)
#if 1
template<class Func,class Eval1,class View1>
struct DomFilterFact1;

template<class Func,class Eval1,class View1,class Eval2,class View2>
struct DomFilterFact2;

template<class Func,class Eval1,class View1,class Eval2,class View2,
		class Eval3,class View3>
struct DomFilterFact3;

#else

template<class Func,class Eval1,class View1>
struct DomFilterFact1
{
	static Filter create(ICPSolver& s,const View1& v1)
	{	return new (s.heap()) DomFilterView1<Func,Eval1,View1>(s,v1);}
};

template<class Func,class Eval1,class View1,class Eval2,class View2>
struct DomFilterFact2
{
	static Filter create(ICPSolver& s,const View1& v1,const View2& v2)
	{	return new (s.heap())
			DomFilterView2<Func,Eval1,View1,Eval2,View2>(s,v1,v2);}
};

template<class Func,class Eval1,class View1,class Eval2,class View2,
		class Eval3,class View3>
struct DomFilterFact3
{
	static Filter create(ICPSolver& s,const View1& v1,const View2& v2,
							const View3& v3)
	{	return new (s.heap())
			DomFilterView3<Func,Eval1,View1,Eval2,View2,Eval3,View3>(s,v1,v2,v3);}
};
#endif


/**
 * Enforces the conjunction of two constraints by creating a Dom filter
 * for each constraint.
 */
template<class Eval1,class View1,class Eval2,class View2>
struct DomFilterView2<And,Eval1,View1,Eval2,View2> : IFilter
{
	DomFilterView2(ICPSolver& s,const View1& p1, const View2& p2) :
		IFilter(s),
		p1(new (s.heap()) DomFilterView<View1>(s,p1)),
		p2(new (s.heap()) DomFilterView<View2>(s,p2)) {}

	bool execute()
	{	return solver().post(p1) and solver().post(p2);	}
	bool entailed() const
	{	return p1->entailed() && p2->entailed(); }
	Filter operator!()
	{	return p1->operator!() or p2->operator!();	}
	void attach(INotifiable* s) { p1->attach(s); p2->attach(s); }
	void detach(INotifiable* s) { p1->detach(s); p2->detach(s); }

	DomFilterView<View1>*	p1;
	DomFilterView<View2>*	p2;
	INotifiable* pOwner;
};

/**
	Enforces the disjunction of two constraints. It succeeds if at least
	one constraint is satisfiable.
	\note It requires that both constraints are reifiable.
*/
template<class Eval1,class View1,class Eval2,class View2>
struct DomFilterView2<Or,Eval1,View1,Eval2,View2> : IFilter
{
	DomFilterView2(ICPSolver& s,const View1& p1, const View2& p2) :
		IFilter(s),p1(s,p1),p2(s,p2),
		f1(new (s.heap()) DomFilterView<View1>(s,p1)),
		f2(new (s.heap()) DomFilterView<View2>(s,p2)) {}

	bool execute()
	{
		if (p1.min() || p2.min())
			return true;
		if (p1.max()==0)
		{
			detach(pOwner);
			return solver().post(f2);
		}
		else
		if (p2.max()==0)
		{
			detach(pOwner);
			return solver().post(f1);
		}
		return true;
	}
	bool entailed() const
	{	return p1.min() || p2.min(); }
	Filter operator!()
	{	return !(*p1) and !(*p2);	}
	void attach(INotifiable* s)
	{	pOwner=s; p1.attach(s); p2.attach(s);	}
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s); }

	BndView<Bool,View1>	p1;
	BndView<Bool,View2>	p2;
	DomFilterView<View1>*	f1;
	DomFilterView<View2>*	f2;
	INotifiable* pOwner;
};

// --- Filters that fallback to BndFilter: ---

///	Enforces the GreaterEqual binary relation between two views.
template<class Eval1,class View1,class Eval2,class View2>
struct DomFilterView2<GreaterEqual,Eval1,View1,Eval2,View2> :
	BndFilterView2<GreaterEqual,Eval1,View1,Eval2,View2>
{
	DomFilterView2(ICPSolver& solver, const View1& p1,const View2& p2) :
		BndFilterView2<GreaterEqual,Eval1,View1,Eval2,View2>(solver,p1,p2) {}
};


///	Enforces the LessEqual binary relation between two views.
template<class Eval1,class View1,class Eval2,class View2>
struct DomFilterView2<LessEqual,Eval1,View1,Eval2,View2> :
	BndFilterView2<GreaterEqual,Eval2,View2,Eval1,View1>
{
	DomFilterView2(ICPSolver& solver,const View1& p1, const View2& p2) :
		BndFilterView2<GreaterEqual,Eval2,View2,Eval1,View1>(solver,p2,p1) {}
};

///	Enforces the Greater binary relation between two views.
template<class Eval1,class View1,class Eval2,class View2>
struct DomFilterView2<Greater,Eval1,View1,Eval2,View2> :
	BndFilterView2<Greater,Eval1,View1,Eval2,View2>
{
	DomFilterView2(ICPSolver& solver, const View1& p1,const View2& p2) :
		BndFilterView2<Greater,Eval1,View1,Eval2,View2>(solver,p1,p2) {}
};

/// Enforces bound consistenct on less constraint between two views.
template<class Eval1,class Eval2,class View1,class View2>
struct DomFilterView2<Less,Eval1,View1,Eval2,View2> :
	BndFilterView2<Greater,Eval2,View2,Eval1,View1>
{
	DomFilterView2(ICPSolver& solver, const View1& p1, const View2& p2) :
		BndFilterView2<Greater,Eval2,View2,Eval1,View1>(solver,p2,p1) {}
};

// dom view over element expression
template<class View1,class View2,class Eval>
struct DomView<Eval,Rel2<Element,View1,View2> > :
	DomView<Eval,DomVar<Eval,typename DomTraits<typename ElemTraits<View1>::Elem>::Dom> >
{
	typedef typename DomTraits<typename ElemTraits<View1>::Elem>::Dom Dom;
	typedef DomView<Eval,DomVar<Eval,Dom> >	Super;


	static Dom* getDom(ICPSolver& s,const View1& p1,const View2& p2)
	{
		if (ValExpr<Int>(s,p2).ground())
			return &(*DomArrayView<Eval,View1>(s,p1)[ValExpr<Int>(s,p2).value()]);
		else
		{
			DomVar<Eval,Dom> v(s,detail::VarDomCreator<Dom>().unionOf(s,p1));
			s.post(element(p1,p2,v));
			return v.pDomain();
		}
	}

	DomView(ICPSolver& solver,const Rel2<Element,View1,View2>& p) :
		Super(solver,DomVar<Eval,Dom>(solver,getDom(solver,p.p1,p.p2))) {}
};

// TODO: DomFilterView2<Equal,Eval1,View1,Eval2,View2>

/* TODO: DomFilterView2<Less,Bool,View1,Bool,View2>
 * should use DomFilterView for filters...
 *
 * example: DomFilter( (a==b) <= (distinct(vars)) ) uses Bnd distinct
 *
 * idea1: ICPSolver::currentFilterFactory
 * 	.... but filterFactory is a compile time object..
 *
 * idea2: always uses Bnd, user must override specific cases:
 *  DomFilter( (a==b) <= (DomFilter(distinct(vars))) ) uses Bnd distinct
 * .... but this is ugly
 */

// TODO: Dom expression from filter (reification)

/**
 * TODO: see BndFilterFact for complete method list
 */
struct DomFilterFact
{
	/*template<class View>
	Filter operator()(const View& v) const
	{	return new (getCPSolver(v).heap()) DomFilterView<View>(getCPSolver(v),v);	}
	*/
	template<class Func,class View1>
	Filter operator()(const Rel1<Func,View1>& v) const
	{
		typedef typename EvalTraits<View1>::Eval	Eval1;
		typedef DomFilterFact1<Func,Eval1,View1>	Fact;
		return Fact::create(getCPSolver(v),v.p1);
	}
	template<class Func,class View1,class View2>
	Filter operator()(const Rel2<Func,View1,View2>& v) const
	{
		typedef typename EvalTraits<View1>::Eval	Eval1;
		typedef typename EvalTraits<View2>::Eval	Eval2;
		typedef DomFilterFact2<Func,Eval1,View1,Eval2,View2>	Fact;
		return Fact::create(getCPSolver(v),v.p1,v.p2);
	}
	template<class Func,class View1,class View2,class View3>
	Filter operator()(const Rel3<Func,View1,View2,View3>& v) const
	{
		typedef typename EvalTraits<View1>::Eval	Eval1;
		typedef typename EvalTraits<View2>::Eval	Eval2;
		typedef typename EvalTraits<View3>::Eval	Eval3;
		typedef DomFilterFact3<Func,Eval1,View1,Eval2,View2,Eval3,View3> Fact;
		return Fact::create(getCPSolver(v),v.p1,v.p2,v.p3);
	}
};

extern DomFilterFact Dom;

};

#endif /*_H_CASPER_KERNEL_GACFILTER */
