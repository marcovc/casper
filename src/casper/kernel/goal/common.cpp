/**************************************************************************\
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

#include <casper/kernel/goal/common.h>
#include <casper/kernel/goal/basic.h>
#include <casper/kernel/filter/common.h>

#include <casper/kernel/goal/scheduler.h>

namespace casper {

Goal	nopCP(ICPSolver& s)
{	return new (s.heap()) GoalView2<Or,Bool,Goal,Bool,Goal>(s,ok(),fail()); }

IGoalSched::IGoalSched(ICPSolver& mSolver) : mSolver(mSolver)
{}

// some Goal methods that only now may be defined
Goal::Goal(Filter f) : Super(new (f.solver().heap()) GoalView<Filter>(f.solver(),f))
{}

#if 0
namespace detail {

struct Succeeds : IGoal
{
	Succeeds(Goal g) : IGoal(g.solver()),g(g) {}
	Goal execute()
	{
		IGoalSched* auxgs = dfs(solver());
		solver().setGoalSched(auxgs);
		//auxgs->setFallBack(true);

		Bool r = auxgs->execute(g);
		auxgs->restart();

		if (r)
			return ok();
		else
			return fail();
	}
	Goal g;
};
}

Goal succeeds(Goal g)
{	return new (g.solver().heap()) detail::Succeeds(g); }
#endif

}
