/***************************************************************************
 *   Copyright (C) 2007 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <casper/kernel.h>
#include <casper/cp/set.h>
#include <casper/cp/int.h>
#include <casper/util/options.h>

#include <boost/math/special_functions/factorials.hpp>
#include <iostream>

using namespace Casper;
using namespace Casper::CP;
//using namespace std;

#if 0
/**
 * Computes the set of all n-ary sums of the elements in a set domain variable.
 * \note needs testing
 */
template<class Arg1,class Arg2>
struct NSumsEqual : IFilter
{
	typedef typename DomView<Set<int>,Arg1>::Dom	DomX;
	typedef typename DomView<Set<int>,Arg2>::Dom	DomY;

	NSumsEqual(CPSolver& solver,const Arg1& xx, const Arg2& yy, const uint& n) :
				  IFilter(solver),x(solver,xx),y(solver,yy),n(n) {}

	bool execute();
	void attach(INotifiable* f)
	{	x->attachOnDomain(f); y->attachOnDomain(f);	}
	void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f);	}

	Cost cost() const
	{	return quadraticLo;	}

	template<class Iteration>
	List<int> getSums(Iteration it, uint tsize);
	template<class Iteration>
	List<int> getSumsSorted(Iteration it, uint tsize);

	DomView<Set<int>,Arg1>			x;
	DomView<Set<int>,Arg2>			y;
	const uint						n;
};

template<class Arg1,class Arg2>
template<class Iteration>
List<int> NSumsEqual<Arg1,Arg2>::getSums(Iteration it, uint tsize)
{
	List<int> ret;
	if (tsize == 1)
	{
		for ( ; it.valid(); it.iterate())
			ret.pushBack(it.value());
		return ret;
	}
	uint c = 0;
	List<int> sums_1(getSums(it,tsize-1));
	for (List<int>::Iterator vit = sums_1.begin(); vit != sums_1.end(); ++vit)
	{
		Iteration iit(it);
		for (uint i = 0; iit.valid() and i < tsize-1; ++i)
			iit.iterate();
		for (uint i = 0; iit.valid() and i < c; ++i)
			iit.iterate();
		for ( ; iit.valid(); iit.iterate())
			ret.pushBack(iit.value()+*vit);
		++c;
	}
	return ret;
}

template<class Arg1,class Arg2>
template<class Iteration>
List<int> NSumsEqual<Arg1,Arg2>::getSumsSorted(Iteration it, uint tsize)
{
	List<int> sumsUnsorted(getSums(it,tsize));
	std::set<int> sumsV(sumsUnsorted.begin(),sumsUnsorted.end());
	List<int> ret(sumsV.begin(),sumsV.end());
	return ret;
}

template<class Arg1,class Arg2>
bool NSumsEqual<Arg1,Arg2>::execute()
{
//	std::cout << "begin: " << *x << " " << *y << std::endl;
	if (x->inSize() >= n)
		if (!Detail::setSafeInsertRange(*y,makeIt(getSumsSorted(makeInIt(*x),n))))
			return false;

	if (x->inSize()+x->possSize() >= n)
		if (!Detail::setSafeEraseRange(*y,makeDiffIt(makePossIt(*y),
										  makeIt(getSumsSorted(makeLUBIt(*x),n)))))
			return false;
//	std::cout << "end: " << *x << " " << *y << std::endl;
	// not sure if processing "in y" events makes sense. ignoring for now.

	// if an element r is removed from y then we remove from x.poss all e such that
	// e+s=r where s is a possible (n-1) sum already in x.in
/*	std::set<int> inSums_1(getSums(makeInIt(*x),n-1));
	for(typename DomX::PIterator pit = x->beginPoss(); pit != x->endPoss(); )
	{
		for (std::set<int>::iterator it = inSums_1.begin(); it != inSums_1.end(); ++it)
			for (yLUBDIt = y->lubDeltas().beginFrom(yLUBDeltasIt);
					yLUBDIt != y->lubDeltas().end(); ++yLUBDIt)
				for (typename DomX::DeltasIterator dit = yLUBDIt->begin();
						dit != yLUBDIt->end(); ++dit)
					if (*it + *pit == *dit)
					{
						if (x->safeErase(pit++))
							return false;
						goto next;
					}
		++pit;
		next:;
	}
	yLUBDeltasIt = y->lubDeltas().end();*/
	return true;
}

template<class Arg1,class Arg2>
Filter nSumsEqual(const Arg1& a1, const Arg2& a2, const uint& n)
{ return new (getState(a1,a2).getHeap()) NSumsEqual<Arg1,Arg2>(getState(a1,a2),a1,a2,n); }
#endif



