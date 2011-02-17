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

#ifndef CASPER_KERNEL_CONTAINER_HASHSET_H_
#define CASPER_KERNEL_CONTAINER_HASHSET_H_

#include "hashutil.h"
#include <casper/kernel/container/common.h>

namespace casper {
namespace detail {

#define CASPER_HASHSET_DEFAULT_SIZE	127

bool isPrime(int n)
{
	for (int i = 2; i < floor(sqrt(n)); i++)
	{
		if (n % i == 0)
			return false;
	}

	return true;
}

int nextPrime(int n)
{
	while (!isPrime(n))
		n++;

	return n;
}


/**
 * Generic hash function. This will be used unless the
 * it is specialized for the specific \a T.
 */
template<class T>
struct Hash
{
	UInt operator()(const T& t) const
	{
		return HashUtil::hash(HashUtil::SEED, t);
	}
};

/**
 * Hash of a Pair type
 */
/*
template<class T1,class T2>
struct Hash<detail::Pair<T1,T2> >
{
	UInt operator()(const detail::Pair<T1,T2>& t) const
	{	return static_cast<UInt>(Hash<T1>()(t.first)+
								 Hash<T1>()(t.second));	}
};
*/

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
struct HashSet
{
	typedef T					Value;

	typedef Value&				Reference;
	typedef const Value& 		ConstReference;
	typedef Value*				Pointer;
	typedef const Value* 		ConstPointer;
	typedef size_t				Difference;
	typedef size_t				Size;

	protected:

	typedef List<Value>			Colisions;
	typedef Vector<Colisions>	HashTable;

	HashTable					htable;
	Hasher						hasher;
	Size 						tSize;

	public:

	class Iterator;
	friend class Iterator;

	/// Iterator for accessing all elements in the hashset
	class Iterator : public std::iterator<bidirectional_iterator_tag,
	  									Value,
	  									Difference>
	{
		friend struct HashSet;
		typedef typename HashSet::Colisions::Iterator	CIterator;
		public:
		Iterator() : pOwner(NULL) {}
		Iterator(const HashSet* pOwner) : pOwner(pOwner),chash(0) {}
		Iterator(const Iterator& s) : pOwner(s.pOwner),chash(s.chash),cit(s.cit) {}
		bool operator==(const Iterator& s) const
		{ assert(pOwner == s.pOwner); return chash == s.chash and
								(chash==pOwner->htable.size() or cit == s.cit); }
		bool operator!=(const Iterator& s) const
		{ assert(pOwner == s.pOwner); return chash != s.chash or
								(chash != pOwner->htable.size() and cit != s.cit); }
		Reference operator*() const
		{ assert(pOwner); return cit.operator*(); }
		Pointer operator->() const
		{ assert(pOwner); return cit.operator->(); }
		Iterator& operator++() // pre-increment
		{
			++cit;
			while (cit == pOwner->htable[chash].end() and
					chash < pOwner->htable.size())
			{
				++chash;
				cit = pOwner->htable[chash].begin();
			}
			return *this;
		}
		Iterator operator++(int) //post-increment
		{ assert(pOwner); Iterator ret(*this); ++(*this); return ret; }
		Iterator& operator--() // pre-decrement
		{
			while (cit == pOwner->htable[chash].begin() and chash > 0)
			{
				--chash;
				cit = pOwner->htable[chash].end();
			}
			--cit;
			return *this;
		}
		Iterator operator--(int) //post-decrement
		{ Iterator ret(*this); --(*this); return ret; }

		const Iterator& operator=(const Iterator& s)
		{ pOwner = s.pOwner; chash = s.chash; cit = s.cit;	return *this; }
		protected:
		Iterator(const HashSet* pOwner, UInt chash, CIterator cit) :
			pOwner(pOwner),chash(chash),cit(cit) {}
		Iterator(const HashSet* pOwner, UInt chash) :
			pOwner(pOwner),chash(chash) {}

		const HashSet*	pOwner;
		UInt		chash;
		CIterator	cit;
	};


	typedef Iterator 					ConstIterator;
	typedef reverse_iterator<Iterator> 	ReverseIterator;
	typedef ReverseIterator 			ConstReverseIterator;

	/// Creates a new HashSet in heap \a h with index of size \a indexsize
	HashSet(Heap h, UInt indexsize = CASPER_HASHSET_DEFAULT_SIZE) :
				htable(h,nextPrime(indexsize),h), tSize(0) {}

	/// Creates a new HashSet in stdHeap with index of size \a indexsize
	HashSet(UInt hashsize = CASPER_HASHSET_DEFAULT_SIZE) :
				htable(stdHeap,nextPrime(hashsize),stdHeap), tSize(0) {}

	Iterator begin() const;
	Iterator end() const;
	ReverseIterator rbegin() const {return ReverseIterator(end());}
	ReverseIterator rend() const {return ReverseIterator(begin());}

	/// Inserts a elem in the hashset. Returns \p false if the element already exists. O(1)
	Bool insert(ConstReference elem);

