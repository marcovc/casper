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
#include <casper/int.h>

#include <iostream>

using namespace casper;
using namespace std;


//a = alphabet size
//n = code size
//l = code length
//d = distance
void codes(Int a, Int n, Int l, Int cd, Bool maximize, Bool lee, const ExampleOptions& opt)
{
	CPSolver solver;

	DomVarArray<Int,2> v(solver,n,l,0,a-1);
	DomVar<Int> d(solver,cd,l);
	MutVar<Int> k(solver);

	for (Int i = 0; i < n; i++)
		for (Int j = i+1; j < n; ++j)
			if (!lee)
				solver.post(sum(all(k,range(0,l-1),true,cast<Int>(v[i][k]!=v[j][k])))>=d);
			else
				solver.post(sum(all(k,range(0,l-1),true,min(abs(v[i][k]-v[j][k]),a-abs(v[i][k]-v[j][k]))))>=d);

	// symmetry breaking
	// sort rows
	for (Int i = 0; i < n-1; i++)
		solver.post(v[i][0]<=v[i+1][0]);
	// sort first row
	for (Int i = 0; i < l-1; i++)
		solver.post(v[0][i]<=v[0][i+1]);

	if (maximize)
		solver.setGoalSched(bbMinimize(solver,-d));
	else
		solver.post(d==cd);

	UInt nbSolutions = 0;
	Bool found = solver.solve(label(v));
	while (found)
	{
		cout << v << " : " << d << endl;
		if (!maximize and ++nbSolutions == opt.nbSolutions())
			break;
		found = solver.next();
	}
	if (opt.showStats())
		cout << solver.stats() << endl;
	if (opt.showRuntime())
		cout << solver.totalTime() << endl;
}

int main(int argc, char** argv)
{
	UInt a = 3;
	UInt c = 15;
	UInt l = 35;
	UInt d = 0;
	Bool m = false;
	Bool lee = false;

	ExampleOptions opt("Fixed-Length Error Correcting Codes Problem");
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
