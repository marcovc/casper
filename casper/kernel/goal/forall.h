/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           	   *
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
 

#ifndef CASPER_KERNEL_GOAL_FORALL_H_
#define CASPER_KERNEL_GOAL_FORALL_H_

#include <casper/kernel/view/iteration.h>

namespace Casper {

NEW_REL_2(forAll,ForAll)
NEW_FN_3(forAll,ForAll)

namespace Traits {
template<class T1,class T2,class T3>
struct GetEval<Rel3<ForAll,T1,T2,T3> >
{	typedef	bool	Type;	};
} // Traits

/**
 * 	Executes \a v for all \a it in \a s. Succeeds if \a v
 * 	always succeeded, fails on the first \a it that fails \a v.
 * 	\ingroup Search
 */
template<class T,class Set,class Obj>
struct GoalView3<ForAll,T,Ref<T>,Seq<T>,Set,bool,Obj> : IGoal
{
	typedef GoalView3<ForAll,T,Ref<T>,Seq<T>,Set,bool,Obj> Self;
	GoalView3(State& state, const Ref<T>& it,
				const Set& s, const Obj& v) :
		state(state),it(it),pSet(new(state)Set(s)),iter(*pSet),v(v) {}
	GoalView3(State& state, const Ref<T>& it,
				const IterationView<Set>& iter, const Obj& v) :
		state(state),it(it),pSet(NULL),iter(iter),v(v) {}

    Goal execute()
	{
    	IterationView<Set> curIter(iter);

    	if (pSet==NULL) // i.e this is not the first iteration
    		curIter.iterate();

		if (!curIter.valid())
			return succeed();

		it = curIter.value();

		return Goal(state,
				rel<And>(v, Goal(new (state) Self(state,it,curIter,v))));
	}
    State& state;
	Ref<T> it;
	Set* pSet;	// IterationView protocol requires *pSet to exist during the full iteration
	IterationView<Set>	iter;
	Obj v;
};

NEW_FN_4(forAll,ForAll)

namespace Traits {
template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<ForAll,T1,T2,T3,T4> >
{	typedef	bool	Type;	};
} // Traits

/**
 * 	Executes \a v for all \a it in \a r for which \a c is true. Succeeds if \a v
 * 	always succeeded, fails on the first \a it that fails \a v.
 * 	\ingroup Search
 */
template<class T,class Set,class Cond,class Obj>
struct GoalView4<ForAll,T,Ref<T>,Seq<T>,Set,bool,Cond,bool,Obj> : IGoal
{
	typedef GoalView4<ForAll,T,Ref<T>,Seq<T>,Set,bool,Cond,bool,Obj> Self;
	GoalView4(State& state, const Ref<T>& it,const Set& s, const Cond& c,const Obj& v) :
		state(state),it(it),pSet(new(state)Set(s)),iter(*pSet),c(state,c),v(v) {}
	GoalView4(State& state, const Ref<T>& it, const IterationView<Set>& iter, const Cond& c, const Obj& v) :
		state(state),it(it),pSet(NULL),iter(iter),c(state,c),v(v) {}

    Goal execute()
	{
    	IterationView<Set> curIter(iter);

    	if (pSet==NULL) // i.e this is not the first iteration
    		curIter.iterate();

    	for ( ; curIter.valid(); curIter.iterate())
    	{
    		it = curIter.value();
    		if (c.value())
    			break;
    	}

		if (!curIter.valid())
			return succeed();

		return Goal(state,
				rel<And>(v, Goal(new (state) Self(state,it,curIter,c.getObj(),v))));
	}
    State& state;
	Ref<T> it;
	Set* pSet;	// IterationView protocol requires *pSet to exist during the full iteration
	IterationView<Set>	iter;
	RefView<bool,Cond> c;
	Obj v;
};

NEW_FN_5(forAll,ForAll)

namespace Traits {
template<class T1,class T2,class T3,class T4,class T5>
struct GetEval<Rel5<ForAll,T1,T2,T3,T4,T5> >
{	typedef	bool	Type;	};
} // Traits

/**
 * 	Executes \a v for all \a it in \a s for which \a c is true, sorted by increasing values of \a o. Succeeds if \a v
 * 	always succeeded, fails on the first \a it that fails \a v.
 * 	\ingroup Search
 */
template<class T,class Set,class Cond,class OrderT,class Order,class Obj>
struct GoalView5<ForAll,T,Ref<T>,Seq<T>,Set,bool,Cond,OrderT,Order,bool,Obj> : IGoal
{
	typedef GoalView5<ForAll,T,Ref<T>,Seq<T>,Set,bool,Cond,OrderT,Order,bool,Obj> Self;
	GoalView5(State& state, const Ref<T>& it,const Set& s, const Cond& c,const Order& o, const Obj& v) :
		state(state),it(it),pSet(new(state)Set(s)),iter(*pSet),c(state,c),o(state,o),v(v),
		pSelected(new (state) SList<int>(state)) {}
	GoalView5(State& state, const Ref<T>& it, const IterationView<Set>& iter, const Cond& c,
			const Order& o,const Obj& v,SList<int>* pSelected) :
		state(state),it(it),pSet(NULL),iter(iter),c(state,c),o(state,o),v(v),pSelected(pSelected) {}

	bool selected(int i)
	{
		for (auto itt = pSelected->begin(); itt != pSelected->end(); ++itt)
			if (*itt == i)
				return true;
		return false;
	}

    Goal execute()
	{
    	// find the it that minimizes o, satisfies c, and was not yet selected
    	int bestIt;
    	OrderT bestS = limits<OrderT>::posInf();
    	for (IterationView<Set> curIter(iter); curIter.valid(); curIter.iterate())
    	{
    		it = curIter.value();
    		if (c.value() and o.value() < bestS and !selected(it.value()))
    		{
    			bestS = o.value();
    			bestIt = it.value();
    		}
    	}

    	if (bestS == limits<OrderT>::posInf()) // i.e., not found
   			return succeed();

    	it = bestIt;

    	pSelected->pushFront(it.value());

		return Goal(state,
				rel<And>(v, Goal(new (state) Self(state,it,iter,c.getObj(),o.getObj(),v,pSelected))));
	}
    State& state;
	Ref<T> it;
	Set* pSet;	// IterationView protocol requires *pSet to exist during the full iteration
	IterationView<Set>	iter;
	RefView<bool,Cond> c;
	RefView<OrderT,Order> o;
	Obj v;
	SList<int>* pSelected;
};

}

#endif /* CASPER_KERNEL_GOAL_FORALL_H_ */
