/***************************************************************************
 *   Copyright (C) 2003/2007 by Marco Correia                              *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#ifndef _H_CASPER_SET_IDXSETFD
#define _H_CASPER_SET_IDXSETFD


#include <casper/kernel/common.h>
#include <casper/kernel/notifiable.h>
#include <casper/kernel/container/hashmap.h>
#include <casper/kernel/container/slist.h>
#include <casper/kernel/susplist.h>
#include <casper/kernel/rangedom.h>

#include <casper/int/fd.h>

//#include <casper/set/common.h>

// required forward declarations
namespace casper {
template<class> class IdxSetFD;

template<class T> 
std::ostream& operator<<(std::ostream& os,const casper::IdxSetFD<T>& f);	

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
struct IdxSetFD
{
	public:
		/// The set element type (same as \p T)
		typedef		T					Value;
		typedef		Hash<T>				Hasher;
	private:
		typedef		IdxSetFD<T>	Self;
 
		struct	Elem 
		{
			typedef enum { in, poss, out } State;
			Elem(ICPSolver& solver, const Value& value, const State& s = poss) : 
				value(value),state(solver,s) 
				{}
			Value				value;
			Reversible<State>	state;
		};
		
	public:	
		/// Type of the data structure holding all elements
		typedef		detail::SList<Elem>			Elems;

		struct PIteration
		{
			typedef typename IdxSetFD::PIterator	Iterator;
			PIteration(IdxSetFD* pOwner) : rOwner(*pOwner) {}
			Iterator begin() const {	return rOwner.beginPoss(); }
			Iterator end() const 	{	return rOwner.endPoss(); }			
			IdxSetFD&	rOwner;
		};
		
		struct IIteration
		{
			typedef typename IdxSetFD::IIterator	Iterator;
			IIteration(IdxSetFD* pOwner) : rOwner(*pOwner) {}
			Iterator begin() const {	return rOwner.beginIn(); }
			Iterator end() const 	{	return rOwner.endIn(); }			
			IdxSetFD&	rOwner;
		};
		
	private:	

		/// Type of the iterator to internal Elems container		
		typedef		typename Elems::Iterator	ElemsIterator;		
		
		/// Type of the hash map
		typedef		detail::HashMap<Value,ElemsIterator>		Index;
		
		// \warning the following works only if Elems is a SList (the general 
		// case has to consider end() != NULL)

		struct ElemFilter 
		{
			typedef	ElemsIterator	UnderIterator;
			typedef typename IdxSetFD<T>::Value	Value;
			static const Value&	dereference(const UnderIterator& it)
			{	assert(it.curNode != NULL);	return it->value;	}
			static Bool valid(const UnderIterator& it) 
			{	return true;	}
		};
		
		template<typename Elem::State s>
		struct SingleStateFilter : ElemFilter 
		{
			SingleStateFilter() {}
			static Bool valid(const typename Elems::Iterator& it)
			{	return it.curNode == NULL or it->state == s;	}
		};

		struct LUBStateFilter : ElemFilter 
		{
			static Bool valid(const typename Elems::Iterator& it)
			{	return it.curNode == NULL or it->state != Elem::out;	}
		};
		
	public:

		/// Iterator for the \p in set
		typedef detail::FilterIterator<SingleStateFilter<Elem::in> >	IIterator;

		/// Iterator for the \p poss set
		typedef detail::FilterIterator<SingleStateFilter<Elem::poss> >	PIterator;

		/// Iterator for the \p glb (in U poss) set
		typedef detail::FilterIterator<LUBStateFilter>					LIterator;
	
		///	Type of the \p card finite domain
		typedef		CurSetFDCard	Card;
		
		/** 
		 *  Creates a IdxSetFD with possible values from the sorted range 
		 *  [\a possBegin ,\a possEnd ).	
		 **/
		template <class InputIterator>
				IdxSetFD(ICPSolver& solver, InputIterator possBegin, 
									  	InputIterator possEnd);

		/** 
		 *  Creates a IdxSetFD with possible values from the sorted range \a rPoss 
		 **/
				IdxSetFD(ICPSolver& solver, const Range<Value>& rPoss); 	
					
		/** 
		 *  Creates a IdxSetFD with possible values from the sorted list \a rPoss 
		 **/
				IdxSetFD(ICPSolver& solver, const detail::List<Value>& rPoss); 	

		/** 
		 *  Creates a IdxSetFD with possible values from the sorted range 
		 *  [\a possBegin ,\a possEnd ), and already containing
		 *  elements in the sorted range [\a inBegin ,\a inEnd ).
		 *  \pre These two sets must be disjoint.	 
		 **/
		template <class InputIterator1,class InputIterator2>
				IdxSetFD(ICPSolver& solver, InputIterator1 inBegin, 
									  	InputIterator1 inEnd,
									  	InputIterator2 possBegin, 
									  	InputIterator2 possEnd);

		/** 
		 *  Creates a IdxSetFD with possible values from the sorted range 
		 *  \a rPoss, and already containing
		 *  elements in the sorted range \a rIn .
		 *  \pre These two sets must be disjoint.	 
		 **/
				IdxSetFD(ICPSolver& solver, const Range<Value>& rIn,
									  	const Range<Value>& rPoss);				 

		/** 
		 *  Creates a IdxSetFD with possible values from the sorted list 
		 *  \a rPoss, and already containing
		 *  elements in the sorted list \a rIn .
		 *  \pre These two sets must be disjoint.	 
		 **/
				IdxSetFD(ICPSolver& solver, const detail::List<Value>& rIn,
									  	const detail::List<Value>& rPoss);				 
				
		///	Copy constructor
				IdxSetFD(const IdxSetFD& s) :
