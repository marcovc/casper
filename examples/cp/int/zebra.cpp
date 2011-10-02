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

int main(int argc, char** argv)
{
	enum
	{
    	red,green,ivory,yellow,blue,						// color
     	englishman,spaniard,ukranian,norwegian,japanese,    // nationality
 		dog,snails,fox,horse,zebra,                         // pet
        water,cofee,tea,milk,juice,                         // drink
        oldgold,kools,chesterfield,luckystrike,parliament   // cigarret
	};

	CPSolver solver;
    VarArray<int> v(solver,5*5,1,5);

    // The Englishman lives in the red house.
    solver.post(v[englishman] == v[red]);

    // The Spaniard owns a dog.
    solver.post(v[spaniard] == v[dog]);

    // Coffee is drunk in the green house.
    solver.post(v[cofee] == v[green]);

	// The Ukrainian drinks tea.
    solver.post(v[ukranian] == v[tea]);

	// The green house is directly to the right of the ivory house.
    solver.post(v[green] == v[ivory] + 1);

	// The Old-Gold smoker owns snails.
    solver.post(v[oldgold] == v[snails]);

	// Kools are being smoked in the yellow house.
    solver.post(v[kools] == v[yellow]);

	// Milk is drunk in the middle house.
    solver.post(v[milk] == 3);

	// The Norwegian lives in the first house on the left.
    solver.post(v[norwegian] == 1);

	// The Chesterfield smoker lives next to the fox owner.
    solver.post(v[chesterfield] >= v[fox] - 1); // no abs constraint yet!
	solver.post(v[chesterfield] <= v[fox] + 1);

	// Kools are smoked in the house next to the house where the horse is kept.
    solver.post(v[kools] >= v[horse] - 1);      // no abs constraint yet!
	solver.post(v[kools] <= v[horse] + 1);

	// The Lucky-Strike smoker drinks orange juice.
    solver.post(v[luckystrike] == v[juice]);

	// The Japanese smokes Parliament.
    solver.post(v[japanese] == v[parliament]);

	// The Norwegian lives next to the blue house.
    solver.post(v[norwegian] >= v[blue] - 1);   // no abs constraint yet!
	solver.post(v[norwegian] <= v[blue] + 1);

    // all_different constraints
    solver.post(allDifferent(solver,v.begin(),v.begin()+5));
    solver.post(allDifferent(solver,v.begin()+5,v.begin()+10));
    solver.post(allDifferent(solver,v.begin()+10,v.begin()+15));
    solver.post(allDifferent(solver,v.begin()+15,v.begin()+20));
    solver.post(allDifferent(solver,v.begin()+20,v.begin()+25));

	cout << solver.solve(label(v)) << endl;

	cout << v << endl;
	cout << solver.totalTime() << endl;
	return 0;
}
