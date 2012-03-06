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

/**
 *	\file
 * 	Basic filters.
 */

#ifndef _H_CASPER_KERNEL_FILTER_BASIC
#define _H_CASPER_KERNEL_FILTER_BASIC

#include <casper/cp/filter/common.h>

#include <casper/cp/view/val.h>

namespace Casper {

namespace CP {

// TODO: make this a global static object (similar to fail() goal)
struct UndefinedFilter : IFilter
{
	template<class T>
	UndefinedFilter(Store& s,const T& t) : IFilter(s)
	{
		assert(0);
		/*int     status;
		char   *fname;
		fname = abi::__cxa_demangle(typeid(t).name(), 0, 0, &status);
		std::cout << "creating " << fname << std::endl;
		 */
		//cerr << "Error: Trying to instantiate an unimplemented filter.\n";
		//assert(0);
	}
	bool execute()
	{
		std::cerr << "Error: Trying to execute an unimplemented filter " << std::endl;
	//		 << fname << endl;
		assert(0);
		return false;
	}
	void attach(INotifiable* pParent)	{ }
	void detach()	{ }
};

template<class Cond,class Func>
struct When : IFilter
{
	When(Store& store, const Cond& cond, const Func& func) :
		IFilter(store),cond(store,cond),func(func)
	{}
	bool execute()
	{
		if (!cond.ground())
			return true;
		if (cond.value() and !func())
			return false;
		detach();
		return true;
	}
	void attach(INotifiable* f)
	{	cond.attach(f);	}
	void detach()
	{	cond.detach();	}
	Cost cost() const
	{	return constantLo;	}

	ValView<bool,Cond> cond;
	Func func;
};

template<class Cond,class Func>
bool Store::when(const Cond& cond, const Func& func)
{	return this->post(new (*this) When<Cond,Func>(*this,cond,func));	}


} // CP
} // Casper

#endif /*_H_CASPER_KERNEL_FILTER_BASIC */
