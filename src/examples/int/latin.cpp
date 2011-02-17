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

#include <casper/int/impacts.h>
#include <casper/int/la.h>
#include <casper/int/sc.h>

#include <iostream>
#include <fstream>

using namespace casper;
using namespace std;

//	Latin Squares. All rows and columns are distinct.
int latin(UInt n, ifstream* pF, bool la, const ExampleOptions& opt)
{
	// create a CPSolver object
	CPSolver solver;

	// create a (square) array of variables with size n X n, where all
	// variables have domains in 0..n-1.
	DomVarArray<Int,2> square(solver,n,n,0,n-1);

	// post constraints to specify that all rows are distinct
	for (UInt r = 0; r < n; r++)
		solver.post(distinct(square[r]),Dom);

	// post constraints to specify that all cols are distinct
	for (UInt c = 0; c < n; c++)
	{
		DomVarArray<Int> col(solver,n);
		for (UInt r = 0; r < n; r++)
			col[r] = square[r][c];
		solver.post(distinct(col),Dom);
	}

	// read partial assignment
	if (pF != NULL)
	{
		Int val;
		for (UInt r = 0; r < n; ++r)
			for (UInt c = 0; c < n; ++c)
			{
				assert(!pF->eof());
				(*pF) >> ws >> val;
				assert(val >= -1 and val < (Int)n);
				if (val>=0)
					solver.post(square[r][c] == val);
			}
	}

	// perform depth first search
	UInt nbSolutions = 0;
	LAInfo<DomVarArray<Int,2> > laInfo(square);
	Bool found;
	if (la)
		found = solver.solve(label(square,rscMinSize(square,2,laInfo),
									selectVarLAMinDomSize(square,laInfo),
									selectValMin(square)));
	else
		found = solver.solve(labelWithImpacts(square));

	while (found)
	{
		cout << square << endl;
		if (++nbSolutions == opt.nbSolutions())
			break;
		found = solver.next();
	}

	// print statistics
	if (opt.showStats())
		cout << solver.stats() << endl;
	if (opt.showRuntime())
		cout << solver.totalTime() << endl;
	return 0;
}

int main(int argc, char** argv)
{
	UInt n = 5;
	std::string f;
	Bool h = 0;

	ExampleOptions opt("Latin Square");
	opt.addOptParam("size",&n,"size of square");
	opt.addOptParam("filename,f",&f,"path to file containing partial assignment (see src/examples/int/data_latin)");
	opt.addOptParam("heuristic",&h,"search heuristic (0=Impacts,1=LookAhead)");

	if (!opt.parse(argc,argv))
		return 1;

	if (!f.empty())
	{
		ifstream i(f);
		char word[1000];
		i >> ws >> word >> ws >> n >> ws; // first row
		latin(n,&i,h,opt);
	}
	else
		latin(n,NULL,h,opt);
}
