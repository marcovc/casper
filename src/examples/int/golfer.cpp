/**************************************************************************\
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

int golfers(UInt w, UInt g, UInt s, Bool labeling,const ExampleOptions& opt)
{
	CPSolver solver;

	int n = g*s; // golfers

	DomVarArray<Int,3> vars(solver,w,g,s,1,n);

	// meet once
	for (UInt wi = 0; wi < w; wi++)
		for (UInt wj = wi+1; wj < w; wj++)
			for (UInt gi = 0; gi < g; gi++)
				for (UInt gj = 0; gj < g; gj++)
				{
					MutVar<Int> i(solver),j(solver);
					solver.post(sum(all(casper::pair(i,j),range(0,s-1),true,
									cast<Int>(vars[wi][gi][i]==vars[wj][gj][j])))<=1);
				}

	// alldifferent
	for (UInt wi = 0; wi < w; wi++)
		solver.post(distinct(vars[wi]),Dom);

	// order positions in groups
	for (UInt wi = 0; wi < w; wi++)
		for (UInt gi = 0; gi < g; gi++)
			for (UInt pi = 0; pi < s-1; pi++)
				solver.post(vars[wi][gi][pi] < vars[wi][gi][pi+1]);

	// order groups in weeks
	for (UInt wi = 0; wi < w; wi++)
		for (UInt gi = 0; gi < g-1; gi++)
			solver.post(vars[wi][gi][0] < vars[wi][gi+1][0]);

	// order weeks
	for (UInt wi = 0; wi < w-1; wi++)
		solver.post(vars[wi][0][0] <= vars[wi+1][0][0]);

	UInt nbSolutions = 0;
	Bool found;

	if (labeling == 0)
		found = solver.solve(label(vars,selectVarMinDom(vars)));
	else
	{
		MutVar<Int> pi(solver),wi(solver),gi(solver),si(solver);
		found = solver.solve(forAll(pi,range(1,g*s),
								forAll(wi,range(0,w-1),
									tryAll(gi,range(0,g-1),
										tryAll(si,range(0,s-1),
											vars[wi][gi][si] == pi
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
		cout << solver.stats() << endl;
	if (opt.showRuntime())
		cout << solver.totalTime() << endl;
	return 0;
}

int main(int argc, char** argv)
{
	UInt w = 4;
	UInt g = 8;
	UInt s = 4;
	Bool h = 1;

	ExampleOptions opt("Social Golfers Problem");
	opt.addPosParam("weeks",&w,"number of weeks");
	opt.addPosParam("groups",&g,"number of groups of players");
	opt.addPosParam("size",&s,"number of players in each group");

	opt.addOptParam("heuristic",&h,"search heuristic (0=assign player to week/group/idx,1=assign week/group/pos to player)");

	if (!opt.parse(argc,argv))
		return 1;

	golfers(w,g,s,h,opt);

	return 0;
}
