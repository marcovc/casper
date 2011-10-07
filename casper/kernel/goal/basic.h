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

#ifndef _H_CASPER_KERNEL_GOAL_BASIC
#define _H_CASPER_KERNEL_GOAL_BASIC

#include <casper/kernel/goal/goal.h>
#include <casper/kernel/goal/explorer.h>
//#include <casper/kernel/filter/mutfilter.h>
#include <casper/kernel/par/par.h>
#include <casper/kernel/par/piteration.h>

namespace Casper {

// FIXME: Why this? disabling...
#if 0
template<class T>
struct GoalView;

template<class F,class E1,class V1>
struct GoalView1 : GoalView<Rel1<F,V1> >
{
	GoalView1(CPSolver& s,const V1& p1) :
		GoalView<Rel1<F,V1> >(s,Rel1<F,V1>(p1)) {}
};

template<class F,class E1,class V1,class E2,class V2>
struct GoalView2 : GoalView<Rel2<F,V1,V2> >
{
	GoalView2(CPSolver& s,const V1& p1,const V2& p2) :
		GoalView<Rel2<F,V1,V2> >(s,Rel2<F,V1,V2>(p1,p2)) {}
};

template<class F,class E1,class V1,class E2,class V2,class E3,class V3>
struct GoalView3 : GoalView<Rel3<F,V1,V2,V3> >
{
	GoalView3(CPSolver& s,const V1& p1, const V2& p2, const V3& p3) :
		GoalView<Rel3<F,V1,V2,V3> >(s,Rel3<F,V1,V2,V3>(p1,p2,p3)) {}
};

template<class F,class E1,class V1,class E2,class V2,class E3,class V3,class E4,class V4>
struct GoalView4 : GoalView<Rel4<F,V1,V2,V3,V4> >
{
	GoalView4(CPSolver& s,const V1& p1, const V2& p2, const V3& p3,const V4& p4) :
		GoalView<Rel4<F,V1,V2,V3,V4> >(s,Rel4<F,V1,V2,V3,V4>(p1,p2,p3,p4)) {}
};
#endif

/*
template<class F,class T1>
struct GoalView<Rel1<F,T1> > :
	public GoalView1<F,typename Traits::GetEval<T1>::Type,T1>
{
	GoalView(CPSolver& s,const Rel1<F,T1>& r) :
		GoalView1<F,typename Traits::GetEval<T1>::Type,T1>(s,r.p1) {}
};

template<class F,class T1,class T2>
struct GoalView<Rel2<F,T1,T2> > :
	public GoalView2<F,typename Traits::GetEval<T1>::Type,T1,
					   typename Traits::GetEval<T2>::Type,T2>
{
	GoalView(CPSolver& s,const Rel2<F,T1,T2>& r) :
		GoalView2<F,typename Traits::GetEval<T1>::Type,T1,
			    	typename Traits::GetEval<T2>::Type,T2>(s,r.p1,r.p2) {}
};

template<class F,class T1,class T2,class T3>
struct GoalView<Rel3<F,T1,T2,T3> > :
	public GoalView3<F,typename Traits::GetEval<T1>::Type,T1,
					   typename Traits::GetEval<T2>::Type,T2,
					   typename Traits::GetEval<T3>::Type,T3>
{
	GoalView(CPSolver& s,const Rel3<F,T1,T2,T3>& r) :
		GoalView3<F,typename Traits::GetEval<T1>::Type,T1,
					typename Traits::GetEval<T2>::Type,T2,
					typename Traits::GetEval<T3>::Type,T3>(s,r.p1,r.p2,r.p3) {}
};*/

/**
	Goal conjunction. Executes \a p1 and if it succeeds then
	executes \a p2. Fails if any of them fails, succeeds otherwise.
	\ingroup Search
*/
template<class Expr1,class Expr2>
struct GoalView2<And,bool,Expr1,bool,Expr2> : IGoal
{
	GoalView2(State& s,const Expr1& p1, const Expr2& p2) :
		g1(s,p1),g2(s,p2) {}

	Goal execute(IExplorer& sched)
	{	sched.pushAnd(g2); sched.pushAnd(g1); return (IGoal*)NULL;	}

	Goal g1;
	Goal g2;
};

// Utilites for flatten And trees (making all subtrees branch from root)
// Case 1/3:
template<class Expr1,class Expr2,class Expr3>
struct GoalView2<And,bool,Rel2<And,Expr1,Expr2>,bool,Expr3> : IGoal
{
	GoalView2(State& s,const Rel2<And,Expr1,Expr2>& p1, const Expr3& p2) :
		g1(s,p1),g2(s,p2) {}

