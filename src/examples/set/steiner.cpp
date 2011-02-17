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

// Steiner triples
void steiner(UInt n,const ExampleOptions& opt)
{
	UInt t = n*(n-1)/6;

	CPSolver solver;
	DomVarArray<IntSet> vars(solver,t,range(1,n));

	for (UInt i = 0; i < vars.size(); i++)
		solver.post( cardinal(vars[i]) == 3 );

	for (UInt i = 0; i < vars.size(); i++)
		for (UInt j = i+1; j < vars.size(); j++)
			solver.post( cardinal(vars[i]*vars[j]) <= 1 );

	// order triples
	for (UInt i = 1; i < t; ++i)
		solver.post( min(vars[i-1]) <= min(vars[i]));

	UInt nbSolutions = 0;
	Bool found = solver.solve( label( vars /*, selectVarLexRR(vars),selectValsMin(vars)*/ ) );
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
	UInt n = 7;

	ExampleOptions opt("Steiner Triples Problem");
	opt.addPosParam("size",&n,"number of triples");

	if (!opt.parse(argc,argv))
		return 1;

	steiner(n,opt);

	return 0;
}
