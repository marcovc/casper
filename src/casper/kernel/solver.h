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

#ifndef _H_CASPER_KERNEL_SOLVER
#define _H_CASPER_KERNEL_SOLVER

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <algorithm>

#include <casper/kernel/debug.h>

#include <casper/kernel/memory.h> // temp
#include <casper/kernel/timer.h>
#include <casper/kernel/trail.h>
#include <casper/kernel/notifiable.h>
#include <casper/kernel/relation.h>
#include <casper/kernel/solver.h>
//#include <casper/kernel/filter/common.h>
#include <casper/kernel/debug.h>
#include <casper/kernel/exception.h>

#include <time.h>

using namespace std;

namespace casper {

struct Filter;
struct IFilterSched;

struct Goal;
struct IGoalSched;


typedef unsigned long long Counter;

//using namespace std;
struct Stats
{
	Counter countCPRestores;
	Counter countCPs;
	Counter countPropagations;
	Counter maxLevel;
	Counter countDomainUpdates;
	Counter curLevel;
	Counter countDomains;
	Counter countFilters;
	Counter countSolutions;

	Stats() : 	countCPRestores(0),
				countCPs(0),
				countPropagations(0),
				maxLevel(0),
				countDomainUpdates(0),
				curLevel(0),
				countDomains(0),
				countFilters(0),
				countSolutions(0){}

	void signalCPRestore()
	{ countCPRestores++; }

	void signalCPCreate()
	{
		countCPs++;
		curLevel++;
		maxLevel = std::max(maxLevel,curLevel);
	}

	void signalCPRemove()
	{ 	curLevel--; }

	void signalPropagation()
	{ countPropagations++; }

	void signalDomainUpdate()
	{ countDomainUpdates++; }

	void signalNewDomain()
	{ countDomains++; }

	void signalNewFilter()
	{ countFilters++; }

	void signalNewSolution()
	{ countSolutions++; }

	Counter	fails()	const
	{	return countCPRestores;	}

	Counter	filters()	const
	{	return countFilters;	}

	Counter	solutions()	const
	{	return countSolutions;	}

};

ostream& operator<<(ostream& os, const casper::Stats& s);

/**
 * 	The ICPSolver& interface. A solver object maintains data structures common
 *  to all components for a particular problem, namely those concerned
 *  with state handling, memory allocation, backtracking, etc.
 *  \ingroup Kernel
 */
struct ICPSolver : INotifiable
{
	/// Returns a unique identifier for the current state
	virtual Counter stateID() const = 0;
	virtual Counter propID() const = 0;
	//Counter propID() const { return stats().countCPs + stats().countCPRestores;}

	/// Adds a new filter to the constraint store
	virtual bool post(Filter) = 0;
	/// Tries to solve goal. Returns \a true if it is possible, \a false otherwise
	virtual bool solve(Goal) = 0;
	/// Search for next solution. Returns \a true if it is possible, \a false otherwise
	virtual bool next() = 0;

	/// Returns reference to current filter scheduler
	virtual IFilterSched&	filterSched() = 0;
	/// Returns reference to current goal scheduler
	virtual IGoalSched&		goalSched() = 0;
	virtual const IGoalSched*	getPGoalSched() const = 0;

	/// Replaces current filter scheduler
	virtual void setFilterSched(IFilterSched*) = 0;
	/// Replaces current goal scheduler
	virtual void setGoalSched(IGoalSched*) = 0;
	/// If true then all posted filters are weighted
	virtual void setFilterWeighting(bool v) = 0;
	/// Returns if filter weighting is enabled
	virtual bool getFilterWeighting() const = 0;

	/// Insert a check point (saves current state)
	virtual void insertCP() = 0;
	/// Restore all changes occurred since last inserted check point
	virtual void restoreCP() = 0;
	/// Removes last inserted check point
	virtual void removeCP() = 0;
	virtual void record(PTrailAgent) = 0;
	virtual void notifyOnNextCP(INotifiable* f) = 0;

	/// Propagates pending filters. Returns if they are consistent
	virtual bool valid() = 0;

	virtual void signalNewDomain(void*) = 0;
	virtual void signalNewFilter(void*) = 0;
	virtual void signalPropagation() = 0;
	virtual void signalDomainUpdate() = 0;
	virtual void signalFailure() = 0;

	/// Returns (reversible) heap
	virtual Heap heap() = 0;
	/// Returns static heap
	virtual Heap sHeap() = 0;
	/// Returns propagation heap
	virtual Heap pHeap() = 0;

	/// Returns total time since ICPSolver& object creation
	virtual CPUTimer& totalTime() = 0;
	/// Returns statistics about solving process
	virtual const Stats& stats() const = 0;
	/// Returns statistics about solving process
	virtual Stats& stats() = 0;

	/// Returns number of fails
	virtual Counter fails() const = 0;

	virtual Logger& logger() = 0;
	virtual const Logger& logger() const = 0;

	virtual ~ICPSolver() {}