#ifndef CASPER_OLDDELTAS					
					mGLBDeltas(s.mGLBDeltas),
					mLUBDeltas(s.mLUBDeltas),
#endif					
					inSetSize(s.solver(),s.inSize()),
					possSetSize(s.solver(),s.possSize()),
					elems(s.elems),index(s.index),
					cardSet(s.cardSet), 
					groundSL(s.groundSL),glbSL(s.glbSL),lubSL(s.lubSL),domainSL(s.domainSL)
				{}

		/// The destructor.
	 			~IdxSetFD()	{	/* Nothing to do - GC takes care. */}	

		/// Returns the ICPSolver& object associated with this IdxSetFD.
		ICPSolver&	solver() const {	return inSetSize.solver(); }
			
		/// \a True if the set is instantiated. O(1).
		Bool 	ground() const { return possSetSize == 0; }

		/// Returns \p true if this set is instantiated and has no elements. O(1).
		Bool 	empty() const { return ground() and inSetSize == 0; }
		
		///	Size of the \p in set. O(1)
		UInt	inSize() const	{	return inSetSize;	}
		
		///	Size of the \p poss set. O(1)
		UInt	possSize() const	{	return possSetSize;	}
		
		PIteration	poss() {	return PIteration(this);	}
		IIteration	in()  {	return IIteration(this);	}
		
		/** 
		 * 	Direct (rw) access to the \a card set. Direct updates
		 *  on this set are safe.
		 **/ 		
		Card&	card()	{	return cardSet;	}

		/// Direct (ro) access to the \a card set.
		const Card& card() const { return cardSet;	}
		
		///	Returns a PIterator pointing to the beginning of the poss set. O(N)
		PIterator beginPoss() const;

		///	Returns a PIterator pointing to the end of the poss set. O(1)
		PIterator endPoss() const { return elems.end(); }
			
		/// Returns an IIterator pointing to the beginning of the in set. O(N)		
		IIterator beginIn() const;

		/// Returns an IIterator pointing to the end of the in set. O(1)		
		IIterator endIn() const  { return elems.end(); }

		/// Returns an LIterator pointing to the beginning of the (in U poss) set. O(N)		
		LIterator beginLUB() const;

		/// Returns an LIterator pointing to the end of the (in U poss) set. O(1)		
		LIterator endLUB() const  { return elems.end(); }
		
		PIterator findInPoss(const Value& v) const;
		IIterator findInIn(const Value& v) const;
		LIterator findInLUB(const Value& v) const;
		
		Bool clearPoss();	
		Bool erase(const Value& min, const Value& max); 
		Bool erase(const Value& k);
		Bool erase(PIterator p);
		Bool erase(PIterator p, PIterator q);
		template<class InputIterator>
		Bool eraseRange(InputIterator b, InputIterator e);

		Bool insert(const Value& k);
		Bool insert(PIterator p);
		Bool insert(PIterator p, PIterator q);
		template<class InputIterator>
		Bool insertRange(InputIterator b, InputIterator e);
		
		Bool safeErase(const Value& k) {return erase(k);}
		Bool safeInsert(const Value& k) {return insert(k);}

	//	Bool insertBefore(IIterator a, PIterator p);
	//	Bool insertBefore(IIterator a, PIterator p, PIterator q);
		
		/// Sends the contents of the set to stream \p os
		friend std::ostream& operator<< <>(std::ostream& os, const IdxSetFD& f);
		
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
		struct Delta {
			typedef detail::FilterIterator<ElemFilter>	Iterator;
			Delta() {}
			Delta(Iterator it) : b(it) {}
			Iterator b;
			Iterator begin() const { return b;	}
			Iterator end() const { return ++Iterator(b); }
		};


