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


// csplib prob036: fixed length error correcting codes


#include <casper/kernel.h>
#include <casper/cp/solver.h>
#include <casper/cp/int.h>
#include <casper/util/options.h>

#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;


//a = alphabet size
//n = code size
//l = code length
//d = distance
void codes(int a, int n, int l, int cd, bool maximize, bool lee, const Util::ExampleOptions& opt)
{
	Solver solver;

	IntVarArray2 v(solver,n,l,0,a-1);
	IntVar	d(solver,cd,l);
	IntPar	k(solver);

	for (int i = 0; i < n; i++)
		for (int j = i+1; j < n; ++j)
			if (!lee)
				solver.post(sum(all(k,range(0,l-1),true,cast<int>(v[i][k]!=v[j][k])))>=d);
			else
				solver.post(sum(all(k,range(0,l-1),true,min(abs(v[i][k]-v[j][k]),a-abs(v[i][k]-v[j][k]))))>=d);

	// symmetry breaking
	// sort rows
	for (int i = 0; i < n-1; i++)
		solver.post(v[i][0]<=v[i+1][0]);
	// sort first row
	for (int i = 0; i < l-1; i++)
		solver.post(v[0][i]<=v[0][i+1]);

	if (maximize)
		solver.setExplorer(bbMinimize(solver,-d));
	else
		solver.post(d==cd);

	uint nbSolutions = 0;
	bool found = solver.solve(label(solver,v));
	while (found)
	{
		cout << v << " : " << d << endl;
		if (!maximize and ++nbSolutions == opt.nbSolutions())
			break;
		found = solver.next();
	}
	if (opt.showStats())
		cout << solver.getStats() << endl;
	if (opt.showRuntime())
		cout << solver.getCPUTimer() << endl;
}

int main(int argc, char** argv)
{
	uint a = 3;
	uint c = 15;
	uint l = 35;
	uint d = 0;
	bool m = false;
	bool lee = false;

	Util::ExampleOptions opt("Fixed-Length Error Correcting Codes Problem");
	opt.addPosParam("alphabet",&a,"alphabet size");
	opt.addPosParam("code",&c,"code size");
	opt.addPosParam("length",&l,"code length");

	opt.addOptParam("distance",&d,"distance (or minimum distance when used in conjunction with --maximize)");
	opt.addOptParam("maximize",&m,"try to maximize distance");
	opt.addOptParam("lee",&lee,"use Lee distance instead of Hamming distance");

	if (!opt.parse(argc,argv))
		return 1;

	codes(a,c,l,d,m,lee,opt);

	return 0;
}
