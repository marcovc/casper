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

struct StateStats
{
	State&	state;			// for other stats
	counter countCPRestores;
	counter countCPs;
	counter maxLevel;
	counter curLevel;
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

	counter	getNbRestores()	const
	{	return countCPRestores;	}

	counter	getNbCPs()	const
	{	return countCPs;	}

	counter	getMaxDepth()	const
	{	return maxLevel;	}

};

struct State
{
	State();
	~State();

	/// Returns an integer that uniquely identifies the current state
	counter id() const  { return curStateID.top();	}

	/// Insert a check point (saves current state)
	void insertCP();
	/// Restore all changes occurred since last inserted check point
	void restoreCP();
	/// Removes last inserted check point
	void removeCP();

	// fires at next choice point creation or restore
	void notifyOnNextCP(INotifiable* f) 	{	onNextCPSL.push(f);	}

	void wakeupCPDemons();

	/**
	 * 	Returns a reference to the reversible heap.
	 *	Memory obtained from this heap is available until the current check
	 *	point is restored.
	 */
	Util::IHeap& getHeap() {	return *glbHeap;	}
	operator Util::IHeap&() {	return *glbHeap;	}
	operator const Util::IHeap&() const {	return *glbHeap;	}

	/**
	 * 	Returns a reference to the check point heap.
	 *	Memory obtained from this heap is available until a new check point
	 *	is created or the current one is restored (whichever happens first).
	 */
	Util::IHeap& getCPHeap() {	return *cpHeap;	}

	void record(PTrailAgent u) { trail.record(u); }

	StateStats&	getStats() {	return stats; }
	const StateStats&	getStats() const {	return stats; }

	Util::IHeap*					glbHeap; 	// global heap
	Util::IHeap*					cpHeap;		// checkpoint heap

	Trail							trail;
	Util::StdStack<counter>			curStateID;
	counter							stateIDCtr;
	counter							mFails;
	Util::StdStack<INotifiable*>	onNextCPSL;
	StateStats						stats;
};


} // Casper

std::ostream& operator<<(std::ostream& os, const Casper::StateStats& s);

#endif /* CASPER_KERNEL_STATE_STATE_H_ */