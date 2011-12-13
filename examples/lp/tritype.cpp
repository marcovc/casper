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
#include <casper/kernel/ref/view.h>

#include <casper/util/container/stdrange.h>

#include <iostream>

#include <casper/cp/int.h>
#include <casper/cp/solver.h>

using namespace std;

bool tritypeFD()
{
	using namespace Casper;
	using namespace Casper::CP;

	const int maxInt = 100;

	Solver solver;
	IntVar i(solver,0,maxInt);
	IntVar j(solver,0,maxInt);
	IntVar k(solver,0,maxInt);
	IntVarArray trityp(solver,5,0,4);

	// program

	solver.post( trityp[0] == 0 );

	solver.post( trityp[1] == trityp[0] + ifThenElse(i==j,1,0) );
	solver.post( trityp[2] == trityp[1] + ifThenElse(i==k,2,0) );
	solver.post( trityp[3] == trityp[2] + ifThenElse(j==k,3,0) );

	solver.post( trityp[4] ==
				   ifThenElse(i==0 or j==0 or k==0,4,
					ifThenElse(trityp[3]==0,
						ifThenElse(i+j<=k or j+k<=i or i+k<=j,4,1),
						ifThenElse(trityp[3]>3,3,
							ifThenElse( (trityp[3]==1 and i+j>k) or
								        (trityp[3]==2 and i+k>j) or
								        (trityp[3]==3 and j+k>i), 2,4)))));

	// (negated) post-conditions
	solver.post( trityp[4] != ifThenElse(i+j<=k or j+k<=i or i+k<=j,4,
								ifThenElse(i==j and j==k,3,
									ifThenElse(i==j or j==k or i==k,2,1))));

	IntVarArray labelVars(solver,{trityp[0],trityp[1],trityp[2],trityp[3],trityp[4],i,j,k});
	bool found = solver.solve(label(solver,labelVars,
									selectVarMinDom(solver,labelVars),
									selectValMin(solver,labelVars)));

	if (found)
		cout << i << " " << j << " " << k << " " << trityp << endl;
	else
		cout << "no solution found!" << endl;

	cout << solver.getStats() << endl;

	return found;
}

bool tritypeFD_BD()
{
	using namespace Casper;
	using namespace Casper::CP;

	const int maxInt = 1000;

	Solver solver;
	IntVar i(solver,0,maxInt);
	IntVar j(solver,0,maxInt);
	IntVar k(solver,0,maxInt);
	IntVarArray trityp(solver,5,0,4);

	BoolVar ij_le_k(solver,false,true);
	BoolVar jk_le_i(solver,false,true);
	BoolVar ik_le_j(solver,false,true);

	BoolVar i_e_j(solver,false,true);
	BoolVar j_e_k(solver,false,true);
	BoolVar i_e_k(solver,false,true);

	// channeling

	solver.post(ij_le_k <= (i+j<=k));
	solver.post(jk_le_i <= (j+k<=i));
	solver.post(ik_le_j <= (i+k<=j));

	solver.post( !ij_le_k <= (i+j>k));
	solver.post( !jk_le_i <= (j+k>i));
	solver.post( !ik_le_j <= (i+k>j));

	solver.post(i_e_j <= (i==j));
	solver.post(j_e_k <= (j==k));
	solver.post(i_e_k <= (i==k));

	// program

	solver.post( trityp[0] == 0 );

	solver.post( trityp[1] == trityp[0] + ifThenElse(i_e_j,1,0) );
	solver.post( trityp[2] == trityp[1] + ifThenElse(i_e_k,2,0) );
	solver.post( trityp[3] == trityp[2] + ifThenElse(j_e_k,3,0) );

	solver.post( trityp[4] ==
				   ifThenElse(i==0 or j==0 or k==0,4,
					ifThenElse(trityp[3]==0,
						ifThenElse(ij_le_k or jk_le_i or ik_le_j,4,1),
						ifThenElse(trityp[3]>3,3,
							ifThenElse( (trityp[3]==1 and !ij_le_k) or
								        (trityp[3]==2 and !ik_le_j) or
								        (trityp[3]==3 and !jk_le_i), 2,4)))));

	// (negated) post-conditions
	solver.post( trityp[4] != ifThenElse(ij_le_k or jk_le_i or ik_le_j,4,
								ifThenElse(i_e_j and j_e_k,3,
									ifThenElse(i_e_j or j_e_k or i_e_k,2,1))));

	IntVarArray labelVars(solver,{trityp[0],trityp[1],trityp[2],trityp[3],trityp[4],i,j,k});
	bool found = solver.solve(
								label(solver,{ij_le_k,jk_le_i,ik_le_j,i_e_j,j_e_k,i_e_k}) and
								label(solver,labelVars,
									selectVarMinDom(solver,labelVars),
									selectValMin(solver,labelVars)));

	if (found)
		cout << i << " " << j << " " << k << " " << trityp << endl;
	else
		cout << "no solution found!" << endl;

	cout << solver.getStats() << endl;

	return found;
}

