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

/** \file
 * 	Filter objects. Filters provide an (inference) mechanism to
 *  narrow domains of variables of a given relation.
 *	\sa FilterScheduler
 */

#ifndef _H_CASPER_KERNEL_FILTER_COMMON
#define _H_CASPER_KERNEL_FILTER_COMMON

#include <casper/kernel/notifiable.h>
#include <casper/kernel/notifier.h>
#include <casper/kernel/relation.h>
#include <casper/kernel/common.h>
#include <casper/kernel/solver.h>

namespace minicasper {
struct MExpr;
};

namespace casper {

/** \defgroup Filtering Filtering
 *
 * 	Filtering is accomplished through Filter objects. Filters provide an
 *  (inference) mechanism to narrow domains of variables of a given relation.
 *	\sa Filter, FilterScheduler
 *	\ingroup Kernel
 */

struct Filter;

/**
 * 	Interface for filter objects. Objects derived from this class
 *  provide a mechanism to narrow domains of variables of a given relation.
 *  Each filter has an associated owner which will be notified each time
 *  the filter has something to do.
 *  \ingroup Filtering
 */
struct IFilter : INotifier
{
	/** (Worst case) cost, relative to the number of variables, of a
	 *  single execution of this filter. */
	typedef enum { 	constantLo 		= 0,
		   			constantHi 		= 1,
		   			linearLo 		= 2,
		   			linearHi 		= 3,
		   			quadraticLo		= 4,
		   			quadraticHi		= 5,
		   			cubicLo			= 6,
		   			cubicHi			= 7,
		   			exponentialLo	= 8,
		   			exponentialHi	= 9,
		   			maxCost 		= 10 } Cost;

	static const UInt noQueue;

	/// Constructs a new filter.
	IFilter(ICPSolver& s) : mSolver(s),uInQueue(noQueue)
	{	s.signalNewFilter(this);	}

	/// Method that implements filtering.
	virtual bool execute() = 0;

	/** Sets the owner of the filter. Additionally this method should also
		attach this filter to all relevant notifiers.	*/
	virtual void attach(INotifiable* pOwner) = 0;

	/** Unsets the owner of the filter. Additionally this method should also
	 	remove this filter from all relevant notifiers. */
	virtual void detach(INotifiable* pOwner) = 0;

	/** Method called by notifiers to signal an update. The \p event parameter
	 *  carries information on the type of update. Derived classes should
	 *  overload this method only for storing information about the update.
	 *  \warning The filter must not change any domains inside this method,
	 *  that should be done in the execute() method. */
	bool notify() { assert(0); return false; }

	/** Returns the (worst case) cost, relative to the number of variables,
	 *  of a single execution of this filter. */
	virtual Cost cost() const {	return constantLo; }

	/// Returns the ICPSolver& object associated with this filter.
	virtual ICPSolver&	solver() const	{	return mSolver;	}

	virtual ~IFilter() {}

	// warning: deliberately NOT virtual  (performance)
	void setInQueue(UInt value = noQueue) { uInQueue = value; }

	/** Returns \a true if this filter is scheduled for execution,
	 *  \a false otherwise.	*/
	UInt inQueue()	const		{ return uInQueue;	}

	protected:
	ICPSolver&		mSolver;
	UInt			uInQueue;
};

template<class,class,class>
struct BndFilterView1;

template<class,class,class,class,class>
struct BndFilterView2;

template<class,class,class,class,class,class,class>
struct BndFilterView3;

template<class,class,class,class,class,class,class,class,class>
struct BndFilterView4;

template<class,class,class,class,class,class,class,class,class,class,class>
struct BndFilterView5;

template<class,class,class>
struct DomFilterView1;

template<class,class,class,class,class>
struct DomFilterView2;

template<class,class,class,class,class,class,class>
struct DomFilterView3;


template<class,class,class>
struct BndFilterFact1;

template<class,class,class,class,class>
struct BndFilterFact2;

template<class,class,class,class,class,class,class>
struct BndFilterFact3;

template<class,class,class,class,class,class,class,class,class>
struct BndFilterFact4;

template<class,class,class,class,class,class,class,class,class,class,class>
struct BndFilterFact5;

/**
 *	The handler class for Filters. See IFilter for a complete description
 *  of all members.
 *  \sa IFilter
 *  \ingroup Filtering
 */
struct Filter : detail::PImplIdiom<IFilter>
{
	typedef Filter					Self;
	typedef detail::PImplIdiom<IFilter> 	Super;
	typedef IFilter::Cost			Cost;

	/// Creates a new filter pointing to an existing filter \p f.
	Filter(IFilter*	f) : Super(f) {}

