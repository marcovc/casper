 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_CP_INT_POSITIVE_H_
#define CASPER_CP_INT_POSITIVE_H_

#include <casper/cp/int/filter.h>
#include <casper/kernel/reversible/list.h>
#include <casper/kernel/reversible/stack.h>
#include <casper/util/container/stdtrie.h>
#include <casper/util/container/stdarray.h>
#include <casper/cp/delta.h>

#include <iostream>

//#ifdef CASPER_PRECOMPILED
//#include <casper/cp/int/spexpr/explicit_postdom.h>
//#endif

namespace Casper {
namespace CP {


#define GACSCHEMA_TABLE
//#define GACSCHEMA_TRIES

#if defined(GACSCHEMA_TABLE)

template<class T>
struct IExtRel
{
	virtual ~IExtRel() {}
	virtual Util::StdArray<T>* seekSupport(uint,T) = 0;
};

template<class T, class View>
struct GACSchema : IFilter
{
	typedef	DomArrayView<T,View>		Doms;
	typedef typename Traits::GetDom<typename Casper::Traits::GetTermElem<View>::Type>::Type		Dom;
	typedef typename Dom::Iterator	DomIterator;
	typedef Util::StdArray<T>		Tuple;
	typedef Tuple*			PTuple;
	typedef Array<bool>	InSupport;

	struct Support
	{
		Support(Store& store, PTuple pTuple) :
			pTuple(pTuple),inSupport(store,pTuple->size(),false) {}
		PTuple		pTuple;
		InSupport	inSupport;
	};

	typedef List<Support>	Supports;
	typedef typename Supports::Iterator	SupportIterator;
	typedef uint			DeltaInfo;

	struct VarDemon : INotifiable
	{
		VarDemon(GACSchema* pOwner, uint idx) :
			rOwner(*pOwner),idx(idx)
		{	rOwner.doms[idx].attach(this);	}
		bool notify()
		{
			rOwner.erased.push(idx);
			return rOwner.pOwner->notify();
		}

		GACSchema& rOwner;
		uint idx;
	};

	GACSchema(Store& store,const View& vars, IExtRel<T>& ss) :
		IFilter(store),
		store(store),
		doms(store,vars),
		supports(store),erased(store),ss(ss),
		firstExecution(store,true) {}

	bool execute();
	bool firstExecute();

	void attach(INotifiable* s);
	void detach() {}
	Cost cost() const {	return exponentialHi;}
	void printSupports();
	private:
	SupportIterator	seekInferableSupport(uint varIdx, const T& val);
	SupportIterator	seekSupport(uint varIdx, const T& val);

