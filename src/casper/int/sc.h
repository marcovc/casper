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
 

#ifndef CASPER_INT_SC_H_
#define CASPER_INT_SC_H_


namespace casper {
namespace detail {

// TODO: may be optimized in the case rsc=false to try to avoid completing the last loop
// (only need to go to the last 'failed' variable)
template<class Vars,class InfoCollector>
struct SC : IGoal
{
	SC(const Vars& vars, Bool rsc, UInt minSize, InfoCollector& infoCollector) :
		IGoal(getCPSolver(vars)),
		doms(getCPSolver(vars),vars),rsc(rsc),minSize(minSize),collectInfo(infoCollector) {}
	Goal execute()
	{
		Bool pruned = true;
		do
		{
			pruned = false;
			for (UInt varidx = 0; varidx < doms.size(); varidx++)
				if (!doms[varidx]->ground() and doms[varidx]->size()<=minSize)
					for (auto valit = doms[varidx]->begin(); valit != doms[varidx]->end(); )
					{
						Int val = *valit;
						++valit;

						collectInfo.onBeforeAssignment(varidx,val);

						solver().insertCP();

						*doms[varidx] = val;
						bool isValid = solver().valid();

						collectInfo.onAfterAssignment(isValid,varidx,val);

						solver().restoreCP();
						solver().removeCP();

						if (!isValid)
						{
							pruned = true;
							if (!doms[varidx]->erase(val) or !solver().valid())
								return fail();
							valit = doms[varidx]->upperBound(val);
						}
					}
		} while (!rsc and pruned);
		return ok();
	}
	DomArrayView<Int,Vars> doms;
	const Bool rsc;
	const UInt minSize;
	InfoCollector& collectInfo;
};

struct NOPInfoCollector
{
	void onBeforeAssignment(UInt,Int) {}
	void onAfterAssignment(Bool,UInt,Int) {}
};
}

/**
 *  \ingroup Goals
 * 	Enforces singleton consistency on the set of variables \a vars.
 *  The optional parameter \a i is used to collect lookahead information during
 *  the propagation algorithm. \sa LAInfo
 */
template<class Vars,class InfoCollector = detail::NOPInfoCollector>
Goal sc(const Vars& vars,InfoCollector i = InfoCollector())
{	return new (getSolver(vars).heap())
		detail::SC<Vars,InfoCollector>(vars,false,limits<UInt>::max(),i); }

/**
 *  \ingroup Goals
 * 	Enforces restricted singleton consistency on the set of variables \a vars.
 *  The optional parameter \a i is used to collect lookahead information during
 *  the propagation algorithm. \sa LAInfo
 */
template<class Vars,class InfoCollector = detail::NOPInfoCollector>
Goal rsc(const Vars& vars,InfoCollector i = InfoCollector())
{	return new (getSolver(vars).heap())
		detail::SC<Vars,InfoCollector>(vars,true,limits<UInt>::max(),i); }

/**
 *  \ingroup Goals
 * 	Enforces singleton consistency on the subset of variables of \a a with at
 *  least \a minSize values in its domain.
 *  The optional parameter \a i is used to collect lookahead information during
 *  the propagation algorithm. \sa LAInfo
 */
template<class Vars,class InfoCollector = detail::NOPInfoCollector>
Goal scMinSize(const Vars& vars,UInt minSize,InfoCollector i = InfoCollector())
{	return new (getSolver(vars).heap())
		detail::SC<Vars,InfoCollector>(vars,false,minSize,i); }

/**
 *  \ingroup Goals
 * 	Enforces restricted singleton consistency on the subset of variables of \a a with at
 *  least \a minSize values in its domain.
 *  The optional parameter \a i is used to collect lookahead information during
 *  the propagation algorithm. \sa LAInfo
 */
template<class Vars,class InfoCollector = detail::NOPInfoCollector>
Goal rscMinSize(const Vars& vars,UInt minSize,InfoCollector i = InfoCollector())
{	return new (getCPSolver(vars).heap())
		detail::SC<Vars,InfoCollector>(vars,true,minSize,i); }

}

#endif /* CASPER_INT_SC_H_ */
