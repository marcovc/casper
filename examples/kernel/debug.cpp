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
#include <casper/cp.h>
#include <casper/kernel/spexpr/expr.h>

#include <iostream>


using namespace Casper;
using namespace Casper::CP;
using namespace std;
 
void test1()
{
	Solver solver;

/*	IntVarArray y(solver,2,0,3);
	IntVarArray3 x(solver,3,3,4,1,3);

	Expr<int> i(0);
	Expr<int> j(1);
	//IntVar i(solver,0,1);

	Expr<Seq<int> > ex(x);

	Expr<Seq<int> > r(rel<Element>(ex,i));
	Expr<Seq<int> > r2(rel<Element>(r,i));

	cout << r2 << endl;
	*/

	//for (auto it = r.toIterationExpr(); it.valid(); it.iterate())
	//	cout << it.value() << endl;

//	Expr<bool> f(rel<Equal>(r,1));
//	bool found  = solver.post(distinct(r1));
//	found = found and solver.solve(label(solver,x));
//	while (found)
//	{
//		cout << i << " " << x << endl;
//		found = solver.next();
//	}

}


int main()
{
	test1();
}

