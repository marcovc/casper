/***************************************************************************
 *   Copyright (C) 2003/2007 by Marco Correia                              *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _H_CASPER_SET_SETFD
#define _H_CASPER_SET_SETFD

#include <casper/set/util.h>

#include <casper/kernel/common.h>
#include <casper/kernel/notifiable.h>
#include <casper/kernel/container/sciterator.h>
#include <casper/kernel/container/rsulist.h>
#include <casper/kernel/susplist.h>
#include <casper/kernel/rangedom.h>

#include <casper/int/fd.h>

//#include <casper/set/common.h>


// required forward declarations
namespace casper {
template<class T> class SetFD;

template<class T>
std::ostream& operator<<(std::ostream& os,const casper::SetFD<T>& f);

/// Describes an atomic prunning of the set (a range of contiguous elements)
template<class T,class PIterator>
struct SetFDDelta
{
	typedef T Eval;
	typedef	PIterator Iterator;
	SetFDDelta() {}
	SetFDDelta(PIterator b,PIterator e) : b(b),e(e) { assert(b!=e);}
	SetFDDelta(PIterator it) : b(it),e(++it) { assert(b!=e);}

	PIterator b;
	PIterator e;
	/// PIterator pointing to the begining of the removed range
	PIterator begin() const { return b; }
	/// PIterator pointing to the end of the removed range
	PIterator end() const  { return e; }
};

/**
	\brief Data structure for set domains (bounds only)

	A set domain is represented by a list of elements which belong to the
	domain (\a in), and another for those which may be included in the domain
	(\a poss).

	Primitives include testing for inclusion in each of these lists,
	migrating an element from the \a poss list to the \a in list, or excluding
	it from \a poss.

	A set becomes instantiated when \a poss is empty. Empty sets
	(if \a in is also empty) are allowed.

	All operations are reversible, which means that every element removal/insertion
	is automatically trailed.

	Additionally, a number of Notifiable's may be attached to this structure
	for receiving notifications on specific events (groundness, glb, lub, any)

	The set keeps track of the last atomic prunning (a removal of a range
	of contiguous elements), which may be consulted using the delta() method.

	\ingroup Set
*/
template<class T = Int>
struct SetFD
{

	private:
		typedef		SetFD<T>	Self;

	public:
		/// The set element type (same as \p T)
		typedef		T				Value;
		/// The implementation type for the \p in set
		typedef		detail::RSUList<Value>	In;
		/// The implementation type for the \p poss set
		typedef		detail::RSUList<Value>	Poss;

		///	The implementation type for the \p card finite domain
		typedef		CurSetFDCard	Card;

		/// Iterator for the \p in set
		typedef		typename In::Iterator IIterator;
		/// Iterator for the \p poss set
		typedef		typename Poss::Iterator PIterator;

		/**
		 *  Creates a SetFD with possible values from the sorted range
		 *  [\a possBegin ,\a possEnd ).
		 **/
		template <class InputIterator>
				SetFD(ICPSolver& solver, InputIterator possBegin,
									InputIterator possEnd);

		/**
		 *  Creates a SetFD with possible values from the sorted range \a rPoss
		 **/
				SetFD(ICPSolver& solver, const Range<Value>& rPoss);

		/**
		 *  Creates a SetFD with possible values from the sorted list \a rPoss
		 **/
				SetFD(ICPSolver& solver, const detail::List<Value>& rPoss);

		/**
		 *  Creates a SetFD with possible values from the sorted range
		 *  [\a possBegin ,\a possEnd ), and already containing
		 *  elements in the sorted range [\a inBegin ,\a inEnd ).
		 *  \pre These two sets must be disjoint.
		 **/
		template <class InputIterator1,class InputIterator2>
				SetFD(ICPSolver& solver, InputIterator1 inBegin,
									InputIterator1 inEnd,
									InputIterator2 possBegin,
									InputIterator2 possEnd);

		/**
		 *  Creates a SetFD with possible values from the sorted range
		 *  \a rPoss, and already containing
		 *  elements in the sorted range \a rIn .
		 *  \pre These two sets must be disjoint.
		 **/
				SetFD(ICPSolver& solver, const Range<Value>& rIn,
									 const Range<Value>& rPoss);

