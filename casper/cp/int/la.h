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
 

#ifndef CASPER_CP_INT_LA_H_
#define CASPER_CP_INT_LA_H_

namespace Casper {
namespace CP {

// this is required since doxygen sometimes
// doesn't autolink namespace scoped things
#ifdef DOXYGEN_PARSER
using namespace Casper;
using namespace Casper::CP;
#endif

/**
 * 	Maintains the information used by Look-Ahead heuristics, which is collected
 *  while performing some kind of singleton consistency.
 *  \sa sc,rsc,selectVarLAMinDomSize
 * 	\ingroup CPDataStructures
 */
template<class Vars>
struct LAInfo
{
	LAInfo(Store& store,const Vars& vars) : doms(store,vars),
							   accDomSize(store,doms.size(),0),
								lastVar(doms.size()) {}

	double getLog2CurSize()
	{
		double s = 0;
		for (uint i = 0; i < doms.size(); ++i)
			s += ::log(static_cast<double>(doms[i]->size()));
		return s/::log(2.0);
	}

	void onBeforeAssignment(uint, int)
	{}

	void onAfterAssignment(bool valid,uint varIdx, int)
	{
		if (varIdx != lastVar and lastVar < doms.size())
		{
			accDomSize[varIdx] = 0;
		}
		accDomSize[varIdx] += valid?getLog2CurSize():0;
		lastVar = varIdx;
	}

	DomArrayView<int,Vars> 	doms;
	Util::StdArray<double>		accDomSize;
	uint 					lastVar;
};

namespace Detail {

// Implements the var1 heuristic in page 6 of
// "On the Integration of Singleton Consistencies and Look-Ahead Heuristics"
template<class Vars>
struct SelectVarLAMinDomSize : IVarSelector
{
	SelectVarLAMinDomSize(Store& store,const Vars& vars, LAInfo<Vars>& info) :
		doms(store,vars),info(info) {}
	int select()
	{
		int bestIdx = -1;
		double bestVal = limits<double>::max();
		for (uint i = 0; i < doms.size(); ++i)
			if (!doms[i]->ground())
				if (info.accDomSize[i] < bestVal)
				{
					bestVal = info.accDomSize[i];
					bestIdx = i;
				}
		return bestIdx;
	}
	DomArrayView<int,Vars> doms;
	LAInfo<Vars>& info;
};
}

/**
 *  Selects the variable \f$ x \in \f$ \a vars which minimizes
 *  \f$ \sum_{v\in d\left(x\right)}\log_{2}\left|\left.d\left(\textrm{vars}\right)\right|_{x=v}\right| \f$
 *
 *
 *  This selector requires the lookahead information to be previously computed and
 *  stored in \a info. \sa LAInfo,sc,rsc,scMinSize,rscMinSize.
 *
 *  \note This selector does not guarantee a fair selection policy (see \ref VariableSelectors).
 *  \ingroup VariableSelectors
 */
template<class Vars>
VarSelector selectVarLAMinDomSize(Store& store,const Vars& vars,LAInfo<Vars>& info)
{	return new (store)
		Detail::SelectVarLAMinDomSize<Vars>(store,vars,info);	}

} // CP
} // Casper

#endif /* CASPER_CP_INT_LA_H_ */
