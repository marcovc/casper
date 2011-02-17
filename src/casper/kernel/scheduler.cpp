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

#include <casper/kernel/scheduler.h>

namespace casper {

GreedyLIFOFilterSched* greedyLIFOFilterSched(ICPSolver& solver,bool suspend)
{
	GreedyLIFOFilterSched* ret = new (solver.heap()) GreedyLIFOFilterSched(solver,suspend);
	return ret;
}

ULIFOFilterSched* ulifoFilterSched(ICPSolver& solver,bool suspend)
{
	ULIFOFilterSched* ret = new (solver.heap()) ULIFOFilterSched(solver,suspend);
	return ret;
}

LIFOFilterSched* lifoFilterSched(ICPSolver& solver,bool suspend)
{
	LIFOFilterSched* ret = new (solver.heap()) LIFOFilterSched(solver,suspend);
	return ret;
}

FIFOFilterSched* fifoFilterSched(ICPSolver& solver,bool suspend)
{
	FIFOFilterSched* ret = new (solver.heap()) FIFOFilterSched(solver,suspend);
	return ret;
}

TwoCostFilterSched* twoCostFilterSched(ICPSolver& solver,
						IFilter::Cost maxFirstStageCost)
{
	TwoCostFilterSched* ret = new (solver.heap())
				TwoCostFilterSched(solver,maxFirstStageCost);
	return ret;
}

CostFilterSched<TwoCostFilterSched>* costFilterSched(ICPSolver& solver)
{
	CostFilterSched<TwoCostFilterSched>* ret =
		new (solver.heap()) CostFilterSched<TwoCostFilterSched>(solver);
	return ret;
}


};

