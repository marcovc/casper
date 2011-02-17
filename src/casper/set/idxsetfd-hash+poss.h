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


namespace detail {

template<class T>
struct IdxSetFDHelper 
{
	typedef T					Value;
	typedef const Value*		ConstPointer;
	typedef const Value&		ConstReference;
	
	//private:
	typedef size_t		Difference;
	
	struct Node
	{
		typedef Node*	PNode;
		typedef enum { in = 1, poss = 2, out = 4, 
					   lub = 3, end = 7 } State;
		Node(ICPSolver& solver,State state,UInt idx,const Value& v) :
			nextPoss(solver),prevPoss(solver),value(v),
			state(solver,state),loIdx(idx) {}
		Reversible<PNode>	nextPoss;
		Reversible<PNode>	prevPoss;
		const 	Value		value;
		Reversible<State>	state;
		const	UInt		loIdx;
	};

	typedef Node*	PNode;
	typedef typename Node::State	NodeState;
	
	typedef	Vector<PNode>			LO;
	typedef HashMap<Value,PNode>	Index;

	LO		lo;
	Index	index;
	Reversible<PNode>				headPoss;
	Reversible<PNode>				tailPoss;
	Reversible<Int>					inMask;
	Reversible<Int>					lubMask;
	
	typedef	typename LO::Iterator	LOIterator;
	
	public:
		
	class PIterator;
	friend class PIterator;
	class PIterator : public std::iterator<bidirectional_iterator_tag,
									 	   Value,
									 	   Difference>
	{
		friend struct IdxSetFDHelper;
		public:
		PIterator() : curNode(NULL) {}
		PIterator(PNode n) : curNode(n) {}
		PIterator(const PIterator& s) : curNode(s.curNode) {}
		bool operator==(const PIterator& s) const 
		{ return curNode == s.curNode; }
		bool operator!=(const PIterator& s) const
		{ return curNode != s.curNode; }
		ConstReference operator*() const
		{ return ((PNode)curNode)->value; }
		ConstPointer operator->() const
		{ return &((PNode)curNode)->value; }
		PIterator& operator++() // pre-increment
		{ curNode = curNode->nextPoss; return *this; }
		PIterator operator++(int) //post-increment
		{ PIterator ret(*this); curNode = curNode->nextPoss; return ret; }
		const PIterator& operator=(const PIterator& i)
		{ curNode = i.curNode;	return *this; }
		PIterator& operator--() // pre-decrement
		{ curNode = curNode->prevPoss; return *this; }
		PIterator operator--(int) //post-decrement
		{ PIterator ret(*this); curNode = curNode->prevPoss; return ret; }
		PNode	get()	{	return curNode;	}
		private:
		PNode	curNode;
	};

	struct FilterIterator : LOIterator
	{
		typedef LOIterator	Super;		
		FilterIterator(const Int& state,PNode* pnode) : 
			Super(pnode),state(state) {}
		FilterIterator(const Int& state,const typename Super::Iterator& s) : 
			Super(s),state(state) {}
		FilterIterator& operator++()  // pre-increment
		{
			do {
				++static_cast<Super&>(*this);
			} while ( (Super::operator*()->state & state) == 0);
			return *this;
		}
		FilterIterator operator++(int) //post-increment
		{	FilterIterator ret(*this); ++*this; return ret;	}
		ConstReference operator*() const
		{ return Super::operator*()->value; }
		ConstPointer operator->() const
		{ return &Super::operator*()->value; }
		PNode	get() {	return *Super::pCurElem;	}
		private:	// disabled
		FilterIterator& operator--(); // pre-decrement
		FilterIterator operator--(int); //post-decrement
		const	Int	state;
	};
	
	typedef FilterIterator	IIterator;
	typedef FilterIterator	LIterator;
	
