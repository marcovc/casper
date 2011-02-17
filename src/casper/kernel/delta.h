 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_KERNEL_DELTA_H_
#define CASPER_KERNEL_DELTA_H_

#include <casper/kernel/notifier.h>
#include <casper/kernel/container/rstack.h>
#include <casper/kernel/cpsolver.h>

namespace casper {
namespace detail {

template<class Delta>
struct DeltaStack
{
	struct ClearDemon : INotifiable
	{
		ClearDemon(DeltaStack* pOwner) : rOwner(*pOwner)
		{	}
		Bool notify()
		{	rOwner.onCPQueue = false; return true;	}
		DeltaStack& rOwner;
	};

	DeltaStack(ICPSolver& s) :
		mSolver(s),
		onCPQueue(false),
		cd(this),
		pStack(NULL) {}

	Void			push(const Delta& d);
	const Delta&	top() const { assert(onCPQueue); return pStack->top();	}
	Void			pop()	{ assert(onCPQueue); pStack->pop();	}
	Bool			empty() const {	return !onCPQueue or pStack->empty();	}
	ICPSolver&			solver() const {	return mSolver;	}
	Void			clear()	{ pStack->clear();	}

	ICPSolver&					mSolver;
	Bool					onCPQueue;
	ClearDemon				cd;
	detail::Stack<Delta>*	pStack;
};

template<class Delta>
Void DeltaStack<Delta>::push(const Delta& d)
{
	if (!onCPQueue)
	{
		solver().notifyOnNextCP(&cd);
		onCPQueue = true;
		pStack = new (solver().pHeap())
			detail::Stack<Delta>(solver().pHeap());
	}
	pStack->push(d);
}

template<class View>
struct DomDeltaStack : INotifiable,
					DeltaStack<typename DomTraits<View>::Dom::Delta>
{
	typedef typename EvalTraits<View>::Eval			Eval;
	typedef typename DomTraits<View>::Dom::Delta	DomDelta;
	typedef DeltaStack<DomDelta> Super;
	DomDeltaStack(const View& v) :
		Super(getCPSolver(v)),pOwner(NULL),v(getCPSolver(v),v) {}
	Void setOwner(INotifiable* p) {	pOwner = p;	}
	Bool notify()
	{
		assert(pOwner);
		push(v->delta());
		return pOwner->notify();
	}
	INotifiable* 			pOwner;
	DomView<Eval,View>		v;
};

template<class View>
struct IdxDeltaStack : DeltaStack<UInt>
{
	typedef typename ElemTraits<View>::Elem	Elem;
	typedef typename EvalTraits<Elem>::Eval	Eval;
	typedef typename DomTraits<Elem>::Dom	Dom;
	struct IdxDeltaDemon : INotifiable
	{
		IdxDeltaDemon(IdxDeltaStack* pOwner, UInt idx) :
			rOwner(*pOwner),idx(idx)
		{	rOwner.doms[idx].attach(this);	}
		Bool notify()
		{	rOwner.push(idx); return rOwner.pOwner->notify();	}
		Void detach()
		{	rOwner.doms[idx].detach(this);	}
		IdxDeltaStack&	rOwner;
		UInt			idx;
	};
	typedef DeltaStack<UInt>	Super;
	IdxDeltaStack(const View& v) :
		Super(getCPSolver(v)),
		pOwner(NULL),
		doms(getCPSolver(v),v),
		demons(getCPSolver(v).heap(),doms.size()) {}
	Void attach(INotifiable* p)
	{
		pOwner = p;
		for (UInt i = 0; i < doms.size(); i++)
			demons[i] = new (doms.solver().heap()) IdxDeltaDemon(this,i);
	}
	Void detach(INotifiable* p)
	{
		for (UInt i = 0; i < doms.size(); i++)
			demons[i]->detach();
	}

