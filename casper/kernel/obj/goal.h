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
 

#ifndef CASPER_KERNEL_OBJ_GOAL_H_
#define CASPER_KERNEL_OBJ_GOAL_H_

#include <casper/kernel/obj/expr.h>

namespace Casper {
namespace Detail {

template<class T>
struct Create<T,Goal>
{
	Goal operator()(State& state, const T& t)
	{	return Goal(state,t);	}
};

} // Detail


template<class Eval>
struct GoalView<Expr<Eval> > : IGoal
{
	GoalView(State& state, const Expr<Eval>& e) :
		g(e.toGoal(state).getImpl()) {}
	Goal execute() {	return g.execute(); }
	IGoal& g;
};


}

#endif /* CASPER_KERNEL_OBJ_GOAL_H_ */
