 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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


void golomb(UInt n,UInt m,UInt g, Bool minimize, const ExampleOptions& opt)
{
	// define the CPSolver object
	CPSolver solver;

	// create an array for storing n marks, each one with the domain [0,m]
    DomVarArray<Int> marks(solver,n,0,m);

	MutVar<Int> i1(solver,0);
	MutVar<Int> i2(solver,0);
	if (g == 0)
		solver.post(distinct(all(casper::pair(i2,i1),range(0,n-1),i1>i2,
									cache(marks[i1]-marks[i2],(Int)1,(Int)m))),Val);
	else
	if (g == 1)
		solver.post(distinct(all(casper::pair(i2,i1),range(0,n-1),i1>i2,
									cache(marks[i1]-marks[i2],(Int)1,(Int)m))),Bnd);
	else
	if (g == 2)
		solver.post(distinct(all(casper::pair(i2,i1),range(0,n-1),i1>i2,
									cache(marks[i1]-marks[i2],(Int)1,(Int)m))),Dom);

	// break some symmetries
    for (UInt i = 0; i < n-1; i++)
    	solver.post(marks[i] < marks[i+1]);

    marks[0].domain() = 0;

	// setup branch and bound
	if (minimize)
		solver.setGoalSched(bbMinimize(solver,marks[n-1]));
	else
		marks[n-1].domain() = m;

	// search
	UInt nbSolutions = 0;
	Bool found = solver.solve(label(marks,selectVarLex(marks)));

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
		cout << solver.stats() << endl;
	if (opt.showRuntime())
		cout << solver.totalTime() << endl;
}


int main(int argc, char** argv)
{
	UInt n = 10,l = 1000,dcons = 1;
	Bool minimize = true;

	ExampleOptions opt("Golomb Ruler");
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
