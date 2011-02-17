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

#ifndef _H_CASPER_INT_DISTINCT_VAL
#define _H_CASPER_INT_DISTINCT_VAL

#define  USE_CASPER_VAL_ALLDIFF

#include <casper/kernel/filter/valfilter.h>
#include <casper/kernel/container/rstack.h>
#include <casper/int/distinct/common.h>
#include <casper/int/variable.h>
#include <casper/kernel/view/arrayview.h>
#include <casper/kernel/timer.h>	// tmp

namespace casper {

#if 0
/**
 *	Filter that enforces node consistency on the distinct
 *  constraint over a sequence of integer variables. Equivalent
 *  to (n*(n-1))/2 inequality constraints on each pair of
 *  variables (but faster).
 *
 *  \ingroup IntFilters
 */
template<class View>
struct ValFilterView1<Distinct,IntSeq,View> : IFilter
{
	ValFilterView1(ICPSolver& solver, const View& v) :
		IFilter(solver),vars(v),toRemove(solver),
		minValue(findMinElem(v)),
		assignedVals(solver,findMaxElem(v)-minValue+1,false),
		pParent(NULL)
		{}

	Bool	execute();
	Bool	entailed() const {	return false;	}			// FIXME
	Cost 	cost() const {	return linearLo; }
	void attach(INotifiable* pParent);
	void detach(INotifiable* pParent);

	private:
	struct VarDemon : INotifiable
	{
		ValFilterView1& rOwner;
		UInt varIdx;
		VarDemon(ValFilterView1* pOwner, UInt varIdx) :
			rOwner(*pOwner),varIdx(varIdx)
		{ rOwner.vars[varIdx].domain().attachOnGround(this); }
		bool notify()
		{
			assert(rOwner.vars[varIdx].domain().singleton());
			UInt valIdx = *rOwner.vars[varIdx].domain().begin()-
						   rOwner.minValue;
			if (rOwner.assignedVals[valIdx])
				return false;
			rOwner.assignedVals[valIdx] = true;
			rOwner.toRemove.push(varIdx);
			return rOwner.pParent->notify();
		}
	};

	DomVarArray<Int>			vars;
	detail::RStack<UInt>		toRemove;
	Int							minValue;
	RevArray<Bool>				assignedVals;
	INotifiable*				pParent;
};

template<class View>
void ValFilterView1<Distinct,IntSeq,View>::attach(INotifiable* pParent)
{
	assert(this->pParent==NULL);
	this->pParent= pParent;
	for (UInt i = 0; i < vars.size(); i++)
		new (solver().heap()) VarDemon(this,i);
}

template<class View>
void ValFilterView1<Distinct,IntSeq,View>::detach(INotifiable* pParent)
{
	this->pParent= NULL;
	// TODO: kill demons
	//for (UInt i = 0; i < vars.size(); i++)
	//	new (solver().heap()) VarDemon(this,i);
}

template<class View>
Bool ValFilterView1<Distinct,IntSeq,View>::execute()
{
	while (!toRemove.empty())
	{
		UInt curToRemove = toRemove.top();
		toRemove.pop();
		assert(!vars[curToRemove].domain().empty());
		Int	val = *vars[curToRemove].domain().begin();
		for (UInt i = 0; i < vars.size(); i++)
			if (i != curToRemove and
				!vars[i].domain().erase(val))
				return false;
	}
	return true;
}

#else


template<class View>
struct ValFilterView1<Distinct,IntSeq,View> : IFilter
{
	ValFilterView1(ICPSolver& solver, const View& v) :
		IFilter(solver),doms(solver,v),toRemove(solver),
		minValue(findMin(v)),
		assignedVars(solver,findMax(v)-minValue+1,-1),
		pParent(NULL)
		{}

	Bool	execute();
	Bool	entailed() const {	return false;	}			// FIXME
	Cost 	cost() const {	return linearLo; }
	void attach(INotifiable* pParent);
	void detach(INotifiable* pParent);

	private:
	struct VarDemon : INotifiable
	{
		ValFilterView1& rOwner;
		UInt varIdx;
		VarDemon(ValFilterView1* pOwner, UInt varIdx) :
			rOwner(*pOwner),varIdx(varIdx)
		{ rOwner.doms[varIdx]->attachOnGround(this); }
		bool notify()
		{
			assert(rOwner.doms[varIdx]->singleton());
			UInt valIdx = *rOwner.doms[varIdx]->begin()-
						   rOwner.minValue;
			if (rOwner.assignedVars[valIdx] >= 0)
				return static_cast<UInt>(rOwner.assignedVars[valIdx]) == varIdx;
			rOwner.assignedVars[valIdx] = varIdx;
			return rOwner.pParent->notify();
		}
		// tmp
		INotifiable* getFirst() { return rOwner.pParent; }
	};

	DomArrayView<Int,View>		doms;
	detail::RStack<UInt>		toRemove;
	Int							minValue;
	RevArray<Int>				assignedVars;
	INotifiable*				pParent;
};

template<class View>
void ValFilterView1<Distinct,IntSeq,View>::attach(INotifiable* pParent)
{
//	assert(this->pParent==NULL);
	this->pParent= pParent;
	for (UInt i = 0; i < doms.size(); i++)
		new (solver().heap()) VarDemon(this,i);
}

template<class View>
void ValFilterView1<Distinct,IntSeq,View>::detach(INotifiable* pParent)
{
	this->pParent= NULL;
	// TODO: kill demons
	//for (UInt i = 0; i < vars.size(); i++)
	//	new (solver().heap()) VarDemon(this,i);
}

template<class View>
Bool ValFilterView1<Distinct,IntSeq,View>::execute()
{
	typedef typename DomTraits<typename ElemTraits<View>::Elem>::Dom::Iterator Iterator;
	for (UInt i = 0; i < doms.size(); i++)
		for (Iterator it = doms[i]->begin(); it != doms[i]->end(); )
			if (assignedVars[*it-minValue]>=0)
			{
				if (static_cast<UInt>(assignedVars[*it-minValue])==i)
				{	assert(doms[i]->singleton()); ++it; }
				else
				if (!doms[i]->erase(it++))
					return false;
			}
			else
				++it;
	return true;
}
#endif

};

#endif
