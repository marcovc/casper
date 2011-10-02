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
#include <casper/util/options.h>

#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

// Steiner triples
void steiner(uint n,const Util::ExampleOptions& opt)
{
	uint t = n*(n-1)/6;

	Solver solver;
	IntSetVarArray	vars(solver,t,range(1,n));

	for (uint i = 0; i < vars.size(); i++)
		solver.post( cardinal(vars[i]) == 3 );

	for (uint i = 0; i < vars.size(); i++)
		for (uint j = i+1; j < vars.size(); j++)
			solver.post( cardinal(vars[i]*vars[j]) <= 1 );

	// order triples
	for (uint i = 1; i < t; ++i)
		solver.post( min(vars[i-1]) <= min(vars[i]));

	uint nbSolutions = 0;
	bool found = solver.solve( label( solver,vars /*, selectVarLexRR(vars),selectValsMin(vars)*/ ) );
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
	uint n = 7;

	Util::ExampleOptions opt("Steiner Triples Problem");
	opt.addPosParam("size",&n,"number of triples");

	if (!opt.parse(argc,argv))
		return 1;

	steiner(n,opt);

	return 0;
}
