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
 * double linked list.
 */

#ifndef _H_CASPER_UTIL_STDLIST
#define _H_CASPER_UTIL_STDLIST

#include <casper/util/memory.h>
#include <casper/util/container/common.h>

#include <iterator>

namespace Casper {
namespace Util {


struct ListNodeI
{
	typedef ListNodeI			Node;
	typedef Node*				PNode;

	PNode		prev;
	PNode 		next;

	ListNodeI() : prev(NULL),next(NULL) {}
	ListNodeI(const PNode& prev, const PNode& next) :
						prev(prev),next(next) {}
	virtual ~ListNodeI() {}
};

template<class T>
struct ListNode : public ListNodeI
{
	typedef ListNodeI			Super;
	typedef Super*				PNodeI;

	T			elem;
	ListNode(const T& elem, const PNodeI& prev, const PNodeI& next) :
						Super(prev,next),elem(elem) {}
};

/**
 * double linked list.
 * \ingroup Containers
 */
template<class T>
struct StdList
{
	typedef ListNode<T>			Node;
	typedef ListNodeI			NodeI;
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
	PNodeI						tail;

	class Iterator;
	friend class Iterator;
	class Iterator : public std::iterator<std::bidirectional_iterator_tag,
									 	  Value,
									 	  Difference>
	{
		friend struct StdList;
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

	StdList(IHeap& heap = stdHeap);
	template <class InputIterator>
	StdList(IHeap& heap,InputIterator first, InputIterator last);
	template <class InputIterator>
	StdList(InputIterator first, InputIterator last);

	StdList(const StdList&);
	virtual ~StdList();
	const StdList& operator=(const StdList& s);

	Iterator begin() const;
	Iterator end() const;
	ReverseIterator rbegin() const {return ReverseIterator(end());}
	ReverseIterator rend() const {return ReverseIterator(begin());}
	bool empty() const;
	Size size() const;
	Iterator insert(Iterator before, ConstReference value);
	Iterator pushFront(ConstReference value);
	Iterator pushBack(ConstReference value);
	virtual bool erase(Iterator p, Iterator q);
	virtual bool erase(Iterator p);

	template <class InputIterator>
	void insert(Iterator before, InputIterator first, InputIterator last);
	virtual void clear();
	IHeap& getHeap() const {	return mHeap;	}

	protected:
	template <class InputIterator>
	void initRange(InputIterator first, InputIterator last);
	void freeForward(Iterator p,Iterator q);
};

template<class T>
typename StdList<T>::Iterator StdList<T>::begin() const
{
	return typename StdList<T>::Iterator(head);
}

template<class T>
typename StdList<T>::Iterator StdList<T>::end() const
{
	return typename StdList<T>::Iterator(tail);
}

template<class T>
StdList<T>::StdList(IHeap& mHeap) :	mHeap(mHeap)
{
	PNodeI cur = new (getHeap()) NodeI(NULL,NULL);
	head = tail = cur;
}

template<class T>
template <class InputIterator>
void StdList<T>::initRange(InputIterator first, InputIterator last)
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

template<class T>
template <class InputIterator>
StdList<T>::StdList(IHeap& mHeap, InputIterator first, InputIterator last) :
								mHeap(mHeap)
{	initRange(first,last);	}

template<class T>
template <class InputIterator>
StdList<T>::StdList(InputIterator first, InputIterator last) :
								mHeap(stdHeap)
{	initRange(first,last);	}

template<class T>
StdList<T>::StdList(const StdList<T>& s) : mHeap(s.getHeap())
{	initRange(s.begin(),s.end());	}

template<class T>
const StdList<T>& StdList<T>::operator=(const StdList<T>& s)
{
	clear();
	initRange(s.begin(),s.end());
	return *this;
}

template<class T>
StdList<T>::~StdList()
{
	clear();
	tail->~NodeI();
	getHeap().deallocate(tail);
}

template<class T>
bool StdList<T>::empty() const
{	return head == tail;	}

template<class T>
typename StdList<T>::Size StdList<T>::size() const
{
	typename StdList::Size ret(0);
	for (typename StdList<T>::ConstIterator it = begin();
						it != end(); ++it)
		ret++;
	return ret;
}

template<class T>
typename StdList<T>::Iterator
StdList<T>::insert(Iterator befPos, ConstReference r)
{
	PNodeI cur = new (getHeap()) Node(r,befPos.curNode->prev,
										befPos.curNode);
	cur->next->prev = cur;
	if (head == befPos.curNode)
		head = cur;
	else
		cur->prev->next = cur;
	return Iterator(cur);
}

template<class T>
typename StdList<T>::Iterator
StdList<T>::pushFront(ConstReference value)
{
	return insert(Iterator(head),value);
}

template<class T>
typename StdList<T>::Iterator
StdList<T>::pushBack(ConstReference value)
{
	return insert(Iterator(tail),value);
}

template<class T>
bool StdList<T>::erase(Iterator p, Iterator q)
{
	if (head != p.curNode)
		p.curNode->prev->next = q.curNode;
	else
		head = q.curNode;
	q.curNode->prev = p.curNode->prev;
	freeForward(p,q);
	return true;
}

template<class T>
bool StdList<T>::erase(Iterator it)
{
	assert(it != end());
	if (head != it.curNode)
		it.curNode->prev->next = it.curNode->next;
	else
		head = it.curNode->next;
	it.curNode->next->prev = it.curNode->prev;
	it.curNode->~NodeI();
	if (!getHeap().doesGC())
		getHeap().deallocate(it.curNode);
	return true;
}

template<class T>
void StdList<T>::clear()
{
	freeForward(head,tail);
	head = tail;
}

template<class T>
template <class InputIterator>
void StdList<T>::insert(Iterator befPos, InputIterator first, InputIterator last)
{
	while (first != last)
	{
		insert(befPos,*first);
		++first;
	}
}

// free memory of elements in [p,q[
template<class T>
void StdList<T>::freeForward(Iterator p, Iterator q)
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

/**
 * 	StdList which allocates all memory at creation and frees at destruction.
 */
template<class T>
struct QuickList : Detail::StaticMemHolder,StdList<T>
{
	typedef StdList<T> Super;

