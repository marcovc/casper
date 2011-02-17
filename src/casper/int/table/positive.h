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

#ifndef CASPER_INT_POSITIVE_H_
#define CASPER_INT_POSITIVE_H_

#include <casper/int/filter.h>
#include <casper/kernel/container/rlist.h>
#include <casper/kernel/container/rstack.h>
#include <casper/kernel/container/trie.h>
#include <casper/kernel/delta.h>

namespace casper {

#define GACSCHEMA_TABLE
//#define GACSCHEMA_TRIES

#if defined(GACSCHEMA_TABLE)

template<class T>
struct IExtRel
{
	virtual ~IExtRel() {}
	virtual Array<T>* seekSupport(UInt,T) = 0;
};

template<class T, class View>
struct GACSchema : IFilter
{
	typedef	DomArrayView<T,View>		Doms;
	typedef typename DomTraits<typename ElemTraits<View>::Elem>::Dom		Dom;
	typedef typename Dom::Iterator	DomIterator;
	typedef Array<T>		Tuple;
	typedef Tuple*			PTuple;
	typedef RevArray<Bool>	InSupport;

	struct Support
	{
		Support(ICPSolver& solver, PTuple pTuple) :
			pTuple(pTuple),inSupport(solver,pTuple->size(),false) {}
		PTuple		pTuple;
		InSupport	inSupport;
	};

	typedef detail::RList<Support>	Supports;
	typedef typename Supports::Iterator	SupportIterator;
	typedef UInt			DeltaInfo;

	struct VarDemon : INotifiable
	{
		VarDemon(GACSchema* pOwner, UInt idx) :
			rOwner(*pOwner),idx(idx)
		{	rOwner.doms[idx].attach(this);	}
		Bool notify()
		{
			rOwner.erased.push(idx);
			return rOwner.pOwner->notify();
		}

		GACSchema& rOwner;
		UInt idx;
	};

	GACSchema(const View& vars, IExtRel<T>& ss) :
		IFilter(getCPSolver(vars)), doms(getCPSolver(vars),vars),
		supports(getCPSolver(vars)),erased(getCPSolver(vars)),ss(ss),
		firstExecution(getCPSolver(vars),true) {}

	bool execute();
	bool firstExecute();

	void attach(INotifiable* s);
	void detach(INotifiable* s) {}
	Cost cost() const {	return exponentialHi;}
	void printSupports();
	private:
	SupportIterator	seekInferableSupport(UInt varIdx, const T& val);
	SupportIterator	seekSupport(UInt varIdx, const T& val);

	DomArrayView<T,View>		doms;
	Supports					supports;
	detail::RStack<DeltaInfo>	erased;
	IExtRel<T>&					ss;
	INotifiable*				pOwner;
	Reversible<Bool>			firstExecution;
};

template<class T, class View>
void GACSchema<T,View>::printSupports()
{
	for (SupportIterator it = supports.begin(); it != supports.end(); ++it)
		std::cout << *it->pTuple << std::endl;
	std::cout << std::endl;
}

template<class T, class View>
Bool GACSchema<T,View>::firstExecute()
{
	// seek initial support

	for (UInt i = 0; i < doms.size(); i++)
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

	for (UInt i = 0; i < doms.size(); i++)
		new (solver().heap()) VarDemon(this,i);
}

template<class T, class View>
typename GACSchema<T,View>::SupportIterator
GACSchema<T,View>::seekInferableSupport(UInt varIdx, const T& val)
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
GACSchema<T,View>::seekSupport(UInt varIdx, const T& val)
{
	//std::cout << "seekSupport: " << varIdx << " = " << val << std::endl;
	//std::cout << "curDoms: " << doms << std::endl;
	//printSupports();
	Array<Int>*	pTuple;
	while ((pTuple = ss.seekSupport(varIdx,val)) != NULL)
	{
		//std::cout << "found support for (" << pVV->idxVar << "," << pVV->val << ") : " << support  << std::endl;

		// validate support pTuple
		UInt i = 0;
		for ( ; i < doms.size(); i++)
		{
			DomIterator domIt = doms[i]->find((*pTuple)[i]);
			if (domIt == doms[i]->end())
				break;
		}
		if (i == doms.size())
		{
			Support support(solver(),pTuple);
			support.inSupport[varIdx] = true;
			return supports.pushBack(support);
		}
	}

	return supports.end();
}

template<class T, class View>
bool GACSchema<T,View>::execute()
{
	solver().log(this,"GACSchema",Logger::filterExecuteBegin);
	if (firstExecution)
	{
		firstExecution = false;
		Bool r = firstExecute();
		solver().log(this,"GACSchema",Logger::filterExecuteEnd);
		return r;
	}

	while (!erased.empty())
	{
		UInt erasedVarIdx = erased.top();
		erased.pop();
		//std::cout << "processing erased val in var " << erasedVarIdx << std::endl;
		//std::cout << "var: " << doms[erasedVarIdx] << std::endl;
		for (SupportIterator itSup = supports.begin();
				itSup != supports.end(); ++itSup)
			if (itSup->inSupport[erasedVarIdx] and
				doms[erasedVarIdx]->find((*itSup->pTuple)[erasedVarIdx]) == doms[erasedVarIdx]->end())
			{
				supports.erase(itSup); // ok since tuples is reversible
				for (UInt varIdx = 0; varIdx < doms.size(); ++varIdx)
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
								solver().log(this,"GACSchema",Logger::filterExecuteEnd);
								return false;
							}
						}
					}
			}
	}
