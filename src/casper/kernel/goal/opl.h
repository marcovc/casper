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

#include <casper/kernel/goal/common.h>
#include <casper/kernel/goal/util.h>
#include <casper/kernel/goal/ordering.h>
#include <casper/kernel/container/range.h>
#include <casper/kernel/view/iteration.h>
namespace casper {

template<class,class,class,class> struct IteratorView2;
template<class,class,class,class,class,class> struct IteratorView3;

template<>
struct IteratorView2<Int,MutVar<Int>,Seq<Int>,Range<Int> >
{
	IteratorView2(ICPSolver& s,const MutVar<Int>& it, const Range<Int>& r) :
		it(it),r(r) {}
	Void 	reset()	{	it = r.min();	}
	Bool	valid()	{	return it.value() >= r.min() && it.value() <= r.max();	}
	Void	next()	{	++it;	}
	MutVar<Int> it;
	Range<Int>	r;
};

NEW_REL_3(select,Select)

template<class Eval1,class View1,class Eval2,class View2,class View3>
struct GoalView3<Select,Eval1,View1,Eval2,View2,Bool,View3> : IGoal
{
	GoalView3(ICPSolver& s, const View1& v1, const View2& v2, const View3& v3) :
		IGoal(s),it(v1),r(v2),e(s,v3) {}
    Goal execute()
	{
		IteratorView2<Eval1,View1,Eval2,View2> mt(solver(),it,r);
		for ( mt.reset(); mt.valid(); mt.next() )
			if (e.value())
				return ok();
		return fail();
	}
	View1 it;
	View2 r;
	MutExpr<Bool> e;
};

/*
NEW_REL_4(Select,select)

template<class Eval1,class View1,class Eval2,class View2,
		 class View3,class Eval4,class View4>
struct GoalView4<Select,Eval1,View1,Eval2,View2,Bool,View3,Eval4,View4> : IGoal
{
	GoalView4(ICPSolver& s, const View1& v1, const View2& v2,
						const View3& v3, const View4& v4) :
		IGoal(s),it(v1),r(v2),e(s,v3),o(v4) {}
    Goal execute()
	{
		IteratorView3<Eval1,View1,Eval2,View2,Eval4,View4> mt(solver(),it,
																r,o.getCRef());
		for ( mt.reset(); mt.valid(); mt.next() )
			if (e)
				return ok();
		return fail();
	}
	View1 it;
	View2 r;
	MutExpr<Bool> e;
	Persist<View4> o;
};*/


template<class T1>
struct SelectInRange : public IGoal
{
	SelectInRange(MutVar<T1> v, T1 min, T1 max, MutExpr<Bool> e ) :
		v(v), min(min), max(max), e(e) {}

    Goal execute()
	{
		for ( v = min; v <= max; ++v)
			if (e)
				return ok();
		return fail();
	}

	MutVar<T1> v;
	T1 min;
	T1 max;
	MutExpr<Bool> e;
};

template<class T1,class T2>
struct SelectInRangeOrder : public IGoal
{
	SelectInRangeOrder(MutVar<T1> v, T1 min, T1 max,
					   MutExpr<Bool> e, MutExpr<T2> o) :
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
		return ok();
	}

	MutVar<T1> v;
	T1 min;
	T1 max;
	MutExpr<Bool> e;
	MutExpr<T2> o;
};

template<class T1>
Goal	selectInc(MutVar<T1> v, T1 min, T1 max, MutExpr<Bool> e)
{	return new SelectInRange<T1>(v,min,max,e);	}

template<class T1>
Goal	selectDec(MutVar<T1> v, T1 min, T1 max, MutExpr<Bool> e)
{	return new SelectInRangeOrder<T1,T1>(v,min,max,e,-v);	}

template<class T1>
Goal	selectInc(MutVar<T1> v, T1 min, T1 max, MutExpr<Bool> e, MutExpr<Int> o)
{	return new SelectInRangeOrder<T1,Int>(v,min,max,e,o);	}
/*
template<class T1>
Goal	selectDec(MutVar<T1> v, T1 min, T1 max, MutExpr<Bool> e, MutExpr<Int> o)
{	return new SelectInRangeOrder<T1,Int>(v,min,max,e,-o);	}
*/

NEW_REL_3(forAll,ForAll)

template<class T1,class T2,class T3>
struct EvalTraits<Rel3<ForAll,T1,T2,T3> >
{	typedef	Bool	Eval;	};

template<class T,class Iter,class View>
struct GoalView3<ForAll,T,MutVar<T>,Seq<T>,Iter,Bool,View> : IGoal
{
	GoalView3(ICPSolver& s, const MutVar<T>& it,
				const Iter& r, const View& v) :
		IGoal(s),it(it),r(r),iter(this->r),v(v) {}
    Goal execute()
	{
		if (!iter.valid())
			return ok();
		it = iter.value();
		return v and forAll(it,iter.next(),v);
	}
	MutVar<T> it;
	Iter r;
	//IterationView<T,Iter>	iter;
	IterationView<Iter>	iter;
	View v;
};




NEW_REL_3(tryAll,TryAll)

template<class T1,class T2,class T3>
struct EvalTraits<Rel3<TryAll,T1,T2,T3> >
{	typedef	Bool	Eval;	};


template<class T,class Iter,class View>
struct GoalView3<TryAll,T,MutVar<T>,Seq<T>,Iter,Bool,View> : IGoal
{
	GoalView3(ICPSolver& s, const MutVar<T>& it, const Iter& r, const View& v) :
		IGoal(s),it(it),r(r),
		iter(this->r),
		v(v)
		{}
    Goal execute()
	{
		if (!iter.valid())
			return fail();

		it = iter.value();

		return v or tryAll(it,iter.next(),v);
	}
	MutVar<T> 				it;
	Iter					r;
	//IterationView<T,Iter>	iter;
	IterationView<Iter>	iter;
	View					v;
};


NEW_REL_4(tryAll,TryAll)

template<class T1,class T2,class T3,class T4>
struct EvalTraits<Rel4<TryAll,T1,T2,T3,T4> >
{	typedef	Bool	Eval;	};

// FIXME: must create a copy of this goal in every branch as above
template<class T,class Iter,class View1,class View2>
struct GoalView4<TryAll,T,MutVar<T>,Seq<T>,Iter,
				Bool,View1,Bool,View2> : IGoal
{
	GoalView4(ICPSolver& s, const MutVar<T>& it,
				const Iter& r, const View1& v1,const View2& v2) :
		IGoal(s),it(it),r(r),iter(this->r),v1(v1),v2(v2) {}
    Goal execute()
	{
		if (!iter.valid())
			return fail();
		it = iter.value();
		iter.iterate();
		return v1 or (v2 and Goal(this));
	}
	MutVar<T> it;
	Iter					r;
	//IterationView<T,Iter>	iter;
	IterationView<Iter>	iter;
	View1	v1;
	View2	v2;
};

};

#endif /*_H_CASPER_KERNEL_GOAL_OPL */
