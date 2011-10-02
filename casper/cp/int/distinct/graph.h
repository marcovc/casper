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

#ifndef _H_CASPER_KERNEL_GRAPH
#define _H_CASPER_KERNEL_GRAPH

#include <casper/kernel/reversible/slist.h>
#include <casper/util/container/stdvector.h>

namespace Casper {
namespace Detail {

#if defined(USE_CASPER_DOM_ALLDIFF_V1) || defined(USE_CASPER_DOM_ALLDIFF_V2_DELTA)

struct DiGraphBoyNode;
struct DiGraphGirlNode;

struct Edge
{
	typedef DiGraphBoyNode*	const	PBoy;
	typedef DiGraphGirlNode* const	PGirl;
	typedef Edge*				PEdge;
	typedef Reversible<PEdge>	RPEdge;

	Edge(CPSolver& s,PBoy pBoy, PGirl pGirl) :
		pBoy(pBoy),pGirl(pGirl),
		pPrevEdgeBoy(s,NULL),pNextEdgeBoy(s,NULL),
		pPrevEdgeGirl(s,NULL),pNextEdgeGirl(s,NULL) {}

	PBoy	pBoy;
	PGirl	pGirl;
	RPEdge	pPrevEdgeBoy;
	RPEdge	pNextEdgeBoy;
	RPEdge	pPrevEdgeGirl;
	RPEdge	pNextEdgeGirl;
};

struct DiGraphNode
{
	typedef	DiGraphNode					Self;
	typedef Self*						PNode;
	typedef	Detail::Edge				Edge;
	typedef Edge*						PEdge;
	typedef Reversible<PNode>			RPNode;
	typedef Reversible<PEdge>			RPEdge;

	struct EdgeIterator
	{
		EdgeIterator(PEdge pEdge) : pEdge(pEdge) {}
		EdgeIterator(const EdgeIterator& s)
		{this->pEdge = s.pEdge;}
		bool operator==(const EdgeIterator& s) const
		{ return pEdge == s.pEdge; }
		bool operator!=(const EdgeIterator& s) const
		{ return pEdge != s.pEdge; }
		Edge& operator*() const
		{ return *pEdge; }
		PEdge operator->() const
		{ return pEdge;  }
		PEdge get() const
		{ return pEdge;  }

		const EdgeIterator& operator=(const EdgeIterator& i)
		{ pEdge = i.pEdge;	return *this; }
		PEdge pEdge;
	};

	DiGraphNode(CPSolver& s, uint idx) :
		edges(s,NULL),match(s,NULL), idx(idx) {}

	bool inMatch() const
	{	return match != NULL;	}

	RPEdge		edges;
	RPEdge		match;
	const uint 	idx;
};

struct DiGraphGirlNode;

struct DiGraphBoyNode : DiGraphNode
{
	typedef	DiGraphNode					Super;
	typedef DiGraphBoyNode				Self;
	typedef Super::PNode				PNode;
	typedef Super::PEdge				PEdge;

	struct EdgeIterator : Super::EdgeIterator
	{
		EdgeIterator(PEdge pEdge) : Super::EdgeIterator(pEdge) {}
		EdgeIterator& operator++() // pre-increment
		{ pEdge = pEdge->pNextEdgeBoy; return *this; }
		EdgeIterator operator++(int) //post-increment
		{ EdgeIterator ret(*this); pEdge = pEdge->pNextEdgeBoy; return ret; }
	};

	DiGraphBoyNode(CPSolver& s, uint idx) :
		Super(s,idx) {}

	EdgeIterator begin() { return EdgeIterator(edges); }
	EdgeIterator end()   { return EdgeIterator(NULL); }

	DiGraphGirlNode* matchNode()
	{	return (DiGraphGirlNode*)match->pGirl;	}
};

struct DiGraphGirlNode : DiGraphNode
{
	typedef	DiGraphNode					Super;
	typedef DiGraphGirlNode				Self;
	typedef Super::PNode				PNode;
	typedef Super::PEdge				PEdge;

	struct EdgeIterator : Super::EdgeIterator
	{
		EdgeIterator(PEdge pEdge) : Super::EdgeIterator(pEdge) {}
		EdgeIterator& operator++() // pre-increment
		{ pEdge = pEdge->pNextEdgeGirl; return *this; }
		EdgeIterator operator++(int) //post-increment
		{ EdgeIterator ret(*this); pEdge = pEdge->pNextEdgeGirl; return ret; }
	};

	DiGraphGirlNode(CPSolver& s, uint idx) :
		Super(s,idx) {}

	EdgeIterator begin() { return EdgeIterator(edges); }
	EdgeIterator end()   { return EdgeIterator(NULL); }

	DiGraphBoyNode* matchNode()
	{	return (DiGraphBoyNode*)match->pBoy;	}
};

struct BiDiGraph
{
	typedef	BiDiGraph			Self;

	typedef DiGraphNode			Node;
	typedef Node*				PNode;

	typedef DiGraphNode::Edge	Edge;
	typedef DiGraphNode::PEdge	PEdge;

	typedef DiGraphNode::EdgeIterator	EdgeIterator;
	typedef DiGraphBoyNode		BoyNode;
	typedef BoyNode*			PBoyNode;

	typedef DiGraphGirlNode		GirlNode;
	typedef GirlNode*			PGirlNode;

	typedef Util::StdVector<PBoyNode>	BoyNodes;
	typedef BoyNodes::Iterator	BoyNodeIterator;

	typedef Util::StdVector<PGirlNode>	GirlNodes;
	typedef GirlNodes::Iterator	GirlNodeIterator;

	BiDiGraph(CPSolver& solver, uint nBoys, uint nGirls) :
		solver(solver),
		boys(solver.sHeap(),nBoys,NULL),
		girls(solver.sHeap(),nGirls,NULL),
		_nBoys(nBoys),
		_nGirls(nGirls),
		count(0),
		comp(solver.sHeap(),nBoys+nGirls,0)
	{
		for (uint i = 0; i < nBoys; i++)
			boys[i] = new (solver.sHeap()) BoyNode(solver,i);
		for (uint i = 0; i < nGirls; i++)
			girls[i] = new (solver.sHeap()) GirlNode(solver,i+nBoys);
	}

	void setBoyGirlEdge(PEdge pEdge)	// match:	O(1)
	{
		pEdge->pBoy->match = pEdge->pGirl->match = pEdge;
		pEdge->pNextEdgeBoy = pEdge->pPrevEdgeBoy =
				pEdge->pNextEdgeGirl = pEdge->pPrevEdgeGirl = NULL;
	}

	void setGirlBoyEdge(PEdge pEdge)	// not in match: O(1)
	{
		pEdge->pNextEdgeBoy = pEdge->pBoy->edges;
		pEdge->pNextEdgeGirl = pEdge->pGirl->edges;
		if (pEdge->pBoy->edges)
			pEdge->pBoy->edges->pPrevEdgeBoy = pEdge;
		if (pEdge->pGirl->edges)
			pEdge->pGirl->edges->pPrevEdgeGirl = pEdge;
		pEdge->pBoy->edges = pEdge;
		pEdge->pGirl->edges = pEdge;
	}

	void addEdgeDec(PGirlNode g, PBoyNode b)	// not in match: O(1)
	{
		PEdge e = new (solver.getHeap()) Edge(solver,b,g);
		setGirlBoyEdge(e);
	}

	void invertBoyGirlEdge(PEdge pEdge)	// match: O(1)
	{	unsetBoyGirlEdge(pEdge); setGirlBoyEdge(pEdge);	}
	void invertGirlBoyEdge(PEdge pEdge)	// not in match: O(1)
	{	removeEdge(pEdge); setBoyGirlEdge(pEdge);	}
	void invertEdge(EdgeIterator it)	// not in match: O(1)
	{	invertGirlBoyEdge(it.get());	}

	bool isBoyGirlEdge(PEdge e) const	// has match: O(1)
	{
		 assert(e->pBoy->match == e || e->pGirl->match != e);
		 return e->pBoy->match == e;
	}

