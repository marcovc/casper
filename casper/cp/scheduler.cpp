/**************************************************************************\
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

#include <casper/cp/scheduler.h>

namespace Casper {
namespace CP {


GreedyLIFOFilterSched* greedyLIFOFilterSched(Store& store,bool suspend)
{
	return new (store) GreedyLIFOFilterSched(store,suspend);
}

ULIFOFilterSched* ulifoFilterSched(Store& store,bool suspend)
{
	return new (store) ULIFOFilterSched(store,suspend);
}

LIFOFilterSched* lifoFilterSched(Store& store,bool suspend)
{
	return new (store) LIFOFilterSched(store,suspend);
}

FIFOFilterSched* fifoFilterSched(Store& store,bool suspend)
{
	return new (store) FIFOFilterSched(store,suspend);
}

TwoCostFilterSched* twoCostFilterSched(Store& store,IFilter::Cost maxFirstStageCost)
{
	return new (store) TwoCostFilterSched(store,maxFirstStageCost);
}

CostFilterSched<TwoCostFilterSched>* costFilterSched(Store& store)
{
	return new (store) CostFilterSched<TwoCostFilterSched>(store);
}


} // CP
} // Casper
