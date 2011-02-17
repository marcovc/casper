 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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
 * Single linked list.
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_SLIST
#define _H_CASPER_KERNEL_CONTAINER_SLIST

#include <iterator>
#include <casper/kernel/memory.h>
#include <casper/kernel/container/common.h>

namespace casper {
namespace detail {

struct SListNodeI
{
	typedef SListNodeI			Node;
	typedef Node*				PNode;

	PNode 		next;
	SListNodeI(const PNode& next) :
						next(next) {}
	virtual ~SListNodeI() {}
};

template<class T>
struct SListNode : public SListNodeI
{
	typedef SListNodeI		Super;
	typedef Super*			PNodeI;

	T			elem;
	SListNode(const T& elem, const PNodeI& next) :
						Super(next),elem(elem) {}
};

/**
 * 	Single linked list.
 * 	\ingroup Containers
 **/
template<class T>
struct SList
{
	typedef SListNode<T>		Node;
	typedef SListNodeI			NodeI;
	typedef NodeI*				PNodeI;

	typedef T 					Value;
	typedef Value&				Reference;
	typedef const Value& 		ConstReference;
	typedef Value*				Pointer;
	typedef const Value* 		ConstPointer;
	typedef size_t				Difference;
	typedef size_t				Size;

	Heap						mHeap;
	PNodeI						head;

	class Iterator;
	friend class Iterator;
	class Iterator : public std::iterator<std::forward_iterator_tag,
									 	  Value,
									 	  Difference>
	{
		friend struct SList;
		public:
		Iterator() : curNode(NULL) {}
		Iterator(PNodeI n) : curNode(n) {}
		Iterator(const Iterator& s) : curNode(s.curNode) {}
		bool operator==(const Iterator& s) const
		{ return curNode == s.curNode; }
		bool operator!=(const Iterator& s) const
		{ return curNode != s.curNode; }
		Reference operator*() const
		{ assert(curNode); return ((Node*)curNode)->elem; }
		Pointer operator->() const
		{ assert(curNode); return &((Node*)curNode)->elem; }
		Iterator& operator++() // pre-increment
		{ assert(curNode); curNode = curNode->next; return *this; }
		Iterator operator++(int) //post-increment
		{ assert(curNode); Iterator ret(*this); curNode = curNode->next; return ret; }
		const Iterator& operator=(const Iterator& i)
		{ curNode = i.curNode;	return *this; }
		//private:
		PNodeI	curNode;
	};

	typedef Iterator 					ConstIterator;

	inline SList(Heap heap = stdHeap);
	template <class InputIterator>
	SList(Heap heap,InputIterator first, InputIterator last);
	SList(const SList&);
	inline virtual ~SList();

	inline Iterator begin() const;
	inline Iterator end() const;
	bool empty() const;
	Size size() const;
	Iterator insertAfter(Iterator pos, ConstReference value);
	Iterator pushFront(ConstReference value);
	// O(n):
	Iterator pushBack(ConstReference value);

	void popFront();
	// TODO: virtual bool erase(Iterator p, Iterator q);
	// TODO: virtual bool erase(Iterator p);
	// TODO: template <class InputIterator>
	// TODO: void insert(Iterator before, InputIterator first, InputIterator last);
	bool erase(ConstReference value);
	void clear();
	Iterator findFirst(ConstReference value) const;
	void eraseFirst(ConstReference value);
	Iterator findPred(Iterator it) const;
	Void eraseAfter(Iterator it);
	Void eraseAllAfter(Iterator it);
	Heap heap() const {	return mHeap;	}

	bool testInternRep() {	return true;	}

