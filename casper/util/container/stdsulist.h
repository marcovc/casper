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
 * Reversible sorted unique list (each element is unique and the
 * list is always sorted).
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_STDSULIST
#define _H_CASPER_KERNEL_CONTAINER_STDSULIST

#include <functional>

#include <casper/util/container/common.h>
#include <casper/util/container/stdpair.h>
#include <casper/util/container/sciterator.h>

namespace Casper {
namespace Util {


struct StdSUListNodeI
{
	typedef StdSUListNodeI		Node;
	typedef Node*				PNode;

	PNode		prev;
	PNode 		next;
	StdSUListNodeI(const PNode& prev, const PNode& next) :
						prev(prev),next(next) {}
	virtual ~StdSUListNodeI() {}
};

template<class T>
struct StdSUListNode : StdSUListNodeI
{
	typedef StdSUListNodeI		Super;
	typedef Super*				PNodeI;

	T			elem;
	StdSUListNode(const T& elem, const PNodeI& prev, const PNodeI& next) :
						Super(prev,next),elem(elem) {}
};

/**
 *  Reversible sorted unique list (each element is unique and the
 * 	list is always sorted).
 * 	\ingroup Containers
 **/
template<class T,class Compare = std::less<T> >
struct StdSUList
{
	typedef StdSUListNode<T>	Node;
	typedef StdSUListNodeI		NodeI;
	typedef Node*				PNode;
	typedef NodeI*				PNodeI;

	typedef T 					Value;
	typedef Value&				Reference;
	typedef const Value& 		ConstReference;
	typedef Value*				Pointer;
	typedef const Value* 		ConstPointer;
	typedef std::size_t			Difference;
	typedef std::size_t			Size;
	typedef Value				Key;

	IHeap&						mHeap;
	PNodeI						head;
	PNodeI						tail;

	class Iterator;
	friend class Iterator;
	class Iterator : public std::iterator<bidirectional_iterator_tag,
									 	  Value,
									 	  Difference>
	{
		friend struct StdSUList;
		public:
		Iterator() : curNode(NULL) {}
		Iterator(PNodeI n) : curNode(n) {}
		Iterator(const Iterator& s)
		{this->curNode = s.curNode;}
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

	typedef	SCIterator<Value,Difference>	scit;

	StdSUList(IHeap& heap = stdHeap);
	template <class InputIterator>
	StdSUList(IHeap& heap,InputIterator first, InputIterator last);
	template <class InputIterator>
	StdSUList(InputIterator first, InputIterator last);

	StdSUList(const StdSUList&);
	virtual ~StdSUList();

	Iterator begin() const;
	Iterator end() const;
	ReverseIterator rbegin() const {return ReverseIterator(end());}
	ReverseIterator rend() const {return ReverseIterator(begin());}
	bool empty() const;
	Size size() const;
	Iterator find(const Key& k) const;
	Size count(const Key& k) const;
	StdPair<Iterator, Iterator> equalRange(const Key& k) const;
	Iterator lowerBound(const Key& k) const;
	Iterator upperBound(const Key& k) const;
	StdPair<Iterator,bool> insert(ConstReference);
	Iterator insert(Iterator hint, ConstReference value);
	Size erase(const Key& k);
	virtual bool erase(Iterator p, Iterator q);
	virtual bool erase(Iterator p);
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last);
	virtual void clear();
	bool operator==(const StdSUList&) const;
	IHeap& getHeap() const {	return mHeap;	}

	bool testInternRep() {	return true;	}

