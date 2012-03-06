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

#ifndef _H_CASPER_CP_FILTER_DOM
#define _H_CASPER_CP_FILTER_DOM

#include <casper/cp/filter/common.h>
#include <casper/cp/filter/basic.h>
#include <casper/cp/view/dom.h>
#include <casper/cp/view/array.h>
#include <casper/cp/filter/bnd.h>

namespace Casper {
namespace CP {

template<class,class,class>
struct DomFilterView1;

template<class,class,class,class,class>
struct DomFilterView2;

template<class,class,class,class,class,class,class>
struct DomFilterView3;

template<class,class,class,class,class,class,class,class,class>
struct DomFilterView4;

template<class,class,class,class,class,class,class,class,class,class,class>
struct DomFilterView5;

template<class Rel>
struct DomFilterView;

// happens for example when the negation for a relation is not defined
template<>
struct DomFilterView<Rel0<UnknownRel> > : UndefinedFilter
{
	DomFilterView(Store& s,const Rel0<UnknownRel>&) : UndefinedFilter(s,*this) {}
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

template<class F,class T1,class T2,class T3,class T4>
struct DomFilterView<Rel4<F,T1,T2,T3,T4> > :
	public DomFilterView4<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 typename Casper::Traits::GetEval<T2>::Type,T2,
						 typename Casper::Traits::GetEval<T3>::Type,T3,
						 typename Casper::Traits::GetEval<T4>::Type,T4>
{
	DomFilterView(Store& s,const Rel4<F,T1,T2,T3,T4>& r) :
		DomFilterView4<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  typename Casper::Traits::GetEval<T2>::Type,T2,
					  typename Casper::Traits::GetEval<T3>::Type,T3,
					  typename Casper::Traits::GetEval<T4>::Type,T4>(s,r.p1,r.p2,r.p3,r.p4) {}
};

template<class F,class T1,class T2,class T3,class T4,class T5>
struct DomFilterView<Rel5<F,T1,T2,T3,T4,T5> > :
	public DomFilterView5<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 typename Casper::Traits::GetEval<T2>::Type,T2,
						 typename Casper::Traits::GetEval<T3>::Type,T3,
						 typename Casper::Traits::GetEval<T4>::Type,T4,
						 typename Casper::Traits::GetEval<T5>::Type,T5>
{
	DomFilterView(Store& s,const Rel5<F,T1,T2,T3,T4,T5>& r) :
		DomFilterView5<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  typename Casper::Traits::GetEval<T2>::Type,T2,
					  typename Casper::Traits::GetEval<T3>::Type,T3,
					  typename Casper::Traits::GetEval<T4>::Type,T4,
					  typename Casper::Traits::GetEval<T5>::Type,T5>(s,r.p1,r.p2,r.p3,r.p4,r.p5) {}
};

template<class Expr1>
struct PostDomFilterGen
{
	static bool post(Store& s,const Expr1& v1)
	{
		std::ostringstream os;
		os << v1;
		throw Casper::Exception::UndefinedFilter(os.str().c_str(),"Domain",Casper::Traits::getTypeStr<Expr1>());
	}
};

template<class Func,class Eval1,class Expr1>
struct PostDomFilter1
{
	static bool post(Store& s,const Expr1& v1)
	{
		std::ostringstream os;
		::operator<<(os,rel<Func>(v1));
		throw Casper::Exception::UndefinedFilter(os.str().c_str(),"Domain",
				Casper::Traits::getTypeStr<Rel1<Func,Expr1> >());
	}
};

template<class Func,class Eval1,class Expr1,class Eval2,class Expr2>
struct PostDomFilter2
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2)
	{
		std::ostringstream os;
		::operator<<(os,rel<Func>(v1,v2));
		throw Casper::Exception::UndefinedFilter(os.str().c_str(),"Domain",
				Casper::Traits::getTypeStr<Rel2<Func,Expr1,Expr2> >());
	}
};

template<class Func,class Eval1,class Expr1,class Eval2,class Expr2,
		class Eval3,class Expr3>
struct PostDomFilter3
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2,const Expr3& v3)
	{
		std::ostringstream os;
		::operator<<(os,rel<Func>(v1,v2,v3));
		throw Casper::Exception::UndefinedFilter(os.str().c_str()
				,"Domain",Casper::Traits::getTypeStr<Rel3<Func,Expr1,Expr2,Expr3> >());
	}
};

template<class Func,class Eval1,class Expr1,class Eval2,class Expr2,
		class Eval3,class Expr3,class Eval4,class Expr4>
