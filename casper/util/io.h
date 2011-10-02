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

#ifndef _H_CASPER_KERNEL_IO
#define _H_CASPER_KERNEL_IO

namespace Casper {
namespace Util {

// general range printer

template<class T,class RangeT>
struct RangePrinter
{
	std::ostream& operator()(std::ostream& os,
						 	 const RangeT& r)
	{
		if (r.min()==r.max())
			os << r.min();
		else
			os << "[" << r.min() << ".." << r.max() << "]";
		return os;
	}
};

// range of float printer
template<class RangeT>
struct RangePrinter<float,RangeT>
{
	std::ostream& operator()(std::ostream& os,
						 	 const RangeT& r)
	{
		if (r.min()==r.max())
			os << r.min();
		else
			os << "[" << r.min() << ";" << r.max() << "]";
		return os;
	}
};

// range of double printer
template<class RangeT>
struct RangePrinter<double,RangeT>
{
	std::ostream& operator()(std::ostream& os,
						 	 const RangeT& r)
	{
		if (r.min()==r.max())
			os << r.min();
		else
			os << "[" << r.min() << ";" << r.max() << "]";
		return os;
	}
};

} // Util
} // Casper

#endif /*_H_CASPER_KERNEL_IO*/
