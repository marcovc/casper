 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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
 * \file
 * Heterogeneous container of a pair of objects, mimicking std::pair.
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_STDPAIR
#define _H_CASPER_KERNEL_CONTAINER_STDPAIR

#include <casper/kernel/common.h>

namespace Casper {
namespace Util {


template<class T1,class T2 = T1>
struct StdPair
{
	typedef	T1	First;
	typedef	T2	Second;

	typedef StdPair<T1,T2>	Self;
	StdPair() : first(0),second(0) {}
	StdPair(const T1& first, const T2& second) : first(first),second(second) {}
	StdPair(const StdPair& s) : first(s.first),second(s.second) {}

	bool	operator<(const Self& s) const
	{	return first < s.first or (first == s.first and second < s.second);	}

	bool	operator==(const Self& s) const
	{	return first == s.first and second == s.second;	}

	bool	operator!=(const Self& s) const
	{	return first != s.first or second != s.second;	}

	First	first;
	Second	second;
};

} // Util


template<class T1,class T2>
Util::StdPair<T1,T2> pair(const T1& p1,const T2& p2)
{	return Util::StdPair<T1,T2>(p1,p2);	}

} // Casper

template<class T1, class T2>
std::ostream& operator<<(std::ostream& os, const Casper::Util::StdPair<T1,T2>& v)
{
	os << "(" << v.first << "," << v.second << ")";
	return os;
}




#endif
