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
 

#ifndef CASPER_LP_VARARRAY_H_
#define CASPER_LP_VARARRAY_H_

#include <casper/util/container/stdarray.h>

namespace Casper {
namespace LP {

namespace Detail {
template<int dims>
struct VarArray : Util::StdArray<Var,dims>
{
	VarArray(Solver& solver, uint n) : Util::StdArray<Var,dims>(solver,n,solver) {}
	VarArray(Solver& solver, uint n1, uint n2) : Util::StdArray<Var,dims>(solver,n1,n2,solver) {}
	VarArray(Solver& solver, uint n1, uint n2, uint n3) : Util::StdArray<Var,dims>(solver,n1,n2,n3,solver) {}
};
} // Detail

typedef Casper::LP::Detail::VarArray<1>	VarArray;
typedef Casper::LP::Detail::VarArray<2>	VarArray2;
typedef Casper::LP::Detail::VarArray<3>	VarArray3;
}
}


#endif /* CASPER_LP_VARARRAY_H_ */
