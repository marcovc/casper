 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_CONTAINER_RANGE
#define _H_CASPER_KERNEL_CONTAINER_RANGE

#include <iterator>
#include <casper/kernel/common.h>
#include <casper/kernel/io.h>

namespace casper {

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
struct Range
{
	typedef T	Value;
	Value	lb;
	Value	ub;
	const Value& min() const {	return lb;	}
	const Value& max() const {	return ub;	}
	Value& min() {	return lb;	}
	Value& max() {	return ub;	}
	Void   range(Value& l, Value& u) const { l = lb; u = ub;	}
	Range(const Value& lb, const Value& ub) : lb(lb),ub(ub) {}
	typedef RangeIterator<Value>	Iterator;
	Iterator begin() const {	return Iterator(lb);	}
	Iterator end() const	 {	return Iterator(ub+1);	}
};

template<class T>
Range<T> range(const T& lb,const T& ub)
{	return Range<T>(lb,ub);	}

Range<Int> range(const UInt& lb, const UInt& ub);

template<class> struct EvalTraits;

template<class T>
struct EvalTraits<Range<T> >
{	typedef Seq<T>	Eval;	};

template<class> struct ElemTraits;

template<class T>
struct ElemTraits<Range<T> >
{	typedef T	Elem;	};

// printing Range
template<class T>
std::ostream& operator<<(std::ostream& os, const casper::Range<T>& i)
{	return casper::detail::RangePrinter<T,casper::Range<T> >()(os,i);	}

};



#endif // _H_CASPER_KERNEL_CONTAINER_RANGE