	bool hasEdge(PGirlNode g,PBoyNode b) const	// not in match: O(N)
	{
		for (GirlNode::EdgeIterator it = g->begin(); it != g->end(); ++it)
			if (it->pBoy == b) return true;
		return false;
	}

	bool hasEdge(PBoyNode b,PGirlNode g) const	// not in match: O(N)
	{	return b->match == g->match && b->match != NULL;	}

	void unsetBoyGirlEdge(PEdge pEdge)	// in match: O(1)
	{	pEdge->pBoy->match = pEdge->pGirl->match =  NULL; 	}

	void unsetGirlBoyEdge(PEdge pEdge)	// not in match: O(1)
	{
		removeEdge(pEdge);
	}

	void removeEdge(PEdge pEdge)	// not in match: O(1)
	{
		if (pEdge->pNextEdgeBoy)
			pEdge->pNextEdgeBoy->pPrevEdgeBoy = pEdge->pPrevEdgeBoy;
		if (pEdge->pPrevEdgeBoy)
			pEdge->pPrevEdgeBoy->pNextEdgeBoy = pEdge->pNextEdgeBoy;
		else
			pEdge->pBoy->edges = pEdge->pNextEdgeBoy;
		if (pEdge->pNextEdgeGirl)
			pEdge->pNextEdgeGirl->pPrevEdgeGirl = pEdge->pPrevEdgeGirl;
		if (pEdge->pPrevEdgeGirl)
			pEdge->pPrevEdgeGirl->pNextEdgeGirl = pEdge->pNextEdgeGirl;
		else
			pEdge->pGirl->edges = pEdge->pNextEdgeGirl;
	}

	void removeAllEdges()
	{
		for (uint g = 0; g < nGirls(); g++)
		{
			girls[g]->match = NULL;
			girls[g]->edges = NULL;
		}
		for (uint b = 0; b < nBoys(); b++)
		{
			boys[b]->match = NULL;
			boys[b]->edges = NULL;
		}
	}

	uint nBoys() const	{ return _nBoys;	}
	uint nGirls() const	{ return _nGirls;	}
	uint nNodes() const	{ return nBoys()+nGirls(); }

	CPSolver&		solver;
	BoyNodes	boys;
	GirlNodes	girls;

	uint	_nBoys;
	uint	_nGirls;

	uint			count;
	Util::StdVector<uint>	comp;
};

struct MaximalMatching
{
	typedef	BiDiGraph::GirlNode		GirlNode;
	typedef	BiDiGraph::PGirlNode	PGirlNode;
	typedef	BiDiGraph::BoyNode		BoyNode;
	typedef	BiDiGraph::PBoyNode		PBoyNode;

	MaximalMatching(CPSolver& sol,BiDiGraph& net) :
							net(net) {}

	bool compute();
	uint size();

	bool inMatch(PBoyNode b, PGirlNode g) const	{	return b->match == g->match;	}

	bool searchMatch(PGirlNode	pGirl);
	bool match(PGirlNode	pGirl);

	protected:
	BiDiGraph&		net;
};

// Hopcroft-Karp algorithm - finds a maximal matching
bool MaximalMatching::compute()
{
	for (uint g = 0; g < net.nGirls(); g++)
		if (!match(net.girls[g]))
			return false;
    assert(size()==net.nGirls());
    return true;
}

// DFS for an augmenting path from the vertice
// currently at the top of the stack to SNK.
// (part of Hopcroft-Karp algorithm)
bool MaximalMatching::searchMatch(PGirlNode	pGirl)
{
	net.comp[pGirl->idx] = net.count;

    // Try to find matching edge cheaply: is there a free edge around?

	//assert(!pGirl->next.empty());
    for (GirlNode::EdgeIterator it = pGirl->begin();
			it != pGirl->end(); ++it)
    	if (it->pBoy->match == NULL)	// if boy is free
        {
        	net.invertEdge(it);
			return true;
        }

    // No, find matching edge by augmenting path method
    for (GirlNode::EdgeIterator it1 = pGirl->begin();
			it1 != pGirl->end(); ++it1)
	{
		BiDiGraph::PEdge pEdge2 = it1->pBoy->match;
		assert(pEdge2 != NULL);
		if(net.comp[pEdge2->pGirl->idx] < net.count && searchMatch(pEdge2->pGirl))
		{
			net.setGirlBoyEdge(pEdge2); // revert (boy->girl)
			net.invertEdge(it1); 	// revert (girl->boy)
          	return true;
        }
	}
    return false;
}

bool MaximalMatching::match(PGirlNode pGirl)
{
    net.count = net.count + 1;
    return searchMatch(pGirl);
}

// public
uint MaximalMatching::size()
{
	uint s = 0;
	for (uint g = 0; g < net.nGirls(); g++)
		if (net.girls[g]->inMatch())
			s++;
	return s;
}

struct AllDiffHelper
{
	typedef	BiDiGraph::PNode		PNode;
	typedef	BiDiGraph::GirlNode		GirlNode;
	typedef	BiDiGraph::PGirlNode	PGirlNode;
	typedef	BiDiGraph::BoyNode		BoyNode;
	typedef	BiDiGraph::PBoyNode		PBoyNode;

	void reset();
	void markDirectPaths();
	void markSCC();
	int marked(PNode b, PNode g) const
	{
		return 	 rindex[b->idx] == rindex[g->idx] ||
			 	 (net.comp[b->idx]==net.count && net.comp[g->idx]==net.count);
	}

	AllDiffHelper(CPSolver& sol,BiDiGraph& net) :
			net(net),
			d(sol.getHeap(),net.nBoys(),net.nGirls(),0),
			toVisit(sol.getHeap(),net.nBoys()+net.nGirls(),0),
			s(sol.getHeap(),net.nBoys()*net.nGirls(),0),
			rindex(sol.getHeap(),net.nNodes(),0),
			pre(sol.getHeap(),net.nNodes(),0),
			low(sol.getHeap(),net.nNodes(),0),
			cnt1(0),
			cnt0(0)
		{}

	//private:
	void visitSCCGirl(PGirlNode);
	void visitSCCBoy(PBoyNode);
	bool usedEdge(PNode n1, PNode n2) const
	{
		if (n1->idx < n2->idx)
			return d[n1->idx][n2->idx-net.nBoys()];
		else
			return d[n2->idx][n1->idx-net.nBoys()];
	}
	void markEdgeAsUsed(PNode n1, PNode n2)
	{
		if (n1->idx < n2->idx)
			d[n1->idx][n2->idx-net.nBoys()] = 1;
		else
			d[n2->idx][n1->idx-net.nBoys()] = 1;
	}

	void	scc(PNode p);

	void	debug() const
	{
		for (uint b = 0; b <net.nBoys(); b++)
		{
			for (uint g = 0; g <net.nGirls(); g++)
				if (net.hasEdge(net.boys[b],net.girls[g]))
					if (marked(net.boys[b],net.girls[g]))
						std::cout << "2 ";
					else
						std::cout << "1 ";
				else
				if (net.hasEdge(net.girls[g],net.boys[b]))
					if (marked(net.girls[g],net.boys[b]))
						std::cout << "-2 ";
					else
						std::cout << "-1 ";
				else
					std::cout << "0 ";
			std::cout << endl;
		}
		std::cout << endl;
	}

	BiDiGraph&		net;
	Util::StdArray<int,2> 	d;			//  where to mark directed paths and
								//  strongly connected components
	Util::StdVector<PBoyNode> 	toVisit;	//	aux vector used in markDirectPaths
	Util::StdVector<PNode> 	s;			// 	global stack for scc
	int 			sSize;		// 	stack size (for efficiency)
	uint			c;			//  aux counter for SCC
	uint			index;		//  aux counter for SCC
	Util::StdVector<int> 	rindex;		//  aux array to color SCC

