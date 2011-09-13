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

#ifndef _H_CASPER_CP_FILTER_BND
#define _H_CASPER_CP_FILTER_BND

#include <casper/cp/filter/common.h>
#include <casper/cp/filter/basic.h>
#include <casper/cp/view/bnd.h>
#include <casper/cp/view/array.h>
#include <casper/kernel/reversible/reversible.h>
#include <casper/util/safeop.h>
//#include <casper/kernel/expression.h>
#include <casper/kernel/rel/rel.h>
#include <casper/util/assertions.h>

#include <casper/cp/store.h>

namespace Casper {
namespace CP {

#if 0
template<class F,class,class V1> // TODO: default is to use FilterExpr1
struct BndFilterView1
{
#ifdef CASPER_CPP0X
	static_assert(sizeof(V1)==0,"Undefined specialization of BndFilterView1");
#else
	BndFilterView1(Store& s,const V1&) { assert(0);	}
#endif
};

template<class,class,class V1,class,class V2>
struct BndFilterView2
{
#ifdef CASPER_CPP0X
	static_assert(sizeof(V1)==0,"Undefined specialization of BndFilterView2");
#else
	BndFilterView2(Store& s,const V1&,const V2&) { assert(0);}
#endif
};

template<class R,class E1,class V1,class E2,class V2,class E3,class V3>
struct BndFilterView3
{
#ifdef CASPER_CPP0X
	static_assert(sizeof(V1)==0,"Undefined specialization of BndFilterView3");
#else
	BndFilterView3(Store& s,const V1&,const V2&,const V3&) { assert(0);}
#endif
};

template<class R,class E1,class V1,class E2,class V2,class E3,class V3,class E4,class V4>
struct BndFilterView4 : NoFilter
{	BndFilterView4(Store& s,const V1&,const V2&,const V3&,const V4&) : NoFilter(s,*this) {}	};

template<class R,class E1,class V1,class E2,class V2,class E3,class V3,class E4,class V4,class E5,class V5>
struct BndFilterView5 : NoFilter
{	BndFilterView5(Store& s,const V1&,const V2&,const V3&,const V4&,const V5&) : NoFilter(s,*this) {}	};
#else

template<class Rel>
struct ChkView2BndFilter : IFilter
{
	ChkView2BndFilter(Store& s,const Rel& rel) : IFilter(s),chk(s,rel) {}
	bool execute() {	return chk.setToTrue();	}
	void attach(INotifiable* f) {	chk.attach(f);	}
	void detach(INotifiable* f) {	chk.detach(f);	}
	ChkView<Rel> chk;
};

template<class F,class,class V1>
struct BndFilterView1 : ChkView2BndFilter<Rel1<F,V1> >
{
	BndFilterView1(Store& s,const V1& v1) :
		ChkView2BndFilter<Rel1<F,V1> >(s,rel<F>(v1)) {}
};

template<class F,class,class V1,class,class V2>
struct BndFilterView2 : ChkView2BndFilter<Rel2<F,V1,V2> >
{
	BndFilterView2(Store& s,const V1& v1,const V2& v2) :
		ChkView2BndFilter<Rel2<F,V1,V2> >(s,rel<F>(v1,v2)) {}
};

template<class F,class E1,class V1,class E2,class V2,class E3,class V3>
struct BndFilterView3 : ChkView2BndFilter<Rel3<F,V1,V2,V3> >
{
	BndFilterView3(Store& s,const V1& v1,const V2& v2,const V3& v3) :
		ChkView2BndFilter<Rel3<F,V1,V2,V3> >(s,rel<F>(v1,v2,v3)) {}
};

template<class F,class E1,class V1,class E2,class V2,class E3,class V3,class E4,class V4>
struct BndFilterView4 : ChkView2BndFilter<Rel4<F,V1,V2,V3,V4> >
{
	BndFilterView4(Store& s,const V1& v1,const V2& v2,const V3& v3,const V4& v4) :
		ChkView2BndFilter<Rel4<F,V1,V2,V3,V4> >(s,rel<F>(v1,v2,v3,v4)) {}
};
/*
template<class F,class E1,class V1,class E2,class V2,class E3,class V3,class E4,class V4,class E5,class V5>
struct BndFilterView5 : ChkView2BndFilter<Rel5<F,V1,V2,V3,V4,V5> >
{
	BndFilterView5(Store& s,const V1& v1,const V2& v2,const V3& v3,
							const V4& v4,const V5& v5) :
				ChkView2BndFilter(s,rel<F>(v1,v2,v3,v4,v5)) {}
};*/


#endif

template<class Func,class Eval1,class View1>
struct PostBndFilter1
{
	static bool post(Store& s,const View1& v1)
	{
		return ChkViewRel1<Func,Eval1,View1>(s,v1).canBeTrue() and
				s.post(new (s) BndFilterView1<Func,Eval1,View1>(s,v1));
	}
};

template<class Func,class Eval1,class View1,class Eval2,class View2>
struct PostBndFilter2
{
	static bool post(Store& s,const View1& v1,const View2& v2)
	{
		return ChkViewRel2<Func,Eval1,View1,Eval2,View2>(s,v1,v2).canBeTrue() and
				s.post(new (s) BndFilterView2<Func,Eval1,View1,Eval2,View2>(s,v1,v2));
	}
};

template<class Func,class Eval1,class View1,class Eval2,class View2,
		 class Eval3,class View3>
struct PostBndFilter3
{
	static bool post(Store& s,const View1& v1,const View2& v2,const View3& v3)
	{
		return ChkViewRel3<Func,Eval1,View1,Eval2,View2,Eval3,View3>(s,v1,v2,v3).canBeTrue() and
				s.post(new (s) BndFilterView3<Func,Eval1,View1,Eval2,View2,Eval3,View3>(s,v1,v2,v3));
	}
};

template<class Func,class Eval1,class View1,class Eval2,class View2,
		 class Eval3,class View3,class Eval4,class View4>
struct PostBndFilter4
{
	static bool post(Store& s,const View1& v1,const View2& v2,const View3& v3,const View4& v4)
	{
		return ChkViewRel4<Func,Eval1,View1,Eval2,View2,Eval3,View3,Eval4,View4>(s,v1,v2,v3,v4).canBeTrue() and
				s.post(new (s) BndFilterView4<Func,Eval1,View1,Eval2,View2,Eval3,View3,Eval4,View4>(s,v1,v2,v3,v4));
	}
};

template<class Func,class Eval1,class View1,class Eval2,class View2,
		 class Eval3,class View3,class Eval4,class View4,class Eval5,class View5>
struct PostBndFilter5
{
	static bool post(Store& s,const View1& v1,const View2& v2,
						 const View3& v3,const View4& v4,const View5& v5)
	{
		return ChkViewRel5<Func,Eval1,View1,Eval2,View2,Eval3,View3,Eval4,View4,Eval5,View5>(s,v1,v2,v3,v4,v5).canBeTrue() and
				s.post(new (s)
			BndFilterView5<Func,Eval1,View1,Eval2,View2,Eval3,View3,Eval4,View4,Eval5,View5>(s,v1,v2,v3,v4,v5));
	}
};

template<class Rel>
struct BndFilterView; // default is to complain at compile time

// directional bounds arc consistency

template<class,class,class> // TODO: default is to use FilterExpr1
struct BndD1FilterView1;

template<class,class,class,class,class>
struct BndD1FilterView2; // TODO: default is to use FilterExpr2

template<class Func,class Eval1,class View1>
struct PostD1BndFilter1
{
	static bool post(Store& s,const View1& v1)
	{	return s.post(new (s) BndD1FilterView1<Func,Eval1,View1>(s,v1));}
};

template<class Func,class Eval1,class View1,class Eval2,class View2>
struct PostD1BndFilter2
{
	static bool post(Store& s,const View1& v1,const View2& v2)
	{	return s.post(new (s)
			BndD1FilterView2<Func,Eval1,View1,Eval2,View2>(s,v1,v2));}
};

template<class Rel>
struct BndD1FilterView; // default is to complain at compile time

// happens for example when the negation for a relation is not defined
template<>
struct BndFilterView<Rel0<UnknownRel> > : NoFilter
{
	BndFilterView(Store& s,const Rel0<UnknownRel>&) : NoFilter(s,*this) {}
};

template<class F,class T1>
struct BndFilterView<Rel1<F,T1> > :
	public BndFilterView1<F,typename Casper::Traits::GetEval<T1>::Type,T1>
{
	BndFilterView(Store& s,const Rel1<F,T1>& r) :
		BndFilterView1<F,typename Casper::Traits::GetEval<T1>::Type,T1>(s,r.p1) {}
};

template<class F,class T1,class T2>
struct BndFilterView<Rel2<F,T1,T2> > :
	public BndFilterView2<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 typename Casper::Traits::GetEval<T2>::Type,T2>
{
	BndFilterView(Store& s,const Rel2<F,T1,T2>& r) :
		BndFilterView2<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  typename Casper::Traits::GetEval<T2>::Type,T2>(s,r.p1,r.p2) {}
};

template<class F,class T1,class T2,class T3>
struct BndFilterView<Rel3<F,T1,T2,T3> > :
	public BndFilterView3<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 typename Casper::Traits::GetEval<T2>::Type,T2,
						 typename Casper::Traits::GetEval<T3>::Type,T3>
{
	BndFilterView(Store& s,const Rel3<F,T1,T2,T3>& r) :
		BndFilterView3<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  typename Casper::Traits::GetEval<T2>::Type,T2,
					  typename Casper::Traits::GetEval<T3>::Type,T3>(s,r.p1,r.p2,r.p3) {}
};

template<class F,class T1,class T2,class T3,class T4>
struct BndFilterView<Rel4<F,T1,T2,T3,T4> > :
	public BndFilterView4<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 	typename Casper::Traits::GetEval<T2>::Type,T2,
						 	typename Casper::Traits::GetEval<T3>::Type,T3,
						 	typename Casper::Traits::GetEval<T4>::Type,T4>
{
	BndFilterView(Store& s,const Rel4<F,T1,T2,T3,T4>& r) :
		BndFilterView4<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  	 typename Casper::Traits::GetEval<T2>::Type,T2,
					  	 typename Casper::Traits::GetEval<T3>::Type,T3,
					  	 typename Casper::Traits::GetEval<T4>::Type,T4>(s,r.p1,r.p2,r.p3,r.p4) {}
};

template<class F,class T1,class T2,class T3,class T4,class T5>
struct BndFilterView<Rel5<F,T1,T2,T3,T4,T5> > :
	public BndFilterView5<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 	typename Casper::Traits::GetEval<T2>::Type,T2,
						 	typename Casper::Traits::GetEval<T3>::Type,T3,
						 	typename Casper::Traits::GetEval<T4>::Type,T4,
						 	typename Casper::Traits::GetEval<T5>::Type,T5>
{
	BndFilterView(Store& s,const Rel5<F,T1,T2,T3,T4,T5>& r) :
		BndFilterView5<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  	 typename Casper::Traits::GetEval<T2>::Type,T2,
					  	 typename Casper::Traits::GetEval<T3>::Type,T3,
					  	 typename Casper::Traits::GetEval<T4>::Type,T4,
					  	 typename Casper::Traits::GetEval<T5>::Type,T5>(s,r.p1,r.p2,r.p3,r.p4,r.p5) {}
};

template<class,class> class Var;
template<class,class> struct DomExpr;


struct PostBndFilter
{
	bool operator()(Store& s,const bool& v) const
	{	return v and PostBndFilter()(s,BndExpr<bool>(s,v));	}

