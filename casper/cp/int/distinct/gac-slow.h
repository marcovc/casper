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

#ifndef _H_CASPER_INT_DISTINCT_GAC_SLOW
#define _H_CASPER_INT_DISTINCT_GAC_SLOW


#include <casper/cp/int/distinct/val.h>
#include <casper/cp/filter/common.h>
#include <casper/cp/filter/dom.h>
#include <casper/kernel/reversible/stack.h>
#include <casper/cp/int/var.h>

#include <casper/cp/int/distinct/graph.h>

//#ifdef CASPER_PRECOMPILED
//#include <casper/cp/int/spexpr/explicit_postdom.h>
//#endif

namespace Casper {
namespace CP {

#if defined(USE_CASPER_DOM_ALLDIFF_V1) || defined(USE_CASPER_DOM_ALLDIFF_V2) || defined(USE_CASPER_DOM_ALLDIFF_V2_DELTA)

template<class View>
struct DomFilterView1<Distinct,IntSeq,View> : public IFilter
{
	struct VarDemon : INotifiable
	{
		DomFilterView1<Distinct,IntSeq,View>& rOwner;
		uint varIdx;
		VarDemon(DomFilterView1<Distinct,IntSeq,View>* pOwner, uint varIdx) :
			rOwner(*pOwner),varIdx(varIdx)
		{ rOwner.vars[varIdx].domain().attachOnDomain(this); }
		bool notify()
		{
			#if defined(USE_CASPER_DOM_ALLDIFF_V2_DELTA) || defined(USE_CASPER_DOM_ALLDIFF_V1)
			uint toMin = rOwner.vars[varIdx].domain().getLastErasedRangeMin() -
						 rOwner.minElem;
			uint toMax = rOwner.vars[varIdx].domain().getLastErasedRangeMax() -
						 rOwner.minElem;

			assert(!rOwner.vars[varIdx].domain().empty());
			rOwner.rematch.push(DomFilterView1<Distinct,IntSeq,View>::Delta(varIdx,toMin,toMax));
			#endif
			return rOwner.pParent->notify();
		}
	};

	DomFilterView1<Distinct,IntSeq,View>(Store& solver, const View& v);

	bool	execute();
	bool	entailed() const {	return false;	}	// FIXME
	void 	attach(INotifiable* pParent);
	void 	detach(INotifiable* pParent) {}
	Cost 	cost() const {	return quadraticHi; }
	private:

	struct Delta	{
		uint	varIdx;
		uint	min;
		uint	max;
		Delta(uint varIdx,uint min,uint	max) :
			varIdx(varIdx),min(min),max(max) {}
	};

	bool removeEdges();

