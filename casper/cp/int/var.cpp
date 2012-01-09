 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <casper/cp/var.h>
#include <casper/cp/int/var.h>
#include <casper/cp/int/fd.h>

namespace Casper {
namespace CP {

#ifdef CASPER_PRECOMPILED
template struct Var<int>;
template struct Var<bool>;
template struct VarArray<int>;
template struct VarArray<bool>;
template struct VarArray<int,2>;
template struct VarArray<bool,2>;
template struct VarArray<int,3>;
template struct VarArray<bool,3>;
template struct VarArray<int,4>;
template struct VarArray<bool,4>;
#endif

} // CP
} // Casper