	template<class InputIterator>
	IdxSetFDHelper(ICPSolver&,UInt,InputIterator possBegin,InputIterator possEnd);
	template<class InputIterator1, class InputIterator2>
	IdxSetFDHelper(ICPSolver&,UInt,InputIterator1 inBegin,InputIterator1 inEnd,
				   UInt,InputIterator2 possBegin,InputIterator2 possEnd);

	PIterator beginPoss() const {	return headPoss.get();	}
	PIterator endPoss() const	{	return tailPoss.get();	}
	IIterator beginIn() const;
	IIterator endIn() const		
	{	
		assert(!lo.empty()); 
		return IIterator(inMask.get(),const_cast<PNode*>(&lo[lo.size()-1]));	
	}

	LIterator beginLUB() const;
	LIterator endLUB() const
	{	
		assert(!lo.empty()); 
		return LIterator(lubMask.get(),const_cast<PNode*>(&lo[lo.size()-1]));	
	}
	
	PIterator findInPoss(const Value& v) const;
	IIterator findInIn(const Value& v) const;
	LIterator findInLUB(const Value& v) const;
	
	Bool isInIn(LIterator lit)  
	{	return (lit.get()->state & inMask.get()) != 0; }
	Bool isInPoss(LIterator lit)  
	{	return (lit.get()->state & inMask.get()) == 0;	}
	
	static PIterator toPossIt(LIterator lit)
	{	return lit.get();	}
	
	typedef LOIterator	Iterator;

	UInt erasePoss(PIterator p, PIterator q);
	Void erasePoss(PIterator p);
	Void erasePoss();

	UInt insertPoss(PIterator p, PIterator q);
	Void insertPoss(PIterator p);
	
