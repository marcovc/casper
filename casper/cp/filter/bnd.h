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
#include <casper/kernel/view/element.h>
#include <casper/util/assertions.h>

#include <casper/cp/store.h>

namespace Casper {

template<class> struct Expr;

namespace CP {


template<class Rel>
struct ChkView2BndFilter : IFilter
{
	ChkView2BndFilter(Store& s,const Rel& rel) : IFilter(s),chk(s,rel) {}
	bool execute() {	return chk.setToTrue();	}
	void attach(INotifiable* f) {	chk.attach(f);	}
	void detach() {	chk.detach();	}
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

template<class F,class E1,class V1,class E2,class V2,class E3,class V3,class E4,class V4,class E5,class V5>
struct BndFilterView5 : ChkView2BndFilter<Rel5<F,V1,V2,V3,V4,V5> >
{
	BndFilterView5(Store& s,const V1& v1,const V2& v2,const V3& v3,
							const V4& v4,const V5& v5) :
		ChkView2BndFilter<Rel5<F,V1,V2,V3,V4,V5> >(s,rel<F>(v1,v2,v3,v4,v5)) {}
};


template<class Func,class Eval1,class Expr1>
struct PostBndFilter1
{
	static bool post(Store& s,const Expr1& v1)
	{
    	typedef Casper::Detail::DeRefAndSimplify<Expr1> SDExpr1;
    	const typename SDExpr1::Type sdv1 = SDExpr1()(v1);

    	//std::cout << "posting " << Casper::Traits::getTypeStr<Rel1<Func,typename SDExpr1::Type> >() << std::endl;

		return ChkViewRel1<Func,Eval1,typename SDExpr1::Type>(s,sdv1).canBeTrue() and
				s.post(new (s) BndFilterView1<Func,Eval1,typename SDExpr1::Type>(s,sdv1));
	}
};

template<class Func,class Eval1,class Expr1,class Eval2,class Expr2>
struct PostBndFilter2
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2)
	{
    	typedef Casper::Detail::DeRefAndSimplify<Expr1> SDExpr1;
    	typedef Casper::Detail::DeRefAndSimplify<Expr2> SDExpr2;
    	const typename SDExpr1::Type sdv1 = SDExpr1()(v1);
    	const typename SDExpr2::Type sdv2 = SDExpr2()(v2);

    	return ChkViewRel2<Func,Eval1,typename SDExpr1::Type,
    							Eval2,typename SDExpr2::Type>(s,sdv1,sdv2).canBeTrue() and
				//(not ChkViewRel2<Func,Eval1,Expr1,Eval2,Expr2>(s,v1,v2).isTrue())  and  // FIXME
				s.post(new (s) BndFilterView2<Func,Eval1,typename SDExpr1::Type,
												   Eval2,typename SDExpr2::Type>(s,sdv1,sdv2));
	}
};

template<class Func,class Eval1,class Expr1,class Eval2,class Expr2,
		 class Eval3,class Expr3>
struct PostBndFilter3
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2,const Expr3& v3)
	{
    	typedef Casper::Detail::DeRefAndSimplify<Expr1> SDExpr1;
    	typedef Casper::Detail::DeRefAndSimplify<Expr2> SDExpr2;
    	typedef Casper::Detail::DeRefAndSimplify<Expr3> SDExpr3;
    	const typename SDExpr1::Type sdv1 = SDExpr1()(v1);
    	const typename SDExpr2::Type sdv2 = SDExpr2()(v2);
    	const typename SDExpr3::Type sdv3 = SDExpr3()(v3);

    	return ChkViewRel3<Func,Eval1,typename SDExpr1::Type,
    							Eval2,typename SDExpr2::Type,
    							Eval3,typename SDExpr3::Type>(s,sdv1,sdv2,sdv3).canBeTrue() and
				s.post(new (s) BndFilterView3<Func,Eval1,typename SDExpr1::Type,
												   Eval2,typename SDExpr2::Type,
												   Eval3,typename SDExpr3::Type>(s,sdv1,sdv2,sdv3));
	}
};