	/// Returns an iterator pointing to position of \a elem, or end(). O(1)
	Iterator find(ConstReference elem) const;

	/// Erases the value pointed to by \a it. O(1)
	Void erase(Iterator it);

	/// Erases \a elem from the hashset. Returns \p false if the element does not exist. O(1)
	Bool erase(ConstReference elem);

	Heap heap() const {	return htable.heap();	}

	/// Remove all elements from the hashset. O(n)
	Void clear();

	/// Number of elements in the hashset. O(1)
	UInt size() const;

	/// Returns true if the hashset is empty. O(1)
	Bool empty() const;

	Float averageCollisions() const;
};

template<class T,class EqualT,class Hasher>
typename HashSet<T,EqualT,Hasher>::Iterator HashSet<T,EqualT,Hasher>::begin() const
{
	Iterator r(this);
	r.chash = 0;
	r.cit = htable[r.chash].begin();
	while (r.cit == htable[r.chash].end() and r.chash < htable.size()-1)
	{
		++r.chash;
		r.cit = htable[r.chash].begin();
	}
	return r;
}

template<class T,class EqualT,class Hasher>
typename HashSet<T,EqualT,Hasher>::Iterator HashSet<T,EqualT,Hasher>::end() const
{
	return Iterator(this,htable.size());
}

// returns true if the element was actually inserted
template<class T,class EqualT,class Hasher>
Bool HashSet<T,EqualT,Hasher>::insert(ConstReference value)
{
	UInt h = hasher(value) % htable.size();
	for (typename Colisions::Iterator it = htable[h].begin();
			it != htable[h].end(); ++it)
		if (EqualT()(*it,value))
			return false;
	htable[h].pushBack(value);
	tSize++;
	return true;
}

template<class T,class EqualT,class Hasher>
typename HashSet<T,EqualT,Hasher>::Iterator
HashSet<T,EqualT,Hasher>::find(ConstReference value) const
{
	UInt h = hasher(value) % htable.size();
	for (typename Colisions::Iterator it = htable[h].begin();
			it != htable[h].end(); ++it)
		if (EqualT()(*it,value))
			return Iterator(this,h,it);
	return end();
}

template<class T,class EqualT,class Hasher>
Void HashSet<T,EqualT,Hasher>::erase(Iterator it)
{	htable[it.chash].erase(it.cit);	}

template<class T,class EqualT,class Hasher>
Bool HashSet<T,EqualT,Hasher>::erase(ConstReference value)
{
	Iterator it = find(value);
	if (it == end())
		return false;
	erase(it);
	tSize--;
	return true;
}

template<class T,class EqualT,class Hasher>
Void HashSet<T,EqualT,Hasher>::clear()
{
	for (UInt i = 0; i < htable.size(); ++i)
		htable[i].clear();
	size = 0;
}

template<class T,class EqualT,class Hasher>
UInt HashSet<T,EqualT,Hasher>::size() const
{
	return tSize;
}

template<class T,class EqualT,class Hasher>
Bool HashSet<T,EqualT,Hasher>::empty() const
{
	return tSize == 0;
}

// ----------------
/*
struct StaticMemHolder
{
	StaticMemHolder(UInt size) : mHeap(size) {}
	Heap	heap() const {	return const_cast<IHeap*>(static_cast<const IHeap*>(&mHeap)); }
	StaticMem	mHeap;
};
*/

/**
 * 	HashSet which allocates all memory at creation and frees at destruction.
 */
template<class T,class EqualT = UseEqualOp<T>,class Hasher = Hash<T> >
struct QuickHashSet : StaticMemHolder,HashSet<T,EqualT,Hasher>
{
	typedef HashSet<T,EqualT,Hasher> Super;

	/**
	 * Create a new QuickHashSet allowing \a maxInserts insertions and index
	 * ratio of \a cratio.
	 * \note maxInserts is the maximum number of insertions. This means that no
	 * more insertions are possible when this limit is reached, even if some
	 * elements are removed.
	 **/
	QuickHashSet(UInt maxInserts, Float cratio = 1.25) :
		StaticMemHolder(
			maxInserts*sizeof(typename Super::Colisions::Node)+
			static_cast<UInt>(nextPrime(ceil(maxInserts*cratio)))*
				(sizeof(typename Super::Colisions)+2*sizeof(Void*))+
					sizeof(Int)*2),
			Super(this->heap(),static_cast<UInt>(nextPrime(ceil(maxInserts*cratio))))
	{}

	Heap heap() const {	return StaticMemHolder::heap();	}

	UInt totalSize() const
	{
		return Super::heap().maxSize();
	}
};

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

template<class T,class EqualT,class Hasher>
Float HashSet<T,EqualT,Hasher>::averageCollisions() const
{
	Int counter = 0;

	for (unsigned int i = 0; i < htable.size(); ++i)
	{
		if (htable[i].size() != 0)
		{
			counter++;
			//std::cout << "Number of collisions: " << htable[i].size() << endl;
		}
	}

	return tSize / counter;
}

}
}


#endif /*CASPER_KERNEL_CONTAINER_HASHSET_H_*/
