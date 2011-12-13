 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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
#include <casper/cp/int.h>
#include <casper/util/options.h>

#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;


void golomb(uint n,uint m,uint g, bool minimize, const Util::ExampleOptions& opt)
{
	// define the CPSolver object
	Solver solver;

	// create an array for storing n marks, each one with the domain [0,m]
    IntVarArray	marks(solver,n,0,m);

	IntRef	i1(solver,0);
	IntRef	i2(solver,0);
	if (g == 0)
		solver.post(distinct(all(Casper::pair(i2,i1),range(0,n-1),i1>i2,
									cache(marks[i1]-marks[i2],(int)1,(int)m))),postValFilter);
	else
	if (g == 1)
		solver.post(distinct(all(Casper::pair(i2,i1),range(0,n-1),i1>i2,
									cache(marks[i1]-marks[i2],(int)1,(int)m))),postBndFilter);
	else
	if (g == 2)
		solver.post(distinct(all(Casper::pair(i2,i1),range(0,n-1),i1>i2,
									cache(marks[i1]-marks[i2],(int)1,(int)m))),postDomFilter);

	// break some symmetries
    for (uint i = 0; i < n-1; i++)
    	solver.post(marks[i] < marks[i+1]);

    marks[0].domain() = 0;

	// setup branch and bound
	if (minimize)
		solver.setExplorer(bbMinimize(solver,marks[n-1]));
	else
		marks[n-1].domain() = m;

	// search
	uint nbSolutions = 0;
	bool found = solver.solve(label(solver,marks,selectVarLex(solver,marks)));

	while (found)
	{
		// show values for marks
		cout << marks << endl;

		if (!minimize and ++nbSolutions == opt.nbSolutions())
			break;

		found = solver.next();
	}

    // show statistics
	if (opt.showStats())
		cout << solver.getStats() << endl;
	if (opt.showRuntime())
		cout << solver.getCPUTimer() << endl;
}


int main(int argc, char** argv)
{
	uint n = 10,l = 1000,dcons = 1;
	bool minimize = true;

	Util::ExampleOptions opt("Golomb Ruler");
	opt.addPosParam("n",&n,"number of marks");
	opt.addPosParam("l",&l,"maximum length");
	opt.addOptParam("minimize,m",&minimize,"try to minimize l");
	opt.addOptParam("distinct",&dcons,
			"consistency for distinct constraint (0=Val,1=Bnd,2=Dom)");

	if (!opt.parse(argc,argv))
		return 1;

	golomb(n,l,dcons,minimize,opt);

	return 0;
}