	//private:
	LOIterator	beginLO() const {	return lo.begin();	}
	LOIterator  endLO() const	
	{	assert(!lo.empty()); return const_cast<PNode*>(&lo[lo.size()-1]);	}
	Void erase(PIterator p, PIterator q);
	Void erase(PIterator p);
	Void erase()	{	headPoss = tailPoss;	}
	Void setStateOut(PIterator pit)	{	pit.curNode->state = Node::out;	}
	Void setStateIn(PIterator pit)	{	pit.curNode->state = Node::in;	}
	ICPSolver& solver() const	{	return headPoss.solver();	}
};

template<class T>
typename IdxSetFDHelper<T>::IIterator 
IdxSetFDHelper<T>::beginIn() const
{
	LOIterator it = beginLO();
	while (it != endLO())
		if (((*it)->state & inMask.get()) != 0)
			return IIterator(inMask.get(),it);
		else
			++it;
	return IIterator(inMask.get(),it);	
}

template<class T>
typename IdxSetFDHelper<T>::LIterator 
IdxSetFDHelper<T>::beginLUB() const
{
	LOIterator it = beginLO();
	while (it != endLO())
		if (((*it)->state & lubMask) != 0)
			return LIterator(lubMask,it);
		else
			++it;
	return LIterator(lubMask,it);	
}

template<class T>
template<class InputIterator>
IdxSetFDHelper<T>::IdxSetFDHelper(ICPSolver& solver,
								  UInt size,
								  InputIterator possB,
								  InputIterator possE) :
								  lo(solver.heap(),size+1),
								  index(solver.heap(),(UInt)ceil(CASPER_IDXSETFD_HASHRATIO*size)),
								  headPoss(solver),
								  tailPoss(solver),
								  inMask(solver,Node::in),
								  lubMask(solver,Node::in | Node::poss)
{
	assert(isStrictOrdered(possB,possE));
	UInt i = 0; 
	
	// create tail
	lo.initialize(size,new (solver.heap()) Node(solver,Node::end,size,*possE));
	
	// first pass: fill lo and index
	while (possB != possE)
	{
		PNode n = new (solver.heap()) Node(solver,Node::poss,i,*possB);
		lo.initialize(i,n);
		index.insert(*possB,n);
		++i;
		++possB;
	}
	// second pass: create poss links
	for (i = 1; i < size; i++)
	{
		lo[i]->prevPoss = lo[i-1];
		lo[i]->nextPoss = lo[i+1];
	}
	
	if (size > 1)
	{
		lo[0]->nextPoss = lo[1];
		lo[size]->prevPoss = lo[size-1];
		headPoss = lo[0];
	}
	else if (size == 1)
	{
		lo[0]->nextPoss = lo[size];
		lo[size]->prevPoss = lo[0];
		headPoss = lo[0];
	}
	else
	{
		headPoss = lo[size];
	}
	
	// set head and tail
	//lo[0]->nextPoss = lo[1];
	//lo[size]->prevPoss = lo[size-1];
	//headPoss = lo[0];
	tailPoss = lo[size];
}

template<class T>
template<class InputIterator1, class InputIterator2>
IdxSetFDHelper<T>::IdxSetFDHelper(ICPSolver& solver,
								  UInt inSize,
								  InputIterator1 inB,
								  InputIterator1 inE,
								  UInt possSize,
								  InputIterator2 possB,
								  InputIterator2 possE) :
								  lo(solver.heap(), inSize + possSize + 1),
								  index(solver.heap(),(UInt)ceil(CASPER_IDXSETFD_HASHRATIO*(inSize + possSize))),
								  headPoss(solver),
								  tailPoss(solver),
								  inMask(solver,Node::in),
								  lubMask(solver,Node::in | Node::poss)
{
	assert(isStrictOrdered(inB,inE));
	assert(isStrictOrdered(possB,possE));

	// create tail
	lo.initialize(inSize + possSize, new (solver.heap()) Node(solver, Node::end, inSize + possSize, T()));
	
	InputIterator1 inIt = inB;
	InputIterator2 possIt = possB;
	
	UInt i = 0;
	std::vector<UInt> pos;
	
	for ( ; inIt != inE or possIt != possE; )
	{
		PNode n;
		
		if (inIt == inE)
		{
			n = new (solver.heap()) Node(solver, Node::poss, i, *possIt);
			pos.push_back(i);
			++possIt;
		}
		else if (possIt == possE)
		{
			n = new (solver.heap()) Node(solver, Node::in, i, *inIt);
			++inIt;
		}
		else if (detail::TestRel<T>::less(*inIt,*possIt))
		{
			n = new (solver.heap()) Node(solver, Node::in, i, *inIt);
			++inIt;
		}
		else
		{
			assert(detail::TestRel<T>::greater(*inIt,*possIt));
			n = new (solver.heap()) Node(solver, Node::poss, i, *possIt);
			pos.push_back(i);
			++possIt;
		}
		
		index.insert(n->value,n);
		lo.initialize(i,n);
		i++;
	}
	
	// poss second pass: create poss links
	for (Int j = 1; j < ((Int)pos.size()) - 1; j++)
	{
		lo[pos[j]]->prevPoss = lo[pos[j-1]];
		lo[pos[j]]->nextPoss = lo[pos[j+1]];
	}
	
	if (pos.size() > 1)
	{
		lo[pos[0]]->nextPoss = lo[pos[1]];
		lo[pos[pos.size() - 1]]->prevPoss = lo[pos[pos.size() - 2]];
		lo[pos[pos.size() - 1]]->nextPoss = lo[inSize + possSize];
		lo[inSize + possSize]->prevPoss = lo[pos[pos.size() - 1]];
		headPoss = lo[pos[0]];
	}
	else if (pos.size() == 1)
	{
		lo[pos[0]]->nextPoss = lo[inSize + possSize];
		lo[inSize + possSize]->prevPoss = lo[pos[0]];
		headPoss = lo[pos[0]];
	}
	else
	{
		headPoss = lo[inSize + possSize];
	}

	tailPoss = lo[inSize + possSize];
}

// return number of elements removed
template<class T>
Void IdxSetFDHelper<T>::erase(PIterator p, PIterator q)
{	
	//assert(isStrictOrdered(beginLUB(),endLUB()));
	if (((PNode)headPoss) != p.curNode)
		p.curNode->prevPoss.get()->nextPoss = q.curNode;
	else
		headPoss = q.curNode;
	q.curNode->prevPoss = p.curNode->prevPoss;
	//assert(isStrictOrdered(beginLUB(),endLUB()));
}

template<class T>
Void IdxSetFDHelper<T>::erase(PIterator it)
{
	assert(it != endPoss());
	//assert(isStrictOrdered(beginLUB(),endLUB()));
	if (((PNode)headPoss) != it.curNode)
		it.curNode->prevPoss->nextPoss = it.curNode->nextPoss;
	else
		headPoss = it.curNode->nextPoss;
	it.curNode->nextPoss->prevPoss = it.curNode->prevPoss;
	//assert(isStrictOrdered(beginLUB(),endLUB()));
}

// return number of elements removed
template<class T>
UInt IdxSetFDHelper<T>::erasePoss(PIterator p, PIterator q)
{	
	//assert(isStrictOrdered(beginLUB(),endLUB()));
	if (p == beginPoss() and q == endPoss())
	{
		lubMask = lubMask.get() & ~Node::poss;
		UInt ret = 0;
		for (PIterator it = p; it != q; ++it)
			++ret;
		erase(p,q);
		return ret;
	}
	else
	{	
		UInt ret = 0;
		for (PIterator it = p; it != q; ++it)
		{
			it.curNode->state = Node::out;
			++ret;
		}
		erase(p,q);
		//assert(isStrictOrdered(beginLUB(),endLUB()));
		return ret;
	}	
}

template<class T>
Void IdxSetFDHelper<T>::erasePoss(PIterator it)
{
	//assert(isStrictOrdered(beginLUB(),endLUB()));
	it.curNode->state = Node::out;
	erase(it);
	//assert(isStrictOrdered(beginLUB(),endLUB()));
}

template<class T>
Void IdxSetFDHelper<T>::erasePoss()
{
	//assert(isStrictOrdered(beginLUB(),endLUB()));
	lubMask = lubMask.get() & ~Node::poss;
	//for (PIterator it = beginPoss(); it != endPoss(); ++it)
	//	it.curNode->state = Node::out;
	erase();
	//assert(isStrictOrdered(beginLUB(),endLUB()));
}

// return number of elements inserted
template<class T>
UInt IdxSetFDHelper<T>::insertPoss(PIterator p, PIterator q)
{	
/*	std::cout << this << " : insertPoss " << solver().stats().fails() << std::endl;
	std::copy(p,q,std::ostream_iterator<Int>(std::cout," "));
	std::cout << std::endl;
	std::copy(beginPoss(),endPoss(),std::ostream_iterator<Int>(std::cout," "));
	std::cout << std::endl;
	std::copy(beginIn(),endIn(),std::ostream_iterator<Int>(std::cout," "));
	std::cout << std::endl;
	std::copy(beginLUB(),endLUB(),std::ostream_iterator<Int>(std::cout," "));
	std::cout << std::endl;
*/	
	//assert(isStrictOrdered(beginLUB(),endLUB()));
	if (p == beginPoss() and q == endPoss())
	{
		inMask = inMask.get() | Node::poss;
		UInt ret = 0;
		for (PIterator it = p; it != q; ++it)
			++ret;
		erase(p,q);
		return ret;
	}
	else
	{	
		UInt ret = 0;
		for (PIterator it = p; it != q; ++it)
		{
			it.curNode->state = Node::in;
			++ret;
		}
		erase(p,q);
		//assert(isStrictOrdered(beginLUB(),endLUB()));
		return ret;
	}	
}

template<class T>
Void IdxSetFDHelper<T>::insertPoss(PIterator it)
{
	//assert(isStrictOrdered(beginLUB(),endLUB()));
	it.curNode->state = Node::in;
	erase(it);
	//assert(isStrictOrdered(beginLUB(),endLUB()));
}

template<class T>
typename IdxSetFDHelper<T>::PIterator 
IdxSetFDHelper<T>::findInPoss(const Value& v) const
{
	assert(inMask == Node::in and
		   lubMask != Node::in);

	typename Index::ConstIterator iit = index.find(v);
	if (iit == index.end())
		return endPoss();
	PNode p = iit->second;	
	if (p->state != Node::poss)
		return endPoss();
	return p;	
}

template<class T>
typename IdxSetFDHelper<T>::IIterator 
IdxSetFDHelper<T>::findInIn(const Value& v) const
{
	typename Index::ConstIterator iit = index.find(v);
	if (iit == index.end())
		return endIn();
	PNode p = iit->second;	
	if ((p->state & inMask) == 0)
		return endIn();
	return IIterator(inMask,const_cast<PNode*>(&lo[p->loIdx]));		
}

template<class T>
typename IdxSetFDHelper<T>::LIterator 
IdxSetFDHelper<T>::findInLUB(const Value& v) const
{
	typename Index::ConstIterator iit = index.find(v);
	if (iit == index.end())
		return endLUB();
	PNode p = iit->second;	
	if ((p->state & lubMask) == 0)
		return endLUB();
	return LIterator(lubMask,const_cast<PNode*>(&lo[p->loIdx]));		
}
}

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
		typedef		IdxSetFD<T>			Self;
		typedef		detail::IdxSetFDHelper<T>	Helper;
		
	public:

		/// Iterator for the \p in set
		typedef typename Helper::IIterator		IIterator;

		/// Iterator for the \p poss set
		typedef typename Helper::PIterator		PIterator;

		/// Iterator for the \p lub (in U poss) set
		typedef typename Helper::LIterator		LIterator;

		///	Type of the \p card finite domain
		typedef			CurSetFDCard	Card;
		
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
					helper(s.solver(),s.inSize(),s.beginIn(),s.endIn(),
							s.possSize(),s.beginPoss(),s.endPoss()),
					cardSet(s.cardSet), 
					groundSL(s.groundSL),glbSL(s.glbSL),
					lubSL(s.lubSL),domainSL(s.domainSL)
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
		
		/** 
		 * 	Direct (rw) access to the \a card set. Direct updates
		 *  on this set are safe.
		 **/ 		
		Card&	card()	{	return cardSet;	}

		/// Direct (ro) access to the \a card set.
		const Card& card() const { return cardSet;	}
		
		///	Returns a PIterator pointing to the begining of the poss set. O(1)
		PIterator beginPoss() const { return helper.beginPoss(); }

		///	Returns a PIterator pointing to the end of the poss set. O(1)
		PIterator endPoss() const { return helper.endPoss(); }
			
		/// Returns an IIterator pointing to the begining of the in set. O(N)		
		IIterator beginIn() const { return helper.beginIn(); }

		/// Returns an IIterator pointing to the end of the in set. O(1)		
		IIterator endIn() const  { return helper.endIn(); }

		/// Returns an LIterator pointing to the begining of the lub (in U poss) set. O(N)		
		LIterator beginLUB() const { return helper.beginLUB(); }

		/// Returns an LIterator pointing to the end of the lub (in U poss) set. O(1)		
		LIterator endLUB() const  { return helper.endLUB(); }
		
		PIterator findInPoss(const Value& v) const 
		{	
			if (possSize() == 0)
				return endPoss();
			else
				return helper.findInPoss(v);	
		}
		IIterator findInIn(const Value& v) const
		{	return helper.findInIn(v);	}
		LIterator findInLUB(const Value& v) const
		{	return helper.findInLUB(v);	}
		
		Bool isInIn(LIterator lit)  
		{	return helper.isInIn(lit);	}
		Bool isInPoss(LIterator lit)  
		{	return helper.isInPoss(lit);}
		
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

		Bool clearPoss();
		
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
			typedef PIterator	Iterator;
			Delta() {}
			Delta(Iterator b,Iterator e) : b(b),e(e) {}
			Delta(Iterator it) : b(it),e(++it) {}
			Iterator b;
			Iterator e;
			Iterator begin() const { return b;	}
			Iterator end() const { return e; }
		};

		/// Data structure for storing and retrieving deltas
		typedef detail::DeltaList<Delta>	Deltas;
		
		/// Iterator for retrieving deltas 
		typedef typename Deltas::Iterator	DeltasIterator;

