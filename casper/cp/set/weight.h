/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2009 - Marco Correia <marco.v.correia@gmail.com>           *
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


#ifndef CASPER_SET_WEIGHT_H_
#define CASPER_SET_WEIGHT_H_

#include <casper/cp/set/config.h>
#include <casper/cp/set/setfd.h>
#include <casper/cp/int/view.h>

#include <casper/kernel/common.h>
#include <casper/util/container/stdhashmap.h>

#include <algorithm>

namespace Casper {
namespace CP {

template<class Elem,class Expr1,class Expr2>
struct PostBndFilter2<SumEqual,Set<Elem>,Expr1,Elem,Expr2>
{
	template<class T>
	struct Eval
	{
		const T& operator[](const T& e) const
		{ return e; }
	};
	static bool post(Store& s, const Expr1& v1, const Expr2& v2)
	{
		return s.post(new (s) BndFilterView3<SumEqual,Set<int>,Expr1,
													  Eval<Elem>,Eval<Elem>,
													  int,Expr2>(s,v1,Eval<Elem>(),v2));

	//	return s.post(sumEqual(v1,Eval<Elem>(),v2));
	}
};

//#define CASPER_SETSUM_N

#ifdef CASPER_SETSUM_N

/**
 * Computes the sum of the evaluations of the elements in a set
 * domain variable where the evaluation of each element is given in a map.
 * \note works with negative evaluations also
 * \note needs testing
 */
template<class Arg1,class EvalMap,class Arg3>
struct BndFilterView3<SumEqual,Set<int>,Arg1,
					  EvalMap,EvalMap,
					  int,Arg3> : IFilter
{
	typedef typename DomView<Set<int>,Arg1>::Dom	DomX;

	BndFilterView3(Store& store,const Arg1& xx,
			  const EvalMap& evals,const Arg3& yy) :
				  IFilter(store),x(store,xx),
				  evals(evals),y(store,yy),
				  first(store,true),
				  knownCost(store,0),
				  posCost(store,0),
				  negCost(store,0),
#ifdef CASPER_OLDDELTAS
				  glbXDelta(*x),lubXDelta(*x)
#else
				  xGLBDeltasIt(x->glbDeltas().begin()),
				  xLUBDeltasIt(x->lubDeltas().begin())
#endif
				  {}

	bool execute();
	void attach(INotifiable* f)
	{
#ifdef CASPER_OLDDELTAS
		x->attachOnGLB(&glbXDelta);
		x->attachOnLUB(&lubXDelta);
		glbXDelta.setOwner(f);
		lubXDelta.setOwner(f);
#else
		x->attachOnDomain(f);
#endif
		y.attach(f);
	}
	void detach(INotifiable* f)
	{
#ifdef CASPER_OLDDELTAS
		x->detachOnGLB(&glbXDelta);
		x->detachOnLUB(&lubXDelta);
#else
		x->detachOnDomain(f);
		y.detach(f);
#endif
	}

	DomView<Set<int>,Arg1>			x;
	EvalMap							evals;
	BndView<int,Arg3>				y;
	Reversible<bool>				first;
	Reversible<int>					knownCost;
	Reversible<int>					posCost;
	Reversible<int>					negCost;
#ifdef CASPER_OLDDELTAS
	Detail::ProxyDelta<DomX>		glbXDelta;
	Detail::ProxyDelta<DomX>		lubXDelta;
#else
	typename DomX::DeltasIterator 	xGLBDeltasIt;
	typename DomX::DeltasIterator 	xLUBDeltasIt;
#endif
};

template<class Arg1,class EvalMap,class Arg3>
bool BndFilterView3<SumEqual,Set<int>,Arg1,
					EvalMap,EvalMap,int,Arg3>::execute()
{
	bool updateY = false;

	if (first)
	{
		first = false;

		// find known cost
		knownCost = 0;
		for (typename DomX::IIterator it = x->beginIn();
				it != x->endIn(); ++it)
			knownCost += evals[*it];

		// find pos and neg cost
		posCost = negCost = 0;
		for (typename DomX::PIterator it = x->beginPoss();
					it != x->endPoss(); ++it)
			if (evals[*it] >= 0)
				posCost += evals[*it];
			else
				negCost += evals[*it];
#if not defined(CASPER_OLDDELTAS)
		xGLBDeltasIt = x->glbDeltas().end();
		xLUBDeltasIt = x->lubDeltas().end();
#endif
		updateY = true;
	}
	else
	{
		int knownCostDelta,posCostDelta,negCostDelta;
		knownCostDelta = posCostDelta = negCostDelta = 0;

#if defined(CASPER_OLDDELTAS)
		while (!glbXDelta.empty())
		{
			for (typename DomX::Delta::Iterator it = glbXDelta.top().begin();
					it != glbXDelta.top().end(); ++it)
			{
				knownCostDelta += evals[*it];
				if (evals[*it]>=0)
					posCostDelta += evals[*it];
				else
					negCostDelta += evals[*it];
			}
			glbXDelta.pop();
		}
		while (!lubXDelta.empty())
		{
			for (typename DomX::Delta::Iterator it = lubXDelta.top().begin();
					it != lubXDelta.top().end(); ++it)
				if (evals[*it] >= 0)
					posCostDelta += evals[*it];
				else
					negCostDelta += evals[*it];
			lubXDelta.pop();
		}
#else
		for (xGLBDeltasIt = x->glbDeltas().beginFrom(xGLBDeltasIt);
				xGLBDeltasIt != x->glbDeltas().end(); ++xGLBDeltasIt)
			for (typename DomX::PIterator it = xGLBDeltasIt->begin();
					it != xGLBDeltasIt->end(); ++it)
			{
				knownCostDelta += evals[*it];
				if (evals[*it]>=0)
					posCostDelta += evals[*it];
				else
					negCostDelta += evals[*it];
			}
		for (xLUBDeltasIt = x->lubDeltas().beginFrom(xLUBDeltasIt);
				xLUBDeltasIt != x->lubDeltas().end(); ++xLUBDeltasIt)
			for (typename DomX::PIterator it = xLUBDeltasIt->begin();
					it != xLUBDeltasIt->end(); ++it)
				if (evals[*it] >= 0)
					posCostDelta += evals[*it];
				else
					negCostDelta += evals[*it];
#endif

		if (knownCostDelta or posCostDelta or negCostDelta)
		{
			knownCost += knownCostDelta;
			posCost -= posCostDelta;
			negCost -= negCostDelta;
			updateY = true;
		}
	}

	// propagate to y
	if (updateY and !y.updateRange(knownCost+negCost,knownCost+posCost))
		return false;

	// remove every element e where weight[e]-known_cost>max
	for (typename DomX::PIterator it = x->beginPoss(); it != x->endPoss(); ++it)
		if ((evals[*it]>=0 and knownCost+evals[*it]+negCost > y.max()) or
			(evals[*it]<=0 and knownCost+evals[*it]+posCost < y.min()))
			if (!x->erase(it))
				return false;

	// we can at least increase card sometimes
	if (y.min() > knownCost or	y.max() < knownCost)
		if (!x->card().updateMin(x->inSize()+1))
			return false;

	return true;
}

#else

/**
 * Computes the sum of the evaluations of the elements in a set
 * domain variable where the evaluation of each element is given in a map.
 * \note works with negative evaluations also
 * \note makes more inference on the cardinality, but is nlog(n) instead of n
 * \note needs testing
 */
template<class Arg1,class EvalMap,class Arg3>
struct BndFilterView3<SumEqual,Set<int>,Arg1,
					  EvalMap,EvalMap,
					  int,Arg3> : IFilter
{
	typedef typename DomView<Set<int>,Arg1>::Dom	DomX;

	BndFilterView3(Store& store,const Arg1& xx,
			  const EvalMap& evals,const Arg3& yy) :
				  IFilter(store),x(store,xx),
				  evals(evals),y(store,yy),
				  pOwner(NULL) {}

	bool execute();
	void attach(INotifiable* f)
	{
		assert(pOwner==NULL or pOwner==f);
		pOwner = f;
		x->attachOnDomain(pOwner);
		y.attach(pOwner);
	}
	void detach()
	{	x->detachOnDomain(pOwner); y.detach();	}

	DomView<Set<int>,Arg1>			x;
	EvalMap						evals;
	BndView<int,Arg3>				y;
	INotifiable*				pOwner;
};

template<class Arg1,class EvalMap,class Arg3>
bool BndFilterView3<SumEqual,Set<int>,Arg1,
					EvalMap,EvalMap,int,Arg3>::execute()
{
	// find known cost
	int knownCost = 0;
	for (typename DomX::IIterator it = x->beginIn();
				it != x->endIn(); ++it)
		knownCost += evals[*it];

	// find pos and neg cost
	int posCost,negCost;
	posCost = negCost = 0;
	if (x->card().max() - x->inSize() >= x->possSize())
	{
		for (typename DomX::PIterator it = x->beginPoss();
						it != x->endPoss(); ++it)
			if (evals[*it] >= 0)
				posCost += evals[*it];
			else
				negCost += evals[*it];
	}
	else	// find tighter bounds
	{
		std::vector<int> w(x->possSize());
		uint c = 0;
		for (typename DomX::PIterator it = x->beginPoss();
						it != x->endPoss(); ++it)
			w[c++] = evals[*it];
		std::sort(w.begin(),w.end());
		for (uint i = 0; i < x->card().max() - x->inSize(); ++i)
			if (w[i]<0)
				negCost += w[i];
		for (uint i = x->possSize()- x->card().max() + x->inSize();
				i < x->possSize(); ++i)
			if (w[i]>0)
				posCost += w[i];
	}

	// propagate to y
	if (!y.updateRange(knownCost+negCost,knownCost+posCost))
		return false;

	// remove every element e where weight[e]-known_cost>max
	for (typename DomX::PIterator it = x->beginPoss(); it != x->endPoss(); )
		if ((evals[*it]>=0 and knownCost+evals[*it]+negCost > y.max()) or
			(evals[*it]<=0 and knownCost+evals[*it]+posCost < y.min()))
		{
			if (!x->erase(it++))
				return false;
		}
		else
			++it;

	// we can at least increase card sometimes
	if (y.min() > knownCost or	y.max() < knownCost)
		if (!x->card().updateMin(x->inSize()+1))
			return false;

	return true;
}

#endif


#if 0
// associates a weight with each element in a set and returns the max
// * this assumes the elements in the set are ints
// (otherwise must use an hashmap for weights)
// * works with negative weights also
// * needs testing
template<class Expr1,class Weights,class Expr2>
struct WeightMax : IFilter
{
	typedef typename DomView<Set<int>,Expr1>::Dom	DomX;