	template<class Dom>
	bool operator()(Store& s,Var<bool,Dom> v) const
	{	return PostBndFilter()(s,BndExpr<bool>(s,v));	}

	template<class Dom>
	bool operator()(Store& s,DomExpr<bool,Dom> v) const
	{	return PostBndFilter()(s,BndExpr<bool>(s,v));	}

	bool operator()(Store& s,BndExpr<bool> v) const;

//	bool operator()(Filter f) const;

	bool operator()(Store& s,const Filter& f) const
	{	return s.post(f); }

	template<class Func,class View1>
	bool operator()(Store& s,const Rel1<Func,View1>& v) const
	{
		typedef typename Casper::Traits::GetEval<View1>::Type	Eval1;
		typedef PostBndFilter1<Func,Eval1,View1>		Post;
		return Post::post(s,v.p1);
	}

	template<class Func,class View1,class View2>
	bool operator()(Store& s,const Rel2<Func,View1,View2>& v) const
	{
		typedef typename Casper::Traits::GetEval<View1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<View2>::Type	Eval2;
		typedef PostBndFilter2<Func,Eval1,View1,Eval2,View2>	Post;
		return Post::post(s,v.p1,v.p2);
	}

	template<class Func,class View1,class View2,class View3>
	bool operator()(Store& s,const Rel3<Func,View1,View2,View3>& v) const
	{
		typedef typename Casper::Traits::GetEval<View1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<View2>::Type	Eval2;
		typedef typename Casper::Traits::GetEval<View3>::Type	Eval3;
		typedef PostBndFilter3<Func,Eval1,View1,Eval2,View2,Eval3,View3>	Post;
		return Post::post(s,v.p1,v.p2,v.p3);
	}

