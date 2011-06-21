/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_LP_LINEAREQVALVIEW_H_
#define CASPER_LP_LINEAREQVALVIEW_H_

#include <casper/lp/lineareqrepr.h>
#include <casper/lp/linearexprvalview.h>
#include <casper/lp/var.h>
#include <casper/cp/view.h>

namespace Casper {
namespace LP {

template<class,class>
struct LinearEqValView;

template<class Store,class T>
struct PostValActor
{
	bool operator()(Store& store,const T& t) const
	{	return static_cast<CP::Store&>(store).post(t);	}
};

template<class Store,class F,class E1,class T1>
struct PostValActor1
{
	bool operator()(Store& store,const T1& t1) const
	{	return static_cast<CP::Store&>(store).post(rel<F>(t1));	}
};

template<class Store,class F,class E1,class T1,class E2,class T2>
struct PostValActor2
{
	bool operator()(Store& store,const T1& t1,const T2& t2) const
	{	return static_cast<CP::Store&>(store).post(rel<F>(t1,t2));	}
};

template<class Store,class F,class E1,class T1,class E2,class T2,class E3,class T3>
struct PostValActor3
{
	bool operator()(Store& store,const T1& t1,const T2& t2,const T3& t3) const
	{	return static_cast<CP::Store&>(store).post(rel<F>(t1,t2,t3));	}
};

// convenience

template<class Store,class F,class T1>
struct PostValActor<Store,Rel1<F,T1> > :
	PostValActor1<Store,F,typename Traits::GetEval<T1>::Type,T1>
{
	typedef PostValActor1<Store,F,typename Traits::GetEval<T1>::Type,T1>
		Super;
	bool operator()(Store& store, const Rel1<F,T1>& r)
	{	return Super::operator()(store,r.p1);	}
};

template<class Store,class F,class T1,class T2>
struct PostValActor<Store,Rel2<F,T1,T2> > :
	PostValActor2<Store,F,typename Traits::GetEval<T1>::Type,T1,
						  typename Traits::GetEval<T2>::Type,T2>
{
	typedef PostValActor2<Store,F,typename Traits::GetEval<T1>::Type,T1,
						  typename Traits::GetEval<T2>::Type,T2>
		Super;
	bool operator()(Store& store, const Rel2<F,T1,T2>& r)
	{	return Super::operator()(store,r.p1,r.p2);	}
};

template<class Store,class F,class T1,class T2,class T3>
struct PostValActor<Store,Rel3<F,T1,T2,T3> > :
	PostValActor3<Store,F,typename Traits::GetEval<T1>::Type,T1,
						  typename Traits::GetEval<T2>::Type,T2,
						  typename Traits::GetEval<T3>::Type,T3>
{
	typedef PostValActor3<Store,F,typename Traits::GetEval<T1>::Type,T1,
						  typename Traits::GetEval<T2>::Type,T2,
						  typename Traits::GetEval<T3>::Type,T3>
		Super;
	bool operator()(Store& store, const Rel3<F,T1,T2,T3>& r)
	{	return Super::operator()(store,r.p1,r.p2,r.p3);	}
};

// demon

template<class Store,class T>
struct ValActor : INotifiable
{
	ValActor(Store& store, const T& r) : store(store),v(store,r)
	{	v.attach(this);	}
	bool notify()
	{
		if (v.isLinear())
		{
			v.detach(this);
			return store.post(v.getRepr());
		}
		return true;
	}
	Store&						store;
	LinearEqValView<Store,T>	v;
};

// posters

template<class Store,class T1,class T2>
struct PostValActor2<Store,Equal,double,T1,double,T2>
{
	bool operator()(Store& store, const T1& t1, const T2& t2)
	{
		typedef Rel2<Equal,T1,T2>	R;
		LinearEqValView<Store,R> r(store,R(t1,t2));
		if (r.isLinear())
			return store.post(r.getRepr());
		new (store) ValActor<Store,R>(store,R(t1,t2));
		return true;
	}
};

template<class Store,class T1,class T2>
struct PostValActor2<Store,LessEqual,double,T1,double,T2>
{
	bool operator()(Store& store, const T1& t1, const T2& t2)
	{
		typedef Rel2<LessEqual,T1,T2>	R;
		LinearEqValView<Store,R> r(store,R(t1,t2));
		if (r.isLinear())
			return store.post(r.getRepr());
		new (store) ValActor<Store,R>(store,R(t1,t2));
		return true;
	}
};

template<class Store,class T1,class T2>
struct PostValActor2<Store,GreaterEqual,double,T1,double,T2>
{
	bool operator()(Store& store, const T1& t1, const T2& t2)
	{
		typedef Rel2<GreaterEqual,T1,T2>	R;
		LinearEqValView<Store,R> r(store,R(t1,t2));
		if (r.isLinear())
			return store.post(r.getRepr());
		new (store) ValActor<Store,R>(store,R(t1,t2));
		return true;
	}
};

template<class Store,class T1,class T2>
struct PostValActor2<Store,IfThen,bool,T1,bool,T2>
{
	bool operator()(Store& store, const T1& t1, const T2& t2)
	{
		typedef Rel2<IfThen,T1,T2>	R;
		LinearEqValView<Store,R> r(store,R(t1,t2));
		if (r.isLinear())
			return store.post(r.getRepr());
		new (store) ValActor<Store,R>(store,R(t1,t2));
		return true;
	}
};

template<class Store,class T1,class T2,class T3>
struct PostValActor3<Store,IfThenElse,bool,T1,bool,T2,bool,T3>
{
	bool operator()(Store& store, const T1& t1, const T2& t2,const T3& t3)
	{
		typedef Rel3<IfThenElse,T1,T2,T3>	R;
		LinearEqValView<Store,R> r(store,R(t1,t2,t3));
		if (r.isLinear())
			return store.post(r.getRepr());
		new (store) ValActor<Store,R>(store,R(t1,t2,t3));
		return true;
	}
};

// ** Linear equations **

template<class Store,class T1,class T2>
struct LinearEqValView<Store,Rel2<Equal,T1,T2> >
{
	LinearEqValView(Store& store,const Rel2<Equal,T1,T2>& r) : v1(store,r.p1),v2(store,r.p2) {}
	bool isLinear() const {	return v1.isLinear() and v2.isLinear();	}
	LinearEqRepr getRepr() const {	assert(isLinear());	return v1.getRepr()==v2.getRepr();	}
	void attach(INotifiable* n) {	v1.attach(n); v2.attach(n);	}
	void detach(INotifiable* n) {	v1.detach(n); v2.detach(n);	}

