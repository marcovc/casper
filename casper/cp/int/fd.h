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

#ifndef _H_CASPER_CP_INT_FD
#define _H_CASPER_CP_INT_FD

#ifdef BOOST_MSVC
// disables warning about empty function macro parameters
#pragma warning( disable: 4003 )
#endif

#include <iostream>
#include <set>	// temporary

#include <casper/kernel/common.h>
#include <casper/kernel/notify/notifiable.h>
#include <casper/kernel/reversible/intervala.h>
#include <casper/kernel/reversible/bitseta.h>
#include <casper/util/container/sciterator.h>
#include <casper/kernel/reversible/sulist.h>
#include <casper/util/container/stdrange.h>
#include <casper/kernel/notify/susplist.h>

#include <casper/cp/int/common.h>

// tmp
#include <casper/cp/scheduler.h>

// required forward declarations
namespace Casper {
namespace CP {
template<FD_TP_SPEC(1)> class FD;
}
}

template<FD_TP_SPEC(1)> std::ostream& operator<<(std::ostream& os, const Casper::CP::FD<FD_TP_LIST(1)>& f);

namespace Casper {
namespace CP {

template<class Container1,class Element,class T1, class Container2,class T2>
bool isEqual(const FD<Container1,Element,T1>&,const FD<Container2,Element,T2>&);

template<class> struct EraseInterval {};

namespace Detail {

template<class T,class CompareT>
SUList<T,CompareT> rangeStruct(SUList<T,CompareT>*,
										Store& store,
										const Util::StdList<Util::StdRange<T> >& l)
{
	SUList<T,CompareT> r(store);
	for (typename Util::StdList<Util::StdRange<T> >::Iterator it1 = l.begin();
				it1 != l.end(); ++it1)
		for (typename Util::StdRange<T>::Iterator it2 = it1->begin();
			it2 != it1->end(); ++it2)
			r.insert(r.end(),*it2);
	return r;
}


template<class T,class ContainerT,class CompareT>
Casper::Detail::IntervalA<T,ContainerT,CompareT>
	rangeStruct(Casper::Detail::IntervalA<T,ContainerT,CompareT>*,
				Store& store,const Util::StdList<Util::StdRange<T> >& l)
{
	Casper::Detail::IntervalA<T,ContainerT,CompareT> r(store);
	for (typename Util::StdList<Util::StdRange<T> >::Iterator it1 = l.begin();
				it1 != l.end(); ++it1)
			r.insert(r.end(),it1->min(),it1->max());
	return r;
}

} // Detail

/**
	\brief Data structure for representing finite domains

	A finite domain is a set of integers sorted using a strict ordering. Once
	created, only element removals are allowed until only one element remains
	(groundness) or it becomes empty (fails). All operations are reversible,
	which means that every element removal is automatically trailed.
	Additionally, a number of Notifiable's may be attached to this structure
	for receiving notifications on specific events (groundness, bound update
	or domain update).	Although these events overlap, a notifiable is
	called only once for the most specific event.

	\param Container Defines how elements are envd. A \p Util::StdList
	(sorted unique list), a \p Tree (sorted unique tree) or a user-defined
	structure are possible parameters (see \p ContainerTraits).
	\param Element Sets the kind of element representation. Currently either a
	\p Interval for interval based representations, or \p Element for single
	elements.
	\param T Any integral type (\p int, \p uint, \p Short, \p Char,
	or user-defined) that represents each element.

	\ingroup IntegerDomain CPIntegerDataStructures
*/
template<class Container = Casper::Detail::List,
		 class Element = Casper::Detail::Element,
		 class T = int>
class FD : private Casper::Detail::SelectElement<Element,T,Container,std::less<T> >::Type
{

	private:
		typedef		FD<Container,Element,T>	Self;
		typedef		typename Casper::Detail::SelectElement<Element,T,
							Container,std::less<T> >::Type Super;

