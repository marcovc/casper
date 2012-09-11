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
		d(d),pOwner(NULL) {}
	
	struct Demon: INotifiable
	{
		Demon(SetFDCard1& rOwner) : rOwner(rOwner) {}
		bool notify()
		{
			return ((uint)rOwner.d.card().min())>=rOwner.d.inSize() or
					rOwner.pOwner->notify();
		}
		SetFDCard1& rOwner;
	};

	bool execute()
	{	
		#ifdef CASPER_LOG_OLD
		d.getStore().getEnv().log(this, "SetFDCard1", Util::Logger::filterExecuteBegin);
		#endif

		return d.card().updateMin(d.inSize());	
	}
	
	void attach(INotifiable* s) 
	{
		assert(pOwner==NULL or pOwner==s);
		pOwner = s;
		pDemon = new (d.getState()) Demon(*this);
		d.attachOnGLB(pDemon);
	}
	void detach()
	{	d.detachOnGLB(pDemon);	}
	
	SetFD<T>& d;
	INotifiable* pOwner;
	Demon*	pDemon;
};

// cardinal(2.2)
template<class T>
struct SetFDCard2 : IFilter
{
	SetFDCard2(SetFD<T>& d) : 
		IFilter(d.getStore()),
		d(d),pOwner(NULL) {}
	
	struct Demon: INotifiable
	{
		Demon(SetFDCard2& rOwner) : rOwner(rOwner) {}
		bool notify()
		{
			return ((uint)rOwner.d.card().max())<=rOwner.d.inSize()+rOwner.d.possSize() or
					rOwner.pOwner->notify();
		}
		SetFDCard2& rOwner;
	};

	bool execute()
	{	
		#ifdef CASPER_LOG_OLD
		d.getStore().getEnv().log(this, "SetFDCard2", Util::Logger::filterExecuteBegin);
		#endif
		return d.card().updateMax(d.inSize()+d.possSize());	
	}
	
	void attach(INotifiable* s) 
	{
		assert(pOwner==NULL or pOwner==s);
		pOwner = s;
		pDemon = new (d.getState()) Demon(*this);
		d.attachOnLUB(pDemon);
	}
	void detach()
	{	d.detachOnLUB(pDemon);	}
	
	SetFD<T>& d;
	INotifiable* pOwner;
	Demon*	pDemon;
};

// cardinal(3)
template<class T>
struct SetFDCard3 : IFilter
{
	SetFDCard3(SetFD<T>& d) : 
		IFilter(d.getStore()),
		d(d),pOwner(NULL)  {}
	
	struct Demon: INotifiable
	{
		Demon(SetFDCard3& rOwner) : rOwner(rOwner) {}
		bool notify()
		{
			return !rOwner.d.card().ground() or
					rOwner.d.poss().empty() or
					((((uint)rOwner.d.card().value()) != rOwner.d.inSize()) and
					(((uint)rOwner.d.card().value()) != rOwner.d.inSize()+rOwner.d.possSize())) or
					rOwner.pOwner->notify();
		}
		SetFDCard3& rOwner;
	};

	bool execute()
	{	
		#ifdef CASPER_LOG_OLD
		d.getStore().getEnv().log(this, "SetFDCard3", Util::Logger::filterExecuteBegin);
		#endif
		
		assert(d.card().ground());
		if (((uint)d.card().value()) == d.inSize())
			return d.poss().empty() or d.clearPoss();
		if (((uint)d.card().value()) == d.inSize()+d.possSize())	
			return d.insert(d.beginPoss(),d.endPoss());
		return true;		
	}
	
	void attach(INotifiable* s) 
	{	
		assert(pOwner==NULL or pOwner==s);
		pOwner = s;
		pDemon = new (d.getState()) Demon(*this);
		d.card().attachOnGround(pDemon);
		d.attachOnDomain(pDemon);
	}
	void detach()
	{	
		d.card().detachOnGround(pDemon);
		d.detachOnDomain(pDemon);
	}
	
	SetFD<T>& d;
	INotifiable*	pOwner;
	Demon*	pDemon;
};

// SetFD constructors

template<class T>
SetFD<T>::SetFD(Store& store, const Util::StdRange<Elem>& rPoss) :
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
	store.filterSched().post(Filter(new (store) SetFDCard1<T>(*this)),false);
	store.filterSched().post(Filter(new (store) SetFDCard2<T>(*this)),false);
	store.filterSched().post(Filter(new (store) SetFDCard3<T>(*this)),false);
}	

template<class T>
SetFD<T>::SetFD(Store& store, const Util::StdRange<Elem>& rIn,
							   const Util::StdRange<Elem>& rPoss)  :
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
	store.filterSched().post(Filter(new (store) SetFDCard1<T>(*this)),false);
	store.filterSched().post(Filter(new (store) SetFDCard2<T>(*this)),false);
	store.filterSched().post(Filter(new (store) SetFDCard3<T>(*this)),false);
}		
			
template<class T>
SetFD<T>::SetFD(Store& store, const Util::StdList<Elem>& rPoss) :
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
	store.filterSched().post(Filter(new (store) SetFDCard1<T>(*this)),false);
	store.filterSched().post(Filter(new (store) SetFDCard2<T>(*this)),false);
	store.filterSched().post(Filter(new (store) SetFDCard3<T>(*this)),false);
}	

template<class T>
SetFD<T>::SetFD(Store& store, const Util::StdList<Elem>& rIn,
							   const Util::StdList<Elem>& rPoss)  :
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
	store.filterSched().post(Filter(new (store) SetFDCard1<T>(*this)),false);
	store.filterSched().post(Filter(new (store) SetFDCard2<T>(*this)),false);
	store.filterSched().post(Filter(new (store) SetFDCard3<T>(*this)),false);
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
	if (!Casper::Util::Detail::isStrictOrdered(possBegin,possEnd))
		throw Casper::Exception::InvalidOperation("attempt to create a set variable from a non strictly ordered range");

	// post internal constraints
	store.filterSched().post(Filter(new (store) SetFDCard1<T>(*this)),false);
	store.filterSched().post(Filter(new (store) SetFDCard2<T>(*this)),false);
	store.filterSched().post(Filter(new (store) SetFDCard3<T>(*this)),false);
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
	if (!Casper::Util::Detail::isStrictOrdered(inBegin,inEnd))
		throw Casper::Exception::InvalidOperation("attempt to create a set variable from a non strictly ordered range");
	if (!Casper::Util::Detail::isStrictOrdered(possBegin,possEnd))
		throw Casper::Exception::InvalidOperation("attempt to create a set variable from a non strictly ordered range");

	// post internal constraints
	store.filterSched().post(Filter(new (store) SetFDCard1<T>(*this)),false);
	store.filterSched().post(Filter(new (store) SetFDCard2<T>(*this)),false);
	store.filterSched().post(Filter(new (store) SetFDCard3<T>(*this)),false);
}

} // CP
} // Casper

#endif /*_H_CASPER_SET_SETFDFILTER*/