	/**
	 * Create a new StdList allowing \a maxInserts insertions.
	 * \note maxInserts is the maximum number of insertions. This means that no
	 * more insertions are possible when this limit is reached, even if some
	 * elements are removed.
	 **/
	QuickList(uint maxInserts) :
		Detail::StaticMemHolder(maxInserts*
						(sizeof(void*)*2+sizeof(typename Super::Node))+
						3*sizeof(void*)),
		Super(this->getHeap())
	{}

	IHeap& getHeap() const {	return StaticMemHolder::getHeap();	}
};
} // Util


template<class> struct GetPState;
struct State;

template<class T>
struct GetPState<Util::StdList<T> >
{
	State* operator()(const Util::StdList<T>& l)
	{
		for (auto it = l.begin();	it != l.end(); ++it)
		{
			State* p = getPState(*it);
			if (p != NULL)
				return p;
		}
		return NULL;
	}
};

} // Casper


/**
	Writes the contents of the StdList to an ostream.
*/
template<typename T>
std::ostream& operator<<(std::ostream& o, const Casper::Util::StdList<T>& a)
{
	o << "{ ";
	for (typename Casper::Util::StdList<T>::ConstIterator it = a.begin();
		it != a.end(); ++it)
		o << *it << " ";
	o << "}";
	return o;
}

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/cat.hpp>

namespace Casper {
#define CASPER_LIST_FUNC_PB(z,n,data)	\
	r.pushBack(BOOST_PP_CAT(p,n));

#define CASPER_LIST_FUNC_1(z,n,data)							\
	template<class T>											\
	Util::StdList<T> list(BOOST_PP_ENUM_PARAMS(n, const T& p))	\
	{															\
		Util::StdList<T> r;										\
		BOOST_PP_REPEAT(n,CASPER_LIST_FUNC_PB,_)				\
		return r;												\
	}

#define CASPER_LIST_FUNC(n)										\
	BOOST_PP_REPEAT(n,CASPER_LIST_FUNC_1,_)

CASPER_LIST_FUNC(20)
} // Casper

#endif // _H_CASPER_UTIL_STDLIST

