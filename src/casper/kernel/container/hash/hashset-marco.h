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

#include <casper/kernel/container/common.h>
#include <casper/kernel/container/pair.h>
#include <casper/kernel/container/vector.h>

namespace casper {
namespace detail {

#define CASPER_HASHSET_DEFAULT_SIZE	100

/**
 * Generic hash function. This will be used unless the
 * it is specialized for the specific \a T.
 */
template<class T>
struct Hash
{
	UInt operator()(const T& t) const
	{	return reinterpret_cast<UInt>(t);	}
};

/**
 * Hash of a Pair type
 */
template<class T1,class T2>
struct Hash<detail::Pair<T1,T2> >
{
	UInt operator()(const detail::Pair<T1,T2>& t) const
	{	return static_cast<UInt>(Hash<T1>()(t.first)+
								 Hash<T1>()(t.second));	}
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

	template<class Value>
	struct HashSetNode
	{
		HashSetNode(const Value& value) :
			value(value),pNext(NULL) {}
		HashSetNode(const Value& value, HashSetNode<Value>* pNext) :
			value(value),pNext(pNext) {}
		const Value			value;
		HashSetNode<Value>*	pNext;
	};

	typedef HashSetNode<Value>	Node;
	typedef Vector<Node*>		HashTable;

	HashTable					htable;
	Hasher						hasher;

	UInt getIndex(const Value& v) const
	{	return hasher(v) % htable.size();	}

	template<class Key>
	UInt getIndex(const Key& v) const
	{	return hasher(v) % htable.size();	}

	public:

	class Iterator;
	friend class Iterator;

	/// Iterator for accessing all elements in the hashset
	class Iterator : public std::iterator<bidirectional_iterator_tag,
	  									  Value,
	  									  Difference>
	{
		friend struct HashSet;
		public:
		//Iterator() : pOwner(NULL),pCurNode(NULL) {}
		//Iterator(const HashSet* pOwner) : pOwner(pOwner) {}
		Iterator(const Iterator& s) : pOwner(s.pOwner),pCurNode(s.pCurNode) {}
		bool operator==(const Iterator& s) const
		{ assert(pOwner == s.pOwner); return pCurNode == s.pCurNode; }
		bool operator!=(const Iterator& s) const
		{ assert(pOwner == s.pOwner); return pCurNode != s.pCurNode; }
		// FIXME: should return ConstReference
		Reference operator*() const
		{ assert(pOwner); return const_cast<Reference>(pCurNode->value); }
		// FIXME: should return ConstPointer
		Pointer operator->() const
		{ assert(pOwner); return const_cast<Pointer>(&pCurNode->value); }
		Iterator& operator++() // pre-increment
		{
			const Node* old = pCurNode;
			pCurNode = pCurNode->pNext;
			if (!pCurNode)
			{
				UInt chash = pOwner->getIndex(old->value);
				while (!pCurNode and ++chash < pOwner->htable.size())
					pCurNode = pOwner->htable[chash];
			}
			return *this;
		}
		Iterator operator++(int) //post-increment
		{ assert(pOwner); Iterator ret(*this); ++(*this); return ret; }

		const Iterator& operator=(const Iterator& s)
		{ pOwner = s.pOwner; pCurNode = s.pCurNode;	return *this; }
		protected:
		Iterator(const HashSet* pOwner, Node* pNode) :
			pOwner(pOwner),pCurNode(pNode) {}

		const HashSet*			pOwner;
		HashSetNode<Value>*		pCurNode;
	};


	typedef Iterator 					ConstIterator;
	typedef reverse_iterator<Iterator> 	ReverseIterator;
	typedef ReverseIterator 			ConstReverseIterator;

	/// Creates a new HashSet in heap \a h with index of size \a indexsize
	HashSet(Heap h, UInt indexsize = CASPER_HASHSET_DEFAULT_SIZE) :
				htable(h,indexsize+1,NULL) {}

	/// Creates a new HashSet in stdHeap with index of size \a indexsize
	HashSet(UInt indexsize = CASPER_HASHSET_DEFAULT_SIZE) :
				htable(stdHeap,indexsize+1,NULL) {}

	Iterator begin() const;
	Iterator end() const;
	ReverseIterator rbegin() const {return ReverseIterator(end());}
	ReverseIterator rend() const {return ReverseIterator(begin());}

	/// Inserts a elem in the hashset. Returns \p false if the element already exists. O(1)
	Bool insert(ConstReference elem);

	/// Returns an iterator pointing to position of \a elem, or end(). O(1)
	Iterator find(ConstReference elem) const;

	/// Returns an iterator pointing to position of \a elem, or end(). O(1)
	template<class Key>
	Iterator find(const Key& key) const;

	/// Erases the value pointed to by \a it. O(1)
	Void erase(Iterator it);

	/// Erases \a elem from the hashset. Returns \p false if the element does not exist. O(1)
	Bool erase(ConstReference elem);

	/// Erases \a key from the hashset. Returns \p false if the element does not exist. O(1)
	template<class Key>
	Bool erase(const Key& key);

	Heap heap() const {	return htable.heap();	}

	/// Remove all elements from the hashset. O(n)
	Void clear();

	// O(n)
	UInt size() const;

