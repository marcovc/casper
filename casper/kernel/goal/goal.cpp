 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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


#include <casper/kernel/goal/goal.h>
#include <casper/kernel/state/state.h>
#include <functional>

namespace Casper {

namespace Detail {

struct StdFunctionAsGoal : IGoal
{
	StdFunctionAsGoal(State& s, const std::function<Goal()>& fn)
		:	fn(fn) {}
	Goal execute()
	{	return fn();	}
	std::function<Goal()> fn;
};

} // Detail

Goal::Goal(State& s, const std::function<Goal()>& fn) :
	Super(new (s) Casper::Detail::StdFunctionAsGoal(s,fn)) {}

}


