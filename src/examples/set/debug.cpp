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

#include <casper/kernel/container/pair.h>
#include <casper/kernel/container/sulist.h>

#include <casper/kernel.h>
#include <casper/set.h>
#include <casper/int.h>

#include <casper/kernel/debug.h>

#include <iostream>

using namespace casper;
using namespace std;



namespace casper {
UInt cninserts = 0;
UInt cavginsertsize = 0;
UInt cnremoves = 0;
UInt cavgremovesize = 0;
}

#if 0
void testDisjoint(UInt n)
{
	ICPSolver& solver;
	DomVarArray<IntSet> vars(solver,n,range(1,n));

/*	for (UInt i = 0; i < vars.size(); i++)
		solver.post( cardinal(vars[i]) <= 1 );
*/
	for (UInt i = 0; i < vars.size(); i++)
		for (UInt j = i+1; j < vars.size(); j++)
			solver.post( disjoint(vars[i],vars[j]) );

	Bool ret = solver.solve();

	//cout << solver.solve( label( vars, selectVarLexRR(vars),bisectValMin(vars) ) ) << endl;
	ret = ret and solver.solve( label( vars ) );

	while (ret)
	{
		cout << vars << endl;
		ret = solver.next();
	}
}
#endif

void testContained(UInt n)
{
	ICPSolver& solver;
	DomVarArray<IntSet> vars(solver,n,range(1,n));

/*	for (UInt i = 0; i < vars.size(); i++)
		solver.post( cardinal(vars[i]) <= 1 );
*/
	for (UInt i = 1; i < vars.size(); i++)
			solver.post( contained(vars[i],vars[i-1]) );

	Bool ret = solver.solve();

	//cout << solver.solve( label( vars, selectVarLexRR(vars),bisectValMin(vars) ) ) << endl;
	ret = ret and solver.solve( label( vars ) );

	while (ret)
	{
		cout << vars << endl;
		ret = solver.next();
	}
}

#if 0
void testIntersect(UInt n)
{
	ICPSolver& solver;
	DomVarArray<IntSet> vars(solver,3,range(1,n));

	solver.post( vars[0] * vars[1] == vars[2] );

	Bool ret = solver.solve();

	//cout << solver.solve( label( vars, selectVarLexRR(vars),bisectValMin(vars) ) ) << endl;
	ret = ret and solver.solve( label( vars ) );

	while (ret)
	{
		cout << vars << endl;
		ret = solver.next();
	}
}

void testUnion(UInt n)
{
	ICPSolver& solver;
	DomVarArray<IntSet> vars(solver,3,range(1,n));

	solver.post( union_(vars[0],vars[1],vars[2] ) );

	Bool ret = solver.solve();

	cout << vars << endl;
	//cout << solver.solve( label( vars, selectVarLexRR(vars),bisectValMin(vars) ) ) << endl;
	ret = ret and solver.solve( label( vars ) );

	while (ret)
	{
		cout << vars << endl;
		ret = solver.next();
	}
}


void testPairs(UInt n)
{
	ICPSolver& solver;
	typedef detail::Pair<Int>	IntPair;
	typedef Set<IntPair>		PairSet;

	detail::SList<IntPair> l;
	l.pushBack(IntPair(0,-6));
	l.pushBack(IntPair(1,3));
	l.pushBack(IntPair(5,2));
	DomVarArray<PairSet>	vars(solver,3,l.begin(),l.end());
	cout << vars << endl;

	solver.post(union_(vars[0],vars[1],vars[2]));

	Bool found = solver.solve();
	while (found)
	{
		cout << vars << endl;
		found = solver.next();
	}
	cout << solver.stats() << endl;
	cout << solver.totalTime() << endl;
}
#endif

/*
void test()
{
	ICPSolver& solver;

	typedef detail::Pair<Int> IntPair;

	DomVar<Set<IntPair> >	v(solver,list(IntPair(0,1),IntPair(5,9)));

	MutVar<IntPair>	i(solver);
	Bool found = solver.solve();

	found = found and solver.solve(forAll(i,detail::List<IntPair>(stdHeap,v.domain().beginPoss(),
											 v.domain().endPoss()),
											 //v.domain().poss(),
								member(i,v) or notMember(i,v)
							));

	while (found)
	{
		cout << v << endl;
		found = solver.next();
	}
	cout << solver.stats() << endl;
	cout << solver.totalTime() << endl;
}*/

void testCard(UInt n, UInt c)
{
	ICPSolver& solver;
	DomVarArray<IntSet> vars(solver,n,range(1,n));

	for (UInt i = 0; i < vars.size(); i++)
		solver.post( cardinal(vars[i]) == c );

	cout << solver.solve() << endl;

	Bool ret = solver.solve( label( vars ) );

	while (ret)
	{
		cout << vars << endl;
		ret = solver.next();
	}
	cout << solver.stats() << endl
		 << solver.totalTime() << endl;
}

