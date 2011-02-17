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
 

#include <casper/kernel/susplist.h>
#include <casper/kernel/filter/weightedfilter.h>
#include <casper/kernel/scheduler.h>

namespace casper {

struct WeightedFilter;

UInt EventSuspList::getNbFilters() const
{
	UInt ret = 0;
	for (SList::Iterator it = l.begin(); it != l.end(); ++it)
		ret += (dynamic_cast<FIFOFilterSched::FilterDemon*>(*it)!=NULL) or
			   (dynamic_cast<FIFOFilterSched::WeightedFilterDemon*>(*it)!=NULL);
	return ret;
}

UInt EventSuspList::getNbWeightedFilters() const
{
	UInt ret = 0;
	for (SList::Iterator it = l.begin(); it != l.end(); ++it)
		ret += (dynamic_cast<FIFOFilterSched::WeightedFilterDemon*>(*it)!=NULL);
	return ret;
}

UInt EventSuspList::getTotalAFC() const
{
	UInt ret = 0;
	for (SList::Iterator it = l.begin(); it != l.end(); ++it)
	{
		FIFOFilterSched::WeightedFilterDemon* pf = dynamic_cast<FIFOFilterSched::WeightedFilterDemon*>(*it);
		if (pf != NULL)
			ret += pf->getAFC();
	}
	return ret;
}

}
