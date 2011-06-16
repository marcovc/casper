/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_MIP_TRAITS_H_
#define CASPER_MIP_TRAITS_H_

//#include <casper/mip/var.h>

namespace Casper {
namespace MIP {
namespace Traits {

template<class>
struct HasLPVar
{	static const bool value = false; };

template<>
struct HasLPVar<Casper::LP::Var>
{	static const bool value = true; };

//template<class T>
//struct HasLPVar<Casper::MIP::Var<T> >
//{	static const bool value = true; };

template<class Func,class T1>
struct HasLPVar<Rel1<Func,T1> >
{	static const bool value = HasLPVar<T1>::value; };

template<class Func,class T1,class T2>
struct HasLPVar<Rel2<Func,T1,T2> >
{	static const bool value = HasLPVar<T1>::value or HasLPVar<T2>::value ; };

template<class Func,class T1,class T2,class T3>
struct HasLPVar<Rel3<Func,T1,T2,T3> >
{	static const bool value = HasLPVar<T1>::value or HasLPVar<T2>::value or HasLPVar<T3>::value; };

}
}
}

#endif /* CASPER_MIP_TRAITS_H_ */
