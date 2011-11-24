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

#define EX 16

#if EX==1

#include <casper/kernel.h>
#include <casper/util.h>
#include <iostream>

using namespace Casper;
using namespace std;

int main()
{
	State state;
	Reversible<int> v(state);
	v = 2;
	cout << "at state " << state.id() << " : " << v << endl;
	state.insertCP();
	v = 3;
	cout << "at state " << state.id() << " : " << v << endl;
	state.restoreCP();
	cout << "at state " << state.id() << " : " << v << endl;
	v = 4;
	cout << "at state " << state.id() << " : " << v << endl;
	state.insertCP();
	v = 5;
	cout << "at state " << state.id() << " : " << v << endl;
	state.restoreCP();
	cout << "at state " << state.id() << " : " << v << endl;
	state.removeCP();
	state.restoreCP();
	cout << "at state " << state.id() << " : " << v << endl;
	cout << state.getStats() << endl;
}

#elif EX==2

#include <casper/kernel.h>
#include <casper/util.h>
#include <iostream>

using namespace Casper;
using namespace std;

int main()
{
	State state;
	List<int> v(state);
	v.pushBack(2);
	cout << "at state " << state.id() << " : " << v << endl;
	state.insertCP();
	v.pushBack(3);
	cout << "at state " << state.id() << " : " << v << endl;
	state.restoreCP();
	cout << "at state " << state.id() << " : " << v << endl;
	v.pushBack(4);
	cout << "at state " << state.id() << " : " << v << endl;
	state.insertCP();
	v.eraseFirst(2);
	cout << "at state " << state.id() << " : " << v << endl;
	state.restoreCP();
	cout << "at state " << state.id() << " : " << v << endl;
	state.removeCP();
	state.restoreCP();
	cout << "at state " << state.id() << " : " << v << endl;
	cout << state.getStats() << endl;
}

#elif EX==3

#include <casper/kernel.h>
#include <casper/util.h>
#include <iostream>

using namespace Casper;
using namespace std;

int main()
{
	State state;
	IntPar v(state);
	Goal searchTree(state,assign(v,1) or assign(v,2));

	DFSExplorer dfs(state);
	bool found = dfs.explore(searchTree);
	while (found)
	{
		cout << v << endl;
		found = dfs.resume();
	}
	cout << dfs.getStats() << endl;
}

#elif EX==4

#include <casper/kernel.h>
#include <casper/util.h>
#include <iostream>

using namespace Casper;
using namespace std;

int main()
{
	State state;
	IntParArray v(state,3);
	Goal a0(state, assign(v[0],1) or assign(v[0],2));
	Goal a1(state, assign(v[1],1) or assign(v[1],2));
	Goal a2(state, assign(v[2],1) or assign(v[2],2));

	Goal searchTree(state,a0 and a1 and a2);

	DFSExplorer dfs(state);
	bool found = dfs.explore(searchTree);
	while (found)
	{
		cout << v << endl;
		found = dfs.resume();
	}
	cout << dfs.getStats() << endl;
}
#elif EX==5

#include <casper/kernel.h>
#include <casper/util.h>
#include <iostream>

using namespace Casper;
using namespace std;

int main()
{
	State state;
	IntParArray v(state,3);

	IntPar i(state);

	Goal searchTree(state,
			forAll(i,range(0,2))
			(
				assign(v[i],0) or assign(v[i],1)
			)
	);


	DFSExplorer dfs(state);
	bool found = dfs.explore(searchTree);
	while (found)
	{
		cout << v << endl;
		found = dfs.resume();
	}
	cout << dfs.getStats() << endl;
}

#elif EX==6

#include <casper/kernel.h>
#include <casper/util.h>
#include <iostream>

using namespace Casper;
using namespace std;

int main()
{
	State state;
	IntParArray v(state,3);

	IntPar i(state),j(state);

	Goal searchTree(state,
			forAll(i,range(0,2)) (
				tryAll(j,{1,5,6})
				( assign(v[i],j) )
			)
	);


	DFSExplorer dfs(state);
	bool found = dfs.explore(searchTree);
	while (found)
	{
		cout << v << endl;
		found = dfs.resume();
	}
	cout << dfs.getStats() << endl;
}

#elif EX==7

#include <casper/kernel.h>
#include <casper/util.h>
#include <iostream>

using namespace Casper;
using namespace std;

int main()
{
	State state;
	IntParArray v(state,4);

	IntPar i(state),j(state);

	Goal searchTree(state,
			forAll(i,range(0,3)) (
				tryAll(j,range(0,1))
				( assign(v[i],j) )
			)
	);

	IntPar r(state, v[0]*8 + v[1]*4 + v[2]*2 + v[3]);

	DDSIteration dds(state,2);
	bool found = dds.explore(searchTree);
	while (found)
	{
		cout << v << " : " << r << endl;
		found = dds.resume();
	}
	cout << dds.getStats() << endl;
}

#elif EX==8

#include <casper/kernel.h>
#include <casper/util.h>
#include <iostream>

using namespace Casper;
using namespace std;

int main()
{
	State state;
	IntParArray v(state,4);

	IntPar i(state),j(state);

	Goal searchTree(state,
			forAll(i,range(0,3)) (
				tryAll(j,range(0,1))
				( assign(v[i],j) )
			)
	);

	IntPar r(state, v[0]*8 + v[1]*4 + v[2]*2 + v[3]);

	LDSIteration lds(state,2);
	bool found = lds.explore(searchTree);
	while (found)
	{
		cout << v << " : " << r << endl;
		found = lds.resume();
	}
	cout << lds.getStats() << endl;
}

#elif EX==9

#include <casper/kernel.h>
#include <casper/util.h>
#include <iostream>

using namespace Casper;
using namespace std;

