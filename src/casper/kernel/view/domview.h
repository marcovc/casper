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

#ifndef _H_CASPER_KERNEL_VIEW_DOMVIEW
#define _H_CASPER_KERNEL_VIEW_DOMVIEW

#include <casper/kernel/expression.h>
#include <casper/kernel/view/common.h>

namespace casper {

// TODO: the Dom& should be stored in the super class IDomExpr

/**
 * 	Provides a specific domain interface to a generic view. This
 * 	interface is inferred from the \a Eval type at compilation time.
 *  The general case is to introduce an auxiliary variable
 *  bounded to the view using an equality constraint. Specializations
 *  of this class typically provide better alternatives.
 *  \ingroup Views
 **/
template<class Eval,class View,
		 class DomT = typename DomTraits<View>::Dom>
struct DomView
{
	typedef	DomT	Dom;
	typedef IDomExpr<Dom>	Super;
	DomView(ICPSolver& solver, const View& v) :
		d(solver,
			(Eval)BndView<Eval,View>(solver,v).min(),
			(Eval)BndView<Eval,View>(solver,v).max())
	{
		solver.signalNewDomain(&d);
		solver.post( v == DomVar<Eval,Dom>(solver,&d) );
	}

	ICPSolver& 	solver() const	{	return d.solver();	}
	Dom*	operator->()			{	return &d;	}
	Dom*	operator->() const	{	return const_cast<Dom*>(&d);	}
	Dom&	operator*()				{	return d;	}
	Dom&	operator*() const	{	return const_cast<Dom&>(d);	}

	void attach(INotifiable* f)	{ 	d.attachOnDomain(f); }
	void detach(INotifiable* f)	{	d.detachOnDomain(f); }

//	View getView() const
//	{	return DomVar<Eval,DomT>(this->solver(),&d);}

	Dom	d;
};

/**
 * 	DomView over a DomVar.
 *  \ingroup Views
 **/
template<class Eval,class DomT>
struct DomView<Eval,DomVar<Eval,DomT>,DomT>
{
	typedef	DomT		Dom;
	DomView(ICPSolver& solver, const DomVar<Eval,Dom>& v) :
		d(v.domain())
		{}
	DomView(ICPSolver& solver, Dom& d) : d(d) {}

	void attach(INotifiable* f)	{ 	d.attachOnDomain(f); }
	void detach(INotifiable* f)	{	d.detachOnDomain(f); }

	ICPSolver&	solver() const			{	return d.solver();	}
	Dom*	operator->()			{	return &d;	}
	Dom*	operator->() const	{	return const_cast<Dom*>(&d);	}
	Dom&	operator*()				{	return d;	}
	Dom&	operator*() const	{	return const_cast<Dom&>(d);	}
	DomVar<Eval,DomT> getView() const
	{	return DomVar<Eval,DomT>(d.solver(),&d);}

	Dom&	d;
};

#if 0
/**
 * 	Identity: DomView over a DomView.
 *  \ingroup Views
 **/
template<class Eval,class View, class DomT>
struct DomView<Eval,DomView<Eval,View,DomT>,DomT> : IDomExpr<DomT>
{
	typedef	DomT		Dom;
	typedef IDomExpr<Dom>	Super;
	DomView(ICPSolver& solver, const DomView<Eval,View,DomT>& v) :
		Super(solver),d(*v) {}
	DomView(ICPSolver& solver, Dom& d) : Super(solver),d(d) {}

	Dom*	operator->()			{	return &d;	}
	Dom*	operator->() const	{	return const_cast<Dom*>(&d);	}
	Dom&	operator*()				{	return d;	}
	Dom&	operator*() const	{	return const_cast<Dom&>(d);	}

	DomView<Eval,View,DomT> getView() const
	{	return DomView<Eval,View,DomT>(this->solver(),d);	}

	Dom&	d;
};
#endif

/**
 * 	DomView over a DomExpr.
 *  \ingroup Views
 **/
template<class Eval,class DomT>
struct DomView<Eval,DomExpr<Eval,DomT>,DomT>
{
	typedef	DomT		Dom;
	DomView(ICPSolver& solver, const DomExpr<Eval,Dom>& v) :
		d(*v)
		{}
	DomView(ICPSolver& solver, Dom& d) : d(d) {}
	void attach(INotifiable* f)	{ 	d.attachOnDomain(f); }
	void detach(INotifiable* f)	{	d.detachOnDomain(f); }

	ICPSolver&	solver() const			{	return d.solver();	}
	Dom*	operator->()			{	return &d;	}
	Dom*	operator->() const	{	return const_cast<Dom*>(&d);	}
	Dom&	operator*()				{	return d;	}
	Dom&	operator*() const	{	return const_cast<Dom&>(d);	}
	DomExpr<Eval,DomT> getView() const
	{	return DomExpr<Eval,DomT>(d.solver(),&d);}
	Dom&	d;
};

template<class Eval,class View,class DomT>
struct DomTraits<DomView<Eval,View,DomT> >
{	typedef DomT	Dom;	};

template<class E,class V,class D>
struct GetCPSolver<DomView<E,V,D> >
{	ICPSolver& operator()(DomView<E,V,D> s)
	{ return s.solver(); }
};

template<class EvalT,class View,class Dom>
struct EvalTraits<DomView<EvalT,View,Dom> >
{	typedef	EvalT	Eval;	};

template<class Eval,class View,class Dom>
std::ostream& operator<<(std::ostream& o, const casper::DomView<Eval,View,Dom>& d)
{	return operator<<(o,*d);	}

};


#endif /*_H_CASPER_KERNEL_VIEW_DOMVIEW */
