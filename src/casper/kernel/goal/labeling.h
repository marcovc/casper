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

#ifndef _H_CASPER_KERNEL_GOAL_LABELING
#define _H_CASPER_KERNEL_GOAL_LABELING

#include <casper/kernel/goal/common.h>
#include <casper/kernel/goal/heuristic.h>

namespace casper {

namespace detail {
template<class Eval,class View>
struct Label;
template<class Eval,class View>
struct LabelWithCall;
};

/**
 *  \name Labeling Primitives
 *	\anchor LabelingPrimitives
 *
 *  The above functions return a new Goal that labels, i.e. assigns, all variables
 *  in \a s.
 *
*/
//@{
/**
*  \ingroup Search
*
*   Labels all variables in \a s.
 *	Variables are selected using the SelectVarLex variable selector, and
 *	values are selected using the SelectValsMin value selector.
 */
template<class View>
Goal label(const View& s)
{	return new (getCPSolver(s).heap()) detail::Label<typename EvalTraits<View>::Eval,View>(getCPSolver(s),s);	}

/**
*   Labels all variables in \a s.
*   Variables are selected using the \a varSel variable selector, and
 *	values are selected using the SelectValsMin value selector.
 */
template<class View>
Goal label(const View& s, VarSelector varSel)
{	return new (getCPSolver(s).heap()) detail::Label<typename EvalTraits<View>::Eval,View>(getCPSolver(s),s,varSel);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the SelectVarLex variable selector, and
 *	values are selected using the \a valSel value selector.
 */
template<class View>
Goal label(const View& s, ValSelector valSel)
{	return new (getCPSolver(s).heap()) detail::Label<typename EvalTraits<View>::Eval,View>(getCPSolver(s),s,valSel);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the varSel variable selector, and
 *	values are selected using the valSel value selector.
 */
template<class View>
Goal label(const View& s, VarSelector varSel, ValSelector valSel)
{	return new (getCPSolver(s).heap()) detail::Label<typename EvalTraits<View>::Eval,View>(getCPSolver(s),s,varSel,valSel);	}

/**
 *  \ingroup Search
 *
*   Labels all variables in \a s.
 *	Variables are selected using the SelectVarLex variable selector, and
 *	values are selected using the SelectValsMin value selector.
 *	A call to \a goal is made before each choice point.
 */
template<class View>
Goal label(const View& s, Goal goal)
{	return new (getCPSolver(s).heap())
		detail::LabelWithCall<typename EvalTraits<View>::Eval,View>(getCPSolver(s),s,goal);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the \a varSel variable selector, and
 *	values are selected using the SelectValsMin value selector.
 *	A call to \a goal is made before each choice point.
 */
template<class View>
Goal label(const View& s, Goal goal, VarSelector varSel)
{	return new (getCPSolver(s).heap())
		detail::LabelWithCall<typename EvalTraits<View>::Eval,View>(getCPSolver(s),s,goal,varSel);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the SelectVarLex variable selector, and
 *	values are selected using the \a valSel value selector.
 *	A call to \a goal is made before each choice point.
 */
template<class View>
Goal label(const View& s, Goal goal,ValSelector valSel)
{	return new (getCPSolver(s).heap())
		detail::LabelWithCall<typename EvalTraits<View>::Eval,View>(getCPSolver(s),s,goal,valSel);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the varSel variable selector, and
 *	values are selected using the valSel value selector.
 *	A call to \a goal is made before each choice point.
 */
template<class View>
Goal label(const View& s, Goal goal,VarSelector varSel, ValSelector valSel)
{	return new (getCPSolver(s).heap())
		detail::LabelWithCall<typename EvalTraits<View>::Eval,View>(getCPSolver(s),s,goal,varSel,valSel);	}
//@}

namespace detail {
template<class T,class View>
struct Label<Seq<T>,View> : IGoal
{
	Label(ICPSolver& solver, const View& vars) :
		IGoal(solver),doms(solver,vars),
		varSel(selectVarLex(vars)),valSel(selectValsMin(vars)) {}
	Label(ICPSolver& solver, const View& vars, VarSelector varSel) :
		IGoal(solver),doms(solver,vars),
		varSel(varSel),valSel(selectValsMin(vars)) {}
	Label(ICPSolver& solver, const View& vars, ValSelector valSel) :
		IGoal(solver),doms(solver,vars),
		varSel(selectVarLex(vars)),valSel(valSel) {}
	Label(ICPSolver& solver, const View& vars,
						VarSelector varSel,ValSelector valSel) :
		IGoal(solver),doms(solver,vars),
		varSel(varSel),valSel(valSel) {}

	Goal execute()
	{
		Int idx = varSel.select();
		if (idx < 0)
			return ok();
		if (doms[idx]->ground())
			return this;
		return valSel.select(idx) and Goal(this);
	}
	DomArrayView<T,View> doms;
	VarSelector varSel;
	ValSelector valSel;
};

// same as previous but where a given Goal is called before each decision
template<class T,class View>
struct LabelWithCall<Seq<T>,View> : IGoal
{
	LabelWithCall(ICPSolver& solver, const View& vars, const Goal& call) :
		IGoal(solver),doms(solver,vars),call(call),
		varSel(selectVarLex(vars)),valSel(selectValsMin(vars)) {}
	LabelWithCall(ICPSolver& solver, const View& vars, const Goal& call, VarSelector varSel) :
		IGoal(solver),doms(solver,vars),call(call),
		varSel(varSel),valSel(selectValsMin(vars)) {}
	LabelWithCall(ICPSolver& solver, const View& vars, const Goal& call,ValSelector valSel) :
		IGoal(solver),doms(solver,vars),call(call),
		varSel(selectVarLex(vars)),valSel(valSel) {}
	LabelWithCall(ICPSolver& solver, const View& vars, const Goal& call,
						VarSelector varSel,ValSelector valSel) :
		IGoal(solver),doms(solver,vars),call(call),
		varSel(varSel),valSel(valSel) {}

	Goal execute()
	{
		Int idx = varSel.select();
		if (idx < 0)
			return ok();
		if (doms[idx]->ground())
			return this;
		return call and valSel.select(idx) and Goal(this);
	}
	DomArrayView<T,View> doms;
	Goal call;
	VarSelector varSel;
	ValSelector valSel;
};
};


};

#endif /*_H_CASPER_KERNEL_GOAL_LABELING */
