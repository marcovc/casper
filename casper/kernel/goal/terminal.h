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
 

#ifndef CASPER_KERNEL_GOAL_TERMINAL_H_
#define CASPER_KERNEL_GOAL_TERMINAL_H_

#include <casper/kernel/goal/goal.h>
#include <casper/kernel/goal/explorer.h>

namespace Casper {

struct Succeed : IGoal
{
	Succeed() {}
	Goal execute(IExplorer&) { return (IGoal*)NULL; }
};

struct Fail : IGoal
{
	Fail() {}
	Goal execute(IExplorer& sched) {	sched.fail(); return (IGoal*)NULL; }
};

#if 0
extern IGoal* const pGlobalSucceedGoal;
extern IGoal* const pGlobalFailGoal;


inline Goal	succeed() 				{	return pGlobalSucceedGoal; }
inline Goal	fail() 					{	return pGlobalFailGoal; }
#else

inline Goal	succeed()
{
	static Succeed g;
	return &g;
}

inline Goal	fail()
{
	static Fail g;
	return &g;
}

#endif
} // Casper

#endif /* CASPER_KERNEL_GOAL_TERMINAL_H_ */