template<class Func,class Eval1,class Expr1,class Eval2,class Expr2,
		 class Eval3,class Expr3,class Eval4,class Expr4>
struct PostBndFilter4
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2,const Expr3& v3,const Expr4& v4)
	{
    	typedef Casper::Detail::DeRefAndSimplify<Expr1> SDExpr1;
    	typedef Casper::Detail::DeRefAndSimplify<Expr2> SDExpr2;
    	typedef Casper::Detail::DeRefAndSimplify<Expr3> SDExpr3;
    	typedef Casper::Detail::DeRefAndSimplify<Expr4> SDExpr4;
    	const typename SDExpr1::Type sdv1 = SDExpr1()(v1);
    	const typename SDExpr2::Type sdv2 = SDExpr2()(v2);
    	const typename SDExpr3::Type sdv3 = SDExpr3()(v3);
    	const typename SDExpr4::Type sdv4 = SDExpr4()(v4);

    	return ChkViewRel4<Func,Eval1,typename SDExpr1::Type,
    							Eval2,typename SDExpr2::Type,
    							Eval3,typename SDExpr3::Type,
    							Eval4,typename SDExpr4::Type>(s,sdv1,sdv2,sdv3,sdv4).canBeTrue() and
				s.post(new (s) BndFilterView4<Func,Eval1,typename SDExpr1::Type,
												   Eval2,typename SDExpr2::Type,
												   Eval3,typename SDExpr3::Type,
												   Eval4,typename SDExpr4::Type>(s,sdv1,sdv2,sdv3,sdv4));
	}
};

template<class Func,class Eval1,class Expr1,class Eval2,class Expr2,
		 class Eval3,class Expr3,class Eval4,class Expr4,class Eval5,class Expr5>
struct PostBndFilter5
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2,
						 const Expr3& v3,const Expr4& v4,const Expr5& v5)
	{
    	typedef Casper::Detail::DeRefAndSimplify<Expr1> SDExpr1;
    	typedef Casper::Detail::DeRefAndSimplify<Expr2> SDExpr2;
    	typedef Casper::Detail::DeRefAndSimplify<Expr3> SDExpr3;
    	typedef Casper::Detail::DeRefAndSimplify<Expr4> SDExpr4;
    	typedef Casper::Detail::DeRefAndSimplify<Expr5> SDExpr5;
    	const typename SDExpr1::Type sdv1 = SDExpr1()(v1);
    	const typename SDExpr2::Type sdv2 = SDExpr2()(v2);
    	const typename SDExpr3::Type sdv3 = SDExpr3()(v3);
    	const typename SDExpr4::Type sdv4 = SDExpr4()(v4);
    	const typename SDExpr5::Type sdv5 = SDExpr4()(v5);
    	return ChkViewRel5<Func,Eval1,typename SDExpr1::Type,
    							Eval2,typename SDExpr2::Type,
    							Eval3,typename SDExpr3::Type,
    							Eval4,typename SDExpr4::Type,
    							Eval5,typename SDExpr5::Type>(s,sdv1,sdv2,sdv3,sdv4,sdv5).canBeTrue() and
				s.post(new (s)
			BndFilterView5<Func,Eval1,typename SDExpr1::Type,
								Eval2,typename SDExpr2::Type,
								Eval3,typename SDExpr3::Type,
								Eval4,typename SDExpr4::Type,
								Eval5,typename SDExpr5::Type>(s,sdv1,sdv2,sdv3,sdv4,sdv5));
	}
};


template<class,class> class Var;
template<class,class> struct DomExpr;

struct PostBndFilter
{
	bool operator()(Store& s,const bool& v) const
	{	return v and PostBndFilter()(s,BndExpr<bool>(s,v));	}

	template<class Dom>
	bool operator()(Store& s,const Var<bool,Dom>& v) const
	{	return PostBndFilter()(s,BndExpr<bool>(s,v));	}

	template<class Dom>
	bool operator()(Store& s,const DomExpr<bool,Dom>& v) const
	{	return PostBndFilter()(s,BndExpr<bool>(s,v));	}

