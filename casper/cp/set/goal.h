/***************************************************************************
 *   Copyright (C) 2006/2007 by Marco Correia                              *
 *   mvc@di.fct.unl.pt													   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _H_CASPER_SET_GOAL
#define _H_CASPER_SET_GOAL

#include <casper/cp/goal/labeling.h>

namespace Casper {
namespace CP {

/**
 * \defgroup SetGoals Set search
 * Goals specific to set domain variables.
 * \ingroup Set
 * \ingroup Search
 */
/*@{*/
	
namespace Detail {


template<class ElemEval>
struct MinDomEvaluator<Set<ElemEval> >
{
	typedef int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.possSize(); }
};

template<class ElemEval>
struct MaxDomEvaluator<Set<ElemEval> >
{
	typedef int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.possSize(); }
};

template<class ElemEval>
struct MinDomOverDegreeEvaluator<Set<ElemEval> >
{
	typedef double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.possSize()/static_cast<double>(d.getNbSuspFilters()); }
};

template<class ElemEval>
struct MaxDomOverDegreeEvaluator<Set<ElemEval> >
{
	typedef double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.possSize()/static_cast<double>(d.getNbSuspFilters()); }
};

template<class ElemEval>
struct MinDomOverWeightedDegreeEvaluator<Set<ElemEval> >
{
	typedef double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.possSize()/static_cast<double>(d.getAFC()); }
};

template<class ElemEval>
struct MaxDomOverWeightedDegreeEvaluator<Set<ElemEval> >
{
	typedef double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.possSize()/static_cast<double>(d.getAFC()); }
};

}



template<class T,class View>	
struct InsertElem : IGoal
{
	typedef typename DomView<Set<T>,View>::Dom	Dom;	
	typedef typename Dom::PIterator PIterator;
		
	InsertElem(Store& s, const View& v,PIterator it) : IGoal(),store(s),v(s,v),it(it) {}
	
	Goal execute(IExplorer& s)
	{
		assert(v->findInPoss(*it) != v->endPoss());
		if (v->insert(it) and store.valid())
			return succeed();	
		return fail();
	}
	
	Store& store;
	DomView<Set<T>,View> v;
	PIterator it;
};

template<class T,class View>
Goal insertElem(Store& s,const View& v,typename DomView<Set<T>,View>::Dom::PIterator it)
{	return new (s) InsertElem<T,View>(s,v,it);	}

template<class T,class View>	
struct EraseElem : IGoal
{
	typedef typename DomView<Set<T>,View>::Dom	Dom;	
	typedef typename Dom::PIterator PIterator;
		
	EraseElem(Store& s,const View& v,PIterator it) : IGoal(),store(s),v(s,v),it(it) {}
	
	Goal execute(IExplorer& s)
	{
		assert(v->findInPoss(*it) != v->endPoss());
		if (v->erase(it) and store.valid())
			return succeed();
		return fail();
	}
	
	Store& store;
	DomView<Set<T>,View> v;
	PIterator it;
};

template<class T,class View>
Goal eraseElem(Store& s,const View& v,typename DomView<Set<T>,View>::Dom::PIterator it)
{	return new (s) EraseElem<T,View>(s,v,it);	}

namespace Detail {
/**
 *	Goal for selecting a set variable. Attempts to assign a value 
 *  to the variable trying all possible values in ascending order on backtracking.   
 */
template<class T,int I,class D>
struct SelectValsMin<VarArray<Set<T>,I,D> > : IValSelector
{
	SelectValsMin(Store& store,VarArray<Set<T>,I,D> vars) : store(store),vars(vars) {}

	Goal select(uint idx) 
	{
		if (vars(idx).domain().ground())
			return succeed();
		return Goal(store,
				(insertElem<T>(store,vars(idx),vars(idx).domain().beginPoss()) or
				 eraseElem<T>(store,vars(idx),vars(idx).domain().beginPoss()))
			    and callValSelector(store,this,idx));
	}
	Store& store;
	VarArray<Set<T>,I,D> vars;
};

/**
 *	Goal for selecting a set variable. Attempts to assign a value 
 *  to the variable trying all possible values in random order on backtracking.   
 */
template<class T,int I,class D>
struct SelectValsRand<VarArray<Set<T>,I,D> > : IValSelector
{
	SelectValsRand(Store& store,VarArray<Set<T>,I,D> vars) : store(store),vars(vars) {}

	Goal select(uint idx) 
	{
		if (vars(idx).domain().ground())
			return succeed();
		uint r = static_cast<uint>(rand()/(double)RAND_MAX * vars(idx).domain().possSize());
		typename D::PIterator it = vars(idx).domain().beginPoss();
		while (r-- > 0)
			++it;
		return Goal(store,
				(insertElem<T>(store,vars(idx),it) or
			   eraseElem<T>(store,vars(idx),it))
			    and callValSelector(store,this,idx));
	}

	Store& store;
	VarArray<Set<T>,I,D> vars;
};

/**
 *	Goal for bisecting a set variable. Attempts to assign a value 
 *  to the variable trying all possible values in ascending order on backtracking.   
 */
template<class T,int I,class D>
struct SelectValMin<VarArray<Set<T>,I,D> > : IValSelector
{
	SelectValMin(Store& store,VarArray<Set<T>,I,D> vars) : store(store),vars(vars) {}

	Goal select(uint idx) 
	{
		if (vars(idx).domain().ground())
			return succeed();
		return Goal(store,
				insertElem<T>(store,vars(idx),vars(idx).domain().beginPoss()) or
			   eraseElem<T>(store,vars(idx),vars(idx).domain().beginPoss()));
	}
	Store&	store;
	VarArray<Set<T>,I,D> vars;
};
};


	
			
/*@}*/

} // CP
} // Casper


#endif /*_H_CASPER_INT_GOAL*/
