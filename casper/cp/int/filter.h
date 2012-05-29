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

#ifndef _H_CASPER_CP_INT_FILTER
#define _H_CASPER_CP_INT_FILTER

#include <casper/cp/filter/common.h>
#include <casper/cp/filter/linear.h>
#include <casper/cp/filter/dom.h>
//#include <casper/kernel/expression.h>
#include <casper/cp/int/fd.h>
#include <casper/cp/view/dom.h>

//#ifdef CASPER_PRECOMPILED
//#include <casper/cp/int/spexpr/explicit_postbnd.h>
//#include <casper/cp/int/spexpr/explicit_postdom.h>
//#endif

namespace Casper {
namespace CP {


template<class,class,class,class>
struct BndDistinctD1;

template<class Expr1,class Expr2>
struct BndDistinctD1<int,Expr1,int,Expr2> : IFilter
{
	BndDistinctD1(Store& store, const Expr1& v1, const Expr2& v2) :
		IFilter(store),v1(store,v1),v2(store,v2),demon(*this)
		#ifdef CASPER_LOG
		,store(store)
		#endif
		{}

	struct Demon : INotifiable
	{
		Demon(BndDistinctD1& rOwner) : rOwner(rOwner) {}
		bool notify()
		{	return !rOwner.v2.ground() or rOwner.pOwner->notify(); }
		BndDistinctD1& rOwner;
	};

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "BndDistinctD1<int,Expr1,int,Expr2>", Util::Logger::filterExecuteBegin);
		#endif

		if (!v2.ground())
			return true;
		if (v2.value() < v1.min() or
			v2.value() > v1.max())
		{
			detach();
			return true;
		}
		if (v2.value() == v1.min())
			return v1.updateMin(v1.min()+1);
		else
		if (v2.value() == v1.max())
			return v1.updateMax(v1.max()-1);
		return true;
	}

	void attach(INotifiable* s)
	{
		pOwner = s;
		v1.attach(&demon);
		v2.attach(&demon);
	}
	void detach()
	{	v1.detach(); v2.detach();	}

	BndView<int,Expr1>	v1;
	ValView<int,Expr2>	v2;
	INotifiable*	pOwner;
	Demon	demon;
	#ifdef CASPER_LOG
	Store& store;
	#endif
};

template<class,class,class,class>
struct DomDistinctD1;

/**
	Directional filter for disequality constraint between integers
*/
template<class Expr1,class Expr2>
struct DomDistinctD1<int,Expr1,int,Expr2> : IFilter
{
	DomDistinctD1(Store& store, const Expr1& p1,const Expr2& p2) :
		IFilter(store),p1(store,p1),p2(store,p2)
		#ifdef CASPER_LOG
		,store(store)
		#endif
		{}

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "DomDistinctD1<int,Expr1,int,Expr2>", Util::Logger::filterExecuteBegin);
		#endif

		if (!p2.ground())
			return true;
		return p1->erase(p2.value());
 	}

	void attach(INotifiable* s) {	p2.attach(s); }
	void detach() {	p2.detach(); }

	DomView<int,Expr1> p1;
	ValView<int,Expr2> p2;
	#ifdef CASPER_LOG
	Store& store;
	#endif
};

/**
	Enforces the disequality constraint
	between two integer expressions.
	\ingroup IntFilters
*/
template<class Expr1,class Expr2>
struct BndFilterView2<Distinct,int,Expr1,int,Expr2> : IFilter
{
	BndFilterView2(Store& store,const Expr1& p1,const Expr2& p2) :
		IFilter(store),p1(store,p1,p2),p2(store,p2,p1)
		#ifdef CASPER_LOG
		,store(store)
		#endif
		{ }

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "BndFilterView2<Distinct,int,Expr1,int,Expr2>", Util::Logger::filterExecuteBegin);
		#endif

		return p1.execute() and p2.execute();
	}
	void attach(INotifiable* s)
	{	p1.attach(s); p2.attach(s); }
	void detach()
	{	p1.detach(); p2.detach();	}

	BndDistinctD1<int,Expr1,int,Expr2> p1;
	BndDistinctD1<int,Expr2,int,Expr1> p2;
	#ifdef CASPER_LOG
	Store& store;
	#endif
};

