 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_INT_NEGATIVE_H_
#define CASPER_INT_NEGATIVE_H_

#include <casper/int/filter.h>
#include <casper/kernel/container/rlist.h>
#include <casper/kernel/container/rstack.h>
#include <casper/kernel/container/trie.h>

namespace casper {

#define GACSCHEMANEG_WL

#ifdef GACSCHEMANEG_WL

template<class T, class View>
struct GACSchemaNeg : IFilter
{
	typedef	DomArrayView<T,View>			Doms;
	typedef typename Doms::Dom::Iterator	DomIterator;

	struct WL {
		WL(UInt row,UInt col,Bool first) : row(row),col(col),first(first) {}
		UInt row;
		UInt col;
		Bool first;
	};
	typedef detail::List<WL*>			WLs;
	typedef typename WLs::Iterator		WLsIterator;

	GACSchemaNeg(const View& vars, const Array<T,2>& ss);

	bool execute();
	bool firstExecute();

	void attach(INotifiable* s) { erased.attach(s); }
	void detach(INotifiable* s) { erased.detach(s); }
	Cost cost() const {	return exponentialLo;}
	//void printSupports();
	private:

	Array<T,2>							ss;
	DomArrayView<T,View>				doms;
	Array<detail::Pair<WL> >			wlsRow;
	Array<WLs>							wlsCol;
	Reversible<Bool>					firstExecution;
	detail::IdxGroundDeltaStack<View>	erased;
};

template<class T, class View>
GACSchemaNeg<T,View>::GACSchemaNeg(const View& vars, const Array<T,2>& ss) :
	IFilter(vars.solver()), ss(ss),doms(vars.solver(),vars),
	wlsRow(stdHeap,ss.size()),
	wlsCol(stdHeap,doms.size(),stdHeap),
	firstExecution(vars.solver(),true),
	erased(vars)
{
}

template<class T, class View>
Bool GACSchemaNeg<T,View>::firstExecute()
{
	// clear data from previous executions (may happen on bt)
	for (UInt j = 0 ; j < doms.size(); j++)
		wlsCol[j].clear();

	// create watch literal structure
	for (UInt i = 0; i < ss.size(); i++)
	{
		UInt j;
		UInt wlc = 0;

		// first: check for entailness
		for (j = 0 ; j < doms.size(); j++)
			if (doms[j]->find(ss[i][j]) == doms[j]->end())
				goto nextTuple;

		// find initial supports
		for (j = 0 ; j < doms.size() and wlc < 2; j++)
			if (!doms[j]->ground())
			{
				if (wlc == 0)
				{
					wlsRow[i].first = WL(i,j,true);
					wlsCol[j].pushFront(&wlsRow[i].first);
				}
				else
				{
					wlsRow[i].second = WL(i,j,false);
					wlsCol[j].pushFront(&wlsRow[i].second);
				}
				++wlc;
			}

		if (wlc == 0)
			return false;
		else
		if (wlc == 1 and
			!doms[wlsRow[i].first.col]->erase(ss[i][wlsRow[i].first.col]))
			return false;

		nextTuple:;
	}
	return true;
}

template<class T, class View>
Bool GACSchemaNeg<T,View>::execute()
{
	if (firstExecution)
	{
		firstExecution = false;
		return firstExecute();
	}

	while (!erased.empty())
	{
		Int erasedVar = erased.top();
		erased.pop();
		for (WLsIterator it = wlsCol[erasedVar].begin();
				it != wlsCol[erasedVar].end();  )
			if (ss[(*it)->row][(*it)->col] == doms[(*it)->col]->value())
			{
				// seek next literal to watch
				UInt oldCol = (*it)->col;
				UInt otherCol = ((*it)->first)?(wlsRow[(*it)->row].second.col):
											   (wlsRow[(*it)->row].first.col);
				for ((*it)->col = ((*it)->col + 1) % doms.size();
						(*it)->col != oldCol;
						(*it)->col = ((*it)->col + 1) % doms.size() )
					if ((*it)->col != otherCol and
					     (!doms[(*it)->col]->ground() or
					       doms[(*it)->col]->value() !=	 ss[(*it)->row][(*it)->col])
						)
						break;

				// if no support found
				if ((*it)->col == oldCol)
				{
					if (!doms[otherCol]->erase(ss[(*it)->row][otherCol]))
						return false;
					++it;
				}
				else
				{
					// else update watch structure
					wlsCol[(*it)->col].pushFront(*it);
					wlsCol[oldCol].erase(it++);
				}
			}
			else
				++it;
	}
	return true;
}

template<class View>
Filter gacSchemaNeg(const View& v,
				 	const Array<Int,2>& ss)
{
	return new (v.solver().heap())
		GACSchemaNeg<Int,View>(v,ss);
}


#else
template<class T, class View>
struct GACSchemaNeg : IFilter
{
	typedef	DomArrayView<T,View>			Doms;
	typedef typename Doms::Dom::Iterator	DomIterator;

