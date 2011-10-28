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
#include <casper/cp/filter/dom.h>
//#include <casper/kernel/expression.h>
#include <casper/cp/int/fd.h>
#include <casper/cp/view/dom.h>

namespace Casper {
namespace CP {


template<class,class,class,class>
struct BndDistinctD1;

template<class Expr1,class Expr2>
struct BndDistinctD1<int,Expr1,int,Expr2> : IFilter
{
	BndDistinctD1(Store& store, const Expr1& v1, const Expr2& v2) :
		IFilter(store),vv2(v2),v1(store,v1),v2(store,v2) {}

	bool execute()
	{
		if (!v2.ground())
			return true;
		if (v2.value() == v1.min())
			return v1.updateMin(v1.min()+1);
		else
		if (v2.value() == v1.max())
			return v1.updateMax(v1.max()-1);
		return true;
	}

	void attach(INotifiable* s)
	{	v1.attach(s); v2.attach(s);	}	// note: v1 must be also attached
	void detach(INotifiable* s)
	{	v1.detach(s); v2.detach(s);	}

	Expr2				vv2;
	BndView<int,Expr1>	v1;
	ValView<int,Expr2>	v2;
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
		IFilter(store),p1(store,p1),p2(store,p2) {}

	bool execute()
	{
		if (!p2.ground())
			return true;
		return p1->erase(p2.value());
 	}

	void attach(INotifiable* s) {	p2.attach(s); }
	void detach(INotifiable* s) {	p2.detach(s); }

	DomView<int,Expr1> p1;
	ValView<int,Expr2> p2;
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
		IFilter(store),p1(store,p1,p2),p2(store,p2,p1)	{ }

	bool execute()
	{	return p1.execute() and p2.execute();	}
	void attach(INotifiable* s)
	{	p1.attach(s); p2.attach(s); }
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	BndDistinctD1<int,Expr1,int,Expr2> p1;
	BndDistinctD1<int,Expr2,int,Expr1> p2;
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
		IFilter(store),p1(store,p1,p2),p2(store,p2,p1)	{ }

	bool execute()
	{	return p1.execute() && p2.execute();	}
	void attach(INotifiable* s)
	{	p1.attach(s); p2.attach(s); }
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	DomDistinctD1<int,Expr1,int,Expr2> p1;
	DomDistinctD1<int,Expr2,int,Expr1> p2;
};

// FIXME: this should be Def consistency since it can prune more than Bnd
template<class V1,class V2>
struct PostBndFilter2<Distinct,int,V1,int,V2>
{
	static bool post(Store& s, const V1& p1,const V2& p2)
	{
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
		IFilter(store),p1(store,p1), p2(store,p2) {}

	bool execute()
	{	return p1.updateMin(p2.min()+1) and
			   p2.updateMax(p1.max()-1);
	}
	bool entailed() const
	{	return p1.min() > p2.max();	}
//	Filter	operator!()
//	{	return Bnd(p2.getObj() >= p1.getObj());	}
	void attach(INotifiable* s)
	{	/*if (!p1.ground() and !p2.ground())*/ {p1.attach(s); p2.attach(s);} }
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	BndView<int,Expr1> p1;
	BndView<int,Expr2> p2;
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


} // CP
} // Casper

#endif
