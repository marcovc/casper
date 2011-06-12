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

#include <casper/util/iteration.h>
#include <casper/cp/view/array.h>

namespace Casper {


NEW_REL_1(whileNotGround,WhileNotGround)
NEW_REL_2(whileNotGround,WhileNotGround)

namespace Traits {
template<class T1,class T2>
struct GetEval<Rel2<WhileNotGround,T1,T2> >
{	typedef	bool	Type;	};
} // Traits

template<class T,class Vars,class Statm>
struct GoalView2<WhileNotGround,Seq<T>,Vars,bool,Statm> : IGoal
{
	GoalView2(State& state, const Vars& vars,const Statm& statm) :
		store(vars.getStore()),vars(store,vars), statm(statm){}
    Goal execute()
	{
    	for (uint i = 0; i < vars.size(); ++i)
    		if (!vars[i].ground())
    			return Goal(store, Goal(store,statm) and Goal(this));
		return succeed();
	}
    CP::Store& 		store;
	CP::ValArrayView<T,Vars> 	vars;
	Statm 		statm;
};

}

#endif /* CASPER_KERNEL_GOAL_WHILENOTGROUND_H_ */
