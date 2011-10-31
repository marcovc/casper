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
 

#ifndef CASPER_KERNEL_GOAL_WHILENOTGROUND_H_
#define CASPER_KERNEL_GOAL_WHILENOTGROUND_H_

#include <casper/kernel/view/iteration.h>
#include <casper/cp/view/array.h>

namespace Casper {


NEW_REL_1(whileNotGround,WhileNotGround)
NEW_FN_2(whileNotGround,WhileNotGround)

namespace Traits {
template<class T1,class T2>
struct GetEval<Rel2<WhileNotGround,T1,T2> >
{	typedef	bool	Type;	};
} // Traits

/**
 * 	While \a vars are not ground, executes \a statm.
 * 	\ingroup CPSearch
 */
template<class T,class Eval,int dims,class Statm>
struct GoalView2<WhileNotGround,Seq<T>,CP::VarArray<Eval,dims>,bool,Statm> : IGoal
{
	GoalView2(State& state, const CP::VarArray<Eval,dims>& vars,const Statm& statm) :
		state(state),vars(vars), statm(statm){}
    Goal execute()
	{
    	if (!vars.ground())
    		return Goal(state, Goal(state,statm) and Goal(this));
		return succeed();
	}
    State& 						state;
    CP::VarArray<Eval,dims> 	vars;
	Statm 						statm;
};

namespace Detail {

template<class Store,class T1,class T2>
struct Post<Store,Rel2<Distinct,T1,T2> >: IGoal
{
	Post(Store& store,const Rel2<Distinct,T1,T2>& p, bool validate) :
		store(store),p(p),validate(validate) {}
	Goal execute()
	{
		if (store.post(p,CP::postDomFilter))
			return Goal(!validate or store.valid());
		else
			return fail();
	}
	Store&	store;
	const Rel2<Distinct,T1,T2>	p;
	const bool	validate;
};
}
}

#endif /* CASPER_KERNEL_GOAL_WHILENOTGROUND_H_ */
