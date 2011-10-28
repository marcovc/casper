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
 


#include <casper/kernel.h>
#include <casper/cp.h>
#include <casper/kernel/obj/expr.h>

using namespace Casper;
using namespace std;

int main()
{
	CP::Solver solver;
	CP::IntVarArray vars(solver,2,0,3);
	Util::StdArray<int,1>	v(solver,2,2);

	Expr<Seq<int> > expr(v);

	IterationExpr<Expr<int> > e(expr.toIterationExpr());
//	cout << expr << endl;
}
