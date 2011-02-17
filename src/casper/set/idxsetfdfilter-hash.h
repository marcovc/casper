/***************************************************************************
 *   Copyright (C) 2003/2007 by Marco Correia                              *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#ifndef _H_CASPER_SET_IDXSETFDFILTER
#define _H_CASPER_SET_IDXSETFDFILTER

#include <casper/set/idxsetfd.h>
#include <casper/set/util.h>
#include <casper/kernel/filter/common.h>
#include <casper/kernel/container/common.h>

#include <math.h>

namespace casper {

// cardinal(2.1)
template<class T>
struct IdxSetFDCard1 : IFilter
{
	IdxSetFDCard1(IdxSetFD<T>& d) : 
		IFilter(d.solver()),
		d(d) {}
	
	Bool execute()
	{	
		//Debug::log(this,"Entering execute");
		return d.card().updateMin(d.inSize());
	}
	
	bool entailed() const
	{	return d.card().ground();	}
	
	void attach(INotifiable* s) 
	{	d.attachOnGLB(s);	}
	void detach(INotifiable* s) 
	{	d.detachOnGLB(s);	}
	
	IdxSetFD<T>& d;
};

// cardinal(2.2)
template<class T>
struct IdxSetFDCard2 : IFilter
{
	IdxSetFDCard2(IdxSetFD<T>& d) : 
		IFilter(d.solver()),
		d(d) {}
	
	Bool execute()
	{	
		//Debug::log(this,"Entering execute");
		return d.card().updateMax(d.inSize()+d.possSize());
	}
	
	bool entailed() const
	{	return d.ground() and d.card().ground();	}
	
	void attach(INotifiable* s) 
	{	d.attachOnLUB(s);	}
	void detach(INotifiable* s) 
	{	d.detachOnLUB(s);	}
	
	IdxSetFD<T>& d;
};

// cardinal(3)
template<class T>
struct IdxSetFDCard3 : IFilter
{
	IdxSetFDCard3(IdxSetFD<T>& d) : 
		IFilter(d.solver()),
		d(d) {}
	
	Bool execute()
	{	
		//Debug::log(this,"Entering execute");

		if (!d.card().ground())
			return true;
	
		if (((UInt)d.card().value()) == d.inSize())
			return d.ground() or d.clearPoss();

		if (((UInt)d.card().value()) == d.inSize()+d.possSize())
			return d.insert(d.beginPoss(),d.endPoss());

		return true;		
	}
	
	bool entailed() const
	{	return d.card().ground();	}
	
	void attach(INotifiable* s) 
	{	
		d.card().attachOnGround(s);
		d.attachOnGLB(s);
		d.attachOnLUB(s);	
	}
	void detach(INotifiable* s) 
	{	
		d.card().detachOnGround(s);	
		d.detachOnGLB(s);
		d.detachOnLUB(s);	
	}
	
	IdxSetFD<T>& d;
};


template<class T>
template<class InputIterator>
Void IdxSetFD<T>::initializePoss(InputIterator possB, InputIterator possE)
{
	assert(detail::isStrictOrdered(possB,possE));

	detail::QuickSList<T> rPoss(detail::count(possB,possE));
	
	detail::copy(possB,possE,detail::SListInserter<T>(rPoss));
	for (typename detail::QuickSList<T>::Iterator rPossIt = rPoss.begin();
			rPossIt != rPoss.end(); ++rPossIt)
	{
		elems.pushFront(Elem(solver(),*rPossIt));
		index.insert(*rPossIt,elems.begin());
	}
}

template<class T>
template<class InputIterator1,class InputIterator2>
Void IdxSetFD<T>::initializeInPoss(InputIterator1 inB, InputIterator1 inE,
								   InputIterator2 possB, InputIterator2 possE)
{
	assert(detail::isStrictOrdered(inB,inE));
	assert(detail::isStrictOrdered(possB,possE));

	detail::QuickSList<Elem> reversed(detail::count(inB,inE)+
									  detail::count(possB,possE));
	
	while ( inB != inE or possB != possE )
	{
		if ( possB == possE )
		{	
			reversed.pushFront(Elem(solver(),*inB,Elem::in));
			++inB;
		}
		else
		if ( inB == inE )	
		{
			reversed.pushFront(Elem(solver(),*possB,Elem::poss));
			++possB;
		}
		else
		if (detail::TestRel<T>::less(*inB,*possB))
		{	
			reversed.pushFront(Elem(solver(),*inB,Elem::in));
			++inB;
		}
		else
		{
			assert(detail::TestRel<T>::greater(*inB,*possB));
			reversed.pushFront(Elem(solver(),*possB,Elem::poss));
			++possB;
		}
			
	}	
	
	for (typename detail::QuickSList<Elem>::Iterator it = reversed.begin(); 
			it != reversed.end(); ++it)
	{	
		elems.pushFront(*it);
		index.insert(it->value,elems.begin());
	}
	
	ElemsIterator it = elems.begin();
	if (it != elems.end())
		for (ElemsIterator itn = (it++); it != elems.end(); ++itn,++it)
			assert(detail::TestRel<T>::greater(it->value,itn->value));
}


// IdxSetFD constructors

template<class T>
IdxSetFD<T>::IdxSetFD(ICPSolver& solver, const Range<Value>& rPoss) :
#ifndef CASPER_OLDDELTAS	
					mGLBDeltas(solver),
					mLUBDeltas(solver),
#endif					
					inSetSize(solver,0),
					possSetSize(solver,rPoss.max()-rPoss.min()+1),				
					elems(solver.heap()),
					index(solver.heap(),(UInt)ceil(CASPER_IDXSETFD_HASHRATIO*possSetSize)),
					cardSet(solver,0,possSetSize),
					groundSL(solver),
					glbSL(solver),
					lubSL(solver),
					domainSL(solver) 
{
	// initialize index and elems
	initializePoss(rPoss.begin(),rPoss.end());
	
	// post internal constraints
	solver.post(Filter(new (solver.heap()) IdxSetFDCard1<T>(*this)));
	solver.post(Filter(new (solver.heap()) IdxSetFDCard2<T>(*this)));
	solver.post(Filter(new (solver.heap()) IdxSetFDCard3<T>(*this)));
}	

template<class T>
IdxSetFD<T>::IdxSetFD(ICPSolver& solver, const Range<Value>& rIn,
							      const Range<Value>& rPoss)  : 
#ifndef CASPER_OLDDELTAS							    	  
					mGLBDeltas(solver),
					mLUBDeltas(solver),
#endif					
					inSetSize(solver,rIn.max()-rIn.min()+1),
					possSetSize(solver,rPoss.max()-rPoss.min()+1),
					elems(solver.heap()),
					index(solver.heap(),(UInt)ceil(CASPER_IDXSETFD_HASHRATIO*(possSetSize+inSetSize))),
					cardSet(solver,(Int)inSetSize,(Int)(inSetSize+possSetSize)),
					groundSL(solver),
					glbSL(solver),
					lubSL(solver),
					domainSL(solver)
{
	// initialize index and elems
	initializeInPoss(rIn.begin(),rIn.end(),rPoss.begin(),rPoss.end());

	// post internal constraints
	solver.post(Filter(new (solver.heap()) IdxSetFDCard1<T>(*this)));
	solver.post(Filter(new (solver.heap()) IdxSetFDCard2<T>(*this)));
	solver.post(Filter(new (solver.heap()) IdxSetFDCard3<T>(*this)));
}		
			
template<class T>
IdxSetFD<T>::IdxSetFD(ICPSolver& solver, const detail::List<Value>& rPoss) :
#ifndef CASPER_OLDDELTAS	
					mGLBDeltas(solver),
					mLUBDeltas(solver),
#endif					
					inSetSize(solver,0),
					possSetSize(solver,rPoss.size()),
					elems(solver.heap()),
					index(solver.heap(),(UInt)ceil(CASPER_IDXSETFD_HASHRATIO*possSetSize)),
					cardSet(solver,0,possSetSize),
					groundSL(solver),
					glbSL(solver),
					lubSL(solver),
					domainSL(solver) 
{
	// initialize index and elems
	initializePoss(rPoss.begin(),rPoss.end());
	
	// post internal constraints
	solver.post(Filter(new (solver.heap()) IdxSetFDCard1<T>(*this)));
	solver.post(Filter(new (solver.heap()) IdxSetFDCard2<T>(*this)));
	solver.post(Filter(new (solver.heap()) IdxSetFDCard3<T>(*this)));
}	

template<class T>
IdxSetFD<T>::IdxSetFD(ICPSolver& solver, const detail::List<Value>& rIn,
							   const detail::List<Value>& rPoss)  : 
#ifndef CASPER_OLDDELTAS								   
					mGLBDeltas(solver),
					mLUBDeltas(solver),
#endif					
					inSetSize(solver,rIn.size()),
					possSetSize(solver,rPoss.size()),
					elems(solver.heap()),
					index(solver.heap(),(UInt)ceil(CASPER_IDXSETFD_HASHRATIO*(possSetSize+inSetSize))),
					cardSet(solver,(Int)inSetSize,(Int)(inSetSize+possSetSize)),
					groundSL(solver),
					glbSL(solver),
					lubSL(solver),
					domainSL(solver)
{
	// initialize index and elems
	initializeInPoss(rIn.begin(),rIn.end(),rPoss.begin(),rPoss.end());
	
	// post internal constraints
	solver.post(Filter(new (solver.heap()) IdxSetFDCard1<T>(*this)));
	solver.post(Filter(new (solver.heap()) IdxSetFDCard2<T>(*this)));
	solver.post(Filter(new (solver.heap()) IdxSetFDCard3<T>(*this)));
}		

template<class T>
template <class InputIterator>
IdxSetFD<T>::IdxSetFD(ICPSolver& solver, InputIterator possBegin, 
							   InputIterator possEnd) : 
#ifndef CASPER_OLDDELTAS								   
					mGLBDeltas(solver),
					mLUBDeltas(solver),
#endif					
					inSetSize(solver,0),
					possSetSize(solver,detail::count(possBegin,possEnd)),
					elems(solver.heap()),
					index(solver.heap(),(UInt)ceil(CASPER_IDXSETFD_HASHRATIO*possSetSize)),
					cardSet(solver,0,possSetSize),
					groundSL(solver),
					glbSL(solver),
					lubSL(solver),
					domainSL(solver) 
{
	// initialize index and elems
	initializePoss(possBegin,possEnd);
	
	// post internal constraints
	solver.post(Filter(new (solver.heap()) IdxSetFDCard1<T>(*this)));
	solver.post(Filter(new (solver.heap()) IdxSetFDCard2<T>(*this)));
	solver.post(Filter(new (solver.heap()) IdxSetFDCard3<T>(*this)));
}

template<class T>
template <class InputIterator1,class InputIterator2>
IdxSetFD<T>::IdxSetFD(ICPSolver& solver, InputIterator1 inBegin, 
							   InputIterator1 inEnd,
							   InputIterator2 possBegin, 
							   InputIterator2 possEnd) : 
#ifndef CASPER_OLDDELTAS								   
					mGLBDeltas(solver),
					mLUBDeltas(solver),
#endif					
					inSetSize(solver,detail::count(inBegin,inEnd)),
					possSetSize(solver,detail::count(possBegin,possEnd)),
					elems(solver.heap()),
					index(solver.heap(),(UInt)ceil(CASPER_IDXSETFD_HASHRATIO*(possSetSize+inSetSize))),
					cardSet(solver,(Int)inSetSize,(Int)(inSetSize+possSetSize)),
					groundSL(solver),
					glbSL(solver),
					lubSL(solver),
					domainSL(solver) 
{
	// initialize index and elems
	initializeInPoss(inBegin,inEnd,possBegin,possEnd);
	
	// post internal constraints
	solver.post(Filter(new (solver.heap()) IdxSetFDCard1<T>(*this)));
	solver.post(Filter(new (solver.heap()) IdxSetFDCard2<T>(*this)));
	solver.post(Filter(new (solver.heap()) IdxSetFDCard3<T>(*this)));
}

};

#endif /*_H_CASPER_SET_IDXSETFDFILTER*/
