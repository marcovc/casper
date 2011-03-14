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
 * Reversible single linked list.
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_SLIST
#define _H_CASPER_KERNEL_CONTAINER_SLIST

#include <casper/kernel/reversible/reversible.h>

namespace Casper {

struct RSListNodeI
{
	typedef RSListNodeI			Node;
	typedef Node*				PNode;
	typedef Reversible<PNode>	RPNode;

	RPNode 		next;
	RSListNodeI(State& s,const PNode& next) :
						next(s,next) {}
	RSListNodeI(const RPNode& next) :
						next(next) {}
};

template<class T>
struct RSListNode : public RSListNodeI
{
	typedef RSListNodeI		Super;
	typedef Super::PNode	PNodeI;
	typedef Super::RPNode	RPNode;

	T			elem;
	RSListNode(State& s,const T& elem, const PNodeI& next) :
						Super(s,next),elem(elem) {}
	RSListNode(const T& elem, const RPNode& next) :
						Super(next),elem(elem) {}
};

/**
 *	Reversible single linked list.
 * 	\ingroup Containers
 **/
template<class T>
struct SList
{
	typedef RSListNode<T>			Node;
	typedef RSListNodeI				NodeI;
	typedef typename Node::PNode	PNode;
	typedef typename Node::RPNode	RPNode;

	typedef T 					Value;
	typedef Value&				Reference;
	typedef const Value& 		ConstReference;
	typedef Value*				Pointer;
	typedef const Value* 		ConstPointer;
	typedef size_t				Difference;
	typedef size_t				Size;

	State&						state;
	RPNode						head;

	class Iterator;
	friend class Iterator;
	class Iterator : public std::iterator<std::forward_iterator_tag,
									 	  Value,
									 	  Difference>
	{
		friend struct SList;
		public:
		Iterator() : curNode(NULL) {}
		Iterator(PNode n) : curNode(n) {}
		Iterator(const Iterator& s) : curNode(s.curNode) {}
		bool operator==(const Iterator& s) const
		{ return curNode == s.curNode; }
		bool operator!=(const Iterator& s) const
		{ return curNode != s.curNode; }
		Reference operator*() const
		{ return ((Node*)curNode)->elem; }
		Pointer operator->() const
		{ return &((Node*)curNode)->elem; }
		Iterator& operator++() // pre-increment
		{ curNode = curNode->next; return *this; }
		Iterator operator++(int) //post-increment
		{ Iterator ret(*this); curNode = curNode->next; return ret; }
		const Iterator& operator=(const Iterator& i)
		{ curNode = i.curNode;	return *this; }
		//private:
		PNode	curNode;
	};

	typedef Iterator 					ConstIterator;

	SList(State& state);
	template <class InputIterator>
	SList(State& state,InputIterator first, InputIterator last);
	SList(const SList&);
	virtual ~SList();

	Iterator begin() const;
	Iterator end() const;
	bool empty() const;
	Size size() const;
	const Value&	front() const {	assert(!empty()); return *begin();	}
	Value&	front() {	assert(!empty()); return *begin();	}

	// TODO: Iterator insert(Iterator before, ConstReference value);
	Iterator pushFront(ConstReference value);
	// O(n):
	Iterator pushBack(ConstReference value);

	void popFront();
	// TODO: virtual bool erase(Iterator p, Iterator q);
	// TODO: virtual bool erase(Iterator p);
	// TODO: template <class InputIterator>
	// TODO: void insert(Iterator before, InputIterator first, InputIterator last);
	bool erase(ConstReference value);
	Iterator findFirst(ConstReference value) const;
	void eraseFirst(ConstReference value);

	Iterator findPred(Iterator it) const;
	void eraseAfter(Iterator it);
	void eraseAllAfter(Iterator it);

	void clear();
	State& getState() const {	return state;	}

	protected:
	template <class InputIterator>
	void initRange(InputIterator first, InputIterator last);
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
SList<T>::SList(State& state) : state(state),
								 	head(state,NULL)
{}

template<class T>
template <class InputIterator>
void SList<T>::initRange(InputIterator first, InputIterator last)
{
	PNode cur,prev;
	if (first != last)
	{
		cur = new (state.getHeap()) Node(state,*first,NULL);
		head = cur;
		while (++first != last)
		{
			prev = cur;
			cur = new (state.getHeap()) Node(state,*first,NULL);
			prev->next = cur;
		}
	}
}

template<class T>
template <class InputIterator>
SList<T>::SList(State& s, InputIterator first, InputIterator last) :
								state(s),
								head(s,NULL)
{	initRange(first,last);	}

template<class T>
SList<T>::SList(const SList<T>& s) :
								state(s.getState()),
								head(s.getState(),NULL)
{	initRange(s.begin(),s.end());	}

template<class T>
SList<T>::~SList()
{ /* garbage collection frees everything */ }

template<class T>
bool SList<T>::empty() const
{	return head.get() == NULL;	}

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
	PNodeI cur = new (state.getHeap()) Node(r,befPos.curNode->prev,
								RPNodeI(state,befPos.curNode));
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
	head = new (state) Node(state,value,head);
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
	PNode cur(head);
	while (cur->next != NULL)
		cur = cur->next;
	cur->next = new (state.getHeap()) Node(state,value,NULL);
	return Iterator(cur);
}

template<class T>
void
SList<T>::popFront()
{
	head = head.get()->next;
}

template<class T>
bool
SList<T>::erase(ConstReference value)
{
	bool found(false);

	while (!empty() && value == ((Node*)((NodeI*)head))->elem)
	{	popFront(); found = true;	}

	if (empty())
		return found;

	PNode cur(head);
	while (cur->next.get() != NULL)
	{
		if (((Node*)((NodeI*)(cur->next)))->elem == value)
		{
			cur->next = cur->next->next;
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

template<class T>
void SList<T>::eraseFirst(ConstReference value)
{
	if (value == *begin())
	{
		popFront();
		return;
	}
	PNode cur(head);
	while  (cur->next != NULL && ((Node*)cur->next.get())->elem != value)
		cur = cur->next;
	if (cur->next != NULL)
		cur->next = cur->next.get()->next;
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
void SList<T>::eraseAfter(Iterator it)
{
	it.curNode->next = it.curNode->next->next;
}

template<class T>
void SList<T>::eraseAllAfter(Iterator it)
{
	it.curNode->next = NULL;
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


} // Casper

/**
	Writes the contents of the SList to an ostream.
*/
template<typename T>
std::ostream& operator<<(std::ostream& o, const Casper::SList<T>& a)
{
	o << "{ ";
	for (typename Casper::SList<T>::ConstIterator it = a.begin();
		it != a.end(); ++it)
		o << *it << " ";
	o << "}";
	return o;
}



#endif // _H_CASPER_KERNEL_CONTAINER_SLIST

