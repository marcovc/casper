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
#include <casper/kernel/variable.h>
#include <casper/set/setfd.h>
#include <casper/set/idxsetfd.h>
#include <casper/kernel/view/arrayview.h>

#include <iostream>

namespace casper {

template<class T>
struct Set 
{	typedef T	Elem;	};

///	Set of integers. \ingroup Set
typedef Set<Int>	IntSet;

///	Set of booleans. \ingroup Set
typedef Set<Bool>	BoolSet;

template<class T>
struct DefaultDomType< Set<T>  >
{
	#ifdef CASPER_DEFAULTSET_IDXSETFD
		typedef IdxSetFD<T>	Type;
	#else	
		typedef SetFD<T>	Type;
	#endif	
};

namespace detail {

template<class T>
struct VarDomCreator< SetFD<T> >
{
	template<class InputIterator>
	SetFD<T>* operator()(ICPSolver& solver, InputIterator b, InputIterator e)
	{ 
		SetFD<T>* ret = new (solver.heap()) SetFD<T>(solver,b,e);
		solver.signalNewDomain(ret);
		return ret;	
	}

	template<class InputIterator1,class InputIterator2>
	SetFD<T>* operator()(ICPSolver& solver, 
						InputIterator1 ib, InputIterator1 ie,
						InputIterator2 pb, InputIterator2 pe)
	{ 
		SetFD<T>* ret = new (solver.heap()) SetFD<T>(solver,ib,ie,pb,pe);
		solver.signalNewDomain(ret);
		return ret;	
	}
	
	SetFD<T>* operator()(ICPSolver& solver, const Range<T>& rPoss)
	{	
		SetFD<T>* ret = new (solver.heap()) SetFD<T>(solver,rPoss);
		solver.signalNewDomain(ret);
		return ret;	
	}
	
	SetFD<T>* operator()(ICPSolver& solver, const Range<T>& rIn,
										const Range<T>& rPoss)
	{	
		SetFD<T>* ret = new (solver.heap()) SetFD<T>(solver,rIn,rPoss);
		solver.signalNewDomain(ret);
		return ret;	
	}
	
	SetFD<T>* operator()(ICPSolver& solver, const List<T>& rPoss)
	{	
		SetFD<T>* ret = new (solver.heap()) SetFD<T>(solver,rPoss);
		solver.signalNewDomain(ret);
		return ret;	
	}
	
	SetFD<T>* operator()(ICPSolver& solver, const List<T>& rIn,
										const List<T>& rPoss)
	{	
		SetFD<T>* ret = new (solver.heap()) SetFD<T>(solver,rIn,rPoss);
		solver.signalNewDomain(ret);
		return ret;	
	}
		
	// returned domain is the union of all domains in the array
	template<class View>
	SetFD<T>* unionOf(ICPSolver& solver, const View& v)
	{ 
		DomArrayView<Set<T>,View> doms(solver,v);
		detail::RSUList<T> u(solver);
		for (UInt i = 0; i < doms.size(); i++)
			u.insert(doms[i]->beginPoss(),doms[i]->endPoss());
		return operator()(solver,u.begin(),u.end());	
	}
};

#ifdef CASPER_DEFAULTSET_IDXSETFD

template<class T>
struct VarDomCreator< IdxSetFD<T> >
{
	template<class InputIterator>
	IdxSetFD<T>* operator()(ICPSolver& solver, InputIterator b, InputIterator e)
	{ 
		IdxSetFD<T>* ret = new (solver.heap()) IdxSetFD<T>(solver,b,e);
		solver.signalNewDomain(ret);
		return ret;	
	}

	template<class InputIterator1,class InputIterator2>
	IdxSetFD<T>* operator()(ICPSolver& solver, 
						InputIterator1 ib, InputIterator1 ie,
						InputIterator2 pb, InputIterator2 pe)
	{ 
		IdxSetFD<T>* ret = new (solver.heap()) IdxSetFD<T>(solver,ib,ie,pb,pe);
		solver.signalNewDomain(ret);
		return ret;	
	}
	
	IdxSetFD<T>* operator()(ICPSolver& solver, const Range<T>& rPoss)
	{	
		IdxSetFD<T>* ret = new (solver.heap()) IdxSetFD<T>(solver,rPoss);
		solver.signalNewDomain(ret);
		return ret;	
	}
	
	IdxSetFD<T>* operator()(ICPSolver& solver, const Range<T>& rIn,
										const Range<T>& rPoss)
	{	
		IdxSetFD<T>* ret = new (solver.heap()) IdxSetFD<T>(solver,rIn,rPoss);
		solver.signalNewDomain(ret);
		return ret;	
	}
	
	IdxSetFD<T>* operator()(ICPSolver& solver, const List<T>& rPoss)
	{	
		IdxSetFD<T>* ret = new (solver.heap()) IdxSetFD<T>(solver,rPoss);
		solver.signalNewDomain(ret);
		return ret;	
	}
	
	IdxSetFD<T>* operator()(ICPSolver& solver, const List<T>& rIn,
										const List<T>& rPoss)
	{	
		IdxSetFD<T>* ret = new (solver.heap()) IdxSetFD<T>(solver,rIn,rPoss);
		solver.signalNewDomain(ret);
		return ret;	
	}
		
	// returned domain is the union of all domains in the array
	template<class View>
	IdxSetFD<T>* unionOf(ICPSolver& solver, const View& v)
	{ 
		DomArrayView<Set<T>,View> doms(solver,v);
		detail::RSUList<T> u(solver);
		for (UInt i = 0; i < doms.size(); i++)
			u.insert(doms[i]->beginPoss(),doms[i]->endPoss());
		return operator()(solver,u.begin(),u.end());	
	}
};
#endif

};

template<class Eval,class Dom>
struct ElemTraits<DomVar<Set<Eval>,Dom> >
{	typedef Eval	Elem;	};

};

#endif

