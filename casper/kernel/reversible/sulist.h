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
 * Reversible sorted unique list (each element is unique and the
 * list is always sorted).
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_SULIST
#define _H_CASPER_KERNEL_CONTAINER_SULIST

#include <iterator>

#include <functional>

#include <casper/kernel/reversible/reversible.h>
#include <casper/util/container/common.h>
#include <casper/util/container/stdpair.h>
#include <casper/util/container/sciterator.h>

namespace Casper {

struct RSUListNodeI
{
	typedef RSUListNodeI		Node;
	typedef Node*				PNode;
	typedef Reversible<PNode>	RPNode;

	RPNode		prev;
	RPNode 		next;

	RSUListNodeI(const RPNode& prev, const RPNode& next) :
						prev(prev),next(next) {}
};

template<class T>
struct RSUListNode : public RSUListNodeI
{
	typedef RSUListNodeI		Super;
	typedef Super*				PNodeI;
	typedef Reversible<PNodeI>	RPNodeI;

	T			elem;
	RSUListNode(const T& elem, const RPNodeI& prev, const RPNodeI& next) :
						Super(prev,next),elem(elem) {}
	RSUListNode(State& state, const T& elem, const PNodeI& prev, const PNodeI& next):
						Super(RPNodeI(state,prev),RPNodeI(state,next)),elem(elem) {}
};

/**
 *  Reversible sorted unique list (each element is unique and the
 * 	list is always sorted).
 * 	\ingroup Containers
 **/
template<class T,class Compare = std::less<T> >
struct SUList
{
	typedef SUList<T,Compare>	Self;
	typedef RSUListNode<T>		Node;
	typedef RSUListNodeI		NodeI;
	typedef Node*				PNode;
	typedef NodeI*				PNodeI;
	typedef Reversible<PNodeI> 	RPNodeI;

	typedef T 					Value;
	typedef Value&				Reference;
	typedef const Value& 		ConstReference;
	typedef Value*				Pointer;
	typedef const Value* 		ConstPointer;
	typedef size_t				Difference;
	typedef size_t				Size;
	typedef Value				Key;

	State&						state;
	RPNodeI						head;
	RPNodeI						tail;

	class Iterator;
	friend class Iterator;
	class Iterator : public std::iterator<std::bidirectional_iterator_tag,
									 	  Value,
									 	  Difference>
	{
		friend struct SUList;
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
	typedef std::reverse_iterator<Iterator> 	ReverseIterator;
	typedef ReverseIterator 			ConstReverseIterator;

	typedef	Util::SCIterator<Value,Difference>	scit;

	SUList(State& state);
	template <class InputIterator>
	SUList(State& state,InputIterator first, InputIterator last);
	SUList(const SUList&);
	virtual ~SUList();

	Iterator begin() const;
	Iterator end() const;
	ReverseIterator rbegin() const {return ReverseIterator(end());}
	ReverseIterator rend() const {return ReverseIterator(begin());}
	bool empty() const;
	Size size() const;
	template<class K>
	Iterator find(const K& k) const;
	Iterator find(const Key& k) const;
	Size count(const Key& k) const;
	Util::StdPair<Iterator, Iterator> equalRange(const Key& k) const;
	template<class K>
	Iterator lowerBound(const K& k) const;
	Iterator lowerBound(const Key& k) const;
	template<class K>
	Iterator upperBound(const K& k) const;
	Iterator upperBound(const Key& k) const;
	Util::StdPair<Iterator,bool> insert(ConstReference);
	Iterator insert(Iterator hint, ConstReference value);
	Size erase(const Key& k);
	virtual bool erase(Iterator p, Iterator q);
	virtual bool erase(Iterator p);
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last);
	virtual void clear();
	bool operator==(const SUList&) const;
	State&	getState() const	{	return state;	}
	const Value& min() const { assert(!empty()); return *begin(); }
	const Value& max() const { assert(!empty()); return *--end(); }
	void updateMin(const Key& k);
	void updateMax(const Key& k);
	inline void updateMin(Iterator it);
	inline void updateMax(Iterator it);

