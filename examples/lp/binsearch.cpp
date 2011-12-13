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

#include <casper/kernel/goal.h>
#include <casper/kernel/ref/view.h>

#include <casper/util/container/stdrange.h>

#include <iostream>

#include <casper/cp/int.h>
#include <casper/cp/int/count/dom.h>
#include <casper/cp/solver.h>
#include <casper/cp/goal/whilenotground.h>

#include <casper/mip/store.h>
//#include <casper/mip/var.h>
//#include <casper/mip/view.h>

using namespace std;

#if 1

bool binsearchFD(int size, int maxint)
{
	using namespace Casper;
	using namespace Casper::CP;

	const int nbLoops = std::ceil(std::log(size+1)/std::log(2.0));
	const int maxInt = maxint;
	const int minInt = -maxInt;

	cout << "nbLoops=" << nbLoops << endl;

	Solver solver;
	IntVar 		x(solver,minInt,maxInt);
	IntVarArray tab(solver,size,minInt,maxInt);
	IntVarArray index(solver,nbLoops+1,-1,size-1);
	IntVarArray m(solver,nbLoops+1,0,size-1);
	IntVarArray l(solver,nbLoops+1,0,size-1);
	IntVarArray u(solver,nbLoops+1,0,size-1);

	// debug

//	solver.post( tab[0]==0 and tab[1]==3);
//	solver.post( x == 500 );

	// program

	solver.post( index[0] == -1 );
	solver.post( m[0] == 0 );
	solver.post( l[0] == 0 );
	solver.post( u[0] == size-1 );

	BoolVarArray b(solver,4*nbLoops,false,true);

	for (int i = 1 ; i < nbLoops+1; ++i)
	{
		solver.post( b[(i-1)*4+0] == (index[i-1] != -1) );
		solver.post( b[(i-1)*4+1] == (l[i-1] > u[i-1]) );
		solver.post( b[(i-1)*4+2] == (m[i]>=0 and tab[m[i]]==x) );
		solver.post( b[(i-1)*4+3] == (m[i]>=0 and tab[m[i]]>x) );

		solver.post( ifThenElse(b[(i-1)*4+0] or b[(i-1)*4+1],
								m[i] == m[i-1] and
								index[i] == index[i-1] and
								l[i] == l[i-1] and
								u[i] == u[i-1],
							m[i]==(l[i-1]+u[i-1])/2 and
							ifThenElse(b[(i-1)*4+2],
									index[i]==m[i] and
									u[i] == u[i-1] and
									l[i] == l[i-1],
									ifThenElse(b[(i-1)*4+3],
											u[i]==m[i]-1 and l[i]==l[i-1],
											l[i]==m[i]+1 and u[i]==u[i-1]) and
									index[i]==index[i-1]))
						 );

		// redundant
		solver.post( (index[i]>=0)==(x>=l[i] and x<=u[i]) );
	}

	// preconditions
	for (int i = 1 ; i < size; ++i)
		solver.post(tab[i]>=tab[i-1]);

	// (negated) post-conditions
	BndExpr<int> occ(solver,0);
	for (int i = 0; i < size; ++i)
		occ = occ + cast<int>(tab[i]==x);

	IntVar j(solver,minInt,maxInt);

#if 1
	solver.post( (occ>0 and index[nbLoops]==-1) or
				 (index[nbLoops]>-1 and tab[j]!=x and
				  (index[nbLoops]>-1 <= (index[nbLoops]==j))));
#else
	solver.post( not ifThenElse(index[nbLoops]==-1,
								occ==0,
								(index[nbLoops]>-1) <= (tab[j]==x)) );
#endif

#if 1
	// Channel boolean to int variables
	IntVarArray ib(solver,b.size(),0,1);
	for (uint i = 0; i < b.size(); ++i)
		solver.post( ib[i] == cast<int>(b[i]) );

	// collect labeling variables
	IntVarArray labelVars(solver,nbLoops*3+1+1+b.size());
	int c = 0;
	for (int i = 0; i < nbLoops; ++i)
	{
		labelVars[c++] = m[i+1];
		labelVars[c++] = l[i+1];
		labelVars[c++] = u[i+1];
		labelVars[c++] = ib[i*4+0];
		labelVars[c++] = ib[i*4+1];
		labelVars[c++] = ib[i*4+2];
		labelVars[c++] = ib[i*4+3];
	}
	labelVars[c++] = x;
	labelVars[c++] = j;

	// solve
	bool found = solver.solve( label(solver,labelVars) and
								label(solver,index) and
								label(solver,tab));
#else
    // collect labeling variables
    IntVarArray labelVars(solver,(nbLoops+1)*4+size+1+1);
    int c = 0;
    for (int i = 0; i < nbLoops+1; ++i)
    {
            labelVars[c++] = index[i];
            labelVars[c++] = m[i];
            labelVars[c++] = l[i];
            labelVars[c++] = u[i];
    }
    for (int i = 0; i < size; ++i)
            labelVars[c++] = tab[i];
    labelVars[c++] = x;
    labelVars[c++] = j;


    // solve
    bool found = solver.solve( label(solver,b) and
								label(solver,labelVars,
										selectVarMinDom(solver,labelVars),
										selectValMin(solver,labelVars)));
#endif

	if (found)
		cout << tab << " " << x << endl << index << endl << l << endl << m << endl << u << endl;
	else
		cout << "no solution found!" << endl;

	cout << solver.getStats() << endl;

	return found;
}


