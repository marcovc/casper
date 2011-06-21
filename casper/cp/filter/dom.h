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

#ifndef _H_CASPER_CP_FILTER_DOM
#define _H_CASPER_CP_FILTER_DOM

#include <casper/cp/filter/common.h>
#include <casper/cp/filter/basic.h>
#include <casper/cp/view/dom.h>
#include <casper/cp/view/array.h>
#include <casper/cp/filter/bnd.h>

namespace Casper {
namespace CP {


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
	DomFilterView(Store& s,const Rel0<UnknownRel>&) : NoFilter(s,*this) {}
};

template<class F,class T1>
struct DomFilterView<Rel1<F,T1> > :
	public DomFilterView1<F,typename Casper::Traits::GetEval<T1>::Type,T1>
{
	DomFilterView(Store& s,const Rel1<F,T1>& r) :
		DomFilterView1<F,typename Casper::Traits::GetEval<T1>::Type,T1>(s,r.p1) {}
};

template<class F,class T1,class T2>
struct DomFilterView<Rel2<F,T1,T2> > :
	public DomFilterView2<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 typename Casper::Traits::GetEval<T2>::Type,T2>
{
	DomFilterView(Store& s,const Rel2<F,T1,T2>& r) :
		DomFilterView2<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  typename Casper::Traits::GetEval<T2>::Type,T2>(s,r.p1,r.p2) {}
};

template<class F,class T1,class T2,class T3>
struct DomFilterView<Rel3<F,T1,T2,T3> > :
	public DomFilterView3<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 typename Casper::Traits::GetEval<T2>::Type,T2,
						 typename Casper::Traits::GetEval<T3>::Type,T3>
{
	DomFilterView(Store& s,const Rel3<F,T1,T2,T3>& r) :
		DomFilterView3<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  typename Casper::Traits::GetEval<T2>::Type,T2,
					  typename Casper::Traits::GetEval<T3>::Type,T3>(s,r.p1,r.p2,r.p3) {}
};

// FIXME: this disables all GAC filters (must specialize to fix)
#if 1
template<class Func,class Eval1,class View1>
struct PostDomFilter1;

template<class Func,class Eval1,class View1,class Eval2,class View2>
struct PostDomFilter2;

template<class Func,class Eval1,class View1,class Eval2,class View2,
		class Eval3,class View3>
struct PostDomFilter3;

#else

template<class Func,class Eval1,class View1>
struct PostDomFilter1
{
	static bool post(Store& s,const View1& v1)
	{	return s.post(new (s) DomFilterView1<Func,Eval1,View1>(s,v1));	}
};

template<class Func,class Eval1,class View1,class Eval2,class View2>
struct PostDomFilter2
{
	static bool post(Store& s,const View1& v1,const View2& v2)
	{	return s.post (new (s) DomFilterView2<Func,Eval1,View1,Eval2,View2>(s,v1,v2);}
};

template<class Func,class Eval1,class View1,class Eval2,class View2,
		class Eval3,class View3>
struct PostDomFilter3
{
	static bool post(Store& s,const View1& v1,const View2& v2,
								const View3& v3)
	{	return s.post(new (s)
			DomFilterView3<Func,Eval1,View1,Eval2,View2,Eval3,View3>(s,v1,v2,v3);}
};
#endif

// FIXME: same problem as for And for Bnd filters
/**
 * Enforces the conjunction of two constraints by creating a Dom filter
 * for each constraint.
 */
template<class Eval1,class View1,class Eval2,class View2>
struct DomFilterView2<And,Eval1,View1,Eval2,View2> : IFilter
{
	DomFilterView2(Store& s,const View1& p1, const View2& p2) :
		IFilter(s),
		store(store),
		p1(new (s) DomFilterView<View1>(s,p1)),
		p2(new (s) DomFilterView<View2>(s,p2)) {}

	bool execute()
	{	return postDomFilter(store,p1) and postDomFilter(store,p2);	}
//	Filter operator!()
//	{	return p1->operator!() or p2->operator!();	}
	void attach(INotifiable* s) { p1->attach(s); p2->attach(s); }
	void detach(INotifiable* s) { p1->detach(s); p2->detach(s); }

	Store&					store;
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
	DomFilterView2(Store& s,const View1& p1, const View2& p2) :
		IFilter(s),store(store),p1(s,p1),p2(s,p2),
		f1(new (s) DomFilterView<View1>(s,p1)),
		f2(new (s) DomFilterView<View2>(s,p2)) {}

	bool execute()
	{
		if (p1.min() || p2.min())
			return true;
		if (p1.max()==0)
		{
			detach(pOwner);
			return postDomFilter(store,f2);
		}
		else
		if (p2.max()==0)
		{
			detach(pOwner);
			return postDomFilter(store,f1);
		}
		return true;
	}
/*	bool entailed() const
	{	return p1.min() || p2.min(); }
	Filter operator!()
	{	return !(*p1) and !(*p2);	}
*/	void attach(INotifiable* s)
	{	pOwner=s; p1.attach(s); p2.attach(s);	}
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s); }

