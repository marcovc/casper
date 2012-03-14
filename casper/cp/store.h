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
 

#ifndef CASPER_CP_STORE_H_
#define CASPER_CP_STORE_H_

#include <casper/kernel/state/env.h>
#include <casper/kernel/reversible/slist.h>
#include <casper/kernel/notify/notifiable.h>
#include <casper/kernel/rel/rel.h>

#include <casper/util/container/stdhashmap.h>

namespace Casper {

#if defined(SWIG_BUILD)
template<class> struct Expr;
#endif

namespace CP {

/**
 * 	Store statistics.
 * 	\ingroup Filtering
 */
struct StoreStats
{
	Env&	env;

	counter countFPComputations;
	counter countPropagations;
	counter countFilters;

	counter countBoolDomains;
	counter countIntDomains;
	counter countSetDomains;
	counter countRangeDomains;

	counter	countBoolDomainUpdates;
	counter	countIntDomainUpdates;
	counter	countSetDomainUpdates;
	counter	countRangeDomainUpdates;

	#ifdef CASPER_EXTRA_STATS
	counter countEffectivePropagations;
	#endif

	#if defined(CASPER_EXTRA_STATS) and defined(CASPER_LOG)
	const void* lastExecFilterId;
	std::string lastExecFilterName;
	const void* lastExecFilterIdAux;
	Util::StdHashMap<const void*,std::string> filterName;
	Util::StdHashMap<const void*,uint> filterProps;
	Util::StdHashMap<const void*,uint> filterEffProps;
	struct IndividualPropCounts : Util::Logger::Callback
	{
		IndividualPropCounts(StoreStats& rOwner) : rOwner(rOwner) {}
		void operator()(const Util::Logger::Event& ev)
		{
			rOwner.lastExecFilterId = ev.pObj;
			rOwner.lastExecFilterName = ev.strObj;
		}
		StoreStats& rOwner;
	};
	IndividualPropCounts individualPropCounts;
	#endif

	StoreStats(Env& env) :
				env(env),
				countFPComputations(0),
				countPropagations(0),
				countFilters(0),
				countBoolDomains(0),
				countIntDomains(0),
				countSetDomains(0),
				countRangeDomains(0),
				countBoolDomainUpdates(0),
				countIntDomainUpdates(0),
				countSetDomainUpdates(0),
				countRangeDomainUpdates(0)
				#ifdef CASPER_EXTRA_STATS
				,countEffectivePropagations(0)
				,individualPropCounts(*this)
				#endif
	{
		#if defined(CASPER_EXTRA_STATS) and defined(CASPER_LOG)
		env.getLogger().registerCallback(Util::Logger::TagMatcher(Util::Logger::filterExecuteBegin), individualPropCounts);
		filterName[NULL] = "unknown";
		filterProps[NULL] = 0;
		filterEffProps[NULL] = 0;
		#endif
	}

	void signalPropagation()
	{
		++countPropagations;
		#if defined(CASPER_EXTRA_STATS) and defined(CASPER_LOG)
		auto mit = filterProps.find(lastExecFilterId);
		if (mit == filterProps.end())
		{
			filterProps[lastExecFilterId] = 1;
			filterEffProps[lastExecFilterId] = 0;
			filterName[lastExecFilterId] = lastExecFilterName;
		}
		else
			++mit->second;
		lastExecFilterIdAux = lastExecFilterId;
		lastExecFilterId = NULL;
		#endif
	}

	void signalNewFilter()
	{ ++countFilters; }

	void signalFPComputation()
	{ ++countFPComputations; }

	counter	getNbPropagations()	const
	{	return countPropagations;	}

	counter	getNbFilters()	const
	{	return countFilters;	}

	counter	getNbFPComputations()	const
	{	return countFPComputations;	}

	void signalBoolDomainUpdate()	{ countBoolDomainUpdates++; }
	void signalIntDomainUpdate()	{ countIntDomainUpdates++; }
	void signalSetDomainUpdate()	{ countSetDomainUpdates++; }
	void signalRangeDomainUpdate()	{ countRangeDomainUpdates++; }

