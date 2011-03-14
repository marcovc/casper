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
#include <casper/cp/int.h>
#include <casper/cp/solver.h>

#include <iostream>

using namespace Casper;
using namespace std;

void test1()
{
	Env env;
	CP::Store store(env);
	CP::VarArray<int> x(store,2,0,1);
	DFSExplorer explorer(env);
	Goal g(env,post(store,x[0]==0) or post(store,x[0]==1));
	cout << explorer.explore(g) << endl;
	cout << x << endl;
	cout << env.getStats() << endl;
	cout << env.getState().getStats() << endl;
	cout << store.getStats() << endl;
	cout << explorer.getStats() << endl;
}

void test2()
{
	using namespace CP;
	Solver solver;
	BoolVarArray x(solver,4,0,1);
	IntPar i(solver);
	Goal g(solver,
		forAll(i,range(0,3)) (
			post(solver,x[i]==false) or post(solver,x[i]==true)
		)
	);
	bool found = solver.solve(g);
	while (found)
	{
		cout << x << endl;
		found = solver.next();
	}
	cout << solver.getStats() << endl;
}

int main()
{
	test2();
}

