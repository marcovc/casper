/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2009 - Marco Correia <marco.v.correia@gmail.com>           *
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

using namespace Casper;
using namespace std;

int main()
{
	CPSolver solver;
	VarArray<int> v(solver,8,0,9);
	enum {s,e,n,d,m,o,r,y};

	solver.post( 		   v[s]*1000 + v[e]*100 + v[n]*10 + v[d] +
						   v[m]*1000 + v[o]*100 + v[r]*10 + v[e] ==
				v[m]*10000 + v[o]*1000 + v[n]*100 + v[e]*10 + v[y]);
	solver.post(distinct(v));
	cout << solver.solve(label(v)) << endl;
	cout << v << endl;

}