	LinearExprValView<Store,T1> v1;
	LinearExprValView<Store,T2> v2;
};

template<class Store,class T1,class T2>
struct LinearEqValView<Store,Rel2<LessEqual,T1,T2> >
{
	LinearEqValView(Store& store,const Rel2<LessEqual,T1,T2>& r) : v1(store,r.p1),v2(store,r.p2) {}
	bool isLinear() const {	return v1.isLinear() and v2.isLinear();	}
	LinearEqRepr getRepr() const {	assert(isLinear());	return v1.getRepr()<=v2.getRepr();	}
	void attach(INotifiable* n) {	v1.attach(n); v2.attach(n);	}
	void detach(INotifiable* n) {	v1.detach(n); v2.detach(n);	}

	LinearExprValView<Store,T1> v1;
	LinearExprValView<Store,T2> v2;
};

template<class Store,class T1,class T2>
struct LinearEqValView<Store,Rel2<GreaterEqual,T1,T2> >
{
	LinearEqValView(Store& store,const Rel2<GreaterEqual,T1,T2>& r) : v1(store,r.p1),v2(store,r.p2) {}
	bool isLinear() const {	return v1.isLinear() and v2.isLinear();	}
	LinearEqRepr getRepr() const {	assert(isLinear());	return v1.getRepr()>=v2.getRepr();	}
	void attach(INotifiable* n) {	v1.attach(n); v2.attach(n);	}
	void detach(INotifiable* n) {	v1.detach(n); v2.detach(n);	}

	LinearExprValView<Store,T1> v1;
	LinearExprValView<Store,T2> v2;
};

// ** Logical equations involving linear equations **

template<class Store,class T1,class T2>
struct LinearEqValView<Store,Rel2<IfThen,T1,T2> >
{
	LinearEqValView(Store& store,const Rel2<IfThen,T1,T2>& r) : v1(store,r.p1),v2(store,r.p2) {}
	bool isLinear() const
	{
		return v1.isTrue() and v2.isLinear();
	}

	LinearEqRepr getRepr() const
	{
		assert(isLinear());
		assert(v1.isTrue());
		return v2.getRepr();
	}

	void attach(INotifiable* n) {	v1.attach(n); v2.attach(n);	}
	void detach(INotifiable* n) {	v1.detach(n); v2.detach(n);	}

	CP::ChkView<T1> v1;
	LinearEqValView<Store,T2> v2;
};

template<class Store,class T1,class T2,class T3>
struct LinearEqValView<Store,Rel3<IfThenElse,T1,T2,T3> >
{
	LinearEqValView(Store& store,const Rel3<IfThenElse,T1,T2,T3>& r) :
		v1(store,r.p1),v2(store,r.p2),v3(store,r.p3) {}
	bool isLinear() const
	{
		return (v1.isTrue() and v2.isLinear()) or
			   (!v1.canBeTrue() and v3.isLinear());
	}

	LinearEqRepr getRepr() const
	{
		assert(isLinear());
		if (v1.isTrue())
			return v2.getRepr();
		assert(!v1.canBeTrue());
		return v3.getRepr();
	}

	void attach(INotifiable* n) {	v1.attach(n); v2.attach(n);	v3.attach(n);	}
	void detach(INotifiable* n) {	v1.detach(n); v2.detach(n);	v3.attach(n);	}

	CP::ChkView<T1> v1;
	LinearEqValView<Store,T2> v2;
	LinearEqValView<Store,T3> v3;
};

}	// LP
}	// Casper

#endif /* CASPER_LP_LINEAREQVALVIEW_H_ */
