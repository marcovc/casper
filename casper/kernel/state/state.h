/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>          	   *
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
 

#ifndef CASPER_KERNEL_STATE_STATE_H_
#define CASPER_KERNEL_STATE_STATE_H_

#include <casper/kernel/common.h>
//#include <casper/kernel/notify/susplist.h>
#include <casper/util/container/stdstack.h>
#include <casper/kernel/state/trail.h>

namespace Casper {

struct INotifiable;

struct State;

/**
 * 	State statistics.
 * 	\ingroup Search
 *
 */
struct StateStats
{
	StateStats(State& state) :
					state(state),
					countCPRestores(0),
					countCPs(0),
					maxLevel(0),
					curLevel(0) {}

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

	/// Returns the number of times a state has been restored
	counter	getNbRestores()	const
	{	return countCPRestores;	}

	/// Returns the number of times a new state has been created
	counter	getNbCPs()	const
	{	return countCPs;	}

	/// Returns the maximum number of states pushed in the stack so far
	counter	getMaxDepth()	const
	{	return maxLevel;	}

	/// Returns the current depth of the search tree
	counter	getCurDepth()	const
	{	return curLevel;	}

	/// Returns the state to which these statistics refer to
	const State& getState() const
	{	return state;	}

protected:
	State&	state;			// for other stats
	counter countCPRestores;
	counter countCPs;
	counter maxLevel;
	counter curLevel;
};

/**
 * 	Maintains a stack of states for non-deterministic programming.
 * 	\ingroup Search
 */
struct State
{
	State();
	~State();

	/// Returns an integer that uniquely identifies the current state
	counter id() const  { return curStateID.top();	}

	/// Insert a check point (saves current state)
	void insertCP();
	/// Undoes all changes occurred since last inserted check point
	void restoreCP();
	/// Removes last inserted check point
	void removeCP();

	/// fires \a f at next check point creation or restore
	void notifyOnNextCP(INotifiable* f) 	{	onNextCPSL.push(f);	}

	void wakeupCPDemons();

	/**
	 * 	Returns a reference to the reversible heap.
	 *	Memory obtained from this heap is available until the current check
	 *	point is restored.
	 */
	Util::IHeap& getHeap() {	return *glbHeap;	}

	/**
	 * 	Returns a reference to the reversible heap.
	 *	Memory obtained from this heap is available until the current check
	 *	point is restored.
	 */
	const Util::IHeap& getHeap() const {	return *glbHeap;	}

	/**
	 * 	Automatically casts a state to its reversible heap. \sa getHeap()
	 */
	operator Util::IHeap&() {	return *glbHeap;	}
	/**
	 * 	Automatically casts a state to its reversible heap. \sa getHeap()
	 */
	operator const Util::IHeap&() const {	return *glbHeap;	}

	/**
	 * 	Returns a reference to the check point heap.
	 *	Memory obtained from this heap is available until a new check point
	 *	is created or the current one is restored (whichever happens first).
	 */
	Util::IHeap& getCPHeap() {	return *cpHeap;	}

#ifndef SWIG
	/**
	 * 	Records a new function object \a u in the trail. This function will be
	 *	called when restoring the last inserted check point.
	 */
	void record(PTrailAgent u) { trail.record(u); }
#endif

	/// Returns state statistics
	StateStats&	getStats() {	return stats; }
	/// Returns state statistics
	const StateStats&	getStats() const {	return stats; }

	protected:

	Util::IHeap*					glbHeap; 	// global heap
	Util::IHeap*					cpHeap;		// checkpoint heap

	Trail							trail;
	Util::StdStack<counter>			curStateID;
	counter							stateIDCtr;
	counter							mFails;
	Util::StdStack<INotifiable*>	onNextCPSL;
	StateStats						stats;
};

template<class T>
struct GetPState
{
	State* operator()(const T&)
	{ 	return NULL;	}
};

// convenience

template<class T>
State*	getPState(const T& t)
{	return GetPState<T>()(t);	}

template<class T1,class T2>
State*	getPState(const T1& t1,const T2& t2)
{
	State* s = getPState(t1);
	if (s != NULL)
		return s;
	else
		return getPState(t2);
}

template<class T1,class T2,class T3>
State*	getPState(const T1& t1,const T2& t2,const T3& t3)
{	return getPState(getPState(t1,t2),t3);	}

template<class T1,class T2,class T3,class T4>
State*	getPState(const T1& t1,const T2& t2,const T3& t3,const T4& t4)
{	return getPState(getPState(t1,t2,t3),t4);	}

template<class T1,class T2,class T3,class T4,class T5>
State*	getPState(const T1& t1,const T2& t2,const T3& t3,const T4& t4,const T5& t5)
{	return getPState(getPState(t1,t2,t3,t4),t5);	}


template<class T>
State& getState(const T& t)
{
	State* p = getPState(t);
	if (p==NULL)
		throw Exception::Extract("State");
	return *p;
}

} // Casper

std::ostream& operator<<(std::ostream& os, const Casper::StateStats& s);

#endif /* CASPER_KERNEL_STATE_STATE_H_ */