int main()
{
	State state;
	IntParArray v(state,4);

	IntPar i(state),j(state);

	Goal searchTree1(state,
			forAll(i,range(0,1)) (
				tryAll(j,range(0,1))
				( assign(v[i],j) )
			)
	);

	Goal searchTree2(state,
			forAll(i,range(2,3)) (
				tryAll(j,range(0,1))
				( assign(v[i],j) )
			)
	);

	IntPar r(state, v[0]*8 + v[1]*4 + v[2]*2 + v[3]);

	DFSExplorer dfs(state);

	bool found = dfs.explore(searchTree1);
	while (found)
	{
		LDSIteration lds(state,1);
		found = lds.explore(searchTree2);
		while (found)
		{
			cout << v << " : " << r << endl;
			found = lds.resume();
		}
		found = dfs.resume();
	}
	cout << dfs.getStats() << endl;
}

#elif EX==10

#include <casper/kernel.h>
#include <casper/cp.h>
#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

int main()
{
	Env env;

	Store store(env);

	IntVarArray v(store,4,1,4);

	cout << v << endl;

	bool valid;

	valid = store.post(distinct(v));
	valid = valid and store.valid();
	cout << valid << " : " << v << endl;

	valid = valid and store.post(v[0]==1);
	valid = valid and store.valid();
	cout << valid << " : " << v << endl;
	cout << store.getStats() << endl;
}

#elif EX==11

#include <casper/kernel.h>
#include <casper/cp.h>
#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

int main()
{
	const uint n = 4;

	Env env;

	Store store(env);

	IntVarArray v(store,n,1,n);

	cout << v << endl;

	store.post(distinct(v),postDomFilter);

	IntPar i(env);
	store.post(distinct(all(i,range(0,n-1),true,v[i]+i)),postDomFilter);
	store.post(distinct(all(i,range(0,n-1),true,v[i]-i)),postDomFilter);

	bool valid = store.valid();
	cout << valid << " : " << v << endl;

	IntPar j(env);
	Goal searchTree(env,
			forAll(i,range(0,n-1)) (
				tryAll(j,range(1,n))
				( post(store,v[i]==j) )
			)
	);

	DFSExplorer dfs(env);

	bool found = dfs.explore(searchTree);
	while (found)
	{
		cout << v << endl;
		found = dfs.resume();
	}
	cout << store.getStats() << endl;
	cout << dfs.getStats() << endl;
}

#elif EX==12

#include <casper/kernel.h>
#include <casper/cp.h>
#include <casper/kernel/goal/whiledo.h>
#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

int main(int argc, char** argv)
{
	const int n = atoi(argv[1]);

	Env env;

	Store store(env);

	IntVarArray v(store,n,0,n-1);

	cout << v << endl;


	IntPar i(env);
	store.post(distinct(v),postDomFilter);
	store.post(distinct(all(i,range(0,n-1),true,v[i]+i)),postDomFilter);
	store.post(distinct(all(i,range(0,n-1),true,v[i]-i)),postDomFilter);

//	store.post(distinct(v),postValFilter);
//	store.post(distinct(all(i,range(0,n-1),true,v[i]+i)),postValFilter);
//	store.post(distinct(all(i,range(0,n-1),true,v[i]-i)),postValFilter);

	bool valid = store.valid();
	cout << valid << " : " << v << endl;

	IntPar idx(env),val(env);
#if 0
	Goal varSelect(store,
			assign(idx,
				argMin(i,nonGroundIdxs(v),
						cast<int>(domainSize(store,v[i]))*n+abs(i-n/2))));
	Goal valSelect(store,
			assign(val,
				argMax(i,domain(store,v[idx]),
						randInRange(0,n-1))));
#else
	Goal varSelect(store,
			assign(idx,
				std::function<int()>([v,n]()
				{
					int midx = 0;
					int mmin = limits<int>::max();
			        for (int j = 0; j < n; ++j)
			        	if (!v[j].ground())
						{
							int h = v[j].domain().size()*n+std::abs(j-n/2);
							if (h < mmin)
							{
								mmin = h;
								midx = j;
							}
						}
			        cout << v << endl;
			        cout << midx;
					return midx;
				})));
	Goal valSelect(store,
			assign(val,
				std::function<int()>([&]()
						{
							int mmit = 0;
							int mmin = n;
							for (auto it = v[idx.value()].domain().begin();
									it != v[idx.value()].domain().end(); ++it)
							{
								int h = std::abs(*it-n/2);
								if (h < mmin)
								{
									mmin = h;
									mmit = *it;
								}
							}
					        cout << " " << mmit << endl;
					        return mmit;
						})));
#endif

//	Goal valSelect(store,
//			assign(val,
//				min(i,domain(store,v[idx]),i)));

	Goal searchTree(env,
			whileDo(std::function<bool()>([&](){return not v.ground();}))
			(
				varSelect and valSelect and
				(
					post(store,v[idx] == val) or
					post(store,v[idx] != val)
				)
			));

	DFSExplorer dfs(env);

	bool found = dfs.explore(searchTree);

/*	while (found)
	{
		cout << v << endl;
		found = dfs.resume();
	}*/
	if (found)
		cout << v << endl;
	else
		cout << "no solution!" << endl;
	cout << store.getStats() << endl;
	cout << dfs.getStats() << endl;
}

#elif EX==13

#include <casper/kernel.h>
#include <casper/cp.h>
#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

int main()
{
	const int n = 100;

	Env env;

	Store store(env);

	IntVarArray v(store,n,1,n);

	cout << v << endl;

	store.post(distinct(v),postDomFilter);

	IntPar i(env);
	store.post(distinct(all(i,range(0,n-1),true,v[i]+i)),postDomFilter);
	store.post(distinct(all(i,range(0,n-1),true,v[i]-i)),postDomFilter);

	bool valid = store.valid();
	cout << valid << " : " << v << endl;

	Goal searchTree(env,label(store,v,selectVarMinDom(store,v),selectValsRand(store,v)));

	DFSExplorer dfs(env);

	bool found = dfs.explore(searchTree);
	while (found)
	{
		cout << v << endl;
		found = dfs.resume();
	}
	cout << store.getStats() << endl;
	cout << dfs.getStats() << endl;
}