	protected:
	template <class InputIterator>
	void initRange(InputIterator first, InputIterator last);
	void freeRange(Iterator first, Iterator last);
	void freeElem(PNodeI p);
};

template<class T>
typename SList<T>::Iterator SList<T>::begin() const
{
	return typename SList<T>::Iterator(head);
}

template<class T>
typename SList<T>::Iterator SList<T>::end() const
{
	return typename SList<T>::Iterator(NULL);
}

template<class T>
SList<T>::SList(Heap mHeap) : mHeap(mHeap),
								 head(NULL)
{}

template<class T>
template <class InputIterator>
void SList<T>::initRange(InputIterator first, InputIterator last)
{
	PNodeI cur,prev;
	if (first != last)
	{
		cur = new (heap()) Node(*first,NULL);
		head = cur;
		while (++first != last)
		{
			prev = cur;
			cur = new (heap()) Node(*first,NULL);
			prev->next = cur;
		}
	}
}

template<class T>
template <class InputIterator>
SList<T>::SList(Heap mHeap, InputIterator first, InputIterator last) :
								mHeap(mHeap),
								head(NULL)
{	initRange(first,last);	}

template<class T>
SList<T>::SList(const SList<T>& s) :
								mHeap(s.heap()),
								head(NULL)
{	initRange(s.begin(),s.end());	}

template<class T>
void SList<T>::freeRange(Iterator first, Iterator last)
{
	if (!heap().doesGC())
		while (first != last)
		{
			first.curNode->~NodeI();
			heap().deallocate((first++).curNode);
		}
	else
		while (first != last)
		{
			first.curNode->~NodeI();
			++first;
		}
}

template<class T>
SList<T>::~SList()
{
	freeRange(begin(),end());
}

template<class T>
bool SList<T>::empty() const
{	return head == NULL;	}

template<class T>
typename SList<T>::Size SList<T>::size() const
{
	typename SList::Size ret(0);
	for (typename SList<T>::ConstIterator it = begin();
						it != end(); ++it)
		ret++;
	return ret;
}

/*
template<class T>
typename SList<T>::Iterator
SList<T>::insert(Iterator befPos, ConstReference r)
{
	PNodeI cur = new (solver.heap()) Node(r,befPos.curNode->prev,
								RPNodeI(solver,befPos.curNode));
	cur->next.get()->prev = cur;
	if (head == befPos.curNode)
		head = cur;
	else
		cur->prev.get()->next = cur;
	return Iterator(cur);
}
*/

template<class T>
typename SList<T>::Iterator
SList<T>::pushFront(ConstReference value)
{
	head = new (heap()) Node(value,head);
	return Iterator(head);
}

/**
 * \note O(n) operation.
 */
template<class T>
typename SList<T>::Iterator
SList<T>::pushBack(ConstReference value)
{
	if (head == NULL)
		return pushFront(value);
	PNodeI cur(head);
	while (cur->next != NULL)
		cur = cur->next;
	cur->next = new (heap()) Node(value,NULL);
	return Iterator(cur->next);
}

template<class T>
void
SList<T>::popFront()
{
	PNodeI pToErase = head;
	head = head->next;
	freeElem(pToErase);
}

template<class T>
typename SList<T>::Iterator
SList<T>::insertAfter(Iterator pos, ConstReference value)
{
	PNodeI ret(new (heap()) Node(value,pos.curNode->next));
	pos.curNode->next = ret;
	return Iterator(ret);
}

template<class T>
bool
SList<T>::erase(ConstReference value)
{
	Bool found(false);

	if (empty())
		return false;

	while (value == ((Node&)*head).elem)
	{	popFront(); found = true;	}

	PNodeI cur(head);
	while (cur->next != NULL)
	{
		if (((Node&)*cur->next).elem == value)
		{
			PNodeI pToErase = cur->next;
			cur->next = cur->next->next;
			freeElem(pToErase);
			found = true;
		}
		else
		cur = cur->next;
	}
	return found;
}

template<class T>
typename SList<T>::Iterator
SList<T>::findFirst(ConstReference value) const
{
	Iterator ret(begin());
	while (ret != end())
	{
		if (*ret == value)
			return ret;
		++ret;
	}
	return ret;
}

// pre: it != begin()
template<class T>
typename SList<T>::Iterator
SList<T>::findPred(Iterator it) const
{
	assert(it != begin());
	Iterator ret(begin());
	while (ret.curNode->next != NULL and
		   ret.curNode->next != it.curNode)
		++ret;
	if (ret.curNode->next == it.curNode)
		return ret;
	else
		return end();
}

template<class T>
void SList<T>::eraseFirst(ConstReference value)
{
	if (value == *begin())
	{
		popFront();
		return;
	}
	PNodeI cur(head);
	while  (cur->next != NULL && ((Node*)cur->next)->elem != value)
		cur = cur->next;
	if (cur->next != NULL)
	{
		PNodeI	pToErase = cur->next;
		cur->next = cur->next->next;
		freeElem(pToErase);
	}
}

template<class T>
Void SList<T>::eraseAllAfter(Iterator it)
{
	freeRange(++Iterator(it),end());
	it.curNode->next = NULL;
}

template<class T>
Void SList<T>::eraseAfter(Iterator it)
{
	PNodeI	pToErase = it.curNode->next;
	it.curNode->next = it.curNode->next->next;
	freeElem(pToErase);
}

template<class T>
Void SList<T>::freeElem(PNodeI p)
{
	p->~NodeI();
	if (!heap().doesGC())
		heap().deallocate(p);
}

/*
template<class T>
typename SList<T>::Iterator
SList<T>::pushBack(ConstReference value)
{
	return insert(Iterator(tail),value);
}

template<class T>
bool SList<T>::erase(Iterator p, Iterator q)
{
	if (((PNodeI)head) != p.curNode)
		p.curNode->prev.get()->next = q.curNode;
	else
		head = q.curNode;
	q.curNode->prev = p.curNode->prev;
	return true;
}

template<class T>
bool SList<T>::erase(Iterator it)
{
	assert(it != end());
	if (((PNodeI)head) != it.curNode)
		it.curNode->prev.get()->next = it.curNode->next;
	else
		head = it.curNode->next;
	it.curNode->next.get()->prev = it.curNode->prev;
	return true;
}
*/
template<class T>
void SList<T>::clear()
{
	freeRange(begin(),end());
	head = NULL;
}
/*
template<class T>
template <class InputIterator>
void SList<T>::insert(Iterator befPos, InputIterator first, InputIterator last)
{
	while (first != last)
	{
		insert(befPos,*first);
		++first;
	}
}
*/

/**
	Writes the contents of the SList to an ostream.
*/
template<typename T>
std::ostream& operator<<(std::ostream& o, const casper::detail::SList<T>& a)
{
	o << "{ ";
	for (typename casper::detail::SList<T>::ConstIterator it = a.begin();
		it != a.end(); ++it)
		o << *it << " ";
	o << "}";
	return o;
}

/**
 * 	SList which allocates all memory at creation and frees at destruction.
 */
template<class T>
struct QuickSList : StaticMemHolder,SList<T>
{
	typedef SList<T> Super;

	/**
	 * Create a new SList allowing \a maxInserts insertions.
	 * \note maxInserts is the maximum number of insertions. This means that no
	 * more insertions are possible when this limit is reached, even if some
	 * elements are removed.
	 **/
	QuickSList(UInt maxInserts) :
		StaticMemHolder(maxInserts*
						(sizeof(Void*)*2+sizeof(typename Super::Node))+
						3*sizeof(Void*)),
		Super(this->heap())
	{}

	Heap heap() const {	return StaticMemHolder::heap();	}
};

};

template<class> struct EvalTraits;

template<class T>
struct EvalTraits<detail::SList<T> >
{	typedef Seq<T>	Eval;	};

};




#endif // _H_CASPER_KERNEL_CONTAINER_SLIST

