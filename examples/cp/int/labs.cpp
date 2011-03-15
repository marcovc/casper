/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2009 - Marco Correia <marco.v.correia@gmail.com>                 *
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


// csplib prob005: low autocorrelation binary sequences


#include <casper/kernel.h>
#include <casper/cp/solver.h>
#include <casper/cp/int.h>
#include <casper/util/options.h>

#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

void labs_open(uint n, const Util::ExampleOptions& opt)
{
	Solver solver;

	IntPar i(solver);
	IntPar j(solver);
	IntVarArray s(solver,n,-1,1);

	for (uint k = 0; k < n; k++)
		solver.post(s[k] != 0);

	IntVar m(solver,0,n*n*n);
	solver.post(m == sum(all(i,range(0,n-2),true,
					sqr(sum(all(j,range(0,n),j<=n-i-2,cache(s[j]*s[j+i+1]))))
					)));

	// symmetry breaking
	solver.post(s[0]==-1);
	solver.post(s[1]==-1);

	solver.setExplorer(bbMinimize(solver,m));

	bool found = solver.solve(label(solver,s));
	while (found)
	{
		cout << s << " " << m << endl;
		found = solver.next();
	}
	if (opt.showStats())
		cout << solver.getStats() << endl;
	if (opt.showRuntime())
		cout << solver.getCPUTimer() << endl;
}


int main(int argc, char** argv)
{
	uint n = 4;

	Util::ExampleOptions opt("Low Autocorrelation Binary Sequences Problem");
	opt.addPosParam("size",&n,"sequence size");

	if (!opt.parse(argc,argv))
		return 1;

	labs_open(n,opt);
}
