 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_INT_DISTINCT_COMMON
#define _H_CASPER_INT_DISTINCT_COMMON

#include <casper/kernel/rel/rel.h>

namespace Casper {

NEW_REL_1(distinct,Distinct)

namespace Traits {
template<class View>
struct GetEval<Rel1<Distinct,View> >
{	typedef	bool	Type;	};
}; // Traits

namespace CP {

// FIXME: should be elsewhere
template<class Dom>
int findMinElem(VarArray<int,1,Dom> v)
{
	int min = limits<int>::max();
	for (typename VarArray<int,1,Dom>::Iterator it = v.begin();
			it != v.end(); ++it)
		if (it->domain().min() < min)
			min = it->domain().min();
	return min;
}

template<class Dom>
int findMaxElem(VarArray<int,1,Dom> v)
{
	int max = limits<int>::min();
	for (typename VarArray<int,1,Dom>::Iterator it = v.begin();
			it != v.end(); ++it)
		if (it->domain().max() > max)
			max = it->domain().max();
	return max;
}
}; // CP

}; // Casper

#endif
