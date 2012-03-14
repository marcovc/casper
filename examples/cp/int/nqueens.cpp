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

/*	A filter that enforces that a queen is not attacking other queen.
 * 	x - The queen being attacked.
 *  y - The attacking queen.
 *  d - The distance (columns) between both queens.
 */
struct NoThreatX : IFilter
{
	// filter constructor, store parameters in local variables
	NoThreatX(Solver& solver,IntVar	x, IntVar y, uint d) :
	 	IFilter(solver),x(x),y(y),d(d),pOwner(NULL)	  { }

	// does propagation
	bool execute()
	{
		// y must be ground
		if (!y.domain().singleton())
			return true;

		// remove y,y+d, and y-d from the domain of x
    	return x.domain().erase(y.domain().value()) and
        	   x.domain().erase(y.domain().value() - d) and
        	   x.domain().erase(y.domain().value() + d);
   	}

	// attach this filter to notifiable s and to the onGround event list
	// of variable y, since it should run whenever y becomes ground
	void attach(INotifiable* s)
	{
		assert(pOwner==NULL or pOwner==s);
		pOwner = s;
		y.domain().attachOnGround(pOwner);
	}

	// does the opposite of the attach method
	void detach()
	{	y.domain().detachOnGround(pOwner);	}

	IntVar	x;
	IntVar	y;
	const uint	d;
	INotifiable*	pOwner;
};

bool postNoThreat(Solver& solver,IntVar x, IntVar y, uint d)
{	return solver.post(new (solver) NoThreatX(solver,x,y,d)) and
		   solver.post(new (solver) NoThreatX(solver,y,x,d));
}

// Try to place n queens on a nXn chessboard, such that no two queens attack
// each other
void queens1(int n, const Util::ExampleOptions& opt)
{
	// create CPSolver object
    Solver solver;

	// declare an array of n variables, each from 0 to n-1
    IntVarArray	vars(solver,n,0,n-1);

	// post the new noThreat constraint in each pair of queens
	for (int i = 0; i < n; i++)
    	for (int j = i + 1; j < n; j++)
        	postNoThreat(solver,vars[i],vars[j],j-i);

	// search
	uint nbSols = 0;
	bool found = solver.solve(label(solver,vars,selectVarMinDom(solver,vars)));
	while (found)
	{
		// print results
		cout << vars << endl;

		if (++nbSols == opt.nbSolutions())
			break;

		found = solver.next();
	}

    if (opt.showStats())
    	cout << solver.getStats() << endl;
    if (opt.showRuntime())
    	cout << solver.getCPUTimer() << endl;
}

// Try to place n queens on a nXn chessboard, such that no two queens attack
// each other (alldiff model)
void queens2(int n, const Util::ExampleOptions& opt)
{
	// create CPSolver object
    Solver solver;

	// declare an array of n variables, each from 0 to n-1
    IntVarArray	vars(solver,n,0,n-1);

	// post the new noThreat constraint in each pair of queens
    IntRef i(solver);
	solver.post(distinct(vars),postDomFilter);
	solver.post(distinct(all(i,range(0,n-1),true,vars[i]+i)),postDomFilter);
	solver.post(distinct(all(i,range(0,n-1),true,vars[i]-i)),postDomFilter);

	// search
	uint nbSols = 0;
	bool found = solver.solve(label(solver,vars,selectVarMinDom(solver,vars)));
	while (found)
	{
		// print results
		cout << vars << endl;

		if (++nbSols == opt.nbSolutions())
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
	int n = 4;
	int m = 0;
	Util::ExampleOptions opt("N-Queens");
	opt.addPosParam("n",&n,"number of queens");
	opt.addPosParam("m",&m,"model to use (0 or 1)");
	if (!opt.parse(argc,argv))
		return 1;
 	if (m==0)
 		queens1(n,opt);
 	else
 		queens2(n,opt);
	return 0;
}
