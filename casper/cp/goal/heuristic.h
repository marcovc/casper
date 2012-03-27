 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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
#include <casper/kernel/goal/goal.h>
#include <casper/util/safeop.h>
#include <casper/cp/view/array.h>

/*! \file	*/

namespace Casper {
namespace CP {

// this is required since doxygen sometimes
// doesn't autolink namespace scoped things
#ifdef DOXYGEN_PARSER
using namespace Casper;
using namespace Casper::CP;
#endif


// base:

/**
 * \ingroup VariableSelectors
 * Variable selectors are used with the Label goal for defining the variable
 * selection strategy.
 */
struct IVarSelector
{
	/// Returns the index of the next variable to enumerate, or -1 to signal termination
	virtual int select() = 0;
	virtual ~IVarSelector() {}
};

/**
 *  \ingroup ValueSelectors
 *  Value selectors are used with the Label goal for defining the value selection
 *  strategy.
 *
 */
struct IValSelector
{
	/// Returns a new goal specifying the assigment of variable with index \a idx.
	virtual Goal select(uint) = 0;
	virtual ~IValSelector() {}
};

/**
 * \ingroup VariableSelectors
 * Wrapper to IVarSelector derived objects.
 */
struct VarSelector : Util::PImplIdiom<IVarSelector>
{
	typedef VarSelector	Self;
	typedef Util::PImplIdiom<IVarSelector> 	Super;

	VarSelector(IVarSelector*	f) : Super(f) {}
	VarSelector(const Super& s) : Super(s.getPImpl()) {}

	int select() {	return Super::pImpl->select(); }
};

/**
 *  \ingroup ValueSelectors
 * Wrapper to IValSelector derived objects.
 */
struct ValSelector : Util::PImplIdiom<IValSelector>
{
	typedef ValSelector	Self;
	typedef Util::PImplIdiom<IValSelector> 	Super;

	ValSelector(IValSelector*	f) : Super(f) {}
	ValSelector(const Super& s) : Super(s.getPImpl()) {}

	Goal select(uint idx) {	return Super::pImpl->select(idx); }
};


struct GroundTermCond
{
	template<class Eval,class Obj>
	bool operator()(const DomArrayView<Eval,Obj>& v) const
	{
		for (uint i = 0; i < v.size(); i++)
			if (!v[i]->ground())
				return false;
		return true;
	}
};

struct SizeTermCond
{
	SizeTermCond(double s) : s(s) {}
	template<class Eval,class Obj>
	bool operator()(const DomArrayView<Eval,Obj>& v) const
	{
		for (uint i = 0; i < v.size(); i++)
			if (v[i]->max()-v[i]->min() > s)
				return false;
		return true;
	}
	const double s;
};

// some popular heuristics

namespace Detail {

template<class Eval,class Obj> struct SelectValsMin;
template<class Eval,class Obj> struct SelectValsMax;
template<class Eval,class Obj> struct SelectValsRand;
template<class Eval,class Obj> struct SelectValMin;
template<class Eval,class Obj> struct SelectValMax;

// selects the variable which minimizes Evaluator from all possible variables
template<class Obj,class TermCond,class Evaluator>
struct SelectVarAll : IVarSelector
{
	SelectVarAll(Store& store,const Obj& obj,const TermCond& tc = TermCond()) :
		doms(store,obj),terminate(tc) {}
	int select()
	{
		typedef typename Evaluator::Eval Eval;
		if (terminate(doms))
			return -1;
		Eval bestVal = limits<Eval>::posInf();
		int bestIdx = -1;
		for (uint i = 0; i < doms.size(); ++i)
			if (!doms[i]->ground())
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
	typedef typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<Obj>::Type>::Type	ElemEval;
	DomArrayView<ElemEval,Obj> doms;
	TermCond terminate;
	Evaluator ev;
};

// selects the variable which minimizes Evaluator and which was not selected
// yet in the current round of selections (round robin)
template<class Obj,class TermCond,class Evaluator>
struct SelectVarAllRR : IVarSelector
{
	SelectVarAllRR(Store& store,const Obj& obj,const TermCond& tc = TermCond()) :
		doms(store,obj),terminate(tc),selected(store,doms.size(),false) {}