	template<class Func,class View1,class View2,class View3,class View4>
	bool operator()(Store& s,const Rel4<Func,View1,View2,View3,View4>& v) const
	{
		typedef typename Casper::Traits::GetEval<View1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<View2>::Type	Eval2;
		typedef typename Casper::Traits::GetEval<View3>::Type	Eval3;
		typedef typename Casper::Traits::GetEval<View4>::Type	Eval4;
		typedef PostBndFilter4<Func,Eval1,View1,Eval2,View2,Eval3,View3,Eval4,View4>	Post;
		return Post::post(s,v.p1,v.p2,v.p3,v.p4);
	}

};

extern PostBndFilter postBndFilter;

struct PostD1BndFilter
{
	template<class Func,class View1>
	bool operator()(Store& s,const Rel1<Func,View1>& v) const
	{
		typedef typename Casper::Traits::GetEval<View1>::Type	Eval1;
		typedef PostD1BndFilter1<Func,Eval1,View1>	Post;
		return Post::post(s,v.p1);
	}

	template<class Func,class View1,class View2>
	bool operator()(Store& s,const Rel2<Func,View1,View2>& v) const
	{
		typedef typename Casper::Traits::GetEval<View1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<View2>::Type	Eval2;
		typedef PostD1BndFilter2<Func,Eval1,View1,Eval2,View2>	Post;
		return Post::post(s,v.p1,v.p2);
	}

};

extern PostD1BndFilter postD1BndFilter;

// FIXME: why this is not just a Post object?
/**
 * Enforces the conjunction of two constraints. It succeeds if both
 * constraints are satisfiable, fails otherwise.
 */
template<class View1,class View2>
struct BndFilterView2<And,bool,View1,bool,View2> : IFilter
{
	BndFilterView2(Store& store,const View1& p1, const View2& p2) :
		IFilter(store),store(store),p1(p1),p2(p2) {}

	bool execute()
	{	return postBndFilter(store,p1) and postBndFilter(store,p2);	}

	void attach(INotifiable*) {}
	void detach(INotifiable*) {}

	Store&	store;
	View1	p1;
	View2	p2;
};

// FIXME: same as above
/**
 * Enforces the conjunction of n constraints. It succeeds if all
 * constraints are satisfiable, fails otherwise.
 */
template<class View1>
struct BndFilterView1<And,Seq<bool>,View1> : IFilter
{
	BndFilterView1(Store& store,const View1& p1) :
		IFilter(store),store(store),p1(store,p1) {}

	bool execute()
	{
		for (uint i = 0; i < p1.size(); ++i)
			if (!postBndFilter(store,p1[i]))
				return false;
		return true;
	}

	void attach(INotifiable*) {}
	void detach(INotifiable*) {}

	Store&						store;
	BndArrayView<bool,View1>	p1;
};

/**
 * Enforces the disjunction of two constraints. It succeeds if at
 * least one of them is satisfiable, fails otherwise.
 */
template<class View1,class View2>
struct BndFilterView2<Or,bool,View1,bool,View2> : IFilter
{
	BndFilterView2(Store& s,const View1& p1, const View2& p2) :
		IFilter(s),c1(s,p1),c2(s,p2) {}

	bool execute()
	{
		if (c1.isTrue() or c2.isTrue())
			return true;
		if (!c1.canBeTrue())
			return c2.setToTrue();
		if (!c2.canBeTrue())
			return c1.setToTrue();
		return true;
	}
	void attach(INotifiable* s)
	{	c1.attach(s); c2.attach(s);}
	void detach(INotifiable* s)
	{	c1.detach(s); c2.detach(s);}

	CChkView<View1>	c1;
	CChkView<View2>	c2;
};

template<class View1, class View2>
struct PostBndFilter2<Or,bool,View1,bool,View2>
{
    static bool post(Store& s,const View1& p1,const View2& p2)
    {
    	ChkView<View1> v1(s,p1);
    	ChkView<View2> v2(s,p2);
    	if (v1.isTrue() or v2.isTrue())
    		return true;
    	if (!v1.canBeTrue())
    		return v2.canBeTrue() and postBndFilter(s,p2);
    	if (!v2.canBeTrue())
    		return v1.canBeTrue() and postBndFilter(s,p1);
    	return s.post(new (s) BndFilterView2<Or,bool,View1,bool,View2>(s,p1,p2));
    }
};

/**
 * Enforces the exclusive disjunction of two constraints. It succeeds if
 * exactly one of them is satisfiable, fails otherwise.
 */
template<class View1,class View2>
struct BndFilterView2<XOr,bool,View1,bool,View2> :
	BndFilterView2<Distinct,bool,View1,bool,View2>
{
	BndFilterView2(Store& s,const View1& p1, const View2& p2) :
		BndFilterView2<Distinct,bool,View1,bool,View2>(s,p1,p2) {}
};

///	Enforces equality
template<class Eval,class View1,class View2>
struct BndFilterView2<Equal,Eval,View1,Eval,View2> : IFilter
{
	BndFilterView2(Store& store,const View1& p1,const View2& p2)
		: IFilter(store),p1(store,p1), p2(store,p2)
		{}

	bool execute()
	{	return p1.updateMin(p2.min()) and p1.updateMax(p2.max()) and
			   p2.updateMin(p1.min()) and p2.updateMax(p1.max());}
	void attach(INotifiable* s)
	{	/*if (!p1.ground() and !p2.ground())*/ {p1.attach(s); p2.attach(s);}	}
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	BndView<Eval,View1>	p1;
	BndView<Eval,View2>	p2;
};

///	Enforces the GreaterEqual binary relation between two views.
template<class Eval,class View1,class View2>
struct BndFilterView2<GreaterEqual,Eval,View1,Eval,View2> : IFilter
{
	BndFilterView2(Store& store, const View1& p1,const View2& p2) :
		IFilter(store),p1(store,p1), p2(store,p2) {}

	bool execute()
	{	return p1.updateMin(p2.min()) && p2.updateMax(p1.max());	}
	void attach(INotifiable* s)
	{	/*if (!p1.ground() and !p2.ground())*/ { p1.attach(s); p2.attach(s);} }
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	BndView<Eval,View1> p1;
	BndView<Eval,View2> p2;
};

///	Enforces the LessEqual binary relation between two views.
template<class Eval1,class View1,class Eval2,class View2>
struct BndFilterView2<LessEqual,Eval1,View1,Eval2,View2> :
	BndFilterView2<GreaterEqual,Eval2,View2,Eval1,View1>
{
	BndFilterView2(Store& store,const View1& p1, const View2& p2) :
		BndFilterView2<GreaterEqual,Eval2,View2,Eval1,View1>(store,p2,p1) {}
};

///	Enforces the Greater binary relation between two views.
template<class Eval,class View1,class View2>
struct BndFilterView2<Greater,Eval,View1,Eval,View2> : IFilter
{
	BndFilterView2(Store& store, const View1& p1,const View2& p2) :
		IFilter(store),p1(store,p1), p2(store,p2) {}