void stressMember(UInt nvars, UInt nelems, UInt nconstraints)
{
	ICPSolver& solver;
	DomVarArray<IntSet> vars(solver,nvars,range(0,nelems-1));

	for (UInt i = 0; i < nconstraints; i++)
		solver.post( member(Int(rand()/(Double)RAND_MAX * nelems),vars[i%nvars]) );

	Timer solvingTime("solving time");
	cout << solver.solve() << endl;
	solvingTime.pause();

	cout << solver.stats() << endl
		 << solver.totalTime() << endl;
	cout << solvingTime << endl;
}

/*
struct ShowDom : IBreakPred
{
	ShowDom(DomVar<IntSet>& v, string message) : v(v),message(message) {}
	Bool eval() const
	{	std::cout << message << v << std::endl; return false; }
	DomVar<IntSet>& v;
	string	message;
};

struct ShowDeltaDom : IBreakPred
{
	ShowDeltaDom(DomVar<IntSet> v) : v(v) {}
	Bool eval() const
	{
		detail::SList<Int>	deltaEraseIn;
		setDifference(lastIn.begin(),lastIn.end(),v.domain().beginIn(),v.domain().endIn(),
				detail::SListInserter<Int>(deltaEraseIn));
		detail::SList<Int>	deltaInsertIn;
		setDifference(v.domain().beginIn(),v.domain().endIn(),lastIn.begin(),lastIn.end(),
				detail::SListInserter<Int>(deltaInsertIn));
		detail::SList<Int>	deltaErasePoss;
		setDifference(lastPoss.begin(),lastPoss.end(),v.domain().beginPoss(),v.domain().endPoss(),
				detail::SListInserter<Int>(deltaErasePoss));
		detail::SList<Int>	deltaInsertPoss;
		setDifference(v.domain().beginPoss(),v.domain().endPoss(),lastPoss.begin(),lastPoss.end(),
				detail::SListInserter<Int>(deltaInsertPoss));
		detail::SList<Int>	deltaEraseCard;
		setDifference(lastCard.begin(),lastCard.end(),v.domain().card().begin(),v.domain().card().end(),
				detail::SListInserter<Int>(deltaEraseCard));
		detail::SList<Int>	deltaInsertCard;
		setDifference(v.domain().card().begin(),v.domain().card().end(),lastCard.begin(),lastCard.end(),
				detail::SListInserter<Int>(deltaInsertCard));

		if (deltaInsertIn.empty() and deltaEraseIn.empty() and
			deltaInsertPoss.empty() and deltaInsertPoss.empty() and
			deltaInsertCard.empty() and deltaEraseCard.empty())
			return false;

		std::cout << "Deltas for variable " << v << std::endl;

		if (!deltaInsertIn.empty())
		{
			std::cout << "in <-- " << deltaInsertIn << std::endl;
			lastIn.clear();
			lastIn.insert(v.domain().beginIn(),v.domain().endIn());
		}
		if (!deltaEraseIn.empty())
		{
			std::cout << "in --> " << deltaEraseIn << std::endl;
			lastIn.clear();
			lastIn.insert(v.domain().beginIn(),v.domain().endIn());
		}
		if (!deltaInsertPoss.empty())
		{
			std::cout << "poss <-- " << deltaInsertPoss << std::endl;
			lastPoss.clear();
			lastPoss.insert(v.domain().beginPoss(),v.domain().endPoss());
		}
		if (!deltaErasePoss.empty())
		{
			std::cout << "poss --> " << deltaErasePoss << std::endl;
			lastPoss.clear();
			lastPoss.insert(v.domain().beginPoss(),v.domain().endPoss());
		}
		if (!deltaInsertCard.empty())
		{
			std::cout << "card <-- " << deltaInsertCard << std::endl;
			lastCard.clear();
			lastCard.insert(v.domain().card().begin(),v.domain().card().end());
		}
		if (!deltaEraseCard.empty())
		{
			std::cout << "card --> " << deltaEraseCard << std::endl;
			lastCard.clear();
			lastCard.insert(v.domain().card().begin(),v.domain().card().end());
		}


		return false;
	}
	DomVar<IntSet> v;
	mutable detail::SUList<Int>	lastIn;
	mutable detail::SUList<Int>	lastPoss;
	mutable detail::SUList<Int>	lastCard;
};

struct ShowDoms : IBreakPred
{
	ShowDoms(DomVarArray<IntSet> v,String msg) : v(v),msg(msg) {}
	Bool eval() const
	{
		for (UInt i = 0; i < v.count(); i++)
		{
			std::cout << v(i) << std::endl;

		}
		return false;
	}
	DomVarArray<IntSet> v;
	String msg;
};

struct ShowDeltaDoms : IBreakPred
{
	ShowDeltaDoms(DomVarArray<IntSet> v) : s(v.count(),(ShowDeltaDom*)0)
	{
		for (UInt i = 0; i < v.count(); i++)
			s[i] = new ShowDeltaDom(v(i));
	}
	Bool eval() const
	{
		for (UInt i = 0; i < s.size(); i++)
			s[i]->eval();
		return false;
	}

	Array<ShowDeltaDom*>	s;
};
*/