#elif EX==14

#include <casper/kernel.h>
#include <casper/cp.h>
#include <iostream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

int main()
{
	const int n = 100;

	Solver solver;

	IntVarArray v(solver,n,1,n);

	cout << v << endl;

	solver.post(distinct(v),postDomFilter);

	IntPar i(solver);
	solver.post(distinct(all(i,range(0,n-1),true,v[i]+i)),postDomFilter);
	solver.post(distinct(all(i,range(0,n-1),true,v[i]-i)),postDomFilter);

	Goal searchTree(solver,label(solver,v,selectVarMinDom(solver,v),selectValsRand(solver,v)));

	bool found = solver.solve(searchTree);
	while (found)
	{
		cout << v << endl;
		found = solver.next();
	}
	cout << solver.getStats() << endl;
}
#elif EX==15

#include <casper/kernel.h>
#include <casper/cp.h>
#include <iostream>
#include <list>
#include <fstream>

using namespace Casper;
using namespace Casper::CP;
using namespace std;

int readNbSubjects(const char* fname)
{
	int nbSubjects;
	ifstream f(fname);
	f >> nbSubjects;
	f.close();
	return nbSubjects;
}

int readScheduleData(const char* fname, Util::StdStringArray& names, Util::StdIntArray2& data)
{
	int nbSubjects;
	ifstream f(fname);
	f >> nbSubjects >> ws;
	for (int i = 0; i < nbSubjects; ++i)
	{
		assert(f.good());
		std::string s;
		f >> s >> ws;
		names[i] = s;
		for (int j = 0; j < nbSubjects; ++j)
			f >> data[i][j] >> ws;
	}
	f.close();
	return nbSubjects;
}

int readClassData(const char* fname, int nbSubjects, Util::StdIntArray& nbTheoretical,Util::StdIntArray& nbPractical)
{
	ifstream f(fname);
	int nbClasses = 0;
	for (int i = 0; i < nbSubjects; ++i)
	{
		assert(f.good());
		f >> nbTheoretical[i] >> ws >> nbPractical[i] >> ws;
		nbClasses += nbTheoretical[i] + nbPractical[i];
	}
	f.close();
	return nbClasses;
}

void printSchedule(int nbSubjects, int hour, const Util::StdStringArray& names,
					const Util::StdIntArray& nbTheoretical, const Util::StdIntArray& nbPractical,
					const IntVarArray& 	startTimesT, const IntVarArray2& startTimesP,
					const IntVarArray& 	daysT, const IntVarArray2& 	daysP)
{
	int w[5] = {0};

	// find width of columns
	for (int d = 2; d <= 6; ++d)
		for (int t = 8*hour; t < 20*hour; ++t)
		{
			int ww = 0;
			for (int s = 0; s < nbSubjects; ++s)
			{
				if (daysT[s].domain().value() == d and
					t>=startTimesT[s].domain().value() and
					t<startTimesT[s].domain().value()+ 2*hour)
					ww += names[s].size()+2+3;
				for (int p = 0; p < nbPractical[s]; ++p)
					if (daysP[s][p].domain().value() == d and
						t>=startTimesP[s][p].domain().value() and
						t<startTimesP[s][p].domain().value()+ 2*hour)
						ww += names[s].size()+2+3;
			}
			w[d-2] = std::max(w[d-2],ww);
		}

	// print header
	for (int d = 2; d <= 6; ++d)
	{
		cout << "+";
		for (int i = 0; i < w[d-2]; ++i)
			cout << "-";
	}
	cout << "+" << endl;

	// print rows
	for (int t = 8*hour; t < 20*hour; ++t)
	{
		cout << "|";
		for (int d = 2; d <= 6; ++d)
		{
			int sused = 0;
			for (int s = 0; s < nbSubjects; ++s)
			{
				if (daysT[s].domain().value() == d and
					t>=startTimesT[s].domain().value() and
					t<startTimesT[s].domain().value()+ 2*hour)
				{
					cout << " " << names[s] << "(T) ";
					sused += names[s].size()+2+3;
				}
				for (int p = 0; p < nbPractical[s]; ++p)
					if (daysP[s][p].domain().value() == d and
						t>=startTimesP[s][p].domain().value() and
						t<startTimesP[s][p].domain().value()+ 2*hour)
					{
						cout << " " << names[s] << "(P) ";
						sused += names[s].size()+2+3;
					}
			}
			for (int i = 0; i < w[d-2]-sused; ++i)
				cout << " ";
			cout << "|";
		}
		cout << endl;
	}

	// print footer
	for (int d = 2; d <= 6; ++d)
	{
		cout << "+";
		for (int i = 0; i < w[d-2]; ++i)
			cout << "-";
	}
	cout << "+" << endl;
}

