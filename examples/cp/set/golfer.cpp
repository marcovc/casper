/***************************************************************************
 *   Copyright (C) 2007 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <casper/cp/set/config.h>

#include <casper/kernel.h>
#include <casper/cp/solver.h>
#include <casper/cp/set.h>
#include <casper/cp/int.h>
#include <casper/util/options.h>

#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

struct SelectVarMinPoss : IVarSelector
{
	SelectVarMinPoss(VarArray<IntSet,2> vars) :
		vars(vars) {}
	int select()
	{
		int minElem = limits<int>::max();
		int minElemVarIdx = -1;
		uint	nvars = vars.count();
		for (uint i = 0; i < nvars; i++)
			if (!vars(i).domain().ground() and
				 *vars(i).domain().beginPoss() < minElem)
			{
				minElem = *vars(i).domain().beginPoss();
				minElemVarIdx = i;
			}
		return minElemVarIdx;
	}
	VarArray<IntSet,2> vars;
};

VarSelector selectVarMinPoss(State& state,VarArray<IntSet,2> vars)
{	return new (state) SelectVarMinPoss(vars);	}

struct SelectValMinPoss : IValSelector
{
	SelectValMinPoss(Casper::CP::Store& store,VarArray<IntSet,2> vars) :
		store(store),vars(vars) {}
	Goal select(uint idx)
	{
		if (vars(idx).ground())
			return succeed();
		int minElem = *vars(idx).domain().beginPoss();

		//cout << vars.solver().stats().curLevel <<  ": selecting var " << idx << " elem " << minElem << endl;
		return Goal(store,
				post(store,member(minElem,vars(idx))) or
				post(store,notMember(minElem,vars(idx))));
	}
	Store&			store;
	VarArray<IntSet,2> vars;
};

ValSelector selectValMinPoss(Store& store,VarArray<IntSet,2> vars)
{	return new (store) SelectValMinPoss(store,vars);	}

// Social golfers
void golfers(uint w, uint g, uint s, uint labeling, const Util::ExampleOptions& opt)
{
	Solver solver;
	IntSetVarArray2 vars(solver,w,g,range(1,g*s));

	for (uint i = 0; i < vars.count(); i++)
		solver.post( cardinal(vars(i)) == s );

	for (uint w1 = 0; w1 < vars.size(0); w1++)
	{
		solver.post( partition(vars[w1]) );

		for (uint w2 = w1+1; w2 < vars.size(0); w2++)
			for (uint g1 = 0; g1 < vars.size(1); g1++)
				for (uint g2 = 0; g2 < vars.size(1); g2++)
					solver.post( cardinal(vars[w1][g1]*vars[w2][g2]) <= 1 );
	}

	// order groups in weeks
	for (uint wi = 0; wi < w; wi++)
		for (uint gi = 0; gi < g-1; gi++)
			solver.post(min(vars[wi][gi]) < min(vars[wi][gi+1]));

	// order weeks
	for (uint wi = 0; wi < w-1; wi++)
		solver.post(min(vars[wi][0]) <= min(vars[wi+1][0]));

	uint nbSolutions = 0;
	bool found;

	if (labeling == 0)
		found = solver.solve( label( solver,vars ) );
	else
	if (labeling == 1)
	{
		IntPar	pi(solver),wi(solver),gi(solver);
		found = solver.solve(forAll(pi,range(1,g*s)) (
								forAll(wi,range(0,w-1)) (
									tryAll(gi,range(0,g-1),
										post(solver,member(pi,vars[wi][gi])),
										post(solver,!member(pi,vars[wi][gi]))
							))));
	}
	else
		found = solver.solve( label( solver,vars, selectVarMinPoss(solver,vars),
										  selectValMinPoss(solver,vars) ));

	while (found)
	{
		cout << vars << endl;
		if (++nbSolutions == opt.nbSolutions())
			break;
		found = solver.next();
	}

	if (opt.showStats())
		cout << solver.getStats() << endl;
	if (opt.showRuntime())
		cout << solver.getCPUTimer() << endl;
}


int main(int argc, char** argv)
{
	uint w = 4;
	uint g = 8;
	uint s = 4;
	uint h = 1;

	Util::ExampleOptions opt("Social Golfers Problem");
	opt.addPosParam("weeks",&w,"number of weeks");
	opt.addPosParam("groups",&g,"number of groups of players");
	opt.addPosParam("size",&s,"number of players in each group");

	opt.addOptParam("heuristic",&h,"search heuristic (0=assign player to week/group/idx,1=assign week/group/pos to player,2=variation of 1)");

	if (!opt.parse(argc,argv))
		return 1;

	golfers(w,g,s,h,opt);

	return 0;
}