	/// Creates a new filter enforcing an unary relation \p r.
	template<class Func,class T1>
	Filter(const Rel1<Func,T1>& r) :
		Super(BndFilterFact1<Func,
							 typename EvalTraits<T1>::Eval,
							 T1>::create(getCPSolver(r),r.p1).getPImpl()) {}


	/// Creates a new filter enforcing a binary relation \p r.
	template<class Func,class T1,class T2>
	Filter(const Rel2<Func,T1,T2>& r) :
		Super(BndFilterFact2<Func,
							 typename EvalTraits<T1>::Eval,
							 T1,
							 typename EvalTraits<T2>::Eval,
							 T2>::create(getCPSolver(r),r.p1,r.p2).getPImpl()) {}

	/// Creates a new filter enforcing a ternary relation \p r.
	template<class Func,class T1,class T2,class T3>
	Filter(const Rel3<Func,T1,T2,T3>& r) :
		Super(BndFilterFact3<Func,
							 typename EvalTraits<T1>::Eval,
							 T1,
							 typename EvalTraits<T2>::Eval,
							 T2,
							 typename EvalTraits<T3>::Eval,
							 T3>::create(getCPSolver(r),r.p1,r.p2,r.p3).getPImpl()) {}

	/// Creates a new filter enforcing a quaternary relation \p r.
	template<class Func,class T1,class T2,class T3,class T4>
	Filter(const Rel4<Func,T1,T2,T3,T4>& r) :
		Super(BndFilterFact4<Func,
							 typename EvalTraits<T1>::Eval,
							 T1,
							 typename EvalTraits<T2>::Eval,
							 T2,
							 typename EvalTraits<T3>::Eval,
							 T3,
							 typename EvalTraits<T4>::Eval,
							 T4>::create(getCPSolver(r),r.p1,r.p2,r.p3,r.p4).getPImpl()) {}

	/// Creates a new filter enforcing a quaternary relation \p r.
	template<class Func,class T1,class T2,class T3,class T4,class T5>
	Filter(const Rel5<Func,T1,T2,T3,T4,T5>& r) :
		Super(BndFilterFact5<Func,
							 typename EvalTraits<T1>::Eval,
							 T1,
							 typename EvalTraits<T2>::Eval,
							 T2,
							 typename EvalTraits<T3>::Eval,
							 T3,
							 typename EvalTraits<T4>::Eval,
							 T4,
							 typename EvalTraits<T5>::Eval,
							 T5>::create(getCPSolver(r),r.p1,r.p2,r.p3,r.p4,r.p5).getPImpl()) {}

	/** Creates a new filter from a generic relation using a specified
	 *  filter factory. */
	template<class View,class FilterFactory>
	Filter(const View& v, const FilterFactory& f) :
		Super(f(v).getPImpl())	{}

	Filter(BndExpr<Bool> b);

	Filter(ValExpr<Bool> b);

	template<class Dom>
	Filter(DomExpr<Bool,Dom> b);

	template<class Dom>
	Filter(DomVar<Bool,Dom> b);

	Filter(minicasper::MExpr);
	Filter(ICPSolver&,minicasper::MExpr);

	bool execute() {	return Super::pImpl->execute(); }

	void attach(INotifiable* pParent)	{	Super::pImpl->attach(pParent); }
	void detach(INotifiable* pParent)	{	Super::pImpl->detach(pParent); }

	ICPSolver&	solver() const  {	return Super::pImpl->solver();	}
	Cost 	cost() const 	{	return Super::pImpl->cost();	}
};

template<>
struct GetCPSolver<Filter>
{	ICPSolver& operator()(Filter f)
	{ return f.solver(); }
};

template<>
struct EvalTraits<Filter>
{	typedef	Bool	Eval;	};

// FIXME: erase the following checker (minicasper depends on it)
template<class>
struct ChkView;

template<>
struct ChkView<Filter>
{
	ChkView(ICPSolver& solver, const Filter& p1) : p1(p1) {}
	Bool isTrue() const	// is it true?
	{	return false; }
	Bool canBeTrue() const 	// can it still be true?
	{	return true; }
	Bool setToTrue()
	{
		return solver().post(p1);
	}
	Bool setToFalse()
	{
		assert(0); return false;
	}
	ICPSolver& solver() const {	return p1.solver();	}

	void attach(INotifiable* f) { 	}
	void detach(INotifiable* f) {	}

	Filter getView()  const
	{ 	return p1;	}

	Filter p1;
};


};

#endif

#ifndef CASPER_NO_OPERATORS

#include <casper/kernel/variable.h>
#include <casper/kernel/expression.h>

namespace casper {

#ifndef CASPER_KERNEL_MACRO_FILTER_H
#define CASPER_KERNEL_MACRO_FILTER_H
#include <casper/kernel/macro/filter.h>
#endif

}

#endif
