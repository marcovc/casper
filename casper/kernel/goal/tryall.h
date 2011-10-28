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
 

#ifndef CASPER_KERNEL_GOAL_TRYALL_H_
#define CASPER_KERNEL_GOAL_TRYALL_H_

namespace Casper {

NEW_REL_2(tryAll,TryAll)
NEW_FN_3(tryAll,TryAll)

namespace Traits {
template<class T1,class T2,class T3>
struct GetEval<Rel3<TryAll,T1,T2,T3> >
{	typedef bool	Type;	};
};

/**
 * 	Executes \a v for all \a it in \a r. Succeeds on the first \a it that makes \a v succeed.
 * 	Fails if, after trying to execute \a v for all \a it in \a r, v never succeeded.
 *
 * 	\ingroup Search
 */
template<class T,class Iter,class Obj>
struct GoalView3<TryAll,T,Par<T>,Seq<T>,Iter,bool,Obj> : IGoal
{
	GoalView3(State& state, const Par<T>& it, const Iter& r, const Obj& v) :
		state(state),it(it),r(r),
		iter(this->r),
		v(v)
		{}
    Goal execute()
	{
		if (!iter.valid())
			return fail();

		it = iter.value();

		// logically redundant: just to avoid an extra disjunction
		if (!iter.next().valid())
			return Goal(state,v);

		return Goal(state,v or tryAll(it,iter.next(),v));
	}
    State&				state;
	Par<T> 				it;
	Iter					r;
	//IterationView<T,Iter>	iter;
	IterationView<Iter>	iter;
	Obj					v;
};


NEW_FN_4(tryAll,TryAll)

namespace Traits {
template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<TryAll,T1,T2,T3,T4> >
{	typedef	bool	Type;	};
} // Traits

/**
 * 	Executes \a v2 for all \a it in \a r where \a v1 is true. Succeeds on the first \a it that makes \a v2 succeed.
 * 	Fails if, after trying to execute \a v2 for all \a it in \a r where \a v1 is true, \a v2 never succeeded.
 *
 * 	\ingroup Search
 */
template<class T,class Iter,class Expr1,class Expr2>
struct GoalView4<TryAll,T,Par<T>,Seq<T>,Iter,
				bool,Expr1,bool,Expr2> : IGoal
{
	GoalView4(State& state, const Par<T>& it,
				const Iter& r, const Expr1& v1,const Expr2& v2) :
		state(state),it(it),r(r),iter(this->r),v1(v1),v2(v2) {}
    Goal execute()
	{
		if (!iter.valid())
			return fail();
		it = iter.value();
		iter.iterate();
		return Goal(state,v1 or (v2 and Goal(this)));
	}
    State&	state;
	Par<T> it;
	Iter					r;
	//IterationView<T,Iter>	iter;
	IterationView<Iter>	iter;
	Expr1	v1;
	Expr2	v2;
};

} // Casper

#endif /* CASPER_KERNEL_GOAL_TRYALL_H_ */
