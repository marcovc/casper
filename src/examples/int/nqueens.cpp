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

/*	A filter that enforces that a queen is not attacking other queen.
 * 	x - The queen being attacked.
 *  y - The attacking queen.
 *  d - The distance (columns) between both queens.
 */
struct NoThreatX : IFilter
{
	// filter constructor, store parameters in local variables
	NoThreatX(ICPSolver& solver,DomVar<Int> x, DomVar<Int> y, UInt d) :
	 	IFilter(solver),x(x),y(y),d(d)	  { }

	// does propagation
	Bool execute()
	{
		// if x is empty than this method should never been called
		assert(!x.domain().empty());

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
	Void attach(INotifiable* s)
	{	y.domain().attachOnGround(s); }

	// does the opposite of the attach method
	Void detach(INotifiable* s)
	{	y.domain().detachOnGround(s);	}

	DomVar<Int> x;
	DomVar<Int> y;
	const UInt	d;
};

//	Creates and returns a new NoThreat filter.
Filter noThreat(DomVar<Int> x, DomVar<Int> y, UInt d)
{	return Filter(new (x.solver().heap()) NoThreatX(x.solver(),x,y,d)) and
		   Filter(new (x.solver().heap()) NoThreatX(x.solver(),y,x,d));
}

// Try to place n queens on a nXn chessboard, such that no two queens attack
// each other
void queens(int n, const ExampleOptions& opt)
{
	// create ICPSolver object
    CPSolver solver;

	// declare an array of n variables, each from 0 to n-1
    DomVarArray<Int> vars(solver,n,0,n-1);

	// post the new noThreat constraint in each pair of queens
	for (int i = 0; i < n; i++)
    	for (int j = i + 1; j < n; j++)
        	solver.post( noThreat(vars[i],vars[j],j-i) );

	// search
	UInt nbSols = 0;
	Bool found = solver.solve(label(vars,selectVarMinDom(vars)));
	while (found)
	{
		// print results
		cout << vars << endl;

		if (++nbSols == opt.nbSolutions())
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
	int n = 4;
	ExampleOptions opt("N-Queens");
	opt.addPosParam("n",&n,"number of queens");
	if (!opt.parse(argc,argv))
		return 1;
 	queens(n,opt);
	return 0;
}
