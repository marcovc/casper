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
#include <casper/cp/solver.h>
#include <casper/cp/int.h>

#include <casper/cp/int/impacts.h>
#include <casper/cp/int/la.h>
#include <casper/cp/int/sc.h>

#include <casper/util/options.h>

#include <iostream>
#include <fstream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

//	Latin Squares. All rows and columns are distinct.
int latin(uint n, ifstream* pF, bool la, const Util::ExampleOptions& opt)
{
	// create a CPSolver object
	Solver solver;

	// create a (square) array of variables with size n X n, where all
	// variables have domains in 0..n-1.
	IntVarArray2 square(solver,n,n,0,n-1);

	// post constraints to specify that all rows are distinct
	for (uint r = 0; r < n; r++)
		solver.post(distinct(square[r]),postDomFilter);

	// post constraints to specify that all cols are distinct
	for (uint c = 0; c < n; c++)
	{
		VarArray<int> col(solver,n);
		for (uint r = 0; r < n; r++)
			col[r] = square[r][c];
		solver.post(distinct(col),postDomFilter);
	}

	// read partial assignment
	if (pF != NULL)
	{
		int val;
		for (uint r = 0; r < n; ++r)
			for (uint c = 0; c < n; ++c)
			{
				assert(!pF->eof());
				(*pF) >> ws >> val;
				assert(val >= -1 and val < (int)n);
				if (val>=0)
					solver.post(square[r][c] == val);
			}
	}

	// perform depth first search
	uint nbSolutions = 0;
	LAInfo<IntVarArray2> laInfo(solver,square);
	bool found;
	if (la)
		found = solver.solve(label(solver,square,rscMinSize(solver,square,2,laInfo),
									selectVarLAMinDomSize(solver,square,laInfo),
									selectValMin(solver,square)));
	else
		found = solver.solve(labelWithImpacts(solver,square));

	while (found)
	{
		cout << square << endl;
		if (++nbSolutions == opt.nbSolutions())
			break;
		found = solver.next();
	}

	// print statistics
	if (opt.showStats())
		cout << solver.getStats() << endl;
	if (opt.showRuntime())
		cout << solver.getCPUTimer() << endl;
	return 0;
}

int main(int argc, char** argv)
{
	uint n = 5;
	std::string f;
	bool h = 0;

	Util::ExampleOptions opt("Latin Square");
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
