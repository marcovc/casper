 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_SCHEDULER
#define _H_CASPER_KERNEL_SCHEDULER

#include <vector>
#include <casper/cp/filter/common.h>
#include <casper/kernel/reversible/sulist.h>
#include <casper/kernel/reversible/list.h>
#include <casper/util/container/stdlist.h>
#include <casper/util/container/stdarray.h>
#include <casper/kernel/reversible/stack.h>
// tmp
#include <casper/cp/filter/weightedfilter.h>

namespace Casper {
namespace CP {


struct IFilterSched : IFilter
{
	IFilterSched(Store& s, bool attach = true) : IFilter(s),pParent(NULL)
	{
		if (attach)
			s.setFilterSched(this);
	}
	virtual bool post(Filter)	=	0;
	void attach(INotifiable* f)
	{ /*assert(pParent==NULL);*/ pParent = f; }
	void detach(INotifiable*)
	{ pParent = NULL; }
	virtual IFilter*	getPActiveFilter()	= 0;

	INotifiable*	pParent;
};

struct FilterSched : Util::PImplIdiom<IFilterSched>
{
	typedef FilterSched							Self;
	typedef Util::PImplIdiom<IFilterSched> 	Super;
	//typedef IFilter::Cost				Cost;

	FilterSched(IFilterSched*	f) : Super(f) {}

	bool post(Filter f) {	return Super::pImpl->post(f); }
	IFilter*	getPActiveFilter() {	return Super::pImpl->getPActiveFilter(); }
};


struct IQueueFilterSched : IFilterSched
{
	IQueueFilterSched(Store& store,bool attach,uint qid = 0) :
		IFilterSched(store,attach),qid(qid) {}
	virtual void clear() = 0;
//	virtual bool empty() const = 0;
	// to enqueue an external (i.e. not attached) filters. Used for nesting.
	virtual bool enqueue(Filter) = 0;
	virtual void yield() = 0;
	// returns a pointer to the running filter or NULL is no filter running.
	uint	qid;
};

struct QueueFilterSched : Util::PImplIdiom<IQueueFilterSched>
{
	typedef QueueFilterSched						Self;
	typedef Util::PImplIdiom<IQueueFilterSched> 	Super;
	//typedef IFilter::Cost				Cost;

	QueueFilterSched(IQueueFilterSched*	f) : Super(f) {}

	bool post(Filter f) {	return Super::pImpl->post(f); }
	void clear() {	Super::pImpl->clear(); }
	bool enqueue(Filter f) {	return Super::pImpl->enqueue(f); }
	IFilter*	getPActiveFilter() {	return Super::pImpl->getPActiveFilter(); }
};

// executes filter right after notification, which has two consequences:
// no data structure needed to hold filters -> faster than LIFOFilterSched
// less control -> cannot be nested in other filter scheds
struct GreedyLIFOFilterSched : IFilterSched
{
	typedef IFilter*							PIFilter;
	typedef INotifiable*						PINotifiable;

	struct FilterDemon : INotifiable
	{
		GreedyLIFOFilterSched& rOwner;
		Filter f;
		FilterDemon(GreedyLIFOFilterSched* pOwner, Filter f) :
			rOwner(*pOwner),f(f) { f.attach(this); }
		bool notify()
		{
			//assert(rOwner.solver().valid());
			if (!rOwner.store.valid())
				return false;
			rOwner.store.getStats().signalPropagation();
			return f.execute();
			//if (/*!f.entailed() &&*/ !f.execute())
			//	return false;
			//return true;
		}
	};

	GreedyLIFOFilterSched(Store& store,bool attach) :
		IFilterSched(store,attach),store(store),pCurFilterDemon(NULL)
	{}

	~GreedyLIFOFilterSched() {}

	bool post(Filter f)
	{
		assert(store.valid());
		pCurFilterDemon = new (store) FilterDemon(this,f);
		bool r = pCurFilterDemon->notify();
		pCurFilterDemon = NULL;
		return r;
	}
	IFilter*	getPActiveFilter()
	{
		if (pCurFilterDemon == NULL)
			return NULL;
		return pCurFilterDemon->f.getPImpl();
	}