	bool execute()
	{
		return p1.updateMin(Util::succ(p2.min())) and
			   p2.updateMax(Util::pred(p1.max()));
	}
	void attach(INotifiable* s)
	{	/*if (!p1.ground() and !p2.ground())*/ {p1.attach(s); p2.attach(s);} }
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	BndView<Eval,View1> p1;
	BndView<Eval,View2> p2;
};


/**
	Filter that enforces bound consistenct on less constraint between
	two views.
*/
template<class Eval1,class Eval2,class View1,class View2>
struct BndFilterView2<Less,Eval1,View1,Eval2,View2> :
	BndFilterView2<Greater,Eval2,View2,Eval1,View1>
{
	BndFilterView2(Store& store, const View1& p1, const View2& p2) :
		BndFilterView2<Greater,Eval2,View2,Eval1,View1>(store,p2,p1) {}
};

/**
 *	BndView over a linear(coefs,vars) relation, where coefs are ground. From
 *  "Yuanlin,Yap, Arc-consistency on n-ary monotonic and linear constraints"
 *  TODO: improve using index deltas
 *  TODO: make sure it also works for reals
 */
template<class Eval,class View1,class View2,class View3>
struct BndFilterView3<Linear,Seq<Eval>,View1,Seq<Eval>,View2,Eval,View3> : IFilter
{
	BndFilterView3(Store& store,const View1& v1, const View2& v2,
					const View3& v3) :
						IFilter(store),c(store,v1),x(store,v2),v(store,v3),
					f(store,c[0].value()*x[0].getObj())
	{
		for (uint i = 1; i < c.size(); i++)
			f = f + c[i].value()*x[i].getObj();
	}
	bool execute()
	{
		Eval fmin = f.min();
		Eval fmax = f.max();
		if (!v.updateMin(fmin) or !v.updateMax(fmax))
			return false;
		fmin = fmin-v.max();
		fmax = fmax-v.min();

		for (uint i = 0; i < c.size(); i++)
		{
			Eval lb = limits<Eval>::negInf();
			Eval ub = limits<Eval>::posInf();
			Detail::mulRange(range(c[i].value(),c[i].value()),
							 range(x[i].min(),x[i].max()),lb,ub);
			Eval lb1 = x[i].min();
			Eval ub1 = x[i].max();
			Detail::divRange(range(ub-fmax,lb-fmin),
							 range(c[i].value(),c[i].value()),lb1,ub1);
			Eval oldxmin = x[i].min();
			Eval oldxmax = x[i].max();
			if (oldxmin < lb1 or oldxmax > ub1)
			{
				if (!x[i].updateMin(lb1) or !x[i].updateMax(ub1))
					return false;
				fmin = f.min()-v.max();
				fmax = f.max()-v.min();
			}
		}
		return true;
	}
	Cost cost() const {	return linearLo; }
	void attach(INotifiable* s)
	{	x.attach(s); v.attach(s);	}
	void detach(INotifiable* s)
	{	x.detach(s); v.detach(s);	}

	ValArrayView<Eval,View1> c;
	BndArrayView<Eval,View2> x;
	BndView<Eval,View3>		 v;
	BndExpr<Eval>			 f;
};

#define GECODE_SUM
#ifdef GECODE_SUM
template<class Eval,class View1,class View2>
struct BndFilterView2<SumEquals,Seq<Eval>,View1,Eval,View2> : IFilter
{
	BndFilterView2(Store& store,const View1& v1, const View2& v2) :
		IFilter(store),store(store),x(store,v1),v(store,v2) {}

	bool execute()
	{
		if (!v.ground())
			return true;

		setInQueue(1);
		// try to rewrite:
		uint nground = 0;
		for (uint i = 0; i < x.size(); ++i)
			if (x[i].min()==x[i].max())
				++nground;
		if (x.size()-nground == 3)
		{
			VarArray<Eval> newx(store,3);
			Eval s = 0;
			uint c = 0;
			for (uint i = 0; i < x.size(); ++i)
				if (x[i].min()!=x[i].max())
					newx[c++] = x[i].getObj();
				else
					s += x[i].min();
			bool ret = postBndFilter(store,newx[0]+newx[1]+newx[2]==v.value()-s);
			setInQueue(noQueue);
			detach(pOwner);
			return ret;
		}
		else
		if (x.size()-nground == 2)
		{
			VarArray<Eval> newx(store,2);
			Eval s = 0;
			uint c = 0;
			for (uint i = 0; i < x.size(); ++i)
				if (x[i].min()!=x[i].max())
					newx[c++] = x[i].getObj();
				else
					s += x[i].min();
			bool ret = postBndFilter(store,newx[0]+newx[1]==v.value()-s);
			setInQueue(noQueue);
			detach(pOwner);
			return ret;
		}
		else
		if (x.size()-nground == 1)
		{
			VarArray<Eval> newx(store,1);
			Eval s = 0;
			uint c = 0;
			for (uint i = 0; i < x.size(); ++i)
				if (x[i].min()!=x[i].max())
					newx[c++] = x[i].getObj();
				else
					s += x[i].min();
			//solver().post(newx[0]==v.value()-s);
			setInQueue(noQueue);
			detach(pOwner);
			return newx[0].domain().updateRange(v.value()-s,v.value()-s);
		}

		Eval sl = 0;
		Eval su = 0;
		for (uint i = 0; i < x.size(); ++i)
		{
			sl -= x[i].min();
			su -= x[i].max();
		}
		sl += v.value(); su += v.value();

		const int mod_sl = 1;
		const int mod_su = 2;

		int mod = mod_sl | mod_su;

		do
		{
			if (mod & mod_sl)
			{
				mod -= mod_sl;
				// Propagate upper bound
				for (uint i = 0; i < x.size(); ++i)
				{
					const Eval xi_max = x[i].max();
					if (!x[i].updateMax(sl + x[i].min()))
					{
						setInQueue(noQueue);
						return false;
					}
					if (x[i].max()<xi_max)
					{
						su += xi_max - x[i].max();
						mod |= mod_su;
					}
				}
			}
			if (mod & mod_su)
			{
				mod -= mod_su;
				// Propagate lower bound
				for (uint i = 0; i < x.size(); ++i)
				{
					const Eval xi_min = x[i].min();
					if (!x[i].updateMin(su + x[i].max()))
					{
						setInQueue(noQueue);
						return false;
					}
					if (x[i].min() > xi_min)
					{
						sl += xi_min - x[i].min();
						mod |= mod_sl;
					}
				}
			}
		} while (mod);
		if (sl == su)
		{
			setInQueue(IFilter::noQueue);
			detach(pOwner);
			return true;
		}
		setInQueue(IFilter::noQueue);
		return true;
	}
	Cost cost() const {	return linearLo; }
	void attach(INotifiable* s)
	{
		pOwner = s;
		x.attach(s);
		v.attach(s);
	}
	void detach(INotifiable* s)
	{
		x.detach(s);
		v.detach(s);
	}