		/**
		 *  Creates a SetFD with possible values from the sorted list
		 *  \a rPoss, and already containing
		 *  elements in the sorted list \a rIn .
		 *  \pre These two sets must be disjoint.
		 **/
				SetFD(ICPSolver& solver, const detail::List<Value>& rIn,
									 const detail::List<Value>& rPoss);

		///	Copy constructor
				SetFD(const SetFD& s) :
					inSetSize(s.solver(),s.inSize()),
					possSetSize(s.solver(),s.possSize()),
					inSet(s.in()),possSet(s.poss()),
					cardSet(s.card()),
					groundSL(s.groundSL),glbSL(s.glbSL),lubSL(s.lubSL),domainSL(s.domainSL)
				{
				}

		/// The destructor.
	 			~SetFD()	{	/* Nothing to do - GC takes care. */}

		/// Returns the ICPSolver& object associated with this SetFD.
		ICPSolver&	solver() const {	return inSet.solver(); }

		/// \a True if the set is instantiated. O(1).
		Bool 	ground() const { return possSet.empty(); }

		/// Returns \p true if this set is instantiated and has no elements. O(1).
		Bool 	empty() const { return ground() and inSet.empty(); }

		/**
		 * 	Direct (rw) access to the \a in set.
		 *  \warning Directly updating this set may lead to
		 *  (undetected) inconsistent states.
		 **/
		In&		in() 	{	return inSet;	}

		/**
		 * 	Direct (rw) access to the \a poss set.
		 *  \warning Directly updating this set may lead to
		 *  (undetected) inconsistent states.
		 **/
		Poss& 	poss() 	{	return possSet;	}

		///	Size of the \p in set. O(1)
		UInt	inSize() const	{	return inSetSize;	}

		///	Size of the \p poss set. O(1)
		UInt	possSize() const	{	return possSetSize;	}

		/// Direct (ro) access to the \a in set.
		const In& in() const 	{ return inSet; }
		/// Direct (ro) access to the \a poss set.
		const Poss& poss() const { return possSet;	}

		/**
		 * 	Direct (rw) access to the \a card set. Direct updates
		 *  on this set are safe.
		 **/
		Card&	card()	{	return cardSet;	}

		/// Direct (ro) access to the \a card set.
		const Card& card() const { return cardSet;	}
		Int minCard() const { return card().min(); }
		Int maxCard() const { return card().max(); }

		///	Returns a PIterator pointing to the beginning of the poss set.
		PIterator beginPoss() const { return possSet.begin(); }

		///	Returns a PIterator pointing to the end of the poss set.
		PIterator endPoss() const { return possSet.end(); }

		/// Returns an IIterator pointing to the beginning of the in set.
		IIterator beginIn() const  { return inSet.begin(); }

		/// Returns an IIterator pointing to the end of the in set.
		IIterator endIn() const  { return inSet.end(); }

		IIterator findInIn(const Value& v) const { return inSet.find(v);	}
		PIterator findInPoss(const Value& v) const { return possSet.find(v);	}

		Bool clearPoss();

		Bool erase(const Value& k);
		Bool safeErase(const Value& k);
		Bool erase(PIterator p);
		Bool erase(PIterator p, PIterator q);

		Bool insert(const Value& k);
		Bool safeInsert(const Value& k);
		Bool insert(PIterator p);
		Bool insert(PIterator p, PIterator q);

		Bool insertBefore(IIterator a, PIterator p);
		Bool insertBefore(IIterator a, PIterator p, PIterator q);

		/// Sends the contents of the set to stream \p os
		friend std::ostream& operator<< <>(std::ostream& os, const SetFD& f);

		/// Registers notifiable \a f on groundness event
		Void attachOnGround(INotifiable*	f) { groundSL.attach(f); }

		/// Registers notifiable \a f on bound update event
		Void attachOnLUB(INotifiable*	f) { lubSL.attach(f); }

		/// Registers notifiable \a f on bound update event
		Void attachOnGLB(INotifiable*	f) { glbSL.attach(f); }

		/// Registers notifiable \a f on domain update event
		Void attachOnDomain(INotifiable*	f) { domainSL.attach(f); }

