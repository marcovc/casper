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

/** \file
 * 	Goal objects. Goals are the mechanism for performing
 * (nondeterministic) search.
 *	\sa Explorer
 */

#ifndef _H_CASPER_KERNEL_GOAL_COMMON
#define _H_CASPER_KERNEL_GOAL_COMMON

#include <casper/kernel/notify/notifiable.h>
#include <casper/kernel/rel/rel.h>
//#include <casper/kernel/par/parexpr.h>
#include <casper/kernel/common.h>
#include <casper/util/pimpl.h>
#include <assert.h>

#include <functional>

//#include <casper/kernel/goal/explorer.h>

namespace Casper {

struct IExplorer;

/** \defgroup Search Search
 *
 * (Nondeterministic) search is accomplished with Goal objects.
 *	\sa Goal,Explorer
 *	\ingroup Kernel
 */

struct IGoal;
struct Goal;
struct State;

struct IGoal;

/**
 * 	\ingroup Search
 * 	This goal succeeds when evaluated - use it to signal success.
 */
Goal	succeed();

/**
 * 	\ingroup Search
 * 	This goal fails when evaluated - use it to signal failure.
 */
Goal	fail();

template<class>
struct GoalView;
template<class,class,class>
struct GoalView1;
template<class,class,class,class,class>
struct GoalView2;
template<class,class,class,class,class,class,class>
struct GoalView3;
template<class,class,class,class,class,class,class,class,class>
struct GoalView4;

/**
 * 	Wrapper over an IGoal object.
 *	\ingroup Search
 */
struct Goal : Util::PImplIdiom<IGoal>
{
	typedef Goal				Self;
	typedef Util::PImplIdiom<IGoal> 	Super;

	Goal(IGoal* g) : Super(g) {}

	// 'explicit' below is needed to avoid creating ambiguity with an
	// overload of && defined in the STL (at least with gcc)
	explicit Goal(bool cond) : Super(cond?succeed():fail()) {}
	Goal(const Goal& g) : Super(g.getPImpl())	{}

	Goal(State&,const Goal& g) : Super(g.getPImpl())	{}

	template<class Func,class T1>
	Goal(State& s,const Rel1<Func,T1>& r) :
		Super(new (s)
			GoalView1<Func,
			typename Traits::GetEval<T1>::Type,T1>(s,r.p1)) {}

	template<class Func,class T1,class T2>
	Goal(State& s,const Rel2<Func,T1,T2>& r) :
		Super(new (s)
			GoalView2<Func,
			typename Traits::GetEval<T1>::Type,T1,
			typename Traits::GetEval<T2>::Type,T2>(s,r.p1,r.p2)) {}

	template<class Func,class T1,class T2,class T3>
	Goal(State& s,const Rel3<Func,T1,T2,T3>& r) :
		Super(new (s)
			GoalView3<Func,
			typename Traits::GetEval<T1>::Type,T1,
			typename Traits::GetEval<T2>::Type,T2,
			typename Traits::GetEval<T3>::Type,T3>(s,r.p1,r.p2,r.p3)) {}

	template<class Func,class T1,class T2,class T3,class T4>
	Goal(State& s,const Rel4<Func,T1,T2,T3,T4>& r) :
		Super(new (s)
			GoalView4<Func,
			typename Traits::GetEval<T1>::Type,T1,
			typename Traits::GetEval<T2>::Type,T2,
			typename Traits::GetEval<T3>::Type,T3,
			typename Traits::GetEval<T4>::Type,T4>(s,r.p1,r.p2,r.p3,r.p4)) {}

	Goal(State& s, const std::function<Goal()>& fn);
	Goal(State& s, const std::function<void()>& fn);

	inline Goal execute(IExplorer& expl);
	//inline Goal create();
	//inline CPSolver& solver() const;
};


namespace Traits {
template<>
struct GetEval<Goal>
{	typedef bool	Type;	};
} // Traits

struct OK;
struct Fail;

struct ISearchPath
{
	virtual uint getFailCount() const = 0;
	virtual uint getDepth() const = 0;
	virtual uint getRightDepth() const = 0;
	virtual double getEvaluation() const = 0;
	virtual ~ISearchPath() {}
};


/**
 * 	A Goal represents a nondeterministic sequence of actions (or ,
 * 	more realistically, a tree of actions),	and is used
 *  to define nondeterministic search. This class defines the
 *  interface of a Goal, essentially a function object with a single method
 *  called execute() which returns a new Goal.
 *  \ingroup Search
 */
struct IGoal
{
	IGoal() {}
	virtual ~IGoal() {}
	/// Executes the current Goal, which returns a new Goal as the result.
	virtual Goal execute() { assert(0); return (IGoal*)NULL; }
	virtual Goal execute(IExplorer&) { return execute(); }

	private:
	friend struct OK;
	friend struct Fail;
};

Goal Goal::execute(IExplorer& sched)
{ return Super::pImpl->execute(sched); }

#if 0
Goal succeeds(Goal g);
Goal fails(Goal g);
#endif

};

#endif

#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_KERNEL_GOAL_OP_GOAL_H
#define CASPER_KERNEL_GOAL_OP_GOAL_H
#include <casper/kernel/goal/op_goal.h>
#endif
#endif