/**
	Enforces domain consistency on the disequality constraint
	between two integer expressions.
	\ingroup IntFilters
*/
template<class Expr1,class Expr2>
struct DomFilterView2<Distinct,int,Expr1,int,Expr2> : IFilter
{
	DomFilterView2(Store& store,const Expr1& p1,const Expr2& p2) :
		IFilter(store),p1(store,p1,p2),p2(store,p2,p1)
		#ifdef CASPER_LOG
		,store(store)
		#endif
		{ }

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "DomFilterView2<Distinct,int,Expr1,int,Expr2>", Util::Logger::filterExecuteBegin);
		#endif

		return p1.execute() && p2.execute();
	}
	void attach(INotifiable* s)
	{	p1.attach(s); p2.attach(s); }
	void detach()
	{	p1.detach(); p2.detach();	}

	DomDistinctD1<int,Expr1,int,Expr2> p1;
	DomDistinctD1<int,Expr2,int,Expr1> p2;
	#ifdef CASPER_LOG
	Store& store;
	#endif
};

// FIXME: this should be Def consistency since it can prune more than Bnd
template<class V1,class V2>
struct PostBndFilter2<Distinct,int,V1,int,V2>
{
	static bool post(Store& s, const V1& p1,const V2& p2)
	{
		ChkViewRel2<Distinct,int,V1,int,V2> chk(s,p1,p2);
		if (chk.isTrue())
			return true;
		if (!chk.canBeTrue())
			return false;

		IFilter* f1;
		IFilter* f2;

		if (Casper::CP::Traits::isDomExpr(p1))
			f1 = new (s) DomDistinctD1<int,V1,int,V2>(s,p1,p2);
		else
			f1 = new (s) BndDistinctD1<int,V1,int,V2>(s,p1,p2);
		if (Casper::CP::Traits::isDomExpr(p2))
			f2 = new (s) DomDistinctD1<int,V2,int,V1>(s,p2,p1);
		else
			f2 = new (s) BndDistinctD1<int,V2,int,V1>(s,p2,p1);

		return s.post(f1) and s.post(f2);
	}
};

template<class V1,class V2>
struct PostBndFilter2<Distinct,bool,V1,bool,V2>
{
	static bool post(Store& s, const V1& p1,const V2& p2)
	{	return postBndFilter(s,cast<int>(p1) != cast<int>(p2));	}
};



template<class V1,class V2>
struct PostDomFilter2<Distinct,int,V1,int,V2>
{
	static bool post(Store& s, const V1& p1,const V2& p2)
	{
		IFilter* f1;
		IFilter* f2;

		f1 = new (s) DomDistinctD1<int,V1,int,V2>(s,p1,p2);
		f2 = new (s) DomDistinctD1<int,V2,int,V1>(s,p2,p1);

		return s.post(f1) and s.post(f2);
	}
};

template<class V1,class V2>
struct PostDomFilter2<Distinct,bool,V1,bool,V2>
{
	static bool post(Store& s, const V1& p1,const V2& p2)
	{	return postDomFilter(s,cast<int>(p1) != cast<int>(p2));	}
};

/**
 * 	Enforces the Greater binary relation between two integer expressions.
 * 	\ingroup IntFilters
 */
template<class Expr1,class Expr2>
struct BndFilterView2<Greater,int,Expr1,int,Expr2> : IFilter
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
		store.getEnv().log(this, "BndFilterView2<Greater,int,Expr1,int,Expr2>", Util::Logger::filterExecuteBegin);
		#endif

		if (p1.min()>p2.max())
		{
			detach();
			return true;
		}
		return p1.updateMin(p2.min()+1) and
			   p2.updateMax(p1.max()-1);
	}

	void attach(INotifiable* s)
	{	/*if (!p1.ground() and !p2.ground())*/ {p1.attach(s); p2.attach(s);} }
	void detach()
	{	p1.detach(); p2.detach();	}

	BndView<int,Expr1> p1;
	BndView<int,Expr2> p2;
	#ifdef CASPER_LOG
	Store& store;
	#endif
};