		/// Unregisters notifiable \a f from groundness event
		Void detachOnGround(INotifiable*	f) { groundSL.detach(f); }

		/// Unregisters notifiable \a f from bound update event
		Void detachOnLUB(INotifiable*	f) { lubSL.detach(f); }

		/// Unregisters notifiable \a f from bound update event
		Void detachOnGLB(INotifiable*	f) { glbSL.detach(f); }

		/// Unregisters notifiable \a f from domain update event
		Void detachOnDomain(INotifiable*	f) { domainSL.detach(f); }

		/// Describes an atomic prunning of the set (a range of contiguous elements)
		typedef SetFDDelta<T,PIterator> Delta;

		/// Data structure for storing and retrieving deltas
		typedef detail::DeltaList<Delta>	Deltas;

		/// Iterator for retrieving deltas
		typedef typename Deltas::Iterator	DeltasIterator;

		/// List of inserts in the set since the last choice point
		const Deltas&	glbDeltas() const	{	return mGLBDeltas;	}

		/// List of removals from the set since the last choice point
		const Deltas&	lubDeltas() const	{	return mLUBDeltas;	}

	private:

		template<class InputIterator>
		UInt count(InputIterator b, InputIterator e);
		Void onBeforeErase(PIterator e);
		Void onBeforeErase(PIterator b, PIterator e);
		Void onBeforeInsert(PIterator e);
		Void onBeforeInsert(PIterator b, PIterator e);

		Bool onAfterErase();
		Bool onAfterInsert();

		Deltas				mGLBDeltas;
		Deltas				mLUBDeltas;

		Reversible<UInt>	inSetSize;		// for efficiency
		Reversible<UInt>	possSetSize;	// for efficiency

		In					inSet;
		Poss				possSet;
		Card				cardSet;
		EventSuspList			groundSL;
		EventSuspList			glbSL;
		EventSuspList			lubSL;
		EventSuspList			domainSL;
};

template<class> struct ElemTraits;

template<class T,class PIterator>
struct ElemTraits<SetFDDelta<T,PIterator> >
{	typedef T	Elem;	};


template<class T>
template<class InputIterator>
UInt SetFD<T>::count(InputIterator b, InputIterator e)
{
	UInt ret = 0;
	while (b != e)
	{
		++b;
		++ret;
	}
	return ret;
}


template<class T>
Void SetFD<T>::onBeforeInsert(PIterator e)
{
	assert(e != endPoss());
	mGLBDeltas.store(Delta(e));
}

template<class T>
Void SetFD<T>::onBeforeInsert(PIterator b, PIterator e)
{
	assert(b != endPoss());
	mGLBDeltas.store(Delta(b,e));
}

template<class T>
Void SetFD<T>::onBeforeErase(PIterator e)
{
	assert(e != endPoss());
	mLUBDeltas.store(Delta(e));
}

template<class T>
Void SetFD<T>::onBeforeErase(PIterator b, PIterator e)
{
	assert(b != endPoss());
	mLUBDeltas.store(Delta(b,e));
}

template<class T>
Bool SetFD<T>::onAfterErase()
{
	//solver().signalDomainUpdate();
	if (ground())
		if (!groundSL.notifyAll())
			return false;
	if (!lubSL.notifyAll())
		return false;
	return domainSL.notifyAll();
}

template<class T>
Bool SetFD<T>::onAfterInsert()
{
	//solver().signalDomainUpdate();
	//std::cout << this << std::endl;
	if (ground())
		if (!groundSL.notifyAll())
			return false;
	if (!glbSL.notifyAll())
		return false;
	return domainSL.notifyAll();
}

/**
	\name Domain pruning methods

	The following methods remove elements from the domain. They also update
	delta information accordingly and informal registered Notifiables.

	\return \a False if the operation leads to failure either by making
	the domain inconsistent or by evaluation of a registered Notifiable.
*/
//@{

/**
 * 	Removes all elements from the (poss) set.
 *  \pre possSet is not empty.
 *  O(1)
 **/
template<class T>
Bool SetFD<T>::clearPoss()
{
	onBeforeErase(possSet.begin(),possSet.end());
	possSet.clear();
	possSetSize	= 0;
	return onAfterErase();
}

