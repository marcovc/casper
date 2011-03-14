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

#define CASPER_SHEAP_INIT_SIZE		static_cast<uint>(1024)
#define CASPER_SHEAP_GROW_RATIO		1.5

ostream& operator<<(ostream& os, const Casper::CP::StoreStats& s)
{
	os  << left << setw (30) << "Number of filters" << ":" <<  std::setw (10) << std::right
							  << s.getNbFilters() << std::endl
	   << left << setw (30) << "Number of filter executions" << ":" <<  std::setw (10) << std::right
							  << s.getNbPropagations() << std::endl
	   << left << setw (30) << "Number of fixpoints" << ":" <<  std::setw (10) << std::right
							  << s.getNbFPComputations();
	return os;
}

namespace Casper {
namespace CP {

Store::Store(Env& env) :
			env(env),
//			globalSHeap(new DynamicHeap(CASPER_SHEAP_INIT_SIZE,
//										CASPER_SHEAP_GROW_RATIO)),
//			propIDCtr(0),
		    weightFilters(false),
		    filters(env),
		    bValid(env,true),
		    bPending(env,false)
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
	filters.pushFront(f.getPImpl());

	if (!pFilterSched->post(f))
	{
		bPending = false;
		bValid = false;
		return false;
	}
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

void Store::setFilterWeighting(bool val)
{	weightFilters = val;	}

bool Store::getFilterWeighting() const
{	return weightFilters;	}

bool Store::valid()
{
	if (bPending)
	{
		bPending = false;
		bValid = pFilterSched->execute();
		stats.signalFPComputation();
	}
	return bValid;
}

Store::operator State&() { return env.getState(); }
Store::operator const State&() const { return env.getState(); }

Store::operator Util::IHeap&() { return env.getState().getHeap(); }
Store::operator const Util::IHeap&() const { return env.getState().getHeap(); }

} // CP
} // Casper