	Util::StdVector<uint>	pre;
	Util::StdVector<uint>	low;
	uint			cnt1;
	uint			cnt0;
	uint			toVisitSize;
};

void AllDiffHelper::reset()
{
}

// toVisit is a limited size stack, implemented with a vector for efficiency
void AllDiffHelper::markDirectPaths()
{
	toVisitSize = 0;
	PBoyNode v;
	net.count = net.count + 1;

	// populate toVisit with free vertices
	// (which must be boys since we have a maximal matching)
	for (uint b = 0; b < net.nBoys(); b++)
		if (!net.boys[b]->inMatch())
		{
			toVisit[toVisitSize++] = net.boys[b];
			net.comp[net.boys[b]->idx] = net.count;
		}

	// depth-first search for direct paths
	while (toVisitSize > 0)
	{
		v = toVisit[--toVisitSize];

		for (BoyNode::EdgeIterator itg = v->begin();
				itg != v->end(); ++itg)
			if (net.comp[itg->pGirl->idx] < net.count)
			{
				net.comp[itg->pGirl->idx] = net.count;
				PBoyNode pb = itg->pGirl->matchNode();
				assert(pb != NULL);
				if (net.comp[pb->idx] < net.count)
				{
					net.comp[pb->idx] = net.count;
					toVisit[toVisitSize++] = pb;
				}
			}
	}
}

void AllDiffHelper::visitSCCGirl(PGirlNode v)
{
	bool root = true;
	rindex[v->idx] = index; index++;
	PNode w;

	for (GirlNode::EdgeIterator  it = v->begin(); it != v->end(); ++it)
		if (net.comp[it->pBoy->idx] < net.count)
		{
			if (rindex[it->pBoy->idx] == 0)
				visitSCCBoy(it->pBoy);
			if (rindex[it->pBoy->idx] < rindex[v->idx])
			{	rindex[v->idx] = rindex[it->pBoy->idx]; root = false;	}
		}

	if (root)
	{
		--index;
		while (sSize > 0 and rindex[v->idx] <= rindex[s[sSize-1]->idx])
		{
			w = s[--sSize];
			rindex[w->idx] = c;
			--index;
		}
		rindex[v->idx] = c;
		--c;
	}
	else
		s[sSize++] = v;
}

void AllDiffHelper::visitSCCBoy(PBoyNode v)
{
	bool root = true;
	rindex[v->idx] = index; index++;
	PNode w;

	PGirlNode g = v->matchNode();
	//GirlNode* g = (GirlNode*)v->next;
	if (g && net.comp[g->idx] < net.count)
	{
		if (rindex[g->idx] == 0)
			visitSCCGirl(g);
		if (rindex[g->idx] < rindex[v->idx])
		{	rindex[v->idx] = rindex[g->idx]; root = false;	}
	}

	if (root)
	{
		--index;
		while (sSize > 0 and rindex[v->idx] <= rindex[s[sSize-1]->idx])
		{
			w = s[--sSize];
			rindex[w->idx] = c;
			--index;
		}
		rindex[v->idx] = c;
		--c;
	}
	else
		s[sSize++] = v;
}

// find strongly connected components of a directed graph g
void AllDiffHelper::markSCC()
{
	sSize = 0;
	index = 1;
	c = net.nNodes()-1;

	// zero array
	rindex = 0;

	// only need visit girls because any SCC has to go through a girl
	for (BiDiGraph::GirlNodeIterator it = net.girls.begin();
			it != net.girls.end(); ++it)
		if (net.comp[(*it)->idx] < net.count)
	{
		// optimization: only visit girls which have at least one flow out
		//if (rindex[getIdx(*it)] == 0 && !((GirlNode*)(*it))->next.empty())
		if (rindex[(*it)->idx] == 0 && (*it)->inMatch())
			visitSCCGirl(*it);
	}

}

#endif //USE_CASPER_DOM_ALLDIFF_V1

#ifdef USE_CASPER_DOM_ALLDIFF_V2

struct DiGraphBoyNode;
struct DiGraphGirlNode;

struct Edge
{
	typedef DiGraphBoyNode*	const	PBoy;
	typedef DiGraphGirlNode* const	PGirl;
	typedef Edge*				PEdge;
//	typedef Reversible<PEdge>	RPEdge;
	typedef PEdge				RPEdge;

	Edge(CPSolver& s,PBoy pBoy, PGirl pGirl) :
		pBoy(pBoy),pGirl(pGirl),
	//	pPrevEdgeBoy(s,NULL),pNextEdgeBoy(s,NULL),
	//	pPrevEdgeGirl(s,NULL),pNextEdgeGirl(s,NULL) {}
		pPrevEdgeBoy(NULL),pNextEdgeBoy(NULL),
		pPrevEdgeGirl(NULL),pNextEdgeGirl(NULL) {}

	PBoy	pBoy;
	PGirl	pGirl;
	RPEdge	pPrevEdgeBoy;
	RPEdge	pNextEdgeBoy;
	RPEdge	pPrevEdgeGirl;
	RPEdge	pNextEdgeGirl;
};

struct DiGraphNode
{
	typedef	DiGraphNode					Self;
	typedef Self*						PNode;
	typedef	Detail::Edge				Edge;
	typedef Edge*						PEdge;
//	typedef Reversible<PNode>			RPNode;
//	typedef Reversible<PEdge>			RPEdge;
	typedef PNode			RPNode;
	typedef PEdge			RPEdge;

	struct EdgeIterator
	{
		EdgeIterator(PEdge pEdge) : pEdge(pEdge) {}
		EdgeIterator(const EdgeIterator& s)
		{this->pEdge = s.pEdge;}
		bool operator==(const EdgeIterator& s) const
		{ return pEdge == s.pEdge; }
		bool operator!=(const EdgeIterator& s) const
		{ return pEdge != s.pEdge; }
		Edge& operator*() const
		{ return *pEdge; }
		PEdge operator->() const
		{ return pEdge;  }
		PEdge get() const
		{ return pEdge;  }

		const EdgeIterator& operator=(const EdgeIterator& i)
		{ pEdge = i.pEdge;	return *this; }
		PEdge pEdge;
	};

	DiGraphNode(CPSolver& s, uint idx) :
		//edges(s,NULL),match(s,NULL), idx(idx) {}
		edges(NULL),match(NULL), idx(idx) {}

	bool inMatch() const
	{	return match != NULL;	}

	RPEdge		edges;
	RPEdge		match;
	const uint 	idx;
};

struct DiGraphGirlNode;

struct DiGraphBoyNode : DiGraphNode
{
	typedef	DiGraphNode					Super;
	typedef DiGraphBoyNode				Self;
	typedef Super::PNode				PNode;
	typedef Super::PEdge				PEdge;

	struct EdgeIterator : Super::EdgeIterator
	{
		EdgeIterator(PEdge pEdge) : Super::EdgeIterator(pEdge) {}
		EdgeIterator& operator++() // pre-increment
		{ pEdge = pEdge->pNextEdgeBoy; return *this; }
		EdgeIterator operator++(int) //post-increment
		{ EdgeIterator ret(*this); pEdge = pEdge->pNextEdgeBoy; return ret; }
	};

	DiGraphBoyNode(CPSolver& s, uint idx) :
		Super(s,idx) {}

	EdgeIterator begin() { return EdgeIterator(edges); }
	EdgeIterator end()   { return EdgeIterator(NULL); }

	DiGraphGirlNode* matchNode()
	{	return (DiGraphGirlNode*)match->pGirl;	}
};

struct DiGraphGirlNode : DiGraphNode
{
	typedef	DiGraphNode					Super;
	typedef DiGraphGirlNode				Self;
	typedef Super::PNode				PNode;
	typedef Super::PEdge				PEdge;

	struct EdgeIterator : Super::EdgeIterator
	{
		EdgeIterator(PEdge pEdge) : Super::EdgeIterator(pEdge) {}
		EdgeIterator& operator++() // pre-increment
		{ pEdge = pEdge->pNextEdgeGirl; return *this; }
		EdgeIterator operator++(int) //post-increment
		{ EdgeIterator ret(*this); pEdge = pEdge->pNextEdgeGirl; return ret; }
	};