	public:
		/// The atomic element type (same as \p T)
		typedef		T			Value;
		/// An integral unsigned Type.
		typedef		typename Super::Size Size;
		/// Reference to \p T.
		typedef		typename Super::Reference Reference;
		/// Const reference to \p T.
		typedef		typename Super::ConstReference ConstReference;
		/// Pointer to \p T.
		typedef		typename Super::Pointer Pointer;
		/// Iterator used to iterate through a \p FD.
		typedef		typename Super::Iterator Iterator;
		/// Const Iterator used to iterate through a \p FD.
		typedef		typename Super::ConstIterator ConstIterator;
		/// Iterator used to iterate backwards through a \p FD.
		typedef 	typename Super::ReverseIterator ReverseIterator;
		/// Const iterator used to iterate backwards through a \p FD.
		typedef 	ReverseIterator ConstReverseIterator;

		/// Creates a FD with a copy of the range [\a first ,\a last ).
		template <class InputIterator>
				FD(Store& store, InputIterator first, InputIterator last) :
					Super(store,first, last),
					_min(store,*first),_max(store,*--last),
					groundSL(store),boundsSL(store),domainSL(store) {}

		/// Creates a FD with a copy of the list of ranges \a l.
				FD(Store& store, const Util::StdList<Util::StdRange<Value> >& l) :
							Super(Detail::rangeStruct(this,store,l)),
							_min(store,*Super::begin()),
							_max(store,*--Super::end()),
							groundSL(store),boundsSL(store),domainSL(store) {}

		/// Creates a new FD containing element \a e
				FD(Store& store, const Value& e) :
					Super(store,Util::SCIterator<Value>(e), ++Util::SCIterator<Value>(e)),
					_min(store,e),_max(store,e),
					groundSL(store),boundsSL(store),domainSL(store) {}

		/** Creates a new FD containing all elements in the interval
			[\a first ,\a last ]. */
				FD(Store& store, Value first, Value last) :
					Super(store,Util::SCIterator<Value>(first),++Util::SCIterator<Value>(last)),
					_min(store,first),_max(store,last),
					groundSL(store),boundsSL(store),domainSL(store) {}

		///	Copy constructor
				FD(const FD& s) :
					Super(s),
					_min(s._min),_max(s._max),
					groundSL(s.groundSL),boundsSL(s.boundsSL),
					domainSL(s.domainSL) {}// CASPER_UNTESTED("FD copy ctor");  }


		/** \brief Generic copy constructor. Creates a copy of the \p FD
			\a s, even if \a s has a different internal structure
		    \note Elements are always sorted according
		    to the \p CompareT function.
		*/
		template<FD_TP_SPEC(1)>
				FD(const FD<FD_TP_LIST(1)>& s)	:
				_min(s._min),_max(s._max),
					groundSL(s.groundSL),boundsSL(s.boundsSL),
					domainSL(s.domainSL)
		{	CASPER_UNTESTED("FD copy ctor"); copyC(*this,s);	}

		/// The destructor.
	 			~FD()	{}

		/// Returns the Env object associated with this FD.
	//	Env&	getEnv() const {	return Super::solver(); }

		bool operator=(const Value& v);
		bool operator=(Iterator it);

		/// Returns \p true if \p this has exactly the same elements as \a s.
		template<FD_TP_SPEC(1)>
		bool 	operator==(const FD<FD_TP_LIST(1)>& s) const
		{ return isEqual(*this,s);}

		/// Returns the first element of the FD. \pre empty() is false.
		inline Value	min() const {assert(!empty()); return _min;}

		/// Returns the last element of the FD. \pre empty() is false.
		inline Value	max() const {assert(!empty()); return _max;}

		/// Returns the (unique) element of the FD. \pre ground() is true.
		inline Value	value() const {assert(ground()); return min(); }

		/** Returns \p true if the FD has exactly one element, \p false
			otherwise. O(1). */
		inline bool 	singleton() const
		{ return _min == _max; }

		/// Same as \a singleton()
		inline bool 	ground() const
		{ return singleton(); }

		/// Returns \p true if this container has no elements. O(1).
		inline bool 	empty() const { return Super::empty(); }

		///	Returns an Iterator pointing to the beginning of the FD.
		inline Iterator begin() const { return Super::begin(); }

