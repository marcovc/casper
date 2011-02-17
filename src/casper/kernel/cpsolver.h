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

#ifndef _H_CASPER_KERNEL_CPSOLVER
#define _H_CASPER_KERNEL_CPSOLVER

#include <casper/kernel/solver.h>
#include <casper/kernel/reversible.h>
#include <casper/kernel/susplist.h>
#include <casper/kernel/debug.h>

namespace casper {

struct IFilter;

struct CPSolver : ICPSolver
{
	typedef detail::RSList<IFilter*>	Filters;

	CPSolver();
	~CPSolver();

	Counter stateID() const { return curStateID.top();	}
	Counter propID() const { return propIDCtr; }

	void setFilterWeighting(bool);
	bool getFilterWeighting() const;

	bool post(Filter);

	// Adds a new constraint to the constraint store, enforced by the filter created by f
	template<class View,class FilterFactory>
	bool post(const View& v, const FilterFactory& f)
	{	return post(f(v));	}

	bool solve(Goal);
	bool next();

	bool notify()
	{	mLogger.log(this,"CPICPSolver&",Logger::solverNotify); return state.notify(); }

	IFilterSched&	filterSched() {	return *pFilterSched;	}
	IGoalSched&		goalSched()	{	return *pGoalSched;	}
	const IGoalSched*		getPGoalSched()	const {	return pGoalSched;	}

	void setFilterSched(IFilterSched* p);
	void setGoalSched(IGoalSched* p) {	pGoalSched = p; }

	inline void wakeupCPDemons();

	void insertCP()
	{
		curStateID.push(++stateIDCtr);
		++propIDCtr;
	//	std::cout << "inserted CP " << curStateID.top() << std::endl;
		trail.stateNew();
		((IRHeap&)globalHeap.getImpl()).signalCPCreate();
		wakeupCPDemons();
	}
	void restoreCP()
	{
	//	curStateID.top() = ++stateIDCtr; // not required when rd = 1
		//std::cout << "restored CP " << curStateID.top() << std::endl;
		++propIDCtr;
		trail.stateRollback();
		++mFails;
		((IRHeap&)globalHeap.getImpl()).signalCPRestore();
		state.resetValid();
		wakeupCPDemons();
		//Debug::log(this,Debug::onFail);
	}
	void removeCP()
	{
	//	std::cout << "popped CP " << curStateID.top() << std::endl;
		curStateID.pop();
	//	std::cout << "now at CP " << curStateID.top() << std::endl;
		trail.statePop();
		((IRHeap&)globalHeap.getImpl()).signalCPRemove();
	}
	void record(PTrailAgent u) { trail.record(u); }

	bool valid();

	void signalNewDomain(void*)	{ mStats.signalNewDomain(); }
	void signalNewFilter(void*)	{ mStats.signalNewFilter(); }
	void signalPropagation() { mStats.signalPropagation(); }
	void signalDomainUpdate() { mStats.signalDomainUpdate(); }
 	void signalFailure() { state.fail(); }

	Heap heap() {	return globalHeap;	}
	Heap sHeap() {	return globalSHeap;	}
	Heap pHeap() {	return propHeap;	}

	CPUTimer& totalTime()
	{	return timerTotalTime;	}
	const Stats& stats() const
	{	return mStats;	}
	Stats& stats()
	{	return mStats;	}
	Counter fails() const
	{	return mFails;	}

	// fires at next choice point creation or restore
	Void notifyOnNextCP(INotifiable* f)	{	onCPSL.push(f);	}

	const Filters&	getFilters() const {	return filters;	}

	Logger& logger()
	{ 	return mLogger;	}
	const Logger& logger() const
	{ 	return mLogger;	}

	Heap							globalHeap;
	Heap							globalSHeap;
	Heap							propHeap;

	CPSolverState 					state;
	Trail							trail;
	Stats							mStats;
	CPUTimer						timerTotalTime;
	detail::Stack<Counter>			curStateID;
	Counter							stateIDCtr;
	Counter							propIDCtr;
	Counter							mFails;
	IFilterSched*					pFilterSched;
	Reversible<IGoalSched*>			pGoalSched;
	detail::Stack<INotifiable*>		onCPSL;
	Bool							weightFilters;
	Filters							filters;
	Logger							mLogger;
};

inline void CPSolver::wakeupCPDemons()
{
	while (!onCPSL.empty())
	{
		onCPSL.top()->notify();
		onCPSL.pop();
	}
	// warning: having this after notification means that nothing can be
	// allocated from this heap on the previous called notify methods
	((StaticHeap&)propHeap.getImpl()).reset();
}

};

#endif
