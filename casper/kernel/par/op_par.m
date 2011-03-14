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

#ifndef _M_CASPER_KERNEL_PAR_OP_PAR
#define _M_CASPER_KERNEL_PAR_OP_PAR
 
#include <casper/kernel/rel/macros.h>

#define AT_Par(T)			Par< AT_ ## T >
#define AD_Par(T)			AD_ ## T

namespace Casper {

REL_FROM_UNARY_OPERATORS(Par(T1))
REL_FROM_BINARY_OPERATORS(Par(T1),T)

REL_FROM_ARITH_OPERATORS(int,Par(int))
REL_FROM_COMPARISON_OPERATORS(int,Par(int))

REL_FROM_ARITH_OPERATORS(double,Par(double))
REL_FROM_COMPARISON_OPERATORS(double,Par(double))

REL_FROM_ARITH_OPERATORS(float,Par(float))
REL_FROM_COMPARISON_OPERATORS(float,Par(float))

REL_FROM_BINARY_OPERATORS(bool,Par(bool))

} // Casper

#endif
