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

#include <unordered_set>
#include <casper/kernel/container/pair.h>
#include <casper/kernel/container/common.h>

namespace casper {
namespace detail {


#define CASPER_HASHSET_DEFAULT_SIZE	127

/**
 * Generic hash function. This will be used unless the
 * it is specialized for the specific \a T.
 */
template<class T>
struct Hash
{
	UInt operator()(const T& t) const
	{	return static_cast<UInt>(t);	}
};

template<class T>
struct Hash<T*>
{
	UInt operator()(T* const& t) const
	{	return reinterpret_cast<UInt>(t);	}
};

/**
 * Hash of a Pair type
 */
template<class T1,class T2>
struct Hash<casper::detail::Pair<T1,T2> >
{
	UInt operator()(const detail::Pair<T1,T2>& t) const
	{	return static_cast<UInt>(Hash<T1>()(t.first)+
								 Hash<T1>()(t.second));	}
};

/**
 * Hash of a String type
 */
template<>
struct Hash<casper::String>
{
	UInt operator()(const casper::String& t) const
	{	return std::hash<const char*>()(t.c_str());	}
};


/**
 * 	Tests equality between two elements of type \a T. Defaults
 *  to use the defined equality operator.
 */
template<class T>
struct UseEqualOp
{
	Bool operator()(const T& t1, const T& t2) const
	{	return t1 == t2;	}
};

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
template<class T, class EqualT = UseEqualOp<T>, class Hasher = Hash<T> >
struct HashSet : std::unordered_set<T,Hasher,EqualT>
{
	typedef std::unordered_set<T,Hasher,EqualT>	Super;

	typedef T					Value;

	typedef const Value& 		ConstReference;

	protected:

	public:

	typedef typename Super::iterator 			Iterator;
	typedef typename Super::const_iterator		ConstIterator;
	//typedef typename Super::reverse_iterator 	ReverseIterator;
	//typedef typename Super::const_reverse_iterator ConstReverseIterator;

	/// Creates a new HashSet in heap \a h with index of size \a indexsize
	HashSet(Heap h, UInt indexsize = CASPER_HASHSET_DEFAULT_SIZE) :
				Super(indexsize) {}


	/// Creates a new HashSet in stdHeap with index of size \a indexsize
	HashSet(UInt indexsize = CASPER_HASHSET_DEFAULT_SIZE) :
				Super(indexsize) {}

	/// Inserts a elem in the hashset. Returns \p false if the element already exists. O(1)
	Bool insert(ConstReference elem) {	return Super::insert(elem).second;	}

	Heap heap() const {	return stdHeap;	}

};

// ----------------

/**
 * 	FIXME same as HashSet
 */
template<class T,class EqualT = UseEqualOp<T>,class Hasher = Hash<T> >
struct QuickHashSet : HashSet<T,EqualT,Hasher>
{
	typedef HashSet<T,EqualT,Hasher> Super;

	QuickHashSet(UInt maxInserts, Float cratio = 1.25) :
		Super(maxInserts)
	{}

	Heap heap() const {	return stdHeap;	}
};

}

/**
	Writes the contents of the HashSet to an ostream.
*/
template<class T,class EqualT,class Hasher>
std::ostream& operator<<(std::ostream& o,
						const casper::detail::HashSet<T,EqualT,Hasher>& a)
{
	o << "{ ";
	for (typename casper::detail::HashSet<T,EqualT,Hasher>::ConstIterator it = a.begin();
		it != a.end(); ++it)
		o << *it << " ";
	o << "}";
	return o;
}

}

#endif /*CASPER_KERNEL_CONTAINER_HASHSETSTD_H_*/
