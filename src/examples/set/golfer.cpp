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

using namespace casper;
using namespace std;

struct SelectVarMinPoss : IVarSelector
{
	SelectVarMinPoss(DomVarArray<IntSet,2> vars) :
		vars(vars) {}
	Int select()
	{
		Int minElem = limits<Int>::max();
		Int minElemVarIdx = -1;
		UInt	nvars = vars.count();
		for (UInt i = 0; i < nvars; i++)
			if (!vars(i).domain().ground() and
				 *vars(i).domain().beginPoss() < minElem)
			{
				minElem = *vars(i).domain().beginPoss();
				minElemVarIdx = i;
			}
		return minElemVarIdx;
	}
	DomVarArray<IntSet,2> vars;
};

VarSelector selectVarMinPoss(DomVarArray<IntSet,2> vars)
{	return new (vars.solver().heap()) SelectVarMinPoss(vars);	}

struct SelectValMinPoss : IValSelector
{
	SelectValMinPoss(DomVarArray<IntSet,2> vars) :
		vars(vars) {}
	Goal select(UInt idx)
	{
		if (vars(idx).ground())
			return ok();
		Int minElem = *vars(idx).domain().beginPoss();

		//cout << vars.solver().stats().curLevel <<  ": selecting var " << idx << " elem " << minElem << endl;
		return member(minElem,vars(idx)) or
			   notMember(minElem,vars(idx));
	}
	DomVarArray<IntSet,2> vars;
};

ValSelector selectValMinPoss(DomVarArray<IntSet,2> vars)
{	return new (vars.solver().heap()) SelectValMinPoss(vars);	}

// Social golfers
void golfers(UInt w, UInt g, UInt s, UInt labeling, const ExampleOptions& opt)
{
	CPSolver solver;
	DomVarArray<IntSet,2> vars(solver,w,g,range(1,g*s));

	for (UInt i = 0; i < vars.count(); i++)
		solver.post( cardinal(vars(i)) == s );

	for (UInt w1 = 0; w1 < vars.size(0); w1++)
	{
		solver.post( partition(vars[w1]) );

		for (UInt w2 = w1+1; w2 < vars.size(0); w2++)
			for (UInt g1 = 0; g1 < vars.size(1); g1++)
				for (UInt g2 = 0; g2 < vars.size(1); g2++)
					solver.post( cardinal(vars[w1][g1]*vars[w2][g2]) <= 1 );
	}

	// order groups in weeks
	for (UInt wi = 0; wi < w; wi++)
		for (UInt gi = 0; gi < g-1; gi++)
			solver.post(min(vars[wi][gi]) < min(vars[wi][gi+1]));

	// order weeks
	for (UInt wi = 0; wi < w-1; wi++)
		solver.post(min(vars[wi][0]) <= min(vars[wi+1][0]));

	UInt nbSolutions = 0;
	Bool found;

	if (labeling == 0)
		found = solver.solve( label( vars ) );
	else
	if (labeling == 1)
	{
		MutVar<Int> pi(solver),wi(solver),gi(solver);
		found = solver.solve(forAll(pi,range(1,g*s),
								forAll(wi,range(0,w-1),
									tryAll(gi,range(0,g-1),
										member(pi,vars[wi][gi]),
										notMember(pi,vars[wi][gi])
							))));
	}
	else
		found = solver.solve( label( vars, selectVarMinPoss(vars),
										  selectValMinPoss(vars) ));

	while (found)
	{
		cout << vars << endl;
		if (++nbSolutions == opt.nbSolutions())
			break;
		found = solver.next();
	}

	if (opt.showStats())
		cout << solver.stats() << endl;
	if (opt.showRuntime())
		cout << solver.totalTime() << endl;
}


int main(int argc, char** argv)
{
	UInt w = 4;
	UInt g = 8;
	UInt s = 4;
	UInt h = 1;

	ExampleOptions opt("Social Golfers Problem");
	opt.addPosParam("weeks",&w,"number of weeks");
	opt.addPosParam("groups",&g,"number of groups of players");
	opt.addPosParam("size",&s,"number of players in each group");

	opt.addOptParam("heuristic",&h,"search heuristic (0=assign player to week/group/idx,1=assign week/group/pos to player,2=variation of 1)");

	if (!opt.parse(argc,argv))
		return 1;

	golfers(w,g,s,h,opt);

	return 0;
}
