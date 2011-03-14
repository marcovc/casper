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

#ifndef CASPER_CP_DELTA_H_
#define CASPER_CP_DELTA_H_

// FIXME: not adapted to new arch

#include <casper/kernel/notify/notifier.h>
#include <casper/kernel/reversible/stack.h>
#include <casper/cp/store.h>

namespace Casper {
namespace CP {
namespace Detail {

template<class Delta>
struct DeltaStack
{
	struct ClearDemon : INotifiable
	{
		ClearDemon(DeltaStack* pOwner) : rOwner(*pOwner)
		{	}
		bool notify()
		{	rOwner.onCPQueue = false; return true;	}
		DeltaStack& rOwner;
	};

	DeltaStack(Store& s) :
		store(s),
		onCPQueue(false),
		cd(this),
		pStack(NULL) {}

	void			push(const Delta& d);
	const Delta&	top() const { assert(onCPQueue); return pStack->top();	}
	void			pop()	{ assert(onCPQueue); pStack->pop();	}
	bool			empty() const {	return !onCPQueue or pStack->empty();	}
	Store&			getStore() const {	return store;	}
	void			clear()	{ pStack->clear();	}

	Store&					store;
	bool					onCPQueue;
	ClearDemon				cd;
	Util::StdStack<Delta>*	pStack;
};

template<class Delta>
void DeltaStack<Delta>::push(const Delta& d)
{
	if (!onCPQueue)
	{
		store.getEnv().getState().notifyOnNextCP(&cd);
		onCPQueue = true;
		pStack = new (store.getEnv().getState().getCPHeap())
			Util::StdStack<Delta>(store.getEnv().getState().getCPHeap());
	}
	pStack->push(d);
}

template<class View>
struct DomDeltaStack : INotifiable,
					DeltaStack<typename Traits::GetDom<View>::Type::Delta>
{
	typedef typename Casper::Traits::GetEval<View>::Type			Eval;
	typedef typename Traits::GetDom<View>::Type::Delta	DomDelta;
	typedef DeltaStack<DomDelta> Super;
	DomDeltaStack(Store& store,const View& v) :
		Super(store),pOwner(NULL),v(store,v) {}
	void setOwner(INotifiable* p) {	pOwner = p;	}
	bool notify()
	{
		assert(pOwner);
		push(v->delta());
		return pOwner->notify();
	}
	INotifiable* 			pOwner;
	DomView<Eval,View>		v;
};

template<class View>
struct IdxDeltaStack : DeltaStack<uint>
{
	typedef typename Casper::Traits::GetElem<View>::Type	Elem;
	typedef typename Casper::Traits::GetEval<Elem>::Type	Eval;
	typedef typename Traits::GetDom<Elem>::Type	Dom;
	struct IdxDeltaDemon : INotifiable
	{
		IdxDeltaDemon(IdxDeltaStack* pOwner, uint idx) :
			rOwner(*pOwner),idx(idx)
		{	rOwner.doms[idx].attach(this);	}
		bool notify()
		{	rOwner.push(idx); return rOwner.pOwner->notify();	}
		void detach()
		{	rOwner.doms[idx].detach(this);	}
		IdxDeltaStack&	rOwner;
		uint			idx;
	};
	typedef DeltaStack<uint>	Super;
	IdxDeltaStack(Store& store,const View& v) :
		Super(store),
		pOwner(NULL),
		doms(store,v),
		demons(store,doms.size()) {}
	void attach(INotifiable* p)
	{
		pOwner = p;
		for (uint i = 0; i < doms.size(); i++)
			demons[i] = new (store) IdxDeltaDemon(this,i);
	}
	void detach(INotifiable* p)
	{
		for (uint i = 0; i < doms.size(); i++)
			demons[i]->detach();
	}

