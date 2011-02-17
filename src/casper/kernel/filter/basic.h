 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <casper/kernel/filter/common.h>

namespace casper {

// TODO: make this a global static object (similar to fail() goal)
struct NoFilter : IFilter
{
	template<class T>
	NoFilter(ICPSolver& s,const T& t) : IFilter(s)
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
		cerr << "Error: Trying to execute an unimplemented filter " << endl;
	//		 << fname << endl;
		assert(0);
		return false;
	}
	bool entailed() const
	{
		cerr << "Error: Trying to check an unimplemented filter " << endl;
	//		 << fname << endl;
		assert(0);
		return false;
	}
	void attach(INotifiable* pParent)	{ }
	void detach(INotifiable* pParent)	{ }
};


};

#endif /*_H_CASPER_KERNEL_FILTER_BASIC */
