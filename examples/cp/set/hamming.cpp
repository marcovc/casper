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

#include <casper/cp/set/config.h>
#include <casper/kernel.h>
#include <casper/cp/solver.h>
#include <casper/cp/set.h>
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
void hamming(uint n, uint l, int dMin, bool maximize, const Util::ExampleOptions& opt)
{
	Solver solver;

	IntSetVarArray v0(solver,n,range(0,l));
	IntSetVarArray v1(solver,n,range(0,l));

	IntVar d(solver,dMin,l);

	// complement
	for (uint i = 0; i < n; i++)
		solver.post(partition(Casper::list(v0[i],v1[i])));

	for (uint i = 0; i < n; i++)
		for (uint j = i+1; j < n; j++)
			solver.post(cardinal(v0[i]*v1[j])+cardinal(v1[i]*v0[j])>=d);

	if (maximize)
		solver.setExplorer(bbMinimize(solver,-d));
	else
		solver.post(d==dMin);

	uint nbSolutions = 0;
	bool found = solver.solve(label(solver,v0));
	while (found)
	{
		cout << v0 << " : " << d << endl;
		if (!maximize and ++nbSolutions==opt.nbSolutions())
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
	uint c = 20;
	uint l = 15;
	uint d = 0;
	bool m = false;

	Util::ExampleOptions opt("Hamming Codes Problem");
	opt.addPosParam("code",&c,"code size");
	opt.addPosParam("length",&l,"code length");

	opt.addOptParam("distance",&d,"distance (or minimum distance when used in conjunction with --maximize)");
	opt.addOptParam("maximize",&m,"try to maximize distance");

	if (!opt.parse(argc,argv))
		return 1;

	hamming(c,l,d,m,opt);

	return 0;
}