	INotifiable* 			pOwner;
	DomArrayView<Eval,View>	doms;
	Array<IdxDeltaDemon*>	demons;
};

template<class View>
struct IdxGroundDeltaStack : DeltaStack<UInt>
{
	typedef typename EvalTraits<View>::Eval::Elem	Eval;
	struct IdxGroundDeltaDemon : INotifiable
	{
		IdxGroundDeltaDemon(IdxGroundDeltaStack* pOwner, UInt idx) :
			rOwner(*pOwner),idx(idx)
		{	rOwner.doms[idx].attach(this);	}
		Bool notify()
		{	rOwner.push(idx); return rOwner.pOwner->notify();	}
		Void detach()
		{	rOwner.doms[idx].detach(this);	}
		IdxGroundDeltaStack&	rOwner;
		UInt					idx;
	};
	typedef DeltaStack<UInt>	Super;
	IdxGroundDeltaStack(const View& v) :
		Super(getCPSolver(v)),
		pOwner(NULL),
		doms(getCPSolver(v),v),
		demons(getCPSolver(v).heap(),doms.size()) {}
	Void attach(INotifiable* p)
	{
		pOwner = p;
		for (UInt i = 0; i < doms.size(); i++)
			demons[i] = new (doms.solver().heap()) IdxGroundDeltaDemon(this,i);
	}
	Void detach(INotifiable* p)
	{
		for (UInt i = 0; i < doms.size(); i++)
			demons[i]->detach();
	}

	INotifiable* 				pOwner;
	ValArrayView<Eval,View>		doms;
	Array<IdxGroundDeltaDemon*>	demons;
};

template<class View>
struct IdxBoundsDeltaStack : DeltaStack<UInt>
{
	typedef typename EvalTraits<View>::Eval::Elem	Eval;
	struct IdxBoundsDeltaDemon : INotifiable
	{
		IdxBoundsDeltaDemon(IdxBoundsDeltaStack* pOwner, UInt idx) :
			rOwner(*pOwner),idx(idx)
		{	rOwner.bnds[idx].attach(this);	}
		Bool notify()
		{	rOwner.push(idx); return rOwner.pOwner->notify();	}
		Void detach()
		{	rOwner.bnds[idx].detach(this);	}
		IdxBoundsDeltaStack&	rOwner;
		UInt					idx;
	};
	typedef DeltaStack<UInt>	Super;
	IdxBoundsDeltaStack(const View& v) :
		Super(getCPSolver(v)),
		pOwner(NULL),
		bnds(getCPSolver(v),v),
		demons(getCPSolver(v).heap(),bnds.size()) {}
	Void attach(INotifiable* p)
	{
		pOwner = p;
		for (UInt i = 0; i < bnds.size(); i++)
			demons[i] = new (bnds.solver().heap()) IdxBoundsDeltaDemon(this,i);
	}
	Void detach(INotifiable* p)
	{
		for (UInt i = 0; i < bnds.size(); i++)
			demons[i]->detach();
	}

	INotifiable* 				pOwner;
	BndArrayView<Eval,View>		bnds;
	Array<IdxBoundsDeltaDemon*>	demons;
};

template<class Dom>
struct IdxDomDelta
{
	IdxDomDelta(UInt idx,typename Dom::Delta delta) :
		idx(idx),delta(delta)	{}
	UInt	idx;
	typename Dom::Delta	delta;
};

template<class ArrayView>
struct IdxDomDeltaStack :
	DeltaStack<IdxDomDelta<typename DomTraits<ArrayView>::Dom> >
{
	typedef typename ElemTraits<ArrayView>::Elem	Elem;
	typedef typename EvalTraits<Elem>::Eval			Eval;
	typedef typename DomTraits<Elem>::Dom 			Dom;
	typedef IdxDomDelta<Dom>	IdxAndDelta;
	typedef DeltaStack<IdxAndDelta>	IdxAndDeltaStack;
	struct IdxDomDeltaDemon : INotifiable
	{
		IdxDomDeltaDemon(IdxDomDeltaStack* pOwner, UInt idx, INotifiable* pNotify) :
			rOwner(*pOwner),idx(idx),rNotify(*pNotify)
		{	rOwner.array[idx].attach(this);	}
		Bool notify()
		{
			rOwner.push(IdxAndDelta(idx,rOwner.array[idx]->delta()));
			return rNotify.notify();
		}
		Void detach()
		{	rOwner.array[idx].detach(this);	}
		IdxDomDeltaStack&	rOwner;
		UInt				idx;
		INotifiable&		rNotify;
	};
	IdxDomDeltaStack(ArrayView& a) :
		IdxAndDeltaStack(getCPSolver(a)),
		pOwner(NULL),
		array(getCPSolver(a),a),
		demons(getCPSolver(a).heap(),array.size()) {}
	Void attach(INotifiable* p)
	{
		for (UInt i = 0; i < array.size(); i++)
			demons[i] = new (array.solver().heap()) IdxDomDeltaDemon(this,i,p);
	}
	Void detach(INotifiable* p)
	{
		for (UInt i = 0; i < array.size(); i++)
			demons[i]->detach();
	}

	INotifiable* 					pOwner;
	DomArrayView<Eval,ArrayView>	array;
	Array<IdxDomDeltaDemon*>		demons;
};

template<class Delta>
struct DeltaList : private casper::detail::SList<Delta>
{
	typedef casper::detail::SList<Delta>	Super;