	DiGraphGirlNode(CPSolver& s, uint idx) :
		Super(s,idx) {}

	EdgeIterator begin() { return EdgeIterator(edges); }
	EdgeIterator end()   { return EdgeIterator(NULL); }

	DiGraphBoyNode* matchNode()
	{	return (DiGraphBoyNode*)match->pBoy;	}
};

struct BiDiGraph
{
	typedef	BiDiGraph			Self;

	typedef DiGraphNode			Node;
	typedef Node*				PNode;

	typedef DiGraphNode::Edge	Edge;
	typedef DiGraphNode::PEdge	PEdge;

	typedef DiGraphNode::EdgeIterator	EdgeIterator;
	typedef DiGraphBoyNode		BoyNode;
	typedef BoyNode*			PBoyNode;

	typedef DiGraphGirlNode		GirlNode;
	typedef GirlNode*			PGirlNode;

	typedef Util::StdVector<PBoyNode>	BoyNodes;
	typedef BoyNodes::Iterator	BoyNodeIterator;

	typedef Util::StdVector<PGirlNode>	GirlNodes;
	typedef GirlNodes::Iterator	GirlNodeIterator;

	BiDiGraph(CPSolver& solver, uint nBoys, uint nGirls) :
		solver(solver),
		boys(solver.sHeap(),nBoys,NULL),
		girls(solver.sHeap(),nGirls,NULL),
		_nBoys(nBoys),
		_nGirls(nGirls),
		count(0),
		comp(solver.sHeap(),nBoys+nGirls,0)
	{
		for (uint i = 0; i < nBoys; i++)
			boys[i] = new (solver.sHeap()) BoyNode(solver,i);
		for (uint i = 0; i < nGirls; i++)
			girls[i] = new (solver.sHeap()) GirlNode(solver,i+nBoys);
	}

	void setBoyGirlEdge(PEdge pEdge)	// match:	O(1)
	{
		pEdge->pBoy->match = pEdge->pGirl->match = pEdge;
		pEdge->pNextEdgeBoy = pEdge->pPrevEdgeBoy =
				pEdge->pNextEdgeGirl = pEdge->pPrevEdgeGirl = NULL;
	}

	void setGirlBoyEdge(PEdge pEdge)	// not in match: O(1)
	{
		pEdge->pNextEdgeBoy = pEdge->pBoy->edges;
		pEdge->pNextEdgeGirl = pEdge->pGirl->edges;
		if (pEdge->pBoy->edges)
			pEdge->pBoy->edges->pPrevEdgeBoy = pEdge;
		if (pEdge->pGirl->edges)
			pEdge->pGirl->edges->pPrevEdgeGirl = pEdge;
		pEdge->pBoy->edges = pEdge;
		pEdge->pGirl->edges = pEdge;
	}

	void addEdgeDec(PGirlNode g, PBoyNode b)	// not in match: O(1)
	{
		PEdge e = new (solver.getHeap()) Edge(solver,b,g);
		setGirlBoyEdge(e);
	}

	void invertBoyGirlEdge(PEdge pEdge)	// match: O(1)
	{	unsetBoyGirlEdge(pEdge); setGirlBoyEdge(pEdge);	}
	void invertGirlBoyEdge(PEdge pEdge)	// not in match: O(1)
	{	removeEdge(pEdge); setBoyGirlEdge(pEdge);	}
	void invertEdge(EdgeIterator it)	// not in match: O(1)
	{	invertGirlBoyEdge(it.get());	}

	bool isBoyGirlEdge(PEdge e) const	// has match: O(1)
	{
		 assert(e->pBoy->match == e || e->pGirl->match != e);
		 return e->pBoy->match == e;
	}

	bool hasEdge(PGirlNode g,PBoyNode b) const	// not in match: O(N)
	{
		for (GirlNode::EdgeIterator it = g->begin(); it != g->end(); ++it)
			if (it->pBoy == b) return true;
		return false;
	}

	bool hasEdge(PBoyNode b,PGirlNode g) const	// not in match: O(N)
	{	return b->match == g->match && b->match != NULL;	}

	void unsetBoyGirlEdge(PEdge pEdge)	// in match: O(1)
	{	pEdge->pBoy->match = pEdge->pGirl->match =  NULL; 	}

	void unsetGirlBoyEdge(PEdge pEdge)	// not in match: O(1)
	{
		removeEdge(pEdge);
	}

	void removeEdge(PEdge pEdge)	// not in match: O(1)
	{
		if (pEdge->pNextEdgeBoy)
			pEdge->pNextEdgeBoy->pPrevEdgeBoy = pEdge->pPrevEdgeBoy;
		if (pEdge->pPrevEdgeBoy)
			pEdge->pPrevEdgeBoy->pNextEdgeBoy = pEdge->pNextEdgeBoy;
		else
			pEdge->pBoy->edges = pEdge->pNextEdgeBoy;
		if (pEdge->pNextEdgeGirl)
			pEdge->pNextEdgeGirl->pPrevEdgeGirl = pEdge->pPrevEdgeGirl;
		if (pEdge->pPrevEdgeGirl)
			pEdge->pPrevEdgeGirl->pNextEdgeGirl = pEdge->pNextEdgeGirl;
		else
			pEdge->pGirl->edges = pEdge->pNextEdgeGirl;
	}

	void removeAllEdges()
	{
		for (uint g = 0; g < nGirls(); g++)
		{
			girls[g]->match = NULL;
			girls[g]->edges = NULL;
		}
		for (uint b = 0; b < nBoys(); b++)
		{
			boys[b]->match = NULL;
			boys[b]->edges = NULL;
		}
	}

	uint nBoys() const	{ return _nBoys;	}
	uint nGirls() const	{ return _nGirls;	}
	uint nNodes() const	{ return nBoys()+nGirls(); }

	CPSolver&		solver;
	BoyNodes	boys;
	GirlNodes	girls;

	uint	_nBoys;
	uint	_nGirls;

	uint			count;
	Util::StdVector<uint>	comp;
};

struct MaximalMatching
{
	typedef	BiDiGraph::GirlNode		GirlNode;
	typedef	BiDiGraph::PGirlNode	PGirlNode;
	typedef	BiDiGraph::BoyNode		BoyNode;
	typedef	BiDiGraph::PBoyNode		PBoyNode;

	MaximalMatching(CPSolver& sol,BiDiGraph& net) :
							net(net) {}

	bool compute();
	uint size();

	bool inMatch(PBoyNode b, PGirlNode g) const	{	return b->match == g->match;	}

	bool searchMatch(PGirlNode	pGirl);
	bool match(PGirlNode	pGirl);

	protected:
	BiDiGraph&		net;
};

// Hopcroft-Karp algorithm - finds a maximal matching
bool MaximalMatching::compute()
{
	for (uint g = 0; g < net.nGirls(); g++)
		if (!match(net.girls[g]))
			return false;
    assert(size()==net.nGirls());
    return true;
}

// DFS for an augmenting path from the vertice
// currently at the top of the stack to SNK.
// (part of Hopcroft-Karp algorithm)
bool MaximalMatching::searchMatch(PGirlNode	pGirl)
{
	net.comp[pGirl->idx] = net.count;

    // Try to find matching edge cheaply: is there a free edge around?

	//assert(!pGirl->next.empty());
    for (GirlNode::EdgeIterator it = pGirl->begin();
			it != pGirl->end(); ++it)
    	if (it->pBoy->match == NULL)	// if boy is free
        {
        	net.invertEdge(it);
			return true;
        }

    // No, find matching edge by augmenting path method
    for (GirlNode::EdgeIterator it1 = pGirl->begin();
			it1 != pGirl->end(); ++it1)
	{
		BiDiGraph::PEdge pEdge2 = it1->pBoy->match;
		assert(pEdge2 != NULL);
		if(net.comp[pEdge2->pGirl->idx] < net.count && searchMatch(pEdge2->pGirl))
		{
			net.setGirlBoyEdge(pEdge2); // revert (boy->girl)
			net.invertEdge(it1); 	// revert (girl->boy)
          	return true;
        }
	}
    return false;
}

bool MaximalMatching::match(PGirlNode pGirl)
{
    net.count = net.count + 1;
    return searchMatch(pGirl);
}

// public
uint MaximalMatching::size()
{
	uint s = 0;
	for (uint g = 0; g < net.nGirls(); g++)
		if (net.girls[g]->inMatch())
			s++;
	return s;
}

struct AllDiffHelper
{
	typedef	BiDiGraph::PNode		PNode;
	typedef	BiDiGraph::GirlNode		GirlNode;
	typedef	BiDiGraph::PGirlNode	PGirlNode;
	typedef	BiDiGraph::BoyNode		BoyNode;
	typedef	BiDiGraph::PBoyNode		PBoyNode;

