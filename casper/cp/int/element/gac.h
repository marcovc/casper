 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_CP_INT_ELEMENT_GAC
#define _H_CASPER_CP_INT_ELEMENT_GAC

#include <casper/cp/int/element/common.h>
#include <casper/util/util.h>

//#define ELEMENT_USE_WATCHED_LITERALS


#ifdef ELEMENT_USE_WATCHED_LITERALS

#include <casper/kernel/filter/gacfilter.h>

namespace Casper {
namespace CP {

// DOES NOT WORK !!

template<class View>
struct WatchedLiteral : INotifier
{
	typedef typename Traits::GetDom<View>::Type	Dom;
	typedef typename Casper::Traits::GetEval<View>::Type	Eval;

	WatchedLiteral(CPSolver& solver,const View& view, const Eval& val) :
		dom(&*DomView<Eval,View>(solver,view)),val(val),on(solver,false) {}

	void attach(INotifiable* pParent)
	{
		this->pParent = pParent;
		dom->attachOnDomain(this);
		on = true;
	}


	void detach(INotifiable* pParent)
	{
		//var.domain().detachOnDomain(this);
		on = false;
	}

	bool notify()
	{
		if (!on)
			return true;
		bool ret = true;
		if (val >= *dom->delta().first() &&
			val <= *dom->delta().last())
		{
			ret = pParent->notify();
			//var.domain().detachOnDomain(this);
			on = false;
		}
		return ret;
	}

	void moveToVar(const View& s)	// costly
	{
		dom->detachOnDomain(this);
		dom = &*DomView<Eval,View>(on.solver(),s);
		dom->attachOnDomain(this);
	}

	void moveToVal(Eval s)
	{	val = s;	}

	INotifiable* 		pParent;
	Dom*				dom;
	Eval				val;
	Reversible<bool> 	on;
};

template<class View>
WatchedLiteral<View>* watchedLiteral(CPSolver& s,const View& var,
					typename Casper::Traits::GetEval<View>::Type val)
{	return new (s.getHeap()) WatchedLiteral<View>(s,var,val);	}


template<class ArrayView>
struct WatchedLiteralTable : INotifier
{
	typedef typename Casper::Traits::GetEval<ArrayView>::Type::Elem	Value;

	struct ValWatch : INotifier
	{
		WatchedLiteralTable&	rOwner;
		uint			varIdx;
		Value 			value;
		INotifiable*	pParent;
		ValWatch(WatchedLiteralTable* pOwner,
			uint varIdx, Value value) :
			rOwner(*pOwner),varIdx(varIdx),value(value) {}
		void attach(INotifiable* pParent)
		{	this->pParent = pParent;	}
		bool notify()
		{
			if (value >= *rOwner.doms[varIdx]->delta().first() &&
				value <= *rOwner.doms[varIdx]->delta().last())
				return pParent->notify();
			return true;
		}
		void detach(INotifiable* pParent)
		{}
	};

	typedef Util::StdList<ValWatch*>					WatchedLiteralList;
	typedef typename WatchedLiteralList::Iterator 	Iterator;

	struct VarWatch : INotifiable
	{
		WatchedLiteralTable& rOwner;
		const uint varIdx;
		WatchedLiteralList	valWatches;
		VarWatch(WatchedLiteralTable* pOwner, uint varIdx) :
			rOwner(*pOwner),varIdx(varIdx),
			valWatches(pOwner->doms.solver().sHeap()) // FIXME: why sHeap ??
		{}
		void attach(INotifiable*)
		{	rOwner.doms[varIdx]->attachOnDomain(this);	}
		Iterator watch(INotifiable* pNotifiable, Value value)
		{
			ValWatch* pvw = new (rOwner.doms.solver().getHeap())
					ValWatch(&rOwner,varIdx,value);
			pvw->attach(pNotifiable);
			return valWatches.pushFront(pvw);
		}
		bool notify()
		{
			if (!rOwner.on)
				return true;
			//std::cout << &rOwner << " " << varIdx << " : ";
			//std::cout.flush();
			//std::cout << valWatches.size() << std::endl;
			for (Iterator it = valWatches.begin();
					it != valWatches.end(); ++it)
			{
				//std::cout << *it << std::endl;
				if (!(*it)->notify())
					return false;
			}
			return true;
		}
		void detach(INotifiable*)
		{	rOwner.doms[varIdx]->detachOnDomain(this);	}
	};

	WatchedLiteralTable(CPSolver& solver,const ArrayView& vars) :
		doms(solver,vars),
		varWatches(solver.getHeap(),doms.size()),
		on(solver,false)
	{
		for (uint i = 0; i < doms.size(); i++)
			varWatches[i] = new (solver.getHeap()) VarWatch(this,i);
	}