	protected:
	template <class InputIterator>
	void initRange(InputIterator first, InputIterator last);
	void freeForward(Iterator p, Iterator q);
};

template<class T,class Compare>
typename StdSUList<T,Compare>::Iterator StdSUList<T,Compare>::begin() const
{
	return typename StdSUList<T>::Iterator(head);
}

template<class T,class Compare>
typename StdSUList<T,Compare>::Iterator StdSUList<T,Compare>::end() const
{
	return typename StdSUList<T>::Iterator(tail);
}

template<class T,class Compare>
StdSUList<T,Compare>::StdSUList(IHeap& mHeap) : mHeap(mHeap)
{
	PNodeI cur = new (getHeap()) NodeI(NULL,NULL);
	head = tail = cur;
}

template<class T,class Compare>
template <class InputIterator>
void StdSUList<T,Compare>::initRange(InputIterator first, InputIterator last)
{
	if (first != last)
	{
		PNodeI cur = new (getHeap()) Node(*first,head,tail);
		head = cur;
		while (++first!=last)
		{
			cur = new (getHeap()) Node(*first,cur,tail);
			cur->prev->next = cur;
		}
		tail = new (getHeap()) NodeI(cur,tail);
		tail->prev->next = tail;
	}
	else
	{
		PNodeI cur = new (getHeap()) NodeI(head,tail);
		head = tail = cur;
	}
}

/// \note Requires range [first,last) to be sorted.
template<class T,class Compare>
template <class InputIterator>
StdSUList<T,Compare>::StdSUList(IHeap& heap, InputIterator first, InputIterator last) :
								mHeap(heap)
{
	assert(isStrictOrdered(first,last));
	initRange(first,last);
}

/// \note Requires range [first,last) to be sorted.
template<class T,class Compare>
template <class InputIterator>
StdSUList<T,Compare>::StdSUList(InputIterator first, InputIterator last) : mHeap(stdHeap)
{
	assert(isStrictOrdered(first,last));
	initRange(first,last);
}

template<class T,class Compare>
StdSUList<T,Compare>::StdSUList(const StdSUList<T,Compare>& s) :
								mHeap(s.mHeap)
{	initRange(s.begin(),s.end());	}

template<class T,class Compare>
StdSUList<T,Compare>::~StdSUList()
{
	clear();
	tail->~NodeI();
	getHeap().deallocate(tail);
}

template<class T,class Compare>
bool StdSUList<T,Compare>::empty() const
{	return head == tail;	}

template<class T,class Compare>
typename StdSUList<T,Compare>::Size StdSUList<T,Compare>::size() const
{
	typename StdSUList::Size ret(0);
	for (typename StdSUList<T,Compare>::ConstIterator it = begin();
						it != end(); ++it)
		ret++;
	return ret;
}

/// linear search
template<class T,class Compare>
typename StdSUList<T,Compare>::Iterator
StdSUList<T,Compare>::find(const Key& k) const
{
	if (empty() or TestRel<T,Compare>::greater(k,*--end()))
		return end();

	for (typename StdSUList<T,Compare>::Iterator it = begin(); it != end(); ++it)
		if (TestRel<T,Compare>::equal(k,*it))
			return it;
		else
		if (TestRel<T,Compare>::less(k,*it))
			break;
	return end();
}

template<class T,class Compare>
typename StdSUList<T,Compare>::Size
StdSUList<T,Compare>::count(const Key& k) const
{
	return find(k) != end();
}

template<class T,class Compare>
StdPair<typename StdSUList<T,Compare>::Iterator,
	 typename StdSUList<T,Compare>::Iterator>
StdSUList<T,Compare>::equalRange(const Key& k) const
{
	typedef StdPair<typename StdSUList<T>::Iterator,
				 typename StdSUList<T>::Iterator> p_t;
	typename StdSUList<T,Compare>::Iterator it = find(k);
	return p_t(it,it);
}

template<class T,class Compare>
typename StdSUList<T,Compare>::Iterator
StdSUList<T,Compare>::lowerBound(const Key& k) const
{
	PNodeI aux(head);
	while (aux != tail and ((PNode)aux)->elem < k)
		aux = aux->next;
	return Iterator(aux);
}

template<class T,class Compare>
typename StdSUList<T,Compare>::Iterator
StdSUList<T,Compare>::upperBound(const Key& k) const
{
	PNodeI aux(tail);
	while (aux != head && ((PNode)aux->prev)->elem > k)
		aux = aux->prev;
	return Iterator(aux);
}

template<class T,class Compare>
StdPair<typename StdSUList<T,Compare>::Iterator,bool>
StdSUList<T,Compare>::insert(ConstReference r)
{
	StdPair<typename StdSUList<T,Compare>::Iterator,bool> ret(begin(),false);
	PNodeI cur;
	for ( ; ret.first != end(); ++ret.first)
	{
		if (TestRel<T,Compare>::greater(*ret.first,r))
		{
			cur = new (getHeap()) Node(r,ret.first.curNode->prev,
									  ret.first.curNode);
			cur->next->prev = cur;
			if (head == ret.first.curNode)
				head = cur;
			else
				cur->prev->next = cur;
			ret.first = cur;
			ret.second = true;
			return ret;
		}
		else
		if (TestRel<T,Compare>::equal(*ret.first,r))
			return ret;
	}
	cur = new (getHeap()) Node(r,tail->prev,tail);
	cur->next->prev = cur;
	if (head == tail)
		head = cur;
	else
		cur->prev->next = cur;
	ret.first = cur;
	ret.second = true;
	return ret;
}

template<class T,class Compare>
typename StdSUList<T,Compare>::Iterator
StdSUList<T,Compare>::insert(typename StdSUList<T,Compare>::Iterator hint,
						   ConstReference r)
{
	PNodeI cur;
	for ( ; hint != end(); ++hint)
	{
		if (TestRel<T,Compare>::greater(*hint,r))
		{
			cur = new (getHeap()) Node(r,hint.curNode->prev,
									  hint.curNode);
			cur->next->prev = cur;
			if (head == hint.curNode)
				head = cur;
			else
				cur->prev->next = cur;
			hint = cur;
			return hint;
		}
		else
		if (TestRel<T,Compare>::equal(*hint,r))
			return hint;
	}
	cur = new (getHeap()) Node(r,tail->prev,tail);
	cur->next->prev = cur;
	if (head == tail)
		head = cur;
	else
		cur->prev->next = cur;
	hint = cur;
	return hint;
}

template<class T,class Compare>
typename StdSUList<T,Compare>::Size
StdSUList<T,Compare>::erase(const Key& k)
{
	typename StdSUList<T,Compare>::Iterator it = find(k);
	if (it != end())
	{
		erase(it);
		return 1;
	}
	return 0;
}

template<class T,class Compare>
bool StdSUList<T,Compare>::erase(Iterator p, Iterator q)
{
	if (((PNodeI)head) != p.curNode)
		p.curNode->prev->next = q.curNode;
	else
		head = q.curNode;
	q.curNode->prev = p.curNode->prev;
	freeForward(p,q);
	return true;
}


template<class T,class Compare>
void StdSUList<T,Compare>::clear()
{
	freeForward(head,tail);
	head = tail;
}

template<class T,class Compare>
bool StdSUList<T,Compare>::erase(Iterator it)
{
	assert(it != end());
	if (((PNodeI)head) != it.curNode)
		it.curNode->prev->next = it.curNode->next;
	else
		head = it.curNode->next;
	it.curNode->next->prev = it.curNode->prev;
	if (!getHeap().doesGC())
	{
		it.curNode->~NodeI();
		getHeap().deallocate(it.curNode);
	}
	return true;
}

template<class T,class Compare>
template <class InputIterator>
void StdSUList<T,Compare>::insert(InputIterator first, InputIterator last)
{
	assert(isStrictOrdered(first,last));
	for (Iterator it = begin(); it != end(); )
	{
		if (first == last)
			return;
		if (TestRel<T,Compare>::greater(*it,*first))
		{
			insert(it,*first);
			first++;
		}
		else
		if (TestRel<T,Compare>::less(*it,*first))
			it++;
		else
		{
			++it;
			++first;
		}
	}
	while (first != last)
	{
		insert(end(),*first);
		++first;
	}
}

template<class T,class Compare>
bool StdSUList<T,Compare>::operator==(const StdSUList& s) const
{
	Iterator itt = begin();
	Iterator its = s.begin();
	while (itt != end() and TestRel<T,Compare>::equal(*itt,*its))
	{
		++itt;
		++its;
	}
	return itt == end();
}

// free memory of elements in [p,q[
template<class T,class Compare>
void StdSUList<T,Compare>::freeForward(Iterator p, Iterator q)
{
	if (!getHeap().doesGC())
		while (p != q)
		{
			p.curNode->~NodeI();
			getHeap().deallocate((p++).curNode);
		}
	else
		while (p != q)
		{
			p.curNode->~NodeI();
			++p;
		}
}

} // Util

namespace Traits {
template<class> struct GetEval<Util::StdSUList<T> >
{	typedef Seq<T>	Type;	};

template<class> struct GetElem<Util::StdSUList<T> >
{	typedef T	Type;	};
} // Traits

} // Casper

/**
	Writes the contents of the sulist to an ostream.
*/
template<typename T,class Compare>
std::ostream& operator<<(std::ostream& o, const Casper::Util::StdSUList<T,Compare>& a)
{
	if (a.size()==1)
		o << *a.begin();
	else
	{
		o << "{ ";
		for (typename Casper::Util::StdSUList<T,Compare>::ConstIterator it = a.begin();
			it != a.end(); ++it)
			o << *it << " ";
		o << "}";
	}
	return o;
}



#endif // _H_CASPER_KERNEL_CONTAINER_SULIST

