/***************************************************************************
 *   Copyright (C) 2003/2007 by Marco Correia                              *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#ifndef _H_CASPER_SET_SETFDFILTER
#define _H_CASPER_SET_SETFDFILTER

#include <casper/set/setfd.h>
#include <casper/kernel/filter/common.h>
#include <casper/kernel/container/common.h>

namespace casper {

// cardinal(2.1)
template<class T>
struct SetFDCard1 : IFilter
{
	SetFDCard1(SetFD<T>& d) : 
		IFilter(d.solver()),
		d(d) {}
	
	Bool execute()
	{	
		//Debug::log( this,"Entering execute");
		return d.card().updateMin(d.inSize());	
	}
	
	bool entailed() const
	{	return d.card().ground();	}
	
	void attach(INotifiable* s) 
	{	d.attachOnGLB(s);	}
	void detach(INotifiable* s) 
	{	d.detachOnGLB(s);	}
	
	SetFD<T>& d;
};

// cardinal(2.2)
template<class T>
struct SetFDCard2 : IFilter
{
	SetFDCard2(SetFD<T>& d) : 
		IFilter(d.solver()),
		d(d) {}
	
	Bool execute()
	{	
		//Debug::log( this,"Entering execute");
		return d.card().updateMax(d.inSize()+d.possSize());	
	}
	
	bool entailed() const
	{	return d.ground() and d.card().ground();	}
	
	void attach(INotifiable* s) 
	{	d.attachOnLUB(s);	}
	void detach(INotifiable* s) 
	{	d.detachOnLUB(s);	}
	
	SetFD<T>& d;
};

// cardinal(3)
template<class T>
struct SetFDCard3 : IFilter
{
	SetFDCard3(SetFD<T>& d) : 
		IFilter(d.solver()),
		d(d) {}
	
	Bool execute()
	{	
		//Debug::log( this,"Entering execute");
		
		if (!d.card().ground())
			return true;
	
		if (((UInt)d.card().value()) == d.inSize())
			return d.poss().empty() or d.clearPoss();
		if (((UInt)d.card().value()) == d.inSize()+d.possSize())	
			return d.insert(d.beginPoss(),d.endPoss());
		return true;		
	}
	
	bool entailed() const
	{	return d.card().ground();	}
	
	void attach(INotifiable* s) 
	{	
		d.card().attachOnGround(s);
		d.attachOnDomain(s);
	}
	void detach(INotifiable* s) 
	{	
		d.card().detachOnGround(s);	
		d.detachOnDomain(s);
	}
	
	SetFD<T>& d;
};

// SetFD constructors

template<class T>
SetFD<T>::SetFD(ICPSolver& solver, const Range<Value>& rPoss) :
					mGLBDeltas(solver),
					mLUBDeltas(solver),
					inSetSize(solver,0),
					possSetSize(solver,rPoss.max()-rPoss.min()+1),				
					inSet(solver),
					possSet(solver,rPoss.begin(),rPoss.end()),
					cardSet(solver,0,possSetSize),
					groundSL(solver),
					glbSL(solver),
					lubSL(solver),
					domainSL(solver) 
{
	// post internal constraints
	solver.post(Filter(new (solver.heap()) SetFDCard1<T>(*this)));
	solver.post(Filter(new (solver.heap()) SetFDCard2<T>(*this)));
	solver.post(Filter(new (solver.heap()) SetFDCard3<T>(*this)));
}	

template<class T>
SetFD<T>::SetFD(ICPSolver& solver, const Range<Value>& rIn,
							   const Range<Value>& rPoss)  : 
					mGLBDeltas(solver),
					mLUBDeltas(solver),
					inSetSize(solver,rIn.max()-rIn.min()+1),
					possSetSize(solver,rPoss.max()-rPoss.min()+1),
					inSet(solver,rIn.begin(),rIn.end()),
					possSet(solver,rPoss.begin(),rPoss.end()),
					cardSet(solver,(Int)inSetSize,(Int)(inSetSize+possSetSize)),
					groundSL(solver),
					glbSL(solver),
					lubSL(solver),
					domainSL(solver)
{
	// post internal constraints
	solver.post(Filter(new (solver.heap()) SetFDCard1<T>(*this)));
	solver.post(Filter(new (solver.heap()) SetFDCard2<T>(*this)));
	solver.post(Filter(new (solver.heap()) SetFDCard3<T>(*this)));
}		
			
template<class T>
SetFD<T>::SetFD(ICPSolver& solver, const detail::List<Value>& rPoss) :
					mGLBDeltas(solver),
					mLUBDeltas(solver),
					inSetSize(solver,0),
					possSetSize(solver,rPoss.size()),				
					inSet(solver),
					possSet(solver,rPoss.begin(),rPoss.end()),
					cardSet(solver,0,possSetSize),
					groundSL(solver),
					glbSL(solver),
					lubSL(solver),
					domainSL(solver) 
{
	// post internal constraints
	solver.post(Filter(new (solver.heap()) SetFDCard1<T>(*this)));
	solver.post(Filter(new (solver.heap()) SetFDCard2<T>(*this)));
	solver.post(Filter(new (solver.heap()) SetFDCard3<T>(*this)));
}	

template<class T>
SetFD<T>::SetFD(ICPSolver& solver, const detail::List<Value>& rIn,
							   const detail::List<Value>& rPoss)  : 
					mGLBDeltas(solver),
					mLUBDeltas(solver),
					inSetSize(solver,rIn.size()),
					possSetSize(solver,rPoss.size()),
					inSet(solver,rIn.begin(),rIn.end()),
					possSet(solver,rPoss.begin(),rPoss.end()),
					cardSet(solver,(Int)inSetSize,(Int)(inSetSize+possSetSize)),
					groundSL(solver),
					glbSL(solver),
					lubSL(solver),
					domainSL(solver)
{
	// post internal constraints
	solver.post(Filter(new (solver.heap()) SetFDCard1<T>(*this)));
	solver.post(Filter(new (solver.heap()) SetFDCard2<T>(*this)));
	solver.post(Filter(new (solver.heap()) SetFDCard3<T>(*this)));
}		

template<class T>
template <class InputIterator>
SetFD<T>::SetFD(ICPSolver& solver, InputIterator possBegin, 
							   InputIterator possEnd) : 
					mGLBDeltas(solver),
					mLUBDeltas(solver),
					inSetSize(solver,0),
					possSetSize(solver,count(possBegin,possEnd)),				
					inSet(solver),
					possSet(solver,possBegin,possEnd),
					cardSet(solver,0,possSetSize),
					groundSL(solver),
					glbSL(solver),
					lubSL(solver),
					domainSL(solver) 
{
	assert(detail::isStrictOrdered(possBegin,possEnd));
	// post internal constraints
	solver.post(Filter(new (solver.heap()) SetFDCard1<T>(*this)));
	solver.post(Filter(new (solver.heap()) SetFDCard2<T>(*this)));
	solver.post(Filter(new (solver.heap()) SetFDCard3<T>(*this)));
}

template<class T>
template <class InputIterator1,class InputIterator2>
SetFD<T>::SetFD(ICPSolver& solver, InputIterator1 inBegin, 
							   InputIterator1 inEnd,
							   InputIterator2 possBegin, 
							   InputIterator2 possEnd) : 
					mGLBDeltas(solver),
					mLUBDeltas(solver),
					inSetSize(solver,count(inBegin,inEnd)),
					possSetSize(solver,count(possBegin,possEnd)),
					inSet(solver,inBegin,inEnd),
					possSet(solver,possBegin,possEnd),
					cardSet(solver,(Int)inSetSize,(Int)(inSetSize+possSetSize)),
					groundSL(solver),
					glbSL(solver),
					lubSL(solver),
					domainSL(solver) 
{
	assert(detail::isStrictOrdered(inBegin,inEnd));	
	assert(detail::isStrictOrdered(possBegin,possEnd));
	// post internal constraints
	solver.post(Filter(new (solver.heap()) SetFDCard1<T>(*this)));
	solver.post(Filter(new (solver.heap()) SetFDCard2<T>(*this)));
	solver.post(Filter(new (solver.heap()) SetFDCard3<T>(*this)));
}

};

#endif /*_H_CASPER_SET_SETFDFILTER*/