	bool operator()(Store& s,const BndExpr<bool>& v) const;

	bool operator()(Store& s,const Casper::Expr<bool>& v) const;

//	bool operator()(Filter f) const;

	bool operator()(Store& s,const Filter& f) const
	{	return s.post(f); }

	template<class Func,class Expr1>
	bool operator()(Store& s,const Rel1<Func,Expr1>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef PostBndFilter1<Func,Eval1,Expr1>		Post;
		return Post::post(s,v.p1);
	}

	template<class Func,class Expr1,class Expr2>
	bool operator()(Store& s,const Rel2<Func,Expr1,Expr2>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Expr2>::Type	Eval2;
		typedef PostBndFilter2<Func,Eval1,Expr1,Eval2,Expr2>	Post;
		return Post::post(s,v.p1,v.p2);
	}

	template<class Func,class Expr1,class Expr2,class Expr3>
	bool operator()(Store& s,const Rel3<Func,Expr1,Expr2,Expr3>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Expr2>::Type	Eval2;
		typedef typename Casper::Traits::GetEval<Expr3>::Type	Eval3;
		typedef PostBndFilter3<Func,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3>	Post;
		return Post::post(s,v.p1,v.p2,v.p3);
	}

	template<class Func,class Expr1,class Expr2,class Expr3,class Expr4>
	bool operator()(Store& s,const Rel4<Func,Expr1,Expr2,Expr3,Expr4>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Expr2>::Type	Eval2;
		typedef typename Casper::Traits::GetEval<Expr3>::Type	Eval3;
		typedef typename Casper::Traits::GetEval<Expr4>::Type	Eval4;
		typedef PostBndFilter4<Func,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3,Eval4,Expr4>	Post;
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
		typedef PostBndFilter5<Func,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3,Eval4,Expr4,Eval5,Expr5>	Post;
		return Post::post(s,v.p1,v.p2,v.p3,v.p4,v.p5);
	}
};


extern PostBndFilter postBndFilter;

template<class Rel>
struct BndFilterView; // default is to complain at compile time

// directional bounds arc consistency

template<class,class,class> // TODO: default is to use FilterExpr1
struct BndD1FilterView1;

template<class,class,class,class,class>
struct BndD1FilterView2; // TODO: default is to use FilterExpr2

template<class Func,class Eval1,class Expr1>
struct PostD1BndFilter1
{
	static bool post(Store& s,const Expr1& v1)
	{	return s.post(new (s) BndD1FilterView1<Func,Eval1,Expr1>(s,v1));}
};

template<class Func,class Eval1,class Expr1,class Eval2,class Expr2>
struct PostD1BndFilter2
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2)
	{	return s.post(new (s)
			BndD1FilterView2<Func,Eval1,Expr1,Eval2,Expr2>(s,v1,v2));}
};

template<class Rel>
struct BndD1FilterView; // default is to complain at compile time

struct PostD1BndFilter
{
	template<class Func,class Expr1>
	bool operator()(Store& s,const Rel1<Func,Expr1>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef PostD1BndFilter1<Func,Eval1,Expr1>	Post;
		return Post::post(s,v.p1);
	}

	template<class Func,class Expr1,class Expr2>
	bool operator()(Store& s,const Rel2<Func,Expr1,Expr2>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Expr2>::Type	Eval2;
		typedef PostD1BndFilter2<Func,Eval1,Expr1,Eval2,Expr2>	Post;
		return Post::post(s,v.p1,v.p2);
	}

};

extern PostD1BndFilter postD1BndFilter;
 