	Store&				store;
	BndView<bool,View1>	p1;
	BndView<bool,View2>	p2;
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
	DomFilterView2(Store& store, const View1& p1,const View2& p2) :
		BndFilterView2<GreaterEqual,Eval1,View1,Eval2,View2>(store,p1,p2) {}
};


///	Enforces the LessEqual binary relation between two views.
template<class Eval1,class View1,class Eval2,class View2>
struct DomFilterView2<LessEqual,Eval1,View1,Eval2,View2> :
	BndFilterView2<GreaterEqual,Eval2,View2,Eval1,View1>
{
	DomFilterView2(Store& store,const View1& p1, const View2& p2) :
		BndFilterView2<GreaterEqual,Eval2,View2,Eval1,View1>(store,p2,p1) {}
};

///	Enforces the Greater binary relation between two views.
template<class Eval1,class View1,class Eval2,class View2>
struct DomFilterView2<Greater,Eval1,View1,Eval2,View2> :
	BndFilterView2<Greater,Eval1,View1,Eval2,View2>
{
	DomFilterView2(Store& store, const View1& p1,const View2& p2) :
		BndFilterView2<Greater,Eval1,View1,Eval2,View2>(store,p1,p2) {}
};

/// Enforces bound consistenct on less constraint between two views.
template<class Eval1,class Eval2,class View1,class View2>
struct DomFilterView2<Less,Eval1,View1,Eval2,View2> :
	BndFilterView2<Greater,Eval2,View2,Eval1,View1>
{
	DomFilterView2(Store& store, const View1& p1, const View2& p2) :
		BndFilterView2<Greater,Eval2,View2,Eval1,View1>(store,p2,p1) {}
};

// dom view over element expression
template<class View1,class View2,class Eval>
struct DomView<Eval,Rel2<Element,View1,View2> > :
	DomView<Eval,Var<Eval,typename Traits::GetDom<typename Casper::Traits::GetTermElem<View1>::Type>::Type> >
{
	typedef typename Traits::GetDom<typename Casper::Traits::GetTermElem<View1>::Type>::Type Dom;
	typedef DomView<Eval,Var<Eval,Dom> >	Super;


	static Dom* getDom(Store& s,const View1& p1,const View2& p2)
	{
		if (ValExpr<int>(s,p2).ground())
			return &(*DomArrayView<Eval,View1>(s,p1)[ValExpr<int>(s,p2).value()]);
		else
		{
			Var<Eval,Dom> v(s,Detail::VarDomCreator<Dom>().unionOf(s,p1));
			postDomFilter(s,elementEqual(p1,p2,v));
			return v.pDomain();
		}
	}

	DomView(Store& store,const Rel2<Element,View1,View2>& p) :
		Super(store,Var<Eval,Dom>(store,getDom(store,p.p1,p.p2))) {}
};



// TODO: DomFilterView2<Equal,Eval1,View1,Eval2,View2>

/* TODO: DomFilterView2<Less,bool,View1,bool,View2>
 * should use DomFilterView for filters...
 *
 * example: DomFilter( (a==b) <= (distinct(vars)) ) uses Bnd distinct
 *
 * idea1: CPSolver::currentFilterFactory
 * 	.... but filterFactory is a compile time object..
 *
 * idea2: always uses Bnd, user must override specific cases:
 *  DomFilter( (a==b) <= (DomFilter(distinct(vars))) ) uses Bnd distinct
 * .... but this is ugly
 */

// TODO: Dom expression from filter (reification)

/**
 * TODO: see PostBndFilter for complete method list
 */
struct PostDomFilter
{

	template<class Func,class View1>
	bool operator()(Store& s,const Rel1<Func,View1>& v) const
	{
		typedef typename Casper::Traits::GetEval<View1>::Type	Eval1;
		typedef PostDomFilter1<Func,Eval1,View1>		Post;
		return Post::post(s,v.p1);
	}
	template<class Func,class View1,class View2>
	bool operator()(Store& s,const Rel2<Func,View1,View2>& v) const
	{
		typedef typename Casper::Traits::GetEval<View1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<View2>::Type	Eval2;
		typedef PostDomFilter2<Func,Eval1,View1,Eval2,View2>	Post;
		return Post::post(s,v.p1,v.p2);
	}
	template<class Func,class View1,class View2,class View3>
	bool operator()(Store& s,const Rel3<Func,View1,View2,View3>& v) const
	{
		typedef typename Casper::Traits::GetEval<View1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<View2>::Type	Eval2;
		typedef typename Casper::Traits::GetEval<View3>::Type	Eval3;
		typedef PostDomFilter3<Func,Eval1,View1,Eval2,View2,Eval3,View3> Post;
		return Post::post(s,v.p1,v.p2,v.p3);
	}
};

extern PostDomFilter postDomFilter;

} // CP
} // Casper

#endif /*_H_CASPER_CP_FILTER_DOM */
