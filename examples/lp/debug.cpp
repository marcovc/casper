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

#include <casper/kernel/goal.h>
#include <casper/kernel/par/view.h>

#include <casper/util/container/stdrange.h>

#include <iostream>

using namespace Casper;

#if 0
int main()
{
	const uint n = 5;

	Env env;
	LP::Solver solver(env);

	int idxCount = 0;

	LP::Solver::Coeffs ones(env,n);
	for (uint i = 0; i < n; ++i)
		ones[i] = 1;

	// q: 0..n-1
	LP::Solver::Idxs q(env,n);
	for (uint i = 0; i < n; ++i)
		q[i] = idxCount++;

	// d: n..n*n+n-1
	Util::StdArray<uint,2> d(env,n,n);
	for (uint i = 0; i < n; ++i)
		for (uint j = 0; j < n; ++j)
			d[i][j] = idxCount++;

	// set q bounds
	for (uint i = 0; i < n; ++i)
		solver.postIn(q[i],1,n);

	// set d bounds
	for (uint i = 0; i < n; ++i)
		for (uint j = 0; j < n; ++j)
			solver.postIn(d[i][j],0,1);

	//  each queen can be in one column only (domain constraint)
	for (uint r = 0; r < n; ++r)
		solver.postEqual(d[r],ones,1);

	//  each queen can be in one row only (domain constraint)
	for (uint c = 0; c < n; ++c)
	{
		LP::Solver::Idxs idxs(n);
		for (uint r = 0; r < n; ++r)
			idxs[r] = d[r][c];
		solver.postEqual(idxs,ones,1);
	}

	// each major diagonal can have at most one queen (distinct on the major diagonals)
	for (int i = -((int)n)+2; i <= ((int)n)-2; ++i)
	{
		Util::StdList<uint> lidxs;
		for (int r = 0; r < (int)n; ++r)
			for (int c = 0; c < (int)n; ++c)
				if (r-c==i)
					lidxs.pushBack(d[r][c]);
		LP::Solver::Idxs idxs(lidxs.size());
		LP::Solver::Coeffs coeffs(lidxs.size());
		uint c = 0;
		for (auto it = lidxs.begin(); it != lidxs.end(); ++it)
		{
			idxs[c] = *it;
			coeffs[c] = 1;
			++c;
		}
		solver.postLessEqual(idxs,coeffs,1);
	}

	// each minor diagonal can have at most one queen (distinct on the minor diagonals)
	for (int i = -((int)n)+2; i <= ((int)n)-2; ++i)
	{
		Util::StdList<uint> lidxs;
		for (int r = 0; r < (int)n; ++r)
			for (int c = 0; c < (int)n; ++c)
				if (r+c-((int)n)+1==i)
					lidxs.pushBack(d[r][c]);
		LP::Solver::Idxs idxs(lidxs.size());
		LP::Solver::Coeffs coeffs(lidxs.size());
		uint c = 0;
		for (auto it = lidxs.begin(); it != lidxs.end(); ++it)
		{
			idxs[c] = *it;
			coeffs[c] = 1;
			++c;
		}
		solver.postLessEqual(idxs,coeffs,1);
	}

	// channel
	for (uint r = 0; r < n; ++r)
	{
		LP::Solver::Idxs idxs(n+1);
		LP::Solver::Coeffs onesp1(n+1);
		for (uint c = 0; c < n; ++c)
		{
			idxs[c] = d[r][c];
			onesp1[c] = c+1;
		}
		idxs[n] = q[r];
		onesp1[n] = -1;
		solver.postEqual(idxs,onesp1,0);
	}

	DoubleParArray dv(env,n*n);
	for (uint i = 0; i < n; ++i)
		for (uint j = 0; j < n; ++j)
			dv[i*n+j] = solver.getValuePar(d[i][j]);

	DoubleParArray qv(env,n);
	for (uint i = 0; i < n; ++i)
		qv[i] = solver.getValuePar(q[i]);

	solver.printModel("nqueens.mps");

	IntPar i(env),j(env);
	DoublePar x(env);

	Goal mipProp = Goal(env,
						solve(solver) and
						whileDo(forSome(i,range(0,n*n-1),abs(dv[i]-round(dv[i]))>1.0e-9))
						(
							//assign(j,argMax(i,range(0,n*n-1),abs(dv[i]-round(dv[i])))) and
							assign(j,argMax(i,range(0,n*n-1),abs(dv[i]-round(dv[i]))>1.0e-9)) and
							assign(x,dv[j]) and
							(LP::postLessEqual(solver,j+n,floor(x)) or
							 LP::postGreaterEqual(solver,j+n,ceil(x))) and
							solve(solver)
						)
					);

	//cp.when(b,[](){ return lp.postLessEqual(1,2) and lp.solve(); });

/*	IntPar k(env),l(env);

	Goal search = Goal(env,
			forAll(k,range(0,n-1))
			(
				tryAll(l,range(1,n))
				( LP::postEqual(solver,k,l) ) and
				mipProp
			)
		);
*/
	DFSExplorer dfs(env);
	bool found = dfs.explore(mipProp);
	if (!found)
		cout << "no solution!\n";
	while (found)
	{
		cout << qv << " " << dv << endl;
		break;
		found = dfs.resume();
	}
	cout << dfs.getStats() << endl;

}

#else

#include <casper/cp/int.h>
#include <casper/cp/solver.h>

using namespace std;

int main()
{
	const int n = 10;

	Env env;

	LP::Solver lp(env);
	CP::Store  cp(env);

	CP::BoolVarArray b(cp,3,false,true);

	lp.postIn({0,1,2},1,n);

	cp.when(b[0],[&](){ lp.postGreaterEqual({0,1},{1,-1},0); return lp.solve(); });
	cp.when(!b[0],[&](){ lp.postGreaterEqual({1,0},{1,-1},1); return lp.solve(); });

	cp.when(b[1],[&](){ lp.postGreaterEqual({1,2},{1,-1},0); return lp.solve(); });
	cp.when(!b[1],[&](){ lp.postGreaterEqual({2,1},{1,-1},1); return lp.solve(); });

	cp.when(b[2],[&](){ lp.postGreaterEqual({0,2},{1,-1},0); return lp.solve(); });
	cp.when(!b[2],[&](){ lp.postGreaterEqual({2,0},{1,-1},1); return lp.solve(); });

	IntPar i(env);

	Goal g(env,
			forAll(i,range(0,2)) (
			 ( post(cp,b[i]==false) or post(cp,b[i]==true) ) and solve(lp)
			)
		   );

	DFSExplorer dfs(env);
	bool found = cp.valid() and dfs.explore(g);

	if (!found)
		cout << "no solution!\n";
	while (found)
	{
		cout << b << endl;
		found = dfs.resume();
	}
	cout << dfs.getStats() << endl;

}
#endif
