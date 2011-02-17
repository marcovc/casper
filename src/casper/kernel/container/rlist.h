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
 * Reversible double linked list.
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_RLIST
#define _H_CASPER_KERNEL_CONTAINER_RLIST

#include <casper/kernel/reversible.h>

namespace casper {
namespace detail {

struct RListNodeI
{
	typedef RListNodeI			Node;
	typedef Node*				PNode;
	typedef Reversible<PNode>	RPNode;

	RPNode		prev;
	RPNode 		next;
	RListNodeI(const RPNode& prev, const RPNode& next) :
						prev(prev),next(next) {}
};

template<class T>
struct RListNode : public RListNodeI
{
	typedef RListNodeI		Super;
	typedef Super*				PNodeI;
	typedef Reversible<PNodeI>	RPNodeI;

	T			elem;
	RListNode(const T& elem, const RPNodeI& prev, const RPNodeI& next) :
						Super(prev,next),elem(elem) {}
	RListNode(ICPSolver& solver, const T& elem, const PNodeI& prev, const PNodeI& next):
						Super(RPNodeI(solver,prev),RPNodeI(solver,next)),elem(elem) {}
};

/**
 * 	Reversible double linked list.
 *	\ingroup Containers
 */
template<class T>
struct RList
{
	typedef RListNode<T>		Node;
	typedef RListNodeI			NodeI;
	typedef NodeI*				PNodeI;
	typedef Reversible<PNodeI> 	RPNodeI;

	typedef T 					Value;
	typedef Value&				Reference;
	typedef const Value& 		ConstReference;
	typedef Value*				Pointer;
	typedef const Value* 		ConstPointer;
	typedef size_t				Difference;
	typedef size_t				Size;

	ICPSolver&							solver;
	RPNodeI						head;
	RPNodeI						tail;

	class Iterator;
	friend class Iterator;
	class Iterator : public std::iterator<bidirectional_iterator_tag,
									 	  Value,
									 	  Difference>
	{
		friend struct RList;
		public:
		Iterator() : curNode(NULL) {}
		Iterator(PNodeI n) : curNode(n) {}
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
		Iterator& operator--() // pre-decrement
		{ curNode = curNode->prev; return *this; }
		Iterator operator--(int) //post-decrement
		{ Iterator ret(*this); curNode = curNode->prev; return ret; }
		private:
		PNodeI	curNode;
	};

	typedef Iterator 					ConstIterator;
	typedef reverse_iterator<Iterator> 	ReverseIterator;
	typedef ReverseIterator 			ConstReverseIterator;

	RList(ICPSolver& solver);
	template <class InputIterator>
	RList(ICPSolver& solver,InputIterator first, InputIterator last);
	RList(const RList&);
	virtual ~RList();

	Iterator begin() const;
	Iterator end() const;
	ReverseIterator rbegin() const {return ReverseIterator(end());}
	ReverseIterator rend() const {return ReverseIterator(begin());}
	bool empty() const;
	Size size() const;
	Iterator insert(Iterator before, ConstReference value);
	Iterator pushFront(ConstReference value);
	Iterator pushBack(ConstReference value);
	bool erase(ConstReference value);
	virtual bool erase(Iterator p, Iterator q);
	virtual bool erase(Iterator p);
	template <class InputIterator>
	void insert(Iterator before, InputIterator first, InputIterator last);
	Iterator findFirst(ConstReference value) const;
	void eraseFirst(ConstReference value)
	{	Iterator it = findFirst(value); if (it!=end()) erase(it);	}
	virtual void clear();

	bool testInternRep() {	return true;	}