	void attach(INotifiable*)
	{
		for (uint i = 0; i < doms.size(); i++)
			varWatches[i]->attach(NULL);
		on = true;
	}

	void detach(INotifiable*)
	{
		on = false;
		//for (uint i = 0; i < vars.size(); i++)
		//	varWatches[i]->destroy();
	}

	Iterator watch(INotifiable* pNotifiable, uint varIdx, Value value)
	{	return varWatches[varIdx]->watch(pNotifiable,value);	}

	bool notify()
	{ assert(0); return false; }

	Iterator updateWatchedVarIdx(Iterator it, uint newVarIdx)	// O(1)
	{
		assert(newVarIdx != (*it)->varIdx);
		//std::cout << *it << ": update varIdx : " << (*it)->varIdx << " -> " << newVarIdx << std::endl;
		varWatches[(*it)->varIdx]->valWatches.erase(it);
		(*it)->varIdx = newVarIdx;
		return varWatches[newVarIdx]->valWatches.pushFront(*it);
	}

	void updateWatchedValue(Iterator it, Value newValue)	// O(1)
	{
		(*it)->value = newValue;
	}

	DomArrayView<int,ArrayView> 	doms;
	//WatchedLiteralTable_	wl;
	//uint	curNotifyingVarIdx;
	//Value	curNotifyingValue;
	Util::StdArray<VarWatch*>		varWatches;
	Reversible<bool>		on;
};

/*
 *  Propagator for enforcing domain consistency
 *  on the element constraint.
 */

template<class ArrayView,class IdxView,class ResView>
struct DomFilterView3<Element,IntSeq,ArrayView,int,IdxView,int,ResView> : IFilter
{
	typedef typename Traits::GetDom<ArrayView>::Type	ArrayDom;
	typedef typename Traits::GetDom<IdxView>::Type	IdxDom;
	typedef typename Traits::GetDom<ResView>::Type	ResDom;

	// fires when index is ground
	struct ArrayDemon : INotifiable
	{
		DomFilterView3& rOwner;
		ArrayDemon(DomFilterView3* pOwner) : rOwner(*pOwner)
		{ rOwner.idx->attachOnGround(this); }
		bool notify()
		{
			assert(rOwner.idx->singleton());
			rOwner.solver().post( rOwner.array[*rOwner.idx->begin()].getObj() == rOwner.eval.getObj());
			rOwner.toDetach = true;
			return true;
		}
		void detach()
		{	 rOwner.idx->detachOnGround(this);  }
	};

	// fires at removal of value from array[varIdx]=value or result=value
	struct IdxDemon : INotifiable
	{
		DomFilterView3& rOwner;
		const uint varIdx;
		WatchedLiteral<ResView>* wlEval;
		typename WatchedLiteralTable<ArrayView>::Iterator lit;
		IdxDemon(DomFilterView3* pOwner, uint varIdx) :
			rOwner(*pOwner),
			varIdx(varIdx)
		{
			int value;
			typename ArrayDom::Iterator it = rOwner.array[varIdx]->begin();
			for (;
			 	it != rOwner.array[varIdx]->end(); ++it)
			 	if (rOwner.eval->find(*it) != rOwner.eval->end())
				{
					value = *it;
					break;
				}
			lit = rOwner.wlt.watch(this,varIdx,value);
			wlEval = watchedLiteral(rOwner.solver(),rOwner.eval.getObj(),value);
			wlEval->attach(this);
		}
		bool notify()
		{
			for (typename ArrayDom::Iterator it = rOwner.array[varIdx]->begin();
			 	it != rOwner.array[varIdx]->end(); ++it)
			 	if (rOwner.eval->find(*it) != rOwner.eval->end())
			 	{
			 		rOwner.wlt.updateWatchedValue(lit,*it);
			 		wlEval->moveToVal(*it);
					return true;
			 	}
			rOwner.pendingIdxDemons.push(this);
			return rOwner.pParent->notify();
		}
		void detach()
		{	/*wlArray->detach(this);*/ wlEval->detach(this);	}
	};

