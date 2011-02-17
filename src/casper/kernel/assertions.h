/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2010-2010 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_KERNEL_ASSERTIONS_H_
#define CASPER_KERNEL_ASSERTIONS_H_

namespace casper {
namespace detail {

// check if two types are the same type

template<class T1,class T2>
struct IsSameType
{	static const bool value;	};

template<class T>
struct IsSameType<T,T>
{	static const bool value;	};

template<class T1,class T2>
const bool IsSameType<T1,T2>::value = false;

template<class T>
const bool IsSameType<T,T>::value = true;

#define CASPER_ASSERT_BNDVIEW_EVAL(T) \
	static_assert(detail::IsSameType<Eval,Bool>::value or \
				  detail::IsSameType<Eval,Int>::value or \
				  detail::IsSameType<Eval,Float>::value or \
				  detail::IsSameType<Eval,Double>::value,\
				  "instantiation of BndView with unsupported evaluation");

#define CASPER_ASSERT_CHKVIEW_EVAL(T) \
	static_assert(detail::IsSameType<Eval,Bool>::value,\
				  "instantiation of ChkView with unsupported evaluation");

}
}

#endif /* CASPER_KERNEL_ASSERTIONS_H_ */
