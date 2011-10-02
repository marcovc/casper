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

#ifndef _H_CASPER_KERNEL_GOAL_LABELING
#define _H_CASPER_KERNEL_GOAL_LABELING

#include <casper/kernel/goal/goal.h>
#include <casper/kernel/goal/terminal.h>
#include <casper/cp/goal/heuristic.h>

namespace Casper {
namespace CP {

// this is required since doxygen sometimes
// doesn't autolink namespace scoped things
#ifdef DOXYGEN_PARSER
using namespace Casper;
using namespace Casper::CP;
#endif

namespace Detail {
template<class Eval,class Obj>
struct Label;
template<class Eval,class Obj>
struct LabelWithCall;
};

/**
 *  \name Labeling Primitives
 *	\anchor LabelingPrimitives
 *
 *  The functions below return a new Goal that labels, i.e. assigns, all variables
 *  in \a s while propagating \a store.
 *
*/
//@{
/**
*   Labels all variables in \a s.
 *	Variables are selected using the SelectVarLex variable selector, and
 *	values are selected using the SelectValsMin value selector.
 *	\ingroup CPSearch
 */
template<class Obj>
Goal label(Store& store,const Obj& s)
{	return new (store) Detail::Label<typename Casper::Traits::GetEval<Obj>::Type,Obj>(store,s);	}

/**
*   Labels all variables in \a s.
*   Variables are selected using the \a varSel variable selector, and
 *	values are selected using the SelectValsMin value selector.
 *	\ingroup CPSearch
 */
template<class Obj>
Goal label(Store& store,const Obj& s, VarSelector varSel)
{	return new (store) Detail::Label<typename Casper::Traits::GetEval<Obj>::Type,Obj>(store,s,varSel);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the SelectVarLex variable selector, and
 *	values are selected using the \a valSel value selector.
 *	\ingroup CPSearch
 */
template<class Obj>
Goal label(Store& store,const Obj& s, ValSelector valSel)
{	return new (store) Detail::Label<typename Casper::Traits::GetEval<Obj>::Type,Obj>(store,s,valSel);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the varSel variable selector, and
 *	values are selected using the valSel value selector.
 *	\ingroup CPSearch
 */
template<class Obj>
Goal label(Store& store,const Obj& s, VarSelector varSel, ValSelector valSel)
{	return new (store) Detail::Label<typename Casper::Traits::GetEval<Obj>::Type,Obj>(store,s,varSel,valSel);	}

/**
 *  \ingroup Search
 *
*   Labels all variables in \a s.
 *	Variables are selected using the SelectVarLex variable selector, and
 *	values are selected using the SelectValsMin value selector.
 *	A call to \a goal is made before each choice point.
 *	\ingroup CPSearch
 */
template<class Obj>
Goal label(Store& store,const Obj& s, Goal goal)
{	return new (store)
		Detail::LabelWithCall<typename Casper::Traits::GetEval<Obj>::Type,Obj>(store,s,goal);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the \a varSel variable selector, and
 *	values are selected using the SelectValsMin value selector.
 *	A call to \a goal is made before each choice point.
 *	\ingroup CPSearch
 */
template<class Obj>
Goal label(Store& store,const Obj& s, Goal goal, VarSelector varSel)
{	return new (store)
		Detail::LabelWithCall<typename Casper::Traits::GetEval<Obj>::Type,Obj>(store,s,goal,varSel);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the SelectVarLex variable selector, and
 *	values are selected using the \a valSel value selector.
 *	A call to \a goal is made before each choice point.
 *	\ingroup CPSearch
 */
template<class Obj>
Goal label(Store& store,const Obj& s, Goal goal,ValSelector valSel)
{	return new (store)
		Detail::LabelWithCall<typename Casper::Traits::GetEval<Obj>::Type,Obj>(store,s,goal,valSel);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the varSel variable selector, and
 *	values are selected using the valSel value selector.
 *	A call to \a goal is made before each choice point.
 *	\ingroup CPSearch
 */
template<class Obj>
Goal label(Store& store,const Obj& s, Goal goal,VarSelector varSel, ValSelector valSel)
{	return new (store)
		Detail::LabelWithCall<typename Casper::Traits::GetEval<Obj>::Type,Obj>(store,s,goal,varSel,valSel);	}
//@}

namespace Detail {
template<class T,class Obj>
struct Label<Seq<T>,Obj> : IGoal
{
	Label(Store& store, const Obj& vars) :
		store(store),
		doms(store,vars),
		varSel(selectVarLex(store,vars)),valSel(selectValsMin(store,vars)) {}
	Label(Store& store, const Obj& vars, VarSelector varSel) :
		store(store),
		doms(store,vars),
		varSel(varSel),valSel(selectValsMin(store,vars)) {}
	Label(Store& store, const Obj& vars, ValSelector valSel) :
		store(store),
		doms(store,vars),
		varSel(selectVarLex(store,vars)),valSel(valSel) {}
	Label(Store& store, const Obj& vars,
						VarSelector varSel,ValSelector valSel) :
		store(store),
		doms(store,vars),
		varSel(varSel),valSel(valSel) {}

	Goal execute()
	{
		if (!store.valid())
			return fail();
		int idx = varSel.select();
		if (idx < 0)
			return succeed();
		if (doms[idx]->ground())
			return this;
		return Goal(store,valSel.select(idx) and Goal(this));
	}
	Store&		store;
	DomArrayView<T,Obj> doms;
	VarSelector varSel;
	ValSelector valSel;
};

// same as previous but where a given Goal is called before each decision
template<class T,class Obj>
struct LabelWithCall<Seq<T>,Obj> : IGoal
{
	LabelWithCall(Store& store, const Obj& vars, const Goal& call) :
		store(store),
		doms(store,vars),call(call),
		varSel(selectVarLex(store,vars)),valSel(selectValsMin(store,vars)) {}
	LabelWithCall(Store& store, const Obj& vars, const Goal& call, VarSelector varSel) :
		store(store),
		doms(store,vars),call(call),
		varSel(varSel),valSel(selectValsMin(store,vars)) {}
	LabelWithCall(Store& store, const Obj& vars, const Goal& call,ValSelector valSel) :
		store(store),
		doms(store,vars),call(call),
		varSel(selectVarLex(store,vars)),valSel(valSel) {}
	LabelWithCall(Store& store, const Obj& vars, const Goal& call,
						VarSelector varSel,ValSelector valSel) :
		store(store),
		doms(store,vars),call(call),
		varSel(varSel),valSel(valSel) {}

	Goal execute()
	{
		if (!store.valid())
			return fail();
		int idx = varSel.select();
		if (idx < 0)
			return succeed();
		if (doms[idx]->ground())
			return this;
		return Goal(store,call and valSel.select(idx) and Goal(this));
	}

	Store&	store;
	DomArrayView<T,Obj> doms;
	Goal call;
	VarSelector varSel;
	ValSelector valSel;
};
};

} // CP
} // Casper

#endif /*_H_CASPER_KERNEL_GOAL_LABELING */
