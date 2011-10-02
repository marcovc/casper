/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _M_CASPER_MIP_OP_VAR
#define _M_CASPER_MIP_OP_VAR
 
#include <casper/kernel/rel/macros.h>

#define AT_MIPVar(T)			MIP::Var< AT_ ## T>
#define AD_MIPVar(T)			AD_ ## T 

namespace Casper {

REL_FROM_UNARY_SCALAR_OPERATORS(MIPVar(T1))
REL_FROM_BINARY_OPERATORS(MIPVar(T1),T)
REL_FROM_BINARY_NONBOOL(int,MIPVar(int))
REL_FROM_BINARY_NONBOOL(double,MIPVar(double))
REL_FROM_BINARY_NONBOOL(float,MIPVar(float)) 
REL_FROM_BINARY_OPERATORS(bool,MIPVar(bool))

}

#endif // _M_CASPER_MIP_OP_VAR