/**
 * 	Removes the element k from the (poss) set.
 *  \pre This element must not be part of the \a in set. This would
 *  lead to an inconsistent state which is not being checked. See safeErase.
 *  O(n)
 **/
template<class T>
Bool SetFD<T>::erase(const Value& k)
{
	assert(this->findInIn(k) == this->endIn());

	PIterator p = possSet.find(k);
	if (p != endPoss())
	{
		onBeforeErase(p);
		--possSetSize;
		possSet.erase(p);
		return onAfterErase();
	}
	return true;
}

/**
 * 	Removes the element k from the \p poss set. Returns false if
 *  the element is in the \p in set.
 *  O(2n)
 **/
template<class T>
Bool SetFD<T>::safeErase(const Value& k)
{
	PIterator p = possSet.find(k);
	if (p != endPoss())
	{
		onBeforeErase(p);
		--possSetSize;
		possSet.erase(p);
		return onAfterErase();
	}
	return this->findInIn(k) == this->endIn();
}

/**
 * 	Removes the element pointed by \a p from the (poss) set.
 *  O(1)
 **/
template<class T>
Bool SetFD<T>::erase(PIterator p)
{
	assert(this->findInIn(*p) == this->endIn());

	onBeforeErase(p);
	--possSetSize;
	possSet.erase(p);
	return onAfterErase();
}

/**
 * 	Removes the elements in the range [p,q) from the (poss) set.
 *  O(q-p)
 **/
template<class T>
Bool SetFD<T>::erase(PIterator p, PIterator q)
{
	assert(p != q);
	onBeforeErase(p,q);
	possSetSize -= count(p,q);
	possSet.erase(p,q);
	return onAfterErase();
}


/**
 * 	Removes the element \a k from the poss set and inserts it
 *  in the \p in set.
 *  \warning This function will succeed in case k is
 *  neither in the \p poss set nor in the \p in set. See safeInsert.
 *  O(n)
 **/
template<class T>
Bool SetFD<T>::insert(const Value& k)
{
	PIterator it = possSet.find(k);
	if (it == possSet.end())
		return true;
	return insert(it);
}

/**
 * 	Removes the element \a k from the poss set and inserts it
 *  in the \p in set. Returns false if the element is not in
 *  the \p possSet nor in \p in set.
 *  O(2n)
 **/
template<class T>
Bool SetFD<T>::safeInsert(const Value& k)
{
	PIterator it = possSet.find(k);
	if (it == possSet.end())
		return inSet.find(k)!=inSet.end();
	return insert(it);
}

/**
 * 	Removes the element pointed by \p p from the poss set and inserts it
 *  in the \p in set.
 *  O(n)
 **/
template<class T>
Bool SetFD<T>::insert(PIterator p)
{
	onBeforeInsert(p);
	inSet.insert(*p);
	++inSetSize;
	possSet.erase(p);
	--possSetSize;
	return onAfterInsert();
}

/**
 * 	Removes the elements in the range [p,q) from the poss set and inserts
 *  them in the \p in set.
 *  O(q-p+n)
 **/
template<class T>
Bool SetFD<T>::insert(PIterator p, PIterator q)
{
	assert(p != q);
	onBeforeInsert(p,q);
	UInt c = count(p,q);
	inSet.insert(p,q);
	inSetSize += c;
	possSet.erase(p,q);
	possSetSize -= c;
	return onAfterInsert();
}

/**
 * 	Removes the element pointed by \p p from the poss set and inserts it
 *  in the \p in set, just before the position \p a.
 *  O(1)
 **/
template<class T>
Bool SetFD<T>::insertBefore(IIterator a, PIterator p)
{
	onBeforeInsert(p);
	inSet.insert(a,*p);
	++inSetSize;
	possSet.erase(p);
	--possSetSize;
	return onAfterInsert();
}

/**
 * 	Removes the elements in the range [p,q) from the poss set and inserts
 *  them in the \p in set, just before the position \p a.
 *  \pre *--q < *a
 *  O(1)
 **/
template<class T>
Bool SetFD<T>::insertBefore(IIterator a, PIterator p, PIterator q)
{
	assert(p != q);
	onBeforeInsert(p,q);
	UInt c = count(p,q);
	for (PIterator t = p; t != q; ++t)
		inSet.insert(a,*t);
	inSetSize += c;
	possSet.erase(p,q);
	possSetSize -= c;
	return onAfterInsert();
}




