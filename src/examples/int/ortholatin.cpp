/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
 *                                                                         *
 *   Licensed under the Apache License, Version 2.0 (the "License");       *
 *   you may not use this file except in compliance with the License.      *
 *   You may obtain a copy of the License at                               *
 *            http://www.apache.org/licenses/LICENSE-2.0                   *
 *   Unless required by applicable law or agreed to in writing, software   *
 *   distributed under the License is distributed on an                    *
 *   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,          *
 *   either express or implied.                                            *
 *   See the License for the specific language governing permissions and   *
 *   limitations under the License.                                        *
 \*************************************************************************/

#include <casper/kernel.h>
#include <casper/int.h>

#include <iostream>

using namespace casper;
using namespace std;

int ortholatin(int n,const ExampleOptions& opt)
{
	CPSolver solver;

	DomVarArray<Int,2> x1(solver,n,n,0,n-1);
	DomVarArray<Int,2> x2(solver,n,n,0,n-1);

	DomVarArray<Int> z(solver,n*n,0,n*n-1);

	solver.post(distinct(z),Dom);


	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			solver.post(x1[r][c]*n+x2[r][c] == z[r*n+c]);

	// Rows
	for (int r = 0; r < n; r++)
	{
		solver.post( distinct(x1[r]),Dom );
		solver.post( distinct(x2[r]),Dom );
	}

	// Cols
	for (int c = 0; c < n; c++)
	{
		DomVarArray<Int> col1(solver,n,1,n);
		DomVarArray<Int> col2(solver,n,1,n);
		for (int r = 0; r < n; r++)
		{
			col1[r] = x1[r][c];
			col2[r] = x2[r][c];
		}
		solver.post( distinct(col1),Dom );
		solver.post( distinct(col2),Dom );
	}

	// symmetry breaking
	for (int r = 1; r < n; r++)
		solver.post( x2[r] <= x1[r-1] );

	UInt nbSolutions = 0;
	Bool found = solver.solve(label(z,selectVarMinDom(z)));
	while (found)
	{
		cout << x1 << " " << x2 << endl;
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
	UInt n = 5;

	ExampleOptions opt("Orthogonal Latin Squares");
	opt.addPosParam("n",&n,"size of squares");

	if (!opt.parse(argc,argv))
		return 1;

	ortholatin(n,opt);
}
