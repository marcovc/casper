/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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

#include <casper/cp/int/distinct/gac.h>
#include <casper/cp/int/distinct/graph.h>


namespace Casper {
namespace CP {
namespace Detail {

#ifdef USE_CASPER_DOM_ALLDIFF_V3

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

#endif

}
}
}