void stressIntersect(UInt nvars,UInt nvals,UInt nconstraints)
{
	ICPSolver& solver;

	DomVarArray<IntSet> vars(solver,nvars,range(0,nvals-1));

	for (UInt i = 1; i < nvars; i++)
		for (UInt j = 0; j < nvals; j++)
			if (rand()/(Double)RAND_MAX < 0.5)
				solver.post( member(j,vars[i]) );
			else
				solver.post( notMember(j,vars[i]) );
/*
	ShowDom sd(vars[0],"after restore: ");
	ShowDoms sd2(vars,"vars");
	ShowDeltaDoms sd3(vars);

	Debug::showOn( Debug::Event(Debug::onFail) and (IBreakPred*)&sd);
	Debug::showOn( Debug::ObjMatch("Mul"));
	Debug::showOn( Debug::ObjMatch("Card"));
	Debug::showOn( Debug::ObjMatch("Contained"));
	Debug::showOn( (IBreakPred*)&sd3);
	*/
	UInt v1;
	UInt v2;
	UInt v3;

	DomVarArray<IntSet> inter(solver,nvars,range(0,nvals-1));

/*	ShowDeltaDoms sd4(inter);
	Debug::showOn( (IBreakPred*)&sd4);
	ShowDoms sd5(inter,"inters");
	Debug::showOn( Debug::Event(Debug::onFilter) and (IBreakPred*)&sd2 );
	Debug::showOn( Debug::Event(Debug::onFilter) and (IBreakPred*)&sd5 );
	*/
	//for (UInt i = 0; i < nconstraints; i++)
	{
		/*do {
			v1 = static_cast<UInt>(rand()/(Double)RAND_MAX * nvars);
			v2 = static_cast<UInt>(rand()/(Double)RAND_MAX * nvars);
			v3 = static_cast<UInt>(rand()/(Double)RAND_MAX * nvars);
		} while (v1 == v2 or v1 == v3 or v2 == v3); // Arrrgghhh*/



		for (UInt j = 1; j < nvars; j++)
		{
			solver.post( intersect(vars[0],vars[j],inter[j]));
			solver.post( cardinal(inter[j])>0 );
		}
	}


	solver.setGoalSched(bbMinimize(solver,cardinal(vars[0])));

	Bool ret = solver.solve( label( vars ) );
	UInt sols=0;
	while (ret)
	{
		cout << solver.stats() << endl;

		//<< solver.totalTime() << endl;
		cout << vars << endl;
		ret = solver.next();
	}

	//cout << solver.stats() << endl;
			 //<< solver.totalTime() << endl;
	//cout << solvingTime << endl;
}

void stressContained(UInt nvars,UInt nvals,UInt nconstraints)
{
	ICPSolver& solver;

	DomVarArray<IntSet> vars(solver,nvars,range(0,nvals-1));

	UInt v1;
	UInt v2;
	UInt v3;
	for (UInt i = 0; i < nconstraints; i++)
	{
		do {
			v1 = static_cast<UInt>(rand()/(Double)RAND_MAX * nvars);
			v2 = static_cast<UInt>(rand()/(Double)RAND_MAX * nvars);
		} while (v1 == v2); // Arrrgghhh
		solver.post( contained(vars[v1],vars[v2]) );
	}


	Timer solvingTime("solving time");
	Bool ret = solver.solve();

	ret = ret and solver.solve( label( vars,selectValsRand(vars)  ) );

	solvingTime.pause();

	while (ret)
	{
		//cout << vars << endl;

		cout << solver.stats() << endl
					 << solver.totalTime() << endl;
		ret = solver.next();
	}

	cout << solvingTime << endl;
}

namespace casper { Timer firstProps("firstProps",false); }


struct ConjArray : IFilter
{
	ConjArray(ICPSolver& solver,Filter f, detail::List<Filter> c,
			DomVar<IntSet> v) :
		IFilter(solver),f(f),c(c),v(v){}

	Bool execute()
	{
		if (f.entailed())
		{
			for (detail::List<Filter>::Iterator it = c.begin();
					it != c.end(); ++it)
				if (!solver().post( *it ))
					return false;
			return true;
		}
		for (detail::List<Filter>::Iterator it = c.begin();
				it != c.end(); ++it)
			if (!it->entailed())
				return true;
		return solver().post( f );
	}
	Void attach(INotifiable* p)
	{
		v.domain().attachOnDomain(p);
		for (detail::List<Filter>::Iterator it = c.begin();
				it != c.end(); ++it)
			it->attach(p);
	}
	Void detach(INotifiable* p)
	{}
	Filter 			f;
	detail::List<Filter> 	c;
	DomVar<IntSet>  v;
};

