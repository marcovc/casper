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

#ifndef _H_CASPER_KERNEL_CONTAINER_RSULIST
#define _H_CASPER_KERNEL_CONTAINER_RSULIST

#include <iterator>

#include <functional>

#include <casper/kernel/reversible.h>
#include <casper/kernel/container/common.h>
#include <casper/kernel/container/pair.h>
#include <casper/kernel/container/sciterator.h>

namespace casper {
namespace detail {

using detail::SCIterator;

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
	RSUListNode(ICPSolver& solver, const T& elem, const PNodeI& prev, const PNodeI& next):
						Super(RPNodeI(solver,prev),RPNodeI(solver,next)),elem(elem) {}
};

/**
 *  Reversible sorted unique list (each element is unique and the
 * 	list is always sorted).
 * 	\ingroup Containers
 **/
template<class T,class Compare = std::less<T> >
struct RSUList
{
	typedef RSUList<T,Compare>	Self;
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

	ICPSolver&						mSolver;
	RPNodeI						head;
	RPNodeI						tail;

	class Iterator;
	friend class Iterator;
	class Iterator : public std::iterator<bidirectional_iterator_tag,
									 	  Value,
									 	  Difference>
	{
		friend struct RSUList;
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

	typedef	SCIterator<Value,Difference>	scit;

	RSUList(ICPSolver& mSolver);
	template <class InputIterator>
	RSUList(ICPSolver& mSolver,InputIterator first, InputIterator last);
	RSUList(const RSUList&);
	virtual ~RSUList();

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
	Pair<Iterator, Iterator> equalRange(const Key& k) const;
	template<class K>
	Iterator lowerBound(const K& k) const;
	Iterator lowerBound(const Key& k) const;
	template<class K>
	Iterator upperBound(const K& k) const;
	Iterator upperBound(const Key& k) const;
	Pair<Iterator,bool> insert(ConstReference);
	Iterator insert(Iterator hint, ConstReference value);
	Size erase(const Key& k);
	virtual bool erase(Iterator p, Iterator q);
	virtual bool erase(Iterator p);
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last);
	virtual void clear();
	Bool operator==(const RSUList&) const;
	ICPSolver&	solver() const	{	return mSolver;	}
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
typename RSUList<T,Compare>::Iterator RSUList<T,Compare>::begin() const
{
	return typename RSUList<T,Compare>::Iterator(head);
}

template<class T,class Compare>
typename RSUList<T,Compare>::Iterator RSUList<T,Compare>::end() const
{
	return typename RSUList<T,Compare>::Iterator(tail);
}

template<class T,class Compare>
RSUList<T,Compare>::RSUList(ICPSolver& mSolver) :
							   mSolver(mSolver),
							   head(typename Self::RPNodeI(mSolver)),
							   tail(typename Self::RPNodeI(mSolver))
{
	PNodeI cur = new (mSolver.heap()) NodeI(typename Self::RPNodeI(mSolver),
									    typename Self::RPNodeI(mSolver));
	head = tail = cur;
}

template<class T,class Compare>
template <class InputIterator>
void RSUList<T,Compare>::initRange(InputIterator first, InputIterator last)
{
	if (first != last)
	{
		PNodeI cur = new (mSolver.heap()) Node(*first,head,tail);
		head = cur;
		while (++first!=last)
		{
			cur = new (mSolver.heap()) Node(*first,RPNodeI(mSolver,cur),tail);
			cur->prev.get()->next = cur;
		}
		tail = new (mSolver.heap()) NodeI(RPNodeI(mSolver,cur),tail);
		tail.get()->prev.get()->next = tail;
	}
	else
	{
		PNodeI cur = new (mSolver.heap()) NodeI(head,tail);
		head = tail = cur;
	}
}

/// \note Requires range [first,last) to be sorted.
template<class T,class Compare>
template <class InputIterator>
RSUList<T,Compare>::RSUList(ICPSolver& mSolver, InputIterator first, InputIterator last) :
								mSolver(mSolver),
								head(typename Self::RPNodeI(mSolver)),
							    tail(typename Self::RPNodeI(mSolver))
{
	assert(isStrictOrdered(first,last));
	initRange(first,last);
}

template<class T,class Compare>
RSUList<T,Compare>::RSUList(const RSUList<T,Compare>& s) :
								mSolver(s.mSolver),
								head(typename Self::RPNodeI(mSolver)),
							   	tail(typename Self::RPNodeI(mSolver))
{	initRange(s.begin(),s.end());	}

template<class T,class Compare>
RSUList<T,Compare>::~RSUList()
{ /* garbage collection frees everything */ }

template<class T,class Compare>
bool RSUList<T,Compare>::empty() const
{	return head == tail;	}

template<class T,class Compare>
typename RSUList<T,Compare>::Size RSUList<T,Compare>::size() const
{
	typename RSUList::Size ret(0);
	for (typename RSUList<T,Compare>::ConstIterator it = begin();
						it != end(); ++it)
		ret++;
	return ret;
}

/// linear search
template<class T,class Compare>
typename RSUList<T,Compare>::Iterator
RSUList<T,Compare>::find(const Key& k) const
{
	if (empty() or TestRel<T,Compare>::greater(k,*--end()))
		return end();

	for (typename RSUList<T,Compare>::Iterator it = begin(); it != end(); ++it)
		if (TestRel<T,Compare>::equal(k,*it))
			return it;
		else
		if (TestRel<T,Compare>::less(k,*it))
			break;
	return end();
}

/// linear search
template<class T,class Compare>
template<class K>
typename RSUList<T,Compare>::Iterator
RSUList<T,Compare>::find(const K& k) const
{
	if (empty() or TestRel<T,Compare>::greater(k,*--end()))
		return end();

	for (typename RSUList<T,Compare>::Iterator it = begin(); it != end(); ++it)
		if (TestRel<T,Compare>::equal(k,*it))
			return it;
		else
		if (TestRel<T,Compare>::less(k,*it))
			break;
	return end();
}

template<class T,class Compare>
typename RSUList<T,Compare>::Size
RSUList<T,Compare>::count(const Key& k) const
{
	return find(k) != end();
}

template<class T,class Compare>
Pair<typename RSUList<T,Compare>::Iterator,
	 typename RSUList<T,Compare>::Iterator>
RSUList<T,Compare>::equalRange(const Key& k) const
{
	typedef Pair<typename Self::Iterator,
				 typename Self::Iterator> p_t;
	typename RSUList<T,Compare>::Iterator it = find(k);
	return p_t(it,it);
}

template<class T,class Compare>
typename RSUList<T,Compare>::Iterator
RSUList<T,Compare>::lowerBound(const Key& k) const
{
	PNodeI aux(head);
	while (aux != tail and TestRel<T,Compare>::less(((PNode)aux)->elem,k))
		aux = aux->next;
	return Iterator(aux);
}

// works with abstract type K, which must implement std::less
template<class T,class Compare>
template<class K>
typename RSUList<T,Compare>::Iterator
RSUList<T,Compare>::lowerBound(const K& k) const
{
	PNodeI aux(head);
	while (aux != tail and TestRel<T,Compare>::less(((PNode)aux)->elem,k))
		aux = aux->next;
	return Iterator(aux);
}

template<class T,class Compare>
typename RSUList<T,Compare>::Iterator
RSUList<T,Compare>::upperBound(const Key& k) const
{
	PNodeI aux(tail);
	while (aux != head and TestRel<T,Compare>::less(k,((PNode)aux->prev.get())->elem))
		aux = aux->prev;
	return Iterator(aux);
}

template<class T,class Compare>
template<class K>
typename RSUList<T,Compare>::Iterator
RSUList<T,Compare>::upperBound(const K& k) const
{
	PNodeI aux(tail);
	while (aux != head and TestRel<T,Compare>::less(k,((PNode)aux->prev.get())->elem))
		aux = aux->prev;
	return Iterator(aux);
}

template<class T,class Compare>
Pair<typename RSUList<T,Compare>::Iterator,bool>
RSUList<T,Compare>::insert(ConstReference r)
{
	Pair<typename RSUList<T,Compare>::Iterator,bool> ret(begin(),false);
	PNodeI cur;
	for ( ; ret.first != end(); ++ret.first)
	{
		if (TestRel<T,Compare>::greater(*ret.first,r))
		{
			cur = new (mSolver.heap()) Node(r,ret.first.curNode->prev,
										  RPNodeI(mSolver,ret.first.curNode));
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
		if (TestRel<T,Compare>::equal(*ret.first,r))
			return ret;
	}
	cur = new (mSolver.heap()) Node(r,tail.get()->prev,tail);
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
typename RSUList<T,Compare>::Iterator
RSUList<T,Compare>::insert(typename RSUList<T,Compare>::Iterator hint,
						   ConstReference r)
{
	PNodeI cur;
	for ( ; hint != end(); ++hint)
	{
		if (TestRel<T,Compare>::greater(*hint,r))
		{
			cur = new (mSolver.heap()) Node(r,hint.curNode->prev,
										  RPNodeI(mSolver,hint.curNode));
			cur->next.get()->prev = cur;
			if (head == hint.curNode)
				head = cur;
			else
				cur->prev.get()->next = cur;
			hint = cur;
			return hint;
		}
		else
		if (TestRel<T,Compare>::equal(*hint,r))
			return hint;
	}
	cur = new (mSolver.heap()) Node(r,tail.get()->prev,tail);
	cur->next.get()->prev = cur;
	if (head == tail)
		head = cur;
	else
		cur->prev.get()->next = cur;
	hint = cur;
	return hint;
}

template<class T,class Compare>
typename RSUList<T,Compare>::Size
RSUList<T,Compare>::erase(const Key& k)
{
	typename RSUList<T,Compare>::Iterator it = find(k);
	if (it != end())
	{
		erase(it);
		return 1;
	}
	return 0;
}

template<class T,class Compare>
bool RSUList<T,Compare>::erase(Iterator p, Iterator q)
{
	if (((PNodeI)head) != p.curNode)
		p.curNode->prev.get()->next = q.curNode;
	else
		head = q.curNode;
	q.curNode->prev = p.curNode->prev;
	return true;
}

template<class T,class Compare>
void RSUList<T,Compare>::updateMin(const Key& k)
{
	PNodeI aux(head);
	while (aux != tail && ((PNode)aux)->elem < k)
		aux = aux->next;
	head = aux;
}

template<class T,class Compare>
void RSUList<T,Compare>::updateMax(const Key& k)
{
	PNodeI aux(tail);
	while (aux != head && ((PNode)aux->prev.get())->elem > k)
		aux = aux->prev;
	tail = aux;
}

template<class T,class Compare>
inline void RSUList<T,Compare>::updateMin(Iterator it)
{
	head = it.curNode;
	it.curNode->prev = NULL;
}

template<class T,class Compare>
inline void RSUList<T,Compare>::updateMax(Iterator it)
{
	assert(it != end());
	it.curNode->next = tail;
	tail->prev = it.curNode;
}

template<class T,class Compare>
void RSUList<T,Compare>::clear()
{
	head = tail;
}

template<class T,class Compare>
bool RSUList<T,Compare>::erase(Iterator it)
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
void RSUList<T,Compare>::insert(InputIterator first, InputIterator last)
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
Bool RSUList<T,Compare>::operator==(const RSUList& s) const
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


/**
	Writes the contents of the RSUList to an ostream.
*/
template<typename T,class Compare>
std::ostream& operator<<(std::ostream& o, const casper::detail::RSUList<T,Compare>& a)
{
/*	if (a.size()==1)
		o << *a.begin();
	else
*/	{
		o << "{ ";
		for (typename casper::detail::RSUList<T,Compare>::ConstIterator it = a.begin();
			it != a.end(); ++it)
			o << *it << " ";
		o << "}";
	}
	return o;
}

};

template<class> struct EvalTraits;

template<class T,class Comp>
struct EvalTraits<detail::RSUList<T,Comp> >
{	typedef Seq<T>	Eval;	};

template<class> struct ElemTraits;

template<class T,class Comp>
struct ElemTraits<detail::RSUList<T,Comp> >
{	typedef T	Elem;	};

};



#endif // _H_CASPER_KERNEL_CONTAINER_RSULIST

