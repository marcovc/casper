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

#include <casper/kernel/solver.h>
#include <casper/kernel/goal/scheduler.h>
#include <casper/kernel/scheduler.h>

#include <casper/kernel/cpsolver.h>
#include <casper/kernel/exception.h>

using namespace std;

namespace casper {

ostream& operator<<(ostream& os, const casper::Stats& s)
{
	os << left << setw (20) << "solutions" << ":" << std::setw (10) << std::right
	  						<< s.countSolutions << std::endl
	   << left << setw (20) << "variables" << ":" << std::setw (10) << std::right
		  					  << s.countDomains << std::endl
	   << left << setw (20) << "filters" << ":" << std::setw (10) << std::right
							  << s.countFilters << std::endl
	   << left << setw (20) << "executions" << ":" << std::setw (10) << std::right
							  << s.countPropagations << std::endl
	   << left << setw (20) << "fails" << ":" <<  std::setw (10) << std::right
							  << s.fails() << std::endl
	   << left << setw (20) << "choice points" << ":" <<  std::setw (10) << std::right
							  << s.countCPs << std::endl
	   << left << setw (20) << "max depth" << ":" <<  std::setw (10) << std::right
							  << s.maxLevel << std::endl
	   << left << setw (20) << "updates" << ":" << std::setw (10) << std::right
							  << s.countDomainUpdates;
	return os;
}

// -------- NULL CP Solver -------------

// the global NullCPSolver object
NullCPSolver glbNullCPSolver;

#define CASPER_MAKE_COMPILER_HAPPY(T) {return *static_cast<T*>(NULL);}

// NULL CP Solver: every operation throws an exception
void NullCPSolver::throwException() const
{	throw ENullSolver();	}

Counter NullCPSolver::stateID() const
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(Counter);	}

Counter NullCPSolver::propID() const
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(Counter);	}

bool NullCPSolver::post(Filter)
{ throwException(); return false; }

bool NullCPSolver::solve(Goal)
{ throwException(); return false; }

bool NullCPSolver::next()
{ throwException(); return false; }

IFilterSched&	NullCPSolver::filterSched()
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(IFilterSched);	}

IGoalSched&		NullCPSolver::goalSched()
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(IGoalSched);	}

const IGoalSched*	NullCPSolver::getPGoalSched() const
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(IGoalSched*);}

void NullCPSolver::setFilterSched(IFilterSched*)
{ throwException(); }

void NullCPSolver::setGoalSched(IGoalSched*)
{ throwException(); }

void NullCPSolver::setFilterWeighting(bool v)
{ throwException(); }

bool NullCPSolver::getFilterWeighting() const
{ throwException(); return false; }

void NullCPSolver::insertCP()
{ throwException(); }

void NullCPSolver::restoreCP()
{ throwException(); }

void NullCPSolver::removeCP()
{ throwException(); }

void NullCPSolver::record(PTrailAgent)
{ throwException(); }

void NullCPSolver::notifyOnNextCP(INotifiable* f)
{ throwException(); }

bool NullCPSolver::valid()
{ throwException(); return false; }

void NullCPSolver::signalNewDomain(void*)
{ throwException(); }

void NullCPSolver::signalNewFilter(void*)
{ throwException(); }

void NullCPSolver::signalPropagation()
{ throwException(); }

void NullCPSolver::signalDomainUpdate()
{ throwException(); }

void NullCPSolver::signalFailure()
{ throwException(); }

Heap NullCPSolver::heap()
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(Heap);}

Heap NullCPSolver::sHeap()
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(Heap);}

Heap NullCPSolver::pHeap()
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(Heap);}

CPUTimer& NullCPSolver::totalTime()
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(CPUTimer);}

const Stats& NullCPSolver::stats() const
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(const Stats);}

Stats& NullCPSolver::stats()
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(Stats);}

Counter NullCPSolver::fails() const
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(Counter);}

Logger& NullCPSolver::logger()
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(Logger);}

const Logger& NullCPSolver::logger() const
{ throwException(); CASPER_MAKE_COMPILER_HAPPY(const Logger); }

bool NullCPSolver::notify()
{	throwException(); return false; }

#undef CASPER_MAKE_COMPILER_HAPPY
};


