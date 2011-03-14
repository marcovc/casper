 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

/**
 *	\file
 *
 *  Gerneralized Arc Consistent distinct constraint [\ref regin94].
 *
 *  Note: This file is just a wrapper for the Gecode library distinct filter,
 *  see \ref gac-gecode-h.
 */

#ifndef _H_CASPER_CP_INT_DISTINCT_GAC
#define _H_CASPER_CP_INT_DISTINCT_GAC

#define	 USE_GECODE_DOM_ALLDIFF

#include <casper/cp/filter/dom.h>
#include <casper/util/container/stdarray.h>
#include <casper/cp/int/distinct/common.h>
#include <casper/cp/int/distinct/val.h>
#include <casper/cp/int/var.h>

#include <casper/cp/int/distinct/gac-gecode.h>

namespace Casper {
namespace CP {


/**
 *	Filter that enforces generalized arc consistency on the
 *  distinct constraint over a sequence of integer variables
 *  [\ref regin94].
 *
 *  \ingroup IntFilters
 */
template<class View>
struct DomFilterView1<Distinct,IntSeq,View> : IFilter
{
	DomFilterView1(Store& store, const View& v);

	struct VarDemon : INotifiable
	{
		VarDemon(DomFilterView1* pOwner,uint idx) : rOwner(*pOwner),
													idx(idx)
		{	rOwner.doms[idx]->attachOnDomain(this);	}
		bool notify()
		{	return rOwner.doms[idx]->ground() or rOwner.pParent->notify();}
		// tmp
		INotifiable* getFirst() { return rOwner.pParent; }

		DomFilterView1& rOwner;
		uint idx;
	};

	bool	execute();

	void 	attach(INotifiable* pParent);
	void 	detach(INotifiable* pParent);
	Cost 	cost() const {	return quadraticHi; }

	private:
	bool	init();
	//VarArray<int> vars;
	Store&						store;
	BacktrackSentinel 			btSentinel;
	typedef DomArrayView<int,View>	Doms;
	Doms						doms;
	Detail::VarValGraph<Doms>* 	vvg;
	INotifiable*				pParent;
};

template<class View>
DomFilterView1<Distinct,IntSeq,View>::DomFilterView1(Store& store, const View& v) :
		IFilter(store),
		store(store),
		btSentinel(store),
		doms(store,v),
		vvg(NULL)
{}

template<class View>
void DomFilterView1<Distinct,IntSeq,View>::attach(INotifiable* pParent)
{
	this->pParent = pParent;
	for (uint i = 0; i < doms.size(); i++)
		new (store) VarDemon(this,i);
		//vars[i].domain().attachOnDomain(this);


}

template<class View>
void DomFilterView1<Distinct,IntSeq,View>::detach(INotifiable* pParent)
{
	for (uint i = 0; i < doms.size(); i++)
		doms[i]->detachOnDomain(pParent);
}

template<class View>
bool DomFilterView1<Distinct,IntSeq,View>::init()
{
  	assert(vvg == NULL);
   	vvg = Detail::VarValGraph<Doms>::init(store,doms.size(),doms);
   	if ((vvg == NULL) || !vvg->initial_match())
     		return false;
     return true;
}


template<class View>
bool DomFilterView1<Distinct,IntSeq,View>::execute()
{
	if (btSentinel.hasBacktracked())
		vvg = NULL;
	btSentinel.update();

	if (vvg == NULL)
	{
		if (!init())
      		return false;
	}
	else
	{
		if (!vvg->sync())
    		return false;
	}
    // propagate
 	vvg->mark();
  //  propTimer.resume();
    bool ret = vvg->tell();
 //	propTimer.pause();
    return ret;
}

template<class View>
struct PostDomFilter1<Distinct,Seq<int>,View>
{
	static bool post(Store& s,const View& v)
	{
		// Val(distinct) is required (not redundant in this implementation)
		// Bnd(distinct) seems to slow down sometimes.
		return postValFilter(s,distinct(v)) and
			   postBndFilter(s,distinct(v)) and
			   s.post(new (s) DomFilterView1<Distinct,Seq<int>,View>(s,v));
	}
};

} // CP
} // Casper

#endif
