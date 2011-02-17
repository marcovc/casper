 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
 *                                                                         *
 *   Licensed under the Apache License, Version 2.0 (the "License");       *
 *   you may not use this file except in compliance with the License.      *
 *   You may obtain a copy of the License at                               *
 *            http://www.apache.org/licenses/LICENSE-2.0                   *
 *   Unless required by applicable law or agreed to in writing, software   *
 *   distributed under the License is distributed on an                    *
 *   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,          *
 *   either express or implied.                                            *
 *   See the License for the specific language governing permissions and   *
 *   limitations under the License.                                        *
 \*************************************************************************/

#ifndef _H_CASPER_KERNEL_HEURISTIC
#define _H_CASPER_KERNEL_HEURISTIC

#include <casper/kernel/common.h>
#include <casper/kernel/goal/common.h>
#include <casper/kernel/safeop.h>
#include <casper/kernel/view/arrayview.h>

/*! \file	*/

namespace casper {

// base:

/**
 * \ingroup Search
 * Variable selectors are used with the Label goal for defining the variable
 * selection strategy.
 */
struct IVarSelector
{
	/// Returns the index of the next variable to enumerate, or -1 to signal termination
	virtual Int select() = 0;
	virtual ~IVarSelector() {}
};

/**
 *  \ingroup Search
 *  Value selectors are used with the Label goal for defining the value selection
 *  strategy.
 *
 */
struct IValSelector
{
	/// Returns a new goal specifying the assigment of variable with index \a idx.
	virtual Goal select(UInt) = 0;
	virtual ~IValSelector() {}
};

/**
 * \ingroup Search
 * Wrapper to IVarSelector derived objects.
 */
struct VarSelector : detail::PImplIdiom<IVarSelector>
{
	typedef VarSelector	Self;
	typedef detail::PImplIdiom<IVarSelector> 	Super;

	VarSelector(IVarSelector*	f) : Super(f) {}
	VarSelector(const Super& s) : Super(s.getPImpl()) {}

	Int select() {	return Super::pImpl->select(); }
};

/**
 *  \ingroup Search
 * Wrapper to IValSelector derived objects.
 */
struct ValSelector : detail::PImplIdiom<IValSelector>
{
	typedef ValSelector	Self;
	typedef detail::PImplIdiom<IValSelector> 	Super;

	ValSelector(IValSelector*	f) : Super(f) {}
	ValSelector(const Super& s) : Super(s.getPImpl()) {}

	Goal select(UInt idx) {	return Super::pImpl->select(idx); }
};


struct GroundTermCond
{
	template<class Eval,class View>
	Bool operator()(const DomArrayView<Eval,View>& v) const
	{
		for (UInt i = 0; i < v.size(); i++)
			if (!v[i]->ground())
				return false;
		return true;
	}
};

// some popular heuristics

namespace detail {

template<class View> struct SelectValsMin;
template<class View> struct SelectValsMax;
template<class View> struct SelectValsRand;
template<class View> struct SelectValMin;
template<class View> struct SelectValMax;

// selects the variable which minimizes Evaluator from all possible variables
template<class View,class TermCond,class Evaluator>
struct SelectVarAll : IVarSelector
{
	SelectVarAll(const View& vars,const TermCond& tc = TermCond()) :
		doms(getCPSolver(vars),vars),terminate(tc) {}
	Int select()
	{
		typedef typename Evaluator::Eval Eval;
		if (terminate(doms))
			return -1;
		Eval bestVal = ev(*doms[0]);
		Int bestIdx = 0;
		for (UInt i = 1; i < doms.size(); ++i)
		{
			Eval curVal = ev(*doms[i]);
			if (curVal < bestVal)
			{
				bestVal = curVal;
				bestIdx = i;
			}
		}
		assert(bestIdx>=0);
		return bestIdx;
	}
	typedef typename EvalTraits<View>::Eval::Elem ElemEval;
	DomArrayView<ElemEval,View> doms;
	TermCond terminate;
	Evaluator ev;
};

// selects the variable which minimizes Evaluator and which was not selected
// yet in the current round of selections (round robin)
template<class View,class TermCond,class Evaluator>
struct SelectVarAllRR : IVarSelector
{
	SelectVarAllRR(const View& vars,const TermCond& tc = TermCond()) :
		doms(getCPSolver(vars),vars),terminate(tc),selected(doms.solver(),doms.size(),false) {}

