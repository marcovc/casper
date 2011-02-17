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

#ifndef _H_CASPER_INT_DISTINCT_GAC
#define _H_CASPER_INT_DISTINCT_GAC

#define	 USE_GECODE_DOM_ALLDIFF

#include <casper/kernel/filter/gacfilter.h>
#include <casper/kernel/container/array.h>
#include <casper/int/distinct/common.h>
#include <casper/int/distinct/val.h>
#include <casper/int/variable.h>

#include <casper/int/distinct/gac-gecode.h>

namespace casper {

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
	DomFilterView1(ICPSolver& solver, const View& v);

	struct VarDemon : INotifiable
	{
		VarDemon(DomFilterView1* pOwner,UInt idx) : rOwner(*pOwner),
													idx(idx)
		{	rOwner.doms[idx]->attachOnDomain(this);	}
		bool notify()
		{	return rOwner.doms[idx]->ground() or rOwner.pParent->notify();}
		// tmp
		INotifiable* getFirst() { return rOwner.pParent; }

		DomFilterView1& rOwner;
		UInt idx;
	};

	Bool	execute();
	Bool	entailed() const {	return false;	}	// FIXME
	void 	attach(INotifiable* pParent);
	void 	detach(INotifiable* pParent);
	Cost 	cost() const {	return quadraticHi; }

	private:
	bool	init();
	//DomVarArray<Int> vars;
	typedef DomArrayView<Int,View>	Doms;
	Doms						doms;
	detail::VarValGraph<Doms>* 	vvg;
	Counter						nfails;
	INotifiable*				pParent;
};

template<class View>
DomFilterView1<Distinct,IntSeq,View>::DomFilterView1(ICPSolver& solver, const View& v) :
		IFilter(solver),
		doms(solver,v),
		vvg(NULL),
		nfails(0)
{}

template<class View>
void DomFilterView1<Distinct,IntSeq,View>::attach(INotifiable* pParent)
{
	this->pParent = pParent;
	for (UInt i = 0; i < doms.size(); i++)
		new (solver().heap()) VarDemon(this,i);
		//vars[i].domain().attachOnDomain(this);


}

template<class View>
void DomFilterView1<Distinct,IntSeq,View>::detach(INotifiable* pParent)
{
	for (UInt i = 0; i < doms.size(); i++)
		doms[i]->detachOnDomain(pParent);
}

template<class View>
Bool DomFilterView1<Distinct,IntSeq,View>::init()
{
  	assert(vvg == NULL);
   	vvg = detail::VarValGraph<Doms>::init(solver(),doms.size(),doms);
   	if ((vvg == NULL) || !vvg->initial_match())
     		return false;
     return true;
}


template<class View>
Bool DomFilterView1<Distinct,IntSeq,View>::execute()
{
	if (solver().fails() > nfails)
		vvg = NULL;
	nfails = solver().fails();

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
struct DomFilterFact1<Distinct,Seq<Int>,View>
{
	static Filter create(ICPSolver& s,const View& v)
	{
		// Val(distinct) is required (not redundant in this implementation)
		// Bnd(distinct) seems to slow down sometimes.
		return Val(distinct(v)) and
				Bnd(distinct(v)) and
				Filter(new (s.heap()) DomFilterView1<Distinct,Seq<Int>,View>(s,v));
	}
};

};

#endif
