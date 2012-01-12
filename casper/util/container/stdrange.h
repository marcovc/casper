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
 * Sorted contiguous range.
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_STDRANGE
#define _H_CASPER_KERNEL_CONTAINER_STDRANGE

#include <iterator>
#include <casper/util/io.h>

#if defined(_MSC_VER) | defined(__APPLE__) | defined(SWIG)
typedef unsigned int uint;
#endif

namespace Casper {

namespace Util {


// iterator over a contiguous range of elements
template<class T>
struct RangeIterator  : std::iterator<std::bidirectional_iterator_tag,T,long>
{
		typedef		T	Value;

		// new constructor
					RangeIterator(Value curVal) : curVal(curVal)
					{ }

		// required from Equality Comparable model
		bool 		operator==(const RangeIterator& s) const
					{ return curVal == s.curVal; }
		bool 		operator!=(const RangeIterator& s) const
					{ return curVal != s.curVal; }

		// required from Trivial Iterator model
		const Value& operator*() const
					{ return curVal; }
		Value* operator->() const
					{ return &curVal; }

		// required from Input Iterator model
		RangeIterator& operator++() // pre-increment
					{ ++curVal; return *this; }
		RangeIterator 	operator++(int) //post-increment
					{ RangeIterator ret = *this; ++(*this); return ret;}

		// required from Output Iterator model
					RangeIterator(const RangeIterator& i)
					{ curVal = i.curVal; }

		// required from Bidirectional Iterator model
		RangeIterator& operator--() // pre-decrement
					{ --curVal; return *this; }
		RangeIterator 	operator--(int) //post-decrement
					{ RangeIterator ret = *this; --(*this); return ret;}
		private:

		Value	curVal;
};


/**
 * 	Sorted contiguous range.
 *  \ingroup Containers
 */
template<class T>
struct StdRange
{
	typedef T	Value;
	Value	lb;
	Value	ub;
	const Value& min() const {	return lb;	}
	const Value& max() const {	return ub;	}
	Value& min() {	return lb;	}
	Value& max() {	return ub;	}
	void   range(Value& l, Value& u) const { l = lb; u = ub;	}
	StdRange(const Value& lb, const Value& ub) : lb(lb),ub(ub) {}
	typedef RangeIterator<Value>	Iterator;
	Iterator begin() const {	return Iterator(lb);	}
	Iterator end() const	 {	return Iterator(ub+1);	}
};

} // Util

template<class T>
Util::StdRange<T> range(const T& lb,const T& ub)
{	return Util::StdRange<T>(lb,ub);	}

Util::StdRange<int> range(const uint& lb, const uint& ub);

};

// printing StdRange
template<class T>
std::ostream& operator<<(std::ostream& os, const Casper::Util::StdRange<T>& i)
{	return Casper::Util::RangePrinter<T,Casper::Util::StdRange<T> >()(os,i);	}


#endif // _H_CASPER_KERNEL_CONTAINER_STDRANGE