	WeightMax(Store& store,const Expr1& xx,
			  const Weights& weights,const Expr2& yy) :
				  IFilter(store),x(store,xx),
				  weights(weights),y(store,yy) {}

	bool execute();
	void attach(INotifiable* f)
	{	pOwner = f; x->attachOnGLB(f); x->attachOnLUB(f); y.attach(f);	}
	void detach(INotifiable* f)
	{	x->detachOnGLB(f); x->detachOnLUB(f); y.detach(f);	}

	DomView<Set<int>,Expr1>			x;
	Weights							weights;
	BndView<int,Expr2>				y;
	INotifiable*					pOwner;
};

template<class Expr1,class Weights,class Expr2>
bool WeightMax<Expr1,Weights,Expr2>::execute()
{
	// remove every element e where weight[e] > max
	for (typename DomX::PIterator it = x->beginPoss();
			it != x->endPoss(); ++it)
		if (weights[*it]> y.max())
			if (!x->erase(it))
				return false;

	// find known max
	int knownMax = (x->inSize())?limits<int>::min():0;
	for (typename DomX::IIterator it = x->beginIn();
			it != x->endIn(); ++it)
		if (weights[*it] > knownMax)
			knownMax = weights[*it];

	// find poss max
	int possMax = limits<int>::min();
	int possMin = limits<int>::max();
	for (typename DomX::PIterator it = x->beginPoss();
				it != x->endPoss(); ++it)
	{
		if (weights[*it] > possMax)
			possMax = weights[*it];
		if (weights[*it] < possMin)
			possMin = weights[*it];
	}

	// propagate to y
	if (!y.updateRange((x->inSize())?knownMax:std::min(knownMax,possMin),
						std::max(knownMax,possMax)))
		return false;

	// check for entailnment
	if (y.max() == y.min() and knownMax == y.max())
	{
		detach(pOwner);
		return true;
	}

	// don't waste time on computations below
	if (knownMax >= y.min() or x->inSize()==0)
		return true;

	// if there is only one element in poss >= y.min, then insert it
	typename DomX::PIterator itm = x->endPoss();
	typename DomX::PIterator it = x->beginPoss();
	for ( ; it != x->endPoss(); ++it)
		if (weights[*it]>= y.min())
			if (itm == x->endPoss())
				itm = it;
			else
				break;
	// if found exactly one element in poss >= y.min
	if (it == x->endPoss() and itm != x->endPoss())
		return x->insert(itm);
	else
		// otherwise just increase card
		return x->card().updateMin(x->inSize()+1);
}

template<class Expr1,class Weights,class Expr2>
Filter	weightMax(const Expr1& v1,const Weights& weights,const Expr2& v2)
{	return new (getState(v1,v2).getHeap())
		WeightMax<Expr1,Weights,Expr2>(getState(v1,v2),v1,weights,v2);	}
#endif

} // CP
} // Casper

#endif /* CASPER_SET_WEIGHT_H_ */