// happens for example when the negation for a relation is not defined
template<>
struct BndFilterView<Rel0<UnknownRel> > : UndefinedFilter
{
	BndFilterView(Store& s,const Rel0<UnknownRel>&) : UndefinedFilter(s,*this) {}
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

// FIXME: why this is not just a Post object?
/**
 * Enforces the conjunction of two Boolean expressions. It succeeds if both
 * expressions can be made true, fails otherwise.
 * \ingroup Filtering
 */
template<class Expr1,class Expr2>
struct BndFilterView2<And,bool,Expr1,bool,Expr2> : IFilter
{
	BndFilterView2(Store& store,const Expr1& p1, const Expr2& p2) :
		IFilter(store),store(store),p1(p1),p2(p2) {}

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "BndFilterView2<And,bool,Expr1,bool,Expr2>", Util::Logger::filterExecuteBegin);
		#endif
		return postBndFilter(store,p1) and postBndFilter(store,p2);	}

	void attach(INotifiable*) {}
	void detach() {}

	Store&	store;
	Expr1	p1;
	Expr2	p2;
};

// FIXME: same as above
/**
 * Enforces the conjunction of n Boolean expressions. It succeeds if all
 * expressions can be made true, fails otherwise.
 * \ingroup Filtering
 */
template<class Expr1>
struct BndFilterView1<And,Seq<bool>,Expr1> : IFilter
{
	BndFilterView1(Store& store,const Expr1& p1) :
		IFilter(store),store(store),p1(store,p1) {}

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "BndFilterView1<And,Seq<bool>,Expr1>", Util::Logger::filterExecuteBegin);
		#endif

		for (uint i = 0; i < p1.size(); ++i)
			if (!postBndFilter(store,p1[i]))
				return false;
		return true;
	}

	void attach(INotifiable*) {}
	void detach() {}

	Store&						store;
	BndArrayView<bool,Expr1>	p1;
};

/**
 * Enforces the disjunction of two Boolean expressions. It succeeds if at
 * least one of them can be made true, fails otherwise.
 * \ingroup Filtering
 */
template<class Expr1,class Expr2>
struct BndFilterView2<Or,bool,Expr1,bool,Expr2> : IFilter
{
	BndFilterView2(Store& s,const Expr1& p1, const Expr2& p2) :
		IFilter(s),c1(s,p1),c2(s,p2)
		#ifdef CASPER_LOG
		,store(s)
		#endif
		{}

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "BndFilterView2<Or,bool,Expr1,bool,Expr2>", Util::Logger::filterExecuteBegin);
		#endif

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
	void detach()
	{	c1.detach(); c2.detach();}

	CChkView<Expr1>	c1;
	CChkView<Expr2>	c2;
	#ifdef CASPER_LOG
	Store& store;
	#endif
};

template<class Expr1, class Expr2>
struct PostBndFilter2<Or,bool,Expr1,bool,Expr2>
{
    static bool post(Store& s,const Expr1& p1,const Expr2& p2)
    {
    	typedef Casper::Detail::DeRefAndSimplify<Expr1> SDExpr1;
    	typedef Casper::Detail::DeRefAndSimplify<Expr2> SDExpr2;
    	const typename SDExpr1::Type sdv1 = SDExpr1()(p1);
    	const typename SDExpr2::Type sdv2 = SDExpr2()(p2);

    	ChkView<typename SDExpr1::Type> v1(s,sdv1);
    	ChkView<typename SDExpr2::Type> v2(s,sdv2);
    	if (v1.isTrue() or v2.isTrue())
    		return true;
    	if (!v1.canBeTrue())
    		return v2.canBeTrue() and postBndFilter(s,sdv2);
    	if (!v2.canBeTrue())
    		return v1.canBeTrue() and postBndFilter(s,sdv1);
    	return s.post(new (s) BndFilterView2<Or,bool,typename SDExpr1::Type,
    											bool,typename SDExpr2::Type>(s,sdv1,sdv2));
    }
};

/**
 * Enforces the exclusive disjunction of two Boolean expressions. It succeeds if
 * exactly one of them can be made true, fails otherwise.
 * \ingroup Filtering
 */
template<class Expr1,class Expr2>
struct BndFilterView2<XOr,bool,Expr1,bool,Expr2> :
	BndFilterView2<Distinct,bool,Expr1,bool,Expr2>
{
	BndFilterView2(Store& s,const Expr1& p1, const Expr2& p2) :
		BndFilterView2<Distinct,bool,Expr1,bool,Expr2>(s,p1,p2) {}
};