	typedef typename detail::Trie<T>::PNode		TriePNode;
	struct Support {
		Support() : pNode(NULL),curLev(0) {}
		Support(TriePNode pNode,UInt curLev) :
			pNode(pNode),curLev(curLev) {}
		TriePNode	pNode;
		UInt		curLev;
	};
	typedef detail::RList<Support>			Supports;
	typedef typename Supports::Iterator		SupportIterator;

	GACSchemaNeg(const View& vars, const Array<T,2>& ss);

	UInt getVarIdx(UInt trieIdx,UInt level) const;

	bool execute();
	bool firstExecute();

	void attach(INotifiable* s);
	void detach(INotifiable* s) {}
	Cost cost() const {	return exponentialLo;}
	//void printSupports();
	private:
	Bool findSupportFrom(UInt trieIdx,UInt curLev,TriePNode p,Support& sup);

	DomArrayView<T,View>		doms;
	Array<Supports>				supports;
	Array<detail::Trie<T>*>		tries;
	INotifiable*				pOwner;
	Reversible<Bool>			firstExecution;
};

template<class T, class View>
GACSchemaNeg<T,View>::GACSchemaNeg(const View& vars, const Array<T,2>& ss) :
	IFilter(vars.solver()), doms(vars.solver(),vars),
	supports(vars.solver().heap(),doms.size(),vars.solver()),
	tries(vars.solver().heap(),doms.size()),
	firstExecution(vars.solver(),true)
{
	for (UInt t = 0; t < doms.size(); t++)
	{
		tries[t] = new (solver().heap())
					detail::Trie<T>(solver().heap(),doms.size());
		for (UInt i = 0; i < ss.size(0); i++)
		{
			detail::Vector<T> aux(stdHeap,doms.size(),0);
			aux[0] = ss[i][t];
			for (UInt j = 1; j <= t; j++)
				aux[j] = ss[i][j-1];
			for (UInt j = t+1; j < doms.size(); j++)
				aux[j] = ss[i][j];
			tries[t]->insert(aux.begin(),aux.end());
		}
		//std::cout << *tries[t] << std::endl;
	}
}

/*
template<class T, class View>
void GACSchemaNeg<T,View>::printSupports()
{
	for (SupportIterator it = supports.begin(); it != supports.end(); ++it)
		std::cout << *it->pTuple << std::endl;
	std::cout << std::endl;
}*/

template<class T, class View>
UInt GACSchemaNeg<T,View>::getVarIdx(UInt trieIdx,UInt level) const
{
	if (trieIdx < level)
		return level;
	else
	if (level > 0)
		return level-1;
	else
		return trieIdx;
}

// version 1 -> recursive
template<class T, class View>
Bool GACSchemaNeg<T,View>::findSupportFrom(UInt trieIdx,
										   UInt curLev,
										   TriePNode p,
										   Support& sup)
{
	assert(p != NULL);

	if (p->pLChild == NULL)
		return false;

	// first count number of subtrees
	UInt i = getVarIdx(trieIdx,curLev+1);
	UInt ntrees = 0;
	for (TriePNode pi = p->pLChild ; !tries[trieIdx]->isDelim(pi); pi = pi->pRSibling)
		++ntrees;

	// if the domain cardinality is larger than the number of trees we're done
	if (doms[i]->size() > ntrees)
	{
		sup.pNode = p;
		sup.curLev = curLev;
		return true;
	}

	// if some domain values don't belong to the set of tree roots we're done
	DomIterator it = doms[i]->begin();
	for (TriePNode pi = p->pLChild  ; !tries[trieIdx]->isDelim(pi) and it!=doms[i]->end();
			pi = pi->pRSibling)
		if (*it < tries[i]->value(pi))
		{
			sup.pNode = p;
			sup.curLev = curLev;
			return true;
		}
		else
		if (*it == tries[i]->value(pi))
			++it;
	if (it != doms[i]->end())
	{
		sup.pNode = p;
		sup.curLev = curLev;
		return true;
	}
	// otherwise search for a support in the subtrees (depth-first)
	for (TriePNode pi = p->pLChild; !tries[trieIdx]->isDelim(pi); pi = pi->pRSibling)
		if (findSupportFrom(trieIdx,curLev+1,pi,sup))
			return true;
	return false;
}

template<class T, class View>
Bool GACSchemaNeg<T,View>::firstExecute()
{
	// seek initial supports
	for (UInt i = 0; i < tries.size(); i++)
	{
		DomIterator it = doms[i]->begin();
		TriePNode p = tries[i]->levels[0];
		while ( p != NULL and it != doms[i]->end() )
			if (*it < tries[i]->value(p))
				++it;
			else
			if (*it > tries[i]->value(p))
				p = p->pRSibling;
			else
			{
				Support sup;
				if (findSupportFrom(i,0,p,sup))
					supports[i].pushBack(sup);
				else
				if (!doms[i]->erase(it))
					return false;
				++it;
				p = p->pRSibling;
			}
	}
	return true;
}

template<class T, class View>
void GACSchemaNeg<T,View>::attach(INotifiable* s)
{
	pOwner = s;

	for (UInt i = 0; i < doms.size(); i++)
		doms[i]->attachOnDomain(s);
	//	new (solver().heap()) VarDemon(this,i);
}

template<class T, class View>
Bool GACSchemaNeg<T,View>::execute()
{
	if (firstExecution)
	{
		firstExecution = false;
		return firstExecute();
	}

	// visit every trie and check if some support was lost
	for (UInt i = 0; i < doms.size(); i++)
		for (SupportIterator it = supports[i].begin();
				it != supports[i].end(); )
		{
			// go up to check if path to support is still valid
			TriePNode p = it->pNode;
			UInt curLev = it->curLev;
			UInt hLev = doms.size();
			TriePNode hp = NULL;
			while ( !tries[i]->isDelim(p) )
			{
				UInt varIdx = getVarIdx(i,curLev);
				if (doms[varIdx]->find(tries[i]->value(p)) == doms[varIdx]->end())
				{
					hLev = curLev;
					hp = p;
				}
				--curLev;
				p = p->pParent;
			}
			// if path is valid then check if support below is still valid
			if (hp == NULL)
			{
				Support sup;
				// if there is no support below
				if (!findSupportFrom(i,it->curLev,it->pNode,sup))
				{
					hp = it->pNode;
					hLev = it->curLev;
				}
				else
				{
					if (sup.pNode == it->pNode)	// support is still valid
						++it;
					else					// another support was found below
					{
						supports[i].erase(it++);
						supports[i].pushBack(sup);
					}
					goto nextSupport;
				}
			}

			// at this point hp is the end of a valid path with no valid subtrees.
			// go up until the root while searching for new support
			{
				supports[i].erase(it++);
				UInt varIdx = getVarIdx(i,hLev);
				DomIterator dit = doms[varIdx]->upperBound(tries[i]->value(hp));
				while (hLev > 0)
				{
					TriePNode hpPParent = hp->pParent;
					while (dit != doms[varIdx]->end())
					{
						while (tries[i]->value(hp) < *dit)
						{
							hp = hp->pRSibling;
							assert(!tries[i]->isDelim(hp));
						}
						assert(tries[i]->value(hp) == *dit);
						Support sup;
						if (findSupportFrom(i,hLev,hp,sup))
						{
							supports[i].pushBack(sup);
							goto nextSupport;
						}
						++dit;
						hp = hp->pRSibling;
					}
					--hLev;
					hp = hpPParent;
					varIdx = getVarIdx(i,hLev);
					dit = doms[varIdx]->upperBound(tries[i]->value(hp));
				}
				/*std::cout << "support not found for value "
							  << tries[i]->value(hp)
							  << " in trie " << i << std::endl;*/
				if (!doms[i]->erase(tries[i]->value(hp)))
					return false;
			}
			nextSupport:;
		}
	return true;
}

template<class View>
Filter gacSchemaNeg(const View& v,
				 	const Array<Int,2>& ss)
{
	return new (v.solver().heap())
		GACSchemaNeg<Int,View>(v,ss);
}
#endif

}

#endif /*NEGATIVE_H_*/