struct PostDomFilter4
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2,const Expr3& v3,const Expr4& v4)
	{
		std::ostringstream os;
		::operator<<(os,rel<Func>(v1,v2,v3,v4));
		throw Casper::Exception::UndefinedFilter(os.str().c_str(),"Domain",
				Casper::Traits::getTypeStr<Rel4<Func,Expr1,Expr2,Expr3,Expr4> >());
	}
};

template<class Func,class Eval1,class Expr1,class Eval2,class Expr2,
		class Eval3,class Expr3,class Eval4,class Expr4,
		class Eval5,class Expr5>
struct PostDomFilter5
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2,const Expr3& v3,const Expr4& v4,const Expr5& v5)
	{
		std::ostringstream os;
		::operator<<(os,rel<Func>(v1,v2,v3,v4,v5));
		throw Casper::Exception::UndefinedFilter(os.str().c_str(),"Domain",
				Casper::Traits::getTypeStr<Rel5<Func,Expr1,Expr2,Expr3,Expr4,Expr5> >());
	}
};



/**
 * TODO: see PostBndFilter for complete method list
 */
struct PostDomFilter
{
	template<class T>
	bool operator()(Store& s,const T& v) const
	{
		typedef PostDomFilterGen<T>		Post;
		return Post::post(s,v);
	}

	template<class Func,class Expr1>
	bool operator()(Store& s,const Rel1<Func,Expr1>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef PostDomFilter1<Func,Eval1,Expr1>		Post;
		return Post::post(s,v.p1);
	}
	template<class Func,class Expr1,class Expr2>
	bool operator()(Store& s,const Rel2<Func,Expr1,Expr2>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Expr2>::Type	Eval2;
		typedef PostDomFilter2<Func,Eval1,Expr1,Eval2,Expr2>	Post;
		return Post::post(s,v.p1,v.p2);
	}
	template<class Func,class Expr1,class Expr2,class Expr3>
	bool operator()(Store& s,const Rel3<Func,Expr1,Expr2,Expr3>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Expr2>::Type	Eval2;
		typedef typename Casper::Traits::GetEval<Expr3>::Type	Eval3;
		typedef PostDomFilter3<Func,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3> Post;
		return Post::post(s,v.p1,v.p2,v.p3);
	}
	template<class Func,class Expr1,class Expr2,class Expr3,class Expr4>
	bool operator()(Store& s,const Rel4<Func,Expr1,Expr2,Expr3,Expr4>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Expr2>::Type	Eval2;
		typedef typename Casper::Traits::GetEval<Expr3>::Type	Eval3;
		typedef typename Casper::Traits::GetEval<Expr4>::Type	Eval4;
		typedef PostDomFilter4<Func,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3,Eval4,Expr4> Post;
		return Post::post(s,v.p1,v.p2,v.p3,v.p4);
	}
	template<class Func,class Expr1,class Expr2,class Expr3,class Expr4,class Expr5>
	bool operator()(Store& s,const Rel5<Func,Expr1,Expr2,Expr3,Expr4,Expr5>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Expr2>::Type	Eval2;
		typedef typename Casper::Traits::GetEval<Expr3>::Type	Eval3;
		typedef typename Casper::Traits::GetEval<Expr4>::Type	Eval4;
		typedef typename Casper::Traits::GetEval<Expr5>::Type	Eval5;
		typedef PostDomFilter5<Func,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3,Eval4,Expr4,Eval5,Expr5> Post;
		return Post::post(s,v.p1,v.p2,v.p3,v.p4,v.p5);
	}
};

extern PostDomFilter postDomFilter;

/**
 * Enforces the conjunction of two constraints by creating a Dom filter
 * for each constraint.
 */
template<class Eval1,class Expr1,class Eval2,class Expr2>
struct DomFilterView2<And,Eval1,Expr1,Eval2,Expr2> : IFilter
{
	DomFilterView2(Store& s,const Expr1& p1, const Expr2& p2) :
		IFilter(s),
		store(store),
		p1(new (s) DomFilterView<Expr1>(s,p1)),
		p2(new (s) DomFilterView<Expr2>(s,p2)) {}

	bool execute()
	{	return postDomFilter(store,p1) and postDomFilter(store,p2);	}
//	Filter operator!()
//	{	return p1->operator!() or p2->operator!();	}
	void attach(INotifiable* s) { p1->attach(s); p2->attach(s); }
	void detach() { p1->detach(); p2->detach(); }

