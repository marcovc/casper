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
 

#ifndef CASPER_KERNEL_OBJ_LIST_H_
#define CASPER_KERNEL_OBJ_LIST_H_

#include <casper/kernel/obj/expr.h>
#include <casper/util/container/stdlist.h>

#include <typeinfo>

namespace Casper {
namespace Detail {

template<class T,class Elem>
struct Create<T,Util::StdList<Elem> >
{
	Util::StdList<Elem> operator()(const T& t)
	{	throw Casper::Exception::TypeCoercion(typeid(T).name(),typeid(Util::StdList<Elem>).name());	}
};

template<class Elem>
struct Create<Util::StdList<Elem>,Util::StdList<Elem> >
{
	const Util::StdList<Elem>& operator()(const Util::StdList<Elem>& t)
	{	return t;	}
};


} // Detail

namespace CP {

} // CP

}

#endif /* CASPER_KERNEL_OBJ_LIST_H_ */
