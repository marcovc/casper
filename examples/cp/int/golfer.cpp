/**************************************************************************\
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

int golfers(uint w, uint g, uint s, bool labeling,const Util::ExampleOptions& opt)
{
	Solver solver;

	int n = g*s; // golfers

	IntVarArray3 vars(solver,w,g,s,1,n);

	// meet once
	for (uint wi = 0; wi < w; wi++)
		for (uint wj = wi+1; wj < w; wj++)
			for (uint gi = 0; gi < g; gi++)
				for (uint gj = 0; gj < g; gj++)
				{
					IntPar i(solver),j(solver);
					solver.post(sum(all(Casper::pair(i,j),range(0,s-1),true,
									cast<int>(vars[wi][gi][i]==vars[wj][gj][j])))<=1);
				}

	// alldifferent
	for (uint wi = 0; wi < w; wi++)
		solver.post(distinct(vars[wi]),postDomFilter);

	// order positions in groups
	for (uint wi = 0; wi < w; wi++)
		for (uint gi = 0; gi < g; gi++)
			for (uint pi = 0; pi < s-1; pi++)
				solver.post(vars[wi][gi][pi] < vars[wi][gi][pi+1]);

	// order groups in weeks
	for (uint wi = 0; wi < w; wi++)
		for (uint gi = 0; gi < g-1; gi++)
			solver.post(vars[wi][gi][0] < vars[wi][gi+1][0]);

	// order weeks
	for (uint wi = 0; wi < w-1; wi++)
		solver.post(vars[wi][0][0] <= vars[wi+1][0][0]);

	uint nbSolutions = 0;
	bool found;

	if (labeling == 0)
		found = solver.solve(label(solver,vars,selectVarMinDom(solver,vars)));
	else
	{
		IntPar pi(solver),wi(solver),gi(solver),si(solver);
		found = solver.solve(forAll(pi,range(1,g*s)) (
							   forAll(wi,range(0,w-1)) (
							   	 tryAll(gi,range(0,g-1)) (
								   tryAll(si,range(0,s-1)) (
									 post(solver,vars[wi][gi][si] == pi)
							)))));
	}

	while (found)
	{
		cout << vars << endl;
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
	uint w = 4;
	uint g = 8;
	uint s = 4;
	bool h = 1;

	Util::ExampleOptions opt("Social Golfers Problem");
	opt.addPosParam("weeks",&w,"number of weeks");
	opt.addPosParam("groups",&g,"number of groups of players");
	opt.addPosParam("size",&s,"number of players in each group");

	opt.addOptParam("heuristic",&h,"search heuristic (0=assign player to week/group/idx,1=assign week/group/pos to player)");

	if (!opt.parse(argc,argv))
		return 1;

	golfers(w,g,s,h,opt);

	return 0;
}