	Int select()
	{
		typedef typename Evaluator::Eval Eval;
		if (terminate(doms))
			return -1;
		// if the current round is over then start again
		UInt i;
		for (i = 0; i < doms.size(); ++i)
			if (!selected[i])
				break;
		if (i == doms.size())
			selected = false;
		Eval bestVal = Eval();
		Int bestIdx = -1;
		for (UInt i = 0; i < doms.size(); ++i)
			if (!selected[i])
			{
				Eval curVal = ev(*doms[i]);
				if (bestIdx==-1 or curVal < bestVal)
				{
					bestVal = curVal;
					bestIdx = i;
				}
			}
		assert(bestIdx>=0);
		selected[bestIdx] = true;
		return bestIdx;
	}
	typedef typename EvalTraits<View>::Eval::Elem ElemEval;
	DomArrayView<ElemEval,View> doms;
	TermCond terminate;
	RevArray<Bool> 	selected;
	Evaluator ev;
};

template<class ElemEval>
struct MinDomEvaluator
{
	typedef Int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.size(); }
};

template<class ElemEval>
struct MaxDomEvaluator
{
	typedef Int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.size(); }
};

template<class ElemEval>
struct MinMinElemEvaluator
{
	typedef Int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.min(); }
};

template<class ElemEval>
struct MaxMinElemEvaluator
{
	typedef Int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.min(); }
};

template<class ElemEval>
struct MinMaxElemEvaluator
{
	typedef Int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.max(); }
};

template<class ElemEval>
struct MaxMaxElemEvaluator
{
	typedef Int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.max(); }
};

template<class ElemEval>
struct MinDegreeEvaluator
{
	typedef Int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.getNbSuspFilters(); }
};

template<class ElemEval>
struct MaxDegreeEvaluator
{
	typedef Int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.getNbSuspFilters(); }
};

template<class ElemEval>
struct MinDomOverDegreeEvaluator
{
	typedef Double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.size()/static_cast<Double>(d.getNbSuspFilters()); }
};

template<class ElemEval>
struct MaxDomOverDegreeEvaluator
{
	typedef Double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.size()/static_cast<Double>(d.getNbSuspFilters()); }
};

template<class ElemEval>
struct MinWeightedDegreeEvaluator
{
	typedef Int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.getAFC(); }
};

template<class ElemEval>
struct MaxWeightedDegreeEvaluator
{
	typedef Int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.getAFC(); }
};

template<class ElemEval>
struct MinDomOverWeightedDegreeEvaluator
{
	typedef Double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.size()/static_cast<Double>(d.getAFC()); }
};

template<class ElemEval>
struct MaxDomOverWeightedDegreeEvaluator
{
	typedef Double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.size()/static_cast<Double>(d.getAFC()); }
};

/**
 *  \ingroup Search
 * 	Variable heuristic that selects variables lexicographically.
 **/
template<class View,class TermCond = GroundTermCond>
struct SelectVarLex : IVarSelector
{
	SelectVarLex(const View& vars,const TermCond& tc = TermCond()) :
		vars(getCPSolver(vars),vars),
		curIdx(getCPSolver(vars),0),
		termCond(tc) {}
	Int select()
	{
		if (termCond(vars))
			return -1;
		if (curIdx >= vars.size())
			curIdx = 0;
		return curIdx++;
	}
	typedef typename EvalTraits<View>::Eval::Elem ElemEval;
	DomArrayView<ElemEval,View> vars;
	Reversible<UInt> curIdx;
	TermCond		termCond;
};

}

template<class View>
VarSelector selectVarLex(const View& view)
{return new (getCPSolver(view).heap()) detail::SelectVarLex<View>(view);}

#define CASPER_DEF_VAR_SELECT(name) \
namespace detail { \
template<class View,class TermCond=GroundTermCond> \
struct SelectVar##name : \
	detail::SelectVarAll<View,TermCond, detail::name##Evaluator<\
	typename EvalTraits<typename ElemTraits<View>::Elem>::Eval> > \
{ \
	typedef typename EvalTraits<typename ElemTraits<View>::Elem>::Eval EEval; \
	SelectVar##name(const View& vars,const TermCond& tc = TermCond()) : \
		detail::SelectVarAll<View,TermCond,detail::name##Evaluator<EEval> >(vars,tc) {} \
}; \
template<class View,class TermCond=GroundTermCond> \
struct SelectVar##name##RR : detail::SelectVarAllRR<View,TermCond, detail::name##Evaluator<\
	typename EvalTraits<typename ElemTraits<View>::Elem>::Eval> > \
{ \
	typedef typename EvalTraits<typename ElemTraits<View>::Elem>::Eval EEval; \
	SelectVar##name##RR(const View& vars,const TermCond& tc = TermCond()) : \
		detail::SelectVarAllRR<View,TermCond,detail::name##Evaluator<EEval> >(vars,tc) {} \
};\
}\
template<class Vars>\
VarSelector selectVar##name(const Vars& vars, bool roundRobin = true) \
{\
	if (roundRobin)\
		return new (getCPSolver(vars).heap()) detail::SelectVar##name##RR<Vars>(vars);\
	else\
		return new (getCPSolver(vars).heap()) detail::SelectVar##name<Vars>(vars);\
}


