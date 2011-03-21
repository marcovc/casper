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
#include <iostream>

using namespace Casper;
using namespace Casper::LP;

int main()
{
	Env env;
	Solver solver(env);
	solver.addVars({0,1},-0.5,0.5);
	env.getState().insertCP();
	solver.setObjective(Solver::Maximize,{0},{1},0);
	solver.postLessEqual({0,1},{1,1},0.3);
	solver.solve();
	cout << solver.getValue(0) << " " << solver.getValue(1) << " " << solver.getObjValue() << endl;

	env.getState().restoreCP();
	solver.setObjective(Solver::Minimize,{0},{1},0);
	solver.postEqual({0,1},{1,1},0.2);
	solver.solve();
	cout << solver.getValue(0) << " " << solver.getValue(1) << " " << solver.getObjValue() << endl;

	//cout << solver.getOptimum() << endl;
}