struct FuncNotifiable : Casper::INotifiable
{
	FuncNotifiable(const function<bool()>& func) : func(func) {}
	bool notify() {	return func();	}
	function<bool()> func;
};

bool binsearchFD_LP(int size, int maxint)
{
	using namespace Casper;

	const int nbLoops = std::ceil(std::log(size+1)/std::log(2.0));
	const int maxInt = maxint;
	const int minInt = -maxInt;

	cout << "nbLoops=" << nbLoops << endl;

	Env env;
	CP::Store 		cp(env);

	CP::IntVar 		x(cp,minInt,maxInt);
	CP::IntVarArray tab(cp,size,minInt,maxInt);
	CP::IntVarArray index(cp,nbLoops+1,-1,size-1);
	CP::IntVarArray m(cp,nbLoops+1,0,size-1);
	CP::IntVarArray l(cp,nbLoops+1,0,size-1);
	CP::IntVarArray u(cp,nbLoops+1,0,size-1);

	LP::Solver lp(env);
	LP::Var			lx(lp);
	LP::VarArray	ltab(lp,size);

	// program

	cp.post( index[0] == -1 );
	cp.post( m[0] == 0 );
	cp.post( l[0] == 0 );
	cp.post( u[0] == size-1 );

	CP::IntVarArray d(cp,nbLoops,0,3);

	for (int i = 1 ; i < nbLoops+1; ++i)
	{
		CP::BoolVarArray b(cp,3,false,true);

		cp.post( b[0] == (index[i-1] != -1 or l[i-1] > u[i-1]) );
		//cp.post( b[1] == (tab[m[i]]==x) );
		//cp.post( b[2] == (tab[m[i]]>x) );

		cp.post( (d[i-1] == 0) == (!b[0] and !b[1] and b[2]) );
		cp.post( (d[i-1] == 1) == (!b[0] and !b[1] and !b[2]) );
		cp.post( (d[i-1] == 2) == (!b[0] and b[1]) );
		cp.post( (d[i-1] == 3) == (b[0]) );

		cp.post( ifThenElse(b[0],
								m[i] == m[i-1] and
								index[i] == index[i-1] and
								l[i] == l[i-1] and
								u[i] == u[i-1],
							m[i]==(l[i-1]+u[i-1])/2 and
							ifThenElse(b[1],
									index[i]==m[i] and
									u[i] == u[i-1] and
									l[i] == l[i-1],
									ifThenElse(b[2],
											u[i]==m[i]-1 and l[i]==l[i-1],
											l[i]==m[i]+1 and u[i]==u[i-1]) and
									index[i]==index[i-1]))
						 );

		// redundant
		//cp.post( (index[i]>=0)==(x>=l[i] and x<=u[i]) );
	}

	DFSExplorer explorer(env);

	for (int i = 1 ; i < nbLoops+1; ++i)
	{
		INotifiable* demon = new (env) FuncNotifiable([&,i]()
				{
					if (!m[i].ground())
						return true;
					if (!d[i-1].ground())
						return true;

					if (d[i-1].domain().value() == 2)
						lp.post( ltab[m[i].domain().value()] == lx );
					else
					if (d[i-1].domain().value() == 0)
						lp.post(ltab[m[i].domain().value()] >= lx + 1.0);
					else
					if (d[i-1].domain().value() == 1)
						lp.post(ltab[m[i].domain().value()] <= lx - 1.0);
					else
					return true;

					return lp.solve();
				});
		m[i].domain().attachOnGround(demon);
		d[i-1].domain().attachOnGround(demon);
	}

	// preconditions
//	for (int i = 1 ; i < size; ++i)
//		cp.post(tab[i] >= tab[i-1]);
	for (int i = 1 ; i < size; ++i)
		lp.post(ltab[i] >= ltab[i-1]);

	// collect labeling variables
	CP::IntVarArray labelVars(cp,nbLoops*4);
	int c = 0;
	for (int i = 0; i < nbLoops; ++i)
	{
		labelVars[c++] = m[i+1];
		labelVars[c++] = l[i+1];
		labelVars[c++] = u[i+1];
		labelVars[c++] = d[i];
	}
	//labelVars[c++] = x;
	//labelVars[c++] = j;

	// solve
	IntRef i(env),v(env);
	Goal g(env, label(cp,labelVars) and
					function<Goal()>([&](){
						assert(index[nbLoops].ground());

						if (index[nbLoops].domain().value()==-1)
						{
							Goal r(fail());
							for (uint k = 0; k < ltab.size(); ++k)
								r = Goal(env,r or post(lp, lx == ltab[k]));
							return r;
//							IntRef k(env);
//							return Goal(env,tryAll(k,range(0,ltab.size()-1)) (
//											post(lp, lx == ltab[k])
//											));
						}
						if (index[nbLoops].domain().value()>=0)
							return Goal(env,
										post(lp,lx >= ltab[index[nbLoops].domain().value()] + 1.0) or
										post(lp,lx <= ltab[index[nbLoops].domain().value()] - 1.0));
						return succeed();
					}) and
					function<Goal()>([](){ cout << "labeled labelVars\n"; return succeed(); }) and
					label(cp,index) and
					label(cp,tab));


	bool found = cp.valid() and explorer.explore(g);

	if (found)
		cout << index << endl << l << endl << m << endl << u << endl << ltab << endl << lx << endl;
	else
		cout << "no solution found!" << endl;

	cout << "CP Stats:\n" << cp.getStats() << "\nLP Stats:\n" << lp.getStats() << endl;
	cout << "Explorer Stats:\n" << explorer.getStats() << endl;

	return found;
}

