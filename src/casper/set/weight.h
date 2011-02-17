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

#include <casper/set/config.h>
#include <casper/set/setfd.h>
#include <casper/int/view.h>

#include <casper/kernel/common.h>
#include <casper/kernel/container/hashmap.h>

#include <algorithm>

namespace casper {

template<class Elem,class View1,class View2>
struct BndFilterFact2<SumEqual,Set<Elem>,View1,Elem,View2>
{
	template<class T>
	struct Eval
	{
		const T& operator[](const T& e) const
		{ return e; }
	};
	static Filter create(ICPSolver& s, const View1& v1, const View2& v2)
	{	return sumEqual(v1,Eval<Elem>(),v2);	}
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
struct BndFilterView3<SumEqual,Set<Int>,Arg1,
					  EvalMap,EvalMap,
					  Int,Arg3> : IFilter
{
	typedef typename DomView<Set<Int>,Arg1>::Dom	DomX;

	BndFilterView3(ICPSolver& solver,const Arg1& xx,
			  const EvalMap& evals,const Arg3& yy) :
				  IFilter(solver),x(solver,xx),
				  evals(evals),y(solver,yy),
				  first(solver,true),
				  knownCost(solver,0),
				  posCost(solver,0),
				  negCost(solver,0),
#ifdef CASPER_OLDDELTAS
				  glbXDelta(*x),lubXDelta(*x)
#else
				  xGLBDeltasIt(x->glbDeltas().begin()),
				  xLUBDeltasIt(x->lubDeltas().begin())
#endif
				  {}

	Bool execute();
	Void attach(INotifiable* f)
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
	Void detach(INotifiable* f)
	{
#ifdef CASPER_OLDDELTAS
		x->detachOnGLB(&glbXDelta);
		x->detachOnLUB(&lubXDelta);
#else
		x->detachOnDomain(f);
		y.detach(f);
#endif
	}

	DomView<Set<Int>,Arg1>			x;
	EvalMap							evals;
	BndView<Int,Arg3>				y;
	Reversible<Bool>				first;
	Reversible<Int>					knownCost;
	Reversible<Int>					posCost;
	Reversible<Int>					negCost;
#ifdef CASPER_OLDDELTAS
	detail::ProxyDelta<DomX>		glbXDelta;
	detail::ProxyDelta<DomX>		lubXDelta;
#else
	typename DomX::DeltasIterator 	xGLBDeltasIt;
	typename DomX::DeltasIterator 	xLUBDeltasIt;
#endif
};

template<class Arg1,class EvalMap,class Arg3>
Bool BndFilterView3<SumEqual,Set<Int>,Arg1,
					EvalMap,EvalMap,Int,Arg3>::execute()
{
	Bool updateY = false;

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
		Int knownCostDelta,posCostDelta,negCostDelta;
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
struct BndFilterView3<SumEqual,Set<Int>,Arg1,
					  EvalMap,EvalMap,
					  Int,Arg3> : IFilter
{
	typedef typename DomView<Set<Int>,Arg1>::Dom	DomX;

	BndFilterView3(ICPSolver& solver,const Arg1& xx,
			  const EvalMap& evals,const Arg3& yy) :
				  IFilter(solver),x(solver,xx),
				  evals(evals),y(solver,yy) {}

	Bool execute();
	Void attach(INotifiable* f)
	{	x->attachOnDomain(f); y.attach(f);	}
	Void detach(INotifiable* f)
	{	x->detachOnDomain(f); y.detach(f);	}

	DomView<Set<Int>,Arg1>			x;
	EvalMap						evals;
	BndView<Int,Arg3>				y;
};

template<class Arg1,class EvalMap,class Arg3>
Bool BndFilterView3<SumEqual,Set<Int>,Arg1,
					EvalMap,EvalMap,Int,Arg3>::execute()
{
	// find known cost
	Int knownCost = 0;
	for (typename DomX::IIterator it = x->beginIn();
				it != x->endIn(); ++it)
		knownCost += evals[*it];

	// find pos and neg cost
	Int posCost,negCost;
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
		std::vector<Int> w(x->possSize());
		UInt c = 0;
		for (typename DomX::PIterator it = x->beginPoss();
						it != x->endPoss(); ++it)
			w[c++] = evals[*it];
		std::sort(w.begin(),w.end());
		for (UInt i = 0; i < x->card().max() - x->inSize(); ++i)
			if (w[i]<0)
				negCost += w[i];
		for (UInt i = x->possSize()- x->card().max() + x->inSize();
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
template<class View1,class Weights,class View2>
struct WeightMax : IFilter
{
	typedef typename DomView<Set<Int>,View1>::Dom	DomX;

	WeightMax(ICPSolver& solver,const View1& xx,
			  const Weights& weights,const View2& yy) :
				  IFilter(solver),x(solver,xx),
				  weights(weights),y(solver,yy) {}

	Bool execute();
	Void attach(INotifiable* f)
	{	pOwner = f; x->attachOnGLB(f); x->attachOnLUB(f); y.attach(f);	}
	Void detach(INotifiable* f)
	{	x->detachOnGLB(f); x->detachOnLUB(f); y.detach(f);	}

	DomView<Set<Int>,View1>			x;
	Weights							weights;
	BndView<Int,View2>				y;
	INotifiable*					pOwner;
};

template<class View1,class Weights,class View2>
Bool WeightMax<View1,Weights,View2>::execute()
{
	// remove every element e where weight[e] > max
	for (typename DomX::PIterator it = x->beginPoss();
			it != x->endPoss(); ++it)
		if (weights[*it]> y.max())
			if (!x->erase(it))
				return false;

	// find known max
	Int knownMax = (x->inSize())?limits<Int>::min():0;
	for (typename DomX::IIterator it = x->beginIn();
			it != x->endIn(); ++it)
		if (weights[*it] > knownMax)
			knownMax = weights[*it];

	// find poss max
	Int possMax = limits<Int>::min();
	Int possMin = limits<Int>::max();
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

template<class View1,class Weights,class View2>
Filter	weightMax(const View1& v1,const Weights& weights,const View2& v2)
{	return new (getCPSolver(v1,v2).heap())
		WeightMax<View1,Weights,View2>(getCPSolver(v1,v2),v1,weights,v2);	}
#endif

}

#endif /* CASPER_SET_WEIGHT_H_ */
