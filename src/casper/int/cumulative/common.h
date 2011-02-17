 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_INT_CUMULATIVE_COMMON_H_
#define CASPER_INT_CUMULATIVE_COMMON_H_

#include <vector>
#include <algorithm>
#include <casper/kernel/container/list.h>
#include <casper/kernel/container/vector.h>
#include <casper/kernel/container/slist.h>

namespace casper {
namespace detail {
namespace scheduling {

template<class Machine,class Start,class Duration,class End, class Height>
struct Sweep
{
	typedef typename EvalTraits<Start>::Eval::Elem		X;
	typedef typename EvalTraits<Height>::Eval::Elem		Y;
	Sweep(ICPSolver& solver,const Machine& machine,
						const Start& start,
						const Duration& duration,
						const End& end,
						const Height& height);
	Bool sweepLB(const Int& resource, const Y& limit)
	{	return sweep(resource,limit,true);	}
	Bool sweepUB(const Int& resource, const Y& limit)
	{	return sweep(resource,limit,false);	}

	protected:
	enum EvType { ECheck,EProf,EPrune };
	struct Event
	{
		Event(EvType type,Int task,	X date,	Y inc, Bool first = false) :
			type(type),task(task),date(date),inc(inc),first(first) {}
		EvType	type;
		Int		task;
		X		date;
		Y		inc;
		Bool	first;
	};
	struct LessEventPred
	{	Bool operator()(const Event* b1, const Event* b2) const
	    {	return b1->date < b2->date; }
	};

	Bool sweep(const Int& resource, const Y& limit,Bool lb);
	UInt collectEventsLB(Int m,Y limit, detail::SList<Event>& events);
	UInt collectEventsUB(Int m,Y limit, detail::SList<Event>& events);
	Bool pruneLB(Int m, Y limit, Int nbTask, Int sumHeight,
				 detail::List<Int>& listPrune,
				 const detail::Vector<Y>& contribution,
				 X low,X up);
	Bool pruneUB(Int m, Y limit, Int nbTask, Y sumHeight,
				 detail::List<Int>& listPrune,
				 const detail::Vector<Y>& contribution,
				 X low,X up);
	Bool prune1(Int m, Int t, X low, X up);
	Bool prune2(Int m, Int t, X low, X up);