	// fires at removal of index=value or array[value]=something
	struct EvalDemon : INotifiable
	{
		DomFilterView3& rOwner;
		int  value;
		//WatchedLiteral<ArrayDom>* wlArray;
		WatchedLiteral<IdxView>* wlIdx;
		typename WatchedLiteralTable<ArrayView>::Iterator lit;
		EvalDemon(DomFilterView3* pOwner, uint varIdx) :
			rOwner(*pOwner),
			value(*rOwner.array[varIdx]->begin())
		{
			typename ResDom::Iterator it = rOwner.eval->begin();
			for (;
				it != rOwner.eval->end(); ++it)
			{
				value = *it;
				typename IdxDom::Iterator it = rOwner.idx->begin();
				for (;
			 		it != rOwner.idx->end(); ++it)
			 		if (rOwner.array[*it]->find(value) != rOwner.array[*it]->end())
						goto found;
			}
			//std::cout << "failed to find initial support!\n";
			found:
			//wlArray = watchedLiteral(rOwner.array[varIdx],value);
			lit = rOwner.wlt.watch(this,varIdx,value);
			wlIdx = watchedLiteral(rOwner.solver(),rOwner.idx.getObj(),(int)varIdx);
			//wlArray->attach(this);
			wlIdx->attach(this);
		}
		bool notify()
		{
			for (typename IdxDom::Iterator it = rOwner.idx->begin();
			 	it != rOwner.idx->end(); ++it)
			 	if (rOwner.array[*it]->find(value) != rOwner.array[*it]->end())
			 	{
			 		//wlArray->moveToVar(rOwner.array[*it]);
			 		lit = rOwner.wlt.updateWatchedVarIdx(lit,*it);
			 		wlIdx->moveToVal(*it);
					return true;
			 	}
			rOwner.pendingEvalDemons.push(this);
			return rOwner.pParent->notify();
		}
		void detach()
		{	/*wlArray->detach(this);*/ wlIdx->detach(this);	}
	};

  public:
    DomFilterView3(CPSolver&, const ArrayView&, const IdxView&, const ResView&);

	bool	execute();
	bool	entailed() const {	return false;	}	// FIXME
	void 	attach(INotifiable* pParent);
	bool 	notify() { assert(0); /* demon */ return true; }
	Cost 	cost() const {	return linearHi; }
	void 	detach(INotifiable* pParent);

  private:

	DomArrayView<int,ArrayView>		array;
	DomView<int,IdxView>			idx;
    DomView<int,ResView>			eval;
    Stack<IdxDemon*>		pendingIdxDemons;
    Stack<EvalDemon*>		pendingEvalDemons;
    ArrayDemon*						arrayDemon;
    Util::StdArray<IdxDemon*>				idxDemons;
    Util::StdArray<EvalDemon*>				evalDemons;
    Reversible<bool>				toDetach;
    WatchedLiteralTable<ArrayView>	wlt;
    INotifiable*					pParent;
};

template<class ArrayView,class IdxView,class ResView>
Filter elementDom( const ArrayView& vars, const IdxView& idx,
				   const ResView& res)
{	return Dom(rel<Element>(vars,idx,res));	}


template<class ArrayView,class IdxView,class ResView>
DomFilterView3<Element,IntSeq,ArrayView,int,IdxView,int,ResView>::
DomFilterView3( CPSolver& solver, const ArrayView& array, const IdxView& idx,
    			const ResView& res) :
    		IFilter(solver),
    		array(solver,array),
    		idx(solver,idx),
    		eval(solver,res),
    		pendingIdxDemons(solver),
    		pendingEvalDemons(solver),
    		idxDemons(solver.getHeap(),array.size()),
    		evalDemons(solver.getHeap(),array.size()),
    		toDetach(solver,false),
    		wlt(solver,array)
{
	// build idx-var list
}


template<class ArrayView,class IdxView,class ResView>
void DomFilterView3<Element,IntSeq,ArrayView,int,IdxView,int,ResView>::
attach(INotifiable* pParent)
{
	this->pParent = pParent;
	wlt.attach(this);
	for (uint i = 0; i < array.size(); i++)
	{
		evalDemons[i] = new (array.solver().getHeap()) EvalDemon(this,i);
		idxDemons[i] = new (array.solver().getHeap()) IdxDemon(this,i);
	}
	arrayDemon = new (array.solver().getHeap()) ArrayDemon(this);
}

template<class ArrayView,class IdxView,class ResView>
void DomFilterView3<Element,IntSeq,ArrayView,int,IdxView,int,ResView>::
detach(INotifiable* pParent)
{
	arrayDemon->detach();
	for (uint i = 0; i < array.size(); i++)
	{
		evalDemons[i]->detach();
		idxDemons[i]->detach();
	}
	wlt.detach(this);
	//this->pParent = NULL;
}

template<class ArrayView,class IdxView,class ResView>
bool DomFilterView3<Element,IntSeq,ArrayView,int,IdxView,int,ResView>::execute()
{
	if (toDetach)
	{
		detach(pParent);
		return true;
	}

	while (!pendingIdxDemons.empty())
	{
		if (!idx->erase(pendingIdxDemons.top()->varIdx))
			return false;
		pendingIdxDemons.pop();
	}

	while (!pendingEvalDemons.empty())
	{
		if (!eval->erase(pendingEvalDemons.top()->value))
			return false;
		pendingEvalDemons.pop();
	}
	return true;
}

} // CP
} // Casper
#else