template<class V1,class V2>
struct PostBndFilter2<Greater,bool,V1,bool,V2>
{
	static bool post(Store& s, const V1& p1,const V2& p2)
	{
		return postBndFilter(s,p1==true) and
			   postBndFilter(s,p2==false);
	}
};

template<class V1,class V2>
struct PostBndFilter2<Less,bool,V1,bool,V2>
{
	static bool post(Store& s, const V1& p1,const V2& p2)
	{
		return postBndFilter(s,p1==false) and
			   postBndFilter(s,p2==true);
	}
};

template<class V1,class V2>
struct PostDomFilter2<Greater,int,V1,int,V2>
{
	static bool post(Store& s, const V1& p1,const V2& p2)
	{	return postBndFilter(s,rel<Greater>(p1,p2)); }
};

template<class V1,class V2>
struct PostDomFilter2<GreaterEqual,int,V1,int,V2>
{
	static bool post(Store& s, const V1& p1,const V2& p2)
	{	return postBndFilter(s,rel<GreaterEqual>(p1,p2)); }
};

template<class V1,class V2>
struct PostDomFilter2<Less,int,V1,int,V2>
{
	static bool post(Store& s, const V1& p1,const V2& p2)
	{	return postBndFilter(s,rel<Less>(p1,p2)); }
};

template<class V1,class V2>
struct PostDomFilter2<LessEqual,int,V1,int,V2>
{
	static bool post(Store& s, const V1& p1,const V2& p2)
	{	return postBndFilter(s,rel<LessEqual>(p1,p2)); }
};

template<class Expr1,class Expr2,class Expr3>
struct PostBndFilter3<LinearGreater,Seq<int>,Expr1,Seq<int>,Expr2,int,Expr3>
{
    static bool post(Store& s,const Expr1& p1,const Expr2& p2,const Expr3& p3)
    {	return s.post(linearGreaterEqual(p1,p2,p3+1));   }
};

template<class Expr1,class Expr2,class Expr3>
struct PostBndFilter3<LinearLess,Seq<int>,Expr1,Seq<int>,Expr2,int,Expr3>
{
    static bool post(Store& s,const Expr1& p1,const Expr2& p2,const Expr3& p3)
    {	return s.post(linearLessEqual(p1,p2,p3-1));   }
};

/**
 * 	Enforces the seq membership constraint.
 */
template<class Expr1,class Elem,class Expr2>
struct BndFilterView2<Member,Elem,Expr1,Seq<Elem>,Expr2> : IFilter
{
	BndFilterView2(Store& s,const Expr1& elem, const Expr2& seq) :
		IFilter(s),elem(s,elem),seq(seq) {}

	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		set->getStore().getEnv().log(this, "BndFilterView2<Member,Elem,Expr1,Seq<Elem>,Expr2>", Util::Logger::filterExecuteBegin);
		#endif

		if (!elem->intersectWithSeq(seq))
			return false;
		return true;
	}

	void attach(INotifiable* s)
	{	}
	void detach()
	{	 }

	DomView<Elem,Expr1>	elem;
	Expr2	seq;
};

template<class Elem,class Expr1,class Expr2>
struct PostBndFilter2<Member,Elem,Expr1,Seq<Elem>,Expr2 >
{
    static bool post(Store& s,const Expr1& p1,const Expr2& p2)
    {
    	IterationView<Expr2> it(p2);
    	if (!it.valid())
    		return false;
    	Elem lastVal = it.value();
    	it.iterate();
    	for ( ; it.valid(); it.iterate())
    	{
    		assert(it.value()>lastVal);
    		lastVal = it.value();
    	}
    	return s.post(new (s) BndFilterView2<Member,Elem,Expr1,Seq<Elem>,Expr2>(s,p1,p2));
    }
};


} // CP
} // Casper

#endif