	protected:
	template <class InputIterator>
	void initRange(InputIterator first, InputIterator last);
};

template<class T>
typename RList<T>::Iterator RList<T>::begin() const
{
	return typename RList<T>::Iterator(head);
}

template<class T>
typename RList<T>::Iterator RList<T>::end() const
{
	return typename RList<T>::Iterator(tail);
}

template<class T>
RList<T>::RList(ICPSolver& solver) :	solver(solver),
							head(typename RList<T>::RPNodeI(solver)),
							tail(typename RList<T>::RPNodeI(solver))
{
	PNodeI cur = new (solver.heap()) NodeI(typename RList<T>::RPNodeI(solver),
									    typename RList<T>::RPNodeI(solver));
	head = tail = cur;
}

template<class T>
template <class InputIterator>
void RList<T>::initRange(InputIterator first, InputIterator last)
{
	if (first != last)
	{
		PNodeI cur = new (solver.heap()) Node(*first,head,tail);
		head = cur;
		while (++first!=last)
		{
			cur = new (solver.heap()) Node(*first,RPNodeI(solver,cur),tail);
			cur->prev.get()->next = cur;
		}
		tail = new (solver.heap()) NodeI(RPNodeI(solver,cur),tail);
		tail.get()->prev.get()->next = tail;
	}
	else
	{
		PNodeI cur = new (solver.heap()) NodeI(head,tail);
		head = tail = cur;
	}
}

template<class T>
template <class InputIterator>
RList<T>::RList(ICPSolver& solver, InputIterator first, InputIterator last) :
								solver(solver),
								head(typename RList<T>::RPNodeI(solver)),
							    tail(typename RList<T>::RPNodeI(solver))
{	initRange(first,last);	}

template<class T>
RList<T>::RList(const RList<T>& s) :
								solver(s.solver),
								head(typename RList<T>::RPNodeI(solver)),
							   	tail(typename RList<T>::RPNodeI(solver))
{	initRange(s.begin(),s.end());	}

template<class T>
RList<T>::~RList()
{ /* garbage collection frees everything */ }

template<class T>
bool RList<T>::empty() const
{	return head == tail;	}

template<class T>
typename RList<T>::Size RList<T>::size() const
{
	typename RList::Size ret(0);
	for (typename RList<T>::ConstIterator it = begin();
						it != end(); ++it)
		ret++;
	return ret;
}

template<class T>
bool
RList<T>::erase(ConstReference value)
{
	Bool found(false);

	while (head!= tail && ((Node&)*head).elem == value)
	{
		head = head.get()->next;
		found = true;
	}
	PNodeI cur(head);

	while (cur != tail)
	{
		if (((Node&)*cur).elem == value)
		{
			cur->prev.get()->next = cur->next;
			cur->next.get()->prev = cur->prev;
			found = true;
		}
		cur = cur->next;
	}
	return found;
}

template<class T>
typename RList<T>::Iterator
RList<T>::insert(Iterator befPos, ConstReference r)
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

template<class T>
typename RList<T>::Iterator
RList<T>::pushFront(ConstReference value)
{
	return insert(Iterator(head),value);
}

template<class T>
typename RList<T>::Iterator
RList<T>::pushBack(ConstReference value)
{
	return insert(Iterator(tail),value);
}

template<class T>
bool RList<T>::erase(Iterator p, Iterator q)
{
	if (((PNodeI)head) != p.curNode)
		p.curNode->prev.get()->next = q.curNode;
	else
		head = q.curNode;
	q.curNode->prev = p.curNode->prev;
	return true;
}

template<class T>
bool RList<T>::erase(Iterator it)
{
	assert(it != end());
	if (((PNodeI)head) != it.curNode)
		it.curNode->prev.get()->next = it.curNode->next;
	else
		head = it.curNode->next;
	it.curNode->next.get()->prev = it.curNode->prev;
	return true;
}

template<class T>
void RList<T>::clear()
{
	head = tail;
}

template<class T>
template <class InputIterator>
void RList<T>::insert(Iterator befPos, InputIterator first, InputIterator last)
{
	while (first != last)
	{
		insert(befPos,*first);
		++first;
	}
}

template<class T>
typename RList<T>::Iterator
RList<T>::findFirst(ConstReference value) const
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

/**
	Writes the contents of the RList to an ostream.
*/
template<typename T>
std::ostream& operator<<(std::ostream& o, const casper::detail::RList<T>& a)
{
	o << "{ ";
	for (typename casper::detail::RList<T>::ConstIterator it = a.begin();
		it != a.end(); ++it)
		o << *it << " ";
	o << "}";
	return o;
}

};

};



#endif // _H_CASPER_KERNEL_CONTAINER_RLIST