template<class T>
std::ostream& operator<<(std::ostream& os, const casper::SetFD<T>& f)
{
	if (f.empty())	// no elements
		os << "{}";
	else
	if (f.ground())
	{
		//os << "{ " << f.in() << "}";
		os << f.in();
	}
	else
	{
		os << "{ " << f.in();
		os << "| " << f.poss() << "| "
				  << f.card()
		;
		os << "}";
	}
	return os;
}

namespace detail {

// finds range b..e in d.poss and inserts it in d.in
// \note this does not fail if some elements of b..e are not in d.poss,
// because they could be already in d.in. See setSafeInsertRange.
template<class T,class InputIterator>
Bool setInsertRange(SetFD<T>& d, InputIterator b, InputIterator e)
{
	typedef typename SetFD<T>::PIterator PIterator;

	assert(b != e);

	PIterator bit = d.beginPoss();

	for ( EVER )
	{
		// find first element in set.poss where *bit == *b
		for ( EVER )
		{
			if (b == e or bit == d.endPoss())
				return true;

			if (detail::less(*b,*bit))
				++b;
			else
			if (detail::less(*bit,*b))
				++bit;
			else
				break;
		}

		// try to find a common sequence [bit..eit] of elements for batch inserting
		PIterator eit = bit;
		while (b != e and eit != d.endPoss() and detail::lessEqual(*b,*eit))
		{
			if (detail::equal(*b,*eit))
				++eit;
			++b;
		}
		if (b == e)
			return d.insert(bit,eit);
		if (eit == d.endPoss())
			return d.insert(bit,eit);
		if (!d.insert(bit,eit))
			return false;
		bit = eit;
	}
}

// finds range b..e in d.poss and erases it
// \note it is ok if some elements of b..e are already not in d.poss
// \note this does not fail if some elements of b..e are in d.in. See
// setSafeEraseRange.
template<class T,class InputIterator>
Bool setEraseRange(SetFD<T>& d, InputIterator b, InputIterator e)
{
	typedef typename SetFD<T>::PIterator PIterator;

	assert(b != e);

	PIterator bit = d.beginPoss();

	for ( EVER )
	{
		// find first element in set.poss where *bit == *b
		for ( EVER )
		{
			if (b == e or bit == d.endPoss())
				return true;

			if (detail::less(*b,*bit))
				++b;
			else
			if (detail::less(*bit,*b))
				++bit;
			else
				break;
		}

		// try to find a common sequence [bit..eit] of elements for batch erasing
		PIterator eit = bit;
		while (b != e and eit != d.endPoss() and detail::lessEqual(*b,*eit))
		{
			if (detail::equal(*b,*eit))
				++eit;
			++b;
		}
		if (b == e)
			return d.erase(bit,eit);
		if (eit == d.endPoss())
			return d.erase(bit,eit);
		if (!d.erase(bit,eit))
			return false;
		bit = eit;
	}
}

// same as the above but using an iteration view
template<class T,class IView>
Bool setEraseRange(SetFD<T>& d, IView iview)
{
	typedef typename SetFD<T>::PIterator PIterator;

	PIterator pit = d.beginPoss();

	for ( EVER )
	{
		// find first element in set.poss where *pit == iview.value()
		for ( EVER )
		{
			if (!iview.valid() or pit == d.endPoss())
				return true;

			if (detail::less(iview.value(),*pit))
				iview.iterate();
			else
			if (detail::less(*pit,iview.value()))
				++pit;
			else
				break;
		}

		// try to find a common sequence [pit..eit] of elements for batch erasing
		PIterator eit = pit;
		while (iview.valid() and eit != d.endPoss() and detail::lessEqual(iview.value(),*eit))
		{
			if (detail::equal(iview.value(),*eit))
				++eit;
			iview.iterate();
		}
		if (!iview.valid())
			return d.erase(pit,eit);
		if (eit == d.endPoss())
			return d.erase(pit,eit);
		if (!d.erase(pit,eit))
			return false;
		pit = eit;
	}
}

// finds range b..e in d.poss and inserts it in d.in
// \note This checks if every element in [b..e] is in lub(d), and fails if it isn't
template<class T,class InputIterator>
Bool setSafeInsertRange(SetFD<T>& d, InputIterator b, InputIterator e)
{
	typedef typename SetFD<T>::PIterator PIterator;
	typedef typename SetFD<T>::IIterator IIterator;

	assert(b != e);

	PIterator pit = d.beginPoss();
	IIterator iit = d.beginIn();

	while ( b != e )
	{
		// try to find *b in d.poss
		while (pit != d.endPoss() and detail::less(*pit,*b))
			++pit;

		// if it is not in d.poss, it must be in d.in
		if (pit == d.endPoss() or detail::less(*b,*pit))
		{
			// try to find *b in d.in
			while (iit != d.endIn() and detail::less(*iit,*b))
				++iit;

			// if it is not in d.in, fail
			if (iit == d.endIn() or detail::less(*b,*iit))
				return false;

			// otherwise go to next *b
			++b;
		}
		else
		{
			// try to find a common sequence [pit..epit] of elements
			// for batch inserting

			PIterator epit = pit;

			while (b != e)
			{
				if (epit == d.endPoss() or detail::less(*epit,*b))
					break;
				else
				if (detail::less(*b,*epit))
				{
					while (iit != d.endIn() and detail::less(*iit,*b))
						++iit;
					if (iit == d.endIn() or detail::less(*b,*iit))
						return false;
				}
				else
					++epit;
				++b;
			}

			if (b == e)
				return d.insert(pit,epit);
			if (!d.insert(pit,epit))
				return false;
			pit = epit;
		}
	}
	return true;
}

// same as above but using IterationViews instead of pair of iterators
template<class T, class IView>
Bool setSafeInsertRange(SetFD<T>& d, IView iview)
{
	typedef typename SetFD<T>::PIterator PIterator;
	typedef typename SetFD<T>::IIterator IIterator;

	//assert(b != e);

	PIterator pit = d.beginPoss();
	IIterator iit = d.beginIn();

	while ( iview.valid() )
	{
		// try to find *b in d.poss
		while (pit != d.endPoss() and detail::less(*pit,iview.value()))
			++pit;

		// if it is not in d.poss, it must be in d.in
		if (pit == d.endPoss() or detail::less(iview.value(),*pit))
		{
			// try to find *b in d.in
			while (iit != d.endIn() and detail::less(*iit,iview.value()))
				++iit;

			// if it is not in d.in, fail
			if (iit == d.endIn() or detail::less(iview.value(),*iit))
				return false;

			// otherwise go to next value
			iview.iterate();
		}
		else
		{
			// try to find a common sequence [pit..epit] of elements
			// for batch inserting

			PIterator epit = pit;

			while (iview.valid())
			{
				if (epit == d.endPoss() or detail::less(*epit,iview.value()))
					break;
				else
				if (detail::less(iview.value(),*epit))
				{
					while (iit != d.endIn() and detail::less(*iit,iview.value()))
						++iit;
					if (iit == d.endIn() or detail::less(iview.value(),*iit))
						return false;
				}
				else
					++epit;
				iview.iterate();
			}

			if (!iview.valid())
				return d.insert(pit,epit);
			if (!d.insert(pit,epit))
				return false;
			pit = epit;
		}
	}
	return true;
}

// finds range b..e in d.poss and erases it
// returns false if some elements in b..e are in d.in
// \note it is ok if some elements of b..e are already not in d.poss
template<class T,class InputIterator>
Bool setSafeEraseRange(SetFD<T>& d, InputIterator b, InputIterator e)
{
	typedef typename SetFD<T>::PIterator PIterator;
	typedef typename SetFD<T>::IIterator IIterator;

	assert(b != e);

	PIterator pit = d.beginPoss();
	IIterator iit = d.beginIn();

	while ( b != e )
	{
		// try to find *b in d.poss
		while (pit != d.endPoss() and detail::less(*pit,*b))
			++pit;

		// if it is not in d.poss, it must not be in d.in
		if (pit == d.endPoss() or detail::less(*b,*pit))
		{
			// try to find *b in d.in
			while (iit != d.endIn() and detail::less(*iit,*b))
				++iit;

			// if it is in d.in, fail
			if (iit != d.endIn() and detail::equal(*b,*iit))
				return false;

			// otherwise go to next *b
			++b;
		}
		else
		{
			// try to find a common sequence [pit..epit] of elements
			// for batch erasing

			PIterator epit = pit;

			while (b != e)
			{
				if (epit == d.endPoss() or detail::less(*epit,*b))
					break;
				else
				if (detail::less(*b,*epit))
				{
					while (iit != d.endIn() and detail::less(*iit,*b))
						++iit;
					if (iit != d.endIn() and detail::equal(*b,*iit))
						return false;
				}
				else
					++epit;
				++b;
			}

			if (b == e)
				return d.erase(pit,epit);
			if (!d.erase(pit,epit))
				return false;
			pit = epit;
		}
	}
	return true;
}

// same as the above but using IterationViews
template<class T,class IView>
Bool setSafeEraseRange(SetFD<T>& d,IView iview)
{
	typedef typename SetFD<T>::PIterator PIterator;
	typedef typename SetFD<T>::IIterator IIterator;

	PIterator pit = d.beginPoss();
	IIterator iit = d.beginIn();

	while ( iview.valid() )
	{
		// try to find iview.value() in d.poss
		while (pit != d.endPoss() and detail::less(*pit,iview.value()))
			++pit;

		// if it is not in d.poss, it must not be in d.in
		if (pit == d.endPoss() or detail::less(iview.value(),*pit))
		{
			// try to find iview.value() in d.in
			while (iit != d.endIn() and detail::less(*iit,iview.value()))
				++iit;

			// if it is in d.in, fail
			if (iit != d.endIn() and detail::equal(iview.value(),*iit))
				return false;

			// otherwise go to next iview.value()
			iview.iterate();
		}
		else
		{
			// try to find a common sequence [pit..epit] of elements
			// for batch erasing

			PIterator epit = pit;

			while (iview.valid())
			{
				if (epit == d.endPoss() or detail::less(*epit,iview.value()))
					break;
				else
				if (detail::less(iview.value(),*epit))
				{
					while (iit != d.endIn() and detail::less(*iit,iview.value()))
						++iit;
					if (iit != d.endIn() and detail::equal(iview.value(),*iit))
						return false;
				}
				else
					++epit;
				iview.iterate();
			}

			if (!iview.valid())
				return d.erase(pit,epit);
			if (!d.erase(pit,epit))
				return false;
			pit = epit;
		}
	}
	return true;
}

// inserts the element referenced by the iterator, or the iteration,
// into the specified domain
// succeeds if elements to be inserted are in LUB(d), fails otherwise
template<class T>
struct SetInserter<SetFD<T> >
{
	typedef typename SetFD<T>::IIterator	IIterator;
	typedef typename SetFD<T>::PIterator	PIterator;

