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

#include <casper/kernel/goal/goal.h>
#include <casper/kernel/goal/terminal.h>
#include <casper/cp/goal/heuristic.h>

namespace Casper {
namespace CP {


namespace Detail {
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
Goal label(Store& store,const View& s)
{	return new (store) Detail::Label<typename Casper::Traits::GetEval<View>::Type,View>(store,s);	}

/**
*   Labels all variables in \a s.
*   Variables are selected using the \a varSel variable selector, and
 *	values are selected using the SelectValsMin value selector.
 */
template<class View>
Goal label(Store& store,const View& v, VarSelector varSel)
{	return new (store) Detail::Label<typename Casper::Traits::GetEval<View>::Type,View>(store,v,varSel);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the SelectVarLex variable selector, and
 *	values are selected using the \a valSel value selector.
 */
template<class View>
Goal label(Store& store,const View& v, ValSelector valSel)
{	return new (store) Detail::Label<typename Casper::Traits::GetEval<View>::Type,View>(store,v,valSel);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the varSel variable selector, and
 *	values are selected using the valSel value selector.
 */
template<class View>
Goal label(Store& store,const View& v, VarSelector varSel, ValSelector valSel)
{	return new (store) Detail::Label<typename Casper::Traits::GetEval<View>::Type,View>(store,v,varSel,valSel);	}

/**
 *  \ingroup Search
 *
*   Labels all variables in \a s.
 *	Variables are selected using the SelectVarLex variable selector, and
 *	values are selected using the SelectValsMin value selector.
 *	A call to \a goal is made before each choice point.
 */
template<class View>
Goal label(Store& store,const View& v, Goal goal)
{	return new (store)
		Detail::LabelWithCall<typename Casper::Traits::GetEval<View>::Type,View>(store,v,goal);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the \a varSel variable selector, and
 *	values are selected using the SelectValsMin value selector.
 *	A call to \a goal is made before each choice point.
 */
template<class View>
Goal label(Store& store,const View& v, Goal goal, VarSelector varSel)
{	return new (store)
		Detail::LabelWithCall<typename Casper::Traits::GetEval<View>::Type,View>(store,v,goal,varSel);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the SelectVarLex variable selector, and
 *	values are selected using the \a valSel value selector.
 *	A call to \a goal is made before each choice point.
 */
template<class View>
Goal label(Store& store,const View& v, Goal goal,ValSelector valSel)
{	return new (store)
		Detail::LabelWithCall<typename Casper::Traits::GetEval<View>::Type,View>(store,v,goal,valSel);	}

/**
*   Labels all variables in \a s.
 *	Variables are selected using the varSel variable selector, and
 *	values are selected using the valSel value selector.
 *	A call to \a goal is made before each choice point.
 */
template<class View>
Goal label(Store& store,const View& v, Goal goal,VarSelector varSel, ValSelector valSel)
{	return new (store)
		Detail::LabelWithCall<typename Casper::Traits::GetEval<View>::Type,View>(store,v,goal,varSel,valSel);	}
//@}

namespace Detail {
template<class T,class View>
struct Label<Seq<T>,View> : IGoal
{
	Label(Store& store, const View& vars) :
		store(store),
		doms(store,vars),
		varSel(selectVarLex(store,vars)),valSel(selectValsMin(store,vars)) {}
	Label(Store& store, const View& vars, VarSelector varSel) :
		store(store),
		doms(store,vars),
		varSel(varSel),valSel(selectValsMin(store,vars)) {}
	Label(Store& store, const View& vars, ValSelector valSel) :
		store(store),
		doms(store,vars),
		varSel(selectVarLex(store,vars)),valSel(valSel) {}
	Label(Store& store, const View& vars,
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
	DomArrayView<T,View> doms;
	VarSelector varSel;
	ValSelector valSel;
};

// same as previous but where a given Goal is called before each decision
template<class T,class View>
struct LabelWithCall<Seq<T>,View> : IGoal
{
	LabelWithCall(Store& store, const View& vars, const Goal& call) :
		store(store),
		doms(store,vars),call(call),
		varSel(selectVarLex(store,vars)),valSel(selectValsMin(store,vars)) {}
	LabelWithCall(Store& store, const View& vars, const Goal& call, VarSelector varSel) :
		store(store),
		doms(store,vars),call(call),
		varSel(varSel),valSel(selectValsMin(store,vars)) {}
	LabelWithCall(Store& store, const View& vars, const Goal& call,ValSelector valSel) :
		store(store),
		doms(store,vars),call(call),
		varSel(selectVarLex(store,vars)),valSel(valSel) {}
	LabelWithCall(Store& store, const View& vars, const Goal& call,
						VarSelector varSel,ValSelector valSel) :
		store(store),
		doms(store,vars),call(call),
		varSel(varSel),valSel(valSel) {}

	Goal execute()
	{
		int idx = varSel.select();
		if (idx < 0)
			return succeed();
		if (doms[idx]->ground())
			return this;
		return Goal(store,call and valSel.select(idx) and Goal(this));
	}

	Store&	store;
	DomArrayView<T,View> doms;
	Goal call;
	VarSelector varSel;
	ValSelector valSel;
};
};

} // CP
} // Casper

#endif /*_H_CASPER_KERNEL_GOAL_LABELING */
