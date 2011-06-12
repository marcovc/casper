/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_CP_GOAL_BB_H_
#define CASPER_CP_GOAL_BB_H_

#include <casper/kernel/goal/explorer.h>
#include <casper/cp/view/bnd.h>

namespace Casper {
namespace CP {

// minimize using Branch and Bound search
template<class Eval,class View>
struct BBMinimizeExplorer : ISinglePathExplorer
{
	BBMinimizeExplorer(Store& store,const View& v) :
		ISinglePathExplorer(store),store(store),v(store,v),
			best(this->v.max()) {}

//	bool execute(Goal g)
//	{	return SinglePathExplorer::execute(g and optPivot(solver(),*this,atSolution));}

#if 1
	bool discard(bool atSolution) //const
	{
		if (v.min() > best)
			return true;

		if (atSolution)
		{
			if (v.max() > best)	// since v may not be ground
				return true;
			best = v.max()-1;
			return false;
		}

		return !v.updateMax(best);
	}
#else
	bool discard(bool atSolution) //const
	{
		//std::cout << atSolution << " : " << v.min() << " " << v.max() << " best=" << best << std::endl;

		if (!(v.min()<=best and v.updateMax(best)/* and store.valid()*/))
			return true;

		best = std::min(best,v.max());

		if (atSolution)
			best = best - 1;

		return false;
	}
#endif

	Store&					store;
	BndView<Eval,View>		v;
	Eval					best;
//	bool					atSolution;
};

template<class View>
ISinglePathExplorer* bbMinimize(Store& store,const View& v)
{	return new (store)
	BBMinimizeExplorer<typename Casper::Traits::GetEval<View>::Type,View>(store,v);	}



// Best First search using MultiplePathExplorer
template<class Eval,class View>
struct BFExplorer : IMultiplePathExplorer
{
	BFExplorer(State& state,const View& v, int maxPaths = -1) :
    	IMultiplePathExplorer(state,maxPaths),v(state,v) {}

	double evaluate() const
	{	return v.min();	}
	bool postpone() const
	{	return evaluate() > getBestPath().getEvaluation();	}

	BndView<Eval,View>	v;
};

template<class View>
IExplorer* bfMinimize(State& state,const View& v, int maxPaths = -1)
{	return new (state)
	BFExplorer<typename Casper::Traits::GetEval<View>::Type,View>(state,v,maxPaths);	}

}
}

#endif /* CASPER_CP_GOAL_BB_H_ */