	Store&					 store;
	BndArrayView<Eval,View1> x;
	ValView<Eval,View2>		 v;
	INotifiable*			 pOwner;
};
#else

template<class Eval,class View1,class View2>
struct BndFilterView2<SumEquals,Seq<Eval>,View1,Eval,View2> : IFilter
{
	BndFilterView2(Store& store,const View1& v1, const View2& v2) :
					IFilter(store),store(store),
					x(store,v1),v(store,v2) {}

	bool execute()
	{
		// try to rewrite:
		uint nground = 0;
		for (uint i = 0; i < x.size(); ++i)
			if (x[i].min()==x[i].max())
				++nground;
		if (x.size()-nground == 3)
		{
			VarArray<Eval> newx(store,3);
			Eval s = 0;
			uint c = 0;
			for (uint i = 0; i < x.size(); ++i)
				if (x[i].min()!=x[i].max())
					newx[c++] = x[i].getObj();
				else
					s += x[i].min();
			store.post(newx[0]+newx[1]+newx[2]==v.value()-s);
			setInQueue(noQueue);
			detach(pOwner);
			return true;
		}
		else
		if (x.size()-nground == 2)
		{
			VarArray<Eval> newx(store,2);
			Eval s = 0;
			uint c = 0;
			for (uint i = 0; i < x.size(); ++i)
				if (x[i].min()!=x[i].max())
					newx[c++] = x[i].getObj();
				else
					s += x[i].min();
			store.post(newx[0]+newx[1]==v.value()-s);
			setInQueue(noQueue);
			detach(pOwner);
			return true;
		}
		else
		if (x.size()-nground == 1)
		{
			VarArray<Eval> newx(store,1);
			Eval s = 0;
			uint c = 0;
			for (uint i = 0; i < x.size(); ++i)
				if (x[i].min()!=x[i].max())
					newx[c++] = x[i].getObj();
				else
					s += x[i].min();
			//solver().post(newx[0]==v.value()-s);
			setInQueue(noQueue);
			detach(pOwner);
			return newx[0].domain().updateRange(v.value()-s,v.value()-s);
		}

		Eval sl = 0;
		Eval su = 0;
		for (uint i = 0; i < x.size(); ++i)
		{
			sl -= x[i].min();
			su -= x[i].max();
		}
		sl += v.value();
		su += v.value();

		// Propagate bounds
		for (uint i = 0; i < x.size(); ++i)
		{
			const Eval oldMax = x[i].max();
			const Eval oldMin = x[i].min();
			if (!x[i].updateRange(su + oldMax,sl + oldMin))
				return false;
			const Eval newMax = x[i].max();
			const Eval newMin = x[i].min();
			if (newMax < oldMax)
				su += oldMax - newMax;
			if (newMin > oldMin)
				sl += oldMin - newMin;
			/*
			const Eval oldDiff = oldMax - oldMin;
			if (oldDiff > sl)
				su += oldDiff - sl;
			if (oldDiff > -su)
				sl += - su - oldDiff;*/
		}

	//	if (sl == su)
	//		detach(pOwner);
	//	setInQueue(IFilter::noQueue);
		return true;
	}
	Cost cost() const {	return linearLo; }
	void attach(INotifiable* s)
	{	pOwner = s; x.attach(s); v.attach(s);	}
	void detach(INotifiable* s)
	{	x.detach(s); v.detach(s);	}

	Store&					 store;
	BndArrayView<Eval,View1> x;
	ValView<Eval,View2>		 v;
	INotifiable*			 pOwner;
};
#endif

/**
 *	BndView over a linear(vars) relation, where coefs are ground. From
 *  "Yuanlin,Yap, Arc-consistency on n-ary monotonic and linear constraints"
 *  TODO: improve using index deltas
 *  TODO: make sure it also works for reals
 */ /*
template<class Eval,class View2,class View3>
struct BndFilterView2<Linear,Seq<Eval>,View2,Eval,View3> : IFilter
{
	BndFilterView2(CPSolver& solver,const View2& v2,const View3& v3) : IFilter(solver),
					x(solver,v2),v(solver,v3),
					f(solver,x[0])
	{
		for (uint i = 1; i < c.size(); i++)
			f = f + x[i];
	}
	bool execute()
	{
		Eval fmin = f.min();
		Eval fmax = f.max();
		if (!v.updateMin(fmin) or !v.updateMax(fmax))
			return false;
		fmin = fmin-v.max();
		fmax = fmax-v.min();

		for (uint i = 0; i < x.size(); i++)
		{
			Eval lb = x[i].min();
			Eval ub = x[i].max();

			Eval lb1 = ub-fmax;
			Eval ub1 = lb-fmin;

			Eval oldxmin = x[i].min();
			Eval oldxmax = x[i].max();
			if (x[i].min() < x[i].max()-fmax or x[i].max() > x[i].min()-fmin)
			{
				if (!x[i].updateMin(lb1) or !x[i].updateMax(ub1))
					return false;
				fmin = f.min()-v.max();
				fmax = f.max()-v.min();
			}
		}
		return true;
	}
	bool entailed() const {	assert(0);	return false; }
	Cost cost() const {	return linearLo; }
	void attach(INotifiable* s)
	{	x.attach(s); v.attach(s);	}
	void detach(INotifiable* s)
	{	x.detach(s); v.detach(s);	}

	ValArrayView<Eval,View1> c;
	BndArrayView<Eval,View2> x;
	BndView<Eval,View3>		 v;
	BndExpr<Eval>			 f;
};*/

// BndFilterView over a Filter
template<>
struct BndFilterView<Filter> : IFilter
{
	BndFilterView(Store& s,Filter f) : IFilter(s),pFilter(f.getPImpl()) {}

	bool execute()
	{	return pFilter->execute();	}
	Cost cost() const { return pFilter->cost(); }

	void attach(INotifiable* s)
	{	pFilter->attach(s); }
	void detach(INotifiable* s)
	{	pFilter->detach(s); }
	IFilter*	pFilter;
};

// BndFilterView over a BndExpr<bool>
template<>
struct BndFilterView<BndExpr<bool> > : IFilter
{
	BndFilterView(Store& s,BndExpr<bool> e) : IFilter(s),rExpr(e.getImpl()) {}

	bool execute()
	{	return rExpr.updateMin(true);	}
//	Filter	operator!();

