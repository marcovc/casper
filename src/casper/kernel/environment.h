 /*************************************************************************\
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

#ifndef _H_CASPER_KERNEL_ENVIRONMENT
#define _H_CASPER_KERNEL_ENVIRONMENT

#include <iostream>
#include <iomanip>
#include <stdio.h>

#include <casper/kernel/timer.h>

#include <time.h>

using namespace std;
using namespace __gnu_cxx;

namespace casper {

struct ICPSolver&;
struct ICPSolver;

struct IEnv
{
	virtual void add(ICPSolver&) = 0;
	virtual bool valid() const = 0;
//	virtual void resetValid() = 0;

	virtual Heap heap() = 0;

	virtual Timer totalTime() const = 0;
 	virtual void signalFailure() = 0;

	virtual ~IEnv() {}
};

// Single-Threaded environment
struct STEnv : IEnv
{
	std::list<ICPSolver&>	solvers;
	Timer				timerTotalTime;
	bool				bIsValid;

	STEnv() : timerTotalTime("time since environment creation")
	{	timerTotalTime.resume(); }

	void add(ICPSolver& f);
	bool valid() const { return bIsValid; }
//	void resetValid() { dispatcher.resetValid(); }

 	void signalFailure() { bIsValid = false; }

	// FIXME
	Heap heap() {	return globalHeap;	}

	Timer totalTime() const
	{	return timerTotalTime;	}
};

struct Env :  detail::PImplIdiom<IEnv>
{
	typedef Env							Self;
	typedef detail::PImplIdiom<IEnv> 	Super;
	Env() : Super(new STEnv()) {}
	Env(IEnv* pEnv) : Super(pEnv) {}

	unsigned long long stateID() const { return Super::pImpl->stateID(); }

	void add(ICPSolver& f);

	bool valid() const { return Super::pImpl->valid(); }
//	void resetValid() { Super::pImpl->resetValid(); }

	void signalFailure()  { Super::pImpl->signalFailure(); }

	Heap heap() { return Super::pImpl->heap(); }

	Timer totalTime() const
	{	return Super::pImpl->totalTime();	}
};


};


#endif