CASPER_DEF_VAR_SELECT(MinDom)
CASPER_DEF_VAR_SELECT(MaxDom)
CASPER_DEF_VAR_SELECT(MinMinElem)
CASPER_DEF_VAR_SELECT(MinMaxElem)
CASPER_DEF_VAR_SELECT(MaxMinElem)
CASPER_DEF_VAR_SELECT(MaxMaxElem)
CASPER_DEF_VAR_SELECT(MinDegree)
CASPER_DEF_VAR_SELECT(MaxDegree)
CASPER_DEF_VAR_SELECT(MinDomOverDegree)
CASPER_DEF_VAR_SELECT(MaxDomOverDegree)
CASPER_DEF_VAR_SELECT(MinWeightedDegree)
CASPER_DEF_VAR_SELECT(MaxWeightedDegree)
CASPER_DEF_VAR_SELECT(MinDomOverWeightedDegree)
CASPER_DEF_VAR_SELECT(MaxDomOverWeightedDegree)

#undef CASPER_DEF_VAR_SELECT

/**
 * \defgroup VariableSelectors Variable Selectors
 * \ingroup Search
 *
 * Variable selectors are function objects which compute the index of the next
 * variable to enumerate during search. They are used to implement variable
 * search heuristics.
 *
 * The above description makes use of the following formalism:
 * \li \f$d\left(x\right)\f$ is the domain of variable \f$ x \f$
 * \li \f$C\left(x\right)\f$ is the set of propagators attached to variable \f$ x \f$
 * (reflects the constraints where \f$ x \f$ participates)
 * \li \f$ \textrm{afc}\left(c\right) \f$ is the Accumulated Failure Count of the
 * propagator \f$ c \f$, i.e. the number of times \f$ c \f$ has failed since
 * the begining of search
 * \li \f$ \left.d\left(\textrm{vars}\right)\right|_{x=v} \f$ is the resulting domain of
 *  \a vars after \f$ x=v \f$ is added to the constraint store and propagated
 *
 * Most variable selectors accept a boolean argument \a roundRobin which when set to \p true
 * guarantees a fair selection policy: a variable may be selected for the n'th
 * time if all variables have been already selected at least n-1 times.
 *
 * \sa VarSelector
 */

/**
 * 	\addtogroup VariableSelectors
 */
//@{
/**
 * \fn template<class Vars> VarSelector selectVarLex(const Vars& vars)
 * Selects variables by their position in the list or array \a view, possibly restarting
 * from the first position after all variables have been selected
 *
 * \fn template<class Vars> VarSelector selectVarMinDom(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which minimizes \f$\left|d\left(x\right)\right|\f$
 *
 * \fn template<class Vars> VarSelector selectVarMaxDom(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which maximizes \f$\left|d\left(x\right)\right|\f$
 *
 * \fn template<class Vars> VarSelector selectVarMinMinElem(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which minimizes \f$\left\lfloor d\left(x\right)\right\rfloor \f$
 *
 * \fn template<class Vars> VarSelector selectVarMinMaxElem(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which minimizes \f$\left\lceil d\left(x\right)\right\rceil \f$
 *
 * \fn template<class Vars> VarSelector selectVarMaxMinElem(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which maximizes \f$\left\lfloor d\left(x\right)\right\rfloor \f$
 *
 * \fn template<class Vars> VarSelector selectVarMaxMaxElem(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which maximizes \f$\left\lceil d\left(x\right)\right\rceil \f$
 *
 * \fn template<class Vars> VarSelector selectVarMinDegree(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which minimizes \f$ \left|C\left(x\right)\right| \f$
 *
 * \fn template<class Vars> VarSelector selectVarMaxDegree(const Vars& view, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which maximizes \f$ \left|C\left(x\right)\right| \f$
 *
 * \fn template<class Vars> VarSelector selectVarMinDomOverDegree(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which minimizes \f$ \left|d\left(x\right)\right|/\left|C\left(x\right)\right| \f$
 *
 * \fn template<class Vars> VarSelector selectVarMaxDomOverDegree(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which maximizes \f$ \left|d\left(x\right)\right|/\left|C\left(x\right)\right| \f$
 *
 * \fn template<class Vars> VarSelector selectVarMinWeightedDegree(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which minimizes \f$ \sum_{c\in C\left(x\right)}\textrm{afc}\left(c\right) \f$
 *
 * \fn template<class Vars> VarSelector selectVarMaxWeightedDegree(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which maximizes \f$ \sum_{c\in C\left(x\right)}\textrm{afc}\left(c\right) \f$
 *
 * \fn template<class Vars> VarSelector selectVarMinDomOverWeightedDegree(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which minimizes \f$ \left|d\left(x\right)\right|/\sum_{c\in C\left(x\right)}\textrm{afc}\left(c\right) \f$
 *
 * \fn template<class Vars> VarSelector selectVarMaxDomOverWeightedDegree(const Vars& vars, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a vars which maximizes \f$ \left|d\left(x\right)\right|/\sum_{c\in C\left(x\right)}\textrm{afc}\left(c\right) \f$
 */
