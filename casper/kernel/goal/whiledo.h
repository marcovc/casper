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
 

#ifndef CASPER_KERNEL_GOAL_WHILEDO_H_
#define CASPER_KERNEL_GOAL_WHILEDO_H_

#include <casper/kernel/view/iteration.h>

namespace Casper {

/*
 *	This could be modeled either as:
 *
 *	whiledo = (!cond) or (block and whiledo)
 *	or
 *	whiledo = (cond and block and whiledo)
 *
 *	The first promotes cond from Par to a Goal and hence
 *	allows backtracking to explore the block part even if
 *	cond is false, which looks like dangerous.
 *
 *  Therefore we implement the second version.
 */

NEW_REL_1(whileDo,WhileDo)
NEW_FN_2(whileDo,WhileDo)

namespace Traits {
template<class T1,class T2,class T3>
struct GetEval<Rel3<WhileDo,T1,T2,T3> >
{	typedef	bool	Type;	};
} // Traits

/**
 * 	While \a cond evaluates to \p true, executes \a statm.
 * 	\ingroup Search
 */
template<class Cond,class Statm>
struct GoalView2<WhileDo,bool,Cond,bool,Statm> : IGoal
{
	GoalView2(State& state, const Cond& cond,const Statm& statm) :
		state(state),cond(state,cond), statm(statm){}
    Goal execute()
	{
		if (!cond.value())
			return succeed();
		return Goal(state,Goal(state,statm) and Goal(this));
	}
    State& 		state;
	Par<bool> 	cond;
	Statm 		statm;
};

}

#endif /* CASPER_KERNEL_GOAL_WHILEDO_H_ */
