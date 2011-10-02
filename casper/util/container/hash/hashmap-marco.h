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

#include <casper/kernel/reversible/hashset.h>

namespace Casper {
namespace Util {


// adaptor for using StdHashSet as the underlying container
template<class PairKeyData,class EqualKey>
struct HashMapEqual
{
	bool operator()(const PairKeyData& t1,const PairKeyData& t2) const
	{	return EqualKey()(t1.first,t2.first);	}
	bool operator()(const typename PairKeyData::First& t1,
					const typename PairKeyData::First& t2) const
	{	return EqualKey()(t1,t2);	}
};

// adaptor for using StdHashSet as the underlying container
template<class PairKeyData,class Hasher>
struct HashMapHasher
{
	uint operator()(const PairKeyData& t) const
	{	return Hasher()(t.first);	}
	uint operator()(const typename PairKeyData::First& t) const
	{	return Hasher()(t);	}
};

/**
 * 	Container for elements of type (\a KeyT,\a DataT) where each key must be unique
 *  and may be used to access the corresponding \a DataT object in O(1).
 *  The number of elements in the container is unbound but the index size is set
 *  at creation and does not change during the life of the object.
 *
 *  \warning This container does not support multiple elements with the same key (that
 *  would be an HashMultiMap).
 * T2
 *  \param	KeyT	Key type.
 *	\param	DataT	Element type.
 *  \param  EqualT	Function used to test two keys for equality.
 *  \param  Hasher	Function used to get the hash value of a key.
 */
template<class KeyT, class DataT, class EqualKey = UseEqualOp<KeyT>, class Hasher = Hash<KeyT> >
struct StdHashMap : StdHashSet<StdPair<const KeyT,DataT>,
						 HashMapEqual<StdPair<const KeyT,DataT>,EqualKey>,
						 HashMapHasher<StdPair<const KeyT,DataT>,Hasher> >
{
	/// Key type
	typedef KeyT					Key;
	/// Element type
	typedef DataT					Data;
	typedef StdPair<const KeyT,DataT>	Value;

	typedef StdHashSet<Value,
					HashMapEqual<Value,EqualKey>,
					HashMapHasher<Value,Hasher> > Super;

	typedef typename Super::Iterator	Iterator;

	/// Creates a new StdHashMap in heap \a h with index of size \a indexsize
	StdHashMap(IHeap& h, uint indexsize = CASPER_HASHSET_DEFAULT_SIZE) :
				Super(h,indexsize) {}

	/// Creates a new StdHashMap in stdHeap with index of size \a indexsize
	StdHashMap(uint indexsize = CASPER_HASHSET_DEFAULT_SIZE) :
				Super(indexsize) {}

	/// Inserts the element \a data with key \a key in the container.
	bool insert(const Key& key, const Data& data);

	/// Returns an iterator pointing to the position of the element with key \a key, or end().
	Iterator find(const Key& key) const;

	/** Returns the data of the element with key \a key.
	 *  \note If no element with key \a key is found in the container,
	 *  this method inserts a default (key,Data()) object and returns it.
	 */
	Data&			operator[](const Key& key);

	/** Returns the data of the element with key \a key.
	 *  \pre The key \a key must exist in the container.
	 */
	const Data&		operator[](const Key& key) const;

	/// Erases the element with key \a key. Returns \p false if \a key does not exist.
	bool erase(const Key& key);
};

// returns true if the element was actually inserted
template<class KeyT, class DataT, class EqualKey, class Hasher>
inline bool StdHashMap<KeyT,DataT,EqualKey,Hasher>::insert(const KeyT& key, const DataT& data)
{	return Super::insert(Value(key,data));	}

template<class KeyT, class DataT, class EqualKey, class Hasher>
inline typename StdHashMap<KeyT,DataT,EqualKey,Hasher>::Iterator
StdHashMap<KeyT,DataT,EqualKey,Hasher>::find(const KeyT& key) const
{	return Super::find(key);	}

template<class KeyT, class DataT, class EqualKey, class Hasher>
inline bool StdHashMap<KeyT,DataT,EqualKey,Hasher>::erase(const Key& key)
{	return Super::erase(key);	}

template<class KeyT, class DataT, class EqualKey, class Hasher>
inline DataT& StdHashMap<KeyT,DataT,EqualKey,Hasher>::operator[](const KeyT& key)
{
	Iterator it = find(key);
	if (it == this->end())
	{
		insert(key,Data());
		return find(key)->second;
	}
	else
		return it->second;
}

template<class KeyT, class DataT, class EqualKey, class Hasher>
inline const DataT& StdHashMap<KeyT,DataT,EqualKey,Hasher>::operator[](const KeyT& key) const
{
	Iterator it = find(key);
	assert(it != this->end());
	return it->second;
}


/**
 * 	StdHashMap which allocates all memory at creation and frees at destruction.
 */
template<class KeyT,class DataT,class EqualKey = UseEqualOp<KeyT>, class Hasher = Hash<KeyT> >
struct QuickHashMap : StaticMemHolder,StdHashMap<KeyT,DataT,EqualKey,Hasher>
{
	typedef StdHashMap<KeyT,DataT,EqualKey,Hasher> Super;

	/**
	 * Create a new QuickHashMap allowing for \a maxInserts insertions
	 * and index ratio of \a cratio.
	 * \note maxInserts is the maximum number of insertions. This means that no
	 * more insertions are possible when this limit is reached, even if some
	 * elements are removed.
	 */
	QuickHashMap(uint maxInserts, float cratio = 1.25) :
		StaticMemHolder(
			maxInserts*sizeof(typename Super::Node)+
			static_cast<uint>(1+ceil(maxInserts*cratio))*sizeof(typename Super::Node*)+
		//		(sizeof(typename Super::Colisions)+sizeof(typename Super::Colisions::NodeI))+
					sizeof(int)*3),
			Super(this->getHeap(),static_cast<uint>(ceil(maxInserts*cratio)))
	{	}

	IHeap& getHeap() const {	return StaticMemHolder::getHeap();	}

	uint totalSize() const
	{
		return Super::getHeap().maxSize();
	}
};

} // Util
} // Casper


#endif /*CASPER_KERNEL_CONTAINER_HASHMAP_H_*/
