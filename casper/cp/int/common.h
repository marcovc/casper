 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_CP_INT_COMMON
#define _H_CASPER_CP_INT_COMMON

// macro for avoiding writing the template specification for
// every method.
#define FD_TP_SPEC(N) \
	 		class Container##N, class Element##N, class T##N

// same as previous macro but not specifying "Element"
#define FD_TP_NE_SPEC(N) \
	 	class Container##N, class T##N

// macro for avoiding writing the template parameter list
#define FD_TP_LIST(N) Container##N,Element##N,T##N

// USE macro for the FD type
//#define FD(X)	Casper::FD<FD_TP_LIST(X)>

// if printing an fdset should also print domain cardinality
#define FD_SHOW_CARD	0

#endif
