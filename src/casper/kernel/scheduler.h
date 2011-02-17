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

#ifndef _H_CASPER_KERNEL_SCHEDULER
#define _H_CASPER_KERNEL_SCHEDULER

#include <vector>
#include <casper/kernel/filter/common.h>
#include <casper/kernel/goal/common.h>
#include <casper/kernel/container/rsulist.h>
#include <casper/kernel/container/rlist.h>
#include <casper/kernel/container/list.h>
#include <casper/kernel/container/rstack.h>
// tmp
#include <casper/kernel/filter/weightedfilter.h>

namespace casper {

struct IFilterSched : IFilter
{
	IFilterSched(ICPSolver& s, Bool attach = true) : IFilter(s),pParent(NULL)
	{
		if (attach)
			s.setFilterSched(this);
	}
	virtual Bool post(Filter)	=	0;
	void attach(INotifiable* f)
	{ /*assert(pParent==NULL);*/ pParent = f; }
	void detach(INotifiable*)
	{ pParent = NULL; }
	virtual IFilter*	getPActiveFilter()	= 0;

	INotifiable*	pParent;
};

struct FilterSched : detail::PImplIdiom<IFilterSched>
{
	typedef FilterSched							Self;
	typedef detail::PImplIdiom<IFilterSched> 	Super;
	//typedef IFilter::Cost				Cost;

	FilterSched(IFilterSched*	f) : Super(f) {}

	Bool post(Filter f) {	return Super::pImpl->post(f); }
	IFilter*	getPActiveFilter() {	return Super::pImpl->getPActiveFilter(); }
};


struct IQueueFilterSched : IFilterSched
{
	IQueueFilterSched(ICPSolver& solver,bool attach,UInt qid = 0) :
		IFilterSched(solver,attach),qid(qid) {}
	virtual Void clear() = 0;
//	virtual Bool empty() const = 0;
	// to enqueue an external (i.e. not attached) filters. Used for nesting.
	virtual bool enqueue(Filter) = 0;
	virtual Void yield() = 0;
	// returns a pointer to the running filter or NULL is no filter running.
	UInt	qid;
};

struct QueueFilterSched : detail::PImplIdiom<IQueueFilterSched>
{
	typedef QueueFilterSched						Self;
	typedef detail::PImplIdiom<IQueueFilterSched> 	Super;
	//typedef IFilter::Cost				Cost;

	QueueFilterSched(IQueueFilterSched*	f) : Super(f) {}

	Bool post(Filter f) {	return Super::pImpl->post(f); }
	Void clear() {	Super::pImpl->clear(); }
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
			if (!rOwner.solver().valid())
				return false;
			rOwner.solver().signalPropagation();
			return f.execute();
			//if (/*!f.entailed() &&*/ !f.execute())
			//	return false;
			//return true;
		}
	};

	GreedyLIFOFilterSched(ICPSolver& solver,bool attach) :
		IFilterSched(solver,attach),pCurFilterDemon(NULL)
	{}

	~GreedyLIFOFilterSched() {}

	Bool post(Filter f)
	{
		assert(solver().valid());
		pCurFilterDemon = new (solver().heap()) FilterDemon(this,f);
		Bool r = pCurFilterDemon->notify();
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

	FilterDemon* pCurFilterDemon;
};

GreedyLIFOFilterSched* greedyLIFOFilterSched(ICPSolver& solver,bool attach=true);

// if the filter is already in the queue then use it in that position
struct ULIFOFilterSched : IQueueFilterSched
{
	typedef IFilter*						PIFilter;
	typedef INotifiable*					PINotifiable;
	typedef	detail::SList<PIFilter>			FilterQueue;

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

	ULIFOFilterSched(ICPSolver& solver,Bool attach = true,UInt qid = 0) :
									IQueueFilterSched(solver,attach,qid),
									toExecute(solver.pHeap()),
									mYield(false),
									pActiveFilter(NULL)
	{}

	Bool post(Filter f)
	{
		FilterDemon* d = new (solver().heap()) FilterDemon(this,f);
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
			solver().signalPropagation();
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

	Void yield() {	mYield = true;	}

	FilterQueue		toExecute;
	Bool			mYield;
	PIFilter		pActiveFilter;
};

ULIFOFilterSched* ulifoFilterSched(ICPSolver& solver, bool attach=true);

// filters are always added at the beginning of the queue
// (they will never run twice since only first occurrence is considered)
struct LIFOFilterSched : IQueueFilterSched
{
	typedef IFilter*						PIFilter;
	typedef INotifiable*					PINotifiable;
	typedef	detail::SList<PIFilter>			FilterQueue;

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