	void log(const void* obj, std::string strObj, const Logger::EventTag& tag)
	{ 	logger().log(obj,strObj,tag); }

};

/**
 * 	For checking if solver could be extracted at runtime
 *
 */
struct NullCPSolver : ICPSolver
{
	~NullCPSolver() {}
private:
	void throwException() const;
	Counter stateID() const;
	Counter propID() const;
	bool post(Filter);
	bool solve(Goal);
	bool next();
	IFilterSched&	filterSched();
	IGoalSched&		goalSched();
	const IGoalSched*	getPGoalSched() const;
	void setFilterSched(IFilterSched*);
	void setGoalSched(IGoalSched*);
	void setFilterWeighting(bool v);
	bool getFilterWeighting() const;
	void insertCP();
	void restoreCP();
	void removeCP();
	void record(PTrailAgent);
	void notifyOnNextCP(INotifiable* f);
	bool valid();
	void signalNewDomain(void*);
	void signalNewFilter(void*);
	void signalPropagation();
	void signalDomainUpdate();
	void signalFailure();
	Heap heap();
	Heap sHeap();
	Heap pHeap();
	CPUTimer& totalTime();
	const Stats& stats() const;
	Stats& stats();
	Counter fails() const;
	Logger& logger();
	const Logger& logger() const;
	Bool notify();
};

extern NullCPSolver glbNullCPSolver;

struct CPSolverState
{
	CPSolverState(ICPSolver& solver) : solver(solver),bIsValid(true),bPending(false)
	{}
	bool notify()	{ bPending = true; return true; }
	//bool isValid()	// FIXME: currently implemented as CPICPSolver::isValid
	void resetValid() { bIsValid=true; bPending=false; }
	void fail()
	{
		solver.log(this,"CPSolverState",Logger::solverFail);
		bIsValid=false;
		bPending=false;
	}

	ICPSolver& solver;
	Bool bIsValid;
	Bool bPending;
};

template<class T>
struct GetCPSolver
{
	ICPSolver& operator()(const T&)
	{ return glbNullCPSolver; }
};

// convenience

template<class T>
ICPSolver&	getCPSolver(const T& t)
{	return GetCPSolver<T>()(t);	}

template<class T1,class T2>
ICPSolver&	getCPSolver(const T1& t1,const T2& t2)
{
	ICPSolver& s = getCPSolver(t1);
	if (&s != &static_cast<ICPSolver&>(glbNullCPSolver))
		return s;
	else
		return getCPSolver(t2);
}

template<class T1,class T2,class T3>
ICPSolver&	getCPSolver(const T1& t1,const T2& t2,const T3& t3)
{	return getCPSolver(getCPSolver(t1,t2),t3);	}

template<class T1,class T2,class T3,class T4>
ICPSolver&	getCPSolver(const T1& t1,const T2& t2,const T3& t3,const T4& t4)
{	return getCPSolver(getCPSolver(t1,t2),t3,t4);	}

template<class T1,class T2,class T3,class T4,class T5>
ICPSolver&	getCPSolver(const T1& t1,const T2& t2,const T3& t3,const T4& t4,const T5& t5)
{	return getCPSolver(getCPSolver(t1,t2),t3,t4,t5);	}

template<>
struct GetCPSolver<ICPSolver>
{
	ICPSolver& operator()(const ICPSolver& solver)
	{	return const_cast<ICPSolver&>(solver);	}
};


template<class Func,class T1>
struct GetCPSolver<Rel1<Func,T1> >
{
	ICPSolver& operator()(const Rel1<Func,T1>& s)
	{ return getCPSolver(s.p1); }
};

template<class Func,class T1,class T2>
struct GetCPSolver<Rel2<Func,T1,T2> >
{
	ICPSolver&	operator()(const Rel2<Func,T1,T2>& r)
	{	return getCPSolver(r.p1,r.p2); }
};

template<class Func,class T1,class T2,class T3>
struct GetCPSolver<Rel3<Func,T1,T2,T3> >
{
	ICPSolver&	operator()(const Rel3<Func,T1,T2,T3>& r)
	{	return getCPSolver(r.p1,r.p2,r.p3);	}
};

template<class Func,class T1,class T2,class T3,class T4>
struct GetCPSolver<Rel4<Func,T1,T2,T3,T4> >
{
	ICPSolver&	operator()(const Rel4<Func,T1,T2,T3,T4>& r)
	{	return getCPSolver(r.p1,r.p2,r.p3,r.p4);}
};

template<class Func,class T1,class T2,class T3,class T4,class T5>
struct GetCPSolver<Rel5<Func,T1,T2,T3,T4,T5> >
{
	ICPSolver&	operator()(const Rel5<Func,T1,T2,T3,T4,T5>& r)
	{	return getCPSolver(r.p1,r.p2,r.p3,r.p4,r.p5);}
};

template<class T>
struct GetCPSolver<detail::List<T> >
{
	ICPSolver& operator()(const detail::List<T>& l)
	{
		for (auto it = l.begin();	it != l.end(); ++it)
		{
			ICPSolver& p = getCPSolver(*it);
			if (&p != &static_cast<ICPSolver&>(glbNullCPSolver))
				return p;
		}
		return glbNullCPSolver;
	}
};

};


#endif