/**
 * 	Enforces equality between two expressions evaluating to a generic type Eval.
 * 	\ingroup Filtering
 */
template<class Eval,class Expr1,class Expr2>
struct BndFilterView2<Equal,Eval,Expr1,Eval,Expr2> : IFilter
{
	BndFilterView2(Store& store,const Expr1& p1,const Expr2& p2)
		: IFilter(store),p1(store,p1), p2(store,p2)
		#ifdef CASPER_LOG
		,store(store)
		#endif
		{}

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "BndFilterView2<Equal,Eval,Expr1,Eval,Expr2>", Util::Logger::filterExecuteBegin);
		#endif
		p1.min();
		p2.min();
		return p1.updateMin(p2.min()) and p1.updateMax(p2.max()) and
			   p2.updateMin(p1.min()) and p2.updateMax(p1.max());
	}
	void attach(INotifiable* s)
	{	/*if (!p1.ground() and !p2.ground())*/ {p1.attach(s); p2.attach(s);}	}
	void detach()
	{	p1.detach(); p2.detach();	}

	BndView<Eval,Expr1>	p1;
	BndView<Eval,Expr2>	p2;
	#ifdef CASPER_LOG
	Store& store;
	#endif
};

/**
 * 	Enforces inequality between two expressions evaluating to a generic type Eval.
 * 	\ingroup Filtering
 */
template<class Eval,class Expr1,class Expr2>
struct BndFilterView2<GreaterEqual,Eval,Expr1,Eval,Expr2> : IFilter
{
	BndFilterView2(Store& store, const Expr1& p1,const Expr2& p2) :
		IFilter(store),p1(store,p1), p2(store,p2)
		#ifdef CASPER_LOG
		,store(store)
		#endif
		{}

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "BndFilterView2<GreaterEqual,Eval,Expr1,Eval,Expr2>", Util::Logger::filterExecuteBegin);
		#endif

		if (p1.min()>=p2.max())
		{
			detach();
			return true;
		}
		else
			return p1.updateMin(p2.min()) and p2.updateMax(p1.max());
	}
	void attach(INotifiable* s)
	{	/*if (!p1.ground() and !p2.ground())*/ { p1.attach(s); p2.attach(s);} }
	void detach()
	{	p1.detach(); p2.detach();	}

	BndView<Eval,Expr1> p1;
	BndView<Eval,Expr2> p2;
	#ifdef CASPER_LOG
	Store& store;
	#endif
};

/**
 * 	Enforces inequality between two expressions evaluating to a generic type Eval.
 * 	\ingroup Filtering
 */
template<class Eval1,class Expr1,class Eval2,class Expr2>
struct BndFilterView2<LessEqual,Eval1,Expr1,Eval2,Expr2> :
	BndFilterView2<GreaterEqual,Eval2,Expr2,Eval1,Expr1>
{
	BndFilterView2(Store& store,const Expr1& p1, const Expr2& p2) :
		BndFilterView2<GreaterEqual,Eval2,Expr2,Eval1,Expr1>(store,p2,p1) {}
};

/**
 * 	Enforces inequality between two expressions evaluating to a generic type Eval.
 * 	\ingroup Filtering
 */
template<class Eval,class Expr1,class Expr2>
struct BndFilterView2<Greater,Eval,Expr1,Eval,Expr2> : IFilter
{
	BndFilterView2(Store& store, const Expr1& p1,const Expr2& p2) :
		IFilter(store),p1(store,p1), p2(store,p2)
		#ifdef CASPER_LOG
		,store(store)
		#endif
		{}

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "BndFilterView2<Greater,Eval,Expr1,Eval,Expr2>", Util::Logger::filterExecuteBegin);
		#endif

		return p1.updateMin(Util::succ(p2.min())) and
			   p2.updateMax(Util::pred(p1.max()));
	}
	void attach(INotifiable* s)
	{	/*if (!p1.ground() and !p2.ground())*/ {p1.attach(s); p2.attach(s);} }
	void detach()
	{	p1.detach(); p2.detach();	}

	BndView<Eval,Expr1> p1;
	BndView<Eval,Expr2> p2;
	#ifdef CASPER_LOG
	Store& store;
	#endif
};