	bool execute()
	{	return true; /*return solver().valid();*/	}

	// should this be implemented?
	bool entailed()	const
	{	return false;	}

	void attach(INotifiable*) {}
	void detach(INotifiable*) {}

	Store&	store;
	FilterDemon* pCurFilterDemon;
};

GreedyLIFOFilterSched* greedyLIFOFilterSched(Store& store,bool attach=true);

// if the filter is already in the queue then use it in that position
struct ULIFOFilterSched : IQueueFilterSched
{
	typedef IFilter*						PIFilter;
	typedef INotifiable*					PINotifiable;
	typedef	Util::StdSList<PIFilter>				FilterQueue;

	struct FilterDemon : INotifiable
	{
		ULIFOFilterSched& rOwner;
		Filter f;
		FilterDemon(ULIFOFilterSched* pOwner, Filter f) :
			rOwner(*pOwner),f(f) { f.attach(this); }
		bool notify()
		{
			return !rOwner.enqueue(f) || rOwner.pParent->notify();
		}
	};

	ULIFOFilterSched(Store& store,bool attach = true,uint qid = 0) :
									IQueueFilterSched(store,attach,qid),
									store(store),
									toExecute(store.getEnv().getState().getCPHeap()),
									mYield(false),
									pActiveFilter(NULL)
	{}

	bool post(Filter f)
	{
		FilterDemon* d = new (store) FilterDemon(this,f);
		return d->notify();
	}

	bool notify()
	{	assert(0); /* demons are notified instead */ return true; }

	bool enqueue(Filter f)
	{
		if (f.getImpl().inQueue() == qid)
			return false;
		assert(f.getImpl().inQueue() == noQueue);
		f.getImpl().setInQueue(qid);
		toExecute.pushFront(f.getPImpl());
		return true;
	}

	PIFilter	getPActiveFilter()
	{	return pActiveFilter;	}

	bool execute()
	{
		mYield = false;
		while (!toExecute.empty() and !mYield)
		{
			store.getStats().signalPropagation();
			pActiveFilter = *toExecute.begin();
			toExecute.popFront();
			assert(pActiveFilter->inQueue()==qid);	// this should always be true
			pActiveFilter->setInQueue(noQueue);
			//Debug::log(this,Debug::onFilter);
			if (!pActiveFilter->execute())
			{
				clear();
				return false;
			}
		}
		pActiveFilter = NULL;
		return true;
	}

	void clear()
	{
		for (FilterQueue::Iterator it = toExecute.begin();
			it != toExecute.end(); ++it)
			(*it)->setInQueue(noQueue);
		toExecute.clear();
		pActiveFilter = NULL;
	}

	bool entailed()	const
	{	return false;	}

	void yield() {	mYield = true;	}

	Store&			store;
	FilterQueue		toExecute;
	bool			mYield;
	PIFilter		pActiveFilter;
};

ULIFOFilterSched* ulifoFilterSched(Store& store, bool attach=true);

// filters are always added at the beginning of the queue
// (they will never run twice since only first occurrence is considered)
struct LIFOFilterSched : IQueueFilterSched
{
	typedef IFilter*						PIFilter;
	typedef INotifiable*					PINotifiable;
	typedef	Util::StdSList<PIFilter>				FilterQueue;

	struct FilterDemon : INotifiable
	{
		LIFOFilterSched& rOwner;
		Filter f;
		FilterDemon(LIFOFilterSched* pOwner, Filter f) :
			rOwner(*pOwner),f(f) { f.attach(this); }
		bool notify()
		{
			return !rOwner.enqueue(f) ||
				   rOwner.pParent->notify();
		}
	};

	LIFOFilterSched(Store& store,bool attach = true,uint qid = 0) :
									IQueueFilterSched(store,attach,qid),
									store(store),
									toExecute(store.getEnv().getState().getCPHeap()),
									mYield(false),
									pActiveFilter(NULL)
	{}