//	std::cout << "returning true\n";
	solver().log(this,"GACSchema",Logger::filterExecuteEnd);
	return true;
}


// ---

template<class T>
struct PosTableRel : IExtRel<T>,Array<T,2>
{
	typedef Array<T,2>	Super;

	PosTableRel(ICPSolver& solver,UInt nvars, Int minElem,Int maxElem, UInt nsols) :
		Super(solver.heap(),nsols,nvars),
		minElem(minElem),
		curSolIdx(solver,nvars,maxElem-minElem+1),
		nsols(nsols)
		{}

	PosTableRel(ICPSolver& solver,const PosTableRel& s) :
		Super((const Super&)s),
		minElem(s.minElem),
		curSolIdx(solver,s.curSolIdx.size(0),s.curSolIdx.size(1)),
		nsols(s.nsols) {}

	PosTableRel(ICPSolver& solver,const Array<Int,2>& s) :
		Super(s),
		minElem(findMin(s)),
		curSolIdx(solver,s.size(1),findMax(s)-minElem+1),
		nsols(s.size(0)) {}

	Array<T>* seekSupport(UInt idxVar, T val)
	{
                if (val-minElem >= static_cast<Int>(curSolIdx.size(1)) or
                    val-minElem < 0)
			return false;
		for (UInt i = curSolIdx[idxVar][val-minElem]; i < nsols; i++)
			if ((*this)[i][idxVar] == val)
			{
				curSolIdx[idxVar][val-minElem] = i+1;
				return &(*this)[i];
			}
		return NULL;
	}

	Int findMin(const Array<Int,2>& s)
	{
		Int r = limits<Int>::max();
		for (UInt i = 0; i < s.count(); i++)
			if (s(i) < r)
				r = s(i);
		return r;
	}

	Int findMax(const Array<Int,2>& s)
	{
		Int r = limits<Int>::min();
		for (UInt i = 0; i < s.count(); i++)
			if (s(i) > r)
				r = s(i);
		return r;
	}

	T					minElem;
	RevArray<UInt,2> 	curSolIdx;
	UInt				nsols;
};

// --------

// \deprecated
/*
template<class View>
Filter gacSchema(const View& v,
				 IExtRel<Int>& ss)
{
	return new (v.solver().heap())
		GACSchema<Int,View>(v,ss);
}*/

template<class Eval,class View>
Filter gacSchema(const View& v,
				 const Array<Eval,2>& ss)
{
	ICPSolver& solver(getCPSolver(v));
	PosTableRel<Eval>* p = new (solver.heap()) PosTableRel<Eval>(solver,ss);
	return new (solver.heap()) GACSchema<Eval,View>(v,*p);
}

template<class Eval,class View>
Filter gacSchema(const View& v,
				 const Array<Eval,1>& ss)
{
	DomArrayView<Eval,View> d(getCPSolver(v),v);
	Array<Eval,2> a(getCPSolver(v).heap(),ss.size()/d.size(),d.size());
	for (UInt i = 0; i < ss.size(); ++i)
		a(i) = ss[i];
	PosTableRel<Eval>* p = new (v.solver().heap()) PosTableRel<Eval>(v.solver(),a);
	return new (v.solver().heap()) GACSchema<Eval,View>(v,*p);
}

