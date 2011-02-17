/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <casper/kernel/cpsolver.h>
#include <casper/kernel/goal/common.h>
#include <casper/kernel/filter/common.h>
#include <casper/kernel/scheduler.h>
#include <casper/kernel/goal/scheduler.h>
#include <casper/kernel/filter/weightedfilter.h>

#define CASPER_RHEAP_INIT_SIZE		static_cast<UInt>(32*1024)
#define CASPER_RHEAP_GROW_RATIO		2
#define CASPER_SHEAP_INIT_SIZE		static_cast<UInt>(1024)
#define CASPER_SHEAP_GROW_RATIO		1.5
#define CASPER_PHEAP_MAX_SIZE		static_cast<UInt>(64E6)

namespace casper {

CPSolver::CPSolver() :
			globalHeap(new RDynamicHeap(CASPER_RHEAP_INIT_SIZE,
										CASPER_RHEAP_GROW_RATIO)),
			globalSHeap(new DynamicHeap(CASPER_SHEAP_INIT_SIZE,
										CASPER_SHEAP_GROW_RATIO)),
		    propHeap(new StaticHeap(CASPER_PHEAP_MAX_SIZE)),
		    state(*this),
			trail(heap()),
			timerTotalTime("runtime"),
			stateIDCtr(0),
			propIDCtr(0),
		    pGoalSched(*this),
		    onCPSL(propHeap),
		    weightFilters(false),
		    filters(*this)
{
	insertCP();
	pFilterSched = new (heap()) FullyPreemptCostFilterSched<FIFOFilterSched>(*this);
//	pFilterSched = new (heap()) FIFOFilterSched(this);
	pGoalSched = new (heap()) DFSGoalSched(*this);
}

CPSolver::~CPSolver()
{
	//pFilterSched;
	pGoalSched.get()->~IGoalSched();

	delete globalHeap.getPImpl();
	delete globalSHeap.getPImpl();
	delete propHeap.getPImpl();
}

void CPSolver::setFilterSched(IFilterSched* p)
{
	pFilterSched = p;
	p->attach(this);
}


Bool CPSolver::post(Filter f)
{
	filters.pushFront(f.getPImpl());

	if (!pFilterSched->post(f))
	{
		signalFailure();
		return false;
	}
	return true;
}

void CPSolver::setFilterWeighting(bool val)
{	weightFilters = val;	}

bool CPSolver::getFilterWeighting() const
{	return weightFilters;	}

bool CPSolver::valid()
{
	if (state.bPending)
	{
		state.bIsValid = pFilterSched->execute();
		state.bPending = false;
	}
	return state.bIsValid;
}


bool CPSolver::solve(Goal g)
{	return valid() and pGoalSched->execute(g);	}

bool CPSolver::next()
{	return pGoalSched->next();	}


};