bool binsearchFD_MIP(int size)
{
	using namespace Casper;

	const int nbLoops = std::ceil(std::log(size+1)/std::log(2.0));

	cout << "nbLoops=" << nbLoops << endl;

	Env env;

	CP::Store 		cp(env);
	CP::IntVarArray index(cp,nbLoops+1,-1,size-1);
	CP::IntVarArray m(cp,nbLoops+1,0,size-1);
	CP::IntVarArray l(cp,nbLoops+1,0,size-1);
	CP::IntVarArray u(cp,nbLoops+1,0,size-1);

	LP::Solver lp(env);
	LP::Var			x(lp);
	LP::VarArray	tab(lp,size);

	// preconditions
	for (int i = 1 ; i < size; ++i)
		lp.post(tab[i] >= tab[i-1]);

	// program

	cp.post( index[0] == -1 );
	cp.post( m[0] == 0 );
	cp.post( l[0] == 0 );
	cp.post( u[0] == size-1 );

	CP::IntVarArray d(cp,nbLoops,0,3);

	MIP::Store mip(cp,lp);

	for (int i = 1 ; i < nbLoops+1; ++i)
	{
		cp.post( (d[i-1] == 0) == (index[i-1] != -1 or l[i-1] > u[i-1]) );

		mip.post( ifThenElse(d[i-1] == 1,tab[m[i]]==x,
					ifThenElse(d[i-1] == 2,tab[m[i]]>=x+1.0,
						ifThen(d[i-1]==3,tab[m[i]]<=x-1.0))));

		cp.post( ifThenElse(d[i-1] == 0,
								m[i] == m[i-1] and
								index[i] == index[i-1] and
								l[i] == l[i-1] and
								u[i] == u[i-1],
							m[i]==(l[i-1]+u[i-1])/2 and
							ifThenElse(d[i-1] == 1,
									index[i]==m[i] and
									u[i] == u[i-1] and
									l[i] == l[i-1],
									ifThenElse(d[i-1] == 2,
											u[i]==m[i]-1 and l[i]==l[i-1],
											l[i]==m[i]+1 and u[i]==u[i-1]) and
									index[i]==index[i-1]))
						 );
	}

	// post condition (negated)

	CP::IntVar diff(cp,0,2);
	CP::IntVar p(cp,0,tab.size()-1);

	mip.post(ifThenElse(diff==0,tab[index[nbLoops]]>=x+1.0,
				ifThenElse(diff==1,tab[index[nbLoops]]<=x-1.0,
									tab[p] == x)));

	cp.post( (diff==2) == (index[nbLoops]==-1) );

	// collect labeling variables

	CP::IntVarArray labelVars(cp,nbLoops*4+2);
	int c = 0;
	for (int i = 0; i < nbLoops; ++i)
	{
		labelVars[c++] = m[i+1];
		labelVars[c++] = l[i+1];
		labelVars[c++] = u[i+1];
		labelVars[c++] = d[i];
	}
	labelVars[c++] = diff;
	labelVars[c++] = p;

	// solving

	IntRef ip(env),vp(env),k(env);

//	Goal g(env,label(cp,labelVars,selectVarMinDom(cp,labelVars)));
//	Goal g(env,label(cp,labelVars,selectVarMinDom(cp,labelVars)) and validate(lp));
	Goal g(env,
			whileDo(not ground(labelVars))
			(
				selectMin(ip,range(0,labelVars.size()-1),not ground(labelVars[ip]),domainSize(labelVars[ip])) and
				selectFirst(vp,domain(labelVars[ip])) and
				(
						post(cp,labelVars[ip]==vp) or
						post(cp,labelVars[ip]!=vp)
				) and validate(mip)
			));

	DFSExplorer explorer(env);

	bool found = mip.valid() and  explorer.explore(g);

	if (found)
		cout << index << endl << l << endl << m << endl << u << endl << tab << endl << x << endl;
	else
		cout << "no solution found!" << endl;

	cout << "CP Stats:\n" << cp.getStats() << "\nLP Stats:\n" << lp.getStats() << endl;
	cout << "Explorer Stats:\n" << explorer.getStats() << endl;

	return found;
}