#ifdef CASPER_OLDDELTAS		
		/// Return information regarding the last atomic prunning of the set.
		const Delta&	delta() const	{	return mDelta;	}
#else		
		/// Data structure for storing and retrieving deltas
		typedef detail::DeltaList<Delta>	Deltas;
		
		/// Iterator for retrieving deltas 
		typedef typename Deltas::Iterator	DeltasIterator;

		/// List of inserts in the set since the last choice point
		const Deltas&	glbDeltas() const	{	return mGLBDeltas;	}
		
		/// List of removals from the set since the last choice point
		const Deltas&	lubDeltas() const	{	return mLUBDeltas;	}
#endif	
	private:

		template<class InputIterator>
		Void initializePoss(InputIterator possB, InputIterator possE);
		template<class InputIterator1,class InputIterator2>
		Void initializeInPoss(InputIterator1 inB, InputIterator1 inE,
						 	  InputIterator2 possB, InputIterator2 possE);

		Void onBeforeInsert(ElemsIterator e);
		Void onBeforeErase(ElemsIterator e);

		Bool onAfterErase();
		Bool onAfterInsert();

#ifdef CASPER_OLDDELTAS
		Delta				mDelta;
#else		
		Deltas				mGLBDeltas;
		Deltas				mLUBDeltas;
#endif
		
		Reversible<UInt>	inSetSize;		// for efficiency
		Reversible<UInt>	possSetSize;	// for efficiency
				
		Elems				elems;
		Index				index;
		Card				cardSet;

		EventSuspList			groundSL;
		EventSuspList			glbSL;
		EventSuspList			lubSL;
		EventSuspList			domainSL;		
};

template<class T>
typename IdxSetFD<T>::PIterator IdxSetFD<T>::beginPoss() const 
{
	ElemsIterator it = elems.begin(); 
	while (it != elems.end())
		if (it->state == Elem::poss)
			return PIterator(it);
		else
			++it;
	return PIterator(elems.end());
}

template<class T>		
typename IdxSetFD<T>::IIterator IdxSetFD<T>::beginIn() const
{
	ElemsIterator it = elems.begin();
	while (it != elems.end())
		if (it->state == Elem::in)
			return IIterator(it);
		else
			++it;
	return IIterator(elems.end());
}

template<class T>		
typename IdxSetFD<T>::LIterator IdxSetFD<T>::beginLUB() const
{
	ElemsIterator it = elems.begin();
	while (it != elems.end())
		if (it->state != Elem::out)
			return LIterator(it);
		else
			++it;
	return LIterator(elems.end());
}

template<class T>
typename IdxSetFD<T>::PIterator IdxSetFD<T>::findInPoss(const Value& v) const 
{	
	typename Index::ConstIterator iit = index.find(v);
	if (iit == index.end())
		return elems.end();
	ElemsIterator it = iit->second;	
	if (it->state != Elem::poss)
		return elems.end();
	return it;
}

template<class T>
typename IdxSetFD<T>::IIterator IdxSetFD<T>::findInIn(const Value& v) const 
{
	typename Index::ConstIterator iit = index.find(v);
	if (iit == index.end())
		return elems.end();
	ElemsIterator it = iit->second;	
	if (it->state != Elem::in)
		return elems.end();
	return it;
}

template<class T>
typename IdxSetFD<T>::LIterator IdxSetFD<T>::findInLUB(const Value& v) const 
{
	typename Index::ConstIterator iit = index.find(v);
	if (iit == index.end())
		return elems.end();
	ElemsIterator it = iit->second;	
	if (it->state == Elem::out)
		return elems.end();
	return it;
}


template<class T>
Void IdxSetFD<T>::onBeforeInsert(ElemsIterator it)
{	
#ifdef CASPER_OLDDELTAS
	mDelta.b = it;		
#else
	mGLBDeltas.store(Delta(it));	
#endif	
/*	cninserts++;
	cavginsertsize++;*/
}