	void reset();
	void markDirectPaths();
	void markSCC();
	int marked(PNode b, PNode g) const
	{
		return 	 rindex[b->idx] == rindex[g->idx] ||
			 	 (net.comp[b->idx]==net.count && net.comp[g->idx]==net.count);
	}

	AllDiffHelper(CPSolver& sol,BiDiGraph& net) :
			net(net),
			d(sol.getHeap(),net.nBoys(),net.nGirls(),0),
			toVisit(sol.getHeap(),net.nBoys()+net.nGirls(),0),
			s(sol.getHeap(),net.nBoys()*net.nGirls(),0),
			rindex(sol.getHeap(),net.nNodes(),0),
			pre(sol.getHeap(),net.nNodes(),0),
			low(sol.getHeap(),net.nNodes(),0),
			cnt1(0),
			cnt0(0)
		{}

	//private:
	void visitSCCGirl(PGirlNode);
	void visitSCCBoy(PBoyNode);
	bool usedEdge(PNode n1, PNode n2) const
	{
		if (n1->idx < n2->idx)
			return d[n1->idx][n2->idx-net.nBoys()];
		else
			return d[n2->idx][n1->idx-net.nBoys()];
	}
	void markEdgeAsUsed(PNode n1, PNode n2)
	{
		if (n1->idx < n2->idx)
			d[n1->idx][n2->idx-net.nBoys()] = 1;
		else
			d[n2->idx][n1->idx-net.nBoys()] = 1;
	}

	void	scc(PNode p);

	void	debug() const
	{
		for (uint b = 0; b <net.nBoys(); b++)
		{
			for (uint g = 0; g <net.nGirls(); g++)
				if (net.hasEdge(net.boys[b],net.girls[g]))
					if (marked(net.boys[b],net.girls[g]))
						std::cout << "2 ";
					else
						std::cout << "1 ";
				else
				if (net.hasEdge(net.girls[g],net.boys[b]))
					if (marked(net.girls[g],net.boys[b]))
						std::cout << "-2 ";
					else
						std::cout << "-1 ";
				else
					std::cout << "0 ";
			std::cout << endl;
		}
		std::cout << endl;
	}

	BiDiGraph&		net;
	Util::StdArray<int,2> 	d;			//  where to mark directed paths and
								//  strongly connected components
	Util::StdVector<PBoyNode> 	toVisit;	//	aux vector used in markDirectPaths
	Util::StdVector<PNode> 	s;			// 	global stack for scc
	int 			sSize;		// 	stack size (for efficiency)
	uint			c;			//  aux counter for SCC
	uint			index;		//  aux counter for SCC
	Util::StdVector<int> 	rindex;		//  aux array to color SCC

	Util::StdVector<uint>	pre;
	Util::StdVector<uint>	low;
	uint			cnt1;
	uint			cnt0;
	uint			toVisitSize;
};

void AllDiffHelper::reset()
{
}

// toVisit is a limited size stack, implemented with a vector for efficiency
void AllDiffHelper::markDirectPaths()
{
	toVisitSize = 0;
	PBoyNode v;
	net.count = net.count + 1;

	// populate toVisit with free vertices
	// (which must be boys since we have a maximal matching)
	for (uint b = 0; b < net.nBoys(); b++)
		if (!net.boys[b]->inMatch())
		{
			toVisit[toVisitSize++] = net.boys[b];
			net.comp[net.boys[b]->idx] = net.count;
		}

	// depth-first search for direct paths
	while (toVisitSize > 0)
	{
		v = toVisit[--toVisitSize];

		for (BoyNode::EdgeIterator itg = v->begin();
				itg != v->end(); ++itg)
			if (net.comp[itg->pGirl->idx] < net.count)
			{
				net.comp[itg->pGirl->idx] = net.count;
				PBoyNode pb = itg->pGirl->matchNode();
				assert(pb != NULL);
				if (net.comp[pb->idx] < net.count)
				{
					net.comp[pb->idx] = net.count;
					toVisit[toVisitSize++] = pb;
				}
			}
	}
}

void AllDiffHelper::visitSCCGirl(PGirlNode v)
{
	bool root = true;
	rindex[v->idx] = index; index++;
	PNode w;

	for (GirlNode::EdgeIterator  it = v->begin(); it != v->end(); ++it)
		if (net.comp[it->pBoy->idx] < net.count)
		{
			if (rindex[it->pBoy->idx] == 0)
				visitSCCBoy(it->pBoy);
			if (rindex[it->pBoy->idx] < rindex[v->idx])
			{	rindex[v->idx] = rindex[it->pBoy->idx]; root = false;	}
		}

	if (root)
	{
		--index;
		while (sSize > 0 and rindex[v->idx] <= rindex[s[sSize-1]->idx])
		{
			w = s[--sSize];
			rindex[w->idx] = c;
			--index;
		}
		rindex[v->idx] = c;
		--c;
	}
	else
		s[sSize++] = v;
}

void AllDiffHelper::visitSCCBoy(PBoyNode v)
{
	bool root = true;
	rindex[v->idx] = index; index++;
	PNode w;

	PGirlNode g = v->matchNode();
	//GirlNode* g = (GirlNode*)v->next;
	if (g && net.comp[g->idx] < net.count)
	{
		if (rindex[g->idx] == 0)
			visitSCCGirl(g);
		if (rindex[g->idx] < rindex[v->idx])
		{	rindex[v->idx] = rindex[g->idx]; root = false;	}
	}

	if (root)
	{
		--index;
		while (sSize > 0 and rindex[v->idx] <= rindex[s[sSize-1]->idx])
		{
			w = s[--sSize];
			rindex[w->idx] = c;
			--index;
		}
		rindex[v->idx] = c;
		--c;
	}
	else
		s[sSize++] = v;
}

// find strongly connected components of a directed graph g
void AllDiffHelper::markSCC()
{
	sSize = 0;
	index = 1;
	c = net.nNodes()-1;

	// zero array
	rindex = 0;

	// only need visit girls because any SCC has to go through a girl
	for (BiDiGraph::GirlNodeIterator it = net.girls.begin();
			it != net.girls.end(); ++it)
		if (net.comp[(*it)->idx] < net.count)
	{
		// optimization: only visit girls which have at least one flow out
		//if (rindex[getIdx(*it)] == 0 && !((GirlNode*)(*it))->next.empty())
		if (rindex[(*it)->idx] == 0 && (*it)->inMatch())
			visitSCCGirl(*it);
	}

}

#endif //USE_CASPER_DOM_ALLDIFF_V2

#ifdef USE_CASPER_DOM_ALLDIFF_V3

struct DiGraphBoyNode;
struct DiGraphGirlNode;

struct Edge
{
	typedef DiGraphBoyNode*	const	PBoy;
	typedef DiGraphGirlNode* const	PGirl;
	typedef Edge*				PEdge;
	typedef PEdge				RPEdge;

