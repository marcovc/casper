 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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
 * Iterator over a sorted contiguous range of values,
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_SCITERATOR
#define _H_CASPER_KERNEL_CONTAINER_SCITERATOR

#include <iterator>

namespace Casper {
namespace Util {


/**
	\brief An iterator over a sorted contiguous range of values

	Iterates over a sorted contiguous range of (integer) values.
*/
template<typename Value, typename Difference=long>
class SCIterator : public std::iterator<std::bidirectional_iterator_tag,
								   Value,Difference>
{
		public:

			static		const Value			max;

		// required from Default Constructible model (is it necessary?)
		// 			SCIterator();

		// new constructor
					SCIterator(Value value)
					{ curVal = value; pte=false;}

		// required from Equality Comparable model
		bool 		operator==(const SCIterator& s) const
					{ return pte == s.pte && (pte || curVal == s.curVal); }
		bool 		operator!=(const SCIterator& s) const
					{ return pte != s.pte || (!pte && curVal != s.curVal); }

		// required from Trivial Iterator model
		const Value& operator*() const
					{ return curVal; }
		Value* operator->() const
					{ return &curVal; }

		// required from Input Iterator model
		SCIterator& operator++() // pre-increment
					{ if (curVal++==max) pte=true; return *this; }
		SCIterator 	operator++(int) //post-increment
					{ SCIterator ret = *this; ++(*this); return ret;}

		// required from Output Iterator model
					SCIterator(const SCIterator& i)
					{ curVal = i.curVal; pte = i.pte;}

		// required from Bidirectional Iterator model
		SCIterator& operator--() // pre-decrement
					{ if (pte) {pte=false;curVal=max;} else --curVal; return *this; }
		SCIterator 	operator--(int) //post-decrement
					{ SCIterator ret = *this; --(*this); return ret;}
		private:

		Value	curVal;
		bool		pte;
};

#undef max

/// Maximum number reached by the iterator
template<typename Value, typename Difference>
const Value	SCIterator<Value,Difference>::max =
		limits<Value>::max();

} // Util
} // Casper

#endif // _H_CASPER_KERNEL_CONTAINER_SCITERATOR
