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

#ifndef _M_CASPER_KERNEL_REL_OP_ARRAY
#define _M_CASPER_KERNEL_REL_OP_ARRAY
 
#include <casper/kernel/rel/macros.h>

#define AT_Array(T,D)			Util::StdArray< AT_ ## T,D>
#define AT_ArrayD(T,D)			Util::StdArray< AT_ ## T,D>

#define AD_Array(T,D)			AD_ ## T (int D)
#define AD_ArrayD(T,D)			AD_ ## T

namespace Casper {

REL_FROM_UNARY_SET_OPERATORS(Array(T1,dims))
REL_FROM_COMPARISON_OPERATORS(Array(T1,dims),T)

}
  
#endif // _M_CASPER_KERNEL_REL_OP_ARRAY