	LIFOFilterSched(ICPSolver& solver,Bool attach = true,UInt qid = 0) :
									IQueueFilterSched(solver,attach,qid),
									toExecute(solver.pHeap()),
									mYield(false),
									pActiveFilter(NULL)
	{}

	Bool post(Filter f)
	{
		FilterDemon* d = new (solver().heap()) FilterDemon(this,f);
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
				solver().signalPropagation();
				pActiveFilter->setInQueue(noQueue);
				solver().log(this,"LIFOFilterSched",Logger::filterSchedIterate);
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

	Void yield() {	mYield = true;	}

	FilterQueue		toExecute;
	Bool			mYield;
	PIFilter		pActiveFilter;
};


LIFOFilterSched* lifoFilterSched(ICPSolver& solver, bool attach=true);

struct FIFOFilterSched : IQueueFilterSched
{
	typedef IFilter*						PIFilter;
	typedef INotifiable*					PINotifiable;
	typedef	detail::SList<PIFilter>			FilterQueue;

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
			rOwner(*pOwner),f(f) { f.attach(this); }
		bool notify()
		{
			return !rOwner.enqueue(&f) ||
				   rOwner.pParent->notify();
		}
		UInt getAFC() const { return f.getWeight(); }
	};

	FIFOFilterSched(ICPSolver& solver,Bool attach = true,UInt qid=1) :
									IQueueFilterSched(solver,attach,qid),
									toExecute(solver.pHeap()),
									mYield(false),
									itLast(toExecute.end()),
									pActiveFilter(NULL)
	{}

	Bool post(Filter f)
	{
		if (this->solver().getFilterWeighting())
		{
			WeightedFilterDemon* d = new (solver().heap()) WeightedFilterDemon(this,f);
			return d->notify();
		}
		else
		{
			FilterDemon* d = new (solver().heap()) FilterDemon(this,f);
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
			solver().signalPropagation();
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
		{
			(*it)->setInQueue(noQueue);
#if 0
			// tmp
#warning "FIFOFilterSched stupid test for phd thesis enabled :)"
			IQueueFilterSched* ff = dynamic_cast<IQueueFilterSched*>(*it);
			if (ff != NULL)
				ff->clear();
#endif
		}
		toExecute.clear();
		pActiveFilter = NULL;
	}

	bool entailed()	const
	{	return false;	}

	Void yield() {	mYield = true;	}	// TODO

	FilterQueue				toExecute;
	Bool					mYield;
	FilterQueue::Iterator	itLast;
	PIFilter				pActiveFilter;
};

FIFOFilterSched* fifoFilterSched(ICPSolver& solver,bool attach=true);


struct TwoCostFilterSched : IQueueFilterSched
{
	typedef IFilter*						PIFilter;
	typedef INotifiable*					PINotifiable;
	typedef	detail::SList<PIFilter>			FilterQueue;

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

	TwoCostFilterSched(ICPSolver& solver, Cost maxFirstStageCost = constantHi,
						Bool attach = true,UInt qid=1) :
							IQueueFilterSched(solver,attach,qid),
							toExecute(solver.heap()),
							maxFirstStageCost(maxFirstStageCost),
							pActiveFilter(NULL)
	{}

