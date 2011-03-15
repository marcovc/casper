/***************************************************************************
 *   Copyright (C) 2007 by Marco Correia                              	   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _H_CASPER_SET_VARIABLE
#define _H_CASPER_SET_VARIABLE

#include <casper/kernel/common.h>
#include <casper/cp/var.h>
#include <casper/cp/set/setfd.h>
#include <casper/cp/view/array.h>

#include <iostream>

namespace Casper {
namespace CP {

namespace Detail {

template<class T>
struct VarDomCreator< SetFD<T> >
{
	template<class InputIterator>
	SetFD<T>* operator()(Store& store, InputIterator b, InputIterator e)
	{ 
		SetFD<T>* ret = new (store) SetFD<T>(store,b,e);
		store.getStats().signalNewSetDomain();
		return ret;	
	}

	template<class InputIterator1,class InputIterator2>
	SetFD<T>* operator()(Store& store, 
						InputIterator1 ib, InputIterator1 ie,
						InputIterator2 pb, InputIterator2 pe)
	{ 
		SetFD<T>* ret = new (store) SetFD<T>(store,ib,ie,pb,pe);
		store.getStats().signalNewSetDomain();
		return ret;	
	}
	
	SetFD<T>* operator()(Store& store, const Util::StdRange<T>& rPoss)
	{	
		SetFD<T>* ret = new (store) SetFD<T>(store,rPoss);
		store.getStats().signalNewSetDomain();
		return ret;	
	}
	
	SetFD<T>* operator()(Store& store, const Util::StdRange<T>& rIn,
										const Util::StdRange<T>& rPoss)
	{	
		SetFD<T>* ret = new (store) SetFD<T>(store,rIn,rPoss);
		store.getStats().signalNewSetDomain();
		return ret;	
	}
	
	SetFD<T>* operator()(Store& store, const Util::StdList<T>& rPoss)
	{	
		SetFD<T>* ret = new (store) SetFD<T>(store,rPoss);
		store.getStats().signalNewSetDomain();
		return ret;	
	}
	
	SetFD<T>* operator()(Store& store, const Util::StdList<T>& rIn,
										const Util::StdList<T>& rPoss)
	{	
		SetFD<T>* ret = new (store) SetFD<T>(store,rIn,rPoss);
		store.getStats().signalNewSetDomain();
		return ret;	
	}
		
	// returned domain is the union of all domains in the array
	template<class View>
	SetFD<T>* unionOf(Store& store, const View& v)
	{ 
		DomArrayView<Set<T>,View> doms(store,v);
		SUList<T> u(store);
		for (uint i = 0; i < doms.size(); i++)
			u.insert(doms[i]->beginPoss(),doms[i]->endPoss());
		return operator()(store,u.begin(),u.end());	
	}
};


} // Detail


typedef Var<IntSet> 		IntSetVar;
typedef VarArray<IntSet>	IntSetVarArray;
typedef VarArray<IntSet,2>	IntSetVarArray2;
typedef VarArray<IntSet,3>	IntSetVarArray3;

} // CP

namespace Traits {
template<class Eval,class Dom>
struct GetElem<CP::Var<Set<Eval>,Dom> >
{	typedef Eval	Type;	};
} // Traits


} // Casper

#endif

