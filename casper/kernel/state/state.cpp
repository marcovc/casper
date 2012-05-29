/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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
 
#include <casper/kernel/state/state.h>
#include <casper/kernel/notify/notifiable.h>
#include <casper/util/memory.h>

#include <iostream>
#include <iomanip>

namespace Casper {

#define CASPER_RHEAP_INIT_SIZE		static_cast<uint>(32*1024)
#define CASPER_RHEAP_GROW_RATIO		2
#define CASPER_PHEAP_MAX_SIZE		static_cast<uint>(64E6)

State::State() :
		glbHeap(new Util::RDynamicHeap(CASPER_RHEAP_INIT_SIZE,
							  	 CASPER_RHEAP_GROW_RATIO)),
		cpHeap(new Util::StaticHeap(CASPER_PHEAP_MAX_SIZE)),

		trail(getHeap()),
		stateIDCtr(0),
	    onNextCPSL(getCPHeap()),
	    stats(*this)
		#ifdef CASPER_LOG
		,eg(this)
		#endif
{
		insertCP();
}

State::~State()
{
	delete glbHeap;
	delete cpHeap;
}

void State::insertCP()
{
	curStateID.push(++stateIDCtr);
	//++propIDCtr;
	trail.stateNew();
	static_cast<Util::RDynamicHeap*>(glbHeap)->signalCPCreate();
	wakeupCPDemons();
	stats.signalCPCreate();
}

void State::restoreCP()
{
	CASPER_AT_STATE_RESTORE_ENTER(eg);
	//++propIDCtr;
	trail.stateRollback();
	++mFails;
	static_cast<Util::RDynamicHeap*>(glbHeap)->signalCPRestore();
	wakeupCPDemons();
	//Debug::log(this,Debug::onFail);
	stats.signalCPRestore();
	CASPER_AT_STATE_RESTORE_LEAVE(eg);
}

void State::removeCP()
{
//	std::cout << "popped CP " << curStateID.top() << std::endl;
	curStateID.pop();
//	std::cout << "now at CP " << curStateID.top() << std::endl;
	trail.statePop();
	static_cast<Util::RDynamicHeap*>(glbHeap)->signalCPRemove();
	stats.signalCPRemove();
}


void State::wakeupCPDemons()
{
	while (!onNextCPSL.empty())
	{
		onNextCPSL.top()->notify();
		onNextCPSL.pop();
	}
	// warning: having this after notification means that nothing can be
	// allocated from this heap on the previous called notify methods
	static_cast<Util::StaticHeap*>(cpHeap)->reset();
}

}

using namespace std;

// TODO: also print heap stats
ostream& operator<<(ostream& os, const Casper::StateStats& s)
{
	os << left << setw (30) << "Number of check points" << ":" <<  std::setw (10) << std::right
							  << s.getNbCPs() << std::endl
	   << left << setw (30) << "Number of restores" << ":" <<  std::setw (10) << std::right
							  << s.getNbRestores() << std::endl
	   << left << setw (30) << "Maximum depth" << ":" <<  std::setw (10) << std::right
							  << s.getMaxDepth() << std::endl
	   << left << setw (30) << "Peak of reversible heap (KiB)" << ":" <<  std::setw (10) << std::right
							  << (s.getState().getHeap().maxAllocated()/1024);

	return os;
}