	Edge(CPSolver& s,PBoy pBoy, PGirl pGirl) :
		pBoy(pBoy),pGirl(pGirl),
		pPrevEdgeBoy(NULL),pNextEdgeBoy(NULL),
		pPrevEdgeGirl(NULL),pNextEdgeGirl(NULL),
		pPrevSortedEdgeGirl(NULL),pNextSortedEdgeGirl(NULL) {}

	PBoy	pBoy;
	PGirl	pGirl;
	RPEdge	pPrevEdgeBoy;
	RPEdge	pNextEdgeBoy;
	RPEdge	pPrevEdgeGirl;
	RPEdge	pNextEdgeGirl;
	RPEdge	pPrevSortedEdgeGirl;
	RPEdge	pNextSortedEdgeGirl;
};

struct DiGraphNode
{
	typedef	DiGraphNode					Self;
	typedef Self*						PNode;
	typedef	Detail::Edge				Edge;
	typedef Edge*						PEdge;
	typedef PNode			RPNode;
	typedef PEdge			RPEdge;

	struct EdgeIterator
	{
		EdgeIterator(PEdge pEdge) : pEdge(pEdge) {}
		EdgeIterator(const EdgeIterator& s)
		{this->pEdge = s.pEdge;}
		bool operator==(const EdgeIterator& s) const
		{ return pEdge == s.pEdge; }
		bool operator!=(const EdgeIterator& s) const
		{ return pEdge != s.pEdge; }
		Edge& operator*() const
		{ return *pEdge; }
		PEdge operator->() const
		{ return pEdge;  }
		PEdge get() const
		{ return pEdge;  }

		const EdgeIterator& operator=(const EdgeIterator& i)
		{ pEdge = i.pEdge;	return *this; }
		PEdge pEdge;
	};

	DiGraphNode(CPSolver& s, uint idx) :
		edges(NULL),match(NULL), idx(idx) {}

	bool inMatch() const
	{	return match != NULL;	}

	RPEdge		edges;
	RPEdge		match;
	const uint 	idx;
};

struct DiGraphGirlNode;

struct DiGraphBoyNode : DiGraphNode
{
	typedef	DiGraphNode					Super;
	typedef DiGraphBoyNode				Self;
	typedef Super::PNode				PNode;
	typedef Super::PEdge				PEdge;

	struct EdgeIterator : Super::EdgeIterator
	{
		EdgeIterator(PEdge pEdge) : Super::EdgeIterator(pEdge) {}
		EdgeIterator& operator++() // pre-increment
		{ pEdge = pEdge->pNextEdgeBoy; return *this; }
		EdgeIterator operator++(int) //post-increment
		{ EdgeIterator ret(*this); pEdge = pEdge->pNextEdgeBoy; return ret; }
	};

	DiGraphBoyNode(CPSolver& s, uint idx) :
		Super(s,idx) {}

	EdgeIterator begin() { return EdgeIterator(edges); }
	EdgeIterator end()   { return EdgeIterator(NULL); }

	DiGraphGirlNode* matchNode()
	{	return (DiGraphGirlNode*)match->pGirl;	}
};

struct DiGraphGirlNode : DiGraphNode
{
	typedef	DiGraphNode					Super;
	typedef DiGraphGirlNode				Self;
	typedef Super::PNode				PNode;
	typedef Super::PEdge				PEdge;

	struct EdgeIterator : Super::EdgeIterator
	{
		EdgeIterator(PEdge pEdge) : Super::EdgeIterator(pEdge) {}
		EdgeIterator& operator++() // pre-increment
		{ pEdge = pEdge->pNextEdgeGirl; return *this; }
		EdgeIterator operator++(int) //post-increment
		{ EdgeIterator ret(*this); pEdge = pEdge->pNextEdgeGirl; return ret; }
	};

	struct SortedEdgeIterator : Super::EdgeIterator
	{
		SortedEdgeIterator(PEdge pEdge) : Super::EdgeIterator(pEdge) {}
		SortedEdgeIterator& operator++() // pre-increment
		{ pEdge = pEdge->pNextSortedEdgeGirl; return *this; }
		SortedEdgeIterator operator++(int) //post-increment
		{ SortedEdgeIterator ret(*this); pEdge = pEdge->pNextSortedEdgeGirl; return ret; }
	};

	DiGraphGirlNode(CPSolver& s, uint idx) :
		Super(s,idx),sortedEdges(NULL) {}

	EdgeIterator begin() { return EdgeIterator(edges); }
	EdgeIterator end()   { return EdgeIterator(NULL); }

	DiGraphBoyNode* matchNode()
	{	return (DiGraphBoyNode*)match->pBoy;	}

	PEdge	sortedEdges;
	SortedEdgeIterator sortedBegin() { return SortedEdgeIterator(sortedEdges); }
	SortedEdgeIterator sortedEnd()   { return SortedEdgeIterator(NULL); }

};

struct BiDiGraph
{
	typedef	BiDiGraph			Self;

	typedef DiGraphNode			Node;
	typedef Node*				PNode;

	typedef DiGraphNode::Edge	Edge;
	typedef DiGraphNode::PEdge	PEdge;

	typedef DiGraphNode::EdgeIterator	EdgeIterator;
	typedef DiGraphBoyNode		BoyNode;
	typedef BoyNode*			PBoyNode;

	typedef DiGraphGirlNode		GirlNode;
	typedef GirlNode*			PGirlNode;

	typedef Util::StdVector<PBoyNode>	BoyNodes;
	typedef BoyNodes::Iterator	BoyNodeIterator;

	typedef Util::StdVector<PGirlNode>	GirlNodes;
	typedef GirlNodes::Iterator	GirlNodeIterator;

	BiDiGraph(CPSolver& solver, uint nBoys, uint nGirls) :
		solver(solver),
		boys(solver.sHeap(),nBoys,NULL),
		girls(solver.sHeap(),nGirls,NULL),
		_nBoys(nBoys),
		_nGirls(nGirls),
		count(0),
		comp(solver.sHeap(),nBoys+nGirls,0)
	{
		for (uint i = 0; i < nBoys; i++)
			boys[i] = new (solver.sHeap()) BoyNode(solver,i);
		for (uint i = 0; i < nGirls; i++)
			girls[i] = new (solver.sHeap()) GirlNode(solver,i+nBoys);
	}

	void setBoyGirlEdge(PEdge pEdge)	// match:	O(1)
	{	pEdge->pBoy->match = pEdge->pGirl->match = pEdge;
		pEdge->pNextEdgeBoy = pEdge->pPrevEdgeBoy
			= pEdge->pNextEdgeGirl = pEdge->pPrevEdgeGirl = NULL;
	}

	void setGirlBoyEdge(PEdge pEdge)	// not in match: O(1)
	{
		pEdge->pNextEdgeBoy = pEdge->pBoy->edges;
		pEdge->pNextEdgeGirl = pEdge->pGirl->edges;
		if (pEdge->pBoy->edges)
			pEdge->pBoy->edges->pPrevEdgeBoy = pEdge;
		if (pEdge->pGirl->edges)
			pEdge->pGirl->edges->pPrevEdgeGirl = pEdge;
		pEdge->pBoy->edges = pEdge;
		pEdge->pGirl->edges = pEdge;
	}

	void addEdgeDec(PGirlNode g, PBoyNode b)	// not in match: O(1)
	{
		PEdge e = new (solver.sHeap()) Edge(solver,b,g);
		setGirlBoyEdge(e);
		g->edges->pNextSortedEdgeGirl = g->sortedEdges;
		if (g->sortedEdges)
			g->sortedEdges->pPrevSortedEdgeGirl = g->edges;
		g->sortedEdges = g->edges;
	}

	void invertBoyGirlEdge(PEdge pEdge)	// match: O(1)
	{	unsetBoyGirlEdge(pEdge); setGirlBoyEdge(pEdge);	}
	void invertGirlBoyEdge(PEdge pEdge)	// not in match: O(1)
	{	removeEdgeKeepSorted(pEdge); setBoyGirlEdge(pEdge);	}
	void invertEdge(EdgeIterator it)	// not in match: O(1)
	{	invertGirlBoyEdge(it.get());	}

