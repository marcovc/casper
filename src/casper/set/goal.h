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

#include <casper/kernel/goal/labeling.h>

namespace casper {

/**
 * \defgroup SetGoals Set search
 * Goals specific to set domain variables.
 * \ingroup Set
 * \ingroup Search
 */
/*@{*/
	
namespace detail {


template<class ElemEval>
struct MinDomEvaluator<Set<ElemEval> >
{
	typedef Int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.possSize(); }
};

template<class ElemEval>
struct MaxDomEvaluator<Set<ElemEval> >
{
	typedef Int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.possSize(); }
};

template<class ElemEval>
struct MinDomOverDegreeEvaluator<Set<ElemEval> >
{
	typedef Double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.possSize()/static_cast<Double>(d.getNbSuspFilters()); }
};

template<class ElemEval>
struct MaxDomOverDegreeEvaluator<Set<ElemEval> >
{
	typedef Double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.possSize()/static_cast<Double>(d.getNbSuspFilters()); }
};

template<class ElemEval>
struct MinDomOverWeightedDegreeEvaluator<Set<ElemEval> >
{
	typedef Double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.possSize()/static_cast<Double>(d.getAFC()); }
};

template<class ElemEval>
struct MaxDomOverWeightedDegreeEvaluator<Set<ElemEval> >
{
	typedef Double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.possSize()/static_cast<Double>(d.getAFC()); }
};

}



template<class T,class View>	
struct InsertElem : IGoal
{
	typedef typename DomView<Set<T>,View>::Dom	Dom;	
	typedef typename Dom::PIterator PIterator;
		
	InsertElem(ICPSolver& s, const View& v,PIterator it) : IGoal(s),v(s,v),it(it) {}
	
	Goal execute(IGoalSched& s)
	{
		assert(v->findInPoss(*it) != v->endPoss());
		if (v->insert(it))
			return ok();	
		return fail();
	}
	
	DomView<Set<T>,View> v;
	PIterator it;
};

template<class T,class View>
Goal insertElem(const View& v,typename DomView<Set<T>,View>::Dom::PIterator it)
{	return new (getCPSolver(v).heap()) InsertElem<T,View>(getCPSolver(v),v,it);	}

template<class T,class View>	
struct EraseElem : IGoal
{
	typedef typename DomView<Set<T>,View>::Dom	Dom;	
	typedef typename Dom::PIterator PIterator;
		
	EraseElem(ICPSolver& s,const View& v,PIterator it) : IGoal(s),v(s,v),it(it) {}
	
	Goal execute(IGoalSched& s)
	{
		assert(v->findInPoss(*it) != v->endPoss());
		if (v->erase(it))
			return ok();
		return fail();
	}
	
	DomView<Set<T>,View> v;
	PIterator it;
};

template<class T,class View>
Goal eraseElem(const View& v,typename DomView<Set<T>,View>::Dom::PIterator it)
{	return new (getCPSolver(v).heap()) EraseElem<T,View>(getCPSolver(v),v,it);	}

namespace detail {
/**
 *	Goal for selecting a set variable. Attempts to assign a value 
 *  to the variable trying all possible values in ascending order on backtracking.   
 */
template<class T,int I,class D>
struct SelectValsMin<DomVarArray<Set<T>,I,D> > : IValSelector
{
	SelectValsMin(DomVarArray<Set<T>,I,D> vars) : vars(vars) {}

	Goal select(UInt idx) 
	{
		if (vars(idx).domain().ground())
			return ok();
		return (insertElem<T>(vars(idx),vars(idx).domain().beginPoss()) or
			   eraseElem<T>(vars(idx),vars(idx).domain().beginPoss()))
			    and callValSelector(vars.solver(),this,idx);	
	}
	DomVarArray<Set<T>,I,D> vars;
};

/**
 *	Goal for selecting a set variable. Attempts to assign a value 
 *  to the variable trying all possible values in random order on backtracking.   
 */
template<class T,int I,class D>
struct SelectValsRand<DomVarArray<Set<T>,I,D> > : IValSelector
{
	SelectValsRand(DomVarArray<Set<T>,I,D> vars) : vars(vars) {}

	Goal select(UInt idx) 
	{
		if (vars(idx).domain().ground())
			return ok();
		UInt r = static_cast<UInt>(rand()/(Double)RAND_MAX * vars(idx).domain().possSize());
		typename D::PIterator it = vars(idx).domain().beginPoss();
		while (r-- > 0)
			++it;
		return (insertElem<T>(vars(idx),it) or
			   eraseElem<T>(vars(idx),it))
			    and callValSelector(vars.solver(),this,idx);	
	}
	DomVarArray<Set<T>,I,D> vars;
};

/**
 *	Goal for bisecting a set variable. Attempts to assign a value 
 *  to the variable trying all possible values in ascending order on backtracking.   
 */
template<class T,int I,class D>
struct SelectValMin<DomVarArray<Set<T>,I,D> > : IValSelector
{
	SelectValMin(DomVarArray<Set<T>,I,D> vars) : vars(vars) {}

	Goal select(UInt idx) 
	{
		if (vars(idx).domain().ground())
			return ok();
		return insertElem<T>(vars(idx),vars(idx).domain().beginPoss()) or
			   eraseElem<T>(vars(idx),vars(idx).domain().beginPoss());	
	}
	DomVarArray<Set<T>,I,D> vars;
};
};


	
			
/*@}*/

};

#endif /*_H_CASPER_INT_GOAL*/
