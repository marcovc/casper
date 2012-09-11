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
 

#include <casper/cp/store.h>
#include <casper/cp/filter/common.h>
#include <casper/cp/scheduler.h>
#include <casper/cp/filter/weightedfilter.h>
#include <casper/cp/filter/bnd.h>

#include <casper/kernel/spexpr/expr.h>

#include <casper/util/container/stdarray.h>

using namespace std;

#define CASPER_SHEAP_INIT_SIZE		static_cast<uint>(1024)
#define CASPER_SHEAP_GROW_RATIO		1.5

ostream& operator<<(ostream& os, const Casper::CP::StoreStats& s)
{
	os  << left << setw (30) << "Number of Boolean domains" << ":" <<  std::setw (10) << std::right
							  << s.getNbBoolDomains() << std::endl
	   << left << setw (30) << "Number of integer domains" << ":" <<  std::setw (10) << std::right
							  << s.getNbIntDomains() << std::endl
	   << left << setw (30) << "Number of set domains" << ":" <<  std::setw (10) << std::right
							  << s.getNSetDomains() << std::endl
	   << left << setw (30) << "Number of range domains" << ":" <<  std::setw (10) << std::right
							  << s.getNbRangeDomains() << std::endl
	   << left << setw (30) << "Number of filters" << ":" <<  std::setw (10) << std::right
							  << s.getNbFilters() << std::endl
	   << left << setw (30) << "Number of filter executions" << ":" <<  std::setw (10) << std::right
							  << s.getNbPropagations() << std::endl
#ifdef CASPER_EXTRA_STATS
	   << left << setw (30) << "Number of effective filter executions" << ":" <<  std::setw (10) << std::right
							  << s.getNbEffectivePropagations() << " ("
							  << (s.getNbEffectivePropagations()*100.0/s.getNbPropagations()) << "%)" << std::endl
#endif

	   << left << setw (30) << "Number of fixpoints" << ":" <<  std::setw (10) << std::right
							  << s.getNbFPComputations();
#ifdef CASPER_EXTRA_STATS
	   os << endl << left << setw (30) << "Number of Boolean domain updates" << ":" <<  std::setw (10) << std::right
							  << s.getNbBoolDomainUpdates()
	   << endl << left << setw (30) << "Number of integer domain updates" << ":" <<  std::setw (10) << std::right
							  << s.getNbIntDomainUpdates()
	   << endl << left << setw (30) << "Number of set domain updates" << ":" <<  std::setw (10) << std::right
							  << s.getNbSetDomainUpdates()
	   << endl << left << setw (30) << "Number of range domain updates" << ":" <<  std::setw (10) << std::right
							  << s.getNbRangeDomainUpdates();

#if defined(CASPER_LOG)

#if 0 // detailed
	   os << endl;
	   for (auto it = s.filterProps.begin(); it != s.filterProps.end(); ++it)
	   {
		   auto eit = s.filterName.find(it->first);
		   assert(eit!=s.filterName.end());
		   os << left << setw (30) << "Number of executions of " << eit->second << "@" << it->first <<  ":" <<  std::setw (10) << std::right
								  << it->second << std::endl;
	   }
	   for (auto it = s.filterEffProps.begin(); it != s.filterEffProps.end(); ++it)
	   {
		   auto eit = s.filterName.find(it->first);
		   assert(eit!=s.filterName.end());
		   os << left << setw (30) << "Number of effective executions of " << eit->second << "@" << it->first <<  ":" <<  std::setw (10) << std::right
								  << it->second << std::endl;
	   }
#else
	   os << endl;
	   std::set<std::string> filterNames;
	   for (auto it = s.filterName.begin(); it != s.filterName.end(); ++it)
		   filterNames.insert(it->second);
	   for (auto nit = filterNames.begin(); nit != filterNames.end(); ++nit)
	   {
		   std::string xmlfriendlyname = *nit;
		   for (auto ait = xmlfriendlyname.begin(); ait != xmlfriendlyname.end(); ++ait)
			   if (*ait == '<')
				   *ait = '(';
			   else
			   if (*ait == '>')
				   *ait = ')';

		   uint execs = 0;
		   uint eexecs = 0;
		   int np = 5;
		   Casper::Util::StdArray<int> execp(np,0);
		   Casper::Util::StdArray<int> eexecp(np,0);
		   for (auto it = s.filterProps.begin(); it != s.filterProps.end(); ++it)
		   {
			   auto eit = s.filterName.find(it->first);
			   if (eit->second == *nit)
			   {
				   uint fexecs = it->second;
				   uint feexecs = s.filterEffProps.find(it->first)->second;
				   execs += fexecs;
				   eexecs += feexecs;
				   for (uint i = 0; i < np; ++i)
				   {
					   if (fexecs >= std::pow(2.0,i))
						   ++execp[i];
					   if (feexecs >= std::pow(2.0,i))
						   ++eexecp[i];
				   }
			   }
		   }
		   os << left << setw (30) << "Number of executions of " << xmlfriendlyname <<  ":" <<  std::setw (10) << std::right
								  << execs << " " << execp << std::endl;
		   os << left << setw (30) << "Number of effective executions of " << xmlfriendlyname <<  ":" <<  std::setw (10) << std::right
								  << eexecs << " " << eexecp << std::endl;

	   }
#endif


#endif

#endif
	return os;
}

