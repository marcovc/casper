/***************************************************************************
 *   Copyright (C) 2007/2010 by Marco Correia                              *
 *   mvc@di.fct.unl.pt                               					   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <iostream>
#include <casper/real.h>

using namespace casper;
using namespace std;

int main()
{
	CPSolver solver;
	DomVarArray<Double> y(solver,2,-1,1);

	solver.post( y[0]*y[0]/y[1] + y[1]*y[1]/y[0] == 2.0 );
	solver.post( y[1] == exp(-y[0]) );

	cout << solver.valid() << endl;
	cout << "before search: " << y << endl;

	bool found = solver.solve( label(y,selectVarLex(y),
								  selectHalfMin(y)) );

	cout.precision(17);

	UInt solutions = 0;
	while (found)
	{
		cout << "solution " << ++solutions << ":" << y << endl;
		found = solver.next();
	}
	return 0;
}