	struct ClearDeltas : INotifiable
	{
		ClearDeltas(DeltaList* pOwner) : rOwner(*pOwner) {}
		Bool notify()
		{	rOwner.clear();	rOwner.firstCPStore = true; return true;	}
		DeltaList& rOwner;
	};

	struct DeltaIterator : Super::Iterator
	{
		typedef Delta Value;
		DeltaIterator(const DeltaIterator& s) :
			Super::Iterator(s),propID(s.propID) {}
		DeltaIterator(ICPSolver& solver,const typename Super::Iterator& s) :
					Super::Iterator(s),propID(solver.propID())
					{assert(this->curNode);}
		const DeltaIterator& operator=(const DeltaIterator& s)
		{
			static_cast<typename Super::Iterator&>(*this) = s;
			propID = s.propID;
			assert(this->curNode);
			return *this;
		}

		Counter propID;
	};

	typedef DeltaIterator	Iterator;
	typedef Iterator		ConstIterator;

	DeltaList(ICPSolver& solver);
	DeltaList(const DeltaList& other);

	force_inline Iterator begin() const;
	force_inline Iterator beginFrom(Iterator it) const;
	force_inline Iterator end() const;
	Void	store(const Delta& delta);
	Void	clear();
	ICPSolver&	solver() const { return mSolver; }

	ICPSolver&						mSolver;
	typename Super::Iterator	beginIt;
	typename Super::Iterator	endIt;
	Delta						defaultDelta;
	Bool						firstCPStore;
	INotifiable*				pClearDemon;
};

template<class Delta>
DeltaList<Delta>::DeltaList(const DeltaList& other) :
	Super(), // como passar isto??? -> static_cast<const CPICPSolver&&>(other.mSolver.getImpl()).pHeap()
	mSolver(other.mSolver),
	beginIt(other.beginIt),
	endIt(other.endIt),
	firstCPStore(other.firstCPStore),
	pClearDemon(other.pClearDemon)
{
}

template<class Delta>
DeltaList<Delta>::DeltaList(ICPSolver& solver) :
	Super(solver.pHeap()),
	mSolver(solver),
	firstCPStore(true),
	pClearDemon(new (solver.heap()) ClearDeltas(this))
{
	// temp (depends on exposing attachToCP as a ICPSolver& method)
//	static_cast<CPICPSolver&&>(solver.getImpl()).
//		attachToCP(new (solver.heap()) ClearDeltas(this)) ;
//	Super::pushFront(defaultDelta);
//	endIt = Super::begin();

	beginIt = typename Super::Iterator(
			new (solver.heap()) detail::SListNode<Delta>(defaultDelta,NULL)
			);
	endIt = beginIt;
}

template<class Delta>
typename DeltaList<Delta>::Iterator DeltaList<Delta>::begin() const
{	return Iterator(solver(),beginIt);	}

template<class Delta>
typename DeltaList<Delta>::Iterator DeltaList<Delta>::beginFrom(Iterator it) const
{
	if (it.propID == solver().propID())
		return it;
	else
		return begin();
}

template<class Delta>
typename DeltaList<Delta>::Iterator DeltaList<Delta>::end() const
{	return Iterator(solver(),endIt);	}

template<class Delta>
Void	DeltaList<Delta>::store(const Delta& delta)
{
	//std::cout << "store\n";
	if (firstCPStore)
	{
		firstCPStore = false;
		solver().notifyOnNextCP(pClearDemon);
		Super::head = NULL;
		Super::pushFront(defaultDelta);
		endIt = Super::begin();
		beginIt.curNode->next = endIt.curNode;
		*beginIt = delta;
	}
	else
	{
		*endIt = delta;
		endIt = insertAfter(endIt, defaultDelta);
	}
}

template<class Delta>
Void	DeltaList<Delta>::clear()
{
	//Super::clear();
/*	Super::head = NULL;
	Super::pushFront(defaultDelta);
	endIt = Super::begin();
	*/
	endIt = beginIt;
}

};
};

#endif /*CASPER_KERNEL_DELTA_H_*/