	Store&						store;
	DomArrayView<T,View>		doms;
	Supports					supports;
	Stack<DeltaInfo>			erased;
	IExtRel<T>&					ss;
	INotifiable*				pOwner;
	Reversible<bool>			firstExecution;
};

template<class T, class View>
void GACSchema<T,View>::printSupports()
{
	for (SupportIterator it = supports.begin(); it != supports.end(); ++it)
	{
		for (uint i = 0; i < (*(it->pTuple)).size(); ++i)
			std::cout << (*(it->pTuple))[i] << " ";
		std::cout << std::endl;
	}
	//	std::cout << (*it->pTuple) << std::endl;
	//std::cout << std::endl;
}

template<class T, class View>
bool GACSchema<T,View>::firstExecute()
{
	// seek initial support
	for (uint i = 0; i < doms.size(); i++)
	{
		for (DomIterator dit = doms[i]->begin(); dit != doms[i]->end(); )
		{
			SupportIterator supTuple = seekInferableSupport(i,*dit);
			if (supTuple == supports.end())
				supTuple = seekSupport(i,*dit);
			if (supTuple == supports.end())
			{
				if (!doms[i]->erase(dit++))
					return false;
			}
			else
				++dit;
		}
	}
	//std::cout << "after initial support:\n";
	//printSupports();
	return true;
}

template<class T, class View>
void GACSchema<T,View>::attach(INotifiable* s)
{
	pOwner = s;

	for (uint i = 0; i < doms.size(); i++)
		new (store) VarDemon(this,i);
}

template<class T, class View>
typename GACSchema<T,View>::SupportIterator
GACSchema<T,View>::seekInferableSupport(uint varIdx, const T& val)
{
	SupportIterator itSup = supports.begin();
	for ( ; itSup != supports.end(); ++itSup)
		if ( (*itSup->pTuple)[varIdx] == val)
		{
			itSup->inSupport[varIdx] = true;
			break;
		}
	return itSup;
}

template<class T, class View>
typename GACSchema<T,View>::SupportIterator
GACSchema<T,View>::seekSupport(uint varIdx, const T& val)
{
	//std::cout << "seekSupport: " << varIdx << " = " << val << std::endl;
	//std::cout << "curDoms: " << doms << std::endl;
	//printSupports();
	Util::StdArray<T>*	pTuple;
	while ((pTuple = ss.seekSupport(varIdx,val)) != NULL)
	{
		// validate support pTuple
		uint i = 0;
		for ( ; i < doms.size(); i++)
		{
			DomIterator domIt = doms[i]->find((*pTuple)[i]);
			if (domIt == doms[i]->end())
				break;
		}
		if (i == doms.size())
		{
			Support support(store,pTuple);
			support.inSupport[varIdx] = true;
			return supports.pushBack(support);
		}
	}

	return supports.end();
}

template<class T, class View>
bool GACSchema<T,View>::execute()
{
	//store.getEnv().log(this,"GACSchema",Util::Logger::filterExecuteBegin);
	if (firstExecution)
	{
		firstExecution = false;
		bool r = firstExecute();
	//	store.getEnv().log(this,"GACSchema",Util::Logger::filterExecuteEnd);
		return r;
	}

	while (!erased.empty())
	{
		uint erasedVarIdx = erased.top();
		erased.pop();
		//std::cout << "processing erased val in var " << erasedVarIdx << std::endl;
		//std::cout << "var: " << doms[erasedVarIdx] << std::endl;
		for (SupportIterator itSup = supports.begin();
				itSup != supports.end(); ++itSup)
			if (itSup->inSupport[erasedVarIdx] and
				doms[erasedVarIdx]->find((*itSup->pTuple)[erasedVarIdx]) == doms[erasedVarIdx]->end())
			{
				supports.erase(itSup); // ok since tuples is reversible
				for (uint varIdx = 0; varIdx < doms.size(); ++varIdx)
					if (itSup->inSupport[varIdx])
					{
						SupportIterator nextSup =
							seekInferableSupport(varIdx,(*itSup->pTuple)[varIdx]);
						if (nextSup == supports.end())
							nextSup = seekSupport(varIdx,(*itSup->pTuple)[varIdx]);
						if (nextSup == supports.end())
						{	//std::cout << "couldn't find support for " << varIdx << " " << (*itTup)[varIdx]->val << std::endl;
							if (!doms[varIdx]->erase((*itSup->pTuple)[varIdx]))
							{
								//store.getEnv().log(this,"GACSchema",Util::Logger::filterExecuteEnd);
								return false;
							}
						}
					}
			}
	}
//	std::cout << "returning true\n";
	//store.getEnv().log(this,"GACSchema",Util::Logger::filterExecuteEnd);
	return true;
}


// ---

template<class T>
struct PosTableRel : IExtRel<T>,Util::StdArray<T,2>
{
	typedef Util::StdArray<T,2>	Super;

	PosTableRel(Store& store,uint nvars, int minElem,int maxElem, uint nsols) :
		Super(store,nsols,nvars),
		minElem(minElem),
		curSolIdx(store,nvars,maxElem-minElem+1,0),
		nsols(nsols)
		{}

	PosTableRel(Store& store,const PosTableRel& s) :
		Super((const Super&)s),
		minElem(s.minElem),
		curSolIdx(store,s.curSolIdx.size(0),s.curSolIdx.size(1),0),
		nsols(s.nsols) {}

	PosTableRel(Store& store,const Util::StdArray<T,2>& s) :
		Super(s),
		minElem(findMin(s)),
		curSolIdx(store,s.size(1),findMax(s)-minElem+1,0),
		nsols(s.size(0))	{}

	Util::StdArray<T>* seekSupport(uint idxVar, T val)
	{
        if (val-minElem >= static_cast<int>(curSolIdx.size(1)) or
                    val-minElem < 0)
			return false;
		for (uint i = curSolIdx[idxVar][val-minElem]; i < nsols; i++)
			if ((*this)[i][idxVar] == val)
			{
				curSolIdx[idxVar][val-minElem] = i+1;
				return &(*this)[i];
			}
		return NULL;
	}

