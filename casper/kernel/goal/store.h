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
 

#ifndef CASPER_KERNEL_GOAL_STORE_H_
#define CASPER_KERNEL_GOAL_STORE_H_

#include <iostream>

namespace Casper {

namespace Detail {
template<class Store,class T>
struct Post: IGoal
{
	Post(Store& store,const T& p, bool validate) :
		store(store),p(p),validate(validate) {}
	Goal execute()
	{
		if (store.post(p))
			return !validate or store.valid();
		else
			return fail();
	}
	Store&	store;
	const T	p;
	const bool	validate;
};

template<class Store>
struct Validate : IGoal
{
	Validate(Store& store) :
		store(store) {}
	Goal execute()
	{
		if (store.valid())
			return succeed();
		else
			return fail();
	}
	Store&	store;
};

} // Detail

template<class Store,class T>
Goal post(Store& s, const T& p, bool validate = true)
{	return new (s) Detail::Post<Store,T>(s,p,validate);}

template<class Store>
Goal validate(Store& s)
{	return new (s) Detail::Validate<Store>(s);}

} // Casper

#endif /* CASPER_KERNEL_GOAL_STORE_H_ */