void calcCollisions(const Util::StdIntArray& nbPractical,
					Util::StdIntArray2& collisions,
					int s1,int s2,
					int& costTT, Util::StdIntArray& costTP,Util::StdIntArray& costPT, Util::StdIntArray2& costPP)
{
	costTT = collisions[s1][s2];
	int aux = collisions[s1][s2];
	for ( int c = 0; c < nbPractical[s2]; ++c)
	{
		costTP[c] = std::ceil(aux/(double)(nbPractical[s2]-c));
		assert(costTP[c]>=0);
		aux -= costTP[c];
	}
	assert(nbPractical[s2] == 0 or aux==0);
	aux = collisions[s1][s2];
	for ( int c = 0; c < nbPractical[s1]; ++c)
	{
		costPT[c] = std::ceil(aux/(double)(nbPractical[s1]-c));
		assert(costPT[c]>=0);
		aux -= costPT[c];
	}
	assert(nbPractical[s1] == 0 or aux==0);
	aux = collisions[s1][s2];
	for ( int c1 = 0; c1 < nbPractical[s1]; ++c1)
	{
		int aux2 = std::ceil(aux/(double)(nbPractical[s1]-c1));
		for ( int c2 = 0; c2 < nbPractical[s2]; ++c2)
		{
			costPP[c1][c2] = std::ceil(aux2/(double)(nbPractical[s2]-c2));
			assert(costPP[c1][c2]>=0);
			aux2 -= costPP[c1][c2];
		}
		assert(nbPractical[s2] == 0 or aux2==0);
		aux -= std::ceil(aux/(double)(nbPractical[s1]-c1));
	}
	assert(nbPractical[s1] == 0 or nbPractical[s2] == 0 or aux==0);
}

void assignRealSolution(Solver& solver,
							const IntVarArray& 	startTimesT, const IntVarArray2& startTimesP,
							const IntVarArray& 	daysT, const IntVarArray2& 	daysP)
{
	solver.post(daysT[0]==3); solver.post(startTimesT[0]==9*2);
	solver.post(daysT[1]==2); solver.post(startTimesT[1]==14*2);
	solver.post(daysT[2]==5); solver.post(startTimesT[2]==8*2);
	solver.post(daysT[3]==4); solver.post(startTimesT[3]==10*2);
	solver.post(daysT[4]==6); solver.post(startTimesT[4]==9*2);
	solver.post(daysT[5]==2); solver.post(startTimesT[5]==12*2);
	solver.post(daysT[6]==6); solver.post(startTimesT[6]==27);
	solver.post(daysT[7]==3); solver.post(startTimesT[7]==14*2);
	solver.post(daysT[8]==2); solver.post(startTimesT[8]==19);
	solver.post(daysT[9]==6); solver.post(startTimesT[9]==9*2);
	solver.post(daysT[10]==3); solver.post(startTimesT[10]==9*2);
	solver.post(daysT[11]==3); solver.post(startTimesT[11]==9*2);
	solver.post(daysT[12]==5); solver.post(startTimesT[12]==12*2);
	solver.post(daysT[13]==6); solver.post(startTimesT[13]==9*2);
	solver.post(daysT[14]==2); solver.post(startTimesT[14]==14*2);
	solver.post(daysT[15]==5); solver.post(startTimesT[15]==16*2);
	solver.post(daysT[16]==5); solver.post(startTimesT[16]==12*2);
}

