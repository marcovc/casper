 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_CONTAINER_INTERVALA
#define _H_CASPER_KERNEL_CONTAINER_INTERVALA

#include <iterator>
#include <sstream>

//#include <list>
//#include <set>
#include <casper/kernel/common.h>
#include <casper/util/container/sciterator.h>
#include <casper/util/container/stdpair.h>
#include <casper/kernel/reversible/reversible.h>
#include <casper/kernel/reversible/sulist.h>

// TODO: Could be improved by storing an explicit end() element

namespace Casper {
namespace Detail {

template<class T>
struct RInterval
{
	typedef Reversible<T>	RT;
	RT	_min;
	RT	_max;
	RInterval(State& state, const T& min, const T& max) :
		_min(state,min),_max(state,max) {}
	RInterval(const RT& min, const RT& max) : _min(min),_max(max) {}
	RInterval(const RInterval<T>& s) : _min(s._min),_max(s._max) {}
	RT&	min()	{	return _min;	}
	RT&	max()	{	return _max;	}
	const RT&	min() const	{	return _min;	}
	const RT&	max() const	{	return _max;	}
	bool operator<(const RInterval<T>& s) const
	{	return _max < s.min();	}
};


};

};

namespace std {
/// @todo This class should go to an "utility.h" file

template<class T>
struct less<Casper::Detail::RInterval<T> > : public binary_function<Casper::Detail::RInterval<T>,Casper::Detail::RInterval<T>,bool>
{
//	less(){}
	bool operator()(const Casper::Detail::RInterval<T>& x, const Casper::Detail::RInterval<T>& y) const
	{ 	return x.max() < y.min(); 	}
	bool operator()(const Casper::Detail::RInterval<T>& x, const T& y) const
	{ 	return x.max() < y; 	}
	bool operator()(const T& x, const Casper::Detail::RInterval<T>& y) const
	{ 	return x < y.min(); 	}
};
};

namespace Casper {

namespace Detail {
template<class T,class Container,class Compare>
class IntervalA;
} // Detail

} // Casper

template<class T,class Container,class Compare>
std::ostream& operator<<(std::ostream& os, const Casper::Detail::IntervalA<T,Container,Compare>& f);

namespace Casper {
namespace Detail {

struct Tree;
struct List;
struct Interval;
struct Element;

// ContainerTraits
template<class ContainerT,class T,class CompareT>
struct SelectContainer{};

template<class T,class CompareT>
struct SelectContainer<Casper::Detail::Tree,T,CompareT>
{ typedef std::set<T,CompareT> Type; };

template<class T,class CompareT>
struct SelectContainer<Casper::Detail::List,T,CompareT>
{ typedef Casper::SUList<T,CompareT> Type; };

template<class ElementT,class T,class ContainerT,class CompareT>
struct SelectElement {};

template<class T,class ContainerT,class CompareT>
struct SelectElement<Casper::Detail::Interval,T,ContainerT,CompareT>
{ typedef Casper::Detail::IntervalA<T,ContainerT,CompareT> Type; };


template<class T,class ContainerT,class CompareT>
struct SelectElement<Casper::Detail::Element,T,ContainerT,CompareT>
{ typedef typename SelectContainer<ContainerT,T,CompareT>::Type	Type; };


/**
This class is an Adaptor, allowing stl containers to handle contiguous
intervals of elements of type T.

- adds: constructor(T min, T max)
- adds: bool insert(T min, T max)
- adds: bool erase(T min, T max)

Additional constraints:
- T must support the operator == (model of EqualityComparable)
- T must support addition and subtraction of integers.

Basically, T should be an integral type, such as char, int, short or long.

@todo The Compare template parameter is currently being ignored (it is always "<").
	  Allowing custom compare operators is possible but will require a careful
	  review of the code.

*/
template<class T,
         class Container = Casper::Detail::List,
		 class Compare = std::less<T> >
class IntervalA
{
	// the allocator that will be used to allocate intervals
	typedef RInterval<T>	Interval;
	typedef	typename SelectContainer<Container,Interval,std::less<Interval> >::Type	DataT;

	public:

	// required from Container model
	typedef T 					Value;
	typedef const Value& 		Reference;
	typedef Reference 			ConstReference;
	typedef const Value* 		Pointer;
	typedef const Pointer 		ConstPointer;
	typedef long 				Difference;
	typedef unsigned long long 	Size;

	// required from Associative Container model
	typedef T key_type; // the key and the value are the same

	typedef	Util::SCIterator<Value,Difference>	scit;

	class Iterator;
	friend class Iterator;
	class Iterator : public std::iterator<std::bidirectional_iterator_tag,
									 	  const Value,
									 	  Difference>
	{
		friend class IntervalA;
		public:
		typedef typename IntervalA::Value Value;
		typedef typename IntervalA::Reference Reference;

		Iterator() {}
		Iterator(typename DataT::Iterator curInt) : curInt(curInt) {}
		Iterator(typename DataT::Iterator curInt,Value curVal) :
					curInt(curInt),curVal(curVal) {}

		bool operator==(const Iterator& s) const;
		bool operator!=(const Iterator& s) const;
		ConstReference operator*() const;
		typename IntervalA::Pointer operator->() const;
		Iterator& operator++(); // pre-increment
		Iterator operator++(int); //post-increment
		Iterator(const Iterator& i);
		const Iterator& operator=(const Iterator& i);
		Iterator& operator--(); // pre-decrement
		Iterator operator--(int); //post-decrement
		private:
		typename DataT::Iterator		curInt;
		Value							curVal;
	};
	typedef Iterator ConstIterator;

	IntervalA(State&);
	template <class InputIterator>
	IntervalA(State&,InputIterator first, InputIterator last);
	IntervalA(State&,scit first, scit last);
	template <class InputIterator>
	IntervalA(State&,InputIterator first, InputIterator last, const Compare& c);
	IntervalA(const IntervalA&);
//	IntervalA(key_compare);
	~IntervalA();

	bool operator==(const IntervalA& s) const;
	bool operator<(const IntervalA& s) const;

	///	Points to the first element of the fdset.
	Iterator begin() const;
	/// Points to the position after the last element of the fdset.
	Iterator end() const;
	Size size() const;
	/// Returns \p true if this container has no elements.
	bool empty() const;
	Iterator find(const key_type& k) const;
	Size count(const key_type& k) const;
	Util::StdPair<Iterator, Iterator> equal_range(const key_type& k) const;
	typedef std::reverse_iterator<Iterator> ReverseIterator;
	typedef ReverseIterator ConstReverseIterator;
	ReverseIterator rbegin() const {return ReverseIterator(end());}
	ReverseIterator rend() const {return ReverseIterator(begin());}
	Iterator lowerBound(const key_type& k) const;
	Iterator upperBound(const key_type& k) const;
	void updateMin(const key_type& k);
	void updateMax(const key_type& k);
	inline void updateMin(Iterator it);
	inline void updateMax(Iterator it);

	void swap(IntervalA&);
	Size erase(const key_type& k);
	void erase(Iterator p);
	void erase(Iterator p, Iterator q);
	void clear();
	IntervalA& operator=(const IntervalA& s);
	Util::StdPair<Iterator,bool> insert(ConstReference);
	Iterator insert(Iterator hint, ConstReference value);
	Iterator insert(Iterator hint, ConstReference f,ConstReference l)
	{	return Iterator(data.insert(hint.curInt,Interval(state,f,l)));	}

	template <class InputIterator>
	void insert(InputIterator first, InputIterator last);
	void insert(scit first, scit last);

	friend std::ostream& ::operator<< <>(std::ostream& os, const IntervalA& f);
  	//friend istream& operator>>(istream& is, IntervalA& ia);
	bool testInternRep() const;

	State&	getState() const	{	return state;	}

	private:
		State&		state;
		DataT		data;
		Iterator	endSentinel;