	bool isBoyGirlEdge(PEdge e) const	// has match: O(1)
	{	assert(e->pBoy->match == e || e->pGirl->match != e);
		 return e->pBoy->match == e;	}

//	bool isGirlBoyEdge(PEdge e) const	// has match: O(1)
//	{	return !isBoyGirlEdge() && e->pGirl;	}

	bool hasEdge(PGirlNode g,PBoyNode b) const	// not in match: O(N)
	{
		for (GirlNode::EdgeIterator it = g->begin(); it != g->end(); ++it)
			if (it->pBoy == b) return true;
		return false;
	}

	bool hasEdge(PBoyNode b,PGirlNode g) const	// not in match: O(N)
	{	return b->match == g->match && b->match != NULL;	}

	bool hasSortedEdge(PGirlNode g,PBoyNode b) const	// not in match: O(N)
	{
		for (GirlNode::SortedEdgeIterator it = g->sortedBegin();
				it != g->sortedEnd(); ++it)
			if (it->pBoy == b) return true;
		return false;
	}

	bool hasSortedEdge(PEdge pEdge) const	// not in match: O(N)
	{
		for (GirlNode::SortedEdgeIterator it = pEdge->pGirl->sortedBegin();
				it != pEdge->pGirl->sortedEnd(); ++it)
			if (pEdge == it.get()) return true;
		return false;
	}

	bool hasSortedEdge(EdgeIterator it) const
	{	return hasSortedEdge(it.get());	}

	void unsetBoyGirlEdge(PEdge pEdge)	// in match: O(1)
	{	pEdge->pBoy->match = pEdge->pGirl->match =  NULL; 	}

	void unsetGirlBoyEdge(PEdge pEdge)	// not in match: O(1)
	{
		removeEdgeFromSorted(pEdge);
		removeEdgeKeepSorted(pEdge);
	}

	void removeEdgeFromSorted(PEdge pEdge)	// not in match: O(1)
	{
		if (pEdge->pNextSortedEdgeGirl)
			pEdge->pNextSortedEdgeGirl->pPrevSortedEdgeGirl = pEdge->pPrevSortedEdgeGirl;
		if (pEdge->pPrevSortedEdgeGirl)
			pEdge->pPrevSortedEdgeGirl->pNextSortedEdgeGirl = pEdge->pNextSortedEdgeGirl;
		else
			pEdge->pGirl->sortedEdges = pEdge->pNextSortedEdgeGirl;
	}

	void removeEdgeKeepSorted(PEdge pEdge)	// not in match: O(1)
	{
		if (pEdge->pNextEdgeBoy)
			pEdge->pNextEdgeBoy->pPrevEdgeBoy = pEdge->pPrevEdgeBoy;
		if (pEdge->pPrevEdgeBoy)
			pEdge->pPrevEdgeBoy->pNextEdgeBoy = pEdge->pNextEdgeBoy;
		else
			pEdge->pBoy->edges = pEdge->pNextEdgeBoy;
		if (pEdge->pNextEdgeGirl)
			pEdge->pNextEdgeGirl->pPrevEdgeGirl = pEdge->pPrevEdgeGirl;
		if (pEdge->pPrevEdgeGirl)
			pEdge->pPrevEdgeGirl->pNextEdgeGirl = pEdge->pNextEdgeGirl;
		else
			pEdge->pGirl->edges = pEdge->pNextEdgeGirl;
	}

	void removeAllEdges()
	{
		for (uint g = 0; g < nGirls(); g++)
		{
			girls[g]->match = NULL;
			girls[g]->edges = NULL;
			girls[g]->sortedEdges = NULL;
		}
		for (uint b = 0; b < nBoys(); b++)
		{
			boys[b]->match = NULL;
			boys[b]->edges = NULL;
		}
	}

	// only for debugging
	void assertConsistency()
	{
		for (uint g = 0; g < nGirls(); g++)
		{
			for (GirlNode::EdgeIterator it = girls[g]->begin();
				it != girls[g]->end(); ++it)
			{
				assert(it->pGirl == girls[g]);
				assert(hasSortedEdge(it)); // all girl->boy edges are in sorted lists
			}
			if (girls[g]->inMatch())
				assert(hasSortedEdge(girls[g],girls[g]->matchNode()));
		}
		for (uint b = 0; b < nBoys(); b++)
		{
			for (BoyNode::EdgeIterator it = boys[b]->begin();
				it != boys[b]->end(); ++it)
				assert(it->pBoy == boys[b]);
			if (boys[b]->inMatch())
				assert(hasSortedEdge(boys[b]->matchNode(),boys[b]));
		}
		// test sorted lists: all sorted girl->boy edges exist, or are in match
		for (uint g = 0; g < nGirls(); g++)
			for (GirlNode::SortedEdgeIterator it = girls[g]->sortedBegin();
				it != girls[g]->sortedEnd(); ++it)
			{
				assert(it->pGirl == girls[g]);
				//assert(hasGirlBoyEdge(it) || hasBoyGirlEdge(it));
			}
	}

	void printSortedBoys(PGirlNode pGirl)
	{
		for (GirlNode::SortedEdgeIterator it = pGirl->sortedBegin();
			it != pGirl->sortedEnd(); ++it)
			std::cout << it->pBoy->idx << " ";
		std::cout << std::endl;
	}

	uint nBoys() const	{ return _nBoys;	}
	uint nGirls() const	{ return _nGirls;	}
	uint nNodes() const	{ return nBoys()+nGirls(); }

	CPSolver&		solver;
	BoyNodes	boys;
	GirlNodes	girls;

	uint	_nBoys;
	uint	_nGirls;

	uint			count;
	Util::StdVector<uint>	comp;

};

struct MaximalMatching
{
	typedef	BiDiGraph::GirlNode		GirlNode;
	typedef	BiDiGraph::PGirlNode	PGirlNode;
	typedef	BiDiGraph::BoyNode		BoyNode;
	typedef	BiDiGraph::PBoyNode		PBoyNode;

	MaximalMatching(CPSolver& sol,BiDiGraph& net) :
							net(net) {}

	bool compute();
	uint size();

	bool inMatch(PBoyNode b, PGirlNode g) const	{	return b->inMatch();	}

	bool searchMatch(PGirlNode	pGirl);
	bool match(PGirlNode	pGirl);

	protected:
	BiDiGraph&		net;
};

// Hopcroft-Karp algorithm - finds a maximal matching
bool MaximalMatching::compute()
{
	for (uint g = 0; g < net.nGirls(); g++)
		if (!match(net.girls[g]))
			return false;
    assert(size()==net.nGirls());
    return true;
}

// DFS for an augmenting path from the vertice
// currently at the top of the stack to SNK.
// (part of Hopcroft-Karp algorithm)
bool MaximalMatching::searchMatch(PGirlNode	pGirl)
{
	net.comp[pGirl->idx] = net.count;

    // Try to find matching edge cheaply: is there a free edge around?

	//assert(!pGirl->next.empty());
    for (GirlNode::EdgeIterator it = pGirl->begin();
			it != pGirl->end(); ++it)
    	if (it->pBoy->match == NULL)	// if boy is free
        {
        	net.invertEdge(it);
			return true;
        }

    // No, find matching edge by augmenting path method
    for (GirlNode::EdgeIterator it1 = pGirl->begin();
			it1 != pGirl->end(); ++it1)
	{
		BiDiGraph::PEdge pEdge2 = it1->pBoy->match;
		assert(pEdge2 != NULL);
		//GirlNode*	pGNode = (GirlNode*) ((BoyNode*)(*it1))->next;
		if(net.comp[pEdge2->pGirl->idx] < net.count && searchMatch(pEdge2->pGirl))
		{
			net.setGirlBoyEdge(pEdge2);
			net.invertEdge(it1); 	// revert (girl->boy)
          	return true;
        }
	}
    return false;
}

bool MaximalMatching::match(PGirlNode pGirl)
{
    net.count = net.count + 1;
    return searchMatch(pGirl);
}

// public
uint MaximalMatching::size()
{
	uint s = 0;
	for (uint g = 0; g < net.nGirls(); g++)
		if (net.girls[g]->inMatch())
			s++;
	return s;
}

struct AllDiffHelper
{
	typedef	BiDiGraph::PNode		PNode;
	typedef	BiDiGraph::GirlNode		GirlNode;
	typedef	BiDiGraph::PGirlNode	PGirlNode;
	typedef	BiDiGraph::BoyNode		BoyNode;
	typedef	BiDiGraph::PBoyNode		PBoyNode;