	Store&						store;
	VarArray<int>			vars;
	int							minElem;
	int							maxElem;
	Detail::BiDiGraph			fn;
	Detail::MaximalMatching		mm;
	Detail::AllDiffHelper		ah;
	uint						bts;
	Stack<Delta>		rematch;
	typedef VarArray<int>::Dom	Dom;
	INotifiable*				pParent;
};

template<class View>
DomFilterView1<Distinct,IntSeq,View>::DomFilterView1(Store& solver, const View& v) :
		IFilter(solver),
		store(solver),
		vars(v),minElem(findMinElem(v)),maxElem(findMaxElem(v)),
		fn(solver,maxElem-minElem+1,v.size()),
		mm(solver,fn),
		ah(solver,fn),
		bts(0),
		rematch(solver)
{
	for (uint i = 0; i < v.size(); i++)
		for (typename Dom::ReverseIterator it = v[i].domain().rbegin();
					it != v[i].domain().rend(); ++it)
			fn.addEdgeDec(fn.girls[i],fn.boys[*it - minElem]);
}

template<class View>
void DomFilterView1<Distinct,IntSeq,View>::attach(INotifiable* pParent)
{
	this->pParent= pParent;
	for (uint i = 0; i < vars.size(); i++)
		VarDemon* d = new (store) VarDemon(this,i);

	// needed to handle some special cases
	postValFilter(store,distinct(vars));
	bool s = mm.compute();
	assert(s);	// FIXME: this is not an assertion
}

template<class View>
bool DomFilterView1<Distinct,IntSeq,View>::execute()
{
	typedef Detail::BiDiGraph::GirlNode::EdgeIterator EdgeIterator;

#if defined(USE_CASPER_DOM_ALLDIFF_V2) || defined(USE_CASPER_DOM_ALLDIFF_V2_DELTA)

#if defined(USE_CASPER_DOM_ALLDIFF_V2_DELTA)
	if (vars.solver().stats().fails() > bts)
#endif
	{
		bts = vars.solver().stats().fails();
		fn.removeAllEdges();
		for (uint i = 0; i < vars.size(); i++)
			for (typename Dom::ReverseIterator it = vars[i].domain().rbegin();
					it != vars[i].domain().rend(); ++it)
				fn.addEdgeDec(fn.girls[i],fn.boys[*it - minElem]);
		if (!mm.compute())
			return false;
	}
#if defined(USE_CASPER_DOM_ALLDIFF_V2_DELTA)
	else
#endif
#endif

#if defined(USE_CASPER_DOM_ALLDIFF_V1) || defined(USE_CASPER_DOM_ALLDIFF_V2_DELTA)
  	{
		bts = store.getState().getNbFails();
		while (!rematch.empty())
		{
			uint varIdx = rematch.top().varIdx;
			uint toMin = rematch.top().min;
			uint toMax = rematch.top().max;

			// girl->boy edges can be safely prunned
			for (EdgeIterator it = fn.girls[varIdx]->begin();
				it != fn.girls[varIdx]->end(); ++it)
				if (it->pBoy->idx >= toMin && it->pBoy->idx <= toMax)
					fn.removeEdge(it.get());

			// try to find a new match if removing
			// an edge belonging to current match
			if (fn.girls[varIdx]->inMatch())
			{
				uint b = fn.girls[varIdx]->matchNode()->idx;
				if (b >= toMin && b <= toMax)
				{
					assert(!vars[varIdx].domain().empty());
					fn.unsetBoyGirlEdge(fn.girls[varIdx]->match);
					if (!mm.match(fn.girls[varIdx]))
						return false;
				}
			}
			rematch.pop();
		}
	}
#endif

	assert(mm.size() == fn.nGirls());
	return removeEdges();
}

template<class View>
bool DomFilterView1<Distinct,IntSeq,View>::removeEdges()
{
	typedef Detail::BiDiGraph::GirlNode::EdgeIterator EdgeIterator;

	ah.reset();
	ah.markDirectPaths();
	ah.markSCC();

	// prune values
	for (uint g = 0; g < fn.nGirls(); g++)
	{
		for (EdgeIterator it = fn.girls[g]->begin();
				it != fn.girls[g]->end(); ++it)
			if (!ah.marked(it->pBoy,fn.girls[g]))
			{
				fn.unsetGirlBoyEdge(it.get());
				if (!vars[g].domain().erase(it->pBoy->idx+minElem))
					return false;
			}
		assert(!vars[g].domain().empty());
	}
	return true;
}


#endif // USE_CASPER_DOM_ALLDIFF_V2

#ifdef USE_CASPER_DOM_ALLDIFF_V3

template<class Eval,class View>
struct DomFilterView1<Distinct,Seq<Eval>,View> : public IFilter
{
	DomFilterView1<Distinct,Seq<Eval>,View>(Store& solver, const View& v);

	//bool	notify(Event event) { 	return pParent->notify(event); }
	bool	execute();
	bool	entailed() const {	return false;	}	// FIXME
	void attach(INotifiable* pParent);
	void detach(INotifiable* pParent) {}
	Cost cost() const {	return quadraticHi; }
	private:

	struct Delta	{
		uint	varIdx;
		uint	min;
		uint	max;
		Delta(uint varIdx,uint min,uint	max) :
			varIdx(varIdx),min(min),max(max) {}
	};

	bool removeEdges();