	bool post(Filter f)
	{
		FilterDemon* d = new (store) FilterDemon(this,f);
		return d->notify();
	}

	bool notify()
	{	assert(0); /* demons are notified instead */ return true; }

	bool enqueue(Filter f)
	{
		f.getImpl().setInQueue(qid);
		toExecute.pushFront(f.getPImpl());
		return true;
	}

	PIFilter	getPActiveFilter()
	{	return pActiveFilter;	}

	bool execute()
	{
		mYield = false;
		while (!toExecute.empty() and !mYield)
		{
			pActiveFilter = *toExecute.begin();
			toExecute.popFront();
			if (pActiveFilter->inQueue()==qid)
			{
				store.getStats().signalPropagation();
				pActiveFilter->setInQueue(noQueue);
				store.getEnv().log(this,"LIFOFilterSched",Util::Logger::filterSchedIterate);
				if (!pActiveFilter->execute())
				{
					clear();
					return false;
				}
			}
		}
		pActiveFilter = NULL;
		return true;
	}

	void clear()
	{
		for (FilterQueue::Iterator it = toExecute.begin();
			it != toExecute.end(); ++it)
			(*it)->setInQueue(noQueue);
		toExecute.clear();
		pActiveFilter = NULL;
	}

	bool entailed()	const
	{	return false;	}

	void yield() {	mYield = true;	}

	Store&			store;
	FilterQueue		toExecute;
	bool			mYield;
	PIFilter		pActiveFilter;
};


LIFOFilterSched* lifoFilterSched(Store& store, bool attach=true);

struct FIFOFilterSched : IQueueFilterSched
{
	typedef IFilter*						PIFilter;
	typedef INotifiable*					PINotifiable;
	typedef	Util::StdSList<PIFilter>				FilterQueue;

	struct FilterDemon : INotifiable
	{
		FIFOFilterSched& rOwner;
		Filter f;
		FilterDemon(FIFOFilterSched* pOwner, Filter f) :
			rOwner(*pOwner),f(f) { f.attach(this); }
		bool notify()
		{
			return !rOwner.enqueue(f) ||
				   rOwner.pParent->notify();
		}
	};

	struct WeightedFilterDemon : INotifiable
	{
		FIFOFilterSched& rOwner;
		WeightedFilter f;
		WeightedFilterDemon(FIFOFilterSched* pOwner, Filter f) :
			rOwner(*pOwner),f(rOwner.store,f) { f.attach(this); }
		bool notify()
		{
			return !rOwner.enqueue(&f) ||
				   rOwner.pParent->notify();
		}
		uint getAFC() const { return f.getWeight(); }
	};

	FIFOFilterSched(Store& store,bool attach = true,uint qid=1) :
									IQueueFilterSched(store,attach,qid),
									store(store),
									toExecute(store.getEnv().getState().getCPHeap()),
									mYield(false),
									itLast(toExecute.end()),
									pActiveFilter(NULL)
	{}

	bool post(Filter f)
	{
		if (store.getFilterWeighting())
		{
			WeightedFilterDemon* d = new (store) WeightedFilterDemon(this,f);
			return d->notify();
		}
		else
		{
			FilterDemon* d = new (store) FilterDemon(this,f);
			return d->notify();
		}
	}

	bool notify()
	{	assert(0); /* demons are notified instead */ return true; }

	bool enqueue(Filter f)
	{
		if (f.getImpl().inQueue() <= qid)
			return false;
		f.getImpl().setInQueue(qid);
		//toExecute.pushBack(f.getPImpl());
		if (toExecute.empty())
			itLast = toExecute.pushFront(f.getPImpl());
		else
			itLast = toExecute.insertAfter(itLast,f.getPImpl());
		return true;
	}

	PIFilter	getPActiveFilter()
	{	return pActiveFilter;	}