bool binsearchFD_MIP_2(int size)
{
	using namespace Casper;

	const int nbLoops = std::ceil(std::log(size+1)/std::log(2.0));

	cout << "nbLoops=" << nbLoops << endl;

	Env env;

	CP::Store 		cp(env);
	CP::IntVarArray index(cp,nbLoops+1,-1,size-1);
	CP::IntVarArray m(cp,nbLoops+1,0,size-1);
	CP::IntVarArray l(cp,nbLoops+1,0,size-1);
	CP::IntVarArray u(cp,nbLoops+1,0,size-1);

	LP::Solver lp(env);
	LP::Var			x(lp);
	LP::VarArray	tab(lp,size);

	// preconditions
	for (int i = 1 ; i < size; ++i)
		lp.post(tab[i] >= tab[i-1]);

	// program

	cp.post( index[0] == -1 );
	cp.post( m[0] == 0 );
	cp.post( l[0] == 0 );
	cp.post( u[0] == size-1 );

	CP::IntVarArray d(cp,nbLoops,0,3);

	MIP::Store mip(cp,lp);

	for (int i = 1 ; i < nbLoops+1; ++i)
	{
		cp.post( (d[i-1] == 0) == (index[i-1] != -1 or l[i-1] > u[i-1]) );

#if 0
		mip.post( ifThenElse(d[i-1] == 1,tab[m[i]]==x,
					ifThenElse(d[i-1] == 2,tab[m[i]]>=x+1.0,
						ifThen(d[i-1]==3,tab[m[i]]<=x-1.0))));
#else
		INotifiable* demon = new (env) FuncNotifiable([&,i]()
				{
					if (!d[i-1].ground())
						return true;

					if (d[i-1].domain().value() == 1)
					{
						lp.post( tab[m[i].domain().min()] <= x );
						lp.post( tab[m[i].domain().max()] >= x );
					}
					else
					if (d[i-1].domain().value() == 2)
						lp.post(tab[m[i].domain().max()] >= x + 1.0);
					else
					if (d[i-1].domain().value() == 3)
						lp.post(tab[m[i].domain().min()] <= x - 1.0);
					else
					return true;
					return lp.valid();
				});
		m[i].domain().attachOnBounds(demon);
		d[i-1].domain().attachOnGround(demon);
#endif
		cp.post( ifThenElse(d[i-1] == 0,
								m[i] == m[i-1] and
								index[i] == index[i-1] and
								l[i] == l[i-1] and
								u[i] == u[i-1],
							m[i]==(l[i-1]+u[i-1])/2 and
							ifThenElse(d[i-1] == 1,
									index[i]==m[i] and
									u[i] == u[i-1] and
									l[i] == l[i-1],
									ifThenElse(d[i-1] == 2,
											u[i]==m[i]-1 and l[i]==l[i-1],
											l[i]==m[i]+1 and u[i]==u[i-1]) and
									index[i]==index[i-1]))
						 );
	}

	// post condition (negated)

	CP::IntVar diff(cp,0,2);
	CP::IntVar p(cp,0,tab.size()-1);

#if 0
	mip.post(ifThenElse(diff==0,tab[index[nbLoops]]>=x+1.0,
				ifThenElse(diff==1,tab[index[nbLoops]]<=x-1.0,
									tab[p] == x)));
#else
	INotifiable* demon = new (env) FuncNotifiable([&]()
			{
				if (!diff.ground())
					return true;

				if (diff.domain().value() == 0)
					lp.post( tab[index[nbLoops].domain().max()] >= x+1.0 );
				else
				if (diff.domain().value() == 1)
					lp.post(tab[index[nbLoops].domain().min()] <= x - 1.0);
				else
				{
					lp.post(tab[p.domain().min()] <= x );
					lp.post(tab[p.domain().max()] >= x );
				}
				return lp.valid();
			});
	p.domain().attachOnBounds(demon);
	index[nbLoops].domain().attachOnBounds(demon);
	diff.domain().attachOnGround(demon);
#endif
	cp.post( (diff==2) == (index[nbLoops]==-1) );

	// collect labeling variables

	CP::IntVarArray labelVars(cp,nbLoops*4+2);
	int c = 0;
	for (int i = 0; i < nbLoops; ++i)
	{
		labelVars[c++] = m[i+1];
		labelVars[c++] = l[i+1];
		labelVars[c++] = u[i+1];
		labelVars[c++] = d[i];
	}
	labelVars[c++] = diff;
	labelVars[c++] = p;

	// solving

	IntRef ip(env),vp(env),k(env);

//	Goal g(env,label(cp,labelVars,selectVarMinDom(cp,labelVars)));
//	Goal g(env,label(cp,labelVars,selectVarMinDom(cp,labelVars)) and validate(lp));
	Goal g(env,
			whileNotGround(labelVars)
			(
				selectMin(ip,range(0,labelVars.size()-1),
							not ground(labelVars[ip]),domainSize(labelVars[k])) and
				selectFirst(vp,domain(labelVars[ip])) and
				(
						post(cp,labelVars[ip]==vp) or
						post(cp,labelVars[ip]!=vp)
				) and validate(mip)
			));

	DFSExplorer explorer(env);

	bool found = mip.valid() and  explorer.explore(g);

	if (found)
		cout << index << endl << l << endl << m << endl << u << endl << tab << endl << x << endl;
	else
		cout << "no solution found!" << endl;

	cout << "CP Stats:\n" << cp.getStats() << "\nLP Stats:\n" << lp.getStats() << endl;
	cout << "Explorer Stats:\n" << explorer.getStats() << endl;

	return found;
}

#endif

void debug()
{
	using namespace Casper;

	Env env;

	CP::Store  cp(env);
	LP::Solver  lp(env);
	MIP::Store mip(cp,lp);

	LP::VarArray	v(lp,3);
	CP::IntVar i(cp,0,2);

	CP::BoolVar b(mip,false,true);


	cout << mip.post( ifThenElse(i==1,v[i] <= v[i-1]-1.0,v[i] >= v[i-1]+1.0) ) << endl;
//	cout << mip.post( b == false ) << endl;

	cout << cp.post( i == 2 ) << endl;

	cout << mip.valid() << endl;
	cout << v << endl;
}

int main(int argc,char** argv)
{
	if (argc!=2 and argc!=3)
		cout << "usage: " << argv[0] << " size [maxint]\n";
	else
		binsearchFD_MIP_2(atoi(argv[1]));
	return 0;
//	debug();
}
