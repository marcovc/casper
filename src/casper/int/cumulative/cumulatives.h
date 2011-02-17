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

#ifndef CASPER_INT_CUMULATIVE_CUMULATIVES_H_
#define CASPER_INT_CUMULATIVE_CUMULATIVES_H_

#include <casper/int/cumulative/common.h>

namespace casper {

/**
 * 	Implements the algorithm from:
 *  ...
 *  \note capacities[i] corresponds to the capacity of machine i.
 *  \pre \forall i, machines[i] \in 0..size(capacities)-1
 */
template<class Machine,class Start,class Duration,class End, class Height,class Capacity>
struct Cumulatives : IFilter
{
	typedef typename EvalTraits<Start>::Eval::Elem		X;
	typedef typename EvalTraits<Height>::Eval::Elem		Y;

	Cumulatives(ICPSolver& solver,const Machine& machine,
							  const Start& start,
							  const Duration& duration,
							  const End& end,
							  const Height& height,
							  const Capacity& capacity);

	Bool execute();
	Bool entailed() const {	assert(0); return false; }
	Cost cost() const	{	return quadraticLo; }

	Void attach(INotifiable* f)
	{
		start.attach(f); duration.attach(f); end.attach(f);
		height.attach(f); capacity.attach(f);
	}

	Void detach(INotifiable* f)
	{
		start.detach(f); duration.detach(f); end.detach(f);
		height.detach(f); capacity.detach(f);
	}

	protected:

	BndArrayView<Int,Machine>	machine;
	BndArrayView<X,Start>		start;
	BndArrayView<X,Duration>	duration;
	BndArrayView<X,End>			end;
	BndArrayView<Y,Height>		height;
	BndArrayView<Y,Capacity>	capacity;

	detail::scheduling::Sweep<Machine,Start,Duration,End,Height> sweep;
};

template<class Machine,class Start,class Duration,class End, class Height,class Capacity>
Cumulatives<Machine,Start,Duration,End,Height,Capacity>::
	Cumulatives(ICPSolver& solver,const Machine& machine,
							  const Start& start,
							  const Duration& duration,
							  const End& end,
							  const Height& height,
							  const Capacity& capacity) :
							IFilter(solver),
							machine(solver,machine),
							start(solver,start),
							duration(solver,duration),
							end(solver,end),
							height(solver,height),
							capacity(solver,capacity),
							sweep(solver,machine,start,duration,
									end,height)
{
	// tmp
	for (UInt i = 0; i < start.size(); i++)
		solver.post(start[i]+duration[i]==end[i]);
}

template<class Machine,class Start,class Duration,class End, class Height,class Capacity>
Bool Cumulatives<Machine,Start,Duration,End,Height,Capacity>::execute()
{
	for (UInt r = 0; r < capacity.size(); r++)
	{
		if (!sweep.sweepLB(r, capacity[r].min()))
			return false;
		if (!sweep.sweepUB(r, capacity[r].max()))
			return false;
	}
	return true;
}


template<class Machine,class Start,class Duration,class End, class Height,class Capacity>
Filter cumulatives(const Machine& machine,const Start& start,
				   const Duration& duration,const End& end,
				   const Height& height,const Capacity& capacity)
{
	ICPSolver& s = getCPSolver(start,end);
	return new (s.heap()) Cumulatives<Machine,Start,Duration,End,Height,Capacity>
			(s,machine,start,duration,end,height,capacity);
}

}

#endif /*CASPER_INT_CUMULATIVE_CUMULATIVES_H_*/