	const UInt					n;
	DomArrayView<Int,Machine>	machine;
	DomArrayView<X,Start>		start;
	BndArrayView<X,Duration>	duration;
	DomArrayView<X,End>			end;
	BndArrayView<Y,Height>		height;
};

template<class Machine,class Start,class Duration,class End, class Height>
Sweep<Machine,Start,Duration,End,Height>::
	Sweep(ICPSolver& solver,const Machine& machine,
						const Start& start,
						const Duration& duration,
						const End& end,
						const Height& height) :
							n(start.size()),
							machine(solver,machine),
							start(solver,start),
							duration(solver,duration),
							end(solver,end),
							height(solver,height)
{}

template<class Machine,class Start,class Duration,class End, class Height>
UInt
Sweep<Machine,Start,Duration,End,Height>::
	collectEventsLB(Int m,Y limit, detail::SList<Event>& events)
{
	UInt nevents = 0;
	Int mm = std::max(0,limit);
	for (UInt i = 0; i < n; i++)
	{
		if (start[i]->max() < end[i]->min() and
			machine[i]->ground() and machine[i]->value() == m)
		{
			if (height[i].max() < mm)
			{
				events.pushFront(Event(ECheck,i,start[i]->max(),1));
				events.pushFront(Event(ECheck,i,end[i]->min(),-1));
				nevents += 2;
			}
			if (height[i].max() < 0)
			{
				events.pushFront(Event(EProf,i,start[i]->max(),height[i].max(),true));
				events.pushFront(Event(EProf,i,end[i]->min(),-height[i].max()));
				nevents += 2;
			}
		}
		if (machine[i]->find(m) !=  machine[i]->end())
		{
			if (height[i].max() > 0)
			{
				events.pushFront(Event(EProf,i,start[i]->min(),height[i].max(),true));
				events.pushFront(Event(EProf,i,end[i]->max(),-height[i].max()));
				nevents += 2;
			}
			if (! (start[i]->min()==start[i]->max() and
				   end[i]->min()==end[i]->max() and
				   machine[i]->min()==machine[i]->max() and
				   height[i].min()==height[i].max()))
			{
				events.pushFront(Event(EPrune,i,start[i]->min(),0));
				nevents += 1;
			}
		}
	}
	return nevents;
}

template<class Machine,class Start,class Duration,class End, class Height>
UInt
Sweep<Machine,Start,Duration,End,Height>::
	collectEventsUB(Int m,Y limit, detail::SList<Event>& events)
{
	UInt nevents = 0;
	Y mm = std::min(0,limit);
	for (UInt i = 0; i < n; i++)
	{
		if (start[i]->max() < end[i]->min() and
			machine[i]->ground() and machine[i]->value() == m)
		{
			if (height[i].min() > mm)
			{
				events.pushFront(Event(ECheck,i,start[i]->max(),1));
				events.pushFront(Event(ECheck,i,end[i]->min(),-1));
				nevents += 2;
			}
			if (height[i].min() > 0)
			{
				events.pushFront(Event(EProf,i,start[i]->max(),height[i].min(),true));
				events.pushFront(Event(EProf,i,end[i]->min(),-height[i].min()));
				nevents += 2;
			}
		}
		if (machine[i]->find(m) !=  machine[i]->end())
		{
			if (height[i].min() < 0)
			{
				events.pushFront(Event(EProf,i,start[i]->min(),height[i].min(),true));
				events.pushFront(Event(EProf,i,end[i]->max(),-height[i].min()));
				nevents += 2;
			}
			if (! (start[i]->min()==start[i]->max() and
				   end[i]->min()==end[i]->max() and
				   machine[i]->min()==machine[i]->max() and
				   height[i].min()==height[i].max()))
			{
				events.pushFront(Event(EPrune,i,start[i]->min(),0));
				nevents += 1;
			}
		}
	}
	return nevents;
}

template<class Machine,class Start,class Duration,class End, class Height>
Bool
Sweep<Machine,Start,Duration,End,Height>::
sweep(const Int& m, const Y& limit, Bool lb)
{
	// get events
	detail::SList<Event> events;
	UInt nevents = (lb?collectEventsLB(m,limit,events):
					   collectEventsUB(m,limit,events));

	if (nevents == 0)
		return true;

	// sort events
	std::vector<Event*> sev(nevents);
	for (typename detail::SList<Event>::Iterator it = events.begin();
			it != events.end(); ++it)
		sev[--nevents] = &*it;
	std::sort(sev.begin(),sev.end(),LessEventPred());

	// this is to prevent overflow to some operations below
	Y initContrib = lb?height[0].min():height[0].max();
	if (lb)
	{
		for (UInt i = 1; i < n; i++)
			if (height[i].min() < initContrib)
				initContrib = height[i].min();
	}
	else
	{
		for (UInt i = 1; i < n; i++)
			if (height[i].max() > initContrib)
				initContrib = height[i].max();
	}

	// sweep
//	detail::Vector<Y> contribution(stdHeap,n,lb?limits<Y>::min:limits<Y>::max);
	detail::Vector<Y> contribution(stdHeap,n,initContrib);
	detail::List<Int>	listPrune;

	typename vector<Event*>::iterator it = sev.begin();
	X d = (*it)->date;
	Int nbTask = 0;
	Y sumHeight = 0;
	while (it != sev.end())
	{
		if ((*it)->type != EPrune)
		{
			if (d != (*it)->date)
			{
				if (nbTask > 0 and (lb?sumHeight<limit:sumHeight>limit))
					return false;
				if (lb and !pruneLB(m,limit,nbTask,sumHeight,
							 listPrune,contribution,d,(*it)->date-1))
					return false;
				if (!lb and !pruneUB(m,limit,nbTask,sumHeight,
							 listPrune,contribution,d,(*it)->date-1))
					return false;
				d = (*it)->date;
			}
			if ((*it)->type == ECheck)
				nbTask += (*it)->inc;
			else
			{
				if ((*it)->first)
				{
					if (lb)
						contribution[(*it)->task] = std::max(contribution[(*it)->task],
													 	 (*it)->inc);
					else
						contribution[(*it)->task] = std::min(contribution[(*it)->task],
														 (*it)->inc);
				}
				sumHeight += (*it)->inc;
			}
		}
		else
			listPrune.pushBack((*it)->task);
		++it;
	}
	if (nbTask > 0 and (lb?sumHeight<limit:sumHeight>limit))
		return false;
	if (lb and !pruneLB(m,limit,nbTask,sumHeight,listPrune,contribution,d,d))
		return false;
	if (!lb and !pruneUB(m,limit,nbTask,sumHeight,listPrune,contribution,d,d))
		return false;
	return true;
}

template<class Machine,class Start,class Duration,class End, class Height>
Bool
Sweep<Machine,Start,Duration,End,Height>::
	pruneLB(Int m,Y limit, Int nbTask, Int sumHeight,
			 detail::List<Int>& listPrune,
			 const detail::Vector<Y>& contribution,
			 X low,X up)
{
	for (typename detail::List<Int>::Iterator it = listPrune.begin();
		it != listPrune.end(); )
	{
		Int t = *it;
		if (nbTask != 0 and sumHeight - contribution[t] < limit and
			!prune1(m,t,low,up))
			return false;
		if (sumHeight - contribution[t] + height[t].max() < limit and
			!prune2(m,t,low,up))
			return false;
		//std::cout << low << " " << up << " " << sumHeight << " " << contribution[t] << std::endl;
		// NB: in the following, paper says start[t]->max() < up
		if (machine[t]->ground() and end[t]->min() > low and
			start[t]->max() <= up and duration[t].min() > 0 and
			!height[t].updateMin(limit-(sumHeight-contribution[t])))
			return false;

		if (end[t]->max() <= up+1 or
			machine[t]->find(m) == machine[t]->end())
			listPrune.erase(it++);
		else
			++it;
	}
	return true;
}

template<class Machine,class Start,class Duration,class End, class Height>
Bool
Sweep<Machine,Start,Duration,End,Height>::
	pruneUB(Int m,Y limit, Int nbTask, Y sumHeight,
			 detail::List<Int>& listPrune,
			 const detail::Vector<Y>& contribution,
			 X low,X up)
{
	for (typename detail::List<Int>::Iterator it = listPrune.begin();
		it != listPrune.end(); )
	{
		Int t = *it;
		if (nbTask != 0 and sumHeight - contribution[t] > limit and
			!prune1(m,t,low,up))
			return false;
		if (sumHeight - contribution[t] + height[t].min() > limit and
			!prune2(m,t,low,up))
			return false;
		//std::cout << low << " " << up << " " << sumHeight << " " << contribution[t] << std::endl;
		// NB: in the following, paper says start[t]->max() < up
		if (machine[t]->ground() and machine[t]->value() == m and
			end[t]->min() > low and
			start[t]->max() <= up and duration[t].min() > 0 and
			//contribution[t] != limits<Y>::max and // because of overflow
			!height[t].updateMax(limit-(sumHeight-contribution[t])))
			return false;

		if (end[t]->max() <= up+1 or
			machine[t]->find(m) == machine[t]->end())
			listPrune.erase(it++);
		else
			++it;
	}
	return true;
}

template<class Machine,class Start,class Duration,class End, class Height>
Bool
Sweep<Machine,Start,Duration,End,Height>::
	prune1(Int m,Int t,X low,X up)
{
	return *machine[t] = m and
		   start[t]->updateMin(up - duration[t].max() + 1) and
		   start[t]->updateMax(low) and
		   end[t]->updateMax(low + duration[t].max()) and
		   end[t]->updateMin(up + 1) and
		   duration[t].updateMin(std::min(up-start[t]->max()+1,
				   						  end[t]->min()-low));
}

template<class Machine,class Start,class Duration,class End, class Height>
Bool
Sweep<Machine,Start,Duration,End,Height>::
	prune2(Int m,Int t, X low,X up)
{
	if (end[t]->min() > low and start[t]->max() <= up and duration[t].min() > 0)
		return machine[t]->erase(m);
	else
	if (machine[t]->ground())
	{
		if (duration[t].min() > 0)
		{
			if (!start[t]->erase(low-duration[t].min()+1,up) or
				!end[t]->erase(low+1,up+duration[t].min()))
				return false;
		}
		X maxd = std::max(low-start[t]->min(),std::max(end[t]->max()-up-1,0));
		return duration[t].updateMax(maxd);
	}
	return true;
}


}
}
}

#endif /*CASPER_INT_CUMULATIVE_COMMON_H_*/
