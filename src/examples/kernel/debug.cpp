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

#include <casper/set/config.h>

#include <casper/kernel.h>
#include <casper/int.h>

#include <iostream>

using namespace casper;
using namespace std;

void test3()
{
	ICPSolver& solver;
	DomVar<Int> X1(solver,1,24),X2(solver,1,24);
	//solver.post((abs(X1/5-X2/5)==1);

	//solver.post(X1 == 6/X2);
	solver.post(X1*X2 == 24);

	DomVarArray<Int> vars(solver,2);
	vars[0] = X1;
	vars[1] = X2;
	Bool found = solver.solve();
	std::cout << vars << std::endl;
	found = found and solver.solve(label(vars));
	while (found)
	{
		std::cout << vars << std::endl;
		found = solver.next();
	}
	sleep(2);
	solver.totalTime().pause();
	cout << solver.totalTime().getSecs() << endl;
}


int main()
{
	test3();
}

