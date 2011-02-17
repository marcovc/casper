 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_GOAL_UTIL
#define _H_CASPER_KERNEL_GOAL_UTIL

#include <casper/kernel/goal/common.h>

namespace casper {

template<class T>
struct Display : IGoal
{
	Display(ICPSolver& s,const T& t) : IGoal(s),t(t) {}

    Goal execute()
	{
    	std::cout << t << std::endl;
    	return ok();
	}

	const T& t;
};

template<class T>
Goal	display(ICPSolver& s,const T& t)
{	return new (s.heap()) Display<T>(s,t);	}

namespace detail {

template<class Var, class Val>
struct GoalAssign : IGoal
{
	GoalAssign(ICPSolver& s,Var& var, const Val& val) :
		IGoal(s),var(var),val(val) {}
	Goal execute()
	{
		var = val;
		return ok();
	}
	Var& var;
	const Val& val;
};

template<class Var,class Val>
Goal	assign(ICPSolver& s,Var& var, const Val& val)
{	return new (s.heap()) GoalAssign<Var,Val>(s,var,val);	}

};

};

#endif /*_H_CASPER_KERNEL_GOAL_BASIC */
