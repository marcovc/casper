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

namespace Casper {
namespace CP {


struct StoreStats
{
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

	StoreStats() :
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
				countRangeDomainUpdates(0){}

	void signalPropagation()
	{ ++countPropagations; }

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
	counter getNSetDomainUpdates() const {	return countSetDomainUpdates;	}
	counter getNbRangeDomainUpdates() const {	return countRangeDomainUpdates;	}

};


struct PostBndFilter;
extern PostBndFilter postBndFilter;

struct IFilter;


struct Filter;
struct IFilterSched;
template<class> struct BndExpr;

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

	void setFilterWeighting(bool);
	bool getFilterWeighting() const;

	bool post(const Filter&);
	bool post(IFilter*);
	bool post(const BndExpr<bool>& b);

	// Adds a new constraint to the constraint store, enforced by the filter created by f
	template<class Op,class T1,class PostFilter = PostBndFilter>
	bool post(const Rel1<Op,T1>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

	template<class Op,class T1,class T2,class PostFilter = PostBndFilter>
	bool post(const Rel2<Op,T1,T2>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

	template<class Op,class T1,class T2,class T3,class PostFilter = PostBndFilter>
	bool post(const Rel3<Op,T1,T2,T3>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

	template<class Op,class T1,class T2,class T3,class T4,class PostFilter = PostBndFilter>
	bool post(const Rel4<Op,T1,T2,T3,T4>& r, const PostFilter& f = PostFilter())
	{	return f(*this,r);	}

	template<class Cond,class Func>
	bool when(const Cond& cond, const Func& func);

	IFilterSched&	filterSched() {	return *pFilterSched;	}
	void setFilterSched(IFilterSched* p);

	bool valid();
	// not sure what this was for
	//IHeap& sHeap() {	assert(0); return globalSHeap;	}

	const StoreStats& getStats() const
	{	return stats;	}
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

	bool notify()
	{
		env.log(this,"CP::Store",Util::Logger::solverNotify);
		bPending = true;
		return bValid;
	}

	Env&				env;
//	IHeap*				globalSHeap;

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