//@}


namespace detail {

template<class View>
struct SelectHalfMin : IValSelector
{
	typedef typename EvalTraits<View>::Eval::Elem	ElemEval;
	SelectHalfMin(const View& vars) :
		vars(getCPSolver(vars),vars) {}
	Goal select(UInt idx)
	{
		ElemEval midpoint = detail::median(vars[idx].min(),vars[idx].max());
		if (detail::IsIntegral<ElemEval>::value)
			return vars[idx].getView() <= midpoint or
				   vars[idx].getView() > midpoint;
		else
			return vars[idx].getView() <= midpoint or
				   vars[idx].getView() >= midpoint;
	}
	BndArrayView<ElemEval,View> vars;
};

template<class View>
struct SelectHalfMax : IValSelector
{
	typedef typename EvalTraits<View>::Eval::Elem	ElemEval;
	SelectHalfMax(const View& vars) :
		vars(getSolver(vars),vars) {}
	Goal select(UInt idx)
	{
		ElemEval midpoint = detail::median(vars[idx].min(),vars[idx].max());
		if (detail::IsIntegral<ElemEval>::value)
			return vars[idx].getView() > midpoint or
				   vars[idx].getView() <= midpoint;
		else
			return vars[idx].getView() >= midpoint or
				   vars[idx].getView() <= midpoint;
	}
	BndArrayView<ElemEval,View> vars;
};
}

/**
 * \defgroup ValueSelectors Value Selectors
 * \ingroup Search
 *
 * Value selectors are function objects that specify the value selection
 * strategy during search.
 *
 * \sa ValSelector
 */

/// \addtogroup ValueSelectors
//@{

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x=v_1 \vee x=v_2 \vee \ldots \vee x=v_{\left|d\left(x\right)\right|} \f$
 **/
template<class View>
ValSelector selectValsMin(const View& view)
{return new (getCPSolver(view).heap()) detail::SelectValsMin<View>(view);}

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x=v_{\left|d\left(x\right)\right|} \vee \ldots \vee x=v_2 \vee x=v_1\f$
 **/
template<class View>
ValSelector selectValsMax(const View& view)
{return new (getCPSolver(view).heap()) detail::SelectValsMax<View>(view);}

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x=p_1 \vee x=p_2 \vee \ldots \vee x=p_{\left|d\left(x\right)\right|} \f$
 *  where \f$\mathbf{p}\f$ is a random permutation of \f$v_1\ldots v_{\left|d\left(x\right)\right|}\f$
 **/
template<class View>
ValSelector selectValsRand(const View& view)
{return new (getCPSolver(view).heap()) detail::SelectValsRand<View>(view);}

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x=v_1 \vee x\neq v_1\f$
 **/
template<class View>
ValSelector selectValMin(const View& view)
{return new (getCPSolver(view).heap()) detail::SelectValMin<View>(view);}

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x=v_{\left|d\left(x\right)\right|} \vee x\neq v_{\left|d\left(x\right)\right|}\f$
 **/
template<class View>
ValSelector selectValMax(const View& view)
{return new (getCPSolver(view).heap()) detail::SelectValMax<View>(view);}

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x\leq\left(v_1+v_{\left|d\left(x\right)\right|}\right)/2 \vee
 *      x>\left(v_1+v_{\left|d\left(x\right)\right|}\right)/2 \f$
 **/
template<class View>
ValSelector selectHalfMin(const View& view)
{return new (getCPSolver(view).heap()) detail::SelectHalfMin<View>(view);}

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x>\left(v_1+v_{\left|d\left(x\right)\right|}\right)/2 \vee
 *  x\leq\left(v_1+v_{\left|d\left(x\right)\right|}\right)/2 \f$
 **/
template<class View>
ValSelector selectHalfMax(const View& view)
{return new (getCPSolver(view).heap()) detail::SelectHalfMax<View>(view);}

//@}

namespace detail {
struct CallValSelector : IGoal
{
	CallValSelector(ICPSolver& solver,ValSelector s, UInt idx) :
		IGoal(solver),s(s),idx(idx) {}
	Goal execute()
	{	return s.select(idx);	}
	ValSelector s;
	UInt idx;
};
}

/**
 *  \ingroup Goals
 * 	Goal that simply calls the given value selector. Use this to create
 *  recursive value selectors.
 **/
Goal callValSelector(ICPSolver& s, ValSelector v,UInt idx);

};

#endif /*_H_CASPER_KERNEL_HEURISTIC*/