	INotifiable* 			pOwner;
	DomArrayView<Eval,View>	doms;
	Util::StdArray<IdxDeltaDemon*>	demons;
};

template<class View>
struct IdxGroundDeltaStack : DeltaStack<uint>
{
	typedef typename Casper::Traits::GetEval<View>::Type::Elem	Eval;
	struct IdxGroundDeltaDemon : INotifiable
	{
		IdxGroundDeltaDemon(IdxGroundDeltaStack* pOwner, uint idx) :
			rOwner(*pOwner),idx(idx)
		{	rOwner.doms[idx].attach(this);	}
		bool notify()
		{	rOwner.push(idx); return rOwner.pOwner->notify();	}
		void detach()
		{	rOwner.doms[idx].detach(this);	}
		IdxGroundDeltaStack&	rOwner;
		uint					idx;
	};
	typedef DeltaStack<uint>	Super;
	IdxGroundDeltaStack(Store& store,const View& v) :
		Super(store),
		pOwner(NULL),
		doms(store,v),
		demons(store,doms.size()) {}
	void attach(INotifiable* p)
	{
		pOwner = p;
		for (uint i = 0; i < doms.size(); i++)
			demons[i] = new (store) IdxGroundDeltaDemon(this,i);
	}
	void detach(INotifiable* p)
	{
		for (uint i = 0; i < doms.size(); i++)
			demons[i]->detach();
	}

	INotifiable* 				pOwner;
	ValArrayView<Eval,View>		doms;
	Util::StdArray<IdxGroundDeltaDemon*>	demons;
};

template<class View>
struct IdxBoundsDeltaStack : DeltaStack<uint>
{
	typedef typename Casper::Traits::GetEval<typename Casper::Traits::GetElem<View>::Type>::Type	Eval;
	struct IdxBoundsDeltaDemon : INotifiable
	{
		IdxBoundsDeltaDemon(IdxBoundsDeltaStack* pOwner, uint idx) :
			rOwner(*pOwner),idx(idx)
		{	rOwner.bnds[idx].attach(this);	}
		bool notify()
		{	rOwner.push(idx); return rOwner.pOwner->notify();	}
		void detach()
		{	rOwner.bnds[idx].detach(this);	}
		IdxBoundsDeltaStack&	rOwner;
		uint					idx;
	};
	typedef DeltaStack<uint>	Super;
	IdxBoundsDeltaStack(Store& store,const View& v) :
		Super(store),
		pOwner(NULL),
		bnds(store,v),
		demons(store,bnds.size()) {}
	void attach(INotifiable* p)
	{
		pOwner = p;
		for (uint i = 0; i < bnds.size(); i++)
			demons[i] = new (store) IdxBoundsDeltaDemon(this,i);
	}
	void detach(INotifiable* p)
	{
		for (uint i = 0; i < bnds.size(); i++)
			demons[i]->detach();
	}

	INotifiable* 				pOwner;
	BndArrayView<Eval,View>		bnds;
	Util::StdArray<IdxBoundsDeltaDemon*>	demons;
};

template<class Dom>
struct IdxDomDelta
{
	IdxDomDelta(uint idx,typename Dom::Delta delta) :
		idx(idx),delta(delta)	{}
	uint	idx;
	typename Dom::Delta	delta;
};

template<class ArrayView>
struct IdxDomDeltaStack :
	DeltaStack<IdxDomDelta<typename Traits::GetDom<ArrayView>::Type> >
{
	typedef typename Casper::Traits::GetElem<ArrayView>::Type	Elem;
	typedef typename Casper::Traits::GetEval<Elem>::Type			Eval;
	typedef typename Traits::GetDom<Elem>::Type 			Dom;
	typedef IdxDomDelta<Dom>	IdxAndDelta;
	typedef DeltaStack<IdxAndDelta>	IdxAndDeltaStack;
	struct IdxDomDeltaDemon : INotifiable
	{
		IdxDomDeltaDemon(IdxDomDeltaStack* pOwner, uint idx, INotifiable* pNotify) :
			rOwner(*pOwner),idx(idx),rNotify(*pNotify)
		{	rOwner.array[idx].attach(this);	}
		bool notify()
		{
			rOwner.push(IdxAndDelta(idx,rOwner.array[idx]->delta()));
			return rNotify.notify();
		}
		void detach()
		{	rOwner.array[idx].detach(this);	}
		IdxDomDeltaStack&	rOwner;
		uint				idx;
		INotifiable&		rNotify;
	};
	IdxDomDeltaStack(Store& store,ArrayView& a) :
		IdxAndDeltaStack(store),
		pOwner(NULL),
		array(store,a),
		demons(store,array.size()) {}
	void attach(INotifiable* p)
	{
		for (uint i = 0; i < array.size(); i++)
			demons[i] = new (this->store) IdxDomDeltaDemon(this,i,p);
	}
	void detach(INotifiable* p)
	{
		for (uint i = 0; i < array.size(); i++)
			demons[i]->detach();
	}

	INotifiable* 					pOwner;
	DomArrayView<Eval,ArrayView>	array;
	Util::StdArray<IdxDomDeltaDemon*>		demons;
};

template<class Delta>
struct DeltaList : private Util::StdSList<Delta>
{
	typedef Util::StdSList<Delta>	Super;