	bool execute()
	{
		mYield = false;

		while (!toExecute.empty() and !mYield)
		{
			store.getStats().signalPropagation();
			pActiveFilter = *toExecute.begin();
			toExecute.popFront();
			pActiveFilter->setInQueue(noQueue);
			if (!pActiveFilter->execute())
			{
				clear();
				return false;
			}
		}
		pActiveFilter = NULL;
		return true;
	}

	void clear()
	{
		for (FilterQueue::Iterator it = toExecute.begin();
			it != toExecute.end(); ++it)
			(*it)->setInQueue(noQueue);
		toExecute.clear();
		pActiveFilter = NULL;
	}

	bool entailed()	const
	{	return false;	}

	void yield() {	mYield = true;	}	// TODO

	Store&					store;
	FilterQueue				toExecute;
	bool					mYield;
	FilterQueue::Iterator	itLast;
	PIFilter				pActiveFilter;
};

FIFOFilterSched* fifoFilterSched(Store& store,bool attach=true);


struct TwoCostFilterSched : IQueueFilterSched
{
	typedef IFilter*						PIFilter;
	typedef INotifiable*					PINotifiable;
	typedef	Util::StdSList<PIFilter>				FilterQueue;

	struct CheapFilterDemon : INotifiable
	{
		TwoCostFilterSched& rOwner;
		Filter f;
		CheapFilterDemon(TwoCostFilterSched* pOwner, Filter f) :
			rOwner(*pOwner),f(f) { f.attach(this); }
		bool notify()
		{
			if (f.getImpl().inQueue() <= rOwner.qid)
				return true;
			f.getImpl().setInQueue(rOwner.qid);
			rOwner.toExecute.pushFront(f.getPImpl());
			return rOwner.pParent->notify();
		}
	};

	struct ExpensiveFilterDemon : INotifiable
	{
		TwoCostFilterSched& rOwner;
		Filter f;
		ExpensiveFilterDemon(TwoCostFilterSched* pOwner, Filter f) :
			rOwner(*pOwner),f(f) { f.attach(this); }
		bool notify()
		{
			if (f.getImpl().inQueue() <= rOwner.qid)
				return true;
			f.getImpl().setInQueue(rOwner.qid);
			rOwner.toExecute.pushBack(f.getPImpl());
			return rOwner.pParent->notify();
		}
	};

	TwoCostFilterSched(Store& store, Cost maxFirstStageCost = constantHi,
						bool attach = true,uint qid=1) :
							IQueueFilterSched(store,attach,qid),
							store(store),
							toExecute(store),
							maxFirstStageCost(maxFirstStageCost),
							pActiveFilter(NULL)
	{}

	bool post(Filter f)
	{
		INotifiable* d;
		if (f.cost() <= maxFirstStageCost)
			d = new (store) CheapFilterDemon(this,f);
		else
			d = new (store) ExpensiveFilterDemon(this,f);
		return d->notify();
	}

	bool notify()
	{	assert(0); /* demons are notified instead */ return true; }

	bool enqueue(Filter f)
	{
		if (f.cost() <= maxFirstStageCost)
			toExecute.pushFront(f.getPImpl());
		else
			toExecute.pushBack(f.getPImpl());
		return true;
	}

	PIFilter	getPActiveFilter()
	{	return pActiveFilter;	}

	bool execute()
	{
		while (!toExecute.empty())
		{
			store.getStats().signalPropagation();
			pActiveFilter = *toExecute.begin();
			toExecute.popFront();
			pActiveFilter->setInQueue(noQueue);
			if (!pActiveFilter->execute())
			{
				clear();
				return false;
			}
		}
		pActiveFilter = NULL;
		return true;
	}

	void clear()
	{
		for (FilterQueue::Iterator it = toExecute.begin();
			it != toExecute.end(); ++it)
			(*it)->setInQueue(noQueue);
		toExecute.clear();
		pActiveFilter = NULL;
	}

//	bool empty() const
//	{	return toExecute.empty();	}

	bool entailed()	const
	{	return false;	}

	void yield() {	assert(0);	}	// TODO