	Goal execute(IExplorer& sched)
	{	sched.pushAnd(g2); g1.execute(sched); return (IGoal*)NULL;	}

	Goal g1;
	Goal g2;
};

// Case 2/3:
template<class Expr1,class Expr2,class Expr3>
struct GoalView2<And,bool,Expr1,bool,Rel2<And,Expr2,Expr3> > : IGoal
{
	GoalView2(State& s,const Expr1& p1, const Rel2<And,Expr2,Expr3>& p2) :
		g1(s,p1),g2(s,p2) {}

	Goal execute(IExplorer& sched)
	{	g2.execute(sched); sched.pushAnd(g1); return (IGoal*)NULL;	}

	Goal g1;
	Goal g2;
};

// Case 3/3: (because compiler doesn't like ambiguity)
template<class Expr1,class Expr2,class Expr3,class Expr4>
struct GoalView2<And,bool,Rel2<And,Expr1,Expr2>,
					 bool,Rel2<And,Expr3,Expr4> > : IGoal
{
	GoalView2(State& s,const Rel2<And,Expr1,Expr2>& p1,
					   const Rel2<And,Expr3,Expr4>& p2) :
		g1(s,p1),g2(s,p2) {}

	Goal execute(IExplorer& sched)
	{	g2.execute(sched); g1.execute(sched); return (IGoal*)NULL;	}

	Goal g1;
	Goal g2;
};

/**
	Goal disjunction (branching). Executes \a p1 and if it fails then
	executes \a p2. Succeeds if any of them succeeds, fails otherwise.
	\ingroup Search
*/
template<class Expr1,class Expr2>
struct GoalView2<Or,bool,Expr1,bool,Expr2> : IGoal
{
	typedef GoalView2<Or,bool,Expr1,bool,Expr2>	Self;

	GoalView2(State& s,const Expr1& p1, const Expr2& p2) :
		g1(s,p1),g2(s,p2) {}

  	Goal execute(IExplorer& sched)
	{
		sched.pushOr(g1,g2);
		return (IGoal*)NULL;
	}

	Goal			g1;
	Goal			g2;
};

/**
	Assigns the evaluation of v2 to v1. Always succeeds.
	\ingroup Search
*/
#if 0
template<class Expr1,class Expr2,class Eval>
struct GoalView2<Assign,Eval,Expr1,Eval,Expr2> : IGoal
{
	GoalView2(State& s,const Expr1& v1, const Expr2& v2) :
		s(s),v1(v1),v2(v2) {}

  	Goal execute()
	{
		v1 = ParView<Eval,Expr2>(s,v2).value();
		return succeed();
	}

  	State&			s;
	Expr1			v1;
	Expr2			v2;
};
#else
template<class Expr1,class Expr2,class Eval>
struct GoalView2<Assign,Eval,Expr1,Eval,Expr2> : IGoal
{
	GoalView2(State& s,const Expr1& v1, const Expr2& v2) :
		s(s),v1(v1),v2(v2) {}

  	Goal execute()
	{
		Par<Eval>(s,v1).setValue(Par<Eval>(s,v2).value());
		return succeed();
	}

  	State&			s;
	Expr1			v1;
	Expr2			v2;
};
#endif

/**
	Assigns the first value in the set \a s to \a p. Fails
	if \a s is empty, succeeds otherwise.
	\ingroup Search
*/
template<class Set,class Eval>
struct GoalView2<SelectFirst,Eval,Par<Eval>,Seq<Eval>,Set> : IGoal
{
	GoalView2(State& state,const Par<Eval>& p, const Set& s) :
		state(state),p(p),s(s) {}

  	Goal execute()
	{
		Casper::Detail::PIteration<Par<Eval>,Set,bool> it(p,s,true);
		if (it.valid())
			return succeed();
		return fail();
	}

  	State&					state;
	Par<Eval>				p;
	Set						s;
};

/**
	Assigns the first value in the set \a s to \a p for which \a cond is \p true. Fails
	if no value is found, succeeds otherwise.
	\ingroup Search
*/
template<class Set,class Eval,class Cond>
struct GoalView3<SelectFirst,Eval,Par<Eval>,Seq<Eval>,Set,bool,Cond> : IGoal
{
	GoalView3(State& state,const Par<Eval>& p, const Set& s, const Cond& cond) :
		state(state),p(p),s(s),cond(cond) {}

  	Goal execute()
	{
		Casper::Detail::PIteration<Par<Eval>,Set,bool> it(p,s,cond);
		if (it.valid())
			return succeed();
		return fail();
	}

  	State&					state;
	Par<Eval>				p;
	Set						s;
	Cond					cond;
};

};

#endif /*_H_CASPER_KERNEL_GOAL_BASIC */
