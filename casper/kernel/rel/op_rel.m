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

#ifndef _M_CASPER_KERNEL_REL_OP_REL
#define _M_CASPER_KERNEL_REL_OP_REL
 
#include <casper/kernel/rel/macros.h>

#define AT_Rel0(M)				Rel0< AT_ ## M >
#define AT_Rel1(M1,M2)			Rel1< AT_ ## M1,AT_ ## M2 >
#define AT_Rel2(M1,M2,M3)		Rel2< AT_ ## M1,AT_ ## M2,AT_ ## M3 >
#define AT_Rel3(M1,M2,M3,M4)	Rel3< AT_ ## M1,AT_ ## M2,AT_ ## M3,AT_ ## M4 >
#define AT_Rel4(M1,M2,M3,M4,M5)	Rel4< AT_ ## M1,AT_ ## M2,AT_ ## M3,AT_ ## M4, \
									 AT_ ## M5 >
									 
#define AD_Rel0(M)				AD_ ## M
#define AD_Rel1(M1,M2)			AD_ ## M1 AD_ ## M2
#define AD_Rel2(M1,M2,M3)		AD_ ## M1 AD_ ## M2 AD_ ## M3
#define AD_Rel3(M1,M2,M3,M4)	AD_ ## M1 AD_ ## M2 AD_ ## M3 AD_ ## M4
#define AD_Rel4(M1,M2,M3,M4,M5)	AD_ ## M1 AD_ ## M2 AD_ ## M3 AD_ ## M4 AD_ ## M5

namespace Casper {

// Rel0
REL_FROM_UNARY_OPERATORS(Rel0(Op))
REL_FROM_BINARY_OPERATORS(Rel0(Op),T)
REL_FROM_COMPARISON_OPERATORS(int,Rel0(Op))
REL_FROM_ARITH_OPERATORS(int,Rel0(Op))
REL_FROM_COMPARISON_OPERATORS(double,Rel0(Op))
REL_FROM_ARITH_OPERATORS(double,Rel0(Op))
REL_FROM_COMPARISON_OPERATORS(float,Rel0(Op))
REL_FROM_ARITH_OPERATORS(float,Rel0(Op))
REL_FROM_BINARY_OPERATORS(bool,Rel0(Op))

// Rel1
REL_FROM_UNARY_OPERATORS(Rel1(Op,T1))
REL_FROM_BINARY_OPERATORS(Rel1(Op,T1),T)
REL_FROM_COMPARISON_OPERATORS(int,Rel1(Op,T1))
REL_FROM_ARITH_OPERATORS(int,Rel1(Op,T1))
REL_FROM_COMPARISON_OPERATORS(double,Rel1(Op,T1))
REL_FROM_ARITH_OPERATORS(double,Rel1(Op,T1))
REL_FROM_COMPARISON_OPERATORS(float,Rel1(Op,T1))
REL_FROM_ARITH_OPERATORS(float,Rel1(Op,T1))
REL_FROM_BINARY_OPERATORS(bool,Rel1(Op,T1))

// Rel2
REL_FROM_UNARY_OPERATORS(Rel2(Op,T1,T2))
REL_FROM_BINARY_OPERATORS(Rel2(Op,T1,T2),T)
REL_FROM_COMPARISON_OPERATORS(int,Rel2(Op,T1,T2))
REL_FROM_ARITH_OPERATORS(int,Rel2(Op,T1,T2))
REL_FROM_COMPARISON_OPERATORS(double,Rel2(Op,T1,T2))
REL_FROM_ARITH_OPERATORS(double,Rel2(Op,T1,T2))
REL_FROM_COMPARISON_OPERATORS(float,Rel2(Op,T1,T2))
REL_FROM_ARITH_OPERATORS(float,Rel2(Op,T1,T2))
REL_FROM_BINARY_OPERATORS(bool,Rel2(Op,T1,T2))

// Rel3
REL_FROM_UNARY_OPERATORS(Rel3(Op,T1,T2,T3))
REL_FROM_BINARY_OPERATORS(Rel3(Op,T1,T2,T3),T)
REL_FROM_COMPARISON_OPERATORS(int,Rel3(Op,T1,T2,T3))
REL_FROM_ARITH_OPERATORS(int,Rel3(Op,T1,T2,T3))
REL_FROM_COMPARISON_OPERATORS(double,Rel3(Op,T1,T2,T3))
REL_FROM_ARITH_OPERATORS(double,Rel3(Op,T1,T2,T3))
REL_FROM_COMPARISON_OPERATORS(float,Rel3(Op,T1,T2,T3))
REL_FROM_ARITH_OPERATORS(float,Rel3(Op,T1,T2,T3))
REL_FROM_BINARY_OPERATORS(bool,Rel3(Op,T1,T2,T3))

// Rel4
REL_FROM_UNARY_OPERATORS(Rel4(Op,T1,T2,T3,T4))
REL_FROM_BINARY_OPERATORS(Rel4(Op,T1,T2,T3,T4),T)
REL_FROM_COMPARISON_OPERATORS(int,Rel4(Op,T1,T2,T3,T4))
REL_FROM_ARITH_OPERATORS(int,Rel4(Op,T1,T2,T3,T4))
REL_FROM_COMPARISON_OPERATORS(double,Rel4(Op,T1,T2,T3,T4))
REL_FROM_ARITH_OPERATORS(double,Rel4(Op,T1,T2,T3,T4))
REL_FROM_COMPARISON_OPERATORS(float,Rel4(Op,T1,T2,T3,T4))
REL_FROM_ARITH_OPERATORS(float,Rel4(Op,T1,T2,T3,T4))
REL_FROM_BINARY_OPERATORS(bool,Rel4(Op,T1,T2,T3,T4))

} // Casper
  
#endif // _M_CASPER_KERNEL_REL_OP_REL