/**
 * 	Enforces inequality between two expressions evaluating to a generic type Eval.
 * 	\ingroup Filtering
 */
template<class Eval,class Expr1,class Expr2>
struct BndFilterView2<Less,Eval,Expr1,Eval,Expr2> :
	BndFilterView2<Greater,Eval,Expr2,Eval,Expr1>
{
	BndFilterView2(Store& store, const Expr1& p1, const Expr2& p2) :
		BndFilterView2<Greater,Eval,Expr2,Eval,Expr1>(store,p2,p1) {}
};

#if 0

#define GECODE_SUM
/**
 *	Enforces the constraint \f$\sum_{i=1}^{n}x_{i}=b\f$.
 *	\tparam Expr1 The sequence of variables \f$x_1\ldots x_n\f$
 *	\tparam Expr2 \f$b\f$.
 *  \ingroup Filtering
 **/
#ifdef GECODE_SUM
template<class Eval,class Expr1,class Expr2>
struct BndFilterView2<SumEqual,Seq<Eval>,Expr1,Eval,Expr2> : IFilter
{
	BndFilterView2(Store& store,const Expr1& v1, const Expr2& v2) :
		IFilter(store),store(store),x(store,v1),v(store,v2) {}

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "BndFilterView2<SumEqual,Seq<Eval>,Expr1,Eval,Expr2>", Util::Logger::filterExecuteBegin);
		#endif

		if (!v.ground())
			return true;

		setInQueue(1);

		// try to rewrite: 		FIXME: this only works if Expr1 is a var array. Doesn't look like there is a workaround...
/*		uint nground = 0;
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
			detach();
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
			detach();
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
			detach();
			return newx[0].domain().updateRange(v.value()-s,v.value()-s);
		}
*/
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
			detach();
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
	void detach()
	{
		x.detach();
		v.detach();
	}

	Store&					 store;
	BndArrayView<Eval,Expr1> x;
	ValView<Eval,Expr2>		 v;
	INotifiable*			 pOwner;
};
#else

template<class Eval,class Expr1,class Expr2>
struct BndFilterView2<SumEqual,Seq<Eval>,Expr1,Eval,Expr2> : IFilter
{
	BndFilterView2(Store& store,const Expr1& v1, const Expr2& v2) :
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
	BndArrayView<Eval,Expr1> x;
	ValView<Eval,Expr2>		 v;
	INotifiable*			 pOwner;
};
#endif

#endif

// BndFilterView over a Filter (still needed?)
#if 0
template<>
struct BndFilterView<Filter> : IFilter
{
	BndFilterView(Store& s,Filter f) : IFilter(s),pFilter(f.getPImpl()) {}

	bool execute()
	{	return pFilter->execute();	}
	Cost cost() const { return pFilter->cost(); }

	void attach(INotifiable* s)
	{	pFilter->attach(s); }
	void detach()
	{	pFilter->detach(); }
	IFilter*	pFilter;
};
#endif

// BndFilterView over a BndExpr<bool>
template<>
struct BndFilterView<BndExpr<bool> > : IFilter
{
	BndFilterView(Store& s,BndExpr<bool> e) :
		IFilter(s),rExpr(e.getImpl())
		#ifdef CASPER_LOG
		,store(store)
		#endif
		{}

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "BndFilterView<BndExpr<bool> >", Util::Logger::filterExecuteBegin);
		#endif

		return rExpr.updateMin(true);
	}
