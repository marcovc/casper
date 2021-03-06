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

#ifndef _H_CASPER_CP_VIEW_DOM
#define _H_CASPER_CP_VIEW_DOM

#include <casper/cp/expr.h>
#include <casper/cp/view/common.h>
#include <casper/cp/store.h>

namespace Casper {
namespace CP {

// TODO: the Dom should be stored in the super class IDomExpr

/**
 * 	DomView over a generic expression. Tries to decompose the
 * 	expression creating one auxiliary variable and propagator.
 * 	\ingroup DomViews
 */
template<class Eval,class View,
		 class DomT = typename Traits::GetDom<View>::Type>
struct DomView
{
	typedef	DomT	Dom;
	//typedef IDomExpr<Dom>	Super;
	DomView(Store& store, const View& v) :
		d(store,
			(Eval)BndView<Eval,View>(store,v).min(),
			(Eval)BndView<Eval,View>(store,v).max()),
		pOwner(NULL)
	{
		store.getStats().signalNewIntDomain();	// FIXME
		store.post( v == Var<Eval,Dom>(store,&d) );
	}

//	Store& 	solver() const	{	return d.solver();	}
	Dom*	operator->()			{	return &d;	}
	Dom*	operator->() const	{	return const_cast<Dom*>(&d);	}
	Dom&	operator*()				{	return d;	}
	Dom&	operator*() const	{	return const_cast<Dom&>(d);	}

	void attach(INotifiable* f)
	{
		assert(pOwner==NULL or pOwner==f);
		pOwner = f;
		d.attachOnDomain(pOwner);
	}
	void detach()	{	d.detachOnDomain(pOwner); }

	bool ground() const {	return d.ground();	}
//	View getObj() const
//	{	return Var<Eval,DomT>(this->solver(),&d);}

	Dom	d;
	INotifiable* pOwner;
};

/**
 * 	DomView over a CP variable.
 *  \ingroup DomViews
 **/
template<class Eval,class DomT>
struct DomView<Eval,Var<Eval,DomT>,DomT>
{
	typedef	DomT		Dom;
	DomView(Store& store, const Var<Eval,Dom>& v) :
		store(store),
		d(v.domain()),
		pOwner(NULL)
		{}
	DomView(Store& store, Dom& d) : store(store),d(d) {}

	void attach(INotifiable* f)
	{
		assert(pOwner==f or pOwner==NULL);
		pOwner = f;
		d.attachOnDomain(pOwner);
	}
	void detach()	{	d.detachOnDomain(pOwner); }

//	CPSolver&	solver() const			{	return d.solver();	}
	Dom*	operator->()			{	return &d;	}
	Dom*	operator->() const	{	return const_cast<Dom*>(&d);	}
	Dom&	operator*()				{	return d;	}
	Dom&	operator*() const	{	return const_cast<Dom&>(d);	}
	Var<Eval,DomT> getObj() const
	{	return Var<Eval,DomT>(store,&d);}
	bool ground() const {	return d.ground();}
	Store& store;
	Dom&	d;
	INotifiable* pOwner;
};

/*
 * 	DomView over a DomExpr.
 *  \ingroup Views
 **/
template<class Eval,class DomT>
struct DomView<Eval,DomExpr<Eval,DomT>,DomT>
{
	typedef	DomT		Dom;
	DomView(Store& store, const DomExpr<Eval,Dom>& v) :
		store(store),
		d(*v),
		pOwner(NULL)
		{}
	DomView(Store& store, Dom& d) : store(store),d(d) {}
	void attach(INotifiable* f)
	{
		assert(pOwner==NULL or pOwner==f);
		pOwner = f;
		d.attachOnDomain(pOwner);
	}
	void detach()	{	d.detachOnDomain(pOwner); }

//	CPSolver&	solver() const			{	return d.solver();	}
	Dom*	operator->()			{	return &d;	}
	Dom*	operator->() const	{	return const_cast<Dom*>(&d);	}
	Dom&	operator*()				{	return d;	}
	Dom&	operator*() const	{	return const_cast<Dom&>(d);	}
	DomExpr<Eval,DomT> getObj() const
	{	return DomExpr<Eval,DomT>(store,&d);}
	Store&	store;
	Dom&	d;
	INotifiable* pOwner;
};

namespace Traits {
template<class Eval,class View,class DomT>
struct GetDom<DomView<Eval,View,DomT> >
{
	typedef DomT	Type;
};
} // Traits


} // CP

namespace Traits {
template<class EvalT,class View,class Dom>
struct GetEval<CP::DomView<EvalT,View,Dom> >
{	typedef EvalT	Type;	};
} // Traits

//template<class E,class V,class D>
//struct GetPState<DomView<E,V,D> >
//{	Env* operator()(DomView<E,V,D> s)
//	{ return s.solver(); }
//};

} // Casper

template<class Eval,class View,class Dom>
std::ostream& operator<<(std::ostream& o, const Casper::CP::DomView<Eval,View,Dom>& d)
{	return operator<<(o,*d);	}


#endif /*_H_CASPER_CP_VIEW_DOM */
