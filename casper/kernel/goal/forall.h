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
 * 	Executes \a v for all \a it in \a r. Succeeds if \a v
 * 	always succeeded, fails on the first \a it that fails \a v.
 * 	\ingroup Search
 */
template<class T,class Iter,class Obj>
struct GoalView3<ForAll,T,Par<T>,Seq<T>,Iter,bool,Obj> : IGoal
{
	GoalView3(State& state, const Par<T>& it,
				const Iter& r, const Obj& v) :
		state(state),it(it),r(r),iter(this->r),v(v) {}
    Goal execute()
	{
		if (!iter.valid())
			return succeed();
		it = iter.value();
		iter.iterate();
		return Goal(state,rel<And>(v,Goal(this)));
	}
    State& state;
	Par<T> it;
	Iter r;
	IterationView<Iter>	iter;
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
template<class T,class Iter,class Cond,class Obj>
struct GoalView4<ForAll,T,Par<T>,Seq<T>,Iter,bool,Cond,bool,Obj> : IGoal
{
	GoalView4(State& state, const Par<T>& it,
				const Iter& r, const Cond& c, const Obj& v) :
		state(state),it(it),r(r),iter(this->r),c(c),v(v) {}
    Goal execute()
	{
		if (!iter.valid())
			return succeed();
		it = iter.value();
		iter.iterate();
		return Goal(state,rel<And>(v,rel<Or>(rel<Not>(c),Goal(this))));
	}
    State& state;
	Par<T> it;
	Iter r;
	IterationView<Iter>	iter;
	Cond c;
	Obj v;
};

}

#endif /* CASPER_KERNEL_GOAL_FORALL_H_ */