	Store&						store;
	DomArrayView<Eval,View>		doms;
	int							minElem;
	int							maxElem;
	Detail::BiDiGraph			fn;
	Detail::MaximalMatching		mm;
	Detail::AllDiffHelper		ah;
	Stack<Delta>				rematch;
	BacktrackSentinel			btSentinel;
	typedef VarArray<int>::Dom Dom;
};

template<class Eval,class View>
DomFilterView1<Distinct,Seq<Eval>,View>::DomFilterView1(Store& solver, const View& v) :
		IFilter(solver),
		store(solver),
		doms(store,v),minElem(Detail::findMin(solver,v)),maxElem(Detail::findMax(solver,v)),
		fn(solver,maxElem-minElem+1,doms.size()),
		mm(solver,fn),
		ah(solver,fn),
		rematch(solver),
		btSentinel(solver)
{
	for (uint i = 0; i < doms.size(); i++)
		for (auto it = doms[i]->rbegin(); it != doms[i]->rend(); ++it)
			fn.addEdgeDec(fn.girls[i],fn.boys[*it - minElem]);
}

template<class Eval,class View>
void DomFilterView1<Distinct,Seq<Eval>,View>::attach(INotifiable* pParent)
{
	for (uint i = 0; i < doms.size(); i++)
		doms[i].attach(pParent);

	//fn.debug();
	bool s = mm.compute();
	assert(s);	// FIXME: this is not an assertion
	//fn.assertConsistency();
	//fn.debug();
}


template<class Eval,class View>
bool DomFilterView1<Distinct,Seq<Eval>,View>::execute()
{
	typedef Detail::BiDiGraph::GirlNode::SortedEdgeIterator SortedEdgeIterator;
	typedef Detail::BiDiGraph::GirlNode::EdgeIterator EdgeIterator;

	bool bt = btSentinel.hasBacktracked();
	btSentinel.update();
	if (bt)
	{
		fn.removeAllEdges();
		for (uint i = 0; i < doms.size(); i++)
			for (auto it = doms[i]->rbegin(); it != doms[i]->rend(); ++it)
				fn.addEdgeDec(fn.girls[i],fn.boys[*it - minElem]);
		if (!mm.compute())
			return false;
	}
	else
	{
		Util::StdStack<Detail::BiDiGraph::PGirlNode>	rematch(store);
		for (uint varIdx = 0; varIdx < doms.size(); varIdx++)
		{
			typename Dom::Iterator dit = doms[varIdx]->begin();
			assert(dit != doms[varIdx]->end());

			SortedEdgeIterator eit = fn.girls[varIdx]->sortedBegin();
			bool toRematch = false;

			while (dit != doms[varIdx]->end())
			{
				assert(eit != fn.girls[varIdx]->sortedEnd());
				while (eit->pBoy->idx < *dit - minElem)
				{
					if (eit->pGirl->match && eit->pGirl->matchNode() == eit->pBoy)
					{
						toRematch = true;
						fn.unsetBoyGirlEdge(eit.get());
						fn.removeEdgeFromSorted(eit.get());
					}
					else
						fn.unsetGirlBoyEdge(eit.get());
					++eit;
					assert(eit != fn.girls[varIdx]->sortedEnd());
				}
				assert(eit->pBoy->idx == *dit - minElem);
				++eit;
				++dit;
			}

			while (eit != fn.girls[varIdx]->sortedEnd())
			{
				if (eit->pBoy->match && eit->pBoy->matchNode() == eit->pGirl)
				{
					toRematch = true;
					fn.unsetBoyGirlEdge(eit.get());
					fn.removeEdgeFromSorted(eit.get());
				}
				else
					fn.unsetGirlBoyEdge(eit.get());
				++eit;
			}
			if (toRematch)
				rematch.push(fn.girls[varIdx]);

		}
		while (!rematch.empty())
		{
			if (!mm.match(rematch.top()))
				return false;
			rematch.pop();
		}

	}
	assert(mm.size() == fn.nGirls());
	return removeEdges();
}


template<class Eval,class View>
bool DomFilterView1<Distinct,Seq<Eval>,View>::removeEdges()
{
	typedef Detail::BiDiGraph::GirlNode::EdgeIterator EdgeIterator;

	ah.reset();
	ah.markDirectPaths();
	ah.markSCC();

	// prune values
	for (uint g = 0; g < fn.nGirls(); g++)
	{
		for (EdgeIterator it = fn.girls[g]->begin();
				it != fn.girls[g]->end(); ++it)
			if (!ah.marked(it->pBoy,fn.girls[g]))
			{
				fn.unsetGirlBoyEdge(it.get());
				if (!doms[g]->erase(it->pBoy->idx+minElem))
					return false;
			}
		assert(!doms[g]->empty());
	}
	return true;
}


#endif // USE_CASPER_DOM_ALLDIFF_V3

template<class Eval,class View>
struct PostDomFilter1<Distinct,Seq<Eval>,View>
{
	static bool post(Store& s,const View& v)
	{
		// Val(distinct) is required (not redundant in this implementation)
		// Bnd(distinct) seems to slow down sometimes.
		return postValFilter(s,distinct(v)) and
			   postBndFilter(s,distinct(v)) and
			   s.post(new (s) DomFilterView1<Distinct,Seq<Eval>,View>(s,v));
	}
};

} // CP
} // Casper

#endif