		/// Returns an Iterator pointing to the end of the FD.
		inline Iterator end() const  { return Super::end(); }

		///	Returns a reverse Iterator pointing to the begin of the reversed FD
		ReverseIterator rbegin() const {return ReverseIterator(end());}

		///	Returns a reverse Iterator pointing to the end of the reversed FD.
		ReverseIterator rend() const {return ReverseIterator(begin());}

		/** Returns an Iterator pointing to the element \a k if exists or to
			 end() otherwise. */
		Iterator find(const Value& k) const  { return Super::find(k); }

		/// Finds the first element not less than k.
		Iterator lowerBound(const Value& k) const
		{return Super::lowerBound(k);}

		/// Finds the first element greater than k.
		Iterator upperBound(const Value& k) const
		{return Super::upperBound(k);}

		/// Returns the number of elements in finite domain.
		inline Size size() const  { return Super::size(); }

		/// Returns the number of filters referencing this domain
		uint getNbSuspFilters() const;
		/// Returns the number of weighted filters referencing this domain
		uint getNbSuspWeightedFilters() const;
		/// Returns the accumulated failure count of the domain
		uint getAFC() const;

		bool erase(Value min, Value max);
		inline bool erase(const Value& k);
		inline bool erase(Iterator p);
		bool erase(Iterator p, Iterator q);
		bool updateMin(Iterator);
		bool updateMax(Iterator);

		inline bool updateMin(const Value& v);
		inline bool updateMax(const Value& v);
		inline bool updateRange(const Value& min,const Value& max);
		template<class Iterator1>
		bool intersect(Iterator1 b, Iterator1 e);
		template<class Iterator1>
		bool erase(Iterator1 b, Iterator1 e);

		/// Erases all elements in the FD.
		inline void clear()  { 	Super::clear(); }

		friend struct EraseInterval<Self> ;
		friend bool isEqual<>(const FD&,const FD&);
#ifndef SWIG
		friend std::ostream& operator<< <>(std::ostream&, const FD&);
#endif
		/// Registers notifiable \a f on groundness event
		void attachOnGround(INotifiable*	f) { groundSL.attach(f); }

		/// Registers notifiable \a f on bound update event
		void attachOnBounds(INotifiable*	f) { boundsSL.attach(f); }

		/// Registers notifiable \a f on domain update event
		void attachOnDomain(INotifiable*	f) { domainSL.attach(f); }

		/// Unregisters notifiable \a f from groundness event
		void detachOnGround(INotifiable*	f) { groundSL.detach(f); }

		/// Unregisters notifiable \a f from bound update event
		void detachOnBounds(INotifiable*	f) { boundsSL.detach(f); }

		/// Unregisters notifiable \a f from domain update event
		void detachOnDomain(INotifiable*	f) { domainSL.detach(f); }

		/// Describes an atomic prunning of the domain (a range of contiguous elements)
		struct Delta {
			Iterator f;
			Iterator l;
			/// Iterator pointing to the first element of the removed range
			Iterator first() const { return f; }
			/// Iterator pointing to the last element of the removed range
			Iterator last() const  { return l; }
		};

		/// Return information regarding the last atomic prunning of the set.
		inline const Delta&	delta() const	{	return mDelta;	}

	private:

		inline void triggerBeforeErase(Iterator it);
		inline void triggerBeforeErase(Iterator it1,Iterator it2);
		inline bool triggerAfterErase();

		Delta				mDelta;

		Reversible<Value>	_min;
		Reversible<Value>	_max;