	SetInserter(SetFD<T>& d) : d(d),
						  lastIIt(d.beginIn()),
						  lastPIt(d.beginPoss()) {}
	template<class InputIterator>
	Bool operator()(InputIterator it)
	{
		// search for *it in [lastIIt,...]
		while (lastIIt != d.endIn() and detail::less(*lastIIt,*it))
			++lastIIt;
		if (lastIIt != d.endIn() and detail::equal(*lastIIt,*it))
			return true;	// succeeds if element already in the set
		// search for *it in [lastPIt,...]
		while (lastPIt != d.endPoss() and detail::less(*lastPIt,*it))
			++lastPIt;
		// fail if not found
		if (lastPIt == d.endPoss() or detail::greater(*lastPIt,*it))
			return false;
		assert(lastIIt == d.endIn() or detail::greater(*lastIIt,*it));
		return d.insertBefore(lastIIt,lastPIt);
	}

	SetFD<T>& d;
	IIterator lastIIt;
	PIterator lastPIt;
};

// removes the element referenced by the iterator
// from the specified domain
template<class T>
struct SetPItEraser<SetFD<T> >
{
	SetPItEraser(SetFD<T>& d) : d(d) {}
	Bool operator()(typename SetFD<T>::PIterator it)
	{	return d.erase(it);	}
	SetFD<T>& d;
};

// finds and removes the element referenced by the iterator
// from the specified domain
// (does not fail if element is not in 'poss' neither checks if is in 'in')
template<class T>
struct SetEraser<SetFD<T> >
{
	SetEraser(SetFD<T>& d) : d(d),bit(d.beginPoss()) {}
	template<class InputIterator>
	Bool operator()(InputIterator it)
	{
		while (bit != d.endPoss())
		{
			if (detail::less(*it,*bit))
				return true;
			else
			if (detail::less(*bit,*it))
				++bit;
			else
			return d.erase(bit++);
		}
		return true;
	}
	SetFD<T>& d;
	typename SetFD<T>::PIterator bit;
};

// finds and removes the element referenced by the iterator
// from the specified domain
// fails if element is in 'in', succeeds otherwise
template<class T>
struct SetSafeEraser<SetFD<T> >
{
	typedef typename SetFD<T>::PIterator PIterator;
	typedef typename SetFD<T>::IIterator IIterator;