//	Filter	operator!();

	void attach(INotifiable* s)
	{	rExpr.attach(s); }
	void detach()
	{	rExpr.detach(); }
	Detail::IBndExpr<bool>&	rExpr;
	#ifdef CASPER_LOG
	Store& store;
	#endif
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
template<class F,class Expr1,class Eval>
struct BndViewRel1 : BndReify<Rel1<F,Expr1> >
{
	typedef Rel1<F,Expr1> Rel;
	typedef BndReify<Rel> Super;
	BndViewRel1(CPSolver& solver, const Expr1& p1) :
		Super(solver,Rel(p1)) {}
};

template<class F, class Expr1,class Expr2,class Eval>
struct BndViewRel2 : BndReify<Rel2<F,Expr1,Expr2> >
{
	typedef Rel2<F,Expr1,Expr2> Rel;
	typedef BndReify<Rel> Super;
	BndViewRel2(CPSolver& solver, const Expr1& p1, const Expr2& p2) :
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
    		Var<Eval> aux(s,&(*b[ValView<int,IdxView>(s,p2).value()-
    		                     CASPER_CP_ELEMENT_ARRAY_BASE]));
    		return postBndFilter(s,aux==p3);
    	}
    	else
    		return s.post(new (s)
    						BndFilterView3<ElementEqual,Seq<Eval>,ArrayView,
													int,IdxView,
													Eval,EvalView>(s,p1,p2,p3));
    }

};

// if defined means that no auxiliary variable will be used for element
//#define CASPER_CP_ELEMENT_BNDVIEW

#ifndef CASPER_CP_ELEMENT_BNDVIEW
// element as expression: may introduce one auxiliary variable
template<class Expr1,class Expr2,class Eval>
struct BndViewRel2<Element,Expr1,Expr2,Eval> :
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
	static Super getSuper(Store& s,const Expr1& p1,const Expr2& p2)
	{
		if (ValView<int,Expr2>(s,p2).ground())
		{
			int idx = ValView<int,Expr2>(s,p2).value()-CASPER_CP_ELEMENT_ARRAY_BASE;

			typedef typename Casper::Traits::GetElem<Expr1>::Type EElem;
			ElementView<Expr1> elem(p1);

			return Super(s,Helper<EElem,Elem>()(s,elem.get(idx)));
		}

		Elem v(s,Detail::VarDomCreator<typename Elem::Dom>().unionOf(s,p1));
		s.post(elementEqual(p1,p2,v));
		return Super(s,v);
	}

	Rel2<Element,Expr1,Expr2> getObj() const
	{	return Rel2<Element,Expr1,Expr2>(v1,idx.getObj()); }

	BndViewRel2(Store& store,const Expr1& p1, const Expr2& p2) :
		Super(getSuper(store,p1,p2)),
		store(store),
		v1(p1),idx(store,p2)
	{}

	Store& store;
	Expr1 v1;
	ValView<int,Expr2> idx;
};
#else

// FIXME: Test: not as strong as version above. Why?
// does not introduce auxiliary variables but might be slower
template<class Expr1,class Expr2,class Eval>
struct BndViewRel2<Element,Expr1,Expr2,Eval>
{
	BndViewRel2(Store& store, const Expr1& v1, const Expr2& v2) :
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

	Rel2<Element,Expr1,Expr2> getObj()  const
	{	return rel<Element>(a.getObj(),i.getObj());	}

	BndArrayView<Eval,Expr1>	a;
	BndView<int,Expr2>	i;
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
	void detach() {	v.detach();	}

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
		{
			#ifdef CASPER_LOG_OLD
			store.getEnv().log(this, "BndViewRel1<Cache,View,Eval>::SyncRange", Util::Logger::filterExecuteBegin);
			#endif
			return rOwner.v.updateRange(rOwner.cachedMin,rOwner.cachedMax) and rOwner.pParent->notify();
		}
		Cost cost() const { return constantLo;}
		void attach(INotifiable* f) { rOwner.v.attach(f); }
		void detach() { rOwner.v.detach(); }
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
		pSyncRange->store.filterSched().post(pSyncRange,false);
		cachedMin = std::max((Eval)cachedMin,v.min());
		cachedMax = std::min((Eval)cachedMax,v.max());
	}
	void detach() {	v.detach();	}

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