void solve(int nbSubjects, int nbClasses,
			const Util::StdStringArray& names, const Util::StdIntArray& nbTheoretical,
			const Util::StdIntArray& nbPractical,
			Util::StdIntArray2& collisions)
{
	const int hour = 2;

	Solver solver;

	// find max number of practical classes (max number of theoretical is assumed to be 1)
	int maxNbPractical = 0;
	for (int i = 0; i < nbSubjects; ++i)
		maxNbPractical = std::max(maxNbPractical,nbPractical[i]);

	// start times of classes indexed by subject
	IntVarArray 	startTimesT(solver,nbSubjects,8*hour,18*hour);
	IntVarArray2 	startTimesP(solver,nbSubjects,maxNbPractical,8*hour,18*hour);

	// days of classes indexed by subject
	IntVarArray 	daysT(solver,nbSubjects,2,6);
	IntVarArray2 	daysP(solver,nbSubjects,maxNbPractical,2,6);

	// for each subject, practical classes must form contiguous blocks of <= 6 hours,
	// and remaining must be contiguous to theoretical
	for (int s = 0; s < nbSubjects; ++s)
	{
		assert(nbTheoretical[s]==1);

		// constrain practical classes in each block of 3 classes
		for ( int b = 0; b < nbPractical[s]/3; ++b)
			for (int i = 1; i < 3; ++i)
			{
				// practical classes in the same block are on the same day
				solver.post(daysP[s][b*3] == daysP[s][b*3+i]);
				// and are contiguous
				solver.post(startTimesP[s][b*3]+2*hour*i == startTimesP[s][b*3+i]);
			}

		int remNbPractical = nbPractical[s] % 3;

		// if practical class do not fit in complete blocks of 3 classes
		if (remNbPractical > 0)
		{
			// constrain the remaining practical classes
			for (int i = 0; i < remNbPractical; ++i)
			{
				// must be on the same day as the theoretical class
				solver.post(daysT[s] == daysP[s][nbPractical[s]-remNbPractical+i]);
				// and are contiguous
				solver.post(startTimesT[s] + (2 + i*2)*hour ==
									startTimesP[s][nbPractical[s]-remNbPractical+i]);
			}
		}
		else
		// otherwise constrain isolated theoretical class to be earlier than all practical classes
		for ( int c = 0; c < nbPractical[s]; ++c)
			solver.post(daysT[s] < daysP[s][c]);
	}

	// there are no classes on wednesdays after 14
	for (int s = 0; s < nbSubjects; ++s)
	{
		// constrain theoretical class
		solver.post( (startTimesT[s] <= 12*hour) >= (daysT[s] == 4) );
		// constraint practical classes
		for ( int c = 0; c < nbPractical[s]; ++c)
			solver.post( (startTimesP[s][c] <= 12*hour) >= (daysP[s][c] == 4));
	}

	// cost function
	Util::StdList<IntVar> costTerms;

	int cc = 0;
	for (int s1 = 0; s1 < nbSubjects; ++s1)
		for (int s2 = s1+1; s2 < nbSubjects; ++s2)
			{
				int costTT;
				Util::StdArray<int> costTP(nbPractical[s2]);
				Util::StdArray<int> costPT(nbPractical[s1]);
				Util::StdArray<int,2> costPP(nbPractical[s1],nbPractical[s2]);
				calcCollisions(nbPractical,collisions,s1,s2,costTT,costTP,costPT,costPP);
				if (costTT>0)
				{
					IntVar costTerm(solver,0,costTT);
					solver.post(costTerm == costTT *
							cast<int>(daysT[s1]==daysT[s2] and
										abs(startTimesT[s1]-startTimesT[s2])< 2*hour));
					costTerms.pushBack(costTerm);
				}
				for (int i = 0; i < nbPractical[s2]; ++i)
					if (costTP[i]>0)
					{
						IntVar costTerm(solver,0,costTP[i]);
						solver.post(costTerm ==
								costTP[i]*cast<int>(daysT[s1]==daysP[s2][i] and
										abs(startTimesT[s1]-startTimesP[s2][i])< 2*hour));
						costTerms.pushBack(costTerm);
					}
				for (int i = 0; i < nbPractical[s1]; ++i)
					if (costPT[i]>0)
					{
						IntVar costTerm(solver,0,costPT[i]);
						solver.post(costTerm ==
								costPT[i]*cast<int>(daysP[s1][i]==daysT[s2] and
										abs(startTimesP[s1][i]-startTimesT[s2])< 2*hour));
						costTerms.pushBack(costTerm);
					}
				for (int i = 0; i < nbPractical[s1]; ++i)
					for (int j = 0; j < nbPractical[s2]; ++j)
						if (costPP[i][j]>0)
						{
							IntVar costTerm(solver,0,costPP[i][j]);
							solver.post(costTerm ==
									costPP[i][j]*cast<int>(daysP[s1][i]==daysP[s2][j] and
											abs(startTimesP[s1][i]-startTimesP[s2][j])< 2*hour));
							costTerms.pushBack(costTerm);
						}
			}
	IntVar negCost(solver,-100000,0);
	costTerms.pushBack(negCost);
	solver.post(sumEqual(costTerms,0));

	IntVar cost(solver,0,100000);
	solver.post(cost == -negCost);

	//assignRealSolution(solver,startTimesT,startTimesP,daysT,daysP);

	cout << solver.valid() << " " << cost << endl;

	// collect all variables
	IntVarArray labelVars(solver,nbClasses*2);
	cc = 0;
	for (int s = 0; s < nbSubjects; ++s)
	{
		assert(nbTheoretical[s]==1);
		labelVars[cc++] = daysT[s];
		labelVars[cc++] = startTimesT[s];
		for (int i = 0; i < nbPractical[s]; ++i)
		{
			labelVars[cc++] = daysP[s][i];
			labelVars[cc++] = startTimesP[s][i];
		}
	}

	Objective<int> obj(solver);
	obj.set(cost);

#define LABELING 1

#if LABELING==1

	solver.setExplorer(lds(solver,6));

	bool found = solver.solve(
			label(solver,daysT,selectVarMinDom(solver,daysT),selectValsRand(solver,daysT)) and
			label(solver,labelVars));

#elif LABELING==2

	//solver.setExplorer(lds(solver,8));

	IntPar idx(solver),val(solver),i(solver),j(solver);

	const int maxHours = (20-8)*hour;
	IntVarArray timeCoord(solver,nbClasses,0,5*maxHours-1);
	for (int d = 2; d <= 6; ++d)
		for (int t = 8*hour; t < 20*hour; ++t)
		{
			cc = 0;
			for (int s = 0; s < nbSubjects; ++s)
			{
				solver.post( (daysT[s]==d and startTimesT[s]==t) == (timeCoord[cc++] == (d-2)*maxHours+t-8*hour));
				for (int p = 0; p < nbPractical[s]; ++p)
					solver.post( (daysP[s][p]==d and startTimesP[s][p]==t) ==
									(timeCoord[cc++] == (d-2)*maxHours+t-8*hour));
			}
		}

	IntVarArray classesPerTimeCoord(solver,5*(20-8)*hour,0,nbClasses);
	for (int d = 2; d <= 6; ++d)
		for (int t = 8*hour; t < 20*hour; ++t)
			solver.post(classesPerTimeCoord[(d-2)*maxHours+t-8*hour] ==
						sum(all(i,range(0,nbClasses-1),true,
								cast<int>(t >= timeCoord[i] and
										  t < timeCoord[i]+2*hour))));

	Goal varSelect(solver,
			assign(idx,
				argMin(i,nonGroundIdxs(solver,timeCoord),domainSize(solver,timeCoord[idx]))));
	Goal valSelect(solver,
			assign(val,
				argMin(i,domain(solver,timeCoord[idx]),
					sum(j,range(0,2*hour-1),minInDomain(solver,classesPerTimeCoord[i+j])))));

	bool found = solver.solve(
			label(solver,daysT,selectVarMinDom(solver,daysT),selectValsRand(solver,daysT)) and
			whileNotGround(timeCoord)
			(
					varSelect and
					valSelect and
					(post(solver,timeCoord[idx]==val) or post(solver,timeCoord[idx]!=val))
			) and label(solver,labelVars));
#endif


	if (!found)
		cout << "no solution\n";
	else
	while (found)
	{
		//cout << found << endl << startTimesT << endl << daysT << endl << startTimesP << endl << daysP << endl << cost << endl;
		printSchedule(nbSubjects,hour,names,nbTheoretical,nbPractical,
						startTimesT,startTimesP,daysT,daysP);
		cout << "cost = " << cost << endl;
		found = obj.decrease(1) and solver.next();
	}
}

