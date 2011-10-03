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

#ifndef CASPER_INT_IMPACTS_H_
#define CASPER_INT_IMPACTS_H_

#include <casper/cp/view/array.h>

namespace Casper {
namespace CP {

// this is required since doxygen sometimes
// doesn't autolink namespace scoped things
#ifdef DOXYGEN_PARSER
using namespace Casper;
using namespace Casper::CP;
#endif

#ifdef _MSC_VER
double log2( double n )  
{  return ::log( n ) / ::log( 2.0 );  }
#endif

namespace Detail {

template<class Vars>
double getSize(const DomArrayView<int,Vars>& doms)
{
	uint cc = doms.size();
	double ret = 0;
	for (uint i = 0; i < cc; i++)
		ret += log2((double)doms[i]->size());
	return ret;
}

template<class Vars>
struct LabelWithImpacts : IGoal
{
	typedef typename Casper::Traits::GetTermElem<Vars>::Type	Var;
	typedef typename Traits::GetDom<Var>::Type	Dom;
	typedef typename Casper::Traits::GetEval<Var>::Type	Eval;
	typedef typename Dom::Iterator Iterator;

	LabelWithImpacts(Store& store,const Vars& vars) :
								IGoal(),
								store(store),
								doms(store,vars),
								minElem(Detail::findMin(store,vars)),
								domSize(Detail::findMax(store,vars)-minElem+1),
								impactCount(doms.size(),domSize,0),
								impactSum(doms.size(),domSize,0),
								impacts(store,vars.count(),0),
								lastBestVar(0),
								lastBestVal(0),
								lastSizeLog2(0),
								first(true)
								{}

	double calcVarImpact(uint idx)
	{
		double varImpact = 0;
		for (Iterator it = doms[idx]->begin();
			it != doms[idx]->end(); ++it)
			if (impactCount[idx][*it-minElem] > 0)
				varImpact += 1-impactSum[idx][*it-minElem]/
							(double)impactCount[idx][*it-minElem];
		return varImpact;
	}


	bool initializeImpacts()
	{
		Env& env = store.getEnv();
		uint count = doms.size();
		double sizeBeforeLog2 = getSize(doms);
		for (uint varidx = 0; varidx < count; varidx++)
			if (!doms[varidx]->ground())
			{
				for (Iterator valit = doms[varidx]->begin();
						valit != doms[varidx]->end(); )
				{
					int val = *valit;
					++valit;

					env.getState().insertCP();

					*doms[varidx] = val;
					bool isValid = store.valid();

					double sizeAfterLog2 = (isValid)?(getSize(doms)):(0);

					env.getState().restoreCP();
					env.getState().removeCP();

					++impactCount[varidx][val-minElem];
					impactSum[varidx][val-minElem] +=
						1-::pow(2.0,sizeAfterLog2-sizeBeforeLog2);

					// remove if inconsistent
					if (!isValid)
					{
						if (!doms[varidx]->erase(val) or
							!store.valid())
							return false;
						valit = doms[varidx]->upperBound(val);
					}
				}
				impacts[varidx] = calcVarImpact(varidx);
			}
		return true;
	}

	Goal execute(IExplorer& e)
	{
		uint nvars = doms.size();

		if (doms.ground())
			return succeed();

		double curSizeLog2 = getSize(doms);

		if (first)
		{
			first = false;
			if (!initializeImpacts())
				return fail();
			if (doms.ground())
				return succeed();
		}
		else
		// update last assigment impact
		if (doms[lastBestVar]->ground() and
				lastBestVal == doms[lastBestVar]->value() and
				curSizeLog2 < lastSizeLog2)
		{
			//std::cout << "updating impact " << lastBestVar << " " << lastBestVal << std::endl;
			assert(curSizeLog2 < lastSizeLog2);
			++impactCount[lastBestVar][lastBestVal-minElem];
			impactSum[lastBestVar][lastBestVal-minElem] +=
				1 - ::pow(2.0,curSizeLog2-lastSizeLog2);
			impacts[lastBestVar] = calcVarImpact(lastBestVar);
		}
		else
		{
			//std::cout << "updating failure impact " << lastBestVar << " " << lastBestVal << std::endl;
			++impactCount[lastBestVar][lastBestVal-minElem];
			impactSum[lastBestVar][lastBestVal-minElem] += 1;
			impacts[lastBestVar] = calcVarImpact(lastBestVar);
		}

		double maxVarImpact = limits<double>::max();
		uint	bestVar = 0;
		for (uint varidx = 0; varidx < nvars; varidx++)
			if (!doms[varidx]->ground())
			{
				double varImpact = impacts[varidx];
				if (varImpact < maxVarImpact)
				{
					maxVarImpact = varImpact;
					bestVar = varidx;
				}
			}

		int bestVal;

		// select the value with the smallest impact
		double bestValImpact = 0;
		for (Iterator it = doms[bestVar]->begin();
				it != doms[bestVar]->end(); ++it)
		{
			double i = 1-impactSum[bestVar][*it-minElem]/(double)impactCount[bestVar][*it-minElem];
			if (i >= bestValImpact)
			{
				bestValImpact = i;
				bestVal = *it;
			}
		}

		lastBestVar = bestVar;
		lastBestVal = bestVal;
		lastSizeLog2 = curSizeLog2;

		return Goal(e,(post(store,doms[bestVar].getObj() == bestVal) or post(store,doms[bestVar].getObj() != bestVal))
				and Goal(this));
	}

	Store&					store;
	DomArrayView<int,Vars> 	doms;
	Eval					minElem;
	uint					domSize;
	Util::StdArray<int,2>			impactCount;
	Util::StdArray<double,2>			impactSum;
	Util::StdVector<double> 		impacts;
	uint					lastBestVar;
	int						lastBestVal;
	double					lastSizeLog2;
	bool					first;
};
} // Detail

/**
 *  \name Labeling Primitives
 *	\anchor LabelingPrimitives
 *
 *  The functions below return a new Goal that labels, i.e. assigns, all variables
 *  in \a s while propagating \a store.
 *
*/
/// @{
/**
 * 	Labels \a vars using a variable and value selection strategy based on
 *  impacts (Refalo).
 *	\ingroup CPSearch
 */
template<class Obj>
Goal labelWithImpacts(Store& store,const Obj& s)
{	return new (store) Detail::LabelWithImpacts<Obj>(store,s);	}
/// @}

} // CP
} // Casper

#endif
