/***************************************************************************
 *   Copyright (C) 2007 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <casper/set/config.h>

#include <casper/kernel.h>
#include <casper/set.h>
#include <casper/int.h>
#include <iostream>

#include <casper/set/weight.h>

#include <boost/math/special_functions/factorials.hpp>

using namespace casper;
using namespace std;

#if 0
/**
 * Computes the set of all n-ary sums of the elements in a set domain variable.
 * \note needs testing
 */
template<class Arg1,class Arg2>
struct NSumsEqual : IFilter
{
	typedef typename DomView<Set<Int>,Arg1>::Dom	DomX;
	typedef typename DomView<Set<Int>,Arg2>::Dom	DomY;

	NSumsEqual(ICPSolver& solver,const Arg1& xx, const Arg2& yy, const UInt& n) :
				  IFilter(solver),x(solver,xx),y(solver,yy),n(n) {}

	Bool execute();
	Void attach(INotifiable* f)
	{	x->attachOnDomain(f); y->attachOnDomain(f);	}
	Void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f);	}

	Cost cost() const
	{	return quadraticLo;	}

	template<class Iteration>
	detail::List<Int> getSums(Iteration it, UInt tsize);
	template<class Iteration>
	detail::List<Int> getSumsSorted(Iteration it, UInt tsize);

	DomView<Set<Int>,Arg1>			x;
	DomView<Set<Int>,Arg2>			y;
	const UInt						n;
};

template<class Arg1,class Arg2>
template<class Iteration>
detail::List<Int> NSumsEqual<Arg1,Arg2>::getSums(Iteration it, UInt tsize)
{
	detail::List<Int> ret;
	if (tsize == 1)
	{
		for ( ; it.valid(); it.iterate())
			ret.pushBack(it.value());
		return ret;
	}
	UInt c = 0;
	detail::List<Int> sums_1(getSums(it,tsize-1));
	for (detail::List<Int>::Iterator vit = sums_1.begin(); vit != sums_1.end(); ++vit)
	{
		Iteration iit(it);
		for (UInt i = 0; iit.valid() and i < tsize-1; ++i)
			iit.iterate();
		for (UInt i = 0; iit.valid() and i < c; ++i)
			iit.iterate();
		for ( ; iit.valid(); iit.iterate())
			ret.pushBack(iit.value()+*vit);
		++c;
	}
	return ret;
}

template<class Arg1,class Arg2>
template<class Iteration>
detail::List<Int> NSumsEqual<Arg1,Arg2>::getSumsSorted(Iteration it, UInt tsize)
{
	detail::List<Int> sumsUnsorted(getSums(it,tsize));
	std::set<int> sumsV(sumsUnsorted.begin(),sumsUnsorted.end());
	detail::List<Int> ret(sumsV.begin(),sumsV.end());
	return ret;
}