#ifdef CASPER_OLDDELTAS		
		/// Return information regarding the last atomic prunning of the set.
		const Delta&	delta() const	{	return mDelta;	}
#else		
		/// List of inserts in the set since the last choice point
		const Deltas&	glbDeltas() const	{	return mGLBDeltas;	}
		
		/// List of removals from the set since the last choice point
		const Deltas&	lubDeltas() const	{	return mLUBDeltas;	}
#endif	
	private:

		template<class InputIterator>
		UInt count(InputIterator b, InputIterator e);

		Void onBeforeInsert(PIterator b,PIterator e);
		Void onBeforeInsert(PIterator e);
		Void onBeforeErase(PIterator b,PIterator e);
		Void onBeforeErase(PIterator e);

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
				
		Helper				helper;
		Card				cardSet;

		EventSuspList			groundSL;
		EventSuspList			glbSL;
		EventSuspList			lubSL;
		EventSuspList			domainSL;		
};

template<class T>
template<class InputIterator>
UInt IdxSetFD<T>::count(InputIterator b, InputIterator e)
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
Void IdxSetFD<T>::onBeforeInsert(PIterator it)
{	
#ifdef CASPER_OLDDELTAS
	mDelta.b = it;	mDelta.e = ++it;	
#else
	mGLBDeltas.store(Delta(it));	
#endif	
/*	cninserts++;
	cavginsertsize++;
	cavgposssize+=possSize();*/
}

