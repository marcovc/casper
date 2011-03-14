 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_SUSPLIST
#define _H_CASPER_KERNEL_SUSPLIST

#include <casper/kernel/state/state.h>
#include <casper/kernel/reversible/list.h>
#include <casper/kernel/reversible/slist.h>
#include <casper/kernel/notify/notifier.h>
#include <vector>

namespace Casper {

// reversible

// event-based suspention list
// always sends the same event to all attached notifiables

struct EventSuspList : INotifier
{
	typedef INotifier	Super;
	//typedef List<INotifiable*>	Util::StdSList;
	typedef SList<INotifiable*>	SL;
	SL	l;
	EventSuspList(State& state) : l(state)
								   {}
	void attach(INotifiable* f)
	{	/*l.pushBack(f);*/ l.pushFront(f);	}
	void detach(INotifiable* f)
	{	l.erase(f); 	}

	uint getNbFilters() const;
	uint getNbWeightedFilters() const;
	uint getTotalAFC() const;

	bool notify() {	assert(0);	return true; }
	bool notifyAll()
	{
		for (SL::Iterator it = l.begin(); it != l.end(); ++it)
			if (!(*it)->notify())
				return false;
		return true;
	}
};

};

#endif /*SUSPLIST_H_*/
