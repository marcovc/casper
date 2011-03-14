/**************************************************************************\
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

#ifndef _M_CASPER_CP_REL_OP_VAR
#define _M_CASPER_CP_REL_OP_VAR
 
#include <casper/kernel/rel/macros.h>

#define AT_CPVar(T,M)			CP::Var< AT_ ## T,AT_ ## M >
#define AD_CPVar(T,M)			AD_ ## T AD_ ## M

namespace Casper {

REL_FROM_UNARY_SCALAR_OPERATORS(CPVar(T1,T2))
REL_FROM_BINARY_OPERATORS(CPVar(T1,T2),T)
REL_FROM_BINARY_NONBOOL(int,CPVar(int,T2))
REL_FROM_BINARY_NONBOOL(double,CPVar(double,T2))
REL_FROM_BINARY_NONBOOL(float,CPVar(float,T2)) 
REL_FROM_BINARY_OPERATORS(bool,CPVar(bool,T2))

}

#endif // _M_CASPER_CP_REL_OP_VAR