		EventSuspList			groundSL;
		EventSuspList			boundsSL;
		EventSuspList			domainSL;
};

template<FD_TP_SPEC(1)>
uint FD<FD_TP_LIST(1)>::getNbSuspFilters() const
{	return groundSL.getNbFilters() + boundsSL.getNbFilters() + domainSL.getNbFilters(); }

template<FD_TP_SPEC(1)>
uint FD<FD_TP_LIST(1)>::getNbSuspWeightedFilters() const
{	return groundSL.getNbWeightedFilters() + boundsSL.getNbWeightedFilters() + domainSL.getNbWeightedFilters(); }

template<FD_TP_SPEC(1)>
uint FD<FD_TP_LIST(1)>::getAFC() const
{
	return groundSL.getTotalAFC() + boundsSL.getTotalAFC() + domainSL.getTotalAFC();
}

/**
	\name Domain prunning methods

	The following methods remove elements from the domain. The also update
	\a lastErasedRangeMin and \a lastErasedRangeMax accordingly and inform
	all registered Notifiable's.

	\return \a False if the operation leads to failure either by making
	the domain empty or by evaluation of a registered Notifiable.
*/
//@{

/**
	\brief Assigns the value \a v to the finite domain.

	If \a v is not in the domain it fails.
*/
template<FD_TP_SPEC(1)>
bool FD<FD_TP_LIST(1)>::operator=(const Value& v)
{
	Iterator it = find(v);
	if (it == end())
		return false;
	if (it != begin() and !erase(begin(),it))
		return false;
	if (++it != end() and !erase(it,end()))
		return false;
	return true;
}

/**
	\brief Assigns the value pointed by \a it to the finite domain.
	\pre it
*/
template<FD_TP_SPEC(1)>
bool FD<FD_TP_LIST(1)>::operator=(Iterator it)
{
	if (it == end())
		return false;
	if (it != begin() and !erase(begin(),it))
		return false;
	if (++it != end() and !erase(it,end()))
		return false;
	return true;
}

/// Erases all elements in the interval [\a min , \a max ].
template<FD_TP_SPEC(1)>
bool FD<FD_TP_LIST(1)>::erase(Value min, Value max)
{
	Iterator b = lowerBound(min);
	Iterator e = upperBound(max);
	if (b==e)	return true;
	triggerBeforeErase(b,e);
	if (!EraseInterval<Self>()(*this,b,e))	return false;
	if (_min >= *delta().first() and _min <= *delta().last())
		_min = *begin();
	if (_max >= *delta().first() and _max <= *delta().last())
		_max = *--end();
	return triggerAfterErase();
}

/// Erases element \a k if it exists.
template<FD_TP_SPEC(1)>
inline bool FD<FD_TP_LIST(1)>::erase(const Value& k)
{
	assert(!empty());
	Iterator it = find(k);
	if (it == end())	return true;
	//std::cout << this << " : erase : " << *this << " = " <<  k << std::endl;
	return erase(it);
}

/// Erases element at position \a p. \pre \a p is a valid Iterator
template<FD_TP_SPEC(1)>
inline bool FD<FD_TP_LIST(1)>::erase(Iterator p)
{
	triggerBeforeErase(p);
	Super::erase(p);
	if (empty()) return false;
	if (_min == *delta().first())
		_min = *begin();
	if (_max == *delta().first())
		_max = *--end();
	return triggerAfterErase();
}

/// Erases all elements in [\a p, \a q )
template<FD_TP_SPEC(1)>
bool FD<FD_TP_LIST(1)>::erase(Iterator p,Iterator q)
{
	triggerBeforeErase(p,q);
	Super::erase(p,q);
	if (empty()) return false;
	if (_min >= *delta().first() && _min <= *delta().last())
		_min = *begin();
	if (_max >= *delta().first() && _max <= *delta().last())
		_max = *--end();
	return triggerAfterErase();
}

/// Removes the interval [min,ith) from the domain
template<FD_TP_SPEC(1)>
bool FD<FD_TP_LIST(1)>::updateMin(Iterator ith)
{
	triggerBeforeErase(begin(),ith);
	Super::updateMin(ith);
	_min = *ith; //*begin();
	return triggerAfterErase();
}

/// Removes the interval [min,v) from the domain
template<FD_TP_SPEC(1)>
inline bool FD<FD_TP_LIST(1)>::updateMin(const Value& v)
{
	if (v <= min())	return true;
	Iterator ith = Super::lowerBound(v);
	if (ith == end()) return false;
	//std::cout << this << " : updateMin : " << *this << " = " <<  v << std::endl;
	return updateMin(ith);
}

/// Removes the interval [ith,max] from the domain
template<FD_TP_SPEC(1)>
bool FD<FD_TP_LIST(1)>::updateMax(Iterator ith)
{
	triggerBeforeErase(ith,end());
	Super::updateMax(--ith);
	_max = *ith; //*--end();
	return triggerAfterErase();
}

/// Removes the interval (v,max] from the domain
template<FD_TP_SPEC(1)>
inline bool FD<FD_TP_LIST(1)>::updateMax(const Value& v)
{
 	if (v >= max())	return true;
 	Iterator ith = Super::upperBound(v);
 	if (ith == begin()) return false;
 	//std::cout << this << " : updateMax : " << *this << " = " <<  v << std::endl;
	return updateMax(ith);
}

/// Removes everything not in [min,max] from the domain
template<FD_TP_SPEC(1)>
inline bool FD<FD_TP_LIST(1)>::updateRange(const Value& min,const Value& max)
{
	// FIXME: make more effient (call triggerAfterErase once)
	return updateMin(min) && updateMax(max);
}

/// Intersects FD with all values in iteratable range defined by [b,e[
/// \pre Values in the range are sorted.
template<FD_TP_SPEC(1)>
template<class Iterator1>
bool FD<FD_TP_LIST(1)>::intersect(Iterator1 b,Iterator1 e)
{
	assert(!empty());
	assert(b!=e);
	if (!updateMax(*--Iterator1(e)) or !updateMin(*b))
		return false;
	Iterator myIt = begin();
	while (b != e and myIt != end())
	{
		if (*myIt < *b)	// prune value
		{
		/*	Iterator myStart(myIt);	// trying to erase in ranges, does it payoff ??
			while (*myIt < *b)
				++myIt;
			if (!erase(myStart,myIt))
				return false;
		*/	if (!erase(myIt++))
				return false;
		}
		else
		if (*myIt == *b)
		{
			++b; ++myIt;
		}
		else
		{
			assert(*b < *myIt);
			++b;
		}
	}
	return true;
}


/// Erases all values in iterateable range defined by [b,e[ from FD
/// \pre Values in the range are sorted.
template<FD_TP_SPEC(1)>
template<class Iterator1>
bool FD<FD_TP_LIST(1)>::erase(Iterator1 b,Iterator1 e)
{
	assert(!empty());

	Iterator it = begin();
	while (b!=e and it!=end())
	{
		if (*b < *it)
			++b;
		else
		if (*b > *it)
			++it;
		else
		if (!erase(it++))
			return false;
		else
			++b;
	}
	return true;
}
//@}

template<FD_TP_SPEC(1)>
inline void FD<FD_TP_LIST(1)>::triggerBeforeErase(Iterator it)
{
	mDelta.f = mDelta.l = it;
}

template<FD_TP_SPEC(1)>
inline void FD<FD_TP_LIST(1)>::triggerBeforeErase(Iterator it1,Iterator it2)
{
	mDelta.f = it1;
	mDelta.l = --it2;
}

/*
template<FD_TP_SPEC(1)>
void FD<FD_TP_LIST(1)>::triggerBeforeErase(Value from, Value to)
{
	lastErasedRangeMin = from;
	lastErasedRangeMax = to;
}*/

template<FD_TP_SPEC(1)>
inline bool FD<FD_TP_LIST(1)>::triggerAfterErase()
{
	assert(!empty());

	//solver().signalDomainUpdate();
	if (singleton())
		if (!groundSL.notifyAll() or empty())
			return false;
	if (*delta().first() < _min or *delta().last() > _max)
		if (!boundsSL.notifyAll() or empty())
			return false;
	return domainSL.notifyAll();
}

/**
	Generic copy constructor. Populates the FD inserting each element. O(N)
*/
template<FD_TP_SPEC(1),FD_TP_SPEC(2)>
inline void copyC(FD<FD_TP_LIST(1)>& me, const FD<FD_TP_LIST(2)>& s)
{ C_DEBUG_COUT("copyC\n");me.insert(s.begin(),s.end());}

/**
	Generic comparison. Compares each element. O(N)
*/
template<FD_TP_SPEC(1),FD_TP_SPEC(2)>
inline bool isEqual(const FD<FD_TP_LIST(1)>& me,
					const FD<FD_TP_LIST(2)>& s)
{
	return (!me.empty() && !s.empty() && equal(me.begin(),me.end(),s.begin()))
			||	(me.empty() && s.empty());
}

/**
	Copy constructor when creating an FD from an FD with the same
	element type. O(s(N))
*/

template<FD_TP_SPEC(1),FD_TP_NE_SPEC(2)>
inline void copyC(FD<FD_TP_LIST(1)>& me,
	  			  const FD<Container2,Element1,T2>& s)
{
	typedef typename FD<FD_TP_LIST(1)>::Super Super_me;
	typedef typename FD<Container2,Element1,T2>::Super Super_s;
	((Super_me&)me)(s);
}

/**
	Comparison between an FD and another FD with the same
	element type. O(s(N))
*/
template<FD_TP_SPEC(1),FD_TP_NE_SPEC(2)>
inline bool
isEqual(const FD<FD_TP_LIST(1)>& me,
	    const FD<Container2,Element1,T2>& s)
{
	typedef typename FD<FD_TP_LIST(1)>::Super Super_me;
	typedef typename FD<Container2,Element1,T2>::Super Super_s;
	return ((const Super_me&)me) == s;
}

/**
	Removes a range of values from the domain.
	@return \p false if the domain is empty, \p true otherwise
	@cost O(s(N))
	@todo Can be optimized when the structure is a list or vector.
*/
template<FD_TP_SPEC(1)>
struct EraseInterval<FD<FD_TP_LIST(1)> >
{
	typedef FD<FD_TP_LIST(1)> Self;
	typedef typename Self::Super	Super;
	typedef typename Self::Iterator Iterator;
	bool operator()(Self& t, Iterator b,Iterator e)
	{
		if (b != t.end())
		{
			//std::cout << t << " : " << *b << " " << *e << std::endl;
			((Super&)t).erase(b,e); //t.erase(b,e);
			return !t.empty();
		}
		else
			return true;
	}
};



/**
	FD specialization for BitsetA

*/

// macro for avoiding writing the template specification for
// every method.
#define FD_BITSET_TP_SPEC(N) \
	 class UnusedT1##N,int Size##N,class T##N

// macro for avoiding writing the template parameter list
#define FD_BITSET_TP_LIST(N)\
 UnusedT1##N,Casper::Detail::Bitset<Size##N>,T##N


/**
	Removes a range of values from the domain. Optimized for bitsets.
	@return \p false if the domain is empty, \p true otherwise
	@cost O(Size)
*/
template<FD_BITSET_TP_SPEC(1)>
struct EraseInterval<FD<FD_BITSET_TP_LIST(1)> >
{
	typedef FD<FD_BITSET_TP_LIST(1)>  Self;
	typedef typename Self::Iterator Iterator;
	bool operator()(Self& t, Iterator b, Iterator e)
	{
		typedef typename Self::Super Super;
		//typename Self::Size _count = ((typename Self::Super&)t).count;
		T1 min = *b;
		T1 max = *--e;
		for (T1 n = min; n <= max; n++)
			if (t[n])
			{
				t[n] = false;
				((Super&)t).count = ((Super&)t).count - 1;
			}
		return ((Super&)t).count > 0;
	}
};

typedef FD<void,Casper::Detail::Bitset<2>,bool> BD;

namespace Traits {
template<class T>
struct GetDefaultDom;

template<>
struct GetDefaultDom<int>
{	typedef FD<Casper::Detail::List,Casper::Detail::Interval,int>	Type;	};
//{	typedef FD<CP::List,Element,int>	Type;	};

template<>
struct GetDefaultDom<bool>
{	typedef BD		Type;	};

} // Traits


template <class,class> class Var;

namespace Detail {

// Removing a set of n random values from a FD is O(n^2). This structure
// allows to cache removals thus reducing the complexity to O(n).
template<class Dom>
struct BatchRangeErase
{
	BatchRangeErase(Util::IHeap& heap, Var<int,Dom> v) :
		v(v),e(heap,v.domain().max()-v.domain().min()+1,false),
		minElem(v.domain().min()),maxElem(v.domain().max()) {}

