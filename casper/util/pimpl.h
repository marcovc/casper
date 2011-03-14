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
 

#ifndef CASPER_UTIL_PIMPL_H_
#define CASPER_UTIL_PIMPL_H_

#include <memory>

namespace Casper {
namespace Util {


/// Pointer to implementation idiom
template<class Iface>
struct PImplIdiom
{
	typedef Iface* PImpl;
	PImplIdiom(Iface* pImpl) : pImpl(pImpl) { }
	PImplIdiom(const PImplIdiom& s) : pImpl(s.getPImpl()) { }
	Iface&				getImpl() 			{ return *pImpl;}
	const Iface&		getImpl() const 	{ return *pImpl;}
	PImpl&				getPImpl() 			{ return pImpl;}
	const PImpl&		getPImpl() const 	{ return pImpl;}

	const PImplIdiom<Iface>& operator=(const PImplIdiom<Iface>& s)
	{	pImpl = s.pImpl; return *this; 	}

	bool operator==(const PImplIdiom<Iface>& s) const
	{	return pImpl == s.pImpl; }

	PImpl pImpl;
};

/// Pointer to implementation idiom, with automatic garbage collection
template<class Iface>
struct SPImplIdiom
{
	typedef std::shared_ptr<Iface> PImpl;
	SPImplIdiom(Iface* pImpl) : pImpl(pImpl) { }
	SPImplIdiom(PImpl pImpl) : pImpl(pImpl) { }
	SPImplIdiom(const SPImplIdiom& s) : pImpl(s.getPImpl()) { }
	Iface&				getImpl() 			{ return *pImpl;}
	const Iface&		getImpl() const 	{ return *pImpl;}
	PImpl&				getPImpl() 			{ return pImpl;}
	const PImpl&		getPImpl() const 	{ return pImpl;}

	const SPImplIdiom<Iface>& operator=(const SPImplIdiom<Iface>& s)
	{	pImpl = s.pImpl; return *this; 	}

	bool operator==(const SPImplIdiom<Iface>& s) const
	{	return pImpl == s.pImpl; }

	PImpl pImpl;
};

}
}

#endif /* CASPER_UTIL_PIMPL_H_ */
