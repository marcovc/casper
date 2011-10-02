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

#include <casper/util/iteration.h>

namespace Casper {

NEW_REL_2(forAll,ForAll)
NEW_REL_3(forAll,ForAll)

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
		return Goal(state,v and forAll(it,iter.next(),v));
	}
    State& state;
	Par<T> it;
	Iter r;
	//IterationView<T,Iter>	iter;
	Util::IterationView<Iter>	iter;
	Obj v;
};

}

#endif /* CASPER_KERNEL_GOAL_FORALL_H_ */