	Store&			store;
	FilterQueue		toExecute;
	const Cost		maxFirstStageCost;
	PIFilter		pActiveFilter;
};

TwoCostFilterSched* twoCostFilterSched(Store& store,
						IFilter::Cost maxFirstStageCost = IFilter::constantHi);

template<class NextFilterSched>
struct CostFilterSched : IQueueFilterSched
{
	typedef IFilter*						PIFilter;
	typedef INotifiable*					PINotifiable;
	typedef IQueueFilterSched*				PQueueFilterSched;

	struct FilterDemon : INotifiable
	{
		CostFilterSched& rOwner;
		IQueueFilterSched& f;
		FilterDemon(CostFilterSched* pOwner, IQueueFilterSched* f) :
			rOwner(*pOwner),f(*f) { f->attach(this); }
		bool notify()
		{
			if (f.inQueue()!=noQueue)
				return true;
			f.setInQueue(1);
			return rOwner.pParent->notify();
		}
	};

	CostFilterSched(Store& store, bool attach = true) :
									IQueueFilterSched(store,attach),
									store(store),
									queues(store,maxCost),
									activeQueueIdx(noQueue)
	{
		for (uint i = 0; i < maxCost; i++)
		{
			queues[i] = new (store) NextFilterSched(store,false,i);
			new (store) FilterDemon(this,queues[i]);
		}
	}

	bool post(Filter f)
	{	return queues[(uint)f.cost()]->post(f);	}

	bool notify()
	{	assert(0); /* demons are notified instead */ return true; }

	bool enqueue(Filter f)
	{
		queues[(uint)f.cost()]->setInQueue(1);
		return queues[(uint)f.cost()]->enqueue(f);
	}

	PIFilter	getPActiveFilter()
	{
		if (activeQueueIdx < queues.size())
			return queues[activeQueueIdx]->getPActiveFilter();
		else
			return NULL;
	}

	bool execute()
	{
		activeQueueIdx = getFirstPendingQueueIdx();
		while (activeQueueIdx < queues.size())
		{
			queues[activeQueueIdx]->setInQueue(noQueue);
			if (!queues[activeQueueIdx]->execute())
			{
				clear();
				return false;
			}
			activeQueueIdx = getFirstPendingQueueIdx();
		}
		return true;
	}

	void clear()
	{
		for (uint i = 0; i < queues.size(); i++)
		{
			queues[i]->setInQueue(noQueue);
			queues[i]->clear();
		}
		activeQueueIdx = queues.size();
	}

	bool entailed()	const
	{	return false;	}

	void yield() {	assert(0);	}	// TODO

//	protected:
	uint getFirstPendingQueueIdx()
	{
		uint i = 0;
		for (; i < queues.size(); i++)
			if (queues[i]->inQueue()==1)
				return i;
		return i;
	}

	Store&						store;
	Util::StdArray<PQueueFilterSched>	queues;
	uint						activeQueueIdx;
};

template<class SlaveFilterSched>
CostFilterSched<SlaveFilterSched>* costFilterSched(Store& store)
{
	CostFilterSched<SlaveFilterSched>* ret =
		new (store) CostFilterSched<SlaveFilterSched>(store);
	return ret;
}

// specialization for twoCostFilterSched
template<>
struct CostFilterSched<TwoCostFilterSched> : public IQueueFilterSched
{
	typedef IFilter*						PIFilter;
	typedef INotifiable*					PINotifiable;
	typedef IQueueFilterSched*				PQueueFilterSched;

	struct FilterDemon : INotifiable
	{
		CostFilterSched& rOwner;
		IQueueFilterSched& f;
		FilterDemon(CostFilterSched* pOwner, IQueueFilterSched* f) :
			rOwner(*pOwner),f(*f) { f->attach(this); }
		bool notify()
		{
			if (f.inQueue()!=noQueue)
				return true;
			f.setInQueue(1);
			return rOwner.pParent->notify();
		}
	};

