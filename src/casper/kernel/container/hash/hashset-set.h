 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_KERNEL_CONTAINER_HASHSETSTD_H_
#define CASPER_KERNEL_CONTAINER_HASHSETSTD_H_

#include <set>
#include <casper/kernel/container/pair.h>
#include <casper/kernel/container/common.h>

namespace casper {
namespace detail {

/**
 * 	Container for elements of type \a T where each element is unique and may
 *  be accessed in O(1).
 *  The number of elements in the container is unbound but the index size is set
 *  at creation and does not change during the life of the object.
 *
 *  \param	T		Element type.
 *  \param  EqualT	Function used to test two elements for equality.
 *  \param  Hasher	Function used to get the hash value of an element.
 */
template<class T, class EqualT = Void, class Hasher = Void >
struct HashSet : std::set<T>
{
	typedef std::set<T>	Super;

	typedef T					Value;

	typedef const Value& 		ConstReference;

	protected:

	public:

	typedef typename Super::iterator 			Iterator;
	typedef typename Super::const_iterator		ConstIterator;
	typedef typename Super::reverse_iterator 	ReverseIterator;
	typedef typename Super::const_reverse_iterator ConstReverseIterator;

	/// Creates a new HashSet in heap \a h with index of size \a indexsize
	HashSet(Heap h, UInt indexsize = 100) :
				Super() {}


	/// Creates a new HashSet in stdHeap with index of size \a indexsize
	HashSet(UInt indexsize = 100) :
				Super() {}

	/// Inserts a elem in the hashset. Returns \p false if the element already exists. O(1)
	Bool insert(ConstReference elem)
	{
		Super::insert(elem);
		return true;
	}

	Heap heap() const {	return stdHeap;	}

};

// ----------------

/**
 * 	FIXME same as HashSet
 */
template<class T,class EqualT = Void ,class Hasher = Void >
struct QuickHashSet : HashSet<T,EqualT,Hasher>
{
	typedef HashSet<T,EqualT,Hasher> Super;

	QuickHashSet(UInt maxInserts, Float cratio = 1.25) :
		Super(maxInserts)
	{}

	Heap heap() const {	return stdHeap;	}
};

}
}

#endif /*CASPER_KERNEL_CONTAINER_HASHSETSTD_H_*/
