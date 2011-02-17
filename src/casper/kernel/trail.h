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

#ifndef _H_CASPER_KERNEL_TRAIL
#define _H_CASPER_KERNEL_TRAIL

#include <iostream>	// temp
#include <stack>
#include <deque>

#include <casper/kernel/container/slist.h>
#include <casper/kernel/container/stack.h>

using namespace std;

namespace casper {

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
	typedef detail::SList<PTrailAgent> 	Level;

	Heap	heap;
	detail::Stack<Level> 	s;

	Trail(Heap heap) : heap(heap), s(heap)
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
