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

#ifndef _H_CASPER_INT_ELEMENT_GAC
#define _H_CASPER_INT_ELEMENT_GAC

#include <casper/int/element/common.h>

//#define ELEMENT_USE_WATCHED_LITERALS


#ifdef ELEMENT_USE_WATCHED_LITERALS

#include <casper/kernel/filter/gacfilter.h>

namespace casper {


// DOES NOT WORK !!

template<class View>
struct WatchedLiteral : INotifier
{
	typedef typename DomTraits<View>::Dom	Dom;
	typedef typename EvalTraits<View>::Eval	Eval;

	WatchedLiteral(ICPSolver& solver,const View& view, const Eval& val) :
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
	Reversible<Bool> 	on;
};

template<class View>
WatchedLiteral<View>* watchedLiteral(ICPSolver& s,const View& var,
					typename EvalTraits<View>::Eval val)
{	return new (s.heap()) WatchedLiteral<View>(s,var,val);	}


template<class ArrayView>
struct WatchedLiteralTable : INotifier
{
	typedef typename EvalTraits<ArrayView>::Eval::Elem	Value;

	struct ValWatch : INotifier
	{
		WatchedLiteralTable&	rOwner;
		UInt			varIdx;
		Value 			value;
		INotifiable*	pParent;
		ValWatch(WatchedLiteralTable* pOwner,
			UInt varIdx, Value value) :
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

	typedef detail::List<ValWatch*>					WatchedLiteralList;
	typedef typename WatchedLiteralList::Iterator 	Iterator;

	struct VarWatch : INotifiable
	{
		WatchedLiteralTable& rOwner;
		const UInt varIdx;
		WatchedLiteralList	valWatches;
		VarWatch(WatchedLiteralTable* pOwner, UInt varIdx) :
			rOwner(*pOwner),varIdx(varIdx),
			valWatches(pOwner->doms.solver().sHeap()) // FIXME: why sHeap ??
		{}
		void attach(INotifiable*)
		{	rOwner.doms[varIdx]->attachOnDomain(this);	}
		Iterator watch(INotifiable* pNotifiable, Value value)
		{
			ValWatch* pvw = new (rOwner.doms.solver().heap())
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

	WatchedLiteralTable(ICPSolver& solver,const ArrayView& vars) :
		doms(solver,vars),
		varWatches(solver.heap(),doms.size()),
		on(solver,false)
	{
		for (UInt i = 0; i < doms.size(); i++)
			varWatches[i] = new (solver.heap()) VarWatch(this,i);
	}

	void attach(INotifiable*)
	{
		for (UInt i = 0; i < doms.size(); i++)
			varWatches[i]->attach(NULL);
		on = true;
	}

	void detach(INotifiable*)
	{
		on = false;
		//for (UInt i = 0; i < vars.size(); i++)
		//	varWatches[i]->destroy();
	}

	Iterator watch(INotifiable* pNotifiable, UInt varIdx, Value value)
	{	return varWatches[varIdx]->watch(pNotifiable,value);	}

	bool notify()
	{ assert(0); return false; }

	Iterator updateWatchedVarIdx(Iterator it, UInt newVarIdx)	// O(1)
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

	DomArrayView<Int,ArrayView> 	doms;
	//WatchedLiteralTable_	wl;
	//UInt	curNotifyingVarIdx;
	//Value	curNotifyingValue;
	Array<VarWatch*>		varWatches;
	Reversible<Bool>		on;
};

/*
 *  Propagator for enforcing domain consistency
 *  on the element constraint.
 */

template<class ArrayView,class IdxView,class ResView>
struct DomFilterView3<Element,IntSeq,ArrayView,Int,IdxView,Int,ResView> : IFilter
{
	typedef typename DomTraits<ArrayView>::Dom	ArrayDom;
	typedef typename DomTraits<IdxView>::Dom	IdxDom;
	typedef typename DomTraits<ResView>::Dom	ResDom;