Filter conj(Filter f, detail::List<Filter> c,DomVar<IntSet> v)
{	return new (f.solver().heap()) ConjArray(f.solver(),f,c,v);	}

struct Disj : IFilter
{
	Disj(ICPSolver& solver, DomVar<Int> r, DomVar<Int> l, DomVar<Int> h) :
		IFilter(solver),r(r),l(l),h(h) {}

	Bool execute()
	{
		Bool s =
		r.domain().updateMin(std::min(l.domain().min(),h.domain().min()+1)) and
		r.domain().updateMax(std::max(l.domain().max(),h.domain().max()+1));

		if (r.domain().max() < h.domain().min()+1 or
			r.domain().min() > h.domain().max()+1)
			s = s and l.domain().updateMin(r.domain().min()) and
				 l.domain().updateMax(r.domain().max());
		else
		if (r.domain().max() < l.domain().min() or
			r.domain().min() > l.domain().max())
			s = s and h.domain().updateMin(r.domain().min()-1) and
				 h.domain().updateMax(r.domain().max()-1);
		return s;
	}

	Void attach(INotifiable* f)
	{
		r.domain().attachOnBounds(f);
		l.domain().attachOnBounds(f);
		h.domain().attachOnBounds(f);
	}
	Void detach(INotifiable* f)
	{}
	DomVar<Int> r;
	DomVar<Int> l;
	DomVar<Int> h;
};

Filter disj(DomVar<Int> r, DomVar<Int> l, DomVar<Int> h)
{	return new (r.solver().heap()) Disj(r.solver(),r,l,h);	}

void testBDDDecomp()
{
	ICPSolver& solver;

	DomVar<Int> l1(solver,0,4);
	DomVar<Int> l21(solver,0,4);
	DomVar<Int> l22(solver,0,4);
	DomVar<Int> l3(solver,0,4);
	DomVar<Int> l41(solver,0,4);
	DomVar<Int> l42(solver,0,4);

	DomVar<IntSet>	x(solver,range(1,2));
	DomVar<IntSet>	y(solver,range(1,2));

	solver.post( disj(l1,l21,l22) );

	solver.post( disj(l21,l3,l3) );
	solver.post( l22 == l3 + 1 );

	solver.post( disj(l3,l41,l42) );

	solver.post( l41 >= 0 );
	solver.post( l41 <= 1 );

	solver.post( l42 >= 1 );
	solver.post( l42 <= 1 );

	//solver.post(conj(member(1,x), list(Filter(l1 >= l22+1)),x));
	//solver.post(conj(notMember(1,x), list(Filter(l1 <= l21)),x));

	solver.post(conj(member(1,y), list(Filter(l1 >= l22+1)),y));

	std::cout << solver.solve() << std::endl;

	std::cout << "Before: " << endl;
	std::cout << l1 << std::endl;
	std::cout << l21 << " " << l22 << std::endl;
	std::cout << l3 << std::endl;
	std::cout << l41 << " " << l42 << std::endl;
	std::cout << x << " " << y << std::endl;

	Goal search = member(1,x) or notMember(1,x);

	Bool found = solver.solve( search );

	while (found)
	{
		std::cout << "Found: " << endl;
		std::cout << l1 << std::endl;
		std::cout << l21 << " " << l22 << std::endl;
		std::cout << l3 << std::endl;
		std::cout << l41 << " " << l42 << std::endl;
		std::cout << x << " " << y << std::endl;
		found = solver.next();
	}

}

int main(int argc,char** argv)
{
	testBDDDecomp();
	return 0;

	if (argc != 4)
	{
		std::cout << "usage: " << argv[0]
		          << " nvars nelems nconstraints\n";
		return 1;
	}


	//Debug::ObjMatch("Heap"));

	//stressMember(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]));
	//stressIntersect(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]));
	//stressContained(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]));

	std::cout << firstProps << std::endl;

/*	ICPSolver& solver;
	DomVar<Set<Int> > v(solver,list(1,2,5,7));
	cout << v << endl;
	cout << (v.domain().findInPoss(2) != v.domain().endPoss()) << endl;
	cout << v.domain().erase(2) << endl;
	cout << v.domain().erase(3) << endl;
	cout << v << endl;
	*/

	//test();
	//testCard(4,2);
	//testContained(3);
	//testDisjoint(3);
	//testIntersect(4);
	//testPairs(3);

	//testIntersect(4);

	//testUnion(4);

	return 0;
}

