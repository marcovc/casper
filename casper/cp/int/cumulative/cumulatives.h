 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_CP_INT_CUMULATIVE_CUMULATIVES_H_
#define CASPER_CP_INT_CUMULATIVE_CUMULATIVES_H_

#include <casper/cp/int/cumulative/common.h>

namespace Casper {
namespace CP {


/**
 * 	Implements the algorithm from:
 *  ...
 *  \note capacities[i] corresponds to the capacity of machine i.
 *  \pre \forall i, machines[i] \in 0..size(capacities)-1
 */
template<class Machine,class Start,class Duration,class End, class Height,class Capacity>
struct Cumulatives : IFilter
{
	typedef typename Casper::Traits::GetEval<Start>::Type::Elem		X;
	typedef typename Casper::Traits::GetEval<Height>::Type::Elem	Y;

	Cumulatives(Store& store,const Machine& machine,
							  const Start& start,
							  const Duration& duration,
							  const End& end,
							  const Height& height,
							  const Capacity& capacity);

	bool execute();
	Cost cost() const	{	return quadraticLo; }

	void attach(INotifiable* f)
	{
		start.attach(f); duration.attach(f); end.attach(f);
		height.attach(f); capacity.attach(f);
	}

	void detach()
	{
		start.detach(); duration.detach(); end.detach();
		height.detach(); capacity.detach();
	}

	protected:

	BndArrayView<int,Machine>	machine;
	BndArrayView<X,Start>		start;
	BndArrayView<X,Duration>	duration;
	BndArrayView<X,End>			end;
	BndArrayView<Y,Height>		height;
	BndArrayView<Y,Capacity>	capacity;

	Detail::Scheduling::Sweep<Machine,Start,Duration,End,Height> sweep;
};

template<class Machine,class Start,class Duration,class End, class Height,class Capacity>
Cumulatives<Machine,Start,Duration,End,Height,Capacity>::
	Cumulatives(Store& store,const Machine& machine,
							  const Start& start,
							  const Duration& duration,
							  const End& end,
							  const Height& height,
							  const Capacity& capacity) :
							machine(store,machine),
							start(store,start),
							duration(store,duration),
							end(store,end),
							height(store,height),
							capacity(store,capacity),
							sweep(store,machine,start,duration,
									end,height)
{
	// tmp
	for (uint i = 0; i < start.size(); i++)
		store.post(start[i]+duration[i]==end[i]);
}

template<class Machine,class Start,class Duration,class End, class Height,class Capacity>
bool Cumulatives<Machine,Start,Duration,End,Height,Capacity>::execute()
{
	for (uint r = 0; r < capacity.size(); r++)
	{
		if (!sweep.sweepLB(r, capacity[r].min()))
			return false;
		if (!sweep.sweepUB(r, capacity[r].max()))
			return false;
	}
	return true;
}

#if 0
template<class Machine,class Start,class Duration,class End, class Height,class Capacity>
Filter cumulatives(const Machine& machine,const Start& start,
				   const Duration& duration,const End& end,
				   const Height& height,const Capacity& capacity)
{
	CPSolver& s = getState(start,end);
	return new (s.getHeap()) Cumulatives<Machine,Start,Duration,End,Height,Capacity>
			(s,machine,start,duration,end,height,capacity);
}
#endif

}
}

#endif /*CASPER_CP_INT_CUMULATIVE_CUMULATIVES_H_*/