int main(int argc, char** argv)
{
	if (argc==3)
	{
		int nbSubjects = readNbSubjects(argv[1]);

		Util::StdIntArray2		collisions(nbSubjects,nbSubjects,0);
		Util::StdIntArray  		nbTheoretical(nbSubjects,0);
		Util::StdIntArray		nbPractical(nbSubjects,0);
		Util::StdStringArray	names(nbSubjects,"");

		readScheduleData(argv[1],names,collisions);
		int nbClasses = readClassData(argv[2],nbSubjects,nbTheoretical,nbPractical);
		solve(nbSubjects,nbClasses,names,nbTheoretical,nbPractical,collisions);
	}
	else
		cerr << "usage: " << argv[0] << " collisions.data subjects.data\n";
}
#elif EX==16

#define CASPER_CP_SAFE_FD_DELTAS

#include <casper/kernel.h>
#include <casper/cp/int.h>
#include <casper/cp/int/support.h>
#include <iostream>
#include <fstream>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

using namespace Casper;
using namespace Casper::CP;

boost::mt19937 gen;

Util::StdList<int> generateRandSet(int lb, int ub)
{
	boost::uniform_int<> dist(0, 1);
	Util::StdList<int> l;
	for (int i = lb; i <= ub; ++i)
		if (dist(gen))
			l.pushBack(i);
	return l;
}

IntVar generateRandDomVar(Store& store, int lb, int ub)
{
	Util::StdList<int> l(generateRandSet(lb,ub));
	return IntVar(store,l.begin(),l.end());
}

struct RandLabelGen
{
	RandLabelGen() : eqtypedist(0,1),opdist0(0,1),opdist1(0,3)
	{}

	template<class Vars1>
	Goal operator()(Store& s1,DomArrayView<int,Vars1>& v1)
	{
		// NOTE: -2 is because last var is the projected one
		boost::uniform_int<> varidxdist(0, v1.size()-2);
		varidx = varidxdist(gen);
		boost::uniform_int<> valdist(v1[varidx]->min(), v1[varidx]->max());
		val = valdist(gen);
		eqtype = eqtypedist(gen);
		if (eqtype == 0)
			op = opdist0(gen);
		else
			op = opdist1(gen);

		if (eqtype == 0)
		{
			if (op == 0)
				return Goal(s1,
					   post(s1,v1[varidx].getObj()==val,false)	or
					   post(s1,v1[varidx].getObj()!=val,false));
			else
				return Goal(s1,
					   post(s1,v1[varidx].getObj()!=val,false) or
					   post(s1,v1[varidx].getObj()==val,false));
		}
		else
		{
			switch(op)
			{
				case 0:
					return Goal(s1,
						   post(s1,v1[varidx].getObj()>=val,false) or
						   post(s1,v1[varidx].getObj()<val,false));
				case 1:
					return Goal(s1,
						   post(s1,v1[varidx].getObj()<=val,false) or
						   post(s1,v1[varidx].getObj()>val,false));
				case 2:
					return Goal(s1,
						   post(s1,v1[varidx].getObj()<val,false) or
						   post(s1,v1[varidx].getObj()>=val,false));
				case 3:
					return Goal(s1,
						   post(s1,v1[varidx].getObj()>val,false) or
						   post(s1,v1[varidx].getObj()<=val,false));
			}
		}
	}

	template<class Vars1,class Vars2>
	Goal operator()(Store& s1,Store& s2,
					DomArrayView<int,Vars1>& v1,DomArrayView<int,Vars2>& v2)
	{
		// NOTE: -2 is because last var is the projected one
		boost::uniform_int<> varidxdist(0, v1.size()-2);
		varidx = varidxdist(gen);
		boost::uniform_int<> valdist(v1[varidx]->min(), v1[varidx]->max());
		val = valdist(gen);
		eqtype = eqtypedist(gen);
		if (eqtype == 0)
			op = opdist0(gen);
		else
			op = opdist1(gen);

		if (eqtype == 0)
		{
			if (op == 0)
				return Goal(s1,
					   (post(s1,v1[varidx].getObj()==val,false) and
						post(s2,v2[varidx].getObj()==val,false))	or
					   (post(s1,v1[varidx].getObj()!=val,false) and
						post(s2,v2[varidx].getObj()!=val,false)));
			else
				return Goal(s1,
					   (post(s1,v1[varidx].getObj()!=val,false) and
						post(s2,v2[varidx].getObj()!=val,false)) or
					   (post(s1,v1[varidx].getObj()==val,false) and
						post(s2,v2[varidx].getObj()==val,false)));
		}
		else
		{
			switch(op)
			{
				case 0:
					return Goal(s1,
						   (post(s1,v1[varidx].getObj()>=val,false) and
							post(s2,v2[varidx].getObj()>=val,false)) or
						   (post(s1,v1[varidx].getObj()<val,false) and
							post(s2,v2[varidx].getObj()<val,false)));
				case 1:
					return Goal(s1,
						   (post(s1,v1[varidx].getObj()<=val,false) and
							post(s2,v2[varidx].getObj()<=val,false)) or
						   (post(s1,v1[varidx].getObj()>val,false) and
							post(s2,v2[varidx].getObj()>val,false)));
				case 2:
					return Goal(s1,
						   (post(s1,v1[varidx].getObj()<val,false) and
							post(s2,v2[varidx].getObj()<val,false)) or
						   (post(s1,v1[varidx].getObj()>=val,false) and
							post(s2,v2[varidx].getObj()>=val,false)));
				case 3:
					return Goal(s1,
						   (post(s1,v1[varidx].getObj()>val,false) and
							post(s2,v2[varidx].getObj()>val,false)) or
						   (post(s1,v1[varidx].getObj()<=val,false) and
							post(s2,v2[varidx].getObj()<=val,false)));
			}
		}

	}
	int varidx;
	int val;
	int eqtype;
	int op;
	boost::uniform_int<> eqtypedist;
	boost::uniform_int<> opdist0;
	boost::uniform_int<> opdist1;
};

