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

#ifndef _M_CASPER_KERNEL_MACRO_RELATION
#define _M_CASPER_KERNEL_MACRO_RELATION
 
#include <casper/kernel/macro/engine.m>

REL_FROM_UNARY_OPERATORS(Rel0(Op))
REL_FROM_BINARY_OPERATORS(Rel0(Op),T)
REL_FROM_BINARY_OPERATORS(Int,Rel0(Op))
REL_FROM_BINARY_OPERATORS(Bool,Rel0(Op))
REL_FROM_BINARY_OPERATORS(Double,Rel0(Op))

REL_FROM_UNARY_OPERATORS(Rel1(Op,T1))
REL_FROM_BINARY_OPERATORS(Rel1(Op,T1),T)
REL_FROM_BINARY_OPERATORS(Int,Rel1(Op,T1))
REL_FROM_BINARY_OPERATORS(Bool,Rel1(Op,T1))
REL_FROM_BINARY_OPERATORS(Double,Rel1(Op,T1))

REL_FROM_UNARY_OPERATORS(Rel2(Op,T1,T2))
REL_FROM_BINARY_OPERATORS(Rel2(Op,T1,T2),T)
REL_FROM_BINARY_OPERATORS(Int,Rel2(Op,T1,T2))
REL_FROM_BINARY_OPERATORS(Bool,Rel2(Op,T1,T2))
REL_FROM_BINARY_OPERATORS(Double,Rel2(Op,T1,T2))

REL_FROM_UNARY_OPERATORS(Rel3(Op,T1,T2,T3))
REL_FROM_BINARY_OPERATORS(Rel3(Op,T1,T2,T3),T)
REL_FROM_BINARY_OPERATORS(Int,Rel3(Op,T1,T2,T3))
REL_FROM_BINARY_OPERATORS(Bool,Rel3(Op,T1,T2,T3))
REL_FROM_BINARY_OPERATORS(Double,Rel3(Op,T1,T2,T3))

REL_FROM_UNARY_OPERATORS(Rel4(Op,T1,T2,T3,T4))
REL_FROM_BINARY_OPERATORS(Rel4(Op,T1,T2,T3,T4),T)
REL_FROM_BINARY_OPERATORS(Int,Rel4(Op,T1,T2,T3,T4))
REL_FROM_BINARY_OPERATORS(Bool,Rel4(Op,T1,T2,T3,T4))
REL_FROM_BINARY_OPERATORS(Double,Rel4(Op,T1,T2,T3,T4))
  
#endif // _M_CASPER_KERNEL_MACRO_RELATION
