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


///	Integer set variable \ingroup CPSetDataStructures
typedef Casper::CP::Var<Casper::IntSet> 		IntSetVar;
/// Integer set variable array \ingroup CPSetDataStructures
typedef Casper::CP::VarArray<Casper::IntSet>	IntSetVarArray;
/// Integer set variable bidimensional matrix \ingroup CPSetDataStructures
typedef Casper::CP::VarArray<Casper::IntSet,2>	IntSetVarArray2;
/// Integer set variable tridimensional matrix \ingroup CPSetDataStructures
typedef Casper::CP::VarArray<Casper::IntSet,3>	IntSetVarArray3;

///	Boolean set variable \ingroup CPSetDataStructures
typedef Casper::CP::Var<Casper::BoolSet> 		BoolSetVar;
/// Boolean set variable array \ingroup CPSetDataStructures
typedef Casper::CP::VarArray<Casper::BoolSet>	BoolSetVarArray;
/// Boolean set variable bidimensional matrix \ingroup CPSetDataStructures
typedef Casper::CP::VarArray<Casper::BoolSet,2>	BoolSetVarArray2;
/// Boolean set variable tridimensional matrix \ingroup CPSetDataStructures
typedef Casper::CP::VarArray<Casper::BoolSet,3>	BoolSetVarArray3;

} // CP

namespace Traits {
template<class Eval,class Dom>
struct GetElem<CP::Var<Set<Eval>,Dom> >
{	typedef Eval	Type;	};
/*
template<class Eval,class Dom>
struct GetTermElem<CP::Var<Set<Eval>,Dom> >
{	typedef Eval	Type;	};
*/
template<class T>
struct GetSetElem
{	typedef T Type;	};

template<class T>
struct GetSetElem<Set<T> >
{	typedef T Type;	};

} // Traits


} // Casper

#endif

