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


// csplib prob036: fixed length error correcting codes, based on gecode solution

#include <casper/set/config.h>
#include <casper/kernel.h>
#include <casper/set.h>
#include <casper/int.h>

#include <iostream>

using namespace casper;
using namespace std;

//a = alphabet size
//n = code size
//l = code length
//d = distance
void hamming(UInt n, UInt l, Int dMin, Bool maximize, const ExampleOptions& opt)
{
	CPSolver solver;

	DomVarArray<IntSet> v0(solver,n,range(0,l));
	DomVarArray<IntSet> v1(solver,n,range(0,l));

	DomVar<Int> d(solver,dMin,l);

	// complement
	for (UInt i = 0; i < n; i++)
		solver.post(partition(list(v0[i],v1[i])));

	for (UInt i = 0; i < n; i++)
		for (UInt j = i+1; j < n; j++)
			solver.post(cardinal(v0[i]*v1[j])+cardinal(v1[i]*v0[j])>=d);

	if (maximize)
		solver.setGoalSched(bbMinimize(solver,-d));
	else
		solver.post(d==dMin);

	UInt nbSolutions = 0;
	Bool found = solver.solve(label(v0));
	while (found)
	{
		cout << v0 << " : " << d << endl;
		if (!maximize and ++nbSolutions==opt.nbSolutions())
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
	UInt c = 20;
	UInt l = 15;
	UInt d = 0;
	Bool m = false;

	ExampleOptions opt("Hamming Codes Problem");
	opt.addPosParam("code",&c,"code size");
	opt.addPosParam("length",&l,"code length");

	opt.addOptParam("distance",&d,"distance (or minimum distance when used in conjunction with --maximize)");
	opt.addOptParam("maximize",&m,"try to maximize distance");

	if (!opt.parse(argc,argv))
		return 1;

	hamming(c,l,d,m,opt);

	return 0;
}
