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
 

#ifndef CASPER_LP_SUSPENDEDLINEAREQ_H_
#define CASPER_LP_SUSPENDEDLINEAREQ_H_

#include <casper/lp/lineareqrepr.h>
#include <casper/lp/linearexprview.h>
#include <casper/lp/var.h>
#include <casper/cp/view.h>

namespace Casper {
namespace LP {

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
{};

template<class Store,class F,class T1,class T2>
struct PostValActor<Store,Rel2<F,T1,T2> > :
	PostValActor2<Store,F,typename Traits::GetEval<T1>::Type,T1,
						  typename Traits::GetEval<T2>::Type,T2>
{};

template<class Store,class F,class T1,class T2,class T3>
struct PostValActor<Store,Rel3<F,T1,T2,T3> > :
	PostValActor3<Store,F,typename Traits::GetEval<T1>::Type,T1,
						  typename Traits::GetEval<T2>::Type,T2,
						  typename Traits::GetEval<T3>::Type,T3>
{};


// ** Linear equations **

template<class Store,class T1,class T2>
struct LinearEqViewRel2<Store,Equal,double,T1,double,T2>
{
	LinearEqViewRel2(Store& store,const T1& t1,const T2& t2) : v1(store,t1),v2(store,t2) {}
	bool isLinear() const {	return v1.isLinear() and v2.isLinear();	}
	bool isTrue() const { return false;	}
	bool canBeTrue() const { return true;	}
	LinearEqRepr getRepr() const {	assert(isLinear());	return v1.getRepr()==v2.getRepr();	}
	void attach(INotifiable* n) {	v1.attach(n); v2.attach(n);	}
	void detach(INotifiable* n) {	v1.detach(n); v2.detach(n);	}

	LinearExprView<Store,T1> v1;
	LinearExprView<Store,T2> v2;
};

template<class Store,class T1,class T2>
struct LinearEqViewRel2<Store,LessEqual,double,T1,double,T2>
{
	LinearEqViewRel2(Store& store,const T1& t1,const T2& t2) : v1(store,t1),v2(store,t2) {}
	bool isLinear() const {	return v1.isLinear() and v2.isLinear();	}
	bool isTrue() const { return false;	}
	bool canBeTrue() const { return true;	}
	LinearEqRepr getRepr() const {	assert(isLinear());	return v1.getRepr()<=v2.getRepr();	}
	void attach(INotifiable* n) {	v1.attach(n); v2.attach(n);	}
	void detach(INotifiable* n) {	v1.detach(n); v2.detach(n);	}

	LinearExprView<Store,T1> v1;
	LinearExprView<Store,T2> v2;
};

template<class Store,class T1,class T2>
struct LinearEqViewRel2<Store,GreaterEqual,double,T1,double,T2>
{
	LinearEqViewRel2(Store& store,const T1& t1,const T2& t2) : v1(store,t1),v2(store,t2) {}
	bool isLinear() const {	return v1.isLinear() and v2.isLinear();	}
	bool isTrue() const { return false;	}
	bool canBeTrue() const { return true;	}
	LinearEqRepr getRepr() const {	assert(isLinear());	return v1.getRepr()>=v2.getRepr();	}
	void attach(INotifiable* n) {	v1.attach(n); v2.attach(n);	}
	void detach(INotifiable* n) {	v1.detach(n); v2.detach(n);	}

	LinearExprView<Store,T1> v1;
	LinearExprView<Store,T2> v2;
};

template<class Store,class F,class T1,class T2>
struct PostValActor2<Store,Equal,double,T1,double,T2>
{
	bool operator()(Store& store, const T1& t1, const T2& t2)
	{
		LinearExprValView<T1> v1(store,t1);
		LinearExprValView<T2> v2(store,t2);
		if (v1.ground() and v2.isLinear())
	}
};

// ** Logical equations involving linear equations **



template<class Store,class T1,class T2>
struct LinearEqViewRel2<Store,IfThen,bool,T1,bool,T2>
{
	LinearEqViewRel2(Store& store,const T1& t1,const T2& t2) : v1(store,t1),v2(store,t2) {}
	bool isLinear() const
	{
		return v1.isTrue() and v2.isLinear();
	}
	bool isTrue() const
	{ return !v1.canBeTrue() or (v1.isTrue() or v2.isTrue());	}

	bool canBeTrue() const
	{ return !(v1.isTrue() and !v2.canBeTrue()); }

	LinearEqRepr getRepr() const
	{
		assert(isLinear());
		assert(v1.isTrue());
		return v2.getRepr();
	}

	void attach(INotifiable* n) {	v1.attach(n); v2.attach(n);	}
	void detach(INotifiable* n) {	v1.detach(n); v2.detach(n);	}

	CP::ChkView<T1> v1;
	LinearEqView<Store,T2> v2;
};

template<class Store,class T1,class T2,class T3>
struct LinearEqViewRel3<Store,IfThenElse,bool,T1,bool,T2,bool,T3>
{
	LinearEqViewRel3(Store& store,const T1& t1,const T2& t2,const T3& t3) :
		v1(store,t1),v2(store,t2),v3(store,t3) {}
	bool isLinear() const
	{
		return (v1.isTrue() and v2.isLinear()) or
			   (!v1.canBeTrue() and v3.isLinear());
	}
	bool isTrue() const
	{ return (v1.isTrue() and v2.isTrue()) or
			 (!v1.canBeTrue() and v3.isTrue());}

	bool canBeTrue() const
	{ return !((v1.isTrue() and !v2.canBeTrue()) or
			   (!v1.canBeTrue() and !v3.canBeTrue())); }

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
	LinearEqView<Store,T2> v2;
	LinearEqView<Store,T3> v3;
};

}	// LP
}	// Casper

#endif /* CASPER_LP_SUSPENDEDLINEAREQ_H_ */