	Bool post(Filter f)
	{
		INotifiable* d;
		if (f.cost() <= maxFirstStageCost)
			d = new (solver().heap()) CheapFilterDemon(this,f);
		else
			d = new (solver().heap()) ExpensiveFilterDemon(this,f);
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
			solver().signalPropagation();
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

//	Bool empty() const
//	{	return toExecute.empty();	}

	bool entailed()	const
	{	return false;	}

	Void yield() {	assert(0);	}	// TODO

	FilterQueue		toExecute;
	const Cost		maxFirstStageCost;
	PIFilter		pActiveFilter;
};

TwoCostFilterSched* twoCostFilterSched(ICPSolver& solver,
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

	CostFilterSched(ICPSolver& solver, Bool attach = true) :
									IQueueFilterSched(solver,attach),
									queues(solver.heap(),maxCost),
									activeQueueIdx(noQueue)
	{
		for (UInt i = 0; i < maxCost; i++)
		{
			queues[i] = new (solver.heap()) NextFilterSched(solver,false,i);
			new (solver.heap()) FilterDemon(this,queues[i]);
		}
	}

	Bool post(Filter f)
	{	return queues[(UInt)f.cost()]->post(f);	}

	bool notify()
	{	assert(0); /* demons are notified instead */ return true; }

	bool enqueue(Filter f)
	{
		queues[(UInt)f.cost()]->setInQueue(1);
		return queues[(UInt)f.cost()]->enqueue(f);
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
		for (UInt i = 0; i < queues.size(); i++)
		{
			queues[i]->setInQueue(noQueue);
			queues[i]->clear();
		}
		activeQueueIdx = queues.size();
	}

	bool entailed()	const
	{	return false;	}

	Void yield() {	assert(0);	}	// TODO

//	protected:
	UInt getFirstPendingQueueIdx()
	{
		UInt i = 0;
		for (; i < queues.size(); i++)
			if (queues[i]->inQueue()==1)
				return i;
		return i;
	}
	Array<PQueueFilterSched>	queues;
	UInt						activeQueueIdx;
};

template<class SlaveFilterSched>
CostFilterSched<SlaveFilterSched>* costFilterSched(ICPSolver& solver)
{
	CostFilterSched<SlaveFilterSched>* ret =
		new (solver.heap()) CostFilterSched<SlaveFilterSched>(solver);
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

	CostFilterSched(ICPSolver& solver, Bool attach = true) :
									IQueueFilterSched(solver,attach),
									queues(solver.heap(),maxCost/2),
									activeQueueIdx(noQueue)
	{
		for (UInt i = 0; i < maxCost/2; i++)
		{
			queues[i] = new (solver.heap()) TwoCostFilterSched(solver,(Cost)(i*2),false,i);
			new (solver.heap()) FilterDemon(this,queues[i]);
		}
	}

	Bool post(Filter f)
	{	return queues[(UInt)(f.cost()/2)]->post(f);	}

	bool notify()
	{	assert(0); /* demons are notified instead */ return true; }

	bool enqueue(Filter f)
	{	return queues[(UInt)(f.cost()/2)]->enqueue(f);	}

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
		for (UInt i = 0; i < queues.size(); i++)
		{
			queues[i]->setInQueue(noQueue);
			queues[i]->clear();
		}
		activeQueueIdx = queues.size();
	}

	Void yield() {	assert(0);	}	// TODO

	bool entailed()	const
	{	return false;	}

	protected:
	UInt getFirstPendingQueueIdx()
	{
		UInt i = 0;
		for (; i < queues.size(); i++)
			if (queues[i]->inQueue()==1)
				return i;
		return i;
	}
	Array<PQueueFilterSched>	queues;
	UInt						activeQueueIdx;
};

CostFilterSched<TwoCostFilterSched>* costFilterSched(ICPSolver& solver);



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
		UInt		queueCost;
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

	FullyPreemptCostFilterSched(ICPSolver& solver, Bool attach = true) :
									IQueueFilterSched(solver,attach),
									queues(solver.heap(),maxCost),
									activeQueueIdx(noQueue)
	{
		for (UInt i = 0; i < maxCost; i++)
		{
			queues[i] = new (solver.heap()) NextFilterSched(solver,false,i);
			new (solver.heap()) FilterDemon(this,queues[i]);
		}
	}

	Bool post(Filter f)
	{		return queues[(UInt)f.cost()]->post(f);	}

	// tmp
/*	Bool postNoExec(Filter f)
	{		return static_cast<NextFilterSched*>(queues[(UInt)f.cost()])->postNoExec(f);	}
*/
	bool notify()
	{	assert(0); /* demons are notified instead */ return true; }

	bool enqueue(Filter f)
	{
		queues[(UInt)f.cost()]->setInQueue(1);
		return queues[(UInt)f.cost()]->enqueue(f);
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
		for (UInt i = 0; i < queues.size(); i++)
		{
			queues[i]->setInQueue(noQueue);
			queues[i]->clear();
		}
		activeQueueIdx = queues.size();
	}

	bool entailed()	const
	{	return false;	}