	SetSafeEraser(SetFD<T>& d) : d(d),pit(d.beginPoss()),iit(d.beginIn()) {}
	template<class InputIterator>
	Bool operator()(InputIterator it)
	{
		while (pit != d.endPoss())
		{
			if (detail::less(*it,*pit))
				break;
			else
			if (detail::less(*pit,*it))
				++pit;
			else
			return d.erase(pit++);
		}
		while (iit != d.endIn() and detail::less(*iit,*it))
			++iit;
		return iit == d.endIn() or detail::less(*it,*iit);
	}

	SetFD<T>& d;
	PIterator pit;
	IIterator iit;
};

}

template<class T>
IterationView<typename SetFD<T>::In> makeInIt(SetFD<T>& s)
{	return makeIt(s.in()); }

template<class T>
IterationView<typename SetFD<T>::Poss> makePossIt(SetFD<T>& s)
{	return makeIt(s.poss()); }

template<class T>
UnionItView<IterationView<typename SetFD<T>::In>,
			IterationView<typename SetFD<T>::Poss> > makeLUBIt(SetFD<T>& s)
{	return makeUnionIt(makeIt(s.in()),makeIt(s.poss())); }

#ifdef CASPER_UNUSED
template<class T>
struct LessDelta
{
	Bool operator()(const typename SetFD<T>::Delta& s1,const typename SetFD<T>::Delta& s2) const
	{	return *s1.begin() < *s2.begin(); }
};

