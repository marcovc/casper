/***************************************************************************
 *   Copyright (C) 2006/2011 by Marco Correia                              *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <casper/kernel.h>
#include <casper/cp/solver.h>
#include <casper/cp/int.h>
#include <casper/util/options.h>

#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

int allinterval(int n,const Util::ExampleOptions& opt)
{
	Solver solver;
	IntVarArray	v(solver,n,0,n-1);
	IntVarArray	d(solver,n-1,1,n-1);

	for (int i = 0; i < n-1; i++)
		solver.post(d[i] == abs(v[i+1]-v[i]));

	solver.post(distinct(v));
	solver.post(distinct(d));

	// break symmetries
	solver.post(v[0] < v[1]);
	solver.post(d[0] > d[n-2]);

	uint nbSolutions = 0;
	bool found = solver.solve(label(solver,v,selectVarMinDom(solver,v)));
	while (found)
	{
		cout << v << endl;
		if (++nbSolutions == opt.nbSolutions())
			break;
		found = solver.next();
	}
	if (opt.showStats())
		cout << solver.getStats() << endl;
	if (opt.showRuntime())
		cout << solver.getCPUTimer() << endl;
	return 0;
}


int main(int argc, char** argv)
{
	uint n = 12;
	
	Util::ExampleOptions opt("All Intervals");
	opt.addPosParam("n",&n,"number of intervals");

	if (!opt.parse(argc,argv))
		return 1;

	allinterval(n,opt);
}
