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
NEW_REL_3(tryAll,TryAll)

namespace Traits {
template<class T1,class T2,class T3>
struct GetEval<Rel3<TryAll,T1,T2,T3> >
{	typedef bool	Type;	};
};

template<class T,class Iter,class View>
struct GoalView3<TryAll,T,Par<T>,Seq<T>,Iter,bool,View> : IGoal
{
	GoalView3(State& state, const Par<T>& it, const Iter& r, const View& v) :
		state(state),it(it),r(r),
		iter(this->r),
		v(v)
		{}
    Goal execute()
	{
		if (!iter.valid())
			return fail();

		it = iter.value();

		return Goal(state,v or tryAll(it,iter.next(),v));
	}
    State&				state;
	Par<T> 				it;
	Iter					r;
	//IterationView<T,Iter>	iter;
	Util::IterationView<Iter>	iter;
	View					v;
};


NEW_REL_4(tryAll,TryAll)

namespace Traits {
template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<TryAll,T1,T2,T3,T4> >
{	typedef	bool	Type;	};
} // Traits

// FIXME: must create a copy of this goal in every branch as above
template<class T,class Iter,class View1,class View2>
struct GoalView4<TryAll,T,Par<T>,Seq<T>,Iter,
				bool,View1,bool,View2> : IGoal
{
	GoalView4(State& state, const Par<T>& it,
				const Iter& r, const View1& v1,const View2& v2) :
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
	Util::IterationView<Iter>	iter;
	View1	v1;
	View2	v2;
};

} // Casper

#endif /* CASPER_KERNEL_GOAL_TRYALL_H_ */