	// fires when index is ground
	struct ArrayDemon : INotifiable
	{
		DomFilterView3& rOwner;
		ArrayDemon(DomFilterView3* pOwner) : rOwner(*pOwner)
		{ rOwner.idx->attachOnGround(this); }
		bool notify()
		{
			assert(rOwner.idx->singleton());
			rOwner.solver().post( rOwner.array[*rOwner.idx->begin()].getView() == rOwner.eval.getView());
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
		const UInt varIdx;
		WatchedLiteral<ResView>* wlEval;
		typename WatchedLiteralTable<ArrayView>::Iterator lit;
		IdxDemon(DomFilterView3* pOwner, UInt varIdx) :
			rOwner(*pOwner),
			varIdx(varIdx)
		{
			Int value;
			typename ArrayDom::Iterator it = rOwner.array[varIdx]->begin();
			for (;
			 	it != rOwner.array[varIdx]->end(); ++it)
			 	if (rOwner.eval->find(*it) != rOwner.eval->end())
				{
					value = *it;
					break;
				}
			lit = rOwner.wlt.watch(this,varIdx,value);
			wlEval = watchedLiteral(rOwner.solver(),rOwner.eval.getView(),value);
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
		Int  value;
		//WatchedLiteral<ArrayDom>* wlArray;
		WatchedLiteral<IdxView>* wlIdx;
		typename WatchedLiteralTable<ArrayView>::Iterator lit;
		EvalDemon(DomFilterView3* pOwner, UInt varIdx) :
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
			wlIdx = watchedLiteral(rOwner.solver(),rOwner.idx.getView(),(Int)varIdx);
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
    DomFilterView3(ICPSolver&, const ArrayView&, const IdxView&, const ResView&);

	Bool	execute();
	Bool	entailed() const {	return false;	}	// FIXME
	Void 	attach(INotifiable* pParent);
	Bool 	notify() { assert(0); /* demon */ return true; }
	Cost 	cost() const {	return linearHi; }
	void 	detach(INotifiable* pParent);

  private:

	DomArrayView<Int,ArrayView>		array;
	DomView<Int,IdxView>			idx;
    DomView<Int,ResView>			eval;
    detail::RStack<IdxDemon*>		pendingIdxDemons;
    detail::RStack<EvalDemon*>		pendingEvalDemons;
    ArrayDemon*						arrayDemon;
    Array<IdxDemon*>				idxDemons;
    Array<EvalDemon*>				evalDemons;
    Reversible<Bool>				toDetach;
    WatchedLiteralTable<ArrayView>	wlt;
    INotifiable*					pParent;
};

template<class ArrayView,class IdxView,class ResView>
Filter elementDom( const ArrayView& vars, const IdxView& idx,
				   const ResView& res)
{	return Dom(rel<Element>(vars,idx,res));	}


template<class ArrayView,class IdxView,class ResView>
DomFilterView3<Element,IntSeq,ArrayView,Int,IdxView,Int,ResView>::
DomFilterView3( ICPSolver& solver, const ArrayView& array, const IdxView& idx,
    			const ResView& res) :
    		IFilter(solver),
    		array(solver,array),
    		idx(solver,idx),
    		eval(solver,res),
    		pendingIdxDemons(solver),
    		pendingEvalDemons(solver),
    		idxDemons(solver.heap(),array.size()),
    		evalDemons(solver.heap(),array.size()),
    		toDetach(solver,false),
    		wlt(solver,array)
{
	// build idx-var list
}


template<class ArrayView,class IdxView,class ResView>
Void DomFilterView3<Element,IntSeq,ArrayView,Int,IdxView,Int,ResView>::
attach(INotifiable* pParent)
{
	this->pParent = pParent;
	wlt.attach(this);
	for (UInt i = 0; i < array.size(); i++)
	{
		evalDemons[i] = new (array.solver().heap()) EvalDemon(this,i);
		idxDemons[i] = new (array.solver().heap()) IdxDemon(this,i);
	}
	arrayDemon = new (array.solver().heap()) ArrayDemon(this);
}

template<class ArrayView,class IdxView,class ResView>
Void DomFilterView3<Element,IntSeq,ArrayView,Int,IdxView,Int,ResView>::
detach(INotifiable* pParent)
{
	arrayDemon->detach();
	for (UInt i = 0; i < array.size(); i++)
	{
		evalDemons[i]->detach();
		idxDemons[i]->detach();
	}
	wlt.detach(this);
	//this->pParent = NULL;
}

template<class ArrayView,class IdxView,class ResView>
Bool DomFilterView3<Element,IntSeq,ArrayView,Int,IdxView,Int,ResView>::execute()
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

}
#else

#include <casper/kernel/util.h>	// FIXME
#include <casper/int/element/bac.h>

namespace casper {

/**
 * 	Incomplete GAC propagation based on basic GAC3.
 *  \TODO improve: 1. using deltas 2. using supports (GAC6)
 */
template<class ArrayView,class IdxView,class EvalView>
struct DomFilterView3<Element,IntSeq,ArrayView,Int,IdxView,Int,EvalView> : IFilter
{
	DomFilterView3(ICPSolver&, const ArrayView&, const IdxView&, const EvalView&);