	int findMin(const Util::StdArray<T,2>& s)
	{
		int r = s(0);
		for (uint i = 1; i < s.count(); i++)
			if (s(i) < r)
				r = s(i);
		return r;
	}

	int findMax(const Util::StdArray<T,2>& s)
	{
		int r = s(0);
		for (uint i = 1; i < s.count(); i++)
			if (s(i) > r)
				r = s(i);
		return r;
	}

	T					minElem;
	Array<uint,2> 		curSolIdx;
	uint				nsols;
};

// --------

// \deprecated
/*
template<class View>
Filter gacSchema(const View& v,
				 IExtRel<int>& ss)
{
	return new (v.solver().getHeap())
		GACSchema<int,View>(v,ss);
}*/

template<class Eval,class View>
Filter gacSchema(Store& store,const View& v,const Util::StdArray<Eval,2>& ss)
{
	PosTableRel<Eval>* p = new (store) PosTableRel<Eval>(store,ss);
	return new (store) GACSchema<Eval,View>(store,v,*p);
}

#if 1
template<class Eval,class View>
Filter gacSchema(Store& store,const View& v,const Util::StdArray<Eval,1>& ss)
{
	DomArrayView<Eval,View> d(store,v);
	Util::StdArray<Eval,2> a(store,ss.size()/d.size(),d.size());
	for (uint i = 0; i < ss.size(); ++i)
		a(i) = ss[i];
	PosTableRel<Eval>* p = new (store) PosTableRel<Eval>(store,a);
	return new (store) GACSchema<Eval,View>(store,v,*p);
}
#endif

template<class Eval,class Expr1,class Expr2>
struct PostBndFilter2<InTable,Seq<Eval>,Expr1,Seq<Eval>,Expr2>
{
	static bool post(Store& store,const Expr1& v1, const Expr2& v2)
	{	return store.post(gacSchema(store,v1,v2));	}
};

template<class Eval,class Expr1,class Expr2>
struct PostDomFilter2<InTable,Seq<Eval>,Expr1,Seq<Eval>,Expr2>
{
	static bool post(Store& store,const Expr1& v1, const Expr2& v2)
	{	return store.post(gacSchema(store,v1,v2));	}
};

#elif defined(GACSCHEMA_TRIES)

template<class T, class View>
struct GACSchema : IFilter
{
	typedef	DomArrayView<T,View>				Doms;
	typedef typename Doms::Dom::Iterator		DomIterator;

	typedef typename Util::StdTrie<T>::PNode		Util::StdTriePNode;
	typedef List<Util::StdTriePNode>			Supports;
	typedef typename Supports::Iterator			SupportIterator;

	GACSchema(const View& vars, const Util::StdArray<T,2>& ss);

	uint getVarIdx(uint trieIdx,uint level) const;

	bool execute();
	bool firstExecute();

	void attach(INotifiable* s) { erased.attach(s); }
	void detach(INotifiable* s) { erased.detach(s);	}
	Cost cost() const {	return exponentialLo;}
	//void printSupports();
	private:
	bool findSupportFrom(uint trieIdx,uint curLev,Util::StdTriePNode p);