	void erase(int elem)
	{
		if (elem >= minElem and elem <= maxElem)
			e[elem-minElem] = true;
	}
	bool commit()
	{
		typename Dom::Iterator startIt,it;
		bool inRange = false;
		for (it = v.domain().begin(); it != v.domain().end(); )
		{
			//assert(*it - minElem >= (int)0 and *it - minElem < (int)e.size());
			if (e[*it - minElem])
			{
				if (!inRange)
				{
					startIt = it;
					inRange = true;
				}
				++it;
			}
			else
			{
				if (inRange)
				{
					if (!v.domain().erase(startIt,it++))
						return false;
					inRange=false;
				}
				else
					++it;
			}
		}
		if (inRange)
			return v.domain().erase(startIt,it);
		return true;
	}
	Var<int,Dom> 	v;
	Util::StdVector<bool>	e;
	int				minElem;
	int				maxElem;
};
} // Detail

} // CP

namespace Traits {
template<class> struct GetEval;
template<class C,class E,class T>
struct GetEval<Casper::CP::FD<C,E,T> >
{	typedef int		Type;	};
}

} // Casper


template<FD_TP_SPEC(1)>
std::ostream& operator<<(std::ostream& os, const Casper::CP::FD<FD_TP_LIST(1)>& f)
{
	typedef typename Casper::CP::FD<FD_TP_LIST(1)>::ConstIterator ConstIterator;
	if (f.empty() == 1)	// no elements
		os << "{}";
	else
	if (f.size() == 1)	// one element
		os << *f.begin();
	else
	{
		ConstIterator itprev = f.begin();
		ConstIterator it1 = f.begin();
		++it1;
		while (it1 != f.end())
		{
			if (*it1 != *itprev+1)
				break;
			++it1; ++itprev;
		}
		if 	(it1 == f.end())	// one range
			os << "[" << *f.begin() << ".." << *itprev << "]";
		else
		{
			ConstIterator rangeStart = f.begin();
			itprev = f.begin();
			it1 = f.begin();
			++it1;
			os << "{" << *itprev;
			while (it1 != f.end())
			{
				if (*it1 != *itprev+1)
				{
					if (rangeStart != itprev)
						os << ".." << *itprev;
					os << "," << *it1;
					rangeStart = it1;
				}
				++it1; ++itprev;
			}
			if (rangeStart != itprev)
				os << ".." << *itprev;
			os << "}";
		}
	}
	//os << (typename FD<FD_TP_LIST(1)>::Super&)f;
	return os;
}

