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
#include <casper/int.h>

#include <iostream>

using namespace casper;
using namespace std;

int allinterval(int n,const ExampleOptions& opt)
{
	CPSolver solver;
	DomVarArray<Int> v(solver,n,0,n-1);
	DomVarArray<Int> d(solver,n-1,1,n-1);

	for (int i = 0; i < n-1; i++)
		solver.post(d[i] == abs(v[i+1]-v[i]));

	solver.post(distinct(v));
	solver.post(distinct(d));

	// break symmetries
	solver.post(v[0] < v[1]);
	solver.post(d[0] > d[n-2]);

	UInt nbSolutions = 0;
	Bool found = solver.solve(label(v,selectVarMinDom(v)));
	while (found)
	{
		cout << v << endl;
		if (++nbSolutions == opt.nbSolutions())
			break;
		found = solver.next();
	}
	if (opt.showStats())
		cout << solver.stats() << endl;
	if (opt.showRuntime())
		cout << solver.totalTime() << endl;
	return 0;
}


int main(int argc, char** argv)
{
	UInt n = 12;
	
	ExampleOptions opt("All Intervals");
	opt.addPosParam("n",&n,"number of intervals");

	if (!opt.parse(argc,argv))
		return 1;

	allinterval(n,opt);
}
