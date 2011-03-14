/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2010-2010 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#include <casper/kernel/notify/susplist.h>
#include <casper/cp/filter/weightedfilter.h>
#include <casper/cp/scheduler.h>

namespace Casper {

struct WeightedFilter;

uint EventSuspList::getNbFilters() const
{
	uint ret = 0;
	for (auto it = l.begin(); it != l.end(); ++it)
		ret += (dynamic_cast<typename CP::FIFOFilterSched::FilterDemon*>(*it)!=NULL) or
			   (dynamic_cast<typename CP::FIFOFilterSched::WeightedFilterDemon*>(*it)!=NULL);
	return ret;
}

uint EventSuspList::getNbWeightedFilters() const
{
	uint ret = 0;
	for (auto it = l.begin(); it != l.end(); ++it)
		ret += (dynamic_cast<typename CP::FIFOFilterSched::WeightedFilterDemon*>(*it)!=NULL);
	return ret;
}

uint EventSuspList::getTotalAFC() const
{
	uint ret = 0;
	for (auto it = l.begin(); it != l.end(); ++it)
	{
		CP::FIFOFilterSched::WeightedFilterDemon* pf = dynamic_cast<typename CP::FIFOFilterSched::WeightedFilterDemon*>(*it);
		if (pf != NULL)
			ret += pf->getAFC();
	}
	return ret;
}

}