template<class Eval,class View1,class View2>
struct BndFilterFact2<InTable,Seq<Eval>,View1,Seq<Eval>,View2>
{
	static Filter create(ICPSolver& solver,const View1& v1, const View2& v2)
	{	return gacSchema(v1,v2);	}
};

template<class Eval,class View1,class View2>
struct DomFilterFact2<InTable,Seq<Eval>,View1,Seq<Eval>,View2>
{
	static Filter create(ICPSolver& solver,const View1& v1, const View2& v2)
	{	return gacSchema(v1,v2);	}
};

#elif defined(GACSCHEMA_TRIES)

template<class T, class View>
struct GACSchema : IFilter
{
	typedef	DomArrayView<T,View>				Doms;
	typedef typename Doms::Dom::Iterator		DomIterator;

	typedef typename detail::Trie<T>::PNode		TriePNode;
	typedef detail::RList<TriePNode>			Supports;
	typedef typename Supports::Iterator			SupportIterator;

	GACSchema(const View& vars, const Array<T,2>& ss);

	UInt getVarIdx(UInt trieIdx,UInt level) const;

	bool execute();
	bool firstExecute();

	void attach(INotifiable* s) { erased.attach(s); }
	void detach(INotifiable* s) { erased.detach(s);	}
	Cost cost() const {	return exponentialLo;}
	//void printSupports();
	private:
	Bool findSupportFrom(UInt trieIdx,UInt curLev,TriePNode p);

	DomArrayView<T,View>		doms;
	Array<Supports>				supports;
	detail::IdxDeltaStack<View>	erased;
	Array<detail::Trie<T>*>		tries;
	Reversible<Bool>			firstExecution;
};

template<class T, class View>
GACSchema<T,View>::GACSchema(const View& vars, const Array<T,2>& ss) :
	IFilter(vars.solver()), doms(vars.solver(),vars),
	supports(vars.solver().heap(),doms.size(),vars.solver()),
	erased(vars),
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

template<class T, class View>
UInt GACSchema<T,View>::getVarIdx(UInt trieIdx,UInt level) const
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
Bool GACSchema<T,View>::findSupportFrom(UInt trieIdx,
										UInt curLev,
										TriePNode p)
{
	if (p->pLChild == NULL)	// terminal condition
	{
		assert(curLev+1 == tries[trieIdx]->maxDepth());
		supports[trieIdx].pushFront(p);
		return true;
	}

	++curLev;
	UInt i = getVarIdx(trieIdx,curLev);
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
Bool GACSchema<T,View>::firstExecute()
{
	// seek initial supports
	for (UInt i = 0; i < tries.size(); i++)
	{
		DomIterator it = doms[i]->begin();
		for (TriePNode p = tries[i]->levels[0];
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
Bool GACSchema<T,View>::execute()
{
	if (firstExecution)
	{
		firstExecution = false;
		return firstExecute();
	}

	// find the two smallest indexes of the changed variables
	detail::Vector<Bool>	changedVars(stdHeap,doms.size(),false);
//	UInt min1,min2;
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
	for (UInt i = 0; i < doms.size(); i++)
	{
		//UInt minLev = std::min(getLevIdx(i,min1),getLevIdx(i,min2));
		for (SupportIterator it = supports[i].begin();
			it != supports[i].end(); )
		{
			// go up to check if support is still valid
			TriePNode p = *it;
			UInt curLev = doms.size()-1;
			UInt hLev = doms.size();
			TriePNode hp = NULL;
			while ( !tries[i]->isDelim(p) /*and curLev >= minLev*/)
			{
				UInt varIdx = getVarIdx(i,curLev);
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
				UInt varIdx = getVarIdx(i,hLev);
				DomIterator dit = doms[varIdx]->upperBound(tries[i]->value(hp));
				while (hLev > 0)
				{
					TriePNode hpPParent = hp->pParent;
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
				 const Array<Int,2>& ss)
{
	return new (v.solver().heap())
		GACSchema<Int,View>(v,ss);
}

#endif



}

#endif /*CASPER_INT_POSITIVE_H_*/