template<FD_TP_SPEC(1)>
std::istream& operator>>(std::istream& is, Casper::CP::FD<FD_TP_LIST(1)>& f)
{
	using namespace std;
	typedef Casper::CP::FD<FD_TP_LIST(1)> Dom;
	typedef typename Dom::Value	Value;
	is >> ws;
	char c = is.peek();

	if (c == '[')	// read range
	{
		is.get(c);
		Value start,end;
		char dot1,dot2;
		is >> ws >> start >> ws >> dot1 >> dot2 >> ws >> end;
		f.updateMin(start);
		f.updateMax(end);
		is >> ws;
		is.get(c);
		assert(c == ']');
	}
	else
	if (c == '{')	// read value list
	{
		is.get(c);
		typename Dom::Iterator it = f.begin();
		Value value;
		char sep = 0;
		while (sep != '}' && it != f.end())
		{
			is >> ws >> value >> ws >> sep >> ws;
			typename Dom::Iterator it2 = f.lowerBound(value);
			if (it2 != it)
				f.erase(it,it2);
			if (sep == '.')	// if reading a range
			{
				is >> sep >> ws >> value;
				assert(sep == '.');
				it = f.upperBound(value);
				is >> ws >> sep;
			}
			else
			{
				it = it2;
				++it;
			}
		}
		if (f.max() > value)
			f.updateMax(value);
	}
	else	// c must be a number
	{
		Value value;
		is >> value;
		f.updateMax(value);
		f.updateMin(value);
	}
	return is;
}

#endif