//#include <casper/kernel/util.h>	// FIXME
#include <casper/cp/int/element/bac.h>

namespace Casper {
namespace CP {

/**
 * 	Incomplete GAC propagation based on basic GAC3.
 *  \TODO improve: 1. using deltas 2. using supports (GAC6)
 */
template<class ArrayView,class IdxView,class EvalView>
struct DomFilterView3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView> : IFilter
{
	DomFilterView3(Store&, const ArrayView&, const IdxView&, const EvalView&);

 	bool	execute();

	void 	attach(INotifiable* f);
	void 	detach(INotifiable* f);
	Cost 	cost() const {	return quadraticHi; }
	Filter  operator!() { return Dom(array.getObj()[idx.getObj()]!=eval.getObj()); }

	private:

	DomArrayView<int,ArrayView>		array;
	DomView<int,IdxView>			idx;
    DomView<int,EvalView>			eval;
};

/*
template<class ArrayView,class IdxView,class EvalView>
Filter elementDom( const ArrayView& vars, const IdxView& idx,
				   const EvalView& res)
{	return Dom(rel<Element>(vars,idx,res));	}
*/

template<class ArrayView,class IdxView,class EvalView>
DomFilterView3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView>::
DomFilterView3(Store& store, const ArrayView& array,
    			const IdxView& idx,const EvalView& eval) :
    IFilter(store),array(store,array),idx(store,idx),eval(store,eval)
{}


template<class ArrayView,class IdxView,class EvalView>
void DomFilterView3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView>::
attach(INotifiable* f)
{
	for (uint i = 0; i < array.size(); i++)
		array[i]->attachOnDomain(f);
	idx->attachOnDomain(f);
	eval->attachOnDomain(f);
}

template<class ArrayView,class IdxView,class EvalView>
void DomFilterView3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView>::
detach(INotifiable* f)
{
	for (uint i = 0; i < array.size(); i++)
		array[i]->detachOnDomain(f);
	idx->detachOnDomain(f);
	eval->detachOnDomain(f);
}

#ifdef CASPER_ELEMENT_IDX_1
template<class ArrayView,class IdxView,class EvalView>
bool DomFilterView3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView>::
execute()
{
	typedef typename Traits::GetDom<typename Casper::Traits::GetTermElem<IdxView>::Type>::Type	IdxDom;

	if (idx->ground())		// FIXME: should post a gac == and detach
		return eval->intersect(array[idx->value()-1]->begin(),
							   array[idx->value()-1]->end()) and
			   array[idx->value()-1]->intersect(eval->begin(),
			   							   	  eval->end());

	for (typename IdxDom::Iterator idxIt = idx->begin();
			idxIt != idx->end(); )
		if (!Detail::intersect(array[*idxIt-1]->begin(),array[*idxIt-1]->end(),
							   eval->begin(),eval->end()))
		{
			if (!idx->erase(idxIt++))
				return false;
		}
		else
			++idxIt;
	return true;
}
#else
template<class ArrayView,class IdxView,class EvalView>
bool DomFilterView3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView>::
execute()
{
	typedef typename Traits::GetDom<IdxView>::Type	IdxDom;

	if (idx->ground())		// FIXME: should post a gac == and detach
		return eval->intersect(array[idx->value()]->begin(),
							   array[idx->value()]->end()) and
			   array[idx->value()]->intersect(eval->begin(),
			   							   	  eval->end());

	for (typename IdxDom::Iterator idxIt = idx->begin();
			idxIt != idx->end(); )
		if (!Util::intersect(array[*idxIt]->begin(),array[*idxIt]->end(),
							   eval->begin(),eval->end()))
		{
			if (!idx->erase(idxIt++))
				return false;
		}
		else
			++idxIt;
	return true;
}
#endif

template<class ArrayView,class IdxView,class EvalView>
struct PostDomFilter3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView>
{
	static bool post(Store& s,const ArrayView& a,const IdxView& idx,
							const EvalView& eval)
	{
		return postBndFilter(s,elementEqual(a,idx,eval)) and
			   s.post(new (s) DomFilterView3<ElementEqual,IntSeq,ArrayView,
										int,IdxView,int,EvalView>(s,a,idx,eval));
	}
};

} // CP
} // Casper

#endif /*  ELEMENT_USE_WATCHED_LITERALS */


#endif /*_H_CASPER_INT_ELEMENT_GAC*/
