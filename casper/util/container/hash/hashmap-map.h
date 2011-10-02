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

#ifndef CASPER_KERNEL_CONTAINER_STDHASHMAP_H_
#define CASPER_KERNEL_CONTAINER_STDHASHMAP_H_

#include <map>
#include <casper/kernel/reversible/stdhashset.h>

namespace Casper {
namespace Util {

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
		 class EqualT = void,
		 class Hasher = void >
struct StdHashMap : std::map<KeyT,DataT>
{
	typedef std::map<KeyT,DataT>	Super;

	/// Key type
	typedef KeyT		Key;
	/// Element type
	typedef DataT		Data;

	typedef typename Super::iterator		Iterator;
	typedef typename Super::const_iterator	ConstIterator;

	/// Creates a new StdHashMap in heap \a h with index of size \a indexsize
	StdHashMap(IHeap& h, uint indexsize = 100) :
				Super() {}

	/// Creates a new StdHashMap in stdHeap with index of size \a indexsize
	StdHashMap(uint indexsize = 100) :
				Super() {}

	/// Inserts the element \a data with key \a key in the container.
	bool insert(const Key& key, const Data& data)
	{
		Super::insert(make_pair(key,data));
		return true;
	}
};


/**
 * 	FIXME same as StdHashMap
 */
template<class KeyT,class DataT,class EqualKey = void, class Hasher = void >
struct QuickHashMap : StdHashMap<KeyT,DataT,EqualKey,Hasher>
{
	typedef StdHashMap<KeyT,DataT,EqualKey,Hasher> Super;

	QuickHashMap(uint maxInserts, float cratio = 1.25) :
		Super(){}

	IHeap& getHeap() const {	return stdHeap;	}
};

} // Util
} // Casper


#endif /*CASPER_KERNEL_CONTAINER_HASHMAP_H_*/