template<class T>
Void IdxSetFD<T>::onBeforeInsert(PIterator b,PIterator e)
{	
#ifdef CASPER_OLDDELTAS
	mDelta.b = b;	mDelta.e = e;	
#else	
	mGLBDeltas.store(Delta(b,e));
#endif	
	/*cninserts++;
	for (PIterator aux = b; aux != e; ++aux)
		++cavginsertsize;
	cavgposssize+=possSize();*/
}

template<class T>
Void IdxSetFD<T>::onBeforeErase(PIterator it)
{	
#ifdef CASPER_OLDDELTAS	
	mDelta.b = it;	mDelta.e = ++it;	
#else
	mLUBDeltas.store(Delta(it));	
#endif
	/*cnremoves++;
	cavgremovesize++;
	cavgposssize+=possSize();*/
}

template<class T>
Void IdxSetFD<T>::onBeforeErase(PIterator b,PIterator e)
{	
#ifdef CASPER_OLDDELTAS	
	mDelta.b = b;	mDelta.e = e;	
#else	
	mLUBDeltas.store(Delta(b,e));	
#endif
	/*cnremoves++;
	for (PIterator aux = b; aux != e; ++aux)
		++cavgremovesize;
	cavgposssize+=possSize();*/
}

template<class T>
Bool IdxSetFD<T>::onAfterErase()
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
 *  O(1)
 **/ 
