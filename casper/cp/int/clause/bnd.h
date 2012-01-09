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
 

#ifndef CASPER_CP_INT_CLAUSE_BND_H_
#define CASPER_CP_INT_CLAUSE_BND_H_

#include <casper/cp/int/clause/common.h>

//#ifdef CASPER_PRECOMPILED
//#include <casper/cp/int/spexpr/explicit_postbnd.h>
//#endif

namespace Casper {
namespace CP {

template<class ArrayView>
struct PostBndFilter1<Or,Seq<bool>,ArrayView>
{
	static bool post(Store& store,const ArrayView& a)
	{	return new (store) Detail::Clause<BndArrayView<bool,ArrayView> >(store,a);	}
};

} // CP
} // Casper

#endif /* CASPER_CP_INT_CLAUSE_DOM_H_ */