	int select()
	{
		typedef typename Evaluator::Eval Eval;
		if (terminate(doms))
			return -1;
		// if the current round is over then start again
		uint i;
		for (i = 0; i < doms.size(); ++i)
			if (!selected[i] and !doms[i]->ground())
				break;
		if (i == doms.size())
			selected = false;
		Eval bestVal = Eval();
		int bestIdx = -1;
		for (uint i = 0; i < doms.size(); ++i)
			if (!selected[i] and !doms[i]->ground())
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
	typedef typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<Obj>::Type>::Type	ElemEval;
	DomArrayView<ElemEval,Obj> doms;
	TermCond terminate;
	Array<bool> 	selected;
	Evaluator ev;
};

template<class ElemEval>
struct MinDomEvaluator
{
	typedef int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.size(); }
};

template<class ElemEval>
struct MaxDomEvaluator
{
	typedef int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.size(); }
};

template<class ElemEval>
struct MinMinElemEvaluator
{
	typedef int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.min(); }
};

template<class ElemEval>
struct MaxMinElemEvaluator
{
	typedef int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.min(); }
};

template<class ElemEval>
struct MinMaxElemEvaluator
{
	typedef int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.max(); }
};

template<class ElemEval>
struct MaxMaxElemEvaluator
{
	typedef int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.max(); }
};

template<class ElemEval>
struct MinDegreeEvaluator
{
	typedef int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.getNbSuspFilters(); }
};

template<class ElemEval>
struct MaxDegreeEvaluator
{
	typedef int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.getNbSuspFilters(); }
};

template<class ElemEval>
struct MinDomOverDegreeEvaluator
{
	typedef double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.size()/static_cast<double>(d.getNbSuspFilters()); }
};

template<class ElemEval>
struct MaxDomOverDegreeEvaluator
{
	typedef double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.size()/static_cast<double>(d.getNbSuspFilters()); }
};

template<class ElemEval>
struct MinWeightedDegreeEvaluator
{
	typedef int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.getAFC(); }
};

template<class ElemEval>
struct MaxWeightedDegreeEvaluator
{
	typedef int	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.getAFC(); }
};

template<class ElemEval>
struct MinDomOverWeightedDegreeEvaluator
{
	typedef double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return d.size()/static_cast<double>(d.getAFC()); }
};

template<class ElemEval>
struct MaxDomOverWeightedDegreeEvaluator
{
	typedef double	Eval;
	template<class Dom>
	Eval operator()(const Dom& d) const
	{	return -d.size()/static_cast<double>(d.getAFC()); }
};

/**
 *  \ingroup CPSearch
 * 	Variable heuristic that selects variables lexicographically.
 **/
template<class Obj,class TermCond = GroundTermCond>
struct SelectVarLex : IVarSelector
{
	SelectVarLex(Store& s,const Obj& obj,const TermCond& tc = TermCond()) :
		obj(s,obj),
		curIdx(s,0),
		termCond(tc) {}
	int select()
	{
		if (termCond(obj))
			return -1;
		for ( ; curIdx < obj.size(); ++curIdx)
			if (!obj[curIdx]->ground())
				return curIdx++;
		curIdx = 0;
		while (obj[curIdx]->ground())
			++curIdx;
		return curIdx++;
	}
	typedef typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<Obj>::Type>::Type	ElemEval;
	DomArrayView<ElemEval,Obj> obj;
	Reversible<uint> curIdx;
	TermCond		termCond;
};

}

// FIXME: leaking - must delay Label goal
template<class Obj>
VarSelector selectVarLex(Store& s,const Obj& obj)
{return new Detail::SelectVarLex<Obj>(s,obj);}

