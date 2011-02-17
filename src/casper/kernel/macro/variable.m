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

#ifndef _M_CASPER_KERNEL_MACRO_VARIABLE
#define _M_CASPER_KERNEL_MACRO_VARIABLE
 
#include <casper/kernel/macro/engine.m>

REL_FROM_UNARY_OPERATORS(DomVar(T1,T2))
REL_FROM_BINARY_OPERATORS(DomVar(T1,T2),T)
REL_FROM_BINARY_OPERATORS(Int,DomVar(Int,T2))
REL_FROM_BINARY_OPERATORS(Bool,DomVar(Bool,T2))
REL_FROM_BINARY_OPERATORS(Double,DomVar(Double,T2)) 

REL_FROM_UNARY_OPERATORS(MutVar(T1))
REL_FROM_BINARY_OPERATORS(MutVar(T1),T)
REL_FROM_BINARY_OPERATORS(Int,MutVar(Int))
REL_FROM_BINARY_OPERATORS(Bool,MutVar(Bool))

#endif
