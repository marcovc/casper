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

#include <casper/kernel/container/hashset.h>

namespace casper {
namespace detail {

// adaptor for using HashSet as the underlying container
template<class PairKeyData,class EqualKey>
struct HashMapEqual
{
	Bool operator()(const PairKeyData& t1,const PairKeyData& t2) const
	{	return EqualKey()(t1.first,t2.first);	}
	Bool operator()(const typename PairKeyData::First& t1,
					const typename PairKeyData::First& t2) const
	{	return EqualKey()(t1,t2);	}
};

// adaptor for using HashSet as the underlying container
template<class PairKeyData,class Hasher>
struct HashMapHasher
{
	UInt operator()(const PairKeyData& t) const
	{	return Hasher()(t.first);	}
	UInt operator()(const typename PairKeyData::First& t) const
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
struct HashMap : HashSet<Pair<const KeyT,DataT>,
						 HashMapEqual<Pair<const KeyT,DataT>,EqualKey>,
						 HashMapHasher<Pair<const KeyT,DataT>,Hasher> >
{
	/// Key type
	typedef KeyT					Key;
	/// Element type
	typedef DataT					Data;
	typedef Pair<const KeyT,DataT>	Value;

	typedef HashSet<Value,
					HashMapEqual<Value,EqualKey>,
					HashMapHasher<Value,Hasher> > Super;

	typedef typename Super::Iterator	Iterator;

	/// Creates a new HashMap in heap \a h with index of size \a indexsize
	HashMap(Heap h, UInt indexsize = CASPER_HASHSET_DEFAULT_SIZE) :
				Super(h,indexsize) {}

	/// Creates a new HashMap in stdHeap with index of size \a indexsize
	HashMap(UInt indexsize = CASPER_HASHSET_DEFAULT_SIZE) :
				Super(indexsize) {}

	/// Inserts the element \a data with key \a key in the container.
	Bool insert(const Key& key, const Data& data);

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
	Bool erase(const Key& key);
};

// returns true if the element was actually inserted
template<class KeyT, class DataT, class EqualKey, class Hasher>
inline Bool HashMap<KeyT,DataT,EqualKey,Hasher>::insert(const KeyT& key, const DataT& data)
{	return Super::insert(Value(key,data));	}

template<class KeyT, class DataT, class EqualKey, class Hasher>
inline typename HashMap<KeyT,DataT,EqualKey,Hasher>::Iterator
HashMap<KeyT,DataT,EqualKey,Hasher>::find(const KeyT& key) const
{	return Super::find(key);	}

template<class KeyT, class DataT, class EqualKey, class Hasher>
inline Bool HashMap<KeyT,DataT,EqualKey,Hasher>::erase(const Key& key)
{	return Super::erase(key);	}

template<class KeyT, class DataT, class EqualKey, class Hasher>
inline DataT& HashMap<KeyT,DataT,EqualKey,Hasher>::operator[](const KeyT& key)
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
inline const DataT& HashMap<KeyT,DataT,EqualKey,Hasher>::operator[](const KeyT& key) const
{
	Iterator it = find(key);
	assert(it != this->end());
	return it->second;
}


/**
 * 	HashMap which allocates all memory at creation and frees at destruction.
 */
template<class KeyT,class DataT,class EqualKey = UseEqualOp<KeyT>, class Hasher = Hash<KeyT> >
struct QuickHashMap : StaticMemHolder,HashMap<KeyT,DataT,EqualKey,Hasher>
{
	typedef HashMap<KeyT,DataT,EqualKey,Hasher> Super;

	/**
	 * Create a new QuickHashMap allowing for \a maxInserts insertions
	 * and index ratio of \a cratio.
	 * \note maxInserts is the maximum number of insertions. This means that no
	 * more insertions are possible when this limit is reached, even if some
	 * elements are removed.
	 */
	QuickHashMap(UInt maxInserts, Float cratio = 1.25) :
		StaticMemHolder(
			maxInserts*sizeof(typename Super::Node)+
			static_cast<UInt>(1+ceil(maxInserts*cratio))*sizeof(typename Super::Node*)+
		//		(sizeof(typename Super::Colisions)+sizeof(typename Super::Colisions::NodeI))+
					sizeof(Int)*3),
			Super(this->heap(),static_cast<UInt>(ceil(maxInserts*cratio)))
	{	}

	Heap heap() const {	return StaticMemHolder::heap();	}

	UInt totalSize() const
	{
		return Super::heap().maxSize();
	}
};

}
}


#endif /*CASPER_KERNEL_CONTAINER_HASHMAP_H_*/
