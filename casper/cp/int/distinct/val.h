 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <casper/cp/filter/val.h>
#include <casper/kernel/reversible/stack.h>
#include <casper/cp/int/var.h>
#include <casper/cp/view/array.h>
#include <casper/util/timer.h>	// tmp

//#ifdef CASPER_PRECOMPILED
//#include <casper/cp/int/spexpr/explicit_postval.h>
//#endif

namespace Casper {
namespace CP {


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
	ValFilterView1(CPSolver& solver, const View& v) :
		IFilter(solver),vars(v),toRemove(solver),
		minValue(findMinElem(v)),
		assignedVals(solver,findMaxElem(v)-minValue+1,false),
		pParent(NULL)
		{}

	bool	execute();
	bool	entailed() const {	return false;	}			// FIXME
	Cost 	cost() const {	return linearLo; }
	void attach(INotifiable* pParent);
	void detach(INotifiable* pParent);

	private:
	struct VarDemon : INotifiable
	{
		ValFilterView1& rOwner;
		uint varIdx;
		VarDemon(ValFilterView1* pOwner, uint varIdx) :
			rOwner(*pOwner),varIdx(varIdx)
		{ rOwner.vars[varIdx].domain().attachOnGround(this); }
		bool notify()
		{
			assert(rOwner.vars[varIdx].domain().singleton());
			uint valIdx = *rOwner.vars[varIdx].domain().begin()-
						   rOwner.minValue;
			if (rOwner.assignedVals[valIdx])
				return false;
			rOwner.assignedVals[valIdx] = true;
			rOwner.toRemove.push(varIdx);
			return rOwner.pParent->notify();
		}
	};

	VarArray<int>			vars;
	Stack<uint>		toRemove;
	int							minValue;
	Array<bool>				assignedVals;
	INotifiable*				pParent;
};

template<class View>
void ValFilterView1<Distinct,IntSeq,View>::attach(INotifiable* pParent)
{
	assert(this->pParent==NULL);
	this->pParent= pParent;
	for (uint i = 0; i < vars.size(); i++)
		new (solver().getHeap()) VarDemon(this,i);
}

template<class View>
void ValFilterView1<Distinct,IntSeq,View>::detach(INotifiable* pParent)
{
	this->pParent= NULL;
	// TODO: kill demons
	//for (uint i = 0; i < vars.size(); i++)
	//	new (solver().getHeap()) VarDemon(this,i);
}

template<class View>
bool ValFilterView1<Distinct,IntSeq,View>::execute()
{
	while (!toRemove.empty())
	{
		uint curToRemove = toRemove.top();
		toRemove.pop();
		assert(!vars[curToRemove].domain().empty());
		int	val = *vars[curToRemove].domain().begin();
		for (uint i = 0; i < vars.size(); i++)
			if (i != curToRemove and
				!vars[i].domain().erase(val))
				return false;
	}
	return true;
}

#else

/**
 *	Enforces the all distinct
 *  constraint over a sequence of integer variables. Equivalent
 *  to (n*(n-1))/2 inequality constraints on each pair of
 *  variables (but faster).
 *
 *  \ingroup IntFilters
 */
template<class Eval,class View>
struct ValFilterView1<Distinct,Seq<Eval>,View> : IFilter
{
	ValFilterView1(Store& store, const View& v) :
		IFilter(store),store(store),doms(store,v),toRemove(store),
		minValue(Detail::findMin(store,v)),
		assignedVars(store,Detail::findMax(store,v)-minValue+1,-1),
		pParent(NULL)
		{}

	bool	execute();
//	bool	entailed() const {	return false;	}			// FIXME
	Cost 	cost() const {	return linearLo; }
	void attach(INotifiable* pParent);
	void detach();

	private:
	struct VarDemon : INotifiable
	{
		ValFilterView1& rOwner;
		uint varIdx;
		VarDemon(ValFilterView1* pOwner, uint varIdx) :
			rOwner(*pOwner),varIdx(varIdx)
		{ rOwner.doms[varIdx]->attachOnGround(this); }
		bool notify()
		{
			assert(rOwner.doms[varIdx]->singleton());
			uint valIdx = *rOwner.doms[varIdx]->begin()-
						   rOwner.minValue;
			if (rOwner.assignedVars[valIdx] >= 0)
				return static_cast<uint>(rOwner.assignedVars[valIdx]) == varIdx;
			rOwner.assignedVars[valIdx] = varIdx;
			return rOwner.pParent->notify();
		}
	};

	Store&						store;
	DomArrayView<Eval,View>		doms;
	Stack<uint>					toRemove;
	Eval						minValue;
	Array<int>					assignedVars;
	INotifiable*				pParent;
};

template<class Eval,class View>
void ValFilterView1<Distinct,Seq<Eval>,View>::attach(INotifiable* pParent)
{
//	assert(this->pParent==NULL);
	this->pParent= pParent;
	for (uint i = 0; i < doms.size(); i++)
		new (store) VarDemon(this,i);
}

template<class Eval,class View>
void ValFilterView1<Distinct,Seq<Eval>,View>::detach()
{
	this->pParent= NULL;
	// TODO: kill demons
	//for (uint i = 0; i < vars.size(); i++)
	//	new (solver().getHeap()) VarDemon(this,i);
}

template<class Eval,class View>
bool ValFilterView1<Distinct,Seq<Eval>,View>::execute()
{
	#ifdef CASPER_LOG_OLD
	store.getEnv().log(this, "ValFilterView1<Distinct,Seq<Eval>,View>", Util::Logger::filterExecuteBegin);
	#endif

	typedef typename Traits::GetDom<typename Casper::Traits::GetTermElem<View>::Type>::Type::Iterator Iterator;
	for (uint i = 0; i < doms.size(); i++)
		for (Iterator it = doms[i]->begin(); it != doms[i]->end(); )
			if (assignedVars[*it-minValue]>=0)
			{
				if (static_cast<uint>(assignedVars[*it-minValue])==i)
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

template<class Eval,class Expr1>
struct PostValFilter1<Distinct,Seq<Eval>,Expr1>
{
	static bool post(Store& s,const Expr1& v1)
	{
    	typedef typename Casper::Detail::DeRefAndSimplify<Expr1>::Type SExpr1;
    	const SExpr1 sv1 =  Casper::Detail::DeRefAndSimplify<Expr1>()(v1);
		return s.post(new (s) ValFilterView1<Distinct,Seq<Eval>,SExpr1>(s,sv1));
	}
};

} // CP
} // Casper

#endif
