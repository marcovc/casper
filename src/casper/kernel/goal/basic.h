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

#ifndef _H_CASPER_KERNEL_GOAL_BASIC
#define _H_CASPER_KERNEL_GOAL_BASIC

#include <casper/kernel/goal/common.h>
//#include <casper/kernel/filter/mutfilter.h>

namespace casper {

// undefined goals are mapped to filters

template<class T>
struct GoalView : IGoal // default is to try to wrap p as filter
{
	GoalView(ICPSolver& s,const T& p) :
		IGoal(s),p(p) {}
	Goal execute(IGoalSched&)
	{
		//if (!this->solver().post(f.instantiate(solver())))
		if (!this->solver().post(p))
			return fail();
		else
			return ok();
	}
	T	p;
	//MutFilter f;
};

template<class F,class E1,class V1>
struct GoalView1 : GoalView<Rel1<F,V1> >
{
	GoalView1(ICPSolver& s,const V1& p1) :
		GoalView<Rel1<F,V1> >(s,Rel1<F,V1>(p1)) {}
};

template<class F,class E1,class V1,class E2,class V2>
struct GoalView2 : GoalView<Rel2<F,V1,V2> >
{
	GoalView2(ICPSolver& s,const V1& p1,const V2& p2) :
		GoalView<Rel2<F,V1,V2> >(s,Rel2<F,V1,V2>(p1,p2)) {}
};

template<class F,class E1,class V1,class E2,class V2,class E3,class V3>
struct GoalView3 : GoalView<Rel3<F,V1,V2,V3> >
{
	GoalView3(ICPSolver& s,const V1& p1, const V2& p2, const V3& p3) :
		GoalView<Rel3<F,V1,V2,V3> >(s,Rel3<F,V1,V2,V3>(p1,p2,p3)) {}
};

template<class F,class E1,class V1,class E2,class V2,class E3,class V3,class E4,class V4>
struct GoalView4 : GoalView<Rel4<F,V1,V2,V3,V4> >
{
	GoalView4(ICPSolver& s,const V1& p1, const V2& p2, const V3& p3,const V4& p4) :
		GoalView<Rel4<F,V1,V2,V3,V4> >(s,Rel4<F,V1,V2,V3,V4>(p1,p2,p3,p4)) {}
};

/*
template<class F,class T1>
struct GoalView<Rel1<F,T1> > :
	public GoalView1<F,typename EvalTraits<T1>::Eval,T1>
{
	GoalView(ICPSolver& s,const Rel1<F,T1>& r) :
		GoalView1<F,typename EvalTraits<T1>::Eval,T1>(s,r.p1) {}
};

template<class F,class T1,class T2>
struct GoalView<Rel2<F,T1,T2> > :
	public GoalView2<F,typename EvalTraits<T1>::Eval,T1,
					   typename EvalTraits<T2>::Eval,T2>
{
	GoalView(ICPSolver& s,const Rel2<F,T1,T2>& r) :
		GoalView2<F,typename EvalTraits<T1>::Eval,T1,
			    	typename EvalTraits<T2>::Eval,T2>(s,r.p1,r.p2) {}
};

template<class F,class T1,class T2,class T3>
struct GoalView<Rel3<F,T1,T2,T3> > :
	public GoalView3<F,typename EvalTraits<T1>::Eval,T1,
					   typename EvalTraits<T2>::Eval,T2,
					   typename EvalTraits<T3>::Eval,T3>
{
	GoalView(ICPSolver& s,const Rel3<F,T1,T2,T3>& r) :
		GoalView3<F,typename EvalTraits<T1>::Eval,T1,
					typename EvalTraits<T2>::Eval,T2,
					typename EvalTraits<T3>::Eval,T3>(s,r.p1,r.p2,r.p3) {}
};*/

/**
	Goal conjunction.
*/
template<class View1,class View2>
struct GoalView2<And,Bool,View1,Bool,View2> : IGoal
{
	GoalView2(ICPSolver& s,const View1& p1, const View2& p2) :
		IGoal(s),g1(p1),g2(p2) {}

	Goal execute(IGoalSched& sched)
	{	sched.pushAnd(g2); sched.pushAnd(g1); return (IGoal*)NULL;	}

	Goal g1;
	Goal g2;
};

// Utilites for flatten And trees (making all subtrees branch from root)
// Case 1/3:
template<class View1,class View2,class View3>
struct GoalView2<And,Bool,Rel2<And,View1,View2>,Bool,View3> : IGoal
{
	GoalView2(ICPSolver& s,const Rel2<And,View1,View2>& p1, const View3& p2) :
		IGoal(s),g1(p1),g2(p2) {}

	Goal execute(IGoalSched& sched)
	{	sched.pushAnd(g2); g1.execute(sched); return (IGoal*)NULL;	}

	Goal g1;
	Goal g2;
};

// Case 2/3:
template<class View1,class View2,class View3>
struct GoalView2<And,Bool,View1,Bool,Rel2<And,View2,View3> > : IGoal
{
	GoalView2(ICPSolver& s,const View1& p1, const Rel2<And,View2,View3>& p2) :
		IGoal(s),g1(p1),g2(p2) {}

	Goal execute(IGoalSched& sched)
	{	g2.execute(sched); sched.pushAnd(g1); return (IGoal*)NULL;	}

	Goal g1;
	Goal g2;
};

// Case 3/3: (because compiler doesn't like ambiguity)
template<class View1,class View2,class View3,class View4>
struct GoalView2<And,Bool,Rel2<And,View1,View2>,
					 Bool,Rel2<And,View3,View4> > : IGoal
{
	GoalView2(ICPSolver& s,const Rel2<And,View1,View2>& p1,
					   const Rel2<And,View3,View4>& p2) :
		IGoal(s),g1(p1),g2(p2) {}

	Goal execute(IGoalSched& sched)
	{	g2.execute(sched); g1.execute(sched); return (IGoal*)NULL;	}

	Goal g1;
	Goal g2;
};

/**
	Goal disjunction (Andorra).
*/
template<class View1,class View2>
struct GoalView2<Or,Bool,View1,Bool,View2> : IGoal
{
	typedef GoalView2<Or,Bool,View1,Bool,View2>	Self;

	GoalView2(ICPSolver& s,const View1& p1, const View2& p2) :
		IGoal(s),g1(p1),g2(p2) {}

  	Goal execute(IGoalSched& sched)
	{
		sched.pushOr(g1,g2);
		return (IGoal*)NULL;
	}

	Goal			g1;
	Goal			g2;
};


};

#endif /*_H_CASPER_KERNEL_GOAL_BASIC */