	void signalNewBoolDomain()	{ countBoolDomains++; }
	void signalNewIntDomain()	{ countIntDomains++; }
	void signalNewSetDomain()	{ countSetDomains++; }
	void signalNewRangeDomain()	{ countRangeDomains++; }

	counter getNbBoolDomains() const {	return countBoolDomains;	}
	counter getNbIntDomains() const {	return countIntDomains;	}
	counter getNSetDomains() const {	return countSetDomains;	}
	counter getNbRangeDomains() const {	return countRangeDomains;	}

	counter getNbBoolDomainUpdates() const {	return countBoolDomainUpdates;	}
	counter getNbIntDomainUpdates() const {	return countIntDomainUpdates;	}
	counter getNbSetDomainUpdates() const {	return countSetDomainUpdates;	}
	counter getNbRangeDomainUpdates() const {	return countRangeDomainUpdates;	}
	counter getNbDomainUpdates() const {	return countBoolDomainUpdates+countIntDomainUpdates+
													countSetDomainUpdates+countRangeDomainUpdates; }

#ifdef CASPER_EXTRA_STATS
	void signalEffectivePropagation()
	{
		++countEffectivePropagations;
		#if defined(CASPER_LOG)
		++filterEffProps[lastExecFilterIdAux];
		lastExecFilterIdAux = NULL;
		#endif
	}
	counter	getNbEffectivePropagations()	const
	{	return countEffectivePropagations;	}
#endif

};


struct PostBndFilter;
extern PostBndFilter postBndFilter;

struct IFilter;


struct Filter;
struct IFilterSched;
template<class> struct BndExpr;
template<class,class> struct Var;

//#if defined(SWIG) | defined(SWIG_BUILD)
enum Consistency { Domain, Bounds, Value };
//#endif


/**
 * 	Stores and propagates constraints.
 * 	\ingroup Filtering
 *
 */
struct Store : INotifiable
{
	typedef SList<IFilter*>	Filters;

	Store(Env& env);
	~Store();

	/// Returns a unique integer for each fixpoint computation
	counter propID() const { return stats.getNbFPComputations(); }

	/**
	 * 	Enables filter weighting which is disabled by default. Filter
	 * 	weighting must be enabled when posting a new constraint in order
	 * 	to perform accumulated failure count (AFC) for the corresponding propagator,
	 * 	which is required by some heuristics.
	 */
	void setFilterWeighting(bool);
	/// Returns whether filter weighting is enabled.
	bool getFilterWeighting() const;

#ifndef SWIG
	/// Add a new filter to the store.
	bool post(const Filter&);
	bool post(IFilter*);
	bool post(const BndExpr<bool>& b);
#endif

//#if defined(SWIG) | defined(SWIG_BUILD)
	bool post(const Casper::Expr<bool>& expr,
			  Casper::CP::Consistency consistency = Casper::CP::Bounds);
//#endif

#ifndef _MSC_VER

	template<class PostFilter = PostBndFilter>
	bool post(bool b,const PostFilter& f = PostFilter()) { return b; }

	template<class Dom,class PostFilter = PostBndFilter>
	bool post(const Var<bool,Dom>& v,const PostFilter& f = PostFilter())
	{	return f(*this,v==true);	}