	void initListAndSentinel();
	void eraseInterval(const typename DataT::Iterator& it);
	void eraseIntervals(const typename DataT::Iterator& b,
						const typename DataT::Iterator& e);
	void insertInterval(const Interval& it);
	void insertInterval(const typename DataT::Iterator& hint,
						const Interval& it);
	bool insertDI(typename DataT::Iterator& it, ConstReference k);
};

// *********** ITERATOR **************

/**
	Copy constructor.
	@param i Iterator to initialize from
*/
template<class T, class Container, class Compare>
inline
IntervalA<T,Container,Compare>::Iterator::
	Iterator(const Iterator& i) :	curInt(i.curInt),
									curVal(i.curVal) { }

template<class T, class Container, class Compare>
inline
const typename IntervalA<T,Container,Compare>::Iterator&
IntervalA<T,Container,Compare>::Iterator::operator=(const Iterator& i)
{
	curInt = i.curInt;
	curVal = i.curVal;
	return *this;
}

template<class T, class Container, class Compare>
inline
bool IntervalA<T,Container,Compare>::Iterator::
		operator==(const Iterator& s) const
{
	return curInt == s.curInt && curVal == s.curVal;
}

template<class T, class Container, class Compare>
inline
bool IntervalA<T,Container,Compare>::Iterator::
		operator!=(const Iterator& s) const
{
	return curInt != s.curInt || curVal != s.curVal;
}

template<class T, class Container, class Compare>
inline
typename IntervalA<T,Container,Compare>::ConstReference
	IntervalA<T,Container,Compare>::Iterator::
		operator*() const
{ return curVal; }

template<class T, class Container, class Compare>
inline
typename IntervalA<T,Container,Compare>::Pointer
		IntervalA<T,Container,Compare>::Iterator::
		operator->() const
{ return &curVal; }

/**
	Pre-increment operator
*/
template<class T, class Container, class Compare>
inline
typename IntervalA<T,Container,Compare>::Iterator&
		IntervalA<T,Container,Compare>::Iterator::
		operator++()
{
	if (curVal == curInt->max())
		curVal = (++curInt)->min();
	else
		++curVal;
	return *this;
}

/**
	Post-increment operator
*/
template<class T, class Container, class Compare>
inline
typename IntervalA<T,Container,Compare>::Iterator
		IntervalA<T,Container,Compare>::Iterator::
		operator++(int)
{ Iterator tmp(*this); ++(*this); return tmp;}

/**
	Pre-decrement operator
*/
template<class T, class Container, class Compare>
inline
typename IntervalA<T,Container,Compare>::Iterator&
		IntervalA<T,Container,Compare>::Iterator::
		operator--()
{
	if (curVal == curInt->min())
		curVal = (--curInt)->max();
	else
		--curVal;
	return *this;
}

/**
	Post-decrement operator
*/
template<class T, class Container, class Compare>
inline
typename IntervalA<T,Container,Compare>::Iterator
	IntervalA<T,Container,Compare>::Iterator::
	operator--(int)
{ Iterator tmp(*this); --(*this); return tmp; }

// ********** USEFUL PRIVATE METHODS ***********

template<class T, class Container, class Compare>
inline
void IntervalA<T,Container,Compare>::
	eraseInterval(const typename DataT::Iterator& it)
{	data.erase(it);	}

template<class T, class Container, class Compare>
inline
void IntervalA<T,Container,Compare>::
	eraseIntervals(const typename DataT::Iterator& b,
				   const typename DataT::Iterator& e)
{	data.erase(b,e);	}

template<class T, class Container, class Compare>
inline
void IntervalA<T,Container,Compare>::
	insertInterval(const Interval& p)
{	data.insert(p);	}

template<class T, class Container, class Compare>
inline
void IntervalA<T,Container,Compare>::
	insertInterval(const typename DataT::Iterator& hint, const Interval& p)
{	data.insert(hint,p); }

/**
	Inserts an element \a k in the interval container,
	at position \a it. After \a it points to the interval
	where \a k was inserted.
	@return \a false if the element already exists, \a true
	otherwise.
	@cost O(1)
*/
template<class T, class Container, class Compare>
bool IntervalA<T,Container,Compare>::
	insertDI(typename DataT::Iterator& it, ConstReference k)
{
	if (it == data.end() || k < it->min() - 1)
	{
		insertInterval(it,Interval(state,k,k));
		--it;
	}
	else
	if (k < it->min())
		it->min() = k;
	else
	if (k > it->max())
	{
		typename DataT::Iterator snd_it = it;
		++snd_it;
		if (snd_it != data.end() && k == snd_it->min() - 1)
		{
			it->max() = snd_it->max();
			eraseInterval(snd_it);
		}
		else
			it->max() = k;
	}
	else
		return false;
	return true;
}

// ********** CONTAINER ***********

template<class T, class Container, class Compare>
inline
typename IntervalA<T,Container,Compare>::Iterator
	IntervalA<T,Container,Compare>::begin() const
{	return Iterator(data.begin(),data.begin()->min());	}

template<class T, class Container, class Compare>
inline
typename IntervalA<T,Container,Compare>::Iterator
	IntervalA<T,Container,Compare>::end() const
{	return endSentinel;	}

/**
	@cost	O(N)
	@return Number of elements in the container.
*/
template<class T, class Container, class Compare>
typename IntervalA<T,Container,Compare>::Size
	IntervalA<T,Container,Compare>::size() const
{
	Size ret = 0;
	for (typename DataT::Iterator it = data.begin();
		 it != data.end(); ++it)
		ret += static_cast<Size>(it->max() - it->min() + 1);

	return ret;
}

/**
	@cost	O(1)
	@return true if the container is empty, false otherwise
*/
template<class T, class Container, class Compare>
inline
bool IntervalA<T,Container,Compare>::empty() const
{ return begin() == end(); }

/**
	Swaps the contents of two IntervalA's.
	@cost	O(Ni)
*/
template<class T, class Container, class Compare>
void IntervalA<T,Container,Compare>::swap(IntervalA& s)
{ data.swap(s.data); }


template<class T, class Container, class Compare>
void IntervalA<T,Container,Compare>::initListAndSentinel()
{
	typedef typename DataT::NodeI 	NodeI;
	typedef typename DataT::Node  	Node;
	typedef typename DataT::RPNodeI	RPNodeI;
	NodeI* cur = new (getState().getHeap())
		Node(Interval(state,0,0),RPNodeI(state),RPNodeI(state));
	data.head = data.tail = cur;
	endSentinel = Iterator(data.end(),0);
}

/**
	Copy constructor
	@cost	O(Ni)
*/
template<class T, class Container, class Compare>
IntervalA<T,Container,Compare>::IntervalA(const IntervalA& s) :
	state(s.state),
	data(state)
{
	initListAndSentinel();
	for (typename DataT::ConstIterator it = s.data.begin();
		 it != s.data.end(); ++it)
		 insertInterval(*it);
}

/**
	Copy operator.
	@cost	O(Ni)
*/
template<class T, class Container, class Compare>
IntervalA<T,Container,Compare>&
		IntervalA<T,Container,Compare>::operator=(const IntervalA& s)
{
	clear();
	for (typename DataT::ConstIterator it = s.data.begin();
		 it != s.data.end(); ++it)
		 insertInterval(*(*it));
	return *this;
}

/**
	@cost	O(Ni)
*/
template<class T, class Container, class Compare>
IntervalA<T,Container,Compare>::~IntervalA()
{
	for (typename DataT::Iterator it = data.begin();
		 it != data.end();)
		 eraseInterval(it++);
}

/**
	@cost	O(Ni)
*/
template<class T, class Container, class Compare>
bool IntervalA<T,Container,Compare>::operator==(const IntervalA& s) const
{
	typename DataT::Iterator it = data.begin(), sit = s.data.begin();

	for (; it != data.end() && sit != s.data.end();
		   ++it,++sit)
		if (it->min() != sit->min() ||
			it->max() != sit->max())
			return false;
	return it == data.end() && sit == s.data.end();
}

/**
	@cost	O(Ni)
	@todo Implementation is completely wrong
*/
template<class T, class Container, class Compare>
 bool IntervalA<T,Container,Compare>::operator<(const IntervalA& s) const
{ return lexicographical_compare(begin(),end(),
                             s.begin(), s.end());}

/**
	Default constructor. Creates a new IntervalA with no elements.
	@cost	O(1)
*/
template<class T, class Container, class Compare>
IntervalA<T,Container,Compare>::IntervalA(State& state) :
	state(state),data(state)
{ 	initListAndSentinel();	}

/**
	Erases an element from the container
	@param k The element to erase
	@cost O(s(Ni))
*/
template<class T, class Container, class Compare>
typename IntervalA<T,Container,Compare>::Size
	IntervalA<T,Container,Compare>::
	erase(const key_type& k)
{
	Iterator it = find(k);
	if (it == end())
		return 0;
	erase(it);
	return 1;
}

template<class T, class Container, class Compare>
void IntervalA<T,Container,Compare>::
	updateMin(const key_type& k)
{
	Iterator it = lowerBound(k);
	if (it == end())
		clear();
	else
		updateMin(it);
}

template<class T, class Container, class Compare>
void IntervalA<T,Container,Compare>::
	updateMax(const key_type& k)
{
	Iterator it = upperBound(k);
	if (it == end())
		return;
	updateMax(it);
}

/// \pre q != end()
template<class T, class Container, class Compare>
inline void IntervalA<T,Container,Compare>::updateMin(Iterator q)
{
	assert(q != end());
	q.curInt->min() = q.curVal;
	if (q != begin())
		data.updateMin(q.curInt);
}

/// \pre p != end()
template<class T, class Container, class Compare>
inline void IntervalA<T,Container,Compare>::updateMax(Iterator p)
{
	assert(p != end());
	p.curInt->max() = p.curVal;
	if (p != --end())
		data.updateMax(p.curInt);
}

/**
	Erases an element from the container
	@param p Points to the element to erase
	@cost O(1)
*/
template<class T, class Container, class Compare>
void IntervalA<T,Container,Compare>::
	erase(typename IntervalA<T,Container,Compare>::Iterator p)
{
	// if erasing the first element of an interval
	if (p.curVal == p.curInt->min())
	{
		if (p.curInt->min() == p.curInt->max())
			eraseInterval(p.curInt);
		else
			p.curInt->min() = p.curInt->min() + 1;
	}
	else
	// if erasing the last element of an interval
	if (p.curVal == p.curInt->max())
	{
		if (p.curInt->min() == p.curInt->max())
			eraseInterval(p.curInt);
		else
			p.curInt->max() = p.curInt->max() - 1;
	}
	else
	// if erasing interior element
	{
		T tmp = p.curInt->min();
		p.curInt->min() = p.curVal + 1; // must be this way
		insertInterval(p.curInt,Interval(state,tmp,p.curVal - 1));
	}
}

/**
	Removes the elements on the range [f,l) from the container
	@cost O(l-f)
*/
template<class T, class Container, class Compare>
void IntervalA<T,Container,Compare>::
	erase(typename IntervalA<T,Container,Compare>::Iterator p,
		  typename IntervalA<T,Container,Compare>::Iterator q)
{
	--q;
	// change/keep first interval if necessary
	if (p.curVal > p.curInt->min())
	{
		// if the range is strictly inside one interval
		if (p.curInt == q.curInt && q.curVal < q.curInt->max())
		{
			T tmp = p.curInt->min();
			p.curInt->min() = q.curVal+1;	// must be this way
			insertInterval(p.curInt,Interval(state,tmp,p.curVal-1));
			return;
		}
		p.curInt->max() = p.curVal - 1;
		++p.curInt;
	}

	// change/keep last interval if necessary
	if (q.curVal < q.curInt->max())
		q.curInt->min() = q.curVal + 1;
	else
		++q.curInt;

	// erase interior intervals
	while (p.curInt != q.curInt)
		eraseInterval(p.curInt++);
}

/**
	Removes all elements from the container.
	@cost O(N)
*/
template<class T, class Container, class Compare>
void IntervalA<T,Container,Compare>::clear()
{
 	for (typename DataT::Iterator it = data.begin();
		 it != data.end(); /* nothing here */)
		 eraseInterval(it++);
}

/**
	\brief Finds the element \a k.
	\return An Iterator pointing to \a k if \a k exists in
	the container or pointing to \a end() otherwise.
	O(s(Ni))
*/
template<class T, class Container, class Compare>
typename IntervalA<T,Container,Compare>::Iterator
	IntervalA<T,Container,Compare>::
	find(const key_type& k) const
{
	Iterator ret;
	ret.curInt = data.find(k);
	if (ret.curInt == data.end())
		return end();
	ret.curVal = k;
	return ret;
}

/**
	Returns 1 if the element \a k exists or 0 otherwise.
	@cost O(N)
*/
template<class T, class Container, class Compare>
typename IntervalA<T,Container,Compare>::Size
	IntervalA<T,Container,Compare>::
	count(const key_type& k) const
{ return find(k) != end(); }

/**
	@return A pair of iterators defining a range of values
	[i,j] where i>=k, j>k and i,j belong to the container.
	@cost O(N)
*/
template<class T, class Container, class Compare>
Util::StdPair<typename IntervalA<T,Container,Compare>::Iterator,
	 typename IntervalA<T,Container,Compare>::Iterator>
	 IntervalA<T,Container,Compare>::
	equal_range(const key_type& k) const
{
	Util::StdPair<Iterator,Iterator> ret(Iterator(*this),Iterator(*this));
	ret.first.curInt = data.lowerBound(k);
	ret.second.curInt = ret.first.curInt;
	if (ret.first.curInt == data.end())
		return end();
	else
	if (k < ret.first.curInt->min())
	{
		ret.first.curVal = ret.first.curInt->min();
		ret.second.curVal = ret.first.curInt->min();
	}
	else
	{
		ret.first.curVal = k;
		if (k < ret.second.curInt->max())
			ret.second.curVal = k+1;
		else
			ret.second.curVal = ret.second.curInt->min();
	}
	return ret;
}

/**
	@return An Iterator pointing to j where j>=k and
	j belong to the container.
	@cost O(N)
*/
template<class T, class Container, class Compare>
typename IntervalA<T,Container,Compare>::Iterator
	IntervalA<T,Container,Compare>::
	lowerBound(const key_type& k) const
{
	Iterator ret;
	ret.curInt = data.lowerBound(k);
	if (ret.curInt == data.end())
		return end();
	else
	if (k < ret.curInt->min())
		ret.curVal = ret.curInt->min();
	else
		ret.curVal = k;
	return ret;
}

/**
	@return An Iterator pointing to j where j>k and
	j belong to the container.
	@cost O(N)
*/
template<class T, class Container, class Compare>
typename IntervalA<T,Container,Compare>::Iterator
	IntervalA<T,Container,Compare>::
	upperBound(const key_type& k) const
{
	Iterator ret;
	ret.curInt = data.lowerBound(k);
	if (ret.curInt == data.end())
		return end();
	else
	if (k < ret.curInt->min())
		ret.curVal = ret.curInt->min();
	else
	if (k < ret.curInt->max())
		ret.curVal = k+1;
	else
	{
		++ret.curInt;
		if (ret.curInt == data.end())
			return end();
		else
			ret.curVal = ret.curInt->min();
	}
	return ret;
}

/**
	Inserts the element \a k in the container.
	@return An Iterator pointing to k and a boolean
	indicating if the element was actually inserted.
	@cost O(s(Ni))
*/
template<class T, class Container, class Compare>
Util::StdPair<typename IntervalA<T,Container,Compare>::Iterator,bool>
	IntervalA<T,Container,Compare>::
	insert(ConstReference k)
{
	Util::StdPair<Iterator,bool> ret(Iterator(),true);
	ret.first.curVal = k;
	ret.first.curInt = data.lowerBound(Interval(state,(k==limits<T>::min())?k:k-1,k));
	ret.second = insertDI(ret.first.curInt,k);
	return ret;
}

/**
	Creates a new IntervalA with elements [amin,amax)
	a range of values defined by the range [first,last).
	@cost O(N)
*/
template<class T, class Container, class Compare>
template <class InputIterator>
IntervalA<T,Container,Compare>::
	IntervalA(State& state,InputIterator first, InputIterator last) :
		state(state),
		data(state)
{
	initListAndSentinel();
	insert(first,last);
}

/**
	Creates a new IntervalA with elements [amin,amax).
	@cost O(1)
*/
template<class T, class Container, class Compare>
IntervalA<T,Container,Compare>::IntervalA(State& state,scit amin, scit amax) :
		state(state),data(state)
{
	initListAndSentinel();
	insertInterval(Interval(state,*amin,*--amax));
}

/**
	Creates a new IntervalA with elements [amin,amax] and using the
	operator \a c for ordering the elements.
	@cost O(N)
	@todo Implement correctly
*/
template<class T, class Container, class Compare>
template <class InputIterator>
IntervalA<T,Container,Compare>::
	IntervalA(State& state,InputIterator first, InputIterator last,
			  const Compare& c) : state(state),data(state)
{
	initListAndSentinel();
	insert(first,last);
}

/**
	Inserts an element in the container, following a hint where the element
	will be inserted.
	@param hint Iterator pointing to the position where k will be inserted
	@param k Element to insert.
	@cost O(1) if the hint is correct or O(s(Ni)) otherwise.
*/
template<class T, class Container, class Compare>
typename IntervalA<T,Container,Compare>::Iterator
	IntervalA<T,Container,Compare>::
	insert(Iterator hint, ConstReference k)
{
	hint.curVal = k;
	if (hint.curInt != data.begin() && k <= (--hint.curInt)->max() + 1)
		hint.curInt = insert(k).first.curInt;
	else	// hint is wrong, insert with no hint
	{
		if (hint.curInt != data.begin())
			++hint.curInt;
		Interval tmp(state,(k==limits<T>::min())?k:k-1,k);
		while (hint.curInt != data.end() &&
				std::less<Interval>()(*hint.curInt,tmp))
			++hint.curInt;
		insertDI(hint.curInt,k);
	}
	return hint;
}

/**
	Inserts a range of values [first,last).
	@cost O(N)
*/
template<class T, class Container, class Compare>
template <class InputIterator>
void IntervalA<T,Container,Compare>::
	insert(InputIterator first, InputIterator last)
{
	for (InputIterator it = first; it != last; ++it)
		insert(*it);
}

/**
	Inserts a contiguous range of values defined by
	[amin,amax).
	@cost O(Ni)
*/
template<class T, class Container, class Compare>
void IntervalA<T,Container,Compare>::
	insert(scit amin, scit amax)
{
	--amax;
	Interval tmp(Reversible<T>(state,(*amin==limits<T>::min())?(*amin):(*amin-1)),
			 Reversible<T>(state,*amin));
	typename DataT::Iterator it = data.lowerBound(tmp);
	if (it == data.end() || (it->min() > limits<T>::min() && *amax < it->min() - 1))
		insertInterval(it,Interval(Reversible<T>(state,*amin),
									Reversible<T>(state,*amax)));
	else
	{
		if (*amin < it->min())
			it->min() = *amin;

		if (*amax <= it->max())
			return;

		typename DataT::Iterator aux = it;
		++aux;
		it->max() = *amax;
		while (aux != data.end())
		{
			if (*amax < aux->min() - 1)
				return;
			if (*amax <= aux->max())
			{
				it->max() = aux->max();
				data.erase(aux);
				return;
			}
			data.erase(aux++);
		}
	}
}

/**
	Writes the contents of the IntervalA to an ostream.
*/
template<class T, class Container, class Compare>
std::ostream& operator<<(std::ostream& o,
					const IntervalA<T,Container,Compare>& a)
{
	o << "{";
	if (a.size()==1)
		o << *a.begin();
	else
	for (typename IntervalA<T,Container,Compare>::
		 DataT::Iterator it = a.data.begin();
		 it != a.data.end();
		 ++it)
	{
		o << "[";
		if (it->min() == limits<T>::min())
			o << "inf";
		else
		if (it->min() == limits<T>::max())
			o << "sup";
		else
			o << it->min();
		o << "..";
		if (it->max() == limits<T>::min())
			o << "inf";
		else
		if (it->max() == limits<T>::max())
			o << "sup";
		else
			o << it->max();
		o << "]";
	}
	//o << "},#" << (long long)a.size();
	o << "}";
	return o;
}


/**
	Tests if the IntervalA internal representation is correct.
	Used for debugging.
*/
template<class T, class Container, class Compare>
bool IntervalA<T,Container,Compare>::
	testInternRep() const
{
	for (typename DataT::Iterator it1 = data.begin();
		 it1 != data.end();
		 ++it1)
	{
		if (it1->min() > it1->max())
			return false;
		typename DataT::Iterator it2 = it1;
		for (++it2;
			 it2 != data.end();
			 ++it2)
			if ((it1->min() >= it2->min() &&
				 it1->min() <= it2->max()) ||
			    (it1->max() >= it2->min() &&
				 it1->max() <= it2->max()))
				 return false;
	}
	return true;
}

} // Detail
} // Casper

#endif // _H_CASPER_KERNEL_CONTAINER_INTERVALA