	void reset();
	void markDirectPaths();
	void markSCC();
	int marked(PNode b, PNode g) const
	{
		return rindex[b->idx] == rindex[g->idx] ||
			  (net.comp[b->idx]==net.count && net.comp[g->idx]==net.count);
	}

	AllDiffHelper(CPSolver& sol,BiDiGraph& net) :
			net(net),
			d(sol.getHeap(),net.nBoys(),net.nGirls(),0),
			toVisit(sol.getHeap(),net.nBoys()+net.nGirls(),0),
			s(sol.getHeap(),net.nBoys()*net.nGirls(),0),
			rindex(sol.getHeap(),net.nNodes(),0),
			pre(sol.getHeap(),net.nNodes(),0),
			low(sol.getHeap(),net.nNodes(),0),
			cnt1(0),
			cnt0(0)
		{}

	//private:
	void visitSCCGirl(PGirlNode);
	void visitSCCBoy(PBoyNode);
	bool usedEdge(PNode n1, PNode n2) const
	{
		if (n1->idx < n2->idx)
			return d[n1->idx][n2->idx-net.nBoys()];
		else
			return d[n2->idx][n1->idx-net.nBoys()];
	}
	void markEdgeAsUsed(PNode n1, PNode n2)
	{
		if (n1->idx < n2->idx)
			d[n1->idx][n2->idx-net.nBoys()] = 1;
		else
			d[n2->idx][n1->idx-net.nBoys()] = 1;
	}

	void	scc(PNode p);

	void	debug() const
	{
		for (uint b = 0; b <net.nBoys(); b++)
		{
			for (uint g = 0; g <net.nGirls(); g++)
				if (net.hasEdge(net.boys[b],net.girls[g]))
					if (marked(net.boys[b],net.girls[g]))
						std::cout << "2 ";
					else
						std::cout << "1 ";
				else
				if (net.hasEdge(net.girls[g],net.boys[b]))
					if (marked(net.girls[g],net.boys[b]))
						std::cout << "-2 ";
					else
						std::cout << "-1 ";
				else
					std::cout << "0 ";
			std::cout << endl;
		}
		std::cout << endl;
	}

	void	debugSorted() const
	{
		for (uint b = 0; b <net.nBoys(); b++)
		{
			for (uint g = 0; g <net.nGirls(); g++)
				if (net.hasEdge(net.boys[b],net.girls[g]))
					if (marked(net.boys[b],net.girls[g]))
						std::cout << "2 ";
					else
						std::cout << "1 ";
				else
				if (net.hasSortedEdge(net.girls[g],net.boys[b]))
					if (marked(net.girls[g],net.boys[b]))
						std::cout << "-2 ";
					else
						std::cout << "-1 ";
				else
					std::cout << "0 ";
			std::cout << endl;
		}
		std::cout << endl;
	}

	BiDiGraph&		net;
	Util::StdArray<int,2> 	d;			//  where to mark directed paths and
								//  strongly connected components
	Util::StdVector<PBoyNode> 	toVisit;	//	aux vector used in markDirectPaths
	Util::StdVector<PNode> 	s;			// 	global stack for scc
	int 			sSize;		// 	stack size (for efficiency)
	uint			c;			//  aux counter for SCC
	uint			index;		//  aux counter for SCC
	Util::StdVector<int> 	rindex;		//  aux array to color SCC

	Util::StdVector<uint>	pre;
	Util::StdVector<uint>	low;
	uint			cnt1;
	uint			cnt0;
	uint			toVisitSize;
};

void AllDiffHelper::reset()
{
}

// toVisit is a limited size stack, implemented with a vector for efficiency
void AllDiffHelper::markDirectPaths()
{
	toVisitSize = 0;
	PBoyNode v;
	net.count = net.count + 1;

	// populate toVisit with free vertices
	// (which must be boys since we have a maximal matching)
	for (uint b = 0; b < net.nBoys(); b++)
		if (!net.boys[b]->inMatch())
		{
			toVisit[toVisitSize++] = net.boys[b];
			net.comp[net.boys[b]->idx] = net.count;
		}

	// depth-first search for direct paths
	while (toVisitSize > 0)
	{
		v = toVisit[--toVisitSize];

		for (BoyNode::EdgeIterator itg = v->begin();
				itg != v->end(); ++itg)
			if (net.comp[itg->pGirl->idx] < net.count)
			{
				net.comp[itg->pGirl->idx] = net.count;
				PBoyNode pb = itg->pGirl->matchNode();
				assert(pb != NULL);
				if (net.comp[pb->idx] < net.count)
				{
					net.comp[pb->idx] = net.count;
					toVisit[toVisitSize++] = pb;
				}
			}
	}
}

void AllDiffHelper::visitSCCGirl(PGirlNode v)
{
	bool root = true;
	rindex[v->idx] = index; index++;
	PNode w;

	for (GirlNode::EdgeIterator  it = v->begin(); it != v->end(); ++it)
		if (net.comp[it->pBoy->idx] < net.count)
		{
			if (rindex[it->pBoy->idx] == 0)
				visitSCCBoy(it->pBoy);
			if (rindex[it->pBoy->idx] < rindex[v->idx])
			{	rindex[v->idx] = rindex[it->pBoy->idx]; root = false;	}
		}

	if (root)
	{
		--index;
		while (sSize > 0 and rindex[v->idx] <= rindex[s[sSize-1]->idx])
		{
			w = s[--sSize];
			rindex[w->idx] = c;
			--index;
		}
		rindex[v->idx] = c;
		--c;
	}
	else
		s[sSize++] = v;
}

void AllDiffHelper::visitSCCBoy(PBoyNode v)
{
	bool root = true;
	rindex[v->idx] = index; index++;
	PNode w;

	PGirlNode g = v->matchNode();
	//GirlNode* g = (GirlNode*)v->next;
	if (g && net.comp[g->idx] < net.count)
	{
		if (rindex[g->idx] == 0)
			visitSCCGirl(g);
		if (rindex[g->idx] < rindex[v->idx])
		{	rindex[v->idx] = rindex[g->idx]; root = false;	}
	}

	if (root)
	{
		--index;
		while (sSize > 0 and rindex[v->idx] <= rindex[s[sSize-1]->idx])
		{
			w = s[--sSize];
			rindex[w->idx] = c;
			--index;
		}
		rindex[v->idx] = c;
		--c;
	}
	else
		s[sSize++] = v;
}

// find strongly connected components of a directed graph g
void AllDiffHelper::markSCC()
{
	sSize = 0;
	index = 1;
	c = net.nNodes()-1;

	// zero array
	rindex = 0;

	// only need visit girls because any SCC has to go through a girl
	for (BiDiGraph::GirlNodeIterator it = net.girls.begin();
			it != net.girls.end(); ++it)
		if (net.comp[(*it)->idx] < net.count)
	{
		// optimization: only visit girls which have at least one flow out
		//if (rindex[getIdx(*it)] == 0 && !((GirlNode*)(*it))->next.empty())
		if (rindex[(*it)->idx] == 0 && (*it)->inMatch())
			visitSCCGirl(*it);
	}

}

#endif //USE_CASPER_DOM_ALLDIFF_V3

};
};

#endif /*_H_CASPER_KERNEL_GRAPH*/