	void attach(INotifiable* s)
	{	rExpr.attach(s); }
	void detach(INotifiable* s)
	{	rExpr.detach(s); }
	IBndExpr<bool>&	rExpr;
};


#if 0
/**
	Bound expression from filter (reification)
*/
template<class View>
struct BndReify : IBndExpr<bool>
{
	private:
	struct EntailDemon : INotifiable
	{
		BndReify&	rOwner;
		IFilter&	f;
		bool		val;
		EntailDemon(BndReify* pOwner,IFilter& f,bool val) :
			rOwner(*pOwner),f(f),val(val)
		{	f.attach(this);	}
		bool notify()
		{
			if (rOwner.isTrue != 2)
				return true;
			if (f.entailed())
			{
				rOwner.set(val);
				return rOwner.pOwner->notify();
			}
			return true;
		}
	};
	public:
	BndReify(CPSolver& solver, const View& p1) :
						IBndExpr<bool>(solver),
						v1(p1),
						pos(p1),
						neg(!pos),
						isTrue(solver,2),
						pOwner(NULL) {}
	bool min() const
	{	return  isTrue==1;	}
	bool max() const
	{	return  isTrue!=0;	}
	bool updateMin(const bool& v)
	{	return !v || isTrue==1 || (isTrue==2 && postPositive());	}
	bool updateMax(const bool& v)
	{	return  v || isTrue==0 || (isTrue==2 && postNegative());	}
	void attach(INotifiable* f)
	{
		pOwner = f;
		new (solver().getHeap()) EntailDemon(this,pos.getImpl(),true);
		new (solver().getHeap()) EntailDemon(this,neg.getImpl(),false);
	}
	void detach(INotifiable* f)
	{ 	pos.detach(this); neg.detach(this);	}
	View  getObj()  const { return v1;	}

	private:
	void set(bool val)
	{ isTrue=val; /*detach(pOwner);*/ }
	bool postPositive()
	{ set(true); return solver().post(pos); }
	bool postNegative()
	{ set(false); return solver().post(neg); }

	View	v1;
	Filter	pos;
	Filter	neg;
	Reversible<int> isTrue;
	INotifiable* 	pOwner;
};
#else

/**
	\note Simplest implementation for reification
*/ /*
template<class View>
struct BndReify : IBndExpr<bool>
{
	BndReify(CPSolver& solver, const View& p1) :
						IBndExpr<bool>(solver),
						v1(p1),
						pos(p1),
						neg(!pos) {}
	bool min() const { return pos.entailed(); }
	bool max() const { return !neg.entailed(); }
	bool updateMin(const bool& v)
	{ 	return !v or solver().post(pos);	}
	bool updateMax(const bool& v)
	{ return v or solver().post(neg); }
	void attach(INotifiable* f) { pos.attach(f); neg.attach(f); 	}
	void detach(INotifiable* f) { pos.detach(f); neg.detach(f); 	}
	View  getObj()  const { return v1;	}
	View	v1;
	Filter	pos;
	Filter	neg;
};*/

/*
template<class View>
struct BndReify
{
	BndReify(CPSolver& solver, const View& p1) :
						v1(p1),
						pos(p1),
						neg(!pos),
						pOwner(NULL),
						assigned(solver,2) {}

	CPSolver& solver() const {	return assigned.solver();	}
	bool min() const
	{
		if (assigned!=2)
			return assigned;
		return pos.entailed();
	}
	bool max() const
	{
		if (assigned!=2)
			return assigned;
		return !neg.entailed();
	}
	bool updateMin(const bool& v)
	{
		if (!v or assigned==1) return true;
		if (assigned==0) return false;
		assigned = 1;
		if (pOwner != NULL) detach(pOwner);
		return solver().post(pos);
	}
	bool updateMax(const bool& v)
	{
		if (v or assigned==0) return true;
		if (assigned==1) return false;
		assigned = 0;
		if (pOwner != NULL) detach(pOwner);
		return solver().post(neg);
	}
	void range(bool& l, bool& u) const
	{
		if (assigned!=2)
			l = u = assigned;
		else
		{
			l = pos.entailed();
			u = !neg.entailed();
		}
	}
	bool updateRange(const bool& l,const bool& r)
	{	return updateMin(l) and updateMax(r);	}

	void attach(INotifiable* f)
	{	pOwner = f; pos.attach(f); neg.attach(f);}
	void detach(INotifiable* f)
	{	assert(f == pOwner); pos.detach(f); neg.detach(f);}
	View  getObj()  const { return v1;	}
	View	v1;
	Filter	pos;
	Filter	neg;
	INotifiable*	pOwner;
	Reversible<int>	assigned;
};*/
#endif

/*
template<class F,class View1,class Eval>
struct BndViewRel1 : BndReify<Rel1<F,View1> >
{
	typedef Rel1<F,View1> Rel;
	typedef BndReify<Rel> Super;
	BndViewRel1(CPSolver& solver, const View1& p1) :
		Super(solver,Rel(p1)) {}
};

template<class F, class View1,class View2,class Eval>
struct BndViewRel2 : BndReify<Rel2<F,View1,View2> >
{
	typedef Rel2<F,View1,View2> Rel;
	typedef BndReify<Rel> Super;
	BndViewRel2(CPSolver& solver, const View1& p1, const View2& p2) :
		Super(solver,Rel(p1,p2)) {}
};
*/

/**
	Bound expression from filter (reification)
*//*
template<class Eval>
struct BndView<Eval,Filter> : BndReify<Filter>
{
	typedef BndReify<Filter> Super;

	BndView(CPSolver& solver, const Filter& v) :
						Super(solver,v) {}
};*/


// factory for Element: optimize when index is ground
template<class Eval,class ArrayView,class IdxView,class EvalView>
struct PostBndFilter3<ElementEqual,Seq<Eval>,ArrayView,int,IdxView,Eval,EvalView>
{
    static bool post(Store& s,const ArrayView& p1,const IdxView& p2,const EvalView& p3)
    {
    	if (ValView<int,IdxView>(s,p2).ground())
    	{
    		DomArrayView<Eval,ArrayView> b(s,p1);
    		Var<Eval> aux(s,&(*b[ValView<int,IdxView>(s,p2).value()]));
    		return postBndFilter(s,aux==p3);
    	}
    	else
    		return s.post(new (s)
    						BndFilterView3<ElementEqual,Seq<Eval>,ArrayView,
													int,IdxView,
													Eval,EvalView>(s,p1,p2,p3));
    }

};

#if 0
template<bool Cond,class IfTrue,class IfFalse>
struct ITE
{	typedef IfTrue Eval;	};

template<class IfTrue,class IfFalse>
struct ITE<false,IfTrue,IfFalse>
{	typedef IfFalse Eval;	};

template<class View1,class View2,class Eval>
struct ElementOverGroundIdx : BndView<Eval,typename Casper::Traits::GetElem<View1>::Type>
{
	typedef	BndView<Eval,typename Casper::Traits::GetElem<View1>::Type>	Super;

