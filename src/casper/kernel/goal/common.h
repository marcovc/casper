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

/** \file
 * 	Goal objects. Goals are the mechanism for performing
 * (nondeterministic) search.
 *	\sa GoalScheduler
 */

#ifndef _H_CASPER_KERNEL_GOAL_COMMON
#define _H_CASPER_KERNEL_GOAL_COMMON

#include <casper/kernel/notifiable.h>
#include <casper/kernel/relation.h>
#include <casper/kernel/expression.h>
#include <casper/kernel/common.h>
#include <casper/kernel/domvararray.h>
#include <casper/kernel/container/rarray.h>
#include <casper/kernel/solver.h>

namespace casper {

/** \defgroup Search Search
 *
 * (Nondeterministic) search is accomplished with Goal objects.
 *	\sa Goal,GoalScheduler
 */

/**
 * 	\defgroup Goals Goals
 *  \ingroup Search
 *
 *  Predefined Goal objects.
 *
 */

struct IGoal;
struct Goal;
struct ICPSolver;

struct IGoal;

/**
 * 	\ingroup Goals
 * 	This goal succeeds when evaluated - use it to signal success.
 */
inline Goal	ok();

/**
 * 	\ingroup Goals
 * 	This goal fails when evaluated - use it to signal failure.
 */
inline Goal	fail();

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
 *
 */
struct Goal : detail::PImplIdiom<IGoal>
{
	typedef Goal				Self;
	typedef detail::PImplIdiom<IGoal> 	Super;

	Goal(IGoal* g) : Super(g) {}
	Goal(Filter f);
	Goal(Bool cond) : Super(cond?ok():fail()) {}
	Goal(const Goal& g) : Super(g.getPImpl())	{}

	template<class Func,class T1>
	Goal(const Rel1<Func,T1>& r) :
		Super(new (getCPSolver(r).heap())
			GoalView1<Func,
			typename EvalTraits<T1>::Eval,T1>(getCPSolver(r),r.p1)) {}

	template<class Func,class T1,class T2>
	Goal(const Rel2<Func,T1,T2>& r) :
		Super(new (getCPSolver(r).heap())
			GoalView2<Func,
			typename EvalTraits<T1>::Eval,T1,
			typename EvalTraits<T2>::Eval,T2>(getCPSolver(r),r.p1,r.p2)) {}

	template<class Func,class T1,class T2,class T3>
	Goal(const Rel3<Func,T1,T2,T3>& r) :
		Super(new (getCPSolver(r).heap())
			GoalView3<Func,
			typename EvalTraits<T1>::Eval,T1,
			typename EvalTraits<T2>::Eval,T2,
			typename EvalTraits<T3>::Eval,T3>(getCPSolver(r),r.p1,r.p2,r.p3)) {}

	template<class Func,class T1,class T2,class T3,class T4>
	Goal(const Rel4<Func,T1,T2,T3,T4>& r) :
		Super(new (getCPSolver(r).heap())
			GoalView4<Func,
			typename EvalTraits<T1>::Eval,T1,
			typename EvalTraits<T2>::Eval,T2,
			typename EvalTraits<T3>::Eval,T3,
			typename EvalTraits<T4>::Eval,T4>(getCPSolver(r),r.p1,r.p2,r.p3,r.p4)) {}

	Goal(minicasper::MExpr);
	Goal(ICPSolver&,minicasper::MExpr);

	inline Goal execute(IGoalSched& sched);
	//inline Goal create();
	inline ICPSolver& solver() const;
};

template<>
struct GetCPSolver<Goal>
{	ICPSolver& operator()(Goal g)
	{ return g.solver(); }
};

template<>
struct EvalTraits<Goal>
{	typedef	Bool	Eval;	};


struct OK;
struct Fail;

struct ISearchPath
{
	virtual UInt getFailCount() const = 0;
	virtual UInt getDepth() const = 0;
	virtual UInt getRightDepth() const = 0;
	virtual Double getEvaluation() const = 0;
	virtual ~ISearchPath() {}
};

struct IGoalSchedStats
{};

struct IGoalSched
{
	IGoalSched(ICPSolver& mSolver);

	virtual void pushAnd(Goal) = 0;
	virtual void pushOr(Goal,Goal) = 0;
	//virtual Goal pop() = 0;
	//virtual Bool empty() = 0;
	virtual void restart() = 0;
	virtual const ISearchPath& getCurPath() const = 0;
	//virtual const IGoalSchedStats& getStats() const = 0;

	virtual Bool execute(Goal) = 0;
	virtual Bool next() = 0;
	virtual void fail() = 0;
	virtual ~IGoalSched() {}
	ICPSolver& solver() const {	return mSolver;	}
	virtual Void setFallBack(Bool value) = 0;
	ICPSolver& mSolver;
};

/**
 * 	A Goal represents a predicate or a logical composition of predicates, and is used
 *  to define nondeterministic search (a la Prolog). This class defines the
 *  interface of a Goal, essentially a function object with a single method
 *  called execute() which returns a new Goal.
 */
struct IGoal
{
	IGoal(ICPSolver& s) : mSolver(s) {}
	virtual ~IGoal() {}
	/// Calls the current Goal, which returns a new Goal as the result.
	virtual Goal execute() { assert(0); return this; }
	virtual Goal execute(IGoalSched&) { return execute(); }
	//virtual Goal create() { return this; }	// ok when goal does not have state
	ICPSolver& solver() const {	return mSolver;	}
	//virtual Bool isLowLevelCP() const { return false; }
	private:
	//IGoal() : mSolver(NULL) {}	// FIXME: this may be dangerous...
	ICPSolver& mSolver;
	friend struct OK;
	friend struct Fail;
};

Goal Goal::execute(IGoalSched& sched)
{ return Super::pImpl->execute(sched); }

//Goal Goal::create()
//{ return Super::pImpl->create(); }

ICPSolver& Goal::solver() const
{ return Super::pImpl->solver();	}

struct OK : IGoal
{
	OK() : IGoal(glbNullCPSolver) {}
	Goal execute(IGoalSched&) { return (IGoal*)NULL; }
};

struct Fail : IGoal
{
	Fail() : IGoal(glbNullCPSolver) {}
	Goal execute(IGoalSched& sched) {	sched.fail(); return (IGoal*)NULL; }
};

Goal	nopCP(ICPSolver& s);

IGoal* const pGlobalOKGoal(new OK());
IGoal* const pGlobalFailGoal(new Fail());


inline Goal	ok() 				{	return pGlobalOKGoal; }
inline Goal	fail() 				{	return pGlobalFailGoal; }

#if 0
Goal succeeds(Goal g);
Goal fails(Goal g);
#endif

};

#endif

namespace casper {
#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_KERNEL_MACRO_GOAL_H
#define CASPER_KERNEL_MACRO_GOAL_H
#include <casper/kernel/macro/goal.h>
#endif
#endif
}