template<class T>
Void IdxSetFD<T>::onBeforeErase(ElemsIterator it)
{	
#ifdef CASPER_OLDDELTAS	
	mDelta.b = it;	
#else
	mLUBDeltas.store(Delta(it));	
#endif
/*	cnremoves++;
	cavgremovesize++;*/
}


template<class T>
Bool IdxSetFD<T>::onAfterErase()
{
	//solver().signalDomainUpdate();
	//assert(detail::isStrictOrdered(beginIn(),endIn()));
	//assert(detail::isStrictOrdered(beginPoss(),endPoss()));
	
	if (ground())
		if (!groundSL.notifyAll())
			return false;
	if (!lubSL.notifyAll())
		return false;		
	return domainSL.notifyAll(); 
}

template<class T>
Bool IdxSetFD<T>::onAfterInsert()
{
	//solver().signalDomainUpdate();

	if (ground())
		if (!groundSL.notifyAll())
			return false;
	if (!glbSL.notifyAll())
		return false;		
	return domainSL.notifyAll(); 
}

/** 
	\name Domain prunning methods

	The following methods remove elements from the domain. They also update 
	delta information accordingly and informall registered Notifiables.  

	\return \a False if the operation leads to failure either by making
	the domain inconsistent or by evaluation of a registered Notifiable.
*/
//@{

/**
 * 	Removes all elements from the (poss) set.
 *  \pre possSet is not empty.
 *  O(n)
 **/ 
template<class T>
Bool IdxSetFD<T>::clearPoss()
{	return erase(beginPoss(),endPoss());	}

/**
 * 	Removes all elements in the interval [min,max] (which are on the poss set) 
 *  from the poss set.
 *  O(max-min+1)
 *  \todo Improve when new deltas are ready (cache deltas) 
 **/ 
template<class T>
Bool IdxSetFD<T>::erase(const Value& min, const Value& max)
{	
	assert(0); // Bug - must check if min is in LUB first
	
	assert(min <= max);
		
	ElemsIterator l = index[min];
	while (l != elems.end() and l->state != Elem::poss)
		++l;
	PIterator pb(l);
	while (l != elems.end() and detail::TestRel<T>::lessEqual(l->value,max))
		++l;
	PIterator pe(l);
	return erase(pb,pe);
}

/**
 * 	Removes the element k from the poss set. Fails if the element k is in the
 *  in set, succeeds otherwise.
 *  O(1)
 **/
template<class T> 
Bool IdxSetFD<T>::erase(const Value& k)
{
	typename Index::Iterator iit = index.find(k);
	if (iit == index.end())
		return true;
	ElemsIterator p = iit->second;
	assert(p != elems.end());
	switch (p->state)
	{
		case Elem::out:
			return true;
		case Elem::in:
			return false;
		case Elem::poss:
			onBeforeErase(p);
			p->state = Elem::out;
			--possSetSize;
			return onAfterErase();			
		default:
			assert(0);
			return false;
	}
}

/**
 * 	Removes the element pointed by \a p from the (poss) set.
 *  O(1)
 **/
template<class T>
Bool IdxSetFD<T>::erase(PIterator p)
{
	onBeforeErase(p);
	static_cast<ElemsIterator&>(p)->state = Elem::out;
	--possSetSize;
	return onAfterErase();
}

/**
 * 	Removes the elements in the range [p,q) from the (poss) set.
 *  O(*q-*p)
*  \todo Improve when new deltas are ready (cache deltas) 
 **/
template<class T>
Bool IdxSetFD<T>::erase(PIterator p, PIterator q)
{
	while (p != q)
		if (!erase(p++))
			return false;
	return true;
}


/**
 * 	Removes the element \a k from the poss set and inserts it
 *  in the \p in set. Fails if \a k is not in \p poss and not in \p in. 
 *  O(1)
 **/
template<class T>
Bool IdxSetFD<T>::insert(const Value& k)
{
	typename Index::Iterator iit = index.find(k);
	if (iit == index.end())
		return false;	
	ElemsIterator p = iit->second;
	assert(p != elems.end());	
	switch (p->state)
	{
		case Elem::out:
			return false;
		case Elem::in:
			return true;
		case Elem::poss:
			onBeforeInsert(p);
			p->state = Elem::in;
			++inSetSize;
			--possSetSize;
			return onAfterInsert();			
		default:
			assert(0);
			return false;
	}
}

/**
 * 	Removes the element pointed by \p p from the poss set and inserts it
 *  in the \p in set.
 *  O(1)
 **/
