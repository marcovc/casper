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
 

#ifndef CASPER_CP_INT_SC_H_
#define CASPER_CP_INT_SC_H_


namespace Casper {
namespace CP {

// this is required since doxygen sometimes
// doesn't autolink namespace scoped things
#ifdef DOXYGEN_PARSER
using namespace Casper;
using namespace Casper::CP;
#endif

namespace Detail {

// TODO: may be optimized in the case rsc=false to try to avoid completing the last loop
// (only need to go to the last 'failed' variable)
template<class Vars,class InfoCollector>
struct SC : IGoal
{
	SC(Store& store, const Vars& vars, bool rsc, uint minSize, InfoCollector& infoCollector) :
		IGoal(),
		store(store),
		doms(store,vars),rsc(rsc),minSize(minSize),collectInfo(infoCollector) {}
	Goal execute(IExplorer& explorer)
	{
		State& state = explorer;
		bool pruned = true;
		do
		{
			pruned = false;
			for (uint varidx = 0; varidx < doms.size(); varidx++)
				if (!doms[varidx]->ground() and doms[varidx]->size()<=minSize)
					for (auto valit = doms[varidx]->begin(); valit != doms[varidx]->end(); )
					{
						int val = *valit;
						++valit;

						collectInfo.onBeforeAssignment(varidx,val);

						state.insertCP();

						*doms[varidx] = val;
						bool isValid = store.valid();

						collectInfo.onAfterAssignment(isValid,varidx,val);

						state.restoreCP();
						state.removeCP();

						if (!isValid)
						{
							pruned = true;
							if (!doms[varidx]->erase(val) or !store.valid())
								return fail();
							valit = doms[varidx]->upperBound(val);
						}
					}
		} while (!rsc and pruned);
		return succeed();
	}
	Store&	store;
	DomArrayView<int,Vars> doms;
	const bool rsc;
	const uint minSize;
	InfoCollector& collectInfo;
};

struct NOPInfoCollector
{
	void onBeforeAssignment(uint,int) {}
	void onAfterAssignment(bool,uint,int) {}
};
}

/**
 *  \name Singleton Consistency
 *	\anchor SingletonConsistency
 *
 *  The functions below return a new Goal that performs some kind of singleton consistency on
 *  \a obj and \a store. The optional parameter \a i is used to collect lookahead information during
 *  the propagation algorithm.
 *
*/
/// @{
/**
 * 	Enforces singleton consistency on the set of variables \a obj.
 *  The optional parameter \a i is used to collect lookahead information during
 *  the propagation algorithm. \sa LAInfo
 *  \ingroup CPSearch
 */
#ifndef _MSC_VER
template<class Obj,class InfoCollector = Detail::NOPInfoCollector>
Goal sc(Store& store, const Obj& obj,InfoCollector i = InfoCollector())
{	return new (store)
		Detail::SC<Obj,InfoCollector>(store,obj,false,limits<uint>::max(),i); }
#else
template<class Obj>
Goal sc(Store& store, const Obj& obj)
{	return new (store)
		Detail::SC<Obj,Detail::NOPInfoCollector>(store,obj,false,limits<uint>::max(),Detail::NOPInfoCollector()); }
template<class Obj,class InfoCollector>
Goal sc(Store& store, const Obj& obj,InfoCollector i = InfoCollector())
{	return new (store)
		Detail::SC<Obj,InfoCollector>(store,obj,false,limits<uint>::max(),i); }
#endif

/**
 * 	Enforces restricted singleton consistency on the set of variables \a obj.
 *  The optional parameter \a i is used to collect lookahead information during
 *  the propagation algorithm. \sa LAInfo
 *  \ingroup CPSearch
 */
#ifndef _MSC_VER
template<class Obj,class InfoCollector = Detail::NOPInfoCollector>
Goal rsc(Store& store, const Obj& obj,InfoCollector i = InfoCollector())
{	return new (store)
		Detail::SC<Obj,InfoCollector>(store,obj,true,limits<uint>::max(),i); }
#else
template<class Obj>
Goal rsc(Store& store, const Obj& obj)
{	return new (store)
		Detail::SC<Obj,Detail::NOPInfoCollector>(store,obj,true,limits<uint>::max(),Detail::NOPInfoCollector()); }
template<class Obj,class InfoCollector>
Goal rsc(Store& store, const Obj& obj,InfoCollector i = InfoCollector())
{	return new (store)
		Detail::SC<Obj,InfoCollector>(store,obj,true,limits<uint>::max(),i); }
#endif

/**
 * 	Enforces singleton consistency on the subset of variables of \a a with at
 *  least \a minSize values in its domain.
 *  The optional parameter \a i is used to collect lookahead information during
 *  the propagation algorithm. \sa LAInfo
 *  \ingroup CPSearch
 */
#ifndef _MSC_VER
template<class Obj,class InfoCollector = Detail::NOPInfoCollector>
Goal scMinSize(Store& store,const Obj& obj,uint minSize,InfoCollector i = InfoCollector())
{	return new (store)
		Detail::SC<Obj,InfoCollector>(store,obj,false,minSize,i); }
#else
template<class Obj>
Goal scMinSize(Store& store,const Obj& obj,uint minSize)
{	return new (store)
		Detail::SC<Obj,Detail::NOPInfoCollector>(store,obj,false,minSize,Detail::NOPInfoCollector()); }
template<class Obj,class InfoCollector>
Goal scMinSize(Store& store,const Obj& obj,uint minSize,InfoCollector i = InfoCollector())
{	return new (store)
		Detail::SC<Obj,InfoCollector>(store,obj,false,minSize,i); }
#endif

/**
 * 	Enforces restricted singleton consistency on the subset of variables of \a a with at
 *  least \a minSize values in its domain.
 *  The optional parameter \a i is used to collect lookahead information during
 *  the propagation algorithm. \sa LAInfo
 *  \ingroup CPSearch
 */
#ifndef _MSC_VER
template<class Obj,class InfoCollector = Detail::NOPInfoCollector>
Goal rscMinSize(Store& store,const Obj& obj,uint minSize,InfoCollector i = InfoCollector())
{	return new (store)
		Detail::SC<Obj,InfoCollector>(store,obj,true,minSize,i); }
#else
template<class Obj>
Goal rscMinSize(Store& store,const Obj& obj,uint minSize)
{	return new (store)
		Detail::SC<Obj,Detail::NOPInfoCollector>(store,obj,true,minSize,Detail::NOPInfoCollector()); }
template<class Obj,class InfoCollector>
Goal rscMinSize(Store& store,const Obj& obj,uint minSize,InfoCollector i = InfoCollector())
{	return new (store)
		Detail::SC<Obj,InfoCollector>(store,obj,true,minSize,i); }

#endif
/// @}

} // CP
} // Casper

#endif /* CASPER_INT_SC_H_ */
