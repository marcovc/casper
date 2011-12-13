
/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_KERNEL_GOAL_TRYALL2_H_
#define CASPER_KERNEL_GOAL_TRYALL2_H_

namespace Casper {

NEW_REL_2(tryAll2,TryAll2)
NEW_FN_4(tryAll2,TryAll2)

namespace Traits {
template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<TryAll2,T1,T2,T3,T4> >
{	typedef bool	Type;	};
};

/**
 * 	Executes \a v for all \a it in \a s. Succeeds on the first \a it that makes \a v succeed.
 * 	Fails if, after trying to execute \a v for all \a it in \a s, v never succeeded.
 * 	Goal \a f is executed for all \a it in \a s whenever \a v fails.
 *
 * 	\ingroup Search
 */
template<class T,class Set,class Obj1,class Obj2>
struct GoalView4<TryAll2,T,Ref<T>,Seq<T>,Set,bool,Obj1,bool,Obj2> : IGoal
{
	typedef GoalView4<TryAll2,T,Ref<T>,Seq<T>,Set,bool,Obj1,bool,Obj2> Self;
	GoalView4(State& state, const Ref<T>& it, const Set& s, const Obj1& v, const Obj2& f) :
		state(state),it(it),
		pSet(new (state) Set(s)),
		iter(*pSet),
		v(v),
		f(f)
		{}
	GoalView4(State& state, const Ref<T>& it, const IterationView<Set>& iter, const Obj1& v, const Obj2& f) :
		state(state),it(it),
		pSet(NULL), // set needs to be saved only once for all iteration
		iter(iter),
		v(v),
		f(f)
		{}
    Goal execute()
	{
       	IterationView<Set> curIter(iter);

		if (pSet==NULL) // i.e this is not the first iteration
			curIter.iterate();

		if (!curIter.valid())
			return fail();

		it = curIter.value();

		return Goal(state,
				rel<Or>(v,
					rel<And>(f,Goal(new (state) Self(state,it,curIter,v,f)))));

	}
    State&				state;
	Ref<T> 				it;
	Set*				pSet;	// IterationView protocol requires *pSet to exist during the full iteration
	IterationView<Set>	iter;
	Obj1				v;
	Obj2				f;
};

// TODO: adapt from tryAll
#if 0
NEW_FN_4(tryAll2,TryAll2)

namespace Traits {
template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<TryAll2,T1,T2,T3,T4> >
{	typedef	bool	Type;	};
} // Traits

/**
 * 	Executes \a v2 for all \a it in \a r where \a v1 is true. Succeeds on the first \a it that makes \a v2 succeed.
 * 	Fails if, after trying to execute \a v2 for all \a it in \a r where \a v1 is true, \a v2 never succeeded.
 *
 * 	\ingroup Search
 */
template<class T,class Set,class Cond,class Obj>
struct GoalView4<TryAll2,T,Ref<T>,Seq<T>,Set,bool,Cond,bool,Obj> : IGoal
{
	typedef GoalView4<TryAll2,T,Ref<T>,Seq<T>,Set,bool,Cond,bool,Obj> Self;
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
			return fail();

		return Goal(state,
				rel<Or>(v, Goal(new (state) Self(state,it,curIter,c.getObj(),v))));
	}
    State& state;
	Ref<T> it;
	Set* pSet;	// IterationView protocol requires *pSet to exist during the full iteration
	IterationView<Set>	iter;
	RefView<bool,Cond> c;
	Obj v;
};

NEW_FN_5(tryAll2,TryAll2)

namespace Traits {
template<class T1,class T2,class T3,class T4,class T5>
struct GetEval<Rel5<TryAll2,T1,T2,T3,T4,T5> >
{	typedef	bool	Type;	};
} // Traits

/**
 * 	Executes \a v2 for all \a it in \a r where \a v1 is true, sorted by ascending values of \a o. Succeeds on the first \a it that makes \a v2 succeed.
 * 	Fails if, after trying to execute \a v2 for all \a it in \a r where \a v1 is true, \a v2 never succeeded.
 *
 * 	\ingroup Search
 */
template<class T,class Set,class Cond,class OrderT,class Order,class Obj>
struct GoalView5<TryAll2,T,Ref<T>,Seq<T>,Set,bool,Cond,OrderT,Order,bool,Obj> : IGoal
{
	typedef GoalView5<TryAll2,T,Ref<T>,Seq<T>,Set,bool,Cond,OrderT,Order,bool,Obj> Self;
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
   			return fail();

    	it = bestIt;

    	pSelected->pushFront(it.value());

		return Goal(state,
				rel<Or>(v, Goal(new (state) Self(state,it,iter,c.getObj(),o.getObj(),v,pSelected))));
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
#endif

} // Casper

#endif /* CASPER_KERNEL_GOAL_TRYALL2_H_ */
