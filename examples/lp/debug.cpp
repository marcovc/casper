/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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
 
#include <casper/lp/solver.h>
#include <casper/lp/goal.h>
#include <casper/lp/vararray.h>

#include <casper/kernel/goal.h>
#include <casper/kernel/ref/view.h>

#include <casper/util/container/stdrange.h>

#include <iostream>

using namespace Casper;
using namespace Casper::LP;

#include <casper/cp/int.h>
#include <casper/cp/solver.h>

using namespace std;

int main()
{
	const int n = 10;

	Env env;

	Solver solver(env);
	VarArray x(solver,2);
	solver.post( 4.0*x[0] - x[1] <= 3.0);
	solver.post( 3.0*x[0] + x[1] >= 8.0);
	solver.post( 2.0*x[0] + x[1] <= 7.5);
	cout << solver.solve() << endl;
	cout << x << endl;
	double lb,ub;
	x[0].getActivityRange(lb,ub);
	cout << lb << ".." << ub << endl;
	x[1].getActivityRange(lb,ub);
	cout << lb << ".." << ub << endl;
}