template<class T>
struct SortedDeltasIt
{
	typedef T Elem;
	typedef typename SetFD<T>::Delta Delta;
	typedef typename SetFD<T>::DeltasIterator DeltasIterator;

	typedef IteratorPairItView<Delta,DeltasIterator> IteratorPairIt;
	typedef SortedItView<IteratorPairIt,LessDelta<T> > SortedIt;

	typedef IterationView<Delta> DeltaIt;

	SortedDeltasIt(DeltasIterator from, DeltasIterator to) :
		sortedDeltasIt(makeIteratorPairIt<T>(from,to))
	{
		if (sortedDeltasIt.valid())
			curDeltaIt = DeltaIt(sortedDeltasIt.value());
	}

	const Elem& value() const
	{	assert(valid()); return curDeltaIt.value(); }

	Void iterate()
	{
		if (curDeltaIt.valid())
			curDeltaIt.iterate();
		else
		{
			sortedDeltasIt.iterate();
			if (sortedDeltasIt.valid())
				curDeltaIt = DeltaIt(sortedDeltasIt.value());
		}
	}

	Bool valid() const
	{	return sortedDeltasIt.valid();	}

	SortedIt sortedDeltasIt;
	DeltaIt curDeltaIt;
};

template<class Elem,class Iterator>
IterationItView<SortedItView<IteratorPairItView<typename SetFD<Elem>::Delta,Iterator>,LessDelta<Elem> > >
makeSortedDeltasIt(Iterator b,Iterator e)
{	return makeIterationIt(makeSortedIt<IteratorPairItView<typename SetFD<Elem>::Delta,Iterator>,LessDelta<Elem> >(makeIteratorPairIt<typename SetFD<Elem>::Delta,Iterator>(b,e)));	}
#endif

}

#endif