 	Bool	execute();
	Bool	entailed() const {	return false;	}	// FIXME
	Void 	attach(INotifiable* f);
	void 	detach(INotifiable* f);
	Cost 	cost() const {	return quadraticHi; }
	Filter  operator!() { return Dom(array.getView()[idx.getView()]!=eval.getView()); }

	private:

	DomArrayView<Int,ArrayView>		array;
	DomView<Int,IdxView>			idx;
    DomView<Int,EvalView>			eval;
};

template<class ArrayView,class IdxView,class EvalView>
Filter elementDom( const ArrayView& vars, const IdxView& idx,
				   const EvalView& res)
{	return Dom(rel<Element>(vars,idx,res));	}


template<class ArrayView,class IdxView,class EvalView>
DomFilterView3<Element,IntSeq,ArrayView,Int,IdxView,Int,EvalView>::
DomFilterView3( ICPSolver& solver, const ArrayView& array,
    			const IdxView& idx,const EvalView& eval) :
    IFilter(solver),array(solver,array),idx(solver,idx),eval(solver,eval)
{}


template<class ArrayView,class IdxView,class EvalView>
Void DomFilterView3<Element,IntSeq,ArrayView,Int,IdxView,Int,EvalView>::
attach(INotifiable* f)
{
	for (UInt i = 0; i < array.size(); i++)
		array[i]->attachOnDomain(f);
	idx->attachOnDomain(f);
	eval->attachOnDomain(f);
}

template<class ArrayView,class IdxView,class EvalView>
Void DomFilterView3<Element,IntSeq,ArrayView,Int,IdxView,Int,EvalView>::
detach(INotifiable* f)
{
	for (UInt i = 0; i < array.size(); i++)
		array[i]->detachOnDomain(f);
	idx->detachOnDomain(f);
	eval->detachOnDomain(f);
}

#ifdef CASPER_ELEMENT_IDX_1
template<class ArrayView,class IdxView,class EvalView>
Bool DomFilterView3<Element,IntSeq,ArrayView,Int,IdxView,Int,EvalView>::
execute()
{
	typedef typename DomTraits<typename ElemTraits<IdxView>::Elem>::Dom	IdxDom;

	if (idx->ground())		// FIXME: should post a gac == and detach
		return eval->intersect(array[idx->value()-1]->begin(),
							   array[idx->value()-1]->end()) and
			   array[idx->value()-1]->intersect(eval->begin(),
			   							   	  eval->end());

	for (typename IdxDom::Iterator idxIt = idx->begin();
			idxIt != idx->end(); )
		if (!detail::intersect(array[*idxIt-1]->begin(),array[*idxIt-1]->end(),
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
Bool DomFilterView3<Element,IntSeq,ArrayView,Int,IdxView,Int,EvalView>::
execute()
{
	typedef typename DomTraits<typename ElemTraits<IdxView>::Elem>::Dom	IdxDom;

	if (idx->ground())		// FIXME: should post a gac == and detach
		return eval->intersect(array[idx->value()]->begin(),
							   array[idx->value()]->end()) and
			   array[idx->value()]->intersect(eval->begin(),
			   							   	  eval->end());

	for (typename IdxDom::Iterator idxIt = idx->begin();
			idxIt != idx->end(); )
		if (!detail::intersect(array[*idxIt]->begin(),array[*idxIt]->end(),
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
struct DomFilterFact3<Element,IntSeq,ArrayView,Int,IdxView,Int,EvalView>
{
	static Filter create(ICPSolver& s,const ArrayView& a,const IdxView& idx,
							const EvalView& eval)
	{
		return Bnd(element(a,idx,eval)) and
				Filter(new (s.heap()) DomFilterView3<Element,IntSeq,ArrayView,
										Int,IdxView,Int,EvalView>(s,a,idx,eval));
	}
};

}

#endif /*  ELEMENT_USE_WATCHED_LITERALS */


#endif /*_H_CASPER_INT_ELEMENT_GAC*/
