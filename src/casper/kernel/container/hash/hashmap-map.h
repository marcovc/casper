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

#ifndef CASPER_KERNEL_CONTAINER_HASHMAP_H_
#define CASPER_KERNEL_CONTAINER_HASHMAP_H_

#include <map>
#include <casper/kernel/container/hashset.h>

namespace casper {
namespace detail {


/**
 * 	Container for elements of type (\a KeyT,\a DataT) where each key must be unique
 *  and may be used to access the corresponding \a DataT object in O(1).
 *  The number of elements in the container is unbound but the index size is set
 *  at creation and does not change during the life of the object.
 *
 *  \warning This container does not support multiple elements with the same key (that
 *  would be an HashMultiMap).
 *
 *  \param	KeyT	Key type.
 *	\param	DataT	Element type.
 *  \param  EqualT	Function used to test two keys for equality.
 *  \param  Hasher	Function used to get the hash value of a key.
 */
template<class KeyT, class DataT,
		 class EqualT = Void,
		 class Hasher = Void >
struct HashMap : map<KeyT,DataT>
{
	typedef map<KeyT,DataT>	Super;

	/// Key type
	typedef KeyT		Key;
	/// Element type
	typedef DataT		Data;

	typedef typename Super::iterator		Iterator;
	typedef typename Super::const_iterator	ConstIterator;

	/// Creates a new HashMap in heap \a h with index of size \a indexsize
	HashMap(Heap h, UInt indexsize = 100) :
				Super() {}

	/// Creates a new HashMap in stdHeap with index of size \a indexsize
	HashMap(UInt indexsize = 100) :
				Super() {}

	/// Inserts the element \a data with key \a key in the container.
	Bool insert(const Key& key, const Data& data)
	{
		Super::insert(make_pair(key,data));
		return true;
	}
};


/**
 * 	FIXME same as HashMap
 */
template<class KeyT,class DataT,class EqualKey = Void, class Hasher = Void >
struct QuickHashMap : HashMap<KeyT,DataT,EqualKey,Hasher>
{
	typedef HashMap<KeyT,DataT,EqualKey,Hasher> Super;

	QuickHashMap(UInt maxInserts, Float cratio = 1.25) :
		Super(){}

	Heap heap() const {	return stdHeap;	}
};

}
}


#endif /*CASPER_KERNEL_CONTAINER_HASHMAP_H_*/
