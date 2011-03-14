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

#ifndef _H_CASPER_KERNEL_GOAL_OPL
#define _H_CASPER_KERNEL_GOAL_OPL

#include <casper/kernel/goal/goal.h>
#include <casper/kernel/goal/util.h>
#include <casper/kernel/goal/ordering.h>
#include <casper/util/container/stdrange.h>
#include <casper/kernel/view/iteration.h>

namespace Casper {

#if 0
template<class,class,class,class> struct IteratorView2;
template<class,class,class,class,class,class> struct IteratorView3;

template<>
struct IteratorView2<int,Par<int>,Seq<int>,StdRange<int> >
{
	IteratorView2(CPSolver& s,const Par<int>& it, const StdRange<int>& r) :
		it(it),r(r) {}
	void 	reset()	{	it = r.min();	}
	bool	valid()	{	return it.value() >= r.min() && it.value() <= r.max();	}
	void	next()	{	++it;	}
	Par<int> it;
	StdRange<int>	r;
};

NEW_REL_3(select,Select)

template<class Eval1,class View1,class Eval2,class View2,class View3>
struct GoalView3<Select,Eval1,View1,Eval2,View2,bool,View3> : IGoal
{
	GoalView3(CPSolver& s, const View1& v1, const View2& v2, const View3& v3) :
		IGoal(s),it(v1),r(v2),e(s,v3) {}
    Goal execute()
	{
		IteratorView2<Eval1,View1,Eval2,View2> mt(solver(),it,r);
		for ( mt.reset(); mt.valid(); mt.next() )
			if (e.value())
				return succeed();
		return fail();
	}
	View1 it;
	View2 r;
	MutExpr<bool> e;
};

/*
NEW_REL_4(Select,select)

template<class Eval1,class View1,class Eval2,class View2,
		 class View3,class Eval4,class View4>
struct GoalView4<Select,Eval1,View1,Eval2,View2,bool,View3,Eval4,View4> : IGoal
{
	GoalView4(CPSolver& s, const View1& v1, const View2& v2,
						const View3& v3, const View4& v4) :
		IGoal(s),it(v1),r(v2),e(s,v3),o(v4) {}
    Goal execute()
	{
		IteratorView3<Eval1,View1,Eval2,View2,Eval4,View4> mt(solver(),it,
																r,o.getCRef());
		for ( mt.reset(); mt.valid(); mt.next() )
			if (e)
				return succeed();
		return fail();
	}
	View1 it;
	View2 r;
	MutExpr<bool> e;
	Persist<View4> o;
};*/


template<class T1>
struct SelectInRange : public IGoal
{
	SelectInRange(Par<T1> v, T1 min, T1 max, MutExpr<bool> e ) :
		v(v), min(min), max(max), e(e) {}

    Goal execute()
	{
		for ( v = min; v <= max; ++v)
			if (e)
				return succeed();
		return fail();
	}

	Par<T1> v;
	T1 min;
	T1 max;
	MutExpr<bool> e;
};

template<class T1,class T2>
struct SelectInRangeOrder : public IGoal
{
	SelectInRangeOrder(Par<T1> v, T1 min, T1 max,
					   MutExpr<bool> e, MutExpr<T2> o) :
		v(v), min(min), max(max), e(e), o(o) {}

    Goal execute()
	{
		bool first = true;
		T1 minV;
		T2 minOrd;
		for ( v = min; v <= max; ++v)
			if (e && (first || o < minOrd))
			{
				minV = v;
				minOrd = o;
				first = false;
			}
		if (first)
			return fail();
		v = minV;
		return succeed();
	}

	Par<T1> v;
	T1 min;
	T1 max;
	MutExpr<bool> e;
	MutExpr<T2> o;
};

template<class T1>
Goal	selectInc(Par<T1> v, T1 min, T1 max, MutExpr<bool> e)
{	return new SelectInRange<T1>(v,min,max,e);	}

template<class T1>
Goal	selectDec(Par<T1> v, T1 min, T1 max, MutExpr<bool> e)
{	return new SelectInRangeOrder<T1,T1>(v,min,max,e,-v);	}

template<class T1>
Goal	selectInc(Par<T1> v, T1 min, T1 max, MutExpr<bool> e, MutExpr<int> o)
{	return new SelectInRangeOrder<T1,int>(v,min,max,e,o);	}
/*
template<class T1>
Goal	selectDec(Par<T1> v, T1 min, T1 max, MutExpr<bool> e, MutExpr<int> o)
{	return new SelectInRangeOrder<T1,int>(v,min,max,e,-o);	}
*/

#endif


};

#endif /*_H_CASPER_KERNEL_GOAL_OPL */