	DomArrayView<T,View>		doms;
	Util::StdArray<Supports>				supports;
	Detail::IdxDeltaStack<View>	erased;
	Util::StdArray<Util::StdTrie<T>*>		tries;
	Reversible<bool>			firstExecution;
};

template<class T, class View>
GACSchema<T,View>::GACSchema(const View& vars, const Util::StdArray<T,2>& ss) :
	IFilter(vars.solver()), doms(vars.solver(),vars),
	supports(vars.solver().getHeap(),doms.size(),vars.solver()),
	erased(vars),
	tries(vars.solver().getHeap(),doms.size()),
	firstExecution(vars.solver(),true)
{
	for (uint t = 0; t < doms.size(); t++)
	{
		tries[t] = new (solver().getHeap())
					Util::StdTrie<T>(solver().getHeap(),doms.size());
		for (uint i = 0; i < ss.size(0); i++)
		{
			Util::StdVector<T> aux(stdHeap,doms.size(),0);
			aux[0] = ss[i][t];
			for (uint j = 1; j <= t; j++)
				aux[j] = ss[i][j-1];
			for (uint j = t+1; j < doms.size(); j++)
				aux[j] = ss[i][j];
			tries[t]->insert(aux.begin(),aux.end());
		}
		//std::cout << *tries[t] << std::endl;
	}
}

template<class T, class View>
uint GACSchema<T,View>::getVarIdx(uint trieIdx,uint level) const
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
bool GACSchema<T,View>::findSupportFrom(uint trieIdx,
										uint curLev,
										Util::StdTriePNode p)
{
	if (p->pLChild == NULL)	// terminal condition
	{
		assert(curLev+1 == tries[trieIdx]->maxDepth());
		supports[trieIdx].pushFront(p);
		return true;
	}

	++curLev;
	uint i = getVarIdx(trieIdx,curLev);
	DomIterator it = doms[i]->begin();
	for (p = p->pLChild; !tries[trieIdx]->isDelim(p); p = p->pRSibling)
	{
		while (it != doms[i]->end() and *it < tries[i]->value(p))
			++it;
		if (it == doms[i]->end())
			return false;
		if (*it == tries[i]->value(p))
			if (findSupportFrom(trieIdx,curLev,p))
				return true;
			else
				++it;
	}
	return false;
}

template<class T, class View>
bool GACSchema<T,View>::firstExecute()
{
	// seek initial supports
	for (uint i = 0; i < tries.size(); i++)
	{
		DomIterator it = doms[i]->begin();
		for (Util::StdTriePNode p = tries[i]->levels[0];
				p != NULL; p = p->pRSibling)
		{
			while (it != doms[i]->end() and *it < tries[i]->value(p))
				if (!doms[i]->erase(it++))
					return false;
			if (it == doms[i]->end())
				break;
			if (*it == tries[i]->value(p))
			{
				if (!findSupportFrom(i,0,p))
				{
					if (!doms[i]->erase(it++))
						return false;
				}
				else
					++it;
			}
		}
		if (it != doms[i]->end())
			if (!doms[i]->erase(it,doms[i]->end()))
				return false;
	}
	return true;
}

template<class T, class View>
bool GACSchema<T,View>::execute()
{
	if (firstExecution)
	{
		firstExecution = false;
		return firstExecute();
	}

	// find the two smallest indexes of the changed variables
	Util::StdVector<bool>	changedVars(stdHeap,doms.size(),false);
//	uint min1,min2;
//	min1 = min2 = doms.size();
	while (!erased.empty())
	{
		changedVars[erased.top()] = true;
/*		if (erased.top() < min1)
			min1 = erased.top();
		else
		if (erased.top() < min2)
			min2 = erased.top();
*/		erased.pop();
	}

	// visit every trie and check if some support was lost
	for (uint i = 0; i < doms.size(); i++)
	{
		//uint minLev = std::min(getLevIdx(i,min1),getLevIdx(i,min2));
		for (SupportIterator it = supports[i].begin();
			it != supports[i].end(); )
		{
			// go up to check if support is still valid
			Util::StdTriePNode p = *it;
			uint curLev = doms.size()-1;
			uint hLev = doms.size();
			Util::StdTriePNode hp = NULL;
			while ( !tries[i]->isDelim(p) /*and curLev >= minLev*/)
			{
				uint varIdx = getVarIdx(i,curLev);
				if (changedVars[varIdx] and
					doms[varIdx]->find(tries[i]->value(p)) == doms[varIdx]->end())
				{
					hLev = curLev;
					hp = p;
				}
				--curLev;
				p = p->pParent;
			}
			if (hLev == 0)
				supports[i].erase(it++);
			else
			if (hLev < doms.size())
			{
				supports[i].erase(it++);
				uint varIdx = getVarIdx(i,hLev);
				DomIterator dit = doms[varIdx]->upperBound(tries[i]->value(hp));
				while (hLev > 0)
				{
					Util::StdTriePNode hpPParent = hp->pParent;
					while (!tries[i]->isDelim(hp))
					{
						while (dit != doms[varIdx]->end() and
							   *dit < tries[i]->value(hp))
							++dit;
						if (dit == doms[varIdx]->end())
							break;
						if (*dit == tries[i]->value(hp) and
							findSupportFrom(i,hLev,hp))
							goto nextSupport;
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
			else
				++it;

			nextSupport:;
		}
	}
	return true;
}

template<class View>
Filter gacSchema(const View& v,
				 const Util::StdArray<int,2>& ss)
{
	return new (v.solver().getHeap())
		GACSchema<int,View>(v,ss);
}

#endif


} // CP
} // Casper

#endif /*CASPER_CP_INT_POSITIVE_H_*/