	static Super getSuper(CPSolver& s,const View1& p1,const View2& p2)
	{
		if (ValExpr<int>(s,p2).ground())
		{
			uint val = ValExpr<int>(s,p2).value();
			typedef typename Casper::Traits::GetElem<View1>::Type	SElem;
			IterationView<SElem,View1>	it(p1);
			for (;  it.valid() and val > 0; it.iterate())
				--val;
			return Super(s,it.value());
		}
	}
	typedef Rel2<Element,View1,View2>	Viewed;
	Viewed getObj()  const {	return viewed;	}

	ElementOverGroundIdx(CPSolver& solver,const View1& p1, const View2& p2) :
		Super(getSuper(solver,p1,p2)),
		viewed(p1,p2){}

	Viewed viewed;
};
#endif

// if defined means that no auxiliary variable will be used for element
//#define CASPER_CP_ELEMENT_BNDVIEW

#ifndef CASPER_CP_ELEMENT_BNDVIEW
// element as expression: may introduce one auxiliary variable
template<class View1,class View2,class Eval>
struct BndViewRel2<Element,View1,View2,Eval> :
	BndView<Eval,Var<Eval,typename Traits::GetDefaultDom<Eval>::Type> >
{
	CASPER_ASSERT_BNDVIEW_EVAL(Eval)

	template<class E,class V> struct Helper	{
		V operator()(Store& s, const E& e)
		{ return V(s,e);}
	};
	template<class V> struct Helper<V,V>	{
		const V& operator()(Store& s,const V& e)
		{ return e;}
	};

	typedef Var<Eval,typename Traits::GetDefaultDom<Eval>::Type> Elem;
	typedef BndView<Eval,Elem>	Super;
	static Super getSuper(Store& s,const View1& p1,const View2& p2)
	{
		if (ValView<int,View2>(s,p2).ground())
		{
			int val = ValView<int,View2>(s,p2).value();

			typedef typename Casper::Traits::GetElem<View1>::Type EElem;
			Util::IterationView<View1>	it(p1);
			for (;  it.valid() and val > 0; it.iterate())
				--val;

			if (!it.valid() or val<0)
				throw Exception::IndexOutOfBounds();

			return Super(s,Helper<EElem,Elem>()(s,it.value()));
		}

		Elem v(s,Detail::VarDomCreator<typename Elem::Dom>().unionOf(s,p1));
		s.post(elementEqual(p1,p2,v));
		return Super(s,v);
	}

	Rel2<Element,View1,View2> getObj() const
	{	return Rel2<Element,View1,View2>(v1,idx.getObj()); }

	BndViewRel2(Store& store,const View1& p1, const View2& p2) :
		Super(getSuper(store,p1,p2)),
		store(store),
		v1(p1),idx(store,p2)
	{}

	Store& store;
	View1 v1;
	ValView<int,View2> idx;
};
#else

// FIXME: Test: not as strong as version above. Why?
// does not introduce auxiliary variables but might be slower
template<class View1,class View2,class Eval>
struct BndViewRel2<Element,View1,View2,Eval>
{
	BndViewRel2(Store& store, const View1& v1, const View2& v2) :
		a(store,v1),i(store,v2) {}

	Eval min() const
	{
		if (i.min()<0 or i.max() >= static_cast<int>(a.size()))
			return Casper::limits<Eval>::negInf();
		Eval r = a[i.min()].min();
		for (int j = i.min()+1; j <= i.max(); ++j)
			if (a[j].min() < r)
				r = a[j].min();
		return r;
	}

	Eval max() const
	{
		if (i.min()<0 or i.max() >= static_cast<int>(a.size()))
			return Casper::limits<Eval>::posInf();
		Eval r = a[i.min()].max();
		for (int j = i.min()+1; j <= i.max(); ++j)
			if (a[j].max() > r)
				r = a[j].max();
		return r;
	}

	void range(Eval& lb, Eval& ub) const
	{
		if (i.min()<0 or i.max() >= static_cast<int>(a.size()))
		{
			lb = Casper::limits<Eval>::negInf();
			ub = Casper::limits<Eval>::posInf();
			return;
		}
		lb = a[i.min()].min();
		ub = a[i.min()].max();
		for (int j = i.min()+1; j <= i.max(); ++j)
		{
			if (a[j].min() < lb)
				lb = a[j].min();
			if (a[j].max() > ub)
				ub = a[j].max();
		}
	}

	bool updateMin(const Eval& m)
	{
		if (i.min()<0 or i.max() >= static_cast<int>(a.size()))
			return true;

		// try to increase index lb
		int mi = i.min();
		for (; mi <= i.max(); ++mi)
			if (a[mi].max() >= m)
				break;
		// try to decrease index ub
		int ma = i.max();
		for (; ma >= i.min(); --ma)
			if (a[ma].max() >= m)
				break;
		if (mi>ma)
			return false;
		bool r = true;
		if (mi > i.min())
			r = r and i.updateMin(mi);
		if (ma < i.max())
			r = r and i.updateMax(ma);
		if (mi==ma)
			r = r and a[mi].updateMin(m);
		return r;
	}

	bool updateMax(const Eval& m)
	{
		if (i.min()<0 or i.max() >= static_cast<int>(a.size()))
			return true;

		// try to increase index lb
		int mi = i.min();
		for (; mi <= i.max(); ++mi)
			if (a[mi].min() <= m)
				break;
		// try to decrease index ub
		int ma = i.max();
		for (; ma >= i.min(); --ma)
			if (a[ma].min() <= m)
				break;
		if (mi>ma)
			return false;
		bool r = true;
		if (mi > i.min())
			r = r and i.updateMin(mi);
		if (ma < i.max())
			r = r and i.updateMax(ma);
		if (mi==ma)
			r = r and a[mi].updateMax(m);
		return r;
	}

	bool updateRange(const Eval& lb, const Eval& ub)
	{
		if (i.min()<0 or i.max() >= static_cast<int>(a.size()))
			return true;

		// try to increase index lb
		int mi = i.min();
		for (; mi <= i.max(); ++mi)
			if (a[mi].max() >= lb and a[mi].min() <= ub)
				break;
		// try to decrease index ub
		int ma = i.max();
		for (; ma >= i.min(); --ma)
			if (a[ma].max() >= lb and a[ma].min() <= ub)
				break;
		if (mi>ma)
			return false;
		bool r = true;
		if (mi > i.min())
			r = r and i.updateMin(mi);
		if (ma < i.max())
			r = r and i.updateMax(ma);
		if (mi==ma)
			r = r and a[mi].updateRange(lb,ub);
		return r;
	}