template<class Arg1,class Arg2>
Bool NSumsEqual<Arg1,Arg2>::execute()
{
//	std::cout << "begin: " << *x << " " << *y << std::endl;
	if (x->inSize() >= n)
		if (!detail::setSafeInsertRange(*y,makeIt(getSumsSorted(makeInIt(*x),n))))
			return false;

	if (x->inSize()+x->possSize() >= n)
		if (!detail::setSafeEraseRange(*y,makeDiffIt(makePossIt(*y),
										  makeIt(getSumsSorted(makeLUBIt(*x),n)))))
			return false;
//	std::cout << "end: " << *x << " " << *y << std::endl;
	// not sure if processing "in y" events makes sense. ignoring for now.

	// if an element r is removed from y then we remove from x.poss all e such that
	// e+s=r where s is a possible (n-1) sum already in x.in
/*	std::set<Int> inSums_1(getSums(makeInIt(*x),n-1));
	for(typename DomX::PIterator pit = x->beginPoss(); pit != x->endPoss(); )
	{
		for (std::set<Int>::iterator it = inSums_1.begin(); it != inSums_1.end(); ++it)
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
Filter nSumsEqual(const Arg1& a1, const Arg2& a2, const UInt& n)
{ return new (getCPSolver(a1,a2).heap()) NSumsEqual<Arg1,Arg2>(getCPSolver(a1,a2),a1,a2,n); }
#endif



template<class Arg1,class Arg2>
struct SortSubsetsWithSameSum : IFilter
{
	typedef typename DomView<Set<Int>,Arg1>::Dom	DomX;
	typedef typename DomView<Set<Int>,Arg2>::Dom	DomY;

	SortSubsetsWithSameSum(ICPSolver& solver,const Arg1& xx, const Arg2& yy) :
				  IFilter(solver),x(solver,xx),y(solver,yy) {}

	Bool execute();
	Void attach(INotifiable* f)
	{	x->attachOnGLB(f); y->attachOnGLB(f);	}
	Void detach(INotifiable* f)
	{	x->detachOnGLB(f); y->detachOnGLB(f);	}

	Cost cost() const
	{	return quadraticLo;	}

	struct Cmp {
		Bool operator()(const detail::Pair<Int>& s1,const detail::Pair<Int>& s2) const
		{
			if (s1.first < s2.first)
				return true;
			if (s1.first > s2.first)
				return false;
			return s1.second < s2.second;
		}
	};

	typedef detail::List<detail::Pair<Int> > Sums;
	typedef std::set<detail::Pair<Int>,Cmp> AllSums;

	template<class Iteration>
	Sums getSums(Iteration it, const Sums&, UInt tsize);
	template<class Iteration>
	AllSums getSums(Iteration it, UInt tsize);

	DomView<Set<Int>,Arg1>			x;
	DomView<Set<Int>,Arg2>			y;
};

template<class Arg1,class Arg2>
template<class Iteration>
typename SortSubsetsWithSameSum<Arg1,Arg2>::Sums
SortSubsetsWithSameSum<Arg1,Arg2>::getSums(Iteration it,const Sums& sums_1,
												UInt tsize)
{
	assert(tsize>1);
	Sums ret;
	UInt c = 0;
	for (Sums::Iterator vit = sums_1.begin(); vit != sums_1.end(); ++vit)
	{
		Iteration iit(it);
		for (UInt i = 0; iit.valid() and i < tsize-1; ++i)
			iit.iterate();
		for (UInt i = 0; iit.valid() and i < c; ++i)
			iit.iterate();
		for ( ; iit.valid(); iit.iterate())
			ret.pushBack(detail::Pair<Int>(iit.value()+vit->first,iit.value()));
		++c;
	}
	return ret;
}

template<class Arg1,class Arg2>
template<class Iteration>
typename SortSubsetsWithSameSum<Arg1,Arg2>::AllSums
SortSubsetsWithSameSum<Arg1,Arg2>::getSums(Iteration it,UInt tsize)
{
	AllSums all;

	Sums* xold = new Sums();
	for (Iteration iit(it) ; iit.valid(); iit.iterate())
	{
		xold->pushBack(detail::Pair<Int>(iit.value(),iit.value()));
		all.insert(detail::Pair<Int>(iit.value(),iit.value()));
	}

	for (UInt i = 2; i <= tsize; ++i)
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
Bool SortSubsetsWithSameSum<Arg1,Arg2>::execute()
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
{ return new (getCPSolver(a1,a2).heap()) SortSubsetsWithSameSum<Arg1,Arg2>(getCPSolver(a1,a2),a1,a2); }

struct GreedyLabeling : IGoal
{
	GreedyLabeling(const DomVarArray<IntSet>& vars) : IGoal(vars.solver()),
			vars(vars) {}
	Goal execute()
	{
		// select maximum unassigned number
		Int maxn = limits<Int>::min();
		for (UInt i = 0; i < vars.size(); ++i)
			for (SetFD<Int>::PIterator it = vars[i].domain().beginPoss();
					it != vars[i].domain().endPoss(); ++it)
				if (*it > maxn)
					maxn = *it;

		// find the set with the smallest sum
		Int mins = limits<Int>::max();
		Int minidx = -1;
		for (UInt i = 0; i < vars.size(); ++i)
		{
			UInt sumIn = 0;
			if (vars[i].domain().findInPoss(maxn)!=vars[i].domain().endPoss())
			{
				for (SetFD<Int>::IIterator it = vars[i].domain().beginIn();
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
			return ok();
		return (member(maxn,vars[minidx]) or notMember(maxn,vars[minidx])) and
				Goal(this);
	}
	DomVarArray<IntSet> vars;
};

Goal greedyLabeling(DomVarArray<IntSet> vars)
{	return new (vars.solver().heap()) GreedyLabeling(vars);	}

void partition(UInt nvalues, UInt nsets, Int optimum = -2)
{
	CPSolver solver;
	DomVarArray<IntSet> vars(solver,nsets,range(1,nvalues));

//	for (UInt i = 0; i < vars.count(); ++i)
//		cout << vars[i].pDomain() << " " << "A" << endl;

#if 1
	for (UInt i = 0; i < vars.count(); i++)
		for (UInt j = i+1; j < vars.count(); j++)
			solver.post( disjoint(vars(i),vars(j)) );
	/*solver.post(unionEqual(vars,DomVar<IntSet>(solver,
						detail::List<Int>(range(1,nvalues).begin(),range(1,nvalues).end()),
						detail::List<Int>())));*/

	DomVarArray<IntSet> avars(solver,nsets,range(1,nvalues));
	avars[0] = vars[0];
	for (UInt i = 0; i < nsets-1; i++)
		solver.post( unionEqual(avars(i),vars(i+1),avars(i+1)) );
	solver.post(avars(nsets-1)==DomVar<IntSet>(solver,
						detail::List<Int>(range(1,nvalues).begin(),range(1,nvalues).end()),
						detail::List<Int>()) );

#else
	solver.post(partition(vars));
#endif

	const UInt total = nvalues/2.0*(2+(nvalues-1));
	DomVar<Int> maxSums(solver,std::ceil(((Float)total)/nsets),total);

	if (optimum==-2)
	{
		const Float magicNumberF = ((Float)total)/nsets;
		const UInt magicNumber = (Int)magicNumberF;
		if (magicNumber!=magicNumberF)	{
			std::cout << "trivially impossible!\n";
			return;
		}

		for (UInt p = 0; p < nsets; ++p)
			solver.post(sumEqual(vars[p],magicNumber));
	}
	else
	if (optimum==-1)
	{
		// at least one element per set so that we can
		// enforce symmetry breaking constraints below

		DomVarArray<Int> sums(solver,nsets,1,total);
		for (UInt p = 0; p < nsets; ++p)
			solver.post(sumEqual(vars[p],sums[p]));
		solver.post(max(sums)==maxSums);
		solver.setGoalSched(bbMinimize(solver,maxSums));
	}
	else
		for (UInt p = 0; p < nsets; ++p)
		{
			DomVar<Int> s(solver,1,total);
			solver.post(sumEqual(vars[p],s));
			solver.post(s<=optimum);
		}

	// symmetry breaking: sets are interchangeable
#if 1
	for (UInt p = 1; p < nsets; ++p)
		solver.post(min(vars[p])>min(vars[p-1]));
#else
	for (UInt p1 = 0; p1 < nsets; ++p1)
		for (UInt p2 = p1+1; p2 < nsets; ++p2)
			solver.post( sortSubsetsWithSameSum(vars[p1],vars[p2]));
#endif

//	Bool ret = solver.solve( label( vars, selectVarFFRR(vars) ) );
	Bool ret = solver.solve( greedyLabeling(vars) );

	if (optimum==-2 or optimum>=0)
		cout << ret << endl << vars << endl;
	else
		while (ret)
		{
			cout << vars << " : " << maxSums << " optimimum="
						<< ((Float)total)/nsets << endl;
			ret = solver.next();
		}
	cout << solver.stats() << endl
				 << solver.totalTime() << endl;
}


int main(int argc, char** argv)
{
	if (argc != 3 and argc != 4)
	{
		cout << "usage: " << argv[0] << " nvalues nsets [optimum]\n";
		cout << "set optimum=-2 for perfect partition, optimum=-1 for minimization\n";
		return 1;
	}
	::partition(atoi(argv[1]),atoi(argv[2]),argc==3?-1:atoi(argv[3]));
}
