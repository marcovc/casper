 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_CP_FILTER_WEIGHTEDFILTER_H_
#define CASPER_CP_FILTER_WEIGHTEDFILTER_H_

#include <casper/cp/filter/common.h>
#include <casper/cp/store.h>

namespace Casper {
namespace CP {

#if 1
// weighted filter
struct WeightedFilter : IFilter
{
	WeightedFilter(Store& store,Filter f) : IFilter(store),
							f(f),
							weight(1) {}
	bool execute()
	{
		if (!f.execute())
		{
			 ++weight;
			return false;
		}
		return true;
	}
	Cost cost() const
	{	return f.cost();	}
	uint getWeight() const
	{	return weight;	}
	void attach(INotifiable* n)
	{	f.attach(n);	}
	void detach(INotifiable* n)
	{	f.detach(n);	}

	Filter	f;
	uint	weight;
};

inline Filter weightedFilter(Store& store,Filter f)
{	return new (store) WeightedFilter(store,f);	}
#endif

} // CP
} // Casper

#endif /* CASPER_KERNEL_FILTER_WEIGHTEDFILTER_H_ */