template<class T>
Bool IdxSetFD<T>::clearPoss()
{
	onBeforeErase(beginPoss(),endPoss());
	helper.erasePoss();
	possSetSize = 0;
	return onAfterErase();	
}

/**
 * 	Removes the element k from the poss set. Fails if the element k is in the
 *  in set, succeeds otherwise.
 *  O(1)
 **/
template<class T> 
Bool IdxSetFD<T>::erase(const Value& k)
{
	LIterator lit = findInLUB(k);
	if (lit == endLUB())
		return true;
	if (helper.isInIn(lit))
		return false;
	PIterator pit = Helper::toPossIt(lit); 
	onBeforeErase(pit);
	--possSetSize;
	helper.erasePoss(pit);
	return onAfterErase();
}

/**
 * 	Removes the element pointed by \a p from the (poss) set.
 *  O(1)
 **/
template<class T>
Bool IdxSetFD<T>::erase(PIterator p)
{
	onBeforeErase(p);
	--possSetSize;
	helper.erasePoss(p);
	return onAfterErase();
}

/**
 * 	Removes the elements in the range [p,q) from the (poss) set.
 *  O(*q-*p) 
 **/
template<class T>
Bool IdxSetFD<T>::erase(PIterator p, PIterator q)
{
	onBeforeErase(p,q);
	possSetSize -= helper.erasePoss(p,q);
	return onAfterErase();
}