	bool testInternRep() {	return true;	}

	protected:
	template <class InputIterator>
	void initRange(InputIterator first, InputIterator last);
};

template<class T,class Compare>
typename SUList<T,Compare>::Iterator SUList<T,Compare>::begin() const
{
	return typename SUList<T,Compare>::Iterator(head);
}

template<class T,class Compare>
typename SUList<T,Compare>::Iterator SUList<T,Compare>::end() const
{
	return typename SUList<T,Compare>::Iterator(tail);
}

template<class T,class Compare>
SUList<T,Compare>::SUList(State& state) :
							   state(state),
							   head(typename Self::RPNodeI(state)),
							   tail(typename Self::RPNodeI(state))
{
	PNodeI cur = new (state.getHeap()) NodeI(typename Self::RPNodeI(state),
									    typename Self::RPNodeI(state));
	head = tail = cur;
}

template<class T,class Compare>
template <class InputIterator>
void SUList<T,Compare>::initRange(InputIterator first, InputIterator last)
{
	if (first != last)
	{
		PNodeI cur = new (state.getHeap()) Node(*first,head,tail);
		head = cur;
		while (++first!=last)
		{
			cur = new (state.getHeap()) Node(*first,RPNodeI(state,cur),tail);
			cur->prev.get()->next = cur;
		}
		tail = new (state.getHeap()) NodeI(RPNodeI(state,cur),tail);
		tail.get()->prev.get()->next = tail;
	}
	else
	{
		PNodeI cur = new (state.getHeap()) NodeI(head,tail);
		head = tail = cur;
	}
}

/// \note Requires range [first,last) to be sorted.
template<class T,class Compare>
template <class InputIterator>
SUList<T,Compare>::SUList(State& state, InputIterator first, InputIterator last) :
								state(state),
								head(typename Self::RPNodeI(state)),
							    tail(typename Self::RPNodeI(state))
{
	assert(Casper::Util::Detail::isStrictOrdered(first,last));
	initRange(first,last);
}

template<class T,class Compare>
SUList<T,Compare>::SUList(const SUList<T,Compare>& s) :
								state(s.state),
								head(typename Self::RPNodeI(state)),
							   	tail(typename Self::RPNodeI(state))
{	initRange(s.begin(),s.end());	}

template<class T,class Compare>
SUList<T,Compare>::~SUList()
{ /* garbage collection frees everything */ }

template<class T,class Compare>
bool SUList<T,Compare>::empty() const
{	return head == tail;	}

template<class T,class Compare>
typename SUList<T,Compare>::Size SUList<T,Compare>::size() const
{
	typename SUList::Size ret(0);
	for (typename SUList<T,Compare>::ConstIterator it = begin();
						it != end(); ++it)
		ret++;
	return ret;
}

/// linear search
template<class T,class Compare>
typename SUList<T,Compare>::Iterator
SUList<T,Compare>::find(const Key& k) const
{
	if (empty() or Util::Detail::TestRel<T,Compare>::greater(k,*--end()))
		return end();

	for (typename SUList<T,Compare>::Iterator it = begin(); it != end(); ++it)
		if (Util::Detail::TestRel<T,Compare>::equal(k,*it))
			return it;
		else
		if (Util::Detail::TestRel<T,Compare>::less(k,*it))
			break;
	return end();
}

/// linear search
template<class T,class Compare>
template<class K>
typename SUList<T,Compare>::Iterator
SUList<T,Compare>::find(const K& k) const
{
	if (empty() or Util::Detail::TestRel<T,Compare>::greater(k,*--end()))
		return end();

	for (typename SUList<T,Compare>::Iterator it = begin(); it != end(); ++it)
		if (Util::Detail::TestRel<T,Compare>::equal(k,*it))
			return it;
		else
		if (Util::Detail::TestRel<T,Compare>::less(k,*it))
			break;
	return end();
}

template<class T,class Compare>
typename SUList<T,Compare>::Size
SUList<T,Compare>::count(const Key& k) const
{
	return find(k) != end();
}

template<class T,class Compare>
Util::StdPair<typename SUList<T,Compare>::Iterator,
	 typename SUList<T,Compare>::Iterator>
SUList<T,Compare>::equalRange(const Key& k) const
{
	typedef Util::StdPair<typename Self::Iterator,
				 typename Self::Iterator> p_t;
	typename SUList<T,Compare>::Iterator it = find(k);
	return p_t(it,it);
}

template<class T,class Compare>
typename SUList<T,Compare>::Iterator
SUList<T,Compare>::lowerBound(const Key& k) const
{
	PNodeI aux(head);
	while (aux != tail and Util::Detail::TestRel<T,Compare>::less(((PNode)aux)->elem,k))
		aux = aux->next;
	return Iterator(aux);
}

// works with abstract type K, which must implement std::less
template<class T,class Compare>
template<class K>
typename SUList<T,Compare>::Iterator
SUList<T,Compare>::lowerBound(const K& k) const
{
	PNodeI aux(head);
	while (aux != tail and Util::Detail::TestRel<T,Compare>::less(((PNode)aux)->elem,k))
		aux = aux->next;
	return Iterator(aux);
}

template<class T,class Compare>
typename SUList<T,Compare>::Iterator
SUList<T,Compare>::upperBound(const Key& k) const
{
	PNodeI aux(tail);
	while (aux != head and Util::Detail::TestRel<T,Compare>::less(k,((PNode)aux->prev.get())->elem))
		aux = aux->prev;
	return Iterator(aux);
}

template<class T,class Compare>
template<class K>
typename SUList<T,Compare>::Iterator
SUList<T,Compare>::upperBound(const K& k) const
{
	PNodeI aux(tail);
	while (aux != head and Util::Detail::TestRel<T,Compare>::less(k,((PNode)aux->prev.get())->elem))
		aux = aux->prev;
	return Iterator(aux);
}

template<class T,class Compare>
Util::StdPair<typename SUList<T,Compare>::Iterator,bool>
SUList<T,Compare>::insert(ConstReference r)
{
	Util::StdPair<typename SUList<T,Compare>::Iterator,bool> ret(begin(),false);
	PNodeI cur;
	for ( ; ret.first != end(); ++ret.first)
	{
		if (Util::Detail::TestRel<T,Compare>::greater(*ret.first,r))
		{
			cur = new (state.getHeap()) Node(r,ret.first.curNode->prev,
										  RPNodeI(state,ret.first.curNode));
			cur->next.get()->prev = cur;
			if (head == ret.first.curNode)
				head = cur;
			else
				cur->prev.get()->next = cur;
			ret.first = cur;
			ret.second = true;
			return ret;
		}
		else
		if (Util::Detail::TestRel<T,Compare>::equal(*ret.first,r))
			return ret;
	}
	cur = new (state.getHeap()) Node(r,tail.get()->prev,tail);
	cur->next.get()->prev = cur;
	if (head == tail)
		head = cur;
	else
		cur->prev.get()->next = cur;
	ret.first = cur;
	ret.second = true;
	return ret;
}

template<class T,class Compare>
typename SUList<T,Compare>::Iterator
SUList<T,Compare>::insert(typename SUList<T,Compare>::Iterator hint,
						   ConstReference r)
{
	PNodeI cur;
	for ( ; hint != end(); ++hint)
	{
		if (Util::Detail::TestRel<T,Compare>::greater(*hint,r))
		{
			cur = new (state.getHeap()) Node(r,hint.curNode->prev,
										  RPNodeI(state,hint.curNode));
			cur->next.get()->prev = cur;
			if (head == hint.curNode)
				head = cur;
			else
				cur->prev.get()->next = cur;
			hint = cur;
			return hint;
		}
		else
		if (Util::Detail::TestRel<T,Compare>::equal(*hint,r))
			return hint;
	}
	cur = new (state.getHeap()) Node(r,tail.get()->prev,tail);
	cur->next.get()->prev = cur;
	if (head == tail)
		head = cur;
	else
		cur->prev.get()->next = cur;
	hint = cur;
	return hint;
}

template<class T,class Compare>
typename SUList<T,Compare>::Size
SUList<T,Compare>::erase(const Key& k)
{
	typename SUList<T,Compare>::Iterator it = find(k);
	if (it != end())
	{
		erase(it);
		return 1;
	}
	return 0;
}

template<class T,class Compare>
bool SUList<T,Compare>::erase(Iterator p, Iterator q)
{
	if (((PNodeI)head) != p.curNode)
		p.curNode->prev.get()->next = q.curNode;
	else
		head = q.curNode;
	q.curNode->prev = p.curNode->prev;
	return true;
}

template<class T,class Compare>
void SUList<T,Compare>::updateMin(const Key& k)
{
	PNodeI aux(head);
	while (aux != tail && ((PNode)aux)->elem < k)
		aux = aux->next;
	head = aux;
}

template<class T,class Compare>
void SUList<T,Compare>::updateMax(const Key& k)
{
	PNodeI aux(tail);
	while (aux != head && ((PNode)aux->prev.get())->elem > k)
		aux = aux->prev;
	tail = aux;
}

template<class T,class Compare>
inline void SUList<T,Compare>::updateMin(Iterator it)
{
	head = it.curNode;
	it.curNode->prev = NULL;
}

template<class T,class Compare>
inline void SUList<T,Compare>::updateMax(Iterator it)
{
	assert(it != end());
	it.curNode->next = tail;
	tail->prev = it.curNode;
}

template<class T,class Compare>
void SUList<T,Compare>::clear()
{
	head = tail;
}

template<class T,class Compare>
bool SUList<T,Compare>::erase(Iterator it)
{
	assert(it != end());
	if (((PNodeI)head) != it.curNode)
		it.curNode->prev->next = it.curNode->next;
	else
		head = it.curNode->next;
	it.curNode->next->prev = it.curNode->prev;
	return true;
}

template<class T,class Compare>
template <class InputIterator>
void SUList<T,Compare>::insert(InputIterator first, InputIterator last)
{
	assert(Util::Detail::isStrictOrdered(first,last));
	for (Iterator it = begin(); it != end(); )
	{
		if (first == last)
			return;
		if (Util::Detail::TestRel<T,Compare>::greater(*it,*first))
		{
			insert(it,*first);
			first++;
		}
		else
		if (Util::Detail::TestRel<T,Compare>::less(*it,*first))
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
bool SUList<T,Compare>::operator==(const SUList& s) const
{
	Iterator itt = begin();
	Iterator its = s.begin();
	while (itt != end() and Util::Detail::TestRel<T,Compare>::equal(*itt,*its))
	{
		++itt;
		++its;
	}
	return itt == end();
}

namespace Traits {
template<class T,class Comp>
struct GetEval<SUList<T,Comp> >
{	typedef Seq<T>	Type;	};

template<class T,class Comp>
struct GetElem<SUList<T,Comp> >
{	typedef T	Type;	};

template<class T,class Comp>
struct GetTermElem<SUList<T,Comp> >
{	typedef typename GetElem<T>::Type	Type;	};

} // Traits

} // Casper


/**
	Writes the contents of the SUList to an ostream.
*/
template<typename T,class Compare>
std::ostream& operator<<(std::ostream& o, const Casper::SUList<T,Compare>& a)
{
/*	if (a.size()==1)
		o << *a.begin();
	else
*/	{
		o << "{ ";
		for (typename Casper::SUList<T,Compare>::ConstIterator it = a.begin();
			it != a.end(); ++it)
			o << *it << " ";
		o << "}";
	}
	return o;
}



#endif // _H_CASPER_KERNEL_CONTAINER_SULIST

