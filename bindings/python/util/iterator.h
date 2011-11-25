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
 

#ifndef CASPER_BINDINGS_PYTHON_UTIL_ITERATOR_H_
#define CASPER_BINDINGS_PYTHON_UTIL_ITERATOR_H_

namespace Casper {
namespace Util {


template<class Obj,class It,class Val>
struct Iterator
{
	Iterator(Obj& obj) : obj(obj),b(obj.begin()) {}
	const Iterator<Obj,It,Val>& __iter__() const { return *this; }
	Val next()
	{	return *(b++);	 }
	bool _valid() const
	{	return b!=obj.end();	}
private:
	Obj& obj;
	It b;
};


}
}


#endif /* CASPER_BINDINGS_PYTHON_UTIL_ITERATOR_H_ */
