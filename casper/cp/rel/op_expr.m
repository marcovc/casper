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

#ifndef _M_CASPER_CP_REL_OP_EXPR
#define _M_CASPER_CP_REL_OP_EXPR
 
#include <casper/kernel/rel/macros.h>

#define AT_ValExpr(M1)			CP::ValExpr< AT_ ## M1 >
#define AT_BndExpr(M1)			CP::BndExpr< AT_ ## M1 >
#define AT_DomExpr(M1,M2)		CP::DomExpr< AT_ ## M1,AT_ ## M2>

#define AD_ValExpr(M1)			AD_ ## M1
#define AD_BndExpr(M1)			AD_ ## M1
#define AD_DomExpr(M1,M2)		AD_ ## M1 AD_ ## M2

namespace Casper {

REL_FROM_UNARY_OPERATORS(ValExpr(T1))
REL_FROM_BINARY_OPERATORS(ValExpr(T1),T)

REL_FROM_BINARY_NONBOOL(int,ValExpr(T1))
REL_FROM_BINARY_NONBOOL(double,ValExpr(T1))
REL_FROM_BINARY_NONBOOL(float,ValExpr(T1))
REL_FROM_BINARY_OPERATORS(bool,ValExpr(T1))

REL_FROM_UNARY_OPERATORS(BndExpr(T1))
REL_FROM_BINARY_OPERATORS(BndExpr(T1),T)
REL_FROM_BINARY_NONBOOL(int,BndExpr(T1))
REL_FROM_BINARY_NONBOOL(double,BndExpr(T1))
REL_FROM_BINARY_NONBOOL(float,BndExpr(T1))
REL_FROM_BINARY_OPERATORS(bool,BndExpr(T1))

REL_FROM_UNARY_OPERATORS(DomExpr(T1,T2))
REL_FROM_BINARY_OPERATORS(DomExpr(T1,T2),T)
REL_FROM_BINARY_NONBOOL(int,DomExpr(T1,T2))
REL_FROM_BINARY_NONBOOL(float,DomExpr(T1,T2))
REL_FROM_BINARY_NONBOOL(double,DomExpr(T1,T2))
REL_FROM_BINARY_OPERATORS(bool,DomExpr(T1,T2))

} // Casper
  
#endif /*_M_CASPER_CP_REL_OP_EXPR*/