	Store&					store;
	DomFilterView<Expr1>*	p1;
	DomFilterView<Expr2>*	p2;
	INotifiable* pOwner;
};

/**
	Enforces the disjunction of two constraints. It succeeds if at least
	one constraint is satisfiable.
	\note It requires that both constraints are reifiable.
*/
template<class Eval1,class Expr1,class Eval2,class Expr2>
struct DomFilterView2<Or,Eval1,Expr1,Eval2,Expr2> : IFilter
{
	DomFilterView2(Store& s,const Expr1& p1, const Expr2& p2) :
		IFilter(s),store(store),p1(s,p1),p2(s,p2),
		f1(new (s) DomFilterView<Expr1>(s,p1)),
		f2(new (s) DomFilterView<Expr2>(s,p2)) {}

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
	void detach()
	{	p1.detach(); p2.detach(); }

	Store&				store;
	BndView<bool,Expr1>	p1;
	BndView<bool,Expr2>	p2;
	DomFilterView<Expr1>*	f1;
	DomFilterView<Expr2>*	f2;
	INotifiable* pOwner;
};

// --- Filters that fallback to BndFilter: ---

///	Enforces the GreaterEqual binary relation between two views.
template<class Eval1,class Expr1,class Eval2,class Expr2>
struct DomFilterView2<GreaterEqual,Eval1,Expr1,Eval2,Expr2> :
	BndFilterView2<GreaterEqual,Eval1,Expr1,Eval2,Expr2>
{
	DomFilterView2(Store& store, const Expr1& p1,const Expr2& p2) :
		BndFilterView2<GreaterEqual,Eval1,Expr1,Eval2,Expr2>(store,p1,p2) {}
};


///	Enforces the LessEqual binary relation between two views.
template<class Eval1,class Expr1,class Eval2,class Expr2>
struct DomFilterView2<LessEqual,Eval1,Expr1,Eval2,Expr2> :
	BndFilterView2<GreaterEqual,Eval2,Expr2,Eval1,Expr1>
{
	DomFilterView2(Store& store,const Expr1& p1, const Expr2& p2) :
		BndFilterView2<GreaterEqual,Eval2,Expr2,Eval1,Expr1>(store,p2,p1) {}
};

///	Enforces the Greater binary relation between two views.
template<class Eval1,class Expr1,class Eval2,class Expr2>
struct DomFilterView2<Greater,Eval1,Expr1,Eval2,Expr2> :
	BndFilterView2<Greater,Eval1,Expr1,Eval2,Expr2>
{
	DomFilterView2(Store& store, const Expr1& p1,const Expr2& p2) :
		BndFilterView2<Greater,Eval1,Expr1,Eval2,Expr2>(store,p1,p2) {}
};

/// Enforces bound consistenct on less constraint between two views.
template<class Eval1,class Eval2,class Expr1,class Expr2>
struct DomFilterView2<Less,Eval1,Expr1,Eval2,Expr2> :
	BndFilterView2<Greater,Eval2,Expr2,Eval1,Expr1>
{
	DomFilterView2(Store& store, const Expr1& p1, const Expr2& p2) :
		BndFilterView2<Greater,Eval2,Expr2,Eval1,Expr1>(store,p2,p1) {}
};

// dom view over element expression
template<class Expr1,class Expr2,class Eval>
struct DomView<Eval,Rel2<Element,Expr1,Expr2> > :
	DomView<Eval,Var<Eval,typename Traits::GetDom<typename Casper::Traits::GetTermElem<Expr1>::Type>::Type> >
{
	typedef typename Traits::GetDom<typename Casper::Traits::GetTermElem<Expr1>::Type>::Type Dom;
	typedef DomView<Eval,Var<Eval,Dom> >	Super;


	static Dom* getDom(Store& s,const Expr1& p1,const Expr2& p2)
	{
		if (ValExpr<int>(s,p2).ground())
			return &(*DomArrayView<Eval,Expr1>(s,p1)[ValExpr<int>(s,p2).value()]);
		else
		{
			Var<Eval,Dom> v(s,Detail::VarDomCreator<Dom>().unionOf(s,p1));
			postDomFilter(s,elementEqual(p1,p2,v));
			return v.pDomain();
		}
	}

	DomView(Store& store,const Rel2<Element,Expr1,Expr2>& p) :
		Super(store,Var<Eval,Dom>(store,getDom(store,p.p1,p.p2))) {}
};


} // CP
} // Casper

#endif /*_H_CASPER_CP_FILTER_DOM */
