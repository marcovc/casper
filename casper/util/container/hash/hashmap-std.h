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

#ifndef CASPER_KERNEL_CONTAINER_HASHMAP_H_
#define CASPER_KERNEL_CONTAINER_HASHMAP_H_

#include <unordered_map>
#include <casper/util/container/stdhashset.h>

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
		 class EqualT = UseEqualOp<KeyT>,
		 class Hasher = Hash<KeyT> >
struct StdHashMap : std::unordered_map<KeyT,DataT,Hasher,EqualT>
{
	typedef std::unordered_map<KeyT,DataT,Hasher,EqualT>	Super;

	/// Key type
	typedef typename Super::key_type		Key;
	/// Element type
	typedef typename Super::mapped_type		Mapped;
	typedef typename Super::value_type		Value;

	typedef typename Super::iterator		Iterator;
	typedef typename Super::const_iterator	ConstIterator;

	/// Creates a new StdHashMap in heap \a h with index of size \a indexsize
	StdHashMap(IHeap& h, uint indexsize = CASPER_HASHSET_DEFAULT_SIZE) :
				Super(indexsize) {}

	/// Creates a new StdHashMap in stdHeap with index of size \a indexsize
	StdHashMap(uint indexsize = CASPER_HASHSET_DEFAULT_SIZE) :
				Super(indexsize) {}

	/// Inserts the element \a data with key \a key in the container.
	bool insert(const Key& key, const Mapped& data)
	{	return Super::insert(Value(key,data)).second;	}
};


/**
 * 	FIXME same as StdHashMap
 */
template<class KeyT,class DataT,class EqualKey = UseEqualOp<KeyT>, class Hasher = Hash<KeyT> >
struct QuickHashMap : StdHashMap<KeyT,DataT,EqualKey,Hasher>
{
	typedef StdHashMap<KeyT,DataT,EqualKey,Hasher> Super;

	QuickHashMap(uint maxInserts, float cratio = 0.8) :
		Super(maxInserts){}

	IHeap& getHeap() const {	return stdHeap();	}
};

} // Util
} // Casper

/**
	Writes the contents of the StdHashMap to an ostream.
*/
template<class KeyT,class DataT,class EqualT,class Hasher>
std::ostream& operator<<(std::ostream& o,
						const Casper::Util::StdHashMap<KeyT,DataT,EqualT,Hasher>& a)
{
	o << "{ ";
	auto it = a.begin();
	if (it != a.end())
	{
		o << it->first << " -> " << it->second;
		++it;
		for ( ; it != a.end(); ++it)
			o << ", " << it->first << " -> " << it->second;
	}
	o << "}";
	return o;
}


#endif /*CASPER_KERNEL_CONTAINER_HASHMAP_H_*/