template<class T>
Bool IdxSetFD<T>::insert(PIterator p)
{
	onBeforeInsert(p);
	
	static_cast<ElemsIterator&>(p)->state = Elem::in;

	++inSetSize;
	--possSetSize;
	
	return onAfterInsert();
}

/**
 * 	Removes the elements in the range [p,q) from the poss set and inserts 
 *  them in the \p in set.
 *  O(*q-*p)
 *  \todo Improve when new deltas are ready (cache deltas) 
 **/
template<class T> 
Bool IdxSetFD<T>::insert(PIterator p, PIterator q)
{
	while (p != q)
		if (!insert(p++))
			return false;
	return true;
}


// finds range b..e in d.poss and inserts it in d.in
// \note it is ok if some elements of b..e are already in d.in		
template<class T>
template<class InputIterator>
Bool IdxSetFD<T>::insertRange(InputIterator b, InputIterator e)
{	
	while (b != e)
		if (!insert(*(b++)))
			return false;
	return true;
}

// finds range b..e in d.poss and erases it
// \note it is ok if some elements of b..e are already not in d.poss	
// \note it is not checked if some elements of b..e are in d.in	
template<class T>
template<class InputIterator>
Bool IdxSetFD<T>::eraseRange(InputIterator b, InputIterator e)
{	
	while (b != e)
		if (!erase(*(b++)))
			return false;
	return true;
}

template<class T> struct Set; 

/*
template<class T>
struct DefaultDomType<Set<T>  >
{	typedef IdxSetFD<T>	Type;	};
*/

template<class T>
std::ostream& operator<<(std::ostream& os, const casper::IdxSetFD<T>& f)	
{
	if (f.empty())	// no elements
		os << "{}";
	else
	{	
		os << "{ ";
		
		for (typename IdxSetFD<T>::IIterator it = f.beginIn(); 
				it != f.endIn(); ++it)
			os << *it << " ";
		
		if (!f.ground())
		{	
			os << "| ";
			for (typename IdxSetFD<T>::PIterator it = f.beginPoss(); 
				it != f.endPoss(); ++it)
				os << *it << " ";
			os << "| " << f.card();
		}	
		os << "}";
	}
	return os;
}


namespace detail {

// finds range b..e in d.poss and inserts it in d.in
// \note it is ok if some elements of b..e are already in d.in		
template<class T,class InputIterator>
Bool setInsertRange(IdxSetFD<T>& d, InputIterator b, InputIterator e)
{	
	return d.insertRange(b,e);
	//return casper::detail::copy(b,e,setInserter(d));	
}

// finds range b..e in d.poss and erases it
// \note it is ok if some elements of b..e are already not in d.poss	
// \note it is not checked if some elements of b..e are in d.in
template<class T,class InputIterator>
Bool setEraseRange(IdxSetFD<T>& d, InputIterator b, InputIterator e)
{	
	return d.eraseRange(b,e);
	//return casper::detail::copy(b,e,setEraser(d));	
}

// inserts the element referenced by the iterator
// into the specified domain
// succeeds if element is already in the set  
template<class T>
struct SetInserter<IdxSetFD<T> >
{
	SetInserter(IdxSetFD<T>& d) : d(d) {}
	template<class InputIterator>
	Bool operator()(InputIterator it)
	{
		typename IdxSetFD<T>::PIterator itp = d.findInPoss(*it);
		if (itp == d.endPoss())
			return d.findInIn(*it) != d.endIn();
		return d.insert(itp);
	}
	IdxSetFD<T>& d;
};

// removes the element referenced by the iterator 
// from the specified domain		
template<class T>
struct SetPItEraser<IdxSetFD<T> >
{
	SetPItEraser(IdxSetFD<T>& d) : d(d) {}
	Bool operator()(typename IdxSetFD<T>::PIterator it)
	{	return d.erase(it);	}
	IdxSetFD<T>& d;
};


// finds and removes the element referenced by the iterator 
// from the specified domain 
// (does not fail if element is not in 'poss' neither checks if is in 'in')
template<class T>
struct SetEraser<IdxSetFD<T> >
{
	SetEraser(IdxSetFD<T>& d) : d(d) {}
	template<class InputIterator>
	Bool operator()(InputIterator it)
	{
		typename IdxSetFD<T>::PIterator itp = d.findInPoss(*it);
		if (itp == d.endPoss())
			return true;
		return d.erase(itp);
	}
	IdxSetFD<T>& d;
};
		
}

}

#endif
