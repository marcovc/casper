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

/** \file
 * 	Filter objects. Filters provide an (inference) mechanism to
 *  narrow domains of variables of a given relation.
 *	\sa FilterScheduler
 */

#ifndef _H_CASPER_CP_FILTER_COMMON
#define _H_CASPER_CP_FILTER_COMMON

#include <casper/util/pimpl.h>
#include <casper/kernel/notify/notifiable.h>
#include <casper/kernel/notify/notifier.h>
#include <casper/kernel/rel/rel.h>
#include <casper/kernel/common.h>
#include <casper/cp/store.h>

namespace Casper {


namespace CP {

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

	static const uint noQueue;

	/// Constructs a new filter.
	IFilter(Store& s);

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

	/// Returns the CPSolver& object associated with this filter.
	//virtual CPSolver&	solver() const	{	return mSolver;	}

	virtual ~IFilter() {}

	// warning: deliberately NOT virtual  (performance)
	void setInQueue(uint value = noQueue) { uInQueue = value; }

	/** Returns \a true if this filter is scheduled for execution,
	 *  \a false otherwise.	*/
	uint inQueue()	const		{ return uInQueue;	}

	protected:
	//CPSolver&		mSolver;
	uint			uInQueue;
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
struct PostBndFilter1;

template<class,class,class,class,class>
struct PostBndFilter2;

template<class,class,class,class,class,class,class>
struct PostBndFilter3;

template<class,class,class,class,class,class,class,class,class>
struct PostBndFilter4;

template<class,class,class,class,class,class,class,class,class,class,class>
struct PostBndFilter5;

struct PostBndFilter;
extern PostBndFilter postBndFilter;

/**
 *	The handler class for Filters. See IFilter for a complete description
 *  of all members.
 *  \sa IFilter
 *  \ingroup Filtering
 */
struct Filter : Util::PImplIdiom<IFilter>
{
	typedef Filter					Self;
	typedef Util::PImplIdiom<IFilter> 	Super;
	typedef IFilter::Cost			Cost;

	/// Creates a new filter pointing to an existing filter \p f.
	Filter(IFilter*	f) : Super(f) {}

	/** Creates a new filter from a generic relation using a specified
	 *  filter factory. */
//	template<class View,class FilterFactory = PostBndFilter>
//	Filter(Store& s,const View& v, const FilterFactory& f = FilterFactory()) :
//		Super(f(s,v).getPImpl())	{}

/*
	Filter(CPSolver& s,ValExpr<bool> b);

	template<class Dom>
	Filter(CPSolver& s,DomExpr<bool,Dom> b);

	template<class Dom>
	Filter(CPSolver& s,Var<bool,Dom> b);
*/
	bool execute() {	return Super::pImpl->execute(); }

	void attach(INotifiable* pParent)	{	Super::pImpl->attach(pParent); }
	void detach(INotifiable* pParent)	{	Super::pImpl->detach(pParent); }

//	CPSolver&	solver() const  {	return Super::pImpl->solver();	}
	Cost 	cost() const 	{	return Super::pImpl->cost();	}
};


// FIXME: erase the following checker (minicasper depends on it)
template<class>
struct ChkView;

template<>
struct ChkView<Filter>
{
	ChkView(Store& store, const Filter& p1) : p1(p1) {}
	bool isTrue() const	// is it true?
	{	return false; }
	bool canBeTrue() const 	// can it still be true?
	{	return true; }
	bool setToTrue();
	bool setToFalse()
	{
		assert(0); return false;
	}
//	CPSolver& solver() const {	return p1.solver();	}

	void attach(INotifiable* f) { 	}
	void detach(INotifiable* f) {	}

	Filter getObj()  const
	{ 	return p1;	}

	Filter p1;
};

};	// CP

//template<class>
//struct GetPState;
//
//template<>
//struct GetPState<CP::Filter>
//{	CPSolver& operator()(Filter f)	{ return f.solver(); }	};

//namespace Traits {
//template<>	struct GetEval<CP::Filter>	{	typedef	bool	Type;	};
//}; // Traits

}; // Casper

#endif

