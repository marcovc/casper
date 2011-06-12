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
 

#ifndef CASPER_LP_LINEAREXPRVIEW_H_
#define CASPER_LP_LINEAREXPRVIEW_H_

#include <casper/lp/linearexprrepr.h>
#include <casper/lp/var.h>
//#include <casper/lp/vararray.h>
#include <casper/cp/view.h>
#include <casper/util/container/stdvector.h>

namespace Casper {
namespace LP {

// attaching on this expression means waiting until expression is linear
// default behaviour is wait until groundness
template<class Store,class T>
struct LinearExprView
{
	typedef typename Casper::Traits::GetEval<T>::Type	Eval;
	LinearExprView(Store& store, const T& v) : v(store,v) {}
	bool isLinear() const {	return v.ground(); }
	bool isConstant() const {	return v.ground(); }
	LinearExprRepr getRepr() const { assert(isLinear()); return LinearExprRepr(static_cast<double>(v.value()));	}
	void attach(INotifiable* n) {	v.attach(n);	}
	void detach(INotifiable* n) {	v.detach(n);	}
	CP::ValView<Eval,T> v;
};

template<class Store>
struct LinearExprView<Store,double>
{
	LinearExprView(Store& store, const double& v) : v(v) {}
	bool isLinear() const {	return true; }
	bool isConstant() const {	return true; }
	LinearExprRepr getRepr() const { assert(isLinear()); return LinearExprRepr(v);	}
	void attach(INotifiable* n) {}
	void detach(INotifiable* n) {}
	double v;
};

template<class Store>
struct LinearExprView<Store,Var>
{
	LinearExprView(Store& store, const Var& v) : v(v) {}
	bool isLinear() const {	return true; }
	bool isConstant() const {	return false; }
	LinearExprRepr getRepr() const { assert(isLinear()); return LinearExprRepr(v);	}
	void attach(INotifiable* n) {}
	void detach(INotifiable* n) {}
	Var v;
};

template<class Store,class T1,class T2>
struct LinearExprView<Store,Rel2<Add,T1,T2> >
{
	LinearExprView(Store& store, const Rel2<Add,T1,T2>& r) :
		v1(store,r.p1),
		v2(store,r.p2) {}
	bool isLinear() const {	return v1.isLinear() and v2.isLinear(); }
	bool isConstant() const {	return v1.isConstant() and v2.isConstant(); }
	LinearExprRepr getRepr() const
	{
		assert(isLinear());
		return v1.getRepr()+v2.getRepr();
	}
	void attach(INotifiable* n)
	{
		v1.attach(n);
		v2.attach(n);
	}
	void detach(INotifiable* n)
	{
		v1.detach(n);
		v2.detach(n);
	}
	LinearExprView<Store,T1>	v1;
	LinearExprView<Store,T2>	v2;
};

template<class Store,class T1,class T2>
struct LinearExprView<Store,Rel2<Sub,T1,T2> >
{
	LinearExprView(Store& store, const Rel2<Sub,T1,T2>& r) :
		v1(store,r.p1),
		v2(store,r.p2) {}
	bool isLinear() const {	return v1.isLinear() and v2.isLinear(); }
	bool isConstant() const {	return v1.isConstant() and v2.isConstant(); }
	LinearExprRepr getRepr() const
	{
		assert(isLinear());
		return v1.getRepr()-v2.getRepr();
	}
	void attach(INotifiable* n)
	{
		v1.attach(n);
		v2.attach(n);
	}
	void detach(INotifiable* n)
	{
		v1.detach(n);
		v2.detach(n);
	}
	LinearExprView<Store,T1>	v1;
	LinearExprView<Store,T2>	v2;
};

template<class Store,class T1,class T2>
struct LinearExprView<Store,Rel2<Mul,T1,T2> >
{
	LinearExprView(Store& store, const Rel2<Mul,T1,T2>& r) :
		v1(store,r.p1),
		v2(store,r.p2) {}
	bool isLinear() const
	{
		return (v1.isLinear() and v2.isConstant()) or
			   (v1.isConstant() and v2.isLinear());
	}
	bool isConstant() const
	{	return v1.isConstant() and v2.isConstant(); }
	LinearExprRepr getRepr() const
	{
		assert(isLinear());
		return v1.getRepr()*v2.getRepr();
	}
	void attach(INotifiable* f)
	{
		v1.attach(f);
		v2.attach(f);
	}
	void detach(INotifiable* f)
	{
		v1.detach(f);
		v2.detach(f);
	}
	LinearExprView<Store,T1>	v1;
	LinearExprView<Store,T2>	v2;
};

template<class Store,class T1,class T2>
struct LinearExprView<Store,Rel2<Div,T1,T2> >
{
	LinearExprView(Store& store, const Rel2<Div,T1,T2>& r) :
		v1(store,r.p1),
		v2(store,r.p2) {}
	bool isLinear() const
	{
		return v1.isLinear() and v2.isConstant();
	}
	LinearExprRepr getRepr() const
	{
		assert(isLinear());
		return v1.getRepr()/v2.getRepr();
	}
	void attach(INotifiable* f)
	{
		v1.attach(f);
		v2.attach(f);
	}
	void detach(INotifiable* f)
	{
		v1.detach(f);
		v2.detach(f);
	}
	LinearExprView<Store,T1>	v1;
	LinearExprView<Store,T2>	v2;
};

struct VarArray;

template<class Store,class VarArray,class T2>
struct LinearExprView<Store,Rel2<Element,VarArray,T2> >
{
	LinearExprView(Store& store, const Rel2<Element,VarArray,T2>& r) :
		v2(store,r.p2),
		views(store,r.p1.size())
	{
		for (uint i = 0; i < views.size(); ++i)
			views[i] = new (store) LinearExprView<Store,Var>(store,r.p1[i]);
	}
	bool isLinear() const
	{
		return v2.ground() and views[v2.value()]->isLinear();
	}
	LinearExprRepr getRepr() const
	{
		assert(isLinear());
		return views[v2.value()]->getRepr();
	}
	void attach(INotifiable* f)
	{
		v2.attach(f);
		for (uint i = 0; i < views.size(); ++i)
			views[i]->attach(f);
	}
	void detach(INotifiable* f)
	{
		v2.detach(f);
		for (uint i = 0; i < views.size(); ++i)
			views[i]->detach(f);
	}
	CP::ValView<int,T2>	v2;
	Util::StdVector<LinearExprView<Store,Var>*> views;
};

// TODO: Symmetric


}	// LP
}	// Casper

#endif /* CASPER_LP_LINEAREXPRVIEW_H_ */