	// O(n)
	UInt empty() const	{	return begin() == end();	}
};

template<class T,class EqualT,class Hasher>
typename HashSet<T,EqualT,Hasher>::Iterator HashSet<T,EqualT,Hasher>::begin() const
{
	for (UInt n = 0; n < htable.size(); ++n)
		if (htable[n])
			return Iterator(this,htable[n]);
	return end();
}

template<class T,class EqualT,class Hasher>
inline typename HashSet<T,EqualT,Hasher>::Iterator HashSet<T,EqualT,Hasher>::end() const
{	return Iterator(this,NULL);	}

// returns true if the element was actually inserted
template<class T,class EqualT,class Hasher>
Bool HashSet<T,EqualT,Hasher>::insert(ConstReference value)
{
	UInt h = getIndex(value);
	for (Node* curNode = htable[h]; curNode != NULL; curNode = curNode->pNext)
		if (EqualT()(curNode->value,value))
			return false;
	htable[h] = new (heap()) Node(value,htable[h]);
	return true;
}

template<class T,class EqualT,class Hasher>
UInt HashSet<T,EqualT,Hasher>::size() const
{
	UInt r = 0;
	for (Iterator it = begin(); it != end(); ++it)
		++r;
	return r;
}

template<class T,class EqualT,class Hasher>
typename HashSet<T,EqualT,Hasher>::Iterator
HashSet<T,EqualT,Hasher>::find(ConstReference value) const
{
	UInt h = getIndex(value);
	for (Node* curNode = htable[h]; curNode != NULL; curNode = curNode->pNext)
		if (EqualT()(curNode->value,value))
			return Iterator(this,curNode);
	return end();
}

template<class T,class EqualT,class Hasher>
template<class Key>
typename HashSet<T,EqualT,Hasher>::Iterator
HashSet<T,EqualT,Hasher>::find(const Key& key) const
{
	UInt h = getIndex(key);
	for (Node* curNode = htable[h]; curNode != NULL; curNode = curNode->pNext)
		if (EqualT()(curNode->value.first,key))
			return Iterator(this,curNode);
	return end();
}

template<class T,class EqualT,class Hasher>
Void HashSet<T,EqualT,Hasher>::erase(Iterator it)
{
	UInt h = getIndex(it->pCurNode->value);

	if (htable[h] = it->pCurNode)
		htable[h] = htable[h]->pNext;
	else
	{
		Node* pCurNode = htable[h];
		while (pCurNode->pNext != it->pCurNode)
			pCurNode = pCurNode->pNext;
		pCurNode = pCurNode->pNext->pNext;
	}
	it->pCurNode->~Node();
	heap().deallocate(it->pCurNode);
}

template<class T,class EqualT,class Hasher>
Bool HashSet<T,EqualT,Hasher>::erase(ConstReference value)
{
	UInt h = getIndex(value);

	if (!htable[h])
		return false;

	if (htable[h]->value == value)
	{
		Node* tmp = htable[h];
		htable[h] = htable[h]->pNext;
		tmp->~Node();
		heap().deallocate(tmp);
	}
	else
	{
		Node* pCurNode = htable[h];
		while (pCurNode->pNext and pCurNode->pNext->value != value)
			pCurNode = pCurNode->pNext;
		if (!pCurNode->pNext)
			return false;
		Node* tmp = pCurNode->pNext;
		pCurNode = pCurNode->pNext->pNext;
		tmp->~Node();
		heap().deallocate(tmp);
	}
	return true;
}

template<class T,class EqualT,class Hasher>
template<class Key>
Bool HashSet<T,EqualT,Hasher>::erase(const Key& key)
{
	UInt h = getIndex(key);

	if (!htable[h])
		return false;

	if (htable[h]->value.first == key)
	{
		Node* tmp = htable[h];
		htable[h] = htable[h]->pNext;
		tmp->~Node();
		heap().deallocate(tmp);
	}
	else
	{
		Node* pCurNode = htable[h];
		while (pCurNode->pNext and pCurNode->pNext->value.first != key)
			pCurNode = pCurNode->pNext;
		if (!pCurNode->pNext)
			return false;
		Node* tmp = pCurNode->pNext;
		pCurNode = pCurNode->pNext->pNext;
		tmp->~Node();
		heap().deallocate(tmp);
	}
	return true;
}

template<class T,class EqualT,class Hasher>
inline Void HashSet<T,EqualT,Hasher>::clear()
{
	if (heap().doesGC())
		for (UInt i = 0; i < htable.size(); ++i)
			while (htable[i])
			{
				Node* tmp = htable[i];
				htable[i] = htable[i]->pNext;
				tmp->~Node();
			}
	else
		for (UInt i = 0; i < htable.size(); ++i)
			while (htable[i])
			{
				Node* tmp = htable[i];
				htable[i] = htable[i]->pNext;
				tmp->~Node();
				heap.deallocate(tmp);
			}
}

// ----------------

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
			maxInserts*sizeof(typename Super::Node)+
			static_cast<UInt>(1+ceil(maxInserts*cratio))*sizeof(typename Super::Node*)+
			//	(sizeof(typename Super::Colisions)+sizeof(typename Super::Colisions::NodeI))+
					sizeof(Int)*3),
			Super(this->heap(),static_cast<UInt>(ceil(maxInserts*cratio)))
	{}

	Heap heap() const {	return StaticMemHolder::heap();	}
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

}

template<class T,class E,class H>
struct ElemTraits<detail::HashSet<T,E,H> >
{	typedef T Elem; };

}


#endif /*CASPER_KERNEL_CONTAINER_HASHSET_H_*/