template<class Arg1,class Arg2>
struct SortSubsetsWithSameSum : IFilter
{
	typedef typename DomView<Set<int>,Arg1>::Dom	DomX;
	typedef typename DomView<Set<int>,Arg2>::Dom	DomY;

	SortSubsetsWithSameSum(Solver& solver,const Arg1& xx, const Arg2& yy) :
				  IFilter(solver),x(solver,xx),y(solver,yy) {}

	bool execute();
	void attach(INotifiable* f)
	{	x->attachOnGLB(f); y->attachOnGLB(f);	}
	void detach(INotifiable* f)
	{	x->detachOnGLB(f); y->detachOnGLB(f);	}

	Cost cost() const
	{	return quadraticLo;	}

	struct Cmp {
		bool operator()(const Util::StdPair<int>& s1,const Util::StdPair<int>& s2) const
		{
			if (s1.first < s2.first)
				return true;
			if (s1.first > s2.first)
				return false;
			return s1.second < s2.second;
		}
	};

	typedef Util::StdList<Util::StdPair<int> > Sums;
	typedef std::set<Util::StdPair<int>,Cmp> AllSums;

	template<class Iteration>
	Sums getSums(Iteration it, const Sums&, uint tsize);
	template<class Iteration>
	AllSums getSums(Iteration it, uint tsize);

	DomView<Set<int>,Arg1>			x;
	DomView<Set<int>,Arg2>			y;
};

template<class Arg1,class Arg2>
template<class Iteration>
typename SortSubsetsWithSameSum<Arg1,Arg2>::Sums
SortSubsetsWithSameSum<Arg1,Arg2>::getSums(Iteration it,const Sums& sums_1,
												uint tsize)
{
	assert(tsize>1);
	Sums ret;
	uint c = 0;
	for (Sums::Iterator vit = sums_1.begin(); vit != sums_1.end(); ++vit)
	{
		Iteration iit(it);
		for (uint i = 0; iit.valid() and i < tsize-1; ++i)
			iit.iterate();
		for (uint i = 0; iit.valid() and i < c; ++i)
			iit.iterate();
		for ( ; iit.valid(); iit.iterate())
			ret.pushBack(Util::StdPair<int>(iit.value()+vit->first,iit.value()));
		++c;
	}
	return ret;
}

template<class Arg1,class Arg2>
template<class Iteration>
typename SortSubsetsWithSameSum<Arg1,Arg2>::AllSums
SortSubsetsWithSameSum<Arg1,Arg2>::getSums(Iteration it,uint tsize)
{
	AllSums all;

	Sums* xold = new Sums();
	for (Iteration iit(it) ; iit.valid(); iit.iterate())
	{
		xold->pushBack(Util::StdPair<int>(iit.value(),iit.value()));
		all.insert(Util::StdPair<int>(iit.value(),iit.value()));
	}

	for (uint i = 2; i <= tsize; ++i)
	{
		Sums* xnew = new Sums(getSums(it,*xold,i));
		all.insert(xnew->begin(),xnew->end());
		delete xold;
		xold = xnew;
	}
	delete xold;
	return all;
}

template<class Arg1,class Arg2>
bool SortSubsetsWithSameSum<Arg1,Arg2>::execute()
{
	AllSums sumsx(getSums(makeInIt(*x),x->inSize()));
	AllSums sumsy(getSums(makeInIt(*y),y->inSize()));
	typename AllSums::iterator itx = sumsx.begin();
	typename AllSums::iterator ity = sumsy.begin();

/*	std::cout << "xin: " << x->in() << std::endl;
	std::cout << "sumsx: ";
	print(sumsx.begin(),sumsx.end());
	std::cout << std::endl;
*/
	while (itx != sumsx.end() and ity != sumsy.end())
	{
		if (itx->first < ity->first)
			++itx;
		if (ity->first < itx->first)
			++ity;
		if (itx->first == ity->first)
		{
			if (itx->second > ity->second)
			{
		//		std::cout << "failing\n";
				return false;
			}
			typename AllSums::iterator next(itx);
			++next;
			if (next != sumsx.end() and next->first == ity->first)
				++itx;
			else
				++ity;
		}
	}
	return true;
}

template<class Arg1,class Arg2>
Filter sortSubsetsWithSameSum(const Arg1& a1, const Arg2& a2)
{ return new (getState(a1,a2).getHeap()) SortSubsetsWithSameSum<Arg1,Arg2>(getState(a1,a2),a1,a2); }

