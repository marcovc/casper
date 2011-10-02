/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

//	Magic Squares.
int magic(int n)
{
	const int magicConstant = n*(n*n+1)/2.0;
	cout << "magicConstant = " << magicConstant << endl;

	// create a CPSolver& object
	CPSolver solver;

	// create a (square) array of variables with size n X n, where all
	// variables have domains in 1..n*n.
	VarArray<int,2> square(solver,n,n,1,n*n);

	solver.post(distinct(square),Dom);
/*
	// post constraints to specify that all rows
	// sum to magicConstant
	for (int r = 0; r < n; r++)
	{
		BndExpr<int> sum(solver,0);
		for (int c = 0; c < n; c++)
			sum = sum + square[r][c];
		solver.post(sum == magicConstant);
	}

	// post constraints to specify that all cols
	// sum to magicConstant
	for (int c = 0; c < n; c++)
	{
		BndExpr<int> sum(solver,0);
		for (int r = 0; r < n; r++)
			sum = sum + square[r][c];
		solver.post(sum == magicConstant);
	}

	// post constraints to specify that major diagonals
	// sum to magicConstant
	BndExpr<int> sumd1(solver,0);
	BndExpr<int> sumd2(solver,0);
	for (int d = 0; d < n; d++)
	{
		sumd1 = sumd1 + square[d][d];
		sumd2 = sumd2 + square[n-d-1][d];
	}
	solver.post(sumd1 == magicConstant);
	solver.post(sumd2 == magicConstant);
*/
	VarArray<int> d1(solver,n);
	VarArray<int> d2(solver,n);
	for (int d = 0; d < n; d++)
	{
		d1[d] = square[d][d];
		d2[d] = square[n-d-1][d];
		VarArray<int> r(solver,n);
		VarArray<int> c(solver,n);
		for (int i = 0; i < n; i++)
		{
			r[i] = square[i][d];
			c[i] = square[d][i];
		}
		solver.post(gacSum(r,magicConstant));
		solver.post(gacSum(c,magicConstant));
	}
	solver.post(gacSum(d1,magicConstant));
	solver.post(gacSum(d2,magicConstant));

	// TEMP: fix some cells
/*	solver.post(square[3][1]==15);
	solver.post(square[3][2]==14);

	// post constraints to specify that all cols
	// sum to magicConstant
	for (int c = 0; c <= 0; c++)
	{
		BndExpr<int> sum(solver,0);
		for (int r = 0; r < n; r++)
			sum = sum + square[r][c];
		solver.post(sum == magicConstant);
	}
	for (int c = 0; c <= 0; c++)
		for (int r = 0; r < n-1; r++)
		{
			solver.post(square[r][c]!=15);
			solver.post(square[r][c]!=14);
			solver.post(square[r][c]!=1);
			solver.post(square[r][c]!=4);
		}
	solver.post(square[3][0]==4);
	VarArray<int> col(solver,n);
	for (int r = 0; r < n; r++)
		col[r] = square[r][0];

	// perform depth first search
	bool found = solver.solve(label(col));

	Array<bool,3> pos(stdHeap,n,n,n*n,false);

	// if previous search found a solution, print it.
	while (found)
	{
		//cout << square << endl;
		for (uint r = 0; r < n; ++r)
			for (uint c = 0; c < n; ++c)
				for (uint i = 0; i < n*n; ++i)
					if (square[r][c].domain().find(i+1)!=
						square[r][c].domain().end())
					pos[r][c][i] = true;
		found = solver.next();
	}
	for (uint r = 0; r < n; ++r)
	{
		for (uint c = 0; c < n; ++c)
		{
			cout << "{";
			for (uint i = 0; i < n*n; ++i)
				if (pos[r][c][i])
					cout << (i+1) << ",";
			cout << "} ";
		}
		cout << endl;
	}
	*/
	//solver.post(square[3][0]==1);
	solver.post(square[3][1]==15);
	solver.post(square[3][2]==14);
	//solver.post(square[3][3]==4);

	cout << solver.valid() << endl;
	cout << "before search:\n";
	cout << square << endl;

	cout << solver.solve(label(square,selectVarMinDom(square),bisectValMin2(square))) << endl;

//	cout << solver.solve(label(square,selectVarFFRR(square),bisectValMin(square))) << endl;
	cout << square << endl;
	// print statistics
	cout << solver.totalTime() << endl;
	cout << solver.stats() << endl;
	return 0;
}

/*int test()
{
	CPSolver& solver;
	Var<int> x(solver,2,3);
	Var<int> y(solver,2,3);
	Var<int> z(solver,9,15);
	//solver.post(x*(y*2)==z);
	Var<int> e(solver,-1000,1000);
	solver.post(e==x*y);
	solver.post(e*2==z);
	cout << solver.solve() << endl;
	cout << x << " " << y << " " << z << endl;
}*/

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " size\n";
		return 1;
	}
	magic(atoi(argv[1]));
}
