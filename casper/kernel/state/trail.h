 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_STATE_TRAIL
#define _H_CASPER_KERNEL_STATE_TRAIL

#include <iostream>	// temp
#include <casper/util/container/stdslist.h>
#include <casper/util/container/stdstack.h>

//using namespace std;

namespace Casper {

struct ITrailAgent
{
//	void* id;
//	ITrailAgent(void* id) : id(id) {}
	virtual void restore() = 0;
	virtual ~ITrailAgent() { }
};

typedef ITrailAgent*	PTrailAgent;


/**
	\brief Container for saving and restoring objects of any type.
*/
struct Trail
{
	typedef Util::StdSList<PTrailAgent> 	Level;

	Util::IHeap&	heap;
	Util::StdStack<Level> 	s;

	Trail(Util::IHeap& heap) : heap(heap), s(heap)
	{}

	void record(PTrailAgent u)
	{
		assert(!s.empty());
		s.top().pushFront(u);
	}

	// implies saving the old one
	void stateNew()
	{	s.push(Level(heap)); }

	void stateRollback()
	{
		assert(!s.empty());
		Level& t = s.top();

		for (Level::Iterator it = t.begin();
				it != t.end(); ++it)
			(*it)->restore();
		t.clear();
	}

	void statePop()
	{	s.pop();	}
};

};



#endif