template<class Vars1,class Vars2>
struct DebugLabel : IGoal
{
	DebugLabel(Store& s1, Store& s2,
			const Vars1& v1, const Vars2& v2) : s1(s1),s2(s2),v1(s1,v1),v2(s2,v2) {}
	// v1 must be contained in v2 at all times
	bool bndsComplete()
	{
		for (int i = 0; i < v1.size(); ++i)
			if (!std::includes(v2[i]->begin(),v2[i]->end(),
							   v1[i]->begin(),v1[i]->end()))
				return false;
		return true;
	}
	// v1 must contain all solutions
	bool sound()
	{
		DFSExplorer dfs(s2);
		VarArray<int> vars(s2,v2.size());
		for (int i = 0; i < v2.size(); ++i)
			vars[i] = Var<int>(s2,&*v2[i]);
		bool f = dfs.explore(label(s2,vars));
		while (f)
		{
			for (int i = 0; i < v1.size(); ++i)
				if (!std::includes(v1[i]->begin(),v1[i]->end(),
								   v2[i]->begin(),v2[i]->end()))
					return false;
			f = dfs.resume();
		}
		return true;
	}

	bool hasSolutions()
	{
		DFSExplorer dfs(s2);
		VarArray<int> vars(s2,v2.size());
		for (int i = 0; i < v2.size(); ++i)
			vars[i] = Var<int>(s2,&*v2[i]);
		bool f = dfs.explore(label(s2,vars));
		return f;
	}

	Goal execute()
	{
		// propagate
		std::cout << "--- begin prop ---\n";
		std::cout << "initial domains: " << v1 << " , " << v2 << std::endl;
		bool f1 = s1.valid();
		std::cout << "final domains: " << v1 << " , " << v2 << std::endl;
		std::cout << "--- end prop (" << f1 << ")---\n";
		bool f2 = s2.valid();

		if (!f1 and !f2)
			return fail();

		// compare propagate() result
		if (!f2 and f1)
		{
			std::cout << "failed : project did not fail as it should.\n";
			throw "unexpected error";
		}

		if (!f1 and f2)
		{
			if (!hasSolutions())
				return fail();
			else
			{
				std::cout << "failed : project failed and it shouldn't.\n";
				throw "unexpected error";
			}
		}

		// TODO test soundness
		if (!sound())
		{
			std::cout << "failed: not sound\n";
			throw "unexpected error";
		}

		// compare resulting domains
		if (!bndsComplete())
		{
			std::cout << "failed: not bounds complete\n";
			throw "unexpected error";
		}

		// if all ground then terminate branch
		if (v1.ground())
			return succeed();


		// copy all domains to v2
		for (int i = 0; i < v1.size(); ++i)
			v2[i]->intersect(v1[i]->begin(),v1[i]->end());
		f2 = s2.valid();
		assert(f2);


		Goal g(labelGen(s1,s2,v1,v2));

		return Goal(s1,g and Goal(this));
	}
	Store& s1;
	Store& s2;
	DomArrayView<int,Vars1> v1;
	DomArrayView<int,Vars2> v2;
	RandLabelGen labelGen;
};

template<class Vars1,class Vars2>
Goal debugLabel(Store& s1, Store& s2,const Vars1& v1,const Vars2& v2)
{	return new (s1) DebugLabel<Vars1,Vars1>(s1,s2,v1,v2);	}

template<class Vars1>
struct RandLabel : IGoal
{
	RandLabel(Store& s1,const Vars1& v1) : s1(s1),v1(s1,v1) {}

	Goal execute()
	{
//		std::cout << " --- begin prop ---\n";
		if (!s1.valid())
		{
//			std::cout << " --- end prop (FAILED) ---\n";
			return Goal(false);
		}
//		std::cout << " --- end prop (Ok) ---\n";

		if (v1.ground())
			return succeed();

		return Goal(s1,labelGen(s1,v1) and Goal(this));
	}
	Store& s1;
	DomArrayView<int,Vars1> v1;
	RandLabelGen labelGen;
};

template<class Vars1>
Goal randLabel(Store& s1,const Vars1& v1)
{	return new (s1) RandLabel<Vars1>(s1,v1);	}

template<class Expr1,class Expr2>
struct ProjectBnd : IFilter
{
	ProjectBnd(Store& store,const Expr1& p1,const Expr2& p2)
		: IFilter(store),p1(store,p1), p2(store,p2)
		{}

	bool execute()
	{	return p1.updateMin(p2.min()) and p1.updateMax(p2.max());}
	void attach(INotifiable* s)
	{	p2.attach(s);	}
	void detach(INotifiable* s)
	{	 p2.detach(s);	}

	BndView<int,Expr1>	p1;
	BndView<int,Expr2>	p2;
};

template<class Expr1,class Expr2>
bool postProjectBnd(Store& s, const Expr1& e1, const Expr2& e2)
{	return s.post(new (s) ProjectBnd<Expr1,Expr2>(s,e1,e2));	}

bool testProjectRel(int testId)
{
	const int lb = 1;
	const int ub = 20;
	const int nvars = 3;

	std::vector<Util::StdList<int> > randSets(nvars);
	for (int i = 0; i < nvars; ++i)
		randSets[i] = generateRandSet(lb,ub);

	Env env;
	Store s1(env);
	Store s2(env);

	IntVarArray lv1(s1,nvars);
	IntVarArray lv2(s2,nvars);

	for (int i = 0; i < nvars; ++i)
	{
		if (randSets[i].empty())
			return false;
		lv1[i] = IntVar(s1,randSets[i].begin(),randSets[i].end());
		lv2[i] = IntVar(s2,randSets[i].begin(),randSets[i].end());
	}

	// change here
	auto r1 = lv1[0]+lv1[1];
	auto r2 = lv2[0]+lv2[1];

	std::cout << "test("<< testId << "): " << r1 << "=" << lv1[nvars-1] << " ... ";
	std::cout.flush();

	bool f1 = postProject<AC6>(s1,lv1[nvars-1],r1);
	bool f2 = postProjectBnd(s2,lv2[nvars-1],r2);
	assert(f1 and f2);

	DFSExplorer dfs(s1);
	bool f = dfs.explore(debugLabel(s1,s2,lv1,lv2));
	while (f)
	{	f = dfs.resume();	}
	return true;
}

