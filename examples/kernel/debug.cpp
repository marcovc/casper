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
#include <casper/cp/int/count/dom.h>

#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

void test1()
{
	Solver solver;
	IntVar n(solver,2,3);

	IntVarArray x(solver,4,0,5);

	solver.post(countEqual(n,x,1),postDomFilter);

	bool found = solver.solve(label(solver,x));

	while (found)
	{
		cout << n << " : " << x << endl;
		found = solver.next();
	}
	cout << solver.getStats() << endl;
}


int main()
{
	test1();
}