	Void yield() {	assert(0);	}	// TODO

//	protected:
	UInt getFirstPendingQueueIdx()
	{
		UInt i = 0;
		for (; i < queues.size(); i++)
			if (queues[i]->inQueue()==1)
				return i;
		return i;
	}
	Array<PQueueFilterSched>	queues;
	UInt						activeQueueIdx;
};

template<class SlaveFilterSched>
FullyPreemptCostFilterSched<SlaveFilterSched>* fullyPreemptCostFilterSched(ICPSolver& solver)
{
	FullyPreemptCostFilterSched<SlaveFilterSched>* ret =
		new (solver.heap()) FullyPreemptCostFilterSched<SlaveFilterSched>(solver);
	return ret;
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

	DynCostFilterSched(ICPSolver& solver, Bool attach = true) :
									IQueueFilterSched(solver,attach),
									queues(solver.heap(),maxCost)
	{
		for (UInt i = 0; i < maxCost; i++)
			queues[i] = new (solver.heap()) NextFilterSched(solver,false,i);
	}

	Bool post(Filter f)
	{
		INotifiable* d = new (solver().heap()) FilterDemon(this,f);
		return d->notify();
	}

	bool notify()
	{	assert(0); /* demons are notified instead */ return true; }

	bool enqueue(Filter)
	{	assert(0); /* TODO */ return true; }

	bool execute()
	{
		UInt idx = getFirstPendingQueueIdx();
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
		for (UInt i = 0; i < queues.size(); i++)
		{
			queues[i]->setInQueue(noQueue);
			queues[i]->clear();
		}
	}

	bool entailed()	const
	{	return false;	}

//	protected:
	UInt getFirstPendingQueueIdx()
	{
		UInt i = 0;
		for (; i < queues.size(); i++)
			if (queues[i]->inQueue()==1)
				return i;
		return i;
	}

	Array<PQueueFilterSched>	queues;
};

template<class NextFilterSched>
DynCostFilterSched<NextFilterSched>* dynCostFilterSched(ICPSolver& solver)
{
	DynCostFilterSched<NextFilterSched>* ret =
		new (solver.heap()) DynCostFilterSched<NextFilterSched>(solver,false);
	return ret;
}

#if 0

template<class FilterSched>
struct FilterWrapper : FilterSched
{
	FilterWrapper(ICPSolver& solver,UInt evPriority) :
		FilterSched(solver,false),evPriority(evPriority) {}
	IFilter::Cost cost() const { return static_cast<IFilter::Cost>(evPriority); }
	Bool execute()
	{
		--this->solver().stats().countPropagations;
		return FilterSched::execute();
	}
	const UInt evPriority;
};

struct IdFilter : IFilter
{
	IdFilter(Filter f) : IFilter(f.solver()),pFilter(f.getPImpl()) {}
	bool execute() { return pFilter->execute(); }
	void attach(INotifiable* f) { pFilter->attach(f); }
	void detach(INotifiable* f) { pFilter->detach(f); }
	Cost cost() const { return pFilter->cost(); }
	IFilter* pFilter;
};

struct NewEventSuspList : INotifier
{
	typedef INotifier	Super;
	//typedef detail::RList<INotifiable*>	SList;
	typedef detail::RSList<INotifiable*>	SList;
	typedef FIFOFilterSched 				LowFilterSched;
//	typedef FilterWrapper<FullyPreemptCostFilterSched<FIFOFilterSched> > 	FilterSched;
	typedef FilterWrapper<FIFOFilterSched> 	FilterSched;

	SList	l;
	FilterSched filterSched;
	Reversible<Bool> first;

	NewEventSuspList(ICPSolver& solver,UInt evPriority) :
		l(solver),filterSched(solver,evPriority),first(solver,true) {}
	void attach(INotifiable* f)
	{
		typedef LowFilterSched::FilterDemon QueueDemon;

		QueueDemon* q = dynamic_cast<QueueDemon*>(f->getFirst());
	//	if (q==NULL)
	//		f->getFirst();
		assert(q!=NULL);

		IdFilter* idf = dynamic_cast<IdFilter*>(q->f.getPImpl());

		// if attaching to the wrapper
		if (idf != NULL)
		{
			//d->f = Filter(idf->pFilter);
			l.pushFront(f);
		}
		else
		{
			if (first)
			{
				first = false;
				l.solver().post(&filterSched);
			}
			filterSched.postNoExec(Filter(new (l.solver().heap()) IdFilter(q->f)));
		}
	}

	void detach(INotifiable* f)
	{	/*assert(0);*/ }

	bool notify() {	assert(0);	return true; }
	bool notifyAll()
	{
		for (SList::Iterator it = l.begin(); it != l.end(); ++it)
			if (!(*it)->notify())
				return false;
		return true;
	}
};
#else
struct NewEventSuspList : EventSuspList
{
	NewEventSuspList(ICPSolver& solver,UInt evPriority) : EventSuspList(solver) {}
};
#endif

};

#endif
