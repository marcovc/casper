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

#define EX 14

#if EX==1

#include <casper/kernel.h>
#include <casper/util.h>
#include <iostream>

using namespace Casper;
using namespace std;

int main()
{
	State state;
	IntPar v(state);
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

	IntPar idx(env),val(env);
	Goal varSelect(store,
			assign(idx,
				argMin(i,nonGroundIdxs(store,v),
						domainSize(store,v[i]))));

	Goal valSelect(store,
			assign(val,
				argMax(i,domain(store,v[idx]),
						randInRange(0,n-1))));


	Goal searchTree(env,
			whileNotGround(v)
			(
				varSelect and valSelect and
				(
					post(store,v[idx] == val) or
					post(store,v[idx] != val)
				)
			));

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

#endif
