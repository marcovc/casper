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

#include <casper/cp/set/setfd.h>
#include <casper/cp/filter/common.h>

namespace Casper {
namespace CP {


// cardinal(2.1)
template<class T>
struct SetFDCard1 : IFilter
{
	SetFDCard1(SetFD<T>& d) : 
		IFilter(d.getStore()),
		d(d) {}
	
	bool execute()
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
		IFilter(d.getStore()),
		d(d) {}
	
	bool execute()
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
		IFilter(d.getStore()),
		d(d) {}
	
	bool execute()
	{	
		//Debug::log( this,"Entering execute");
		
		if (!d.card().ground())
			return true;
	
		if (((uint)d.card().value()) == d.inSize())
			return d.poss().empty() or d.clearPoss();
		if (((uint)d.card().value()) == d.inSize()+d.possSize())	
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
SetFD<T>::SetFD(Store& store, const Util::StdRange<Value>& rPoss) :
					store(store),
					mGLBDeltas(store),
					mLUBDeltas(store),
					inSetSize(store,0),
					possSetSize(store,rPoss.max()-rPoss.min()+1),				
					inSet(store),
					possSet(store,rPoss.begin(),rPoss.end()),
					cardSet(store,0,possSetSize),
					groundSL(store),
					glbSL(store),
					lubSL(store),
					domainSL(store) 
{
	// post internal constraints
	store.post(Filter(new (store) SetFDCard1<T>(*this)));
	store.post(Filter(new (store) SetFDCard2<T>(*this)));
	store.post(Filter(new (store) SetFDCard3<T>(*this)));
}	

template<class T>
SetFD<T>::SetFD(Store& store, const Util::StdRange<Value>& rIn,
							   const Util::StdRange<Value>& rPoss)  :
					store(store),
					mGLBDeltas(store),
					mLUBDeltas(store),
					inSetSize(store,rIn.max()-rIn.min()+1),
					possSetSize(store,rPoss.max()-rPoss.min()+1),
					inSet(store,rIn.begin(),rIn.end()),
					possSet(store,rPoss.begin(),rPoss.end()),
					cardSet(store,(int)inSetSize,(int)(inSetSize+possSetSize)),
					groundSL(store),
					glbSL(store),
					lubSL(store),
					domainSL(store)
{
	// post internal constraints
	store.post(Filter(new (store) SetFDCard1<T>(*this)));
	store.post(Filter(new (store) SetFDCard2<T>(*this)));
	store.post(Filter(new (store) SetFDCard3<T>(*this)));
}		
			
template<class T>
SetFD<T>::SetFD(Store& store, const Util::StdList<Value>& rPoss) :
					store(store),
					mGLBDeltas(store),
					mLUBDeltas(store),
					inSetSize(store,0),
					possSetSize(store,rPoss.size()),				
					inSet(store),
					possSet(store,rPoss.begin(),rPoss.end()),
					cardSet(store,0,possSetSize),
					groundSL(store),
					glbSL(store),
					lubSL(store),
					domainSL(store) 
{
	// post internal constraints
	store.post(Filter(new (store) SetFDCard1<T>(*this)));
	store.post(Filter(new (store) SetFDCard2<T>(*this)));
	store.post(Filter(new (store) SetFDCard3<T>(*this)));
}	

template<class T>
SetFD<T>::SetFD(Store& store, const Util::StdList<Value>& rIn,
							   const Util::StdList<Value>& rPoss)  : 
					store(store),
					mGLBDeltas(store),
					mLUBDeltas(store),
					inSetSize(store,rIn.size()),
					possSetSize(store,rPoss.size()),
					inSet(store,rIn.begin(),rIn.end()),
					possSet(store,rPoss.begin(),rPoss.end()),
					cardSet(store,(int)inSetSize,(int)(inSetSize+possSetSize)),
					groundSL(store),
					glbSL(store),
					lubSL(store),
					domainSL(store)
{
	// post internal constraints
	store.post(Filter(new (store) SetFDCard1<T>(*this)));
	store.post(Filter(new (store) SetFDCard2<T>(*this)));
	store.post(Filter(new (store) SetFDCard3<T>(*this)));
}		

template<class T>
template <class InputIterator>
SetFD<T>::SetFD(Store& store, InputIterator possBegin, 
							   InputIterator possEnd) : 
					store(store),
					mGLBDeltas(store),
					mLUBDeltas(store),
					inSetSize(store,0),
					possSetSize(store,count(possBegin,possEnd)),				
					inSet(store),
					possSet(store,possBegin,possEnd),
					cardSet(store,0,possSetSize),
					groundSL(store),
					glbSL(store),
					lubSL(store),
					domainSL(store) 
{
	assert(Casper::Util::Detail::isStrictOrdered(possBegin,possEnd));
	// post internal constraints
	store.post(Filter(new (store) SetFDCard1<T>(*this)));
	store.post(Filter(new (store) SetFDCard2<T>(*this)));
	store.post(Filter(new (store) SetFDCard3<T>(*this)));
}

template<class T>
template <class InputIterator1,class InputIterator2>
SetFD<T>::SetFD(Store& store, InputIterator1 inBegin, 
							   InputIterator1 inEnd,
							   InputIterator2 possBegin, 
							   InputIterator2 possEnd) : 
					store(store),
					mGLBDeltas(store),
					mLUBDeltas(store),
					inSetSize(store,count(inBegin,inEnd)),
					possSetSize(store,count(possBegin,possEnd)),
					inSet(store,inBegin,inEnd),
					possSet(store,possBegin,possEnd),
					cardSet(store,(int)inSetSize,(int)(inSetSize+possSetSize)),
					groundSL(store),
					glbSL(store),
					lubSL(store),
					domainSL(store) 
{
	assert(Casper::Util::Detail::isStrictOrdered(inBegin,inEnd));
	assert(Casper::Util::Detail::isStrictOrdered(possBegin,possEnd));
	// post internal constraints
	store.post(Filter(new (store) SetFDCard1<T>(*this)));
	store.post(Filter(new (store) SetFDCard2<T>(*this)));
	store.post(Filter(new (store) SetFDCard3<T>(*this)));
}

} // CP
} // Casper

#endif /*_H_CASPER_SET_SETFDFILTER*/