#define CASPER_DEF_VAR_SELECT(name) \
namespace Detail { \
template<class Obj,class TermCond=GroundTermCond> \
struct SelectVar##name : \
	Detail::SelectVarAll<Obj,TermCond, Detail::name##Evaluator<\
	typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<Obj>::Type>::Type> > \
{ \
	typedef typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<Obj>::Type>::Type EEval; \
	SelectVar##name(Store& s,const Obj& obj,const TermCond& tc = TermCond()) : \
		Detail::SelectVarAll<Obj,TermCond,Detail::name##Evaluator<EEval> >(s,obj,tc) {} \
}; \
template<class Obj,class TermCond=GroundTermCond> \
struct SelectVar##name##RR : Detail::SelectVarAllRR<Obj,TermCond, Detail::name##Evaluator<\
	typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<Obj>::Type>::Type> > \
{ \
	typedef typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<Obj>::Type>::Type EEval; \
	SelectVar##name##RR(Store& s,const Obj& obj,const TermCond& tc = TermCond()) : \
		Detail::SelectVarAllRR<Obj,TermCond,Detail::name##Evaluator<EEval> >(s,obj,tc) {} \
};\
}\
template<class Obj>\
VarSelector selectVar##name(Store& s,const Obj& obj, bool roundRobin = true) \
{\
	if (roundRobin)\
		return new (s) Detail::SelectVar##name##RR<Obj>(s,obj);\
	else\
		return new (s) Detail::SelectVar##name<Obj>(s,obj);\
}\
template<class Obj,class TermCond>\
VarSelector selectVar##name(Store& s,const Obj& obj, const TermCond& tc) \
{\
	return new (s) Detail::SelectVar##name##RR<Obj,TermCond>(s,obj,tc);\
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
 * \ingroup CPSearch
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
 * \li \f$ \left.d\left(\textrm{obj}\right)\right|_{x=v} \f$ is the resulting domain of
 *  \a obj after \f$ x=v \f$ is added to the constraint store and propagated
 *
 * Most variable selectors accept a boolean argument \a roundRobin which when set to \p true
 * guarantees a fair selection policy: a variable may be selected for the n'th
 * time only if all variables have been already selected at least n-1 times.
 *
 * \sa VarSelector
 */

/**
 * 	\addtogroup VariableSelectors
 */
//@{
/**
 * \fn template<class Obj> VarSelector selectVarLex(Store& store, const Obj& obj)
 * Selects variables by their position in the list or array \a obj, possibly restarting
 * from the first position after all variables have been selected
 *
 * \fn template<class Obj> VarSelector selectVarMinDom(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which minimizes \f$\left|d\left(x\right)\right|\f$
 *
 * \fn template<class Obj> VarSelector selectVarMaxDom(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which maximizes \f$\left|d\left(x\right)\right|\f$
 *
 * \fn template<class Obj> VarSelector selectVarMinMinElem(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which minimizes \f$\left\lfloor d\left(x\right)\right\rfloor \f$
 *
 * \fn template<class Obj> VarSelector selectVarMinMaxElem(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which minimizes \f$\left\lceil d\left(x\right)\right\rceil \f$
 *
 * \fn template<class Obj> VarSelector selectVarMaxMinElem(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which maximizes \f$\left\lfloor d\left(x\right)\right\rfloor \f$
 *
 * \fn template<class Obj> VarSelector selectVarMaxMaxElem(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which maximizes \f$\left\lceil d\left(x\right)\right\rceil \f$
 *
 * \fn template<class Obj> VarSelector selectVarMinDegree(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which minimizes \f$ \left|C\left(x\right)\right| \f$
 *
 * \fn template<class Obj> VarSelector selectVarMaxDegree(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which maximizes \f$ \left|C\left(x\right)\right| \f$
 *
 * \fn template<class Obj> VarSelector selectVarMinDomOverDegree(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which minimizes \f$ \left|d\left(x\right)\right|/\left|C\left(x\right)\right| \f$
 *
 * \fn template<class Obj> VarSelector selectVarMaxDomOverDegree(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which maximizes \f$ \left|d\left(x\right)\right|/\left|C\left(x\right)\right| \f$
 *
 * \fn template<class Obj> VarSelector selectVarMinWeightedDegree(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which minimizes \f$ \sum_{c\in C\left(x\right)}\textrm{afc}\left(c\right) \f$
 *
 * \fn template<class Obj> VarSelector selectVarMaxWeightedDegree(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which maximizes \f$ \sum_{c\in C\left(x\right)}\textrm{afc}\left(c\right) \f$
 *
 * \fn template<class Obj> VarSelector selectVarMinDomOverWeightedDegree(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which minimizes \f$ \left|d\left(x\right)\right|/\sum_{c\in C\left(x\right)}\textrm{afc}\left(c\right) \f$
 *
 * \fn template<class Obj> VarSelector selectVarMaxDomOverWeightedDegree(Store& store, const Obj& obj, bool roundRobin = true)
 * Selects the variable \f$x\in\f$\a obj which maximizes \f$ \left|d\left(x\right)\right|/\sum_{c\in C\left(x\right)}\textrm{afc}\left(c\right) \f$
 */
//@}


namespace Detail {

template<class Obj>
struct SelectHalfMin : IValSelector
{
	typedef typename Casper::Traits::GetEval<
			typename Casper::Traits::GetElem<Obj>::Type>::Type ElemEval;
	SelectHalfMin(Store& s,const Obj& obj) :
		store(s),obj(s,obj) {}
	Goal select(uint idx)
	{
		ElemEval midpoint = Util::median(obj[idx].min(),obj[idx].max());
		if (Casper::Traits::IsIntegral<ElemEval>::value)
			return Goal(store,post(store,rel<LessEqual>(obj[idx].getObj(),midpoint)) or
							  post(store,rel<Greater>(obj[idx].getObj(),midpoint)));
		else
			return Goal(store,post(store,rel<LessEqual>(obj[idx].getObj(),midpoint)) or
							  post(store,rel<GreaterEqual>(obj[idx].getObj(),midpoint)));
	}
	Store&						store;
	BndArrayView<ElemEval,Obj> obj;
};

template<class Obj>
struct SelectHalfMax : IValSelector
{
	typedef typename Casper::Traits::GetEval<
			typename Casper::Traits::GetElem<Obj>::Type>::Type	ElemEval;
	SelectHalfMax(Store& s,const Obj& obj) :
		store(s),obj(s,obj) {}
	Goal select(uint idx)
	{
		ElemEval midpoint = Util::median(obj[idx].min(),obj[idx].max());
		if (Casper::Traits::IsIntegral<ElemEval>::value)
			return Goal(store,post(store,rel<Greater>(obj[idx].getObj(),midpoint)) or
							  post(store,rel<LessEqual>(obj[idx].getObj(),midpoint)));
		else
			return Goal(store,post(store,rel<GreaterEqual>(obj[idx].getObj(),midpoint)) or
							  post(store,rel<LessEqual>(obj[idx].getObj(),midpoint)));
	}
	Store&						store;
	BndArrayView<ElemEval,Obj> obj;
};
}

/**
 * \defgroup ValueSelectors Value Selectors
 * \ingroup CPSearch
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
template<class Obj>
ValSelector selectValsMin(Store& s,const Obj& obj)
{return new (s) Detail::SelectValsMin<typename Casper::Traits::GetEval<Obj>::Type,Obj>(s,obj);}

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x=v_{\left|d\left(x\right)\right|} \vee \ldots \vee x=v_2 \vee x=v_1\f$
 **/
template<class Obj>
ValSelector selectValsMax(Store& s,const Obj& obj)
{return new (s) Detail::SelectValsMax<typename Casper::Traits::GetEval<Obj>::Type,Obj>(s,obj);}

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x=p_1 \vee x=p_2 \vee \ldots \vee x=p_{\left|d\left(x\right)\right|} \f$
 *  where \f$\mathbf{p}\f$ is a random permutation of \f$v_1\ldots v_{\left|d\left(x\right)\right|}\f$
 **/
template<class Obj>
ValSelector selectValsRand(Store& s,const Obj& obj)
{return new (s) Detail::SelectValsRand<typename Casper::Traits::GetEval<Obj>::Type,Obj>(s,obj);}

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x=v_1 \vee x\neq v_1\f$
 **/
template<class Obj>
ValSelector selectValMin(Store& s,const Obj& obj)
{return new (s) Detail::SelectValMin<typename Casper::Traits::GetEval<Obj>::Type,Obj>(s,obj);}

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x=v_{\left|d\left(x\right)\right|} \vee x\neq v_{\left|d\left(x\right)\right|}\f$
 **/
template<class Obj>
ValSelector selectValMax(Store& s,const Obj& obj)
{return new (s) Detail::SelectValMax<typename Casper::Traits::GetEval<Obj>::Type,Obj>(s,obj);}

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x\leq\left(v_1+v_{\left|d\left(x\right)\right|}\right)/2 \vee
 *      x>\left(v_1+v_{\left|d\left(x\right)\right|}\right)/2 \f$
 **/
template<class Obj>
ValSelector selectHalfMin(Store& s,const Obj& obj)
{return new (s) Detail::SelectHalfMin<Obj>(s,obj);}

/**
 * 	For a given variable \f$ x \f$ returns the goal
 *  \f$ x>\left(v_1+v_{\left|d\left(x\right)\right|}\right)/2 \vee
 *  x\leq\left(v_1+v_{\left|d\left(x\right)\right|}\right)/2 \f$
 **/
template<class Obj>
ValSelector selectHalfMax(Store& s,const Obj& obj)
{return new (s) Detail::SelectHalfMax<Obj>(s,obj);}

//@}

namespace Detail {
struct CallValSelector : IGoal
{
	CallValSelector(ValSelector s, uint idx) :
		s(s),idx(idx) {}
	Goal execute()
	{	return s.select(idx);	}
	ValSelector s;
	uint idx;
};
}

/**
 *  \ingroup Search
 * 	Goal that simply calls the given value selector. Use this to create
 *  recursive value selectors.
 **/
Goal callValSelector(State& s, ValSelector v,uint idx);

} //CP
} // Casper

#endif /*_H_CASPER_KERNEL_HEURISTIC*/