namespace Casper {
namespace CP {

Store::Store(Env& env) :
			env(env),
			globalSHeap(new Util::DynamicHeap(CASPER_SHEAP_INIT_SIZE,
											  CASPER_SHEAP_GROW_RATIO)),
			stats(env),
//			propIDCtr(0),
		    weightFilters(false),
		    filters(env),
		    bValid(env,true),
		    bPending(env,false)
			#ifdef CASPER_LOG
			,eg(this)
			#endif
{
	pFilterSched = new (env) FullyPreemptCostFilterSched<FIFOFilterSched>(*this);
}

Store::~Store()
{
//	delete pFilterSched;
//	delete globalSHeap;
}

void Store::setFilterSched(IFilterSched* p)
{
	pFilterSched = p;
	p->attach(this);
}


bool Store::post(const Filter& f)
{
	CASPER_AT_STORE_POST_ENTER(eg);
	filters.pushFront(f.getPImpl());

	if (!pFilterSched->post(f))
	{
		bPending = false;
		bValid = false;
		CASPER_AT_STORE_POST_LEAVE(eg);
		return false;
	}
	CASPER_AT_STORE_POST_LEAVE(eg);
	return true;
}

bool Store::post(IFilter* f)
{
	filters.pushFront(f);

	if (!pFilterSched->post(f))
	{
		bPending = false;
		bValid = false;
		return false;
	}
	return true;
}

bool Store::post(const BndExpr<bool>& b)
{
	return post(new (*this) BndFilterView<BndExpr<bool> >(*this,b));
}

void Store::setFilterWeighting(bool val)
{	weightFilters = val;	}

bool Store::getFilterWeighting() const
{	return weightFilters;	}

bool Store::valid()
{
	CASPER_AT_STORE_FP_ENTER(eg);
	if (bPending)
	{
		bValid = pFilterSched->execute();
		bPending = false;
		stats.signalFPComputation();
	}
	CASPER_AT_STORE_FP_LEAVE(eg);
	return bValid;
}

Store::operator State&() { return env.getState(); }
Store::operator const State&() const { return env.getState(); }

Store::operator Util::IHeap&() { return env.getState().getHeap(); }
Store::operator const Util::IHeap&() const { return env.getState().getHeap(); }

Util::IHeap& Store::getHeap() { return env.getState().getHeap(); }
const Util::IHeap& Store::getHeap() const { return env.getState().getHeap(); }

bool Store::post(const Casper::Expr<bool>& expr,
		  Casper::CP::Consistency consistency)
{
	switch (consistency)
	{
		case Casper::CP::Domain:
			return expr.postDomFilter(*this);
		case Casper::CP::Bounds:
			return expr.postBndFilter(*this);
		case Casper::CP::Value:
			return expr.postValFilter(*this);
		default:
			throw Casper::Exception::InvalidOperation("invalid value for consistency parameter");
	}
}


struct LimitFPs : ISinglePathExplorer
{
	LimitFPs(CP::Store& store,uint n, ISinglePathExplorer* search) :
		ISinglePathExplorer(store),
		store(store),
		n(n+store.getStats().getNbFPComputations()),search(search) {}
	bool discard(bool atSolution)
	{
		if (store.getStats().getNbFPComputations() > n)
			return true;
		else
			return search->discard(atSolution);
	}
	void reset(uint n)
	{	this->n = store.getStats().getNbFPComputations()+n; }

	CP::Store& store;
	counter n;
	ISinglePathExplorer* search;
};

IExplorer* limitFPs(CP::Store& store,uint n, ISinglePathExplorer* s)
{
	return new (store) LimitFPs(store,n,s);
}

} // CP


} // Casper