	void attach(INotifiable* f) {	a.attach(f); i.attach(f); }
	void detach(INotifiable* f) {	a.detach(f); i.detach(f); }

	Rel2<Element,View1,View2> getObj()  const
	{	return rel<Element>(a.getObj(),i.getObj());	}

	BndArrayView<Eval,View1>	a;
	BndView<int,View2>	i;
};

#endif

/*
template<class T1,class T2>
struct ComposedFilter : IFilter
{
	ComposedFilter(CPSolver& solver, const T1& pos, const T2& neg) :
		IFilter(solver),pos(solver,pos),neg(neg) {}
	bool execute()
	{	return pos.execute();	}
	bool entailed() const
	{	return pos.entailed();	}
	Filter operator!() {	return Filter(neg);	}
	void attach(INotifiable* pOwner)	{	pos.attach(pOwner);	}
	void detach(INotifiable* pOwner)	{ 	pos.detach(pOwner);	}

	BndFilterView<T1> pos;
	T2 neg;
};

template<class T1,class T2>
Filter compose(const T1& t1, const T2& t2)
{	return new (getState(t1).getHeap()) ComposedFilter<T1,T2>(getState(t1),t1,t2);}
*/

#if 1
struct FastCache;

template<class Eval,class View>
struct BndViewRel1<FastCache,View,Eval>
{
	BndViewRel1(Store& store,const View& p) : v(store,p),
		cachedMin(store,limits<Eval>::min()),
		cachedMax(store,limits<Eval>::max())
		{}

	BndViewRel1(Store& store,const View& p,const Eval& mi, const Eval& ma) : v(store,p),
		cachedMin(store,mi),cachedMax(store,ma)
		{}

	Eval min() const
	{
		//return cachedMin;
		return std::max((Eval)cachedMin,v.min());
	}
	Eval max() const
	{
		//return cachedMax;
		return std::min((Eval)cachedMax,v.max());
	}
	void range(Eval& min, Eval& max) const
	{
		//min = cachedMin;
		//max = cachedMax;
		min = std::max((Eval)cachedMin,v.min());
		max = std::min((Eval)cachedMax,v.max());
	}

	bool updateMin(const Eval& m)
	{
		if (m>cachedMin)
		{
			cachedMin = m;
			return m <= cachedMax and v.updateMin(cachedMin);
		}
		return true;
	}
	bool updateMax(const Eval& m)
	{
		if (m<cachedMax)
		{
			cachedMax = m;
			return m >= cachedMin and v.updateMax(cachedMax);
		}
		return true;
	}

	bool updateRange(const Eval& lb, const Eval& ub)
	{
		bool ch = false;
		if (lb>cachedMin)
		{
			cachedMin = lb;
			ch = true;
		}
		if (ub<cachedMax)
		{
			cachedMax = ub;
			ch = true;
		}
		return cachedMin<=cachedMax and
				(!ch or v.updateRange(cachedMin,cachedMax));
	}

	void attach(INotifiable* f)
	{
		v.attach(f);
		cachedMin = std::max((Eval)cachedMin,v.min());
		cachedMax = std::min((Eval)cachedMax,v.max());
	}
	void detach(INotifiable* f) {	v.detach(f);	}

	View getObj()  const
	{ 	return v.getObj();	}

	BndView<Eval,View>			v;
	Reversible<Eval>			cachedMin;
	Reversible<Eval>			cachedMax;
};
#endif

template<class Eval,class View>
struct BndViewRel1<Cache,View,Eval>
{
	BndViewRel1(Store& store,const View& p) : v(store,p),
		cachedMin(store,limits<Eval>::min()),
		cachedMax(store,limits<Eval>::max()),
		pSyncRange(new SyncRange(store,this))
		{}

	BndViewRel1(Store& store,const View& p,const Eval& mi, const Eval& ma) : v(store,p),
		cachedMin(store,mi),
		cachedMax(store,ma),
		pSyncRange(new SyncRange(store,this))
		{}

	struct SyncRange : IFilter
	{
		SyncRange(Store& store,BndViewRel1* pOwner) : IFilter(store),store(store), rOwner(*pOwner) {}
		bool execute()
		{	return rOwner.v.updateRange(rOwner.cachedMin,rOwner.cachedMax) and rOwner.pParent->notify();	}
		Cost cost() const { return constantLo;}
		void attach(INotifiable* f) { rOwner.v.attach(f); }
		void detach(INotifiable* f) { rOwner.v.detach(f); }
		Store&	store;
		BndViewRel1& rOwner;
	};

	Eval min() const
	{
		//return cachedMin;
		return std::max((Eval)cachedMin,v.min());
	}
	Eval max() const
	{
		//return cachedMax;
		return std::min((Eval)cachedMax,v.max());
	}
	void range(Eval& min, Eval& max) const
	{
		//min = cachedMin;
		//max = cachedMax;
		min = std::max((Eval)cachedMin,v.min());
		max = std::min((Eval)cachedMax,v.max());
	}

	bool updateMin(const Eval& m)
	{
		if (m>cachedMin)
		{
			cachedMin = m;
			return m <= cachedMax and v.updateMin(cachedMin) and pParent->notify();
		}
		return true;
	}
	bool updateMax(const Eval& m)
	{
		if (m<cachedMax)
		{
			cachedMax = m;
			return m >= cachedMin and v.updateMax(cachedMax) and pParent->notify();
		}
		return true;
	}

	bool updateRange(const Eval& lb, const Eval& ub)
	{
		bool ch = false;
		if (lb>cachedMin)
		{
			cachedMin = lb;
			ch = true;
		}
		if (ub<cachedMax)
		{
			cachedMax = ub;
			ch = true;
		}
		return cachedMin<=cachedMax and
				(!ch or
				 (v.updateRange(cachedMin,cachedMax) and pParent->notify()));
	}

	void attach(INotifiable* f)
	{
		pParent = f;
		pSyncRange->store.post(pSyncRange);
		cachedMin = std::max((Eval)cachedMin,v.min());
		cachedMax = std::min((Eval)cachedMax,v.max());
	}
	void detach(INotifiable* f) {	v.detach(f);	}

	View getObj()  const
	{ 	return v.getObj();	}

	BndView<Eval,View>			v;
	Reversible<Eval>			cachedMin;
	Reversible<Eval>			cachedMax;
	SyncRange*					pSyncRange;
	INotifiable*				pParent;
};

template<class Eval,class View>
struct BndViewRel3<Cache,View,Eval,Eval,Eval> :
			BndViewRel1<Cache,View,Eval>
{
	typedef BndViewRel1<Cache,View,Eval> Super;
	BndViewRel3(Store& store,const View& p,const Eval& mi, const Eval& ma) :
			Super(store,p,mi,ma) {}
};

} // CP
} // Casper

#endif /*_H_CASPER_KERNEL_BNDFILTER */