	struct ClearDeltas : INotifiable
	{
		ClearDeltas(DeltaList* pOwner) : rOwner(*pOwner) {}
		bool notify()
		{	rOwner.clear();	rOwner.firstCPStore = true; return true;	}
		DeltaList& rOwner;
	};

	struct DeltaIterator : Super::Iterator
	{
		typedef Delta Value;
		DeltaIterator(const DeltaIterator& s) :
			Super::Iterator(s),propID(s.propID) {}
		DeltaIterator(Store& store,const typename Super::Iterator& s) :
					Super::Iterator(s),propID(store.propID())
					{assert(this->curNode);}
		const DeltaIterator& operator=(const DeltaIterator& s)
		{
			static_cast<typename Super::Iterator&>(*this) = s;
			propID = s.propID;
			assert(this->curNode);
			return *this;
		}

		counter propID;
	};

	typedef DeltaIterator	Iterator;
	typedef Iterator		ConstIterator;

	DeltaList(Store& store);
	DeltaList(const DeltaList& other);

	force_inline Iterator begin() const;
	force_inline Iterator beginFrom(Iterator it) const;
	force_inline Iterator end() const;
	void	store(const Delta& delta);
	void	clear();
	Store&	getStore() const { return cpstore; }

	Store&						cpstore;
	typename Super::Iterator	beginIt;
	typename Super::Iterator	endIt;
	Delta						defaultDelta;
	bool						firstCPStore;
	INotifiable*				pClearDemon;
};

template<class Delta>
DeltaList<Delta>::DeltaList(const DeltaList& other) :
	Super(), // como passar isto??? -> static_cast<const CPCPSolver&&>(other.mSolver.getImpl()).pHeap()
	cpstore(other.getStore()),
	beginIt(other.beginIt),
	endIt(other.endIt),
	firstCPStore(other.firstCPStore),
	pClearDemon(other.pClearDemon)
{
}

template<class Delta>
DeltaList<Delta>::DeltaList(Store& cpstore) :
	Super(cpstore.getEnv().getState().getCPHeap()),
	cpstore(cpstore),
	firstCPStore(true),
	pClearDemon(new (cpstore) ClearDeltas(this))
{
	// temp (depends on exposing attachToCP as a CPSolver& method)
//	static_cast<CPCPSolver&&>(solver.getImpl()).
//		attachToCP(new (solver.getHeap()) ClearDeltas(this)) ;
//	Super::pushFront(defaultDelta);
//	endIt = Super::begin();

	beginIt = typename Super::Iterator(
			new (cpstore) Util::SListNode<Delta>(defaultDelta,NULL)
			);
	endIt = beginIt;
}

template<class Delta>
typename DeltaList<Delta>::Iterator DeltaList<Delta>::begin() const
{	return Iterator(cpstore,beginIt);	}

template<class Delta>
typename DeltaList<Delta>::Iterator DeltaList<Delta>::beginFrom(Iterator it) const
{
	if (it.propID == cpstore.propID())
		return it;
	else
		return begin();
}

template<class Delta>
typename DeltaList<Delta>::Iterator DeltaList<Delta>::end() const
{	return Iterator(cpstore,endIt);	}

template<class Delta>
void	DeltaList<Delta>::store(const Delta& delta)
{
	//std::cout << "store\n";
	if (firstCPStore)
	{
		firstCPStore = false;
		cpstore.getEnv().getState().notifyOnNextCP(pClearDemon);
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
void	DeltaList<Delta>::clear()
{
	//Super::clear();
/*	Super::head = NULL;
	Super::pushFront(defaultDelta);
	endIt = Super::begin();
	*/
	endIt = beginIt;
}

};
} // CP
} // Casper

#endif /*CASPER_CP_DELTA_H_*/



