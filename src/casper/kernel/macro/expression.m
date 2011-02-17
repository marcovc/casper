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

#ifndef _M_CASPER_KERNEL_MACRO_EXPRESSION
#define _M_CASPER_KERNEL_MACRO_EXPRESSION
 
#include <casper/kernel/macro/engine.m>


REL_FROM_UNARY_OPERATORS(ValExpr(T1))
REL_FROM_BINARY_OPERATORS(ValExpr(T1),T)
REL_FROM_BINARY_OPERATORS(Int,ValExpr(T1))
REL_FROM_BINARY_OPERATORS(Bool,ValExpr(T1))
REL_FROM_BINARY_OPERATORS(Double,ValExpr(T1))
REL_FROM_UNARY_OPERATORS(BndExpr(T1))
REL_FROM_BINARY_OPERATORS(BndExpr(T1),T)
REL_FROM_BINARY_OPERATORS(Int,BndExpr(T1))
REL_FROM_BINARY_OPERATORS(Bool,BndExpr(T1))
REL_FROM_BINARY_OPERATORS(Double,BndExpr(T1))
REL_FROM_UNARY_OPERATORS(DomExpr(T1,T2))
REL_FROM_BINARY_OPERATORS(DomExpr(T1,T2),T)
REL_FROM_BINARY_OPERATORS(Int,DomExpr(T1,T2))
REL_FROM_BINARY_OPERATORS(Bool,DomExpr(T1,T2))
REL_FROM_UNARY_OPERATORS(MutExpr(T1))
REL_FROM_BINARY_OPERATORS(MutExpr(T1),T)
REL_FROM_BINARY_OPERATORS(Int,MutExpr(T1))
REL_FROM_BINARY_OPERATORS(Bool,MutExpr(T1))
REL_FROM_BINARY_OPERATORS(Double,MutExpr(T1))
  
#endif /*_M_CASPER_KERNEL_MACRO_EXPRESSION*/
