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

#include <casper/kernel/view/arrayview.h>

namespace casper {

namespace detail {

template<class Vars>
Double getSize(const DomArrayView<Int,Vars>& doms)
{
	UInt cc = doms.size();
	Double ret = 0;
	for (UInt i = 0; i < cc; i++)
		ret += log2((Double)doms[i]->size());
	return ret;
}

template<class Vars>
struct LabelWithImpacts : IGoal
{
	typedef typename ElemTraits<Vars>::Elem	Var;
	typedef typename DomTraits<Var>::Dom	Dom;
	typedef typename EvalTraits<Var>::Eval	Eval;
	typedef typename Dom::Iterator Iterator;

	LabelWithImpacts(const Vars& vars) :
								IGoal(getCPSolver(vars)),
								doms(getCPSolver(vars),vars),
								minElem(findMin(vars)),
								domSize(findMax(vars)-minElem+1),
								impactCount(vars.count(),domSize,0),
								impactSum(vars.count(),domSize,0),
								impacts(vars.solver().heap(),vars.count(),0),
								lastBestVar(0),
								lastBestVal(0),
								lastSizeLog2(0),
								first(true)
								{}

	Double calcVarImpact(UInt idx)
	{
		Double varImpact = 0;
		for (Iterator it = doms[idx]->begin();
			it != doms[idx]->end(); ++it)
			if (impactCount[idx][*it-minElem] > 0)
				varImpact += 1-impactSum[idx][*it-minElem]/
							(Double)impactCount[idx][*it-minElem];
		return varImpact;
	}


	Bool initializeImpacts()
	{
		UInt count = doms.size();
		Double sizeBeforeLog2 = getSize(doms);
		for (UInt varidx = 0; varidx < count; varidx++)
			if (!doms[varidx]->ground())
			{
				for (Iterator valit = doms[varidx]->begin();
						valit != doms[varidx]->end(); )
				{
					Int val = *valit;
					++valit;

					doms.solver().insertCP();

					*doms[varidx] = val;
					bool isValid = doms.solver().valid();

					Double sizeAfterLog2 = (isValid)?(getSize(doms)):(0);

					doms.solver().restoreCP();
					doms.solver().removeCP();

					++impactCount[varidx][val-minElem];
					impactSum[varidx][val-minElem] +=
						1-::pow(2.0,sizeAfterLog2-sizeBeforeLog2);

					// remove if inconsistent
					if (!isValid)
					{
						if (!doms[varidx]->erase(val) or
							!doms.solver().valid())
							return false;
						valit = doms[varidx]->upperBound(val);
					}
				}
				impacts[varidx] = calcVarImpact(varidx);
			}
		return true;
	}

	Goal execute()
	{
		UInt nvars = doms.size();

		if (doms.ground())
			return ok();

		Double curSizeLog2 = getSize(doms);

		if (first)
		{
			first = false;
			if (!initializeImpacts())
				return fail();
			if (doms.ground())
				return ok();
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

		Double maxVarImpact = limits<Double>::max();
		UInt	bestVar = 0;
		for (UInt varidx = 0; varidx < nvars; varidx++)
			if (!doms[varidx]->ground())
			{
				Double varImpact = impacts[varidx];
				if (varImpact < maxVarImpact)
				{
					maxVarImpact = varImpact;
					bestVar = varidx;
				}
			}

		Int bestVal;

		// select the value with the smallest impact
		Double bestValImpact = 0;
		for (Iterator it = doms[bestVar]->begin();
				it != doms[bestVar]->end(); ++it)
		{
			Double i = 1-impactSum[bestVar][*it-minElem]/(Double)impactCount[bestVar][*it-minElem];
			if (i >= bestValImpact)
			{
				bestValImpact = i;
				bestVal = *it;
			}
		}

		lastBestVar = bestVar;
		lastBestVal = bestVal;
		lastSizeLog2 = curSizeLog2;

		return (doms[bestVar].getView() == bestVal or doms[bestVar].getView() != bestVal)
				and Goal(this);
	}

	DomArrayView<Int,Vars> 	doms;
	Eval					minElem;
	UInt					domSize;
	Array<Int,2>			impactCount;
	Array<Double,2>			impactSum;
	detail::Vector<Double> 	impacts;
	UInt					lastBestVar;
	Int						lastBestVal;
	Double					lastSizeLog2;
	Bool					first;
};
};

/**
 * 	Labels \a vars using a variable and value selection strategy based on
 *  impacts (Refalo).
 * 	\ingroup Goals
 *
 */
template<class Vars>
Goal labelWithImpacts(const Vars& vars)
{	return new (getCPSolver(vars).heap()) detail::LabelWithImpacts<Vars>(vars);	}

}

#endif