bool tritypeFD_LP()
{
	using namespace Casper;
	using namespace Casper::CP;

	Solver solver;
	BoolVarArray inBool(solver,18,false,true);
	BoolVarArray exBool(solver,36,false,true);

	solver.post(inBool[0]==(exBool[0] or (exBool[1] or exBool[2])));
	solver.post(inBool[10]==(exBool[5] and exBool[8]));
	solver.post(inBool[11]==(exBool[5] or exBool[11] or exBool[8]));
	solver.post(inBool[1]==exBool[5]);
	solver.post(inBool[2]==exBool[8]);
	solver.post(inBool[3]==exBool[11]);

	solver.post(inBool[5]==(exBool[15] or exBool[16] or exBool[17]));
	solver.post(inBool[7]==(exBool[22] and !exBool[15]));
	solver.post(inBool[8]==(exBool[25] and !exBool[17]));
	solver.post(inBool[9]==(exBool[28] and !exBool[16]));
	solver.post(inBool[12]==(!inBool[0] and exBool[14]));
	solver.post(inBool[13]==(!inBool[0] and !exBool[14]));
	solver.post(inBool[14]==(inBool[13] and !exBool[20]));
	solver.post(inBool[15]==(inBool[14] and !inBool[7]));
	solver.post(inBool[16]==(inBool[15] and !inBool[8]));
	solver.post(inBool[17]==(inBool[16] and !inBool[9]));

	solver.post(inBool[0]<=exBool[3]);
	solver.post(!inBool[0]<=exBool[4]);

	solver.post(( !inBool[0] and inBool[1])<=exBool[6]);
	solver.post(( !inBool[0] and !inBool[1])<=exBool[7]);
	solver.post(( !inBool[0] and inBool[2])<=exBool[9]);
	solver.post(( !inBool[0] and !inBool[2])<=exBool[10]);
	solver.post(( !inBool[0] and inBool[3])<=exBool[12]);
	solver.post(( !inBool[0] and !inBool[3])<=exBool[13]);

	solver.post((inBool[12] and inBool[5])<=exBool[18]);
	solver.post((inBool[12] and !inBool[5])<=exBool[19]);
	solver.post((inBool[13] and exBool[20])<=exBool[21]);
	solver.post((inBool[14] and inBool[7])<=exBool[24]);
	solver.post((inBool[15] and inBool[8])<=exBool[24]);
	solver.post((inBool[16] and inBool[9])<=exBool[24]);
	solver.post(inBool[17]<=exBool[18]);

	// Precondition *****************
	solver.post(exBool[33] and exBool[34] and exBool[35]);

	// Postconditions *****************
	solver.post((inBool[5] and !exBool[18]) or
				(!inBool[5] and inBool[10] and !exBool[21]) or
				(!inBool[5] and !inBool[10] and inBool[11] and !exBool[24]) or
				(!inBool[5] and !inBool[10] and !inBool[11] and !exBool[19]));

	// tmp
	const int maxInt = 1000;

	IntVar i(solver,0,maxInt);
	IntVar j(solver,0,maxInt);
	IntVar k(solver,0,maxInt);
	IntVarArray trityp(solver,5,0,4);

	// chaneling CP-LP
	solver.post(exBool[0] == (i==0));
	solver.post(exBool[1] == (j==0));
	solver.post(exBool[2] == (k==0));
	solver.post(exBool[3] == (trityp[4]==4));
	solver.post(exBool[4] == (trityp[0]==0));
	solver.post(exBool[5] == (i==j));
	solver.post(exBool[6] == (trityp[1]==(trityp[0]+1)));
	solver.post(exBool[7] == (trityp[1]==trityp[0]));
	solver.post(exBool[8] == (i==k));
	solver.post(exBool[9] == (trityp[2]==(trityp[1]+2)));
	solver.post(exBool[10] == (trityp[2]==trityp[1]));
	solver.post(exBool[11] == (j==k));
	solver.post(exBool[12] == (trityp[3]==(trityp[2]+3)));
	solver.post(exBool[13] == (trityp[3]==trityp[2]));
	solver.post(exBool[14] == (trityp[3]==0));
	solver.post(exBool[15] == ((i+j)<=k));
	solver.post(exBool[16] == ((j+k)<=i));
	solver.post(exBool[17] == ((i+k)<=j));
	solver.post(exBool[19] == (trityp[4]==1));
	solver.post(exBool[20] == (trityp[3]>=3));
	solver.post(exBool[21] == (trityp[4]==3));
	solver.post(exBool[22] == (trityp[3]==1));
	solver.post(exBool[24] == (trityp[4]==2));
	solver.post(exBool[25] == (trityp[3]==2));
	solver.post(exBool[28] == (trityp[3]==3));
	solver.post(exBool[33] == (i>=0));
	solver.post(exBool[34] == (j>=0));
	solver.post(exBool[35] == (k>=0));

	bool found = solver.solve(label(solver,inBool) and
							  label(solver,exBool) and
							  label(solver,{i,j,k,trityp[0],trityp[1],trityp[2],trityp[3],trityp[4]}));

	if (found)
		cout << i << " " << j << " " << k << " " << trityp << endl;
	else
		cout << "no solution\n";

	cout << solver.getStats() << endl;

	return found;
}

int main()
{
	tritypeFD_BD();
	//tritypeFD_LP();
	return 0;
}
