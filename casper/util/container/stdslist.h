 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_CONTAINER_STDSLIST
#define _H_CASPER_KERNEL_CONTAINER_STDSLIST

#include <iterator>
#include <casper/util/memory.h>
#include <casper/util/container/common.h>

namespace Casper {
namespace Util {


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
struct StdSList
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

	IHeap&						mHeap;
	PNodeI						head;

	class Iterator;
	friend class Iterator;
	class Iterator : public std::iterator<std::forward_iterator_tag,
									 	  Value,
									 	  Difference>
	{
		friend struct StdSList;
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

	inline StdSList(IHeap& heap = stdHeap());
	template <class InputIterator>
	StdSList(IHeap& heap,InputIterator first, InputIterator last);
	StdSList(const StdSList&);
	inline virtual ~StdSList();

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
	void eraseAfter(Iterator it);
	void eraseAllAfter(Iterator it);
	IHeap& getHeap() const {	return mHeap;	}

	bool testInternRep() {	return true;	}

	protected:
	template <class InputIterator>
	void initRange(InputIterator first, InputIterator last);
	void freeRange(Iterator first, Iterator last);
	void freeElem(PNodeI p);
};

template<class T>
typename StdSList<T>::Iterator StdSList<T>::begin() const
{
	return typename StdSList<T>::Iterator(head);
}

template<class T>
typename StdSList<T>::Iterator StdSList<T>::end() const
{
	return typename StdSList<T>::Iterator(NULL);
}

template<class T>
StdSList<T>::StdSList(IHeap& mHeap) : mHeap(mHeap),
								 head(NULL)
{}

template<class T>
template <class InputIterator>
void StdSList<T>::initRange(InputIterator first, InputIterator last)
{
	PNodeI cur,prev;
	if (first != last)
	{
		cur = new (getHeap()) Node(*first,NULL);
		head = cur;
		while (++first != last)
		{
			prev = cur;
			cur = new (getHeap()) Node(*first,NULL);
			prev->next = cur;
		}
	}
}

template<class T>
template <class InputIterator>
StdSList<T>::StdSList(IHeap& mHeap, InputIterator first, InputIterator last) :
								mHeap(mHeap),
								head(NULL)
{	initRange(first,last);	}

template<class T>
StdSList<T>::StdSList(const StdSList<T>& s) :
								mHeap(s.getHeap()),
								head(NULL)
{	initRange(s.begin(),s.end());	}

template<class T>
void StdSList<T>::freeRange(Iterator first, Iterator last)
{
	if (!getHeap().doesGC())
		while (first != last)
		{
			first.curNode->~NodeI();
			getHeap().deallocate((first++).curNode);
		}
	else
		while (first != last)
		{
			first.curNode->~NodeI();
			++first;
		}
}

template<class T>
StdSList<T>::~StdSList()
{
	freeRange(begin(),end());
}

template<class T>
bool StdSList<T>::empty() const
{	return head == NULL;	}

template<class T>
typename StdSList<T>::Size StdSList<T>::size() const
{
	typename StdSList::Size ret(0);
	for (typename StdSList<T>::ConstIterator it = begin();
						it != end(); ++it)
		ret++;
	return ret;
}

/*
template<class T>
typename StdSList<T>::Iterator
StdSList<T>::insert(Iterator befPos, ConstReference r)
{
	PNodeI cur = new (solver.getHeap()) Node(r,befPos.curNode->prev,
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
typename StdSList<T>::Iterator
StdSList<T>::pushFront(ConstReference value)
{
	head = new (getHeap()) Node(value,head);
	return Iterator(head);
}

/**
 * \note O(n) operation.
 */
template<class T>
typename StdSList<T>::Iterator
StdSList<T>::pushBack(ConstReference value)
{
	if (head == NULL)
		return pushFront(value);
	PNodeI cur(head);
	while (cur->next != NULL)
		cur = cur->next;
	cur->next = new (getHeap()) Node(value,NULL);
	return Iterator(cur->next);
}

template<class T>
void
StdSList<T>::popFront()
{
	PNodeI pToErase = head;
	head = head->next;
	freeElem(pToErase);
}

template<class T>
typename StdSList<T>::Iterator
StdSList<T>::insertAfter(Iterator pos, ConstReference value)
{
	PNodeI ret(new (getHeap()) Node(value,pos.curNode->next));
	pos.curNode->next = ret;
	return Iterator(ret);
}

template<class T>
bool
StdSList<T>::erase(ConstReference value)
{
	bool found(false);

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
typename StdSList<T>::Iterator
StdSList<T>::findFirst(ConstReference value) const
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
typename StdSList<T>::Iterator
StdSList<T>::findPred(Iterator it) const
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
void StdSList<T>::eraseFirst(ConstReference value)
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
void StdSList<T>::eraseAllAfter(Iterator it)
{
	freeRange(++Iterator(it),end());
	it.curNode->next = NULL;
}

template<class T>
void StdSList<T>::eraseAfter(Iterator it)
{
	PNodeI	pToErase = it.curNode->next;
	it.curNode->next = it.curNode->next->next;
	freeElem(pToErase);
}

template<class T>
void StdSList<T>::freeElem(PNodeI p)
{
	p->~NodeI();
	if (!getHeap().doesGC())
		getHeap().deallocate(p);
}

/*
template<class T>
typename StdSList<T>::Iterator
StdSList<T>::pushBack(ConstReference value)
{
	return insert(Iterator(tail),value);
}

template<class T>
bool StdSList<T>::erase(Iterator p, Iterator q)
{
	if (((PNodeI)head) != p.curNode)
		p.curNode->prev.get()->next = q.curNode;
	else
		head = q.curNode;
	q.curNode->prev = p.curNode->prev;
	return true;
}

template<class T>
bool StdSList<T>::erase(Iterator it)
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
void StdSList<T>::clear()
{
	freeRange(begin(),end());
	head = NULL;
}
/*
template<class T>
template <class InputIterator>
void StdSList<T>::insert(Iterator befPos, InputIterator first, InputIterator last)
{
	while (first != last)
	{
		insert(befPos,*first);
		++first;
	}
}
*/

/**
 * 	StdSList which allocates all memory at creation and frees at destruction.
 */
template<class T>
struct QuickSList : Detail::StaticMemHolder,StdSList<T>
{
	typedef StdSList<T> Super;

	/**
	 * Create a new StdSList allowing \a maxInserts insertions.
	 * \note maxInserts is the maximum number of insertions. This means that no
	 * more insertions are possible when this limit is reached, even if some
	 * elements are removed.
	 **/
	QuickSList(uint maxInserts) :
		Detail::StaticMemHolder(maxInserts*
						(sizeof(void*)*2+sizeof(typename Super::Node))+
						3*sizeof(void*)),
		Super(this->getHeap())
	{}

	IHeap& getHeap() const {	return StaticMemHolder::getHeap();	}
};
} // Util

}; // Casper



/**
	Writes the contents of the StdSList to an ostream.
*/
template<typename T>
std::ostream& operator<<(std::ostream& o, const Casper::Util::StdSList<T>& a)
{
	o << "{ ";
	for (typename Casper::Util::StdSList<T>::ConstIterator it = a.begin();
		it != a.end(); ++it)
		o << *it << " ";
	o << "}";
	return o;
}


#endif // _H_CASPER_KERNEL_CONTAINER_STDSLIST