	/// Adds a new constraint to the constraint store, enforced by the filter created by f
	template<class Op,class T1,class PostFilter = PostBndFilter>
	bool post(const Rel1<Op,T1>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

	/// Adds a new constraint to the constraint store, enforced by the filter created by f
	template<class Op,class T1,class T2,class PostFilter = PostBndFilter>
	bool post(const Rel2<Op,T1,T2>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

	/// Adds a new constraint to the constraint store, enforced by the filter created by f
	template<class Op,class T1,class T2,class T3,class PostFilter = PostBndFilter>
	bool post(const Rel3<Op,T1,T2,T3>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

	/// Adds a new constraint to the constraint store, enforced by the filter created by f
	template<class Op,class T1,class T2,class T3,class T4,class PostFilter = PostBndFilter>
	bool post(const Rel4<Op,T1,T2,T3,T4>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

	/// Adds a new constraint to the constraint store, enforced by the filter created by f
	template<class Op,class T1,class T2,class T3,class T4,class T5,class PostFilter = PostBndFilter>
	bool post(const Rel5<Op,T1,T2,T3,T4,T5>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

#else
	/// Adds a new constraint to the constraint store.
	template<class Op,class T1>
	bool post(const Rel1<Op,T1>& r)
	{	return postBndFilter(*this,r);	}

	/// Adds a new constraint to the constraint store.
	template<class Op,class T1,class T2>
	bool post(const Rel2<Op,T1,T2>& r)
	{	return postBndFilter(*this,r);	}

	/// Adds a new constraint to the constraint store.
	template<class Op,class T1,class T2,class T3>
	bool post(const Rel3<Op,T1,T2,T3>& r)
	{	return postBndFilter(*this,r);	}

	/// Adds a new constraint to the constraint store.
	template<class Op,class T1,class T2,class T3,class T4>
	bool post(const Rel4<Op,T1,T2,T3,T4>& r)
	{	return postBndFilter(*this,r);	}

	/// Adds a new constraint to the constraint store, enforced by the filter created by f
	template<class Op,class T1,class PostFilter>
	bool post(const Rel1<Op,T1>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

	/// Adds a new constraint to the constraint store, enforced by the filter created by f
	template<class Op,class T1,class T2,class PostFilter>
	bool post(const Rel2<Op,T1,T2>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

	/// Adds a new constraint to the constraint store, enforced by the filter created by f
	template<class Op,class T1,class T2,class T3,class PostFilter>
	bool post(const Rel3<Op,T1,T2,T3>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

	/// Adds a new constraint to the constraint store, enforced by the filter created by f
	template<class Op,class T1,class T2,class T3,class T4,class PostFilter>
	bool post(const Rel4<Op,T1,T2,T3,T4>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

	/// Adds a new constraint to the constraint store, enforced by the filter created by f
	template<class Op,class T1,class T2,class T3,class T4,class T5,class PostFilter>
	bool post(const Rel5<Op,T1,T2,T3,T4,T5>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

#endif
	template<class Cond,class Func>
	bool when(const Cond& cond, const Func& func);

	IFilterSched&	filterSched() {	return *pFilterSched;	}
	void setFilterSched(IFilterSched* p);

	/// Applies filters for all constraints in the store.
	bool valid();
	// not sure what this was for
	//IHeap& sHeap() {	assert(0); return globalSHeap;	}

	/// Returns store statistics.
	const StoreStats& getStats() const
	{	return stats;	}
	/// Returns store statistics.
	StoreStats& getStats()
	{	return stats;	}

	const Filters&	getFilters() const {	return filters;	}

	Env& getEnv() {	return env;	}
	const Env& getEnv() const {	return env;	}

	operator Env&() { return env; }
	operator const Env&() const { return env; }

	operator State&();
	operator const State&() const;

	operator Util::IHeap&();
	operator const Util::IHeap&() const;

	Util::IHeap& getStdHeap() {	return *globalSHeap; }
	const Util::IHeap& getStdHeap() const {	return *globalSHeap; }

	bool notify()
	{
		env.log(this,"CP::Store",Util::Logger::solverNotify);
		bPending = true;
		return bValid;
	}

protected:
	Env&				env;
	Util::IHeap*		globalSHeap;

	StoreStats			stats;
//	counter				propIDCtr;
	IFilterSched*		pFilterSched;
	bool				weightFilters;
	Filters				filters;

	Reversible<bool> 	bValid;
	Reversible<bool> 	bPending;
};


} // CP
} // Casper

std::ostream& operator<<(std::ostream& os, const Casper::CP::StoreStats& s);

#endif /* CASPER_KERNEL_CPSTORE_H_ */
