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
#include <casper/lp/var.h>
#include <casper/lp/vararray.h>

#include <casper/kernel.h>
#include <casper/util/container/stdrange.h>

#include <iostream>

#include <casper/cp/int.h>
#include <casper/cp/int/count/dom.h>


#include <casper/mip/store.h>
//#include <casper/mip/var.h>
//#include <casper/mip/view.h>
#include <casper/cp/goal/whilenotground.h>
#include <casper/kernel/goal/whiledo.h>

using namespace std;

bool bubbleFD(int size)
{
	using namespace Casper;
	using namespace Casper::CP;

	const int maxInt = 11;
	const int minInt = 1;

	Solver solver;
	IntVarArray 	tabInit(solver,size,minInt,maxInt);
	IntVarArray3 	tab(solver,size,size,2,minInt,maxInt);

//	for (int j = 0; j < size; ++j )
//		solver.post( tab[0][j][0] == size-j+1);

	int cc = 0;
	for (int i = 0; i < size-1; ++i)
		for (int j = 0; j < size; ++j)
			if (j < size-i-1)
				++cc;

	BoolVarArray 	labelVars(solver,cc,false,true);

	cc = 0;
	for (int i = 0; i < size-1; ++i)
	{
		solver.post( tab[i][0][0] == tab[i][0][1]);
		for (int j = 0; j < size; ++j)
			if (j < size-i-1)
			{
				solver.post( labelVars[cc] == (tab[i][j][1] > tab[i][j+1][0]));
				solver.post(
						ifThenElse(labelVars[cc],
							tab[i+1][j][0] == tab[i][j+1][0] and
							tab[i][j+1][1] == tab[i][j][1],
							tab[i+1][j][0] == tab[i][j][1] and
							tab[i][j+1][1] == tab[i][j+1][0]) );
				++cc;
			}
			else
			{
				solver.post( tab[i+1][j][0] == tab[i][j][1]);
				if (j > size-i-1)
					solver.post( tab[i][j][1] == tab[i][j][0]);
			}
	}

	IntVarArray terms(solver,size-1,0,1);
	for (int j = 1; j < size; ++j)
		solver.post( terms[j-1] == cast<int>(tab[size-1][j-1][0]>tab[size-1][j][0] ) );
	solver.post( sum(terms) >= 1);
//	for (int j = 1; j < size; ++j)
//		negPostCond = negPostCond or tab[size-1][j-1][0]>tab[size-1][j][0];
//	solver.post( negPostCond );

//	solver.post( tab[size-1][0][0]>tab[size-1][1][0] or
//				 tab[size-1][1][0]>tab[size-1][2][0] or
//				 tab[size-1][2][0]>tab[size-1][3][0] );

    // solve
    bool found = solver.solve( label(solver,labelVars) and
    			 label(solver,terms) and
    			std::function<Goal()>([&](){ std::cout << tab << std::endl; return succeed();}) and
    			label(solver,tab) );

	if (found)
		cout << tab << endl;
	else
		cout << "no solution found!" << endl;

	cout << solver.getStats() << endl;

	return found;
}

bool bubbleMIP(int size)
{
	using namespace Casper;

	Env env;

	CP::Store	cp(env);

	LP::Solver		lp(env);
	LP::VarArray3 	tab(lp,size,size,2);

	int cc = 0;
	for (int i = 0; i < size-1; ++i)
		for (int j = 0; j < size; ++j)
			if (j < size-i-1)
				++cc;

	MIP::Store mip(cp,lp);

	CP::BoolVarArray 	labelVars(cp,cc,false,true);

	cc = 0;
	for (int i = 0; i < size-1; ++i)
	{
		lp.post( tab[i][0][0] == tab[i][0][1]);
		for (int j = 0; j < size; ++j)
			if (j < size-i-1)
			{
				mip.post( ifThenElse(labelVars[cc],
								tab[i][j][1] >= tab[i][j+1][0]+1.0,
								tab[i][j][1] <= tab[i][j+1][0]) );

				mip.post(
						ifThenElse(labelVars[cc],
							tab[i+1][j][0] == tab[i][j+1][0],
							tab[i+1][j][0] == tab[i][j][1]) );
				mip.post(
						ifThenElse(labelVars[cc],
							tab[i][j+1][1] == tab[i][j][1],
							tab[i][j+1][1] == tab[i][j+1][0]) );

				++cc;
			}
			else
			{
				lp.post( tab[i+1][j][0] == tab[i][j][1]);
				if (j > size-i-1)
					lp.post( tab[i][j][1] == tab[i][j][0]);
			}
	}

	CP::IntVarArray terms(cp,size-1,0,1);
	for (int j = 1; j < size; ++j)
		mip.post( ifThenElse(cast<bool>(terms[j-1])==true,
						tab[size-1][j-1][0]>=tab[size-1][j][0]+1.0,
						tab[size-1][j-1][0]<=tab[size-1][j][0] ));
	cp.post( sum(terms) >= 1);


	IntPar ip(env),k(env);
	BoolPar vp(env);

	Goal labelIneqs(env,
			forAll( ip, range(0,labelVars.size()-1) )
			(
				(
						post(cp,labelVars[ip]==true) or
						post(cp,labelVars[ip]==false)
				) and validate(mip)
			)
		);

	Goal labelPostCond(env,
			tryAll( ip, range(0,terms.size()-1) )
			(
				post(cp,terms[ip]==1)
			) and validate(mip)
		);

	DFSExplorer explorer(env);

	bool found = mip.valid() and  explorer.explore(Goal(env,labelIneqs and labelPostCond));

	if (found)
		cout << tab << endl;
	else
		cout << "no solution found!" << endl;

	cout << cp.getStats() << endl;
	cout << lp.getStats() << endl;
	cout << explorer.getStats() << endl;

	return found;
}
 
int main(int argc,char** argv)
{
	if (argc!=3)
	{
		cout << "usage: " << argv[0] << " size fd|mip\n";
		return 1;
	}
	else
	if (strcmp(argv[2],"fd")==0)
		bubbleFD(atoi(argv[1]));
	else
		bubbleMIP(atoi(argv[1]));
	return 0;
}