/**
 * 	Removes the element \a k from the poss set and inserts it
 *  in the \p in set. Fails if \a k is not in \p poss and not in \p in. 
 *  O(1)
 **/
template<class T>
Bool IdxSetFD<T>::insert(const Value& k)
{
	LIterator lit = findInLUB(k);
	if (lit == endLUB())
		return false;
	if (helper.isInIn(lit))
		return true;
	PIterator pit = Helper::toPossIt(lit); 
	onBeforeInsert(pit);
	--possSetSize;
	++inSetSize;
	helper.insertPoss(pit);
	return onAfterInsert();
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
	++inSetSize;
	--possSetSize;
	helper.insertPoss(p);
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
	onBeforeInsert(p,q);
	UInt ninserts = helper.insertPoss(p,q);
	possSetSize -= ninserts;
	inSetSize += ninserts;
	return onAfterInsert();
}

// safe
template<class T>
template<class InputIterator>
Bool IdxSetFD<T>::insertRange(InputIterator b, InputIterator e)
{	
	while (b != e)
	{	
		LIterator lit = findInLUB(*b);
		if (lit == endLUB())
			return false;
		if (helper.isInPoss(lit))
		{
			UInt nremoves = 0;
			PIterator itb = helper.toPossIt(lit);
			PIterator ite = itb;
			while (b != e and ite != endPoss() and
				   detail::TestRel<T>::lessEqual(*b,*ite))
			{	
				if (not detail::TestRel<T>::less(*b,*ite))
				{	
					helper.setStateIn(ite);
					++ite;
					++nremoves;
				}
				else
					if (findInIn(*b) == endIn())
						return false;
				++b;
			}
			onBeforeInsert(itb,ite);
			possSetSize -= nremoves;
			inSetSize += nremoves;
			helper.erase(itb,ite);
			if (!onAfterInsert())
				return false;
		}
		else
			++b;
	}
	return true;
}

// safe
template<class T>
template<class InputIterator>
Bool IdxSetFD<T>::eraseRange(InputIterator b, InputIterator e)
{	
	while (b != e)
	{	
		LIterator lit = findInLUB(*b);
		
		if (lit == endLUB())
			++b;
		else
		if (isInPoss(lit))
		{
			UInt nremoves = 0;
			PIterator itb = helper.toPossIt(lit);
			PIterator ite = itb;
			while (b != e and ite != endPoss() and
				   detail::TestRel<T>::lessEqual(*b,*ite))
			{	
				if (not detail::TestRel<T>::less(*b,*ite))
				{	
					helper.setStateOut(ite);
					++ite;
					++nremoves;
				}
				else
				if (findInIn(*b) != endIn())
					return false;
				
				++b;
			}
			onBeforeErase(itb,ite);
			possSetSize -= nremoves;	
			helper.erase(itb,ite);
			if (!onAfterErase())
				return false;	
		}
		else
			return false;
	}
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
{	return d.insertRange(b,e);	}

// finds range b..e in d.poss and erases it
// \note it is ok if some elements of b..e are already not in d.poss	
// \note it is not checked if some elements of b..e are in d.in
template<class T,class InputIterator>
Bool setEraseRange(IdxSetFD<T>& d, InputIterator b, InputIterator e)
{	return d.eraseRange(b,e);	}

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