	CostFilterSched(Store& store, bool attach = true) :
									IQueueFilterSched(store,attach),
									store(store),
									queues(store,maxCost/2),
									activeQueueIdx(noQueue)
	{
		for (uint i = 0; i < maxCost/2; i++)
		{
			queues[i] = new (store) TwoCostFilterSched(store,(Cost)(i*2),false,i);
			new (store) FilterDemon(this,queues[i]);
		}
	}

	bool post(Filter f)
	{	return queues[(uint)(f.cost()/2)]->post(f);	}

	bool notify()
	{	assert(0); /* demons are notified instead */ return true; }

	bool enqueue(Filter f)
	{	return queues[(uint)(f.cost()/2)]->enqueue(f);	}

	PIFilter	getPActiveFilter()
	{
		if (activeQueueIdx < queues.size())
			return queues[activeQueueIdx]->getPActiveFilter();
		else
			return NULL;
	}

	bool execute()
	{
		activeQueueIdx = getFirstPendingQueueIdx();
		while (activeQueueIdx < queues.size())
		{
			//queues[idx]->inQueue = false;
			queues[activeQueueIdx]->setInQueue(noQueue);
			if (!queues[activeQueueIdx]->execute())
			{
				clear();
				return false;
			}
			activeQueueIdx = getFirstPendingQueueIdx();
		}
		return true;
	}

	void clear()
	{
		for (uint i = 0; i < queues.size(); i++)
		{
			queues[i]->setInQueue(noQueue);
			queues[i]->clear();
		}
		activeQueueIdx = queues.size();
	}

	void yield() {	assert(0);	}	// TODO

	bool entailed()	const
	{	return false;	}

	protected:
	uint getFirstPendingQueueIdx()
	{
		uint i = 0;
		for (; i < queues.size(); i++)
			if (queues[i]->inQueue()==1)
				return i;
		return i;
	}

	Store&						store;
	Util::StdArray<PQueueFilterSched>	queues;
	uint						activeQueueIdx;
};

CostFilterSched<TwoCostFilterSched>* costFilterSched(Store& store);



template<class NextFilterSched>
struct FullyPreemptCostFilterSched : IQueueFilterSched
{
	typedef IFilter*						PIFilter;
	typedef INotifiable*					PINotifiable;
	typedef IQueueFilterSched*				PQueueFilterSched;

	struct FilterDemon : INotifiable
	{
		FullyPreemptCostFilterSched& rOwner;
		IQueueFilterSched& f;
		uint		queueCost;
		FilterDemon(FullyPreemptCostFilterSched* pOwner, IQueueFilterSched* f) :
			rOwner(*pOwner),f(*f),queueCost(f->qid) { f->attach(this); }
		bool notify()
		{
			if (f.inQueue()!=noQueue or rOwner.activeQueueIdx == queueCost)
				return true;

			if (rOwner.activeQueueIdx < rOwner.queues.size() and
					queueCost < rOwner.activeQueueIdx)
			{
				rOwner.queues[rOwner.activeQueueIdx]->yield();
				rOwner.queues[rOwner.activeQueueIdx]->setInQueue(1);
			}
			f.setInQueue(1);
			return rOwner.pParent->notify();
		}
	};

	FullyPreemptCostFilterSched(Store& store, bool attach = true) :
									IQueueFilterSched(store,attach),
									store(store),
									queues(store,maxCost),
									activeQueueIdx(noQueue)
	{
		for (uint i = 0; i < maxCost; i++)
		{
			queues[i] = new (store) NextFilterSched(store,false,i);
			new (store) FilterDemon(this,queues[i]);
		}
	}

	bool post(Filter f)
	{		return queues[(uint)f.cost()]->post(f);	}

	// tmp
/*	bool postNoExec(Filter f)
	{		return static_cast<NextFilterSched*>(queues[(uint)f.cost()])->postNoExec(f);	}
*/
	bool notify()
	{	assert(0); /* demons are notified instead */ return true; }

