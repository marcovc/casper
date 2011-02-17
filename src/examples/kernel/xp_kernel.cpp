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

#if 0
int main()
{
	ICPSolver&	solver;
	DomVarArray<Int>	v(solver,4,0,3);

	cout << v << endl;

	MutVar<Int>	v1(solver),v2(solver);

//	Goal g(	forAll(v1,1,3,true,orderDec(v1),
//				   v[v1]>v[v1-1] or v[v1]<v[v1-1]));

	Goal g(	forAll(v1,1,3,
				forAll(v2,1,3,v1<v2,
					display(solver,v1) and display(solver,v2)
				)
			)
		  );

//	bool found = solver.solve(g);

	bool found = solver.solve(g);

	UInt solution = 0;
	while (found)
	{
		cout << "solution " << ++solution << " : " << v << endl;
		found = solver.next();
	}

}

Void nqueens(UInt n)
{
	ICPSolver& solver;
	DomVarArray<Int> vars(solver,n,1,n);
	for (Int i = 0; i < n; i++)
		for (Int j = i+1; j < n; j++)
		{
			solver.post( vars[i] != vars[j] );
			solver.post( vars[i] != vars[j]+i-j );
			solver.post( vars[i] != vars[j]+j-i );
		}

	bool found = solver.solve();

	//found = found and solver.solve( label(vars) );
	MutVar<Int> v1(solver),v2(solver);

	Goal g(forAll(v1,range(0,n-1),
				tryAll(v2,range(1,n),
					vars[v1] == v2 )));

//	Goal g(forAll(v1,range(0,(Int)(n-1)),
//				vars[v1] == v2 ));

	found = found and solver.solve( g );
	while (found)
	{
		cout << vars << endl;
		found = solver.next();
	}
	cout << solver.stats() << endl;
}
#endif

int main()
{
	ICPSolver&	solver;
	DomVarArray<Int,3> vars(solver,2,3,4,1,10);
	MutVar<Int>	v1(solver),v2(solver);
	BndArrayView<Int,Rel2<Element,Rel2<Element,DomVarArray<Int,3>,MutVar<Int> >,MutVar<Int> > > v(solver,vars[v1][v2]);

	v1 = 1;
	v2 = 2;
	cout << v.size() << endl;
	v[2].updateMin(3);
	cout << vars << endl;

/*	Goal g( tryAll(v1,range(1,10), display(solver,v1) ) );
	bool found = solver.solve(g);

	UInt solution = 0;
	while (found)
	{
		cout << "solution " << ++solution << " : " << v1 << endl;
		found = solver.next();
	}
	*/
}