struct BenchmarkStats
{
	int nbFails;
	int nbProps;
};

void benchmarkProjectRel(BenchmarkStats& stats)
{
	const int lb = 0;
	const int ub = 20;
	const int nvars = 4;

	std::vector<Util::StdList<int> > randSets(nvars);
	for (int i = 0; i < nvars; ++i)
		randSets[i] = generateRandSet(lb,ub);

	Env env;
	Store s1(env);

	IntVarArray lv1(s1,nvars);

	for (int i = 0; i < nvars; ++i)
	{
		if (randSets[i].empty())
			return;
		lv1[i] = IntVar(s1,randSets[i].begin(),randSets[i].end());
	}

	// change here
	auto r1 = lv1[0]+lv1[1]+lv1[2];

	std::cout << "benchmarking: " << r1 << "=" << lv1[nvars-1] << "...";
	std::cout.flush();

	bool f1 = postProject<AC3>(s1,lv1[nvars-1],r1);
	assert(f1);

	DFSExplorer dfs(s1);
	bool f = dfs.explore(randLabel(s1,lv1));
	while (f)
	{	f = dfs.resume();	}
	std::cout << "done\n";
	stats.nbFails = dfs.getStats().getNbFails();
	stats.nbProps = s1.getStats().getNbPropagations();
}


bool testProject(int nbTests)
{
	for (int i = 0; i < nbTests; ++i)
	{
		std::ofstream of("last_benchmark.gen");
		of << gen;
		of.close();

		if (testProjectRel(i+1))
			std::cout << "Ok\n";
		else
			--i;
	}
	return true;
}

void benchmarkProject(int nbProblems)
{
	Util::StdList<int> nbFails;
	Util::StdList<int> nbProps;
	for (int i = 0; i < nbProblems; ++i)
	{
		std::ofstream of("last_benchmark.gen");
		of << gen;
		of.close();

		BenchmarkStats stats;
		benchmarkProjectRel(stats);
		nbFails.pushBack(stats.nbFails);
		nbProps.pushBack(stats.nbProps);
	}
	// statistics
	int avgNbFails,avgNbProps;
	avgNbFails = avgNbProps = 0;
	for (auto it = nbFails.begin(); it != nbFails.end(); ++it)
		avgNbFails += *it;
	for (auto it = nbProps.begin(); it != nbProps.end(); ++it)
		avgNbProps += *it;
	avgNbFails /= nbProblems;
	avgNbProps /= nbProblems;
	int stdNbFails,stdNbProps;
	stdNbFails = stdNbProps = 0;
	for (auto it = nbFails.begin(); it != nbFails.end(); ++it)
		stdNbFails += ::pow(*it-avgNbFails,2);
	for (auto it = nbProps.begin(); it != nbProps.end(); ++it)
		stdNbProps += ::pow(*it-avgNbProps,2);
	stdNbFails = sqrt(stdNbFails/nbProblems);
	stdNbProps = sqrt(stdNbProps/nbProblems);
	std::cout << "average nb fails: " << avgNbFails << " +- " << stdNbFails << std::endl;
	std::cout << "average nb props: " << avgNbProps << " +- " << stdNbProps << std::endl;
}

int main(int argc, char** argv)
{
//	Solver solver;
//	IntVar v(solver,{2,5,6,7,8,9});
//	std::cout << v << std::endl;
//	v.domain().updateMin(7);

	if (argc>1)
	{
		std::ifstream ff(argv[1]);
		ff >> gen;
		ff.close();
	}
	std::cout << testProject(1000) << std::endl;
	//benchmarkProject(1000);

//	Solver solver;
//	IntVar v1(solver,{1,3,4,6,7,9});
//	IntVar v2(solver,{2,5,6,7,8,9,10});
//	IntVar v3(solver,{2,3,4,5,8,9});
//	IntVar r(solver,{3,4,6,8,10});
//	bool f = postProject<AC3>(solver,r,v1-v2) and solver.valid();
//	std::cout << f << std::endl;

//	IntVar v1(solver,{3});
//	IntVar v2(solver,{2});
//	std::cout << v1.domain().intersectFwd(v2.domain().begin(),v2.domain().end()) << std::endl;

//	Env env;
//	Store store(env);
//	IntVar v1(store,{1,3,6});
//	IntVar v2(store,{1,4});
//	IntVar v3(store,{1,3,5,6,7});
//	std::cout << v1 << " " << v2 << " " << v3 << std::endl;

//	ValuesView<int,Rel2<Sub,IntVar,IntVar> > vv(solver,rel<Sub>(v1,v2));
//	for (auto it = vv.begin(); it != vv.end(); ++it)
//		std::cout << *it << std::endl;
//	std::cout << "---\n";
//	for (auto it = vv.rbegin(); it != vv.rend(); ++it)
//		std::cout << *it << std::endl;

//	std::cout << (postProject<AC6>(store,v3,v1+v2) and store.valid()) << std::endl;
//	std::cout << v1 << " " << v2 << " " << v3 << std::endl;
//	std::cout << (store.post(v1!=3) and store.valid()) << std::endl;
//	std::cout << v1 << " " << v2 << " " << v3 << std::endl;
//	std::cout << (store.post(v1!=6) and store.valid()) << std::endl;
//	std::cout << v1 << " " << v2 << " " << v3 << std::endl;
//	std::cout << store.getStats() << std::endl;
//	std::cout << env.getStats() << std::endl;
}

#endif