struct GreedyLabeling : IGoal
{
	GreedyLabeling(Store& store,const VarArray<IntSet>& vars) : IGoal(),store(store),
			vars(vars) {}
	Goal execute()
	{
		// select maximum unassigned number
		int maxn = limits<int>::min();
		for (uint i = 0; i < vars.size(); ++i)
			for (SetFD<int>::PIterator it = vars[i].domain().beginPoss();
					it != vars[i].domain().endPoss(); ++it)
				if (*it > maxn)
					maxn = *it;

		// find the set with the smallest sum
		int mins = limits<int>::max();
		int minidx = -1;
		for (uint i = 0; i < vars.size(); ++i)
		{
			uint sumIn = 0;
			if (vars[i].domain().findInPoss(maxn)!=vars[i].domain().endPoss())
			{
				for (SetFD<int>::IIterator it = vars[i].domain().beginIn();
						it != vars[i].domain().endIn(); ++it)
					sumIn += *it;
				if (sumIn < mins)
				{
					mins = sumIn;
					minidx = i;
				}
			}
		}
		if (minidx<0)
			return succeed();
		return Goal(store,
				(post(store,member(maxn,vars[minidx])) or post(store,notMember(maxn,vars[minidx]))) and
				Goal(this));
	}
	Store& store;
	VarArray<IntSet> vars;
};

Goal greedyLabeling(Store& store,VarArray<IntSet> vars)
{	return new (store) GreedyLabeling(store,vars);	}

void partition(uint nvalues, uint nsets, int optimum = -2)
{
	Solver solver;
	IntSetVarArray vars(solver,nsets,range(1,nvalues));

//	for (uint i = 0; i < vars.count(); ++i)
//		cout << vars[i].pDomain() << " " << "A" << endl;

#if 1
	for (uint i = 0; i < vars.count(); i++)
		for (uint j = i+1; j < vars.count(); j++)
			solver.post( disjoint(vars(i),vars(j)) );
	/*solver.post(unionEqual(vars,Var<IntSet>(solver,
						List<int>(range(1,nvalues).begin(),range(1,nvalues).end()),
						List<int>())));*/

	VarArray<IntSet> avars(solver,nsets,range(1,nvalues));
	avars[0] = vars[0];
	for (uint i = 0; i < nsets-1; i++)
		solver.post( unionEqual(avars(i),vars(i+1),avars(i+1)) );
	solver.post(avars(nsets-1)==Var<IntSet>(solver,
						Util::StdList<int>(range(1,nvalues).begin(),range(1,nvalues).end()),
						Util::StdList<int>()) );

#else
	solver.post(partition(vars));
#endif

	const uint total = nvalues/2.0*(2+(nvalues-1));
	IntVar maxSums(solver,std::ceil(((float)total)/nsets),total);

	if (optimum==-2)
	{
		const float magicNumberF = ((float)total)/nsets;
		const uint magicNumber = (int)magicNumberF;
		if (magicNumber!=magicNumberF)	{
			std::cout << "trivially impossible!\n";
			return;
		}

		for (uint p = 0; p < nsets; ++p)
			solver.post(sumEqual(vars[p],magicNumber));
	}
	else
	if (optimum==-1)
	{
		// at least one element per set so that we can
		// enforce symmetry breaking constraints below

		IntVarArray sums(solver,nsets,1,total);
		for (uint p = 0; p < nsets; ++p)
			solver.post(sumEqual(vars[p],sums[p]));
		solver.post(max(sums)==maxSums);
		solver.setExplorer(bbMinimize(solver,maxSums));
	}
	else
		for (uint p = 0; p < nsets; ++p)
		{
			IntVar s(solver,1,total);
			solver.post(sumEqual(vars[p],s));
			solver.post(s<=optimum);
		}

	// symmetry breaking: sets are interchangeable
#if 1
	for (uint p = 1; p < nsets; ++p)
		solver.post(min(vars[p])>min(vars[p-1]));
#else
	for (uint p1 = 0; p1 < nsets; ++p1)
		for (uint p2 = p1+1; p2 < nsets; ++p2)
			solver.post( sortSubsetsWithSameSum(vars[p1],vars[p2]));
#endif

//	bool ret = solver.solve( label( vars, selectVarFFRR(vars) ) );
	bool ret = solver.solve( greedyLabeling(solver,vars) );

	if (optimum==-2 or optimum>=0)
		std::cout << ret << std::endl << vars << std::endl;
	else
		while (ret)
		{
			std::cout << vars << " : " << maxSums << " optimimum="
						<< ((float)total)/nsets << std::endl;
			ret = solver.next();
		}
	std::cout << solver.getStats() << std::endl
				 << solver.getCPUTimer() << std::endl;
}


int main(int argc, char** argv)
{
	if (argc != 3 and argc != 4)
	{
		std::cout << "usage: " << argv[0] << " nvalues nsets [optimum]\n";
		std::cout << "set optimum=-2 for perfect partition, optimum=-1 for minimization\n";
		return 1;
	}
	::partition(atoi(argv[1]),atoi(argv[2]),argc==3?-1:atoi(argv[3]));
}