	bool enqueue(Filter f)
	{
		queues[(uint)f.cost()]->setInQueue(1);
		return queues[(uint)f.cost()]->enqueue(f);
	}

	PIFilter	getPActiveFilter()
	{
		if (activeQueueIdx < queues.size())
			return queues[activeQueueIdx]->getPActiveFilter();
		else
			return NULL;
	}

	bool execute()
	{
		activeQueueIdx = getFirstPendingQueueIdx();
		while (activeQueueIdx < queues.size())
		{
			queues[activeQueueIdx]->setInQueue(noQueue);
			if (!queues[activeQueueIdx]->execute())
			{
				activeQueueIdx = noQueue;
				clear();
				return false;
			}
			activeQueueIdx = getFirstPendingQueueIdx();
		}
		return true;
	}

	void clear()
	{
		for (uint i = 0; i < queues.size(); i++)
		{
			queues[i]->setInQueue(noQueue);
			queues[i]->clear();
		}
		activeQueueIdx = queues.size();
	}

	bool entailed()	const
	{	return false;	}

	void yield() {	assert(0);	}	// TODO

//	protected:
	uint getFirstPendingQueueIdx()
	{
		uint i = 0;
		for (; i < queues.size(); i++)
			if (queues[i]->inQueue()==1)
				return i;
		return i;
	}
	Store&						store;
	Util::StdArray<PQueueFilterSched>	queues;
	uint						activeQueueIdx;
};

template<class SlaveFilterSched>
FullyPreemptCostFilterSched<SlaveFilterSched>* fullyPreemptCostFilterSched(Store& store)
{
	return new (store) FullyPreemptCostFilterSched<SlaveFilterSched>(store);
}

//----

template<class NextFilterSched>
struct DynCostFilterSched : IQueueFilterSched
{
	typedef IFilter*						PIFilter;
	typedef INotifiable*					PINotifiable;
	typedef IQueueFilterSched*				PQueueFilterSched;

	struct FilterDemon : INotifiable
	{
		DynCostFilterSched& rOwner;
		Filter f;
		FilterDemon(DynCostFilterSched* pOwner, Filter f) :
			rOwner(*pOwner),f(f) { f.attach(this); }
		bool notify()
		{
			Cost c = f.cost();
			rOwner.queues[c]->setInQueue(1);
			return !rOwner.queues[c]->enqueue(f) ||
						rOwner.pParent->notify();
		}
	};

	DynCostFilterSched(Store& store, bool attach = true) :
									IQueueFilterSched(store,attach),
									store(store),
									queues(store,maxCost)
	{
		for (uint i = 0; i < maxCost; i++)
			queues[i] = new (store) NextFilterSched(store,false,i);
	}

	bool post(Filter f)
	{
		INotifiable* d = new (store) FilterDemon(this,f);
		return d->notify();
	}

	bool notify()
	{	assert(0); /* demons are notified instead */ return true; }

	bool enqueue(Filter)
	{	assert(0); /* TODO */ return true; }

	bool execute()
	{
		uint idx = getFirstPendingQueueIdx();
		while (idx < queues.size())
		{
			queues[idx]->setInQueue(noQueue);
			if (!queues[idx]->execute())
			{
				clear();
				return false;
			}
			idx = getFirstPendingQueueIdx();
		}
		return true;
	}

	void clear()
	{
		for (uint i = 0; i < queues.size(); i++)
		{
			queues[i]->setInQueue(noQueue);
			queues[i]->clear();
		}
	}

	bool entailed()	const
	{	return false;	}

//	protected:
	uint getFirstPendingQueueIdx()
	{
		uint i = 0;
		for (; i < queues.size(); i++)
			if (queues[i]->inQueue()==1)
				return i;
		return i;
	}

	Store&						store;
	Util::StdArray<PQueueFilterSched>	queues;
};

template<class NextFilterSched>
DynCostFilterSched<NextFilterSched>* dynCostFilterSched(Store& store)
{
	return new (store) DynCostFilterSched<NextFilterSched>(store,false);
}


} // CP
} // Casper

#endif
