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
#include <casper/util/options.h>

#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

int langford(int k, int n,const Util::ExampleOptions& opt)
{
	Solver solver;
	IntVarArray v(solver,k*n,1,n);
	IntVarArray p(solver,k*n,0,k*n-1);

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < k; ++j)
			solver.post(v[p[k*i+j]] == i+1);

		for (int j = 1; j < k; ++j)
			solver.post(p[k*i]+(i+2)*j == p[k*i+j]);
	}

	solver.post(distinct(p));	// helps...

	// break symmetries
	solver.post(v[0] <= v[k*n-1]);

	uint nbSolutions = 0;
	bool found = solver.solve(label(solver,p,selectVarMinDom(solver,p),selectValMin(solver,p)));
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
	uint k = 2;
	uint n = 4;

	Util::ExampleOptions opt("Langford Problem");
	opt.addPosParam("occurrences",&k,"number of occurrences of each digit in the sequence");
	opt.addPosParam("digits",&n,"number of digits");

	if (!opt.parse(argc,argv))
		return 1;

	langford(k,n,opt);
	return 0;
}
