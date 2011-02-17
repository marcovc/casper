/***************************************************************************
 *   Copyright (C) 2007/2009 by Marco Correia                              *
 *   mvc@di.fct.unl.pt                               					   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <iostream>
#include <casper/kernel.h>
#include <casper/real.h>
#include <casper/int.h>

using namespace casper;
using namespace std;


///	Enforces the Greater binary relation between two double views
template<class View1,class View2>
struct GreaterNotSound : IFilter
{
	GreaterNotSound(ICPSolver& solver, const View1& p1,const View2& p2) :
		IFilter(solver),p1(solver,p1),p2(solver,p2) {}
	Bool execute()
	{	return p1.updateMin(detail::succ(p2.min())) and
			   p2.updateMax(detail::pred(p1.max())); }
	bool entailed() const
	{	return p1.min() > p2.max();	}
	Filter	operator!()
	{	return Bnd(this->p1.getView() <= this->p2.getView());	}
	void attach(INotifiable* s)
	{	p1.attach(s); p2.attach(s); }
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}
	BndView<Double,View1> p1;
	BndView<Double,View2> p2;
};

template<class T1,class T2>
Filter greaterNotSound(const T1& t1, const T2& t2)
{	return new (getCPSolver(t1,t2).heap()) GreaterNotSound<T1,T2>(getCPSolver(t1,t2),t1,t2);	}

///	Enforces the Greater binary relation between two double views
template<class View1,class View2>
struct DistinctNotSound : IFilter
{
	DistinctNotSound(ICPSolver& solver, const View1& p1,const View2& p2) :
		IFilter(solver),p1(solver,p1),p2(solver,p2) {}
	Bool execute()
	{
		if (p1.min()==p1.max() or detail::isCanonical(p1.min(),p1.max()))
		{
			if (p2.min()>=p1.min() and !p2.updateMin(detail::succ(p1.max())))
				return false;
			if (p2.max()<=p1.max() and !p2.updateMax(detail::pred(p1.min())))
				return false;
		}
		if (p2.min()==p2.max() or detail::isCanonical(p2.min(),p2.max()))
		{
			if (p1.min()>=p2.min() and !p1.updateMin(detail::succ(p2.max())))
				return false;
			if (p1.max()<=p2.max() and !p1.updateMax(detail::pred(p2.min())))
				return false;
		}
		return true;
	}
	bool entailed() const
	{	return p1.min() > p2.max() or p2.max() < p1.min();	}
	Filter	operator!()
	{	return Bnd(p1.getView() == p2.getView());	}
	void attach(INotifiable* s)
	{	p1.attach(s); p2.attach(s); }
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}
	BndView<Double,View1> p1;
	BndView<Double,View2> p2;
};

template<class T1,class T2>
Filter distinctNotSound(const T1& t1, const T2& t2)
{	return new (getCPSolver(t1,t2).heap()) DistinctNotSound<T1,T2>(getCPSolver(t1,t2),t1,t2);	}

#if 0
Void mutual3FixedDenom()
{
	ICPSolver& solver;
	DomVarArray<Int> s(solver,8,0,2000);

	solver.post( 2000*(s[0]+s[1]) == (s[0]+s[1]+s[2]+s[3])*(s[0]+s[1]+s[4]+s[5]) );
	solver.post( 2000*(s[0]+s[2]) == (s[0]+s[1]+s[2]+s[3])*(s[0]+s[2]+s[4]+s[6]) );
	solver.post( 2000*(s[0]+s[4]) == (s[0]+s[1]+s[4]+s[5])*(s[0]+s[2]+s[4]+s[6]) );
	solver.post( 2000*2000*s[0] != (s[0]+s[1]+s[2]+s[3])*(s[0]+s[1]+s[4]+s[5])*(s[0]+s[2]+s[4]+s[6]));
	solver.post( s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6]+s[7] == 2000);

	cout << solver.solve() << endl;
	cout << "before search: " << s << endl;

	bool found = solver.solve( label(s,selectVarLexRR(s),
								  selectHalfMin(s)) );

	UInt solutions = 0;
	while (found)
	{
		cout << "solution " << ++solutions << ":" << s << endl;
		found = solver.next();
	}
}

Void mutual3()
{
	ICPSolver& solver;
	const Int largeInt = 4000;

	DomVarArray<Int> p(solver,3,0,largeInt);
	DomVarArray<Int> s(solver,8,0,largeInt);
	DomVar<Int> d(solver,1,largeInt);
	DomVar<Int> dd(solver,1,largeInt);

	solver.post(dd == d*d);
	for (UInt i = 0; i < 8; i++)
		solver.post(s[i] <= d);

	solver.post(p[0]==s[0]+s[1]+s[2]+s[3]);
	solver.post(p[1]==s[0]+s[1]+s[4]+s[5]);
	solver.post(p[2]==s[0]+s[2]+s[4]+s[6]);
	solver.post((s[0]+s[1])*d==p[0]*p[1]);
	solver.post((s[0]+s[2])*d==p[0]*p[2]);
	solver.post((s[0]+s[4])*d==p[1]*p[2]);
	solver.post((s[0])*dd!=p[0]*p[1]*p[2]);
	solver.post(s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6]+s[7]==d);

	// symmetry breaking

	solver.post(p[1]>=p[0]);
	solver.post(p[2]>=p[1]);

	// labeling

	DomVarArray<Int> vars(solver,9);
	for (UInt i = 0; i < 8; i++)
		vars[i] = s[i];
	vars[8] = d;

	cout << solver.solve() << endl;
	cout << "before search: " << s << endl;

	bool found = solver.solve( label(vars,selectVarLexRR(vars),
								  bisectHalfMin(vars)) );

	UInt solutions = 0;
	while (found)
	{
		cout << "solution " << ++solutions << ":" << vars << endl;
		found = solver.next();
	}
}

Void mutual4()
{
	ICPSolver& solver;
	const Int largeInt = 1000;

	DomVarArray<Int> p(solver,4,0,largeInt);
	DomVarArray<Int> s(solver,16,0,largeInt);
	DomVar<Int> d(solver,1,largeInt);
	DomVar<Int> dd(solver,1,largeInt);
	DomVar<Int> ddd(solver,1,largeInt);

	solver.post(dd == d*d);
	solver.post(ddd == dd*d);
	for (UInt i = 0; i < 16; i++)
		solver.post(s[i] <= d);

	solver.post(p[0]==s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6]+s[7]);
	solver.post(p[1]==s[0]+s[1]+s[2]+s[3]+s[8]+s[9]+s[10]+s[11]);
	solver.post(p[2]==s[0]+s[1]+s[4]+s[5]+s[8]+s[9]+s[12]+s[13]);
	solver.post(p[3]==s[0]+s[2]+s[4]+s[6]+s[8]+s[10]+s[12]+s[14]);
	solver.post((s[0]+s[1]+s[2]+s[3])*d==p[0]*p[1]);
	solver.post((s[0]+s[1]+s[4]+s[5])*d==p[0]*p[2]);
	solver.post((s[0]+s[2]+s[4]+s[6])*d==p[0]*p[3]);
	solver.post((s[0]+s[1]+s[8]+s[9])*d==p[1]*p[2]);
	solver.post((s[0]+s[8]+s[2]+s[10])*d==p[1]*p[3]);
	solver.post((s[0]+s[8]+s[4]+s[12])*d==p[2]*p[3]);
/*	solver.post((s[0]+s[1])*dd==p[0]*p[1]*p[2]);
	solver.post((s[0]+s[2])*dd==p[0]*p[1]*p[3]);
	solver.post((s[0]+s[4])*dd==p[0]*p[2]*p[3]);
	solver.post((s[0]+s[8])*dd==p[1]*p[2]*p[3]);
*/	solver.post((s[0])*ddd!=p[0]*p[1]*p[2]*p[3]);
	solver.post(s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6]+s[7]+s[8]+s[9]+s[10]+s[11]+s[12]+s[13]+s[14]+s[15]==d);

	// symmetry breaking

	solver.post(p[1]>=p[0]);
	solver.post(p[2]>=p[1]);
	solver.post(p[3]>=p[2]);

	// labeling

	DomVarArray<Int> vars(solver,17);
	for (UInt i = 0; i < 16; i++)
		vars[i] = s[i];
	vars[16] = d;

	cout << solver.solve() << endl;
	cout << "before search: " << s << endl;

	bool found = solver.solve( label(vars,selectVarLexRR(vars),
								  bisectHalfMin(vars)) );

	UInt solutions = 0;
	while (found)
	{
		cout << "solution " << ++solutions << ":" << vars << endl;
		found = solver.next();
	}
}

Void greg()
{
	ICPSolver& solver;
	const Int dmax = 3000;

	// declarations

	DomVarArray<Int> s(solver,16,0,dmax);
	DomVar<Int> d(solver,1,dmax);

	DomVar<Int> Pa0(solver,0,dmax);
	DomVar<Int> Pa1(solver,0,dmax);
	DomVar<Int> Pb0(solver,0,dmax);
	DomVar<Int> Pb1(solver,0,dmax);
	DomVar<Int> Pf0(solver,0,dmax);
	DomVar<Int> Pf1(solver,0,dmax);
	DomVar<Int> Ph0(solver,0,dmax);
	DomVar<Int> Ph1(solver,0,dmax);
	DomVar<Int> Pa0b0(solver,0,dmax);
	DomVar<Int> Pa0b1(solver,0,dmax);
	DomVar<Int> Pa1b0(solver,0,dmax);
	DomVar<Int> Pa1b1(solver,0,dmax);
	DomVar<Int> Pa0f0(solver,0,dmax);
	DomVar<Int> Pa0f1(solver,0,dmax);
	DomVar<Int> Pa1f0(solver,0,dmax);
	DomVar<Int> Pa1f1(solver,0,dmax);
	DomVar<Int> Pa0h0(solver,0,dmax);
	DomVar<Int> Pa0h1(solver,0,dmax);
	DomVar<Int> Pa1h0(solver,0,dmax);
	DomVar<Int> Pa1h1(solver,0,dmax);
	DomVar<Int> Pb0f0(solver,0,dmax);
	DomVar<Int> Pb0f1(solver,0,dmax);
	DomVar<Int> Pb1f0(solver,0,dmax);
	DomVar<Int> Pb1f1(solver,0,dmax);
	DomVar<Int> Pb0h0(solver,0,dmax);
	DomVar<Int> Pb0h1(solver,0,dmax);
	DomVar<Int> Pb1h0(solver,0,dmax);
	DomVar<Int> Pb1h1(solver,0,dmax);
	DomVar<Int> Pf0h0(solver,0,dmax);
	DomVar<Int> Pf0h1(solver,0,dmax);
	DomVar<Int> Pf1h0(solver,0,dmax);
	DomVar<Int> Pf1h1(solver,0,dmax);
	DomVar<Int> Pa0b0f0(solver,0,dmax);
	DomVar<Int> Pa0b0f1(solver,0,dmax);
	DomVar<Int> Pa0b1f0(solver,0,dmax);
	DomVar<Int> Pa0b1f1(solver,0,dmax);
	DomVar<Int> Pa1b0f0(solver,0,dmax);
	DomVar<Int> Pa1b0f1(solver,0,dmax);
	DomVar<Int> Pa1b1f0(solver,0,dmax);
	DomVar<Int> Pa1b1f1(solver,0,dmax);
	DomVar<Int> Pa0b0h0(solver,0,dmax);
	DomVar<Int> Pa0b0h1(solver,0,dmax);
	DomVar<Int> Pa0b1h0(solver,0,dmax);
	DomVar<Int> Pa0b1h1(solver,0,dmax);
	DomVar<Int> Pa1b0h0(solver,0,dmax);
	DomVar<Int> Pa1b0h1(solver,0,dmax);
	DomVar<Int> Pa1b1h0(solver,0,dmax);
	DomVar<Int> Pa1b1h1(solver,0,dmax);
	DomVar<Int> Pa0f0h0(solver,0,dmax);
	DomVar<Int> Pa0f0h1(solver,0,dmax);
	DomVar<Int> Pa0f1h0(solver,0,dmax);
	DomVar<Int> Pa0f1h1(solver,0,dmax);
	DomVar<Int> Pa1f0h0(solver,0,dmax);
	DomVar<Int> Pa1f0h1(solver,0,dmax);
	DomVar<Int> Pa1f1h0(solver,0,dmax);
	DomVar<Int> Pa1f1h1(solver,0,dmax);
	DomVar<Int> Pb0f0h0(solver,0,dmax);
	DomVar<Int> Pb0f0h1(solver,0,dmax);
	DomVar<Int> Pb0f1h0(solver,0,dmax);
	DomVar<Int> Pb0f1h1(solver,0,dmax);
	DomVar<Int> Pb1f0h0(solver,0,dmax);
	DomVar<Int> Pb1f0h1(solver,0,dmax);
	DomVar<Int> Pb1f1h0(solver,0,dmax);
	DomVar<Int> Pb1f1h1(solver,0,dmax);
	DomVar<Int> Pa0b0f0h0(solver,0,dmax);
	DomVar<Int> Pa0b0f0h1(solver,0,dmax);
	DomVar<Int> Pa0b0f1h0(solver,0,dmax);
	DomVar<Int> Pa0b0f1h1(solver,0,dmax);
	DomVar<Int> Pa0b1f0h0(solver,0,dmax);
	DomVar<Int> Pa0b1f0h1(solver,0,dmax);
	DomVar<Int> Pa0b1f1h0(solver,0,dmax);
	DomVar<Int> Pa0b1f1h1(solver,0,dmax);
	DomVar<Int> Pa1b0f0h0(solver,0,dmax);
	DomVar<Int> Pa1b0f0h1(solver,0,dmax);
	DomVar<Int> Pa1b0f1h0(solver,0,dmax);
	DomVar<Int> Pa1b0f1h1(solver,0,dmax);
	DomVar<Int> Pa1b1f0h0(solver,0,dmax);
	DomVar<Int> Pa1b1f0h1(solver,0,dmax);
	DomVar<Int> Pa1b1f1h0(solver,0,dmax);
	DomVar<Int> Pa1b1f1h1(solver,0,dmax);

	// state <-> probs

	solver.post(s[0] +s[1] +s[2] +s[3] +s[4] +s[5] +s[6] +s[7]  == Pa0);
	solver.post(s[8] +s[9] +s[10] +s[11] +s[12] +s[13] +s[14] +s[15]  == Pa1);
	solver.post(s[0] +s[1] +s[2] +s[3] +s[8] +s[9] +s[10] +s[11]  == Pb0);
	solver.post(s[4] +s[5] +s[6] +s[7] +s[12] +s[13] +s[14] +s[15]  == Pb1);
	solver.post(s[0] +s[1] +s[4] +s[5] +s[8] +s[9] +s[12] +s[13]  == Pf0);
	solver.post(s[2] +s[3] +s[6] +s[7] +s[10] +s[11] +s[14] +s[15]  == Pf1);
	solver.post(s[0] +s[2] +s[4] +s[6] +s[8] +s[10] +s[12] +s[14]  == Ph0);
	solver.post(s[1] +s[3] +s[5] +s[7] +s[9] +s[11] +s[13] +s[15]  == Ph1);
	solver.post(s[0] +s[1] +s[2] +s[3]  == Pa0b0);
	solver.post(s[4] +s[5] +s[6] +s[7]  == Pa0b1);
	solver.post(s[8] +s[9] +s[10] +s[11]  == Pa1b0);
	solver.post(s[12] +s[13] +s[14] +s[15]  == Pa1b1);
	solver.post(s[0] +s[1] +s[4] +s[5]  == Pa0f0);
	solver.post(s[2] +s[3] +s[6] +s[7]  == Pa0f1);
	solver.post(s[8] +s[9] +s[12] +s[13]  == Pa1f0);
	solver.post(s[10] +s[11] +s[14] +s[15]  == Pa1f1);
	solver.post(s[0] +s[2] +s[4] +s[6]  == Pa0h0);
	solver.post(s[1] +s[3] +s[5] +s[7]  == Pa0h1);
	solver.post(s[8] +s[10] +s[12] +s[14]  == Pa1h0);
	solver.post(s[9] +s[11] +s[13] +s[15]  == Pa1h1);
	solver.post(s[0] +s[1] +s[8] +s[9]  == Pb0f0);
	solver.post(s[2] +s[3] +s[10] +s[11]  == Pb0f1);
	solver.post(s[4] +s[5] +s[12] +s[13]  == Pb1f0);
	solver.post(s[6] +s[7] +s[14] +s[15]  == Pb1f1);
	solver.post(s[0] +s[2] +s[8] +s[10]  == Pb0h0);
	solver.post(s[1] +s[3] +s[9] +s[11]  == Pb0h1);
	solver.post(s[4] +s[6] +s[12] +s[14]  == Pb1h0);
	solver.post(s[5] +s[7] +s[13] +s[15]  == Pb1h1);
	solver.post(s[0] +s[4] +s[8] +s[12]  == Pf0h0);
	solver.post(s[1] +s[5] +s[9] +s[13]  == Pf0h1);
	solver.post(s[2] +s[6] +s[10] +s[14]  == Pf1h0);
	solver.post(s[3] +s[7] +s[11] +s[15]  == Pf1h1);
	solver.post(s[0] +s[1]  == Pa0b0f0);
	solver.post(s[2] +s[3]  == Pa0b0f1);
	solver.post(s[4] +s[5]  == Pa0b1f0);
	solver.post(s[6] +s[7]  == Pa0b1f1);
	solver.post(s[8] +s[9]  == Pa1b0f0);
	solver.post(s[10] +s[11]  == Pa1b0f1);
	solver.post(s[12] +s[13]  == Pa1b1f0);
	solver.post(s[14] +s[15]  == Pa1b1f1);
	solver.post(s[0] +s[2]  == Pa0b0h0);
	solver.post(s[1] +s[3]  == Pa0b0h1);
	solver.post(s[4] +s[6]  == Pa0b1h0);
	solver.post(s[5] +s[7]  == Pa0b1h1);
	solver.post(s[8] +s[10]  == Pa1b0h0);
	solver.post(s[9] +s[11]  == Pa1b0h1);
	solver.post(s[12] +s[14]  == Pa1b1h0);
	solver.post(s[13] +s[15]  == Pa1b1h1);
	solver.post(s[0] +s[4]  == Pa0f0h0);
	solver.post(s[1] +s[5]  == Pa0f0h1);
	solver.post(s[2] +s[6]  == Pa0f1h0);
	solver.post(s[3] +s[7]  == Pa0f1h1);
	solver.post(s[8] +s[12]  == Pa1f0h0);
	solver.post(s[9] +s[13]  == Pa1f0h1);
	solver.post(s[10] +s[14]  == Pa1f1h0);
	solver.post(s[11] +s[15]  == Pa1f1h1);
	solver.post(s[0] +s[8]  == Pb0f0h0);
	solver.post(s[1] +s[9]  == Pb0f0h1);
	solver.post(s[2] +s[10]  == Pb0f1h0);
	solver.post(s[3] +s[11]  == Pb0f1h1);
	solver.post(s[4] +s[12]  == Pb1f0h0);
	solver.post(s[5] +s[13]  == Pb1f0h1);
	solver.post(s[6] +s[14]  == Pb1f1h0);
	solver.post(s[7] +s[15]  == Pb1f1h1);
	solver.post(s[0]  == Pa0b0f0h0);
	solver.post(s[1]  == Pa0b0f0h1);
	solver.post(s[2]  == Pa0b0f1h0);
	solver.post(s[3]  == Pa0b0f1h1);
	solver.post(s[4]  == Pa0b1f0h0);
	solver.post(s[5]  == Pa0b1f0h1);
	solver.post(s[6]  == Pa0b1f1h0);
	solver.post(s[7]  == Pa0b1f1h1);
	solver.post(s[8]  == Pa1b0f0h0);
	solver.post(s[9]  == Pa1b0f0h1);
	solver.post(s[10]  == Pa1b0f1h0);
	solver.post(s[11]  == Pa1b0f1h1);
	solver.post(s[12]  == Pa1b1f0h0);
	solver.post(s[13]  == Pa1b1f0h1);
	solver.post(s[14]  == Pa1b1f1h0);
	solver.post(s[15]  == Pa1b1f1h1);

	// less than d

	solver.post(Pa0<=d);
	solver.post(Pa1<=d);
	solver.post(Pb0<=d);
	solver.post(Pb1<=d);
	solver.post(Pf0<=d);
	solver.post(Pf1<=d);
	solver.post(Ph0<=d);
	solver.post(Ph1<=d);
	solver.post(Pa0b0<=d);
	solver.post(Pa0b1<=d);
	solver.post(Pa1b0<=d);
	solver.post(Pa1b1<=d);
	solver.post(Pa0f0<=d);
	solver.post(Pa0f1<=d);
	solver.post(Pa1f0<=d);
	solver.post(Pa1f1<=d);
	solver.post(Pa0h0<=d);
	solver.post(Pa0h1<=d);
	solver.post(Pa1h0<=d);
	solver.post(Pa1h1<=d);
	solver.post(Pb0f0<=d);
	solver.post(Pb0f1<=d);
	solver.post(Pb1f0<=d);
	solver.post(Pb1f1<=d);
	solver.post(Pb0h0<=d);
	solver.post(Pb0h1<=d);
	solver.post(Pb1h0<=d);
	solver.post(Pb1h1<=d);
	solver.post(Pf0h0<=d);
	solver.post(Pf0h1<=d);
	solver.post(Pf1h0<=d);
	solver.post(Pf1h1<=d);
	solver.post(Pa0b0f0<=d);
	solver.post(Pa0b0f1<=d);
	solver.post(Pa0b1f0<=d);
	solver.post(Pa0b1f1<=d);
	solver.post(Pa1b0f0<=d);
	solver.post(Pa1b0f1<=d);
	solver.post(Pa1b1f0<=d);
	solver.post(Pa1b1f1<=d);
	solver.post(Pa0b0h0<=d);
	solver.post(Pa0b0h1<=d);
	solver.post(Pa0b1h0<=d);
	solver.post(Pa0b1h1<=d);
	solver.post(Pa1b0h0<=d);
	solver.post(Pa1b0h1<=d);
	solver.post(Pa1b1h0<=d);
	solver.post(Pa1b1h1<=d);
	solver.post(Pa0f0h0<=d);
	solver.post(Pa0f0h1<=d);
	solver.post(Pa0f1h0<=d);
	solver.post(Pa0f1h1<=d);
	solver.post(Pa1f0h0<=d);
	solver.post(Pa1f0h1<=d);
	solver.post(Pa1f1h0<=d);
	solver.post(Pa1f1h1<=d);
	solver.post(Pb0f0h0<=d);
	solver.post(Pb0f0h1<=d);
	solver.post(Pb0f1h0<=d);
	solver.post(Pb0f1h1<=d);
	solver.post(Pb1f0h0<=d);
	solver.post(Pb1f0h1<=d);
	solver.post(Pb1f1h0<=d);
	solver.post(Pb1f1h1<=d);
	solver.post(Pa0b0f0h0<=d);
	solver.post(Pa0b0f0h1<=d);
	solver.post(Pa0b0f1h0<=d);
	solver.post(Pa0b0f1h1<=d);
	solver.post(Pa0b1f0h0<=d);
	solver.post(Pa0b1f0h1<=d);
	solver.post(Pa0b1f1h0<=d);
	solver.post(Pa0b1f1h1<=d);
	solver.post(Pa1b0f0h0<=d);
	solver.post(Pa1b0f0h1<=d);
	solver.post(Pa1b0f1h0<=d);
	solver.post(Pa1b0f1h1<=d);
	solver.post(Pa1b1f0h0<=d);
	solver.post(Pa1b1f0h1<=d);
	solver.post(Pa1b1f1h0<=d);
	solver.post(Pa1b1f1h1<=d);

	// all states sum to d
	solver.post(s[0]+s[1]+s[2]+s[3]+
				s[4]+s[5]+s[6]+s[7]+
				s[8]+s[9]+s[10]+s[11]+
				s[12]+s[13]+s[14]+s[15] == d);

	// eq

	solver.post(Pa1 > 0);
	solver.post(Pa0 > 0);

	solver.post(Pb0 > 0);
	solver.post(Pb1 > 0);
	solver.post(Pf1 > 0);
	solver.post(Pf0 > 0);
	solver.post(Ph1 > 0);
	solver.post(Ph0 > 0);

	solver.post(Pa1b1 > 0);
	solver.post(Pa1b0 > 0);
	solver.post(Pa0b1 > 0);
	solver.post(Pa0b0 > 0);

	solver.post(Pa1f1 > 0);
	solver.post(Pa0f1 > 0);
	solver.post(Pa1f0 > 0);
	solver.post(Pa0f0 > 0);

	solver.post(Pa1h1 > 0);
	solver.post(Pa1h0 > 0);
	solver.post(Pa0h1 > 0);
	solver.post(Pa0h0 > 0);

	solver.post(Pb1f1 > 0);
	solver.post(Pb0f1 > 0);
	solver.post(Pb1f0 > 0);
	solver.post(Pb0f0 > 0);

	solver.post(Pb1h1 > 0);
	solver.post(Pb1h0 > 0);
	solver.post(Pb0h1 > 0);
	solver.post(Pb0h0 > 0);

	solver.post(Pf1h1 > 0);
	solver.post(Pf1h0 > 0);
	solver.post(Pf0h1 > 0);
	solver.post(Pf0h0 > 0);

	solver.post(Pa1*Pb1 != Pa1b1*d);

	solver.post(Pa1b1f1 * Pf1 != Pa1f1 * Pb1f1);
	solver.post(Pa1b1f0 * Pf0 != Pa1f0 * Pb1f0);

	solver.post(Pa1b1h1 * Ph1 == Pa1h1 * Pb1h1);
	solver.post(Pa1b1h0 * Ph0 == Pa1h0 * Pb1h0);

	solver.post(Pa1b1f1h1 * Pf1h1 == Pa1f1h1 * Pb1f1h1);
	solver.post(Pa1b1f1h0 * Pf1h0 == Pa1f1h0 * Pb1f1h0);
	solver.post(Pa1b1f0h1 * Pf0h1 == Pa1f0h1 * Pb1f0h1);
	solver.post(Pa1b1f0h0 * Pf0h0 == Pa1f0h0 * Pb1f0h0);

	solver.post(Pa1 * Pf1 == Pa1f1*d);
/*
	solver.post(Pa1b1f1 * Pb1 != Pa1b1 * Pb1f1);
	solver.post(Pa1b0f1 * Pb0 != Pa1b0 * Pb0f1);

	solver.post(Pa1f1h1 * Ph1 != Pa1h1 * Pf1h1);
	solver.post(Pa1f1h0 * Ph0 != Pa1h0 * Pf1h0);

	solver.post(Pa1b1f1h1 * Pb1h1  != Pa1b1h1 * Pb1f1h1);
	solver.post(Pa1b1f1h0 * Pb1h0  != Pa1b1h0 * Pb1f1h0);
	solver.post(Pa1b0f1h1 * Pb0h1  != Pa1b0h1 * Pb0f1h1);
	solver.post(Pa1b0f1h0 * Pb0h0  != Pa1b0h0 * Pb0f1h0);

	solver.post(Pb1f1*d != Pb1 * Pf1);

	solver.post(Pa1b1f1 * Pa1 != Pa1b1 * Pa1f1);
	solver.post(Pa0b1f1 * Pa0 != Pa0b1 * Pa0f1);

	solver.post(Pb1f1h1 * Ph1 == Pb1h1 * Pf1h1);
	solver.post(Pb1f1h0 * Ph0 == Pb1h0 * Pf1h0);

	solver.post(Pa1b1f1h1 * Pa1h1 == Pa1b1h1 * Pa1f1h1);
	solver.post(Pa1b1f1h0 * Pa1h0 == Pa1b1h0 * Pa1f1h0);
	solver.post(Pa0b1f1h1 * Pa0h1 == Pa0b1h1 * Pa0f1h1);
	solver.post(Pa0b1f1h0 * Pa0h0 == Pa0b1h0 * Pa0f1h0);

	solver.post(Pa1h1*d != Pa1 * Ph1);

	solver.post(Pa1b1h1 * Pb1 != Pa1b1 * Pb1h1);
	solver.post(Pa1b0h1 * Pb0 != Pa1b0 * Pb0h1);

	solver.post(Pa1f1h1 * Pf1 != Pa1f1 * Pf1h1);
	solver.post(Pa1f0h1 * Pf0 != Pa1f0 * Pf0h1);

	solver.post(Pa1b1f1h1 * Pb1f1 == Pa1b1f1 * Pb1f1h1);
	solver.post(Pa1b0f1h1 * Pb0f1 == Pa1b0f1 * Pb0f1h1);
	solver.post(Pa1b1f0h1 * Pb1f0 == Pa1b1f0 * Pb1f0h1);
	solver.post(Pa1b0f0h1 * Pb0f0 == Pa1b0f0 * Pb0f0h1);

	solver.post(Pb1h1*d != Pb1 * Ph1);

	solver.post(Pa1b1h1 * Pa1 != Pa1b1 * Pa1h1);
	solver.post(Pa0b1h1 * Pa0 != Pa0b1 * Pa0h1);

	solver.post(Pb1f1h1 * Pf1!= Pb1f1 * Pf1h1);
	solver.post(Pb1f0h1 * Pf0!= Pb1f0 * Pf0h1);

	solver.post(Pa1b1f1h1 * Pa1f1 != Pa1b1f1 * Pa1f1h1);
	solver.post(Pa0b1f1h1 * Pa0f1 != Pa0b1f1 * Pa0f1h1);
	solver.post(Pa1b1f0h1 * Pa1f0 != Pa1b1f0 * Pa1f0h1);
	solver.post(Pa0b1f0h1 * Pa0f0 != Pa0b1f0 * Pa0f0h1);

	solver.post(Pf1h1*d != Pf1 * Ph1);

	solver.post(Pb1f1h1 * Pb1 != Pb1f1 * Pb1h1);
	solver.post(Pb0f1h1 * Pb0 != Pb0f1 * Pb0h1);

	solver.post(Pa1f1h1 * Pa1 != Pa1f1 * Pa1h1);
	solver.post(Pa0f1h1 * Pa0 != Pa0f1 * Pa0h1);

	solver.post(Pa1b1f1h1 * Pa1b1 != Pa1b1f1 * Pa1b1h1);
	solver.post(Pa1b0f1h1 * Pa1b0 != Pa1b0f1 * Pa1b0h1);
	solver.post(Pa0b1f1h1 * Pa0b1 != Pa0b1f1 * Pa0b1h1);
	solver.post(Pa0b0f1h1 * Pa0b0 != Pa0b0f1 * Pa0b0h1);
*/
//	solver.post(Pa1h1*d > Pa1 * Ph1);
//	solver.post(Pa0h1*d < Pa0 * Ph1);
/*	solver.post(Pb1h1*d > Pb1 * Ph1);
	solver.post(Pb0h1*d < Pb0 * Ph1);
	solver.post(Pf1h1*d > Pf1 * Ph1);
	solver.post(Pf0h0*d < Pf0 * Ph1);
*/
	solver.post(Pa1h1 * Pb1 == Pb1h1 * Pa1);
	solver.post(Pa0h1 * Pb0 == Pb0h1 * Pa0);
	solver.post(Pb1h1 * Pf1 == Pf1h1 * Pb1);
	solver.post(Pb0h1 * Pf0 == Pf0h1 * Pb0);

	// labeling

	//d.domain().updateRange(1680,1680);

	DomVarArray<Int> vars(solver,17);
	for (UInt i = 0; i < 16; i++)
		vars[i] = s[i];
	vars[16] = d;

	cout << solver.solve() << endl;
	cout << "before search: " << s << endl;

	bool found = solver.solve( label(vars,selectVarFFRR(vars),
								  bisectHalfMin(vars)) );

	UInt solutions = 0;
	while (found)
	{
		cout << "solution " << ++solutions << ":" << vars << endl;
		found = solver.next();
	}

}
#endif


Void debug()
{
	ICPSolver& solver;

	DomVarArray<Double> s(solver,16,0,1);
	DomVar<Double> Pa1(solver,0,1);
	DomVar<Double> Pf1(solver,0,1);
	DomVar<Double> Pa1f1(solver,0,1);
	solver.post(s[8] +s[9] +s[10] +s[11] +s[12] +s[13] +s[14] +s[15]  == Pa1);
	solver.post(s[2] +s[3] +s[6] +s[7] +s[10] +s[11] +s[14] +s[15]  == Pf1);
	solver.post(s[10] +s[11] +s[14] +s[15]  == Pa1f1);
	solver.post(Pa1 * Pf1 == Pa1f1);
	//solver.post(Pa1>0.5);
	//solver.post(Pf1>0.5);
	//solver.post(Pa1f1>0.5);
	// all states sum to 1
	solver.post(s[0]+s[1]+s[2]+s[3]+
				s[4]+s[5]+s[6]+s[7]+
				s[8]+s[9]+s[10]+s[11]+
				s[12]+s[13]+s[14]+s[15] == 1.0);
	DomVarArray<Double> vars(solver,16+3);
	for (UInt i = 0; i < 16; ++i)
		vars[i] = s[i];
	vars[16] = Pa1;
	vars[17] = Pf1;
	vars[18] = Pa1f1;

	cout << solver.solve() << endl;
	cout << "before search: " << s << endl;

	bool found = solver.solve( label(vars,selectVarFFRR(vars),
								  bisectHalfMin(vars)) );

	UInt solutions = 0;
	while (found)
	{
		cout << "solution " << ++solutions << ":" << vars << endl;
		found = solver.next();
	}
}


struct BisectHalfMinUntil : IValSelector
{
	BisectHalfMinUntil(const DomVarArray<Double>& vars,Double size) :
		vars(vars),size(size) {}
	Goal select(UInt idx)
	{
		Double midpoint = detail::median(vars[idx].domain().min(),vars[idx].domain().max());
		Double maxSize = 0;
		for (UInt i = 0; i < vars.count(); ++i)
			if (vars(i).domain().size()>maxSize)
				maxSize = vars(i).domain().size();
		if (maxSize <= size)
			return vars[idx] <= midpoint;
		else
			return vars[idx] <= midpoint or
				   vars[idx] >= midpoint;
	}
	DomVarArray<Double> vars;
	Double size;
};

ValSelector bisectHalfMinUntil(const DomVarArray<Double>& vars, Double size)
{return new (vars.solver().heap()) BisectHalfMinUntil(vars,size);}

struct LabelUntil : IGoal
{
	LabelUntil(ICPSolver& solver, const DomVarArray<Double>& vars,
						VarSelector varSel,ValSelector valSel,
						const Double size) :
		IGoal(solver),vars(vars),
		varSel(varSel),valSel(valSel),
		size(size) {}

	Goal execute()
	{
		Int idx = varSel.select();
		if (idx < 0)
			return ok();
		if (vars[idx].domain().ground())
			return this;
		Double maxSize = 0;
		for (UInt i = 0; i < vars.count(); ++i)
			if (vars(i).domain().size()>maxSize)
				maxSize = vars(i).domain().size();
		if (maxSize <= size)
			return ok();
		return valSel.select(idx) and Goal(this);
	}
	DomVarArray<Double> vars;
	VarSelector varSel;
	ValSelector valSel;
	const Double size;
};

Goal labelUntil(const DomVarArray<Double>& vars, const VarSelector& varSel,
				const ValSelector& valSel, const Double size)
{	return new (vars.solver().heap()) LabelUntil(vars.solver(),vars,varSel,valSel,size);}

template<class T1,class T2>
Filter myDistinct(const T1& t1, const T2& t2)
{	return new (getCPSolver(t1,t2).heap()) DistinctNotSound<T1,T2>(getCPSolver(t1,t2),t1,t2);	}

template<class T1,class T2>
Filter myGreater(const T1& t1, const T2& t2)
{	return new (getCPSolver(t1,t2).heap()) GreaterNotSound<T1,T2>(getCPSolver(t1,t2),t1,t2);	}

Void gregReal()
{
	ICPSolver& solver;

	DomVarArray<Double> s(solver,16,0,1);

	DomVar<Double> Pa0(solver,0,1);
	DomVar<Double> Pa1(solver,0,1);
	DomVar<Double> Pb0(solver,0,1);
	DomVar<Double> Pb1(solver,0,1);
	DomVar<Double> Pf0(solver,0,1);
	DomVar<Double> Pf1(solver,0,1);
	DomVar<Double> Ph0(solver,0,1);
	DomVar<Double> Ph1(solver,0,1);
	DomVar<Double> Pa0b0(solver,0,1);
	DomVar<Double> Pa0b1(solver,0,1);
	DomVar<Double> Pa1b0(solver,0,1);
	DomVar<Double> Pa1b1(solver,0,1);
	DomVar<Double> Pa0f0(solver,0,1);
	DomVar<Double> Pa0f1(solver,0,1);
	DomVar<Double> Pa1f0(solver,0,1);
	DomVar<Double> Pa1f1(solver,0,1);
	DomVar<Double> Pa0h0(solver,0,1);
	DomVar<Double> Pa0h1(solver,0,1);
	DomVar<Double> Pa1h0(solver,0,1);
	DomVar<Double> Pa1h1(solver,0,1);
	DomVar<Double> Pb0f0(solver,0,1);
	DomVar<Double> Pb0f1(solver,0,1);
	DomVar<Double> Pb1f0(solver,0,1);
	DomVar<Double> Pb1f1(solver,0,1);
	DomVar<Double> Pb0h0(solver,0,1);
	DomVar<Double> Pb0h1(solver,0,1);
	DomVar<Double> Pb1h0(solver,0,1);
	DomVar<Double> Pb1h1(solver,0,1);
	DomVar<Double> Pf0h0(solver,0,1);
	DomVar<Double> Pf0h1(solver,0,1);
	DomVar<Double> Pf1h0(solver,0,1);
	DomVar<Double> Pf1h1(solver,0,1);
	DomVar<Double> Pa0b0f0(solver,0,1);
	DomVar<Double> Pa0b0f1(solver,0,1);
	DomVar<Double> Pa0b1f0(solver,0,1);
	DomVar<Double> Pa0b1f1(solver,0,1);
	DomVar<Double> Pa1b0f0(solver,0,1);
	DomVar<Double> Pa1b0f1(solver,0,1);
	DomVar<Double> Pa1b1f0(solver,0,1);
	DomVar<Double> Pa1b1f1(solver,0,1);
	DomVar<Double> Pa0b0h0(solver,0,1);
	DomVar<Double> Pa0b0h1(solver,0,1);
	DomVar<Double> Pa0b1h0(solver,0,1);
	DomVar<Double> Pa0b1h1(solver,0,1);
	DomVar<Double> Pa1b0h0(solver,0,1);
	DomVar<Double> Pa1b0h1(solver,0,1);
	DomVar<Double> Pa1b1h0(solver,0,1);
	DomVar<Double> Pa1b1h1(solver,0,1);
	DomVar<Double> Pa0f0h0(solver,0,1);
	DomVar<Double> Pa0f0h1(solver,0,1);
	DomVar<Double> Pa0f1h0(solver,0,1);
	DomVar<Double> Pa0f1h1(solver,0,1);
	DomVar<Double> Pa1f0h0(solver,0,1);
	DomVar<Double> Pa1f0h1(solver,0,1);
	DomVar<Double> Pa1f1h0(solver,0,1);
	DomVar<Double> Pa1f1h1(solver,0,1);
	DomVar<Double> Pb0f0h0(solver,0,1);
	DomVar<Double> Pb0f0h1(solver,0,1);
	DomVar<Double> Pb0f1h0(solver,0,1);
	DomVar<Double> Pb0f1h1(solver,0,1);
	DomVar<Double> Pb1f0h0(solver,0,1);
	DomVar<Double> Pb1f0h1(solver,0,1);
	DomVar<Double> Pb1f1h0(solver,0,1);
	DomVar<Double> Pb1f1h1(solver,0,1);
	DomVar<Double> Pa0b0f0h0(solver,0,1);
	DomVar<Double> Pa0b0f0h1(solver,0,1);
	DomVar<Double> Pa0b0f1h0(solver,0,1);
	DomVar<Double> Pa0b0f1h1(solver,0,1);
	DomVar<Double> Pa0b1f0h0(solver,0,1);
	DomVar<Double> Pa0b1f0h1(solver,0,1);
	DomVar<Double> Pa0b1f1h0(solver,0,1);
	DomVar<Double> Pa0b1f1h1(solver,0,1);
	DomVar<Double> Pa1b0f0h0(solver,0,1);
	DomVar<Double> Pa1b0f0h1(solver,0,1);
	DomVar<Double> Pa1b0f1h0(solver,0,1);
	DomVar<Double> Pa1b0f1h1(solver,0,1);
	DomVar<Double> Pa1b1f0h0(solver,0,1);
	DomVar<Double> Pa1b1f0h1(solver,0,1);
	DomVar<Double> Pa1b1f1h0(solver,0,1);
	DomVar<Double> Pa1b1f1h1(solver,0,1);

	// state <-> probs

	solver.post(s[0] +s[1] +s[2] +s[3] +s[4] +s[5] +s[6] +s[7]  == Pa0);
	solver.post(s[8] +s[9] +s[10] +s[11] +s[12] +s[13] +s[14] +s[15]  == Pa1);
	solver.post(s[0] +s[1] +s[2] +s[3] +s[8] +s[9] +s[10] +s[11]  == Pb0);
	solver.post(s[4] +s[5] +s[6] +s[7] +s[12] +s[13] +s[14] +s[15]  == Pb1);
	solver.post(s[0] +s[1] +s[4] +s[5] +s[8] +s[9] +s[12] +s[13]  == Pf0);
	solver.post(s[2] +s[3] +s[6] +s[7] +s[10] +s[11] +s[14] +s[15]  == Pf1);
	solver.post(s[0] +s[2] +s[4] +s[6] +s[8] +s[10] +s[12] +s[14]  == Ph0);
	solver.post(s[1] +s[3] +s[5] +s[7] +s[9] +s[11] +s[13] +s[15]  == Ph1);
	solver.post(s[0] +s[1] +s[2] +s[3]  == Pa0b0);
	solver.post(s[4] +s[5] +s[6] +s[7]  == Pa0b1);
	solver.post(s[8] +s[9] +s[10] +s[11]  == Pa1b0);
	solver.post(s[12] +s[13] +s[14] +s[15]  == Pa1b1);
	solver.post(s[0] +s[1] +s[4] +s[5]  == Pa0f0);
	solver.post(s[2] +s[3] +s[6] +s[7]  == Pa0f1);
	solver.post(s[8] +s[9] +s[12] +s[13]  == Pa1f0);
	solver.post(s[10] +s[11] +s[14] +s[15]  == Pa1f1);
	solver.post(s[0] +s[2] +s[4] +s[6]  == Pa0h0);
	solver.post(s[1] +s[3] +s[5] +s[7]  == Pa0h1);
	solver.post(s[8] +s[10] +s[12] +s[14]  == Pa1h0);
	solver.post(s[9] +s[11] +s[13] +s[15]  == Pa1h1);
	solver.post(s[0] +s[1] +s[8] +s[9]  == Pb0f0);
	solver.post(s[2] +s[3] +s[10] +s[11]  == Pb0f1);
	solver.post(s[4] +s[5] +s[12] +s[13]  == Pb1f0);
	solver.post(s[6] +s[7] +s[14] +s[15]  == Pb1f1);
	solver.post(s[0] +s[2] +s[8] +s[10]  == Pb0h0);
	solver.post(s[1] +s[3] +s[9] +s[11]  == Pb0h1);
	solver.post(s[4] +s[6] +s[12] +s[14]  == Pb1h0);
	solver.post(s[5] +s[7] +s[13] +s[15]  == Pb1h1);
	solver.post(s[0] +s[4] +s[8] +s[12]  == Pf0h0);
	solver.post(s[1] +s[5] +s[9] +s[13]  == Pf0h1);
	solver.post(s[2] +s[6] +s[10] +s[14]  == Pf1h0);
	solver.post(s[3] +s[7] +s[11] +s[15]  == Pf1h1);
	solver.post(s[0] +s[1]  == Pa0b0f0);
	solver.post(s[2] +s[3]  == Pa0b0f1);
	solver.post(s[4] +s[5]  == Pa0b1f0);
	solver.post(s[6] +s[7]  == Pa0b1f1);
	solver.post(s[8] +s[9]  == Pa1b0f0);
	solver.post(s[10] +s[11]  == Pa1b0f1);
	solver.post(s[12] +s[13]  == Pa1b1f0);
	solver.post(s[14] +s[15]  == Pa1b1f1);
	solver.post(s[0] +s[2]  == Pa0b0h0);
	solver.post(s[1] +s[3]  == Pa0b0h1);
	solver.post(s[4] +s[6]  == Pa0b1h0);
	solver.post(s[5] +s[7]  == Pa0b1h1);
	solver.post(s[8] +s[10]  == Pa1b0h0);
	solver.post(s[9] +s[11]  == Pa1b0h1);
	solver.post(s[12] +s[14]  == Pa1b1h0);
	solver.post(s[13] +s[15]  == Pa1b1h1);
	solver.post(s[0] +s[4]  == Pa0f0h0);
	solver.post(s[1] +s[5]  == Pa0f0h1);
	solver.post(s[2] +s[6]  == Pa0f1h0);
	solver.post(s[3] +s[7]  == Pa0f1h1);
	solver.post(s[8] +s[12]  == Pa1f0h0);
	solver.post(s[9] +s[13]  == Pa1f0h1);
	solver.post(s[10] +s[14]  == Pa1f1h0);
	solver.post(s[11] +s[15]  == Pa1f1h1);
	solver.post(s[0] +s[8]  == Pb0f0h0);
	solver.post(s[1] +s[9]  == Pb0f0h1);
	solver.post(s[2] +s[10]  == Pb0f1h0);
	solver.post(s[3] +s[11]  == Pb0f1h1);
	solver.post(s[4] +s[12]  == Pb1f0h0);
	solver.post(s[5] +s[13]  == Pb1f0h1);
	solver.post(s[6] +s[14]  == Pb1f1h0);
	solver.post(s[7] +s[15]  == Pb1f1h1);
	solver.post(s[0]  == Pa0b0f0h0);
	solver.post(s[1]  == Pa0b0f0h1);
	solver.post(s[2]  == Pa0b0f1h0);
	solver.post(s[3]  == Pa0b0f1h1);
	solver.post(s[4]  == Pa0b1f0h0);
	solver.post(s[5]  == Pa0b1f0h1);
	solver.post(s[6]  == Pa0b1f1h0);
	solver.post(s[7]  == Pa0b1f1h1);
	solver.post(s[8]  == Pa1b0f0h0);
	solver.post(s[9]  == Pa1b0f0h1);
	solver.post(s[10]  == Pa1b0f1h0);
	solver.post(s[11]  == Pa1b0f1h1);
	solver.post(s[12]  == Pa1b1f0h0);
	solver.post(s[13]  == Pa1b1f0h1);
	solver.post(s[14]  == Pa1b1f1h0);
	solver.post(s[15]  == Pa1b1f1h1);

	// all states sum to 1
	solver.post(s[0]+s[1]+s[2]+s[3]+
				s[4]+s[5]+s[6]+s[7]+
				s[8]+s[9]+s[10]+s[11]+
				s[12]+s[13]+s[14]+s[15] == 1.0);

	// eq

	solver.post(Pa1 > 0.0);
	solver.post(Pa0 > 0.0);

	solver.post(Pb0 > 0.0);
	solver.post(Pb1 > 0.0);
	solver.post(Pf1 > 0.0);
	solver.post(Pf0 > 0.0);
	solver.post(Ph1 > 0.0);
	solver.post(Ph0 > 0.0);

	solver.post(Pa1b1 > 0.0);
	solver.post(Pa1b0 > 0.0);
	solver.post(Pa0b1 > 0.0);
	solver.post(Pa0b0 > 0.0);

	solver.post(Pa1f1 > 0.0);
	solver.post(Pa0f1 > 0.0);
	solver.post(Pa1f0 > 0.0);
	solver.post(Pa0f0 > 0.0);

	solver.post(Pa1h1 > 0.0);
	solver.post(Pa1h0 > 0.0);
	solver.post(Pa0h1 > 0.0);
	solver.post(Pa0h0 > 0.0);

	solver.post(Pb1f1 > 0.0);
	solver.post(Pb0f1 > 0.0);
	solver.post(Pb1f0 > 0.0);
	solver.post(Pb0f0 > 0.0);

	solver.post(Pb1h1 > 0.0);
	solver.post(Pb1h0 > 0.0);
	solver.post(Pb0h1 > 0.0);
	solver.post(Pb0h0 > 0.0);

	solver.post(Pf1h1 > 0.0);
	solver.post(Pf1h0 > 0.0);
	solver.post(Pf0h1 > 0.0);
	solver.post(Pf0h0 > 0.0);

	solver.post(myDistinct(Pa1*Pb1,Pa1b1));

	solver.post(myDistinct(Pa1b1f1 * Pf1,Pa1f1 * Pb1f1));
	solver.post(myDistinct(Pa1b1f0 * Pf0,Pa1f0 * Pb1f0));

	solver.post(Pa1b1h1 * Ph1 == Pa1h1 * Pb1h1);
	solver.post(Pa1b1h0 * Ph0 == Pa1h0 * Pb1h0);

	solver.post(Pa1b1f1h1 * Pf1h1 == Pa1f1h1 * Pb1f1h1);
	solver.post(Pa1b1f1h0 * Pf1h0 == Pa1f1h0 * Pb1f1h0);
	solver.post(Pa1b1f0h1 * Pf0h1 == Pa1f0h1 * Pb1f0h1);
	solver.post(Pa1b1f0h0 * Pf0h0 == Pa1f0h0 * Pb1f0h0);

	solver.post(Pa1 * Pf1 == Pa1f1);


	solver.post(myDistinct(Pa1b1f1 * Pb1,Pa1b1 * Pb1f1));
	solver.post(myDistinct(Pa1b0f1 * Pb0,Pa1b0 * Pb0f1));

	solver.post(myDistinct(Pa1f1h1 * Ph1,Pa1h1 * Pf1h1));
	solver.post(myDistinct(Pa1f1h0 * Ph0,Pa1h0 * Pf1h0));

	solver.post(myDistinct(Pa1b1f1h1 * Pb1h1,Pa1b1h1 * Pb1f1h1));
	solver.post(myDistinct(Pa1b1f1h0 * Pb1h0,Pa1b1h0 * Pb1f1h0));
	solver.post(myDistinct(Pa1b0f1h1 * Pb0h1,Pa1b0h1 * Pb0f1h1));
	solver.post(myDistinct(Pa1b0f1h0 * Pb0h0,Pa1b0h0 * Pb0f1h0));

	solver.post(myDistinct(Pb1f1,Pb1 * Pf1));

	solver.post(myDistinct(Pa1b1f1 * Pa1,Pa1b1 * Pa1f1));
	solver.post(myDistinct(Pa0b1f1 * Pa0,Pa0b1 * Pa0f1));

	solver.post(Pb1f1h1 * Ph1 == Pb1h1 * Pf1h1);
	solver.post(Pb1f1h0 * Ph0 == Pb1h0 * Pf1h0);

	solver.post(Pa1b1f1h1 * Pa1h1 == Pa1b1h1 * Pa1f1h1);
	solver.post(Pa1b1f1h0 * Pa1h0 == Pa1b1h0 * Pa1f1h0);
	solver.post(Pa0b1f1h1 * Pa0h1 == Pa0b1h1 * Pa0f1h1);
	solver.post(Pa0b1f1h0 * Pa0h0 == Pa0b1h0 * Pa0f1h0);

	solver.post(myDistinct(Pa1h1,Pa1 * Ph1));

	solver.post(myDistinct(Pa1b1h1 * Pb1,Pa1b1 * Pb1h1));
	solver.post(myDistinct(Pa1b0h1 * Pb0,Pa1b0 * Pb0h1));

	solver.post(myDistinct(Pa1f1h1 * Pf1,Pa1f1 * Pf1h1));
	solver.post(myDistinct(Pa1f0h1 * Pf0,Pa1f0 * Pf0h1));

	solver.post(myDistinct(Pa1b1f1h1 * Pb1f1,Pa1b1f1 * Pb1f1h1));
	solver.post(myDistinct(Pa1b0f1h1 * Pb0f1,Pa1b0f1 * Pb0f1h1));
	solver.post(myDistinct(Pa1b1f0h1 * Pb1f0,Pa1b1f0 * Pb1f0h1));
	solver.post(myDistinct(Pa1b0f0h1 * Pb0f0,Pa1b0f0 * Pb0f0h1));

	solver.post(myDistinct(Pb1h1,Pb1 * Ph1));

	solver.post(myDistinct(Pa1b1h1 * Pa1,Pa1b1 * Pa1h1));
	solver.post(myDistinct(Pa0b1h1 * Pa0,Pa0b1 * Pa0h1));

	solver.post(myDistinct(Pb1f1h1 * Pf1,Pb1f1 * Pf1h1));
	solver.post(myDistinct(Pb1f0h1 * Pf0,Pb1f0 * Pf0h1));

	solver.post(myDistinct(Pa1b1f1h1 * Pa1f1,Pa1b1f1 * Pa1f1h1));
	solver.post(myDistinct(Pa0b1f1h1 * Pa0f1,Pa0b1f1 * Pa0f1h1));
	solver.post(myDistinct(Pa1b1f0h1 * Pa1f0,Pa1b1f0 * Pa1f0h1));
	solver.post(myDistinct(Pa0b1f0h1 * Pa0f0,Pa0b1f0 * Pa0f0h1));

	solver.post(myDistinct(Pf1h1,Pf1 * Ph1));

	solver.post(myDistinct(Pb1f1h1 * Pb1,Pb1f1 * Pb1h1));
	solver.post(myDistinct(Pb0f1h1 * Pb0,Pb0f1 * Pb0h1));

	solver.post(myDistinct(Pa1f1h1 * Pa1,Pa1f1 * Pa1h1));
	solver.post(myDistinct(Pa0f1h1 * Pa0,Pa0f1 * Pa0h1));

	solver.post(myDistinct(Pa1b1f1h1 * Pa1b1,Pa1b1f1 * Pa1b1h1));
	solver.post(myDistinct(Pa1b0f1h1 * Pa1b0,Pa1b0f1 * Pa1b0h1));
	solver.post(myDistinct(Pa0b1f1h1 * Pa0b1,Pa0b1f1 * Pa0b1h1));
	solver.post(myDistinct(Pa0b0f1h1 * Pa0b0,Pa0b0f1 * Pa0b0h1));

	solver.post(myGreater(Pa1h1,Pa1 * Ph1));
	solver.post(myGreater(Pa0 * Ph1,Pa0h1));
	solver.post(myGreater(Pb1h1,Pb1 * Ph1));
	solver.post(myGreater(Pb0 * Ph1,Pb0h1));
	solver.post(myGreater(Pf1h1,Pf1 * Ph1));
	solver.post(myGreater(Pf0 * Ph1,Pf0h1));

	solver.post(Pa1h1 * Pb1 == Pb1h1 * Pa1);
	solver.post(Pa0h1 * Pb0 == Pb0h1 * Pa0);
	solver.post(Pb1h1 * Pf1 == Pf1h1 * Pb1);
	solver.post(Pb0h1 * Pf0 == Pf0h1 * Pb0);

	// labeling
	Double d = 1E-5;
	solver.post(s[15]>=0.00700405572718865000-d);
	solver.post(s[14]>=0.01737949919282470000-d);
	solver.post(s[13]>=0.04847735865982570000-d);
	solver.post(s[12]>=0.02679048218140380000-d);
	solver.post(s[11]>=0.00670868107310696000-d);
	solver.post(s[10]>=0.05599638501903770000-d);
	solver.post(s[9]>=0.04643297414852670000-d);
	solver.post(s[8]>=0.08631837652116670000-d);
	solver.post(s[7]>=0.04847735865982570000-d);
	solver.post(s[6]>=0.02679048218140440000-d);
	solver.post(s[5]>=0.00466429656180786000-d);
	solver.post(s[4]>=0.11552427935880000000-d);
	solver.post(s[3]>=0.04643297414852670000-d);
	solver.post(s[2]>=0.08631837652116870000-d);
	solver.post(s[1]>=0.00446759410295530000-d);
	solver.post(s[0]>=0.37221682594243000000-d);

	solver.post(s[15]<=0.00700405572718865000+d);
	solver.post(s[14]<=0.01737949919282470000+d);
	solver.post(s[13]<=0.04847735865982570000+d);
	solver.post(s[12]<=0.02679048218140380000+d);
	solver.post(s[11]<=0.00670868107310696000+d);
	solver.post(s[10]<=0.05599638501903770000+d);
	solver.post(s[9]<=0.04643297414852670000+d);
	solver.post(s[8]<=0.08631837652116670000+d);
	solver.post(s[7]<=0.04847735865982570000+d);
	solver.post(s[6]<=0.02679048218140440000+d);
	solver.post(s[5]<=0.00466429656180786000+d);
	solver.post(s[4]<=0.11552427935880000000+d);
	solver.post(s[3]<=0.04643297414852670000+d);
	solver.post(s[2]<=0.08631837652116870000+d);
	solver.post(s[1]<=0.00446759410295530000+d);
	solver.post(s[0]<=0.37221682594243000000+d);

	cout << solver.solve() << endl;
	cout << "before search: " << s << endl;

	bool found = solver.solve( labelUntil(s,selectVarFFRR(s),
								  bisectHalfMin(s),1E-9) );
	cout << "after 1st search: " << found << endl;
	UInt solutions = 0;
	while (found)
	{
		cout << "solution " << ++solutions << ":" << s << endl;
		found = solver.next();
	}

}

Void mutual3Real()
{
#if 0
	boost::numeric::interval_lib::rounded_arith_std<Double> b;
	double v = 1.23941e-161;

	cout << (1.0 > b.sub_down(1.0,v)) << endl;


	cout << v << endl;
	mpfr_t r;
	mpfr_t x1;
	mpfr_t x2;
	mpfr_init(r);
	mpfr_init_set_d(x1, 1.0, GMP_RNDN);
    mpfr_init_set_d(x2, v, GMP_RNDN);
    mpfr_sub(r,x1, x2, GMP_RNDD);
	double res = mpfr_get_d(r, GMP_RNDD);
	mpfr_clear(r);
	mpfr_clear(x1);
	mpfr_clear(x2);
	cout << (res<1.0) << endl;

//	cout << detail::subLb(1.0,1.23941e-161) << endl;
//	cout << solver.solve() << " " << x << endl;
#endif

#if 1
	ICPSolver& solver;

	DomVarArray<Double> p(solver,3,0,1);
	DomVarArray<Double> s(solver,8,0,1);

	solver.post(p[0]==s[0]+s[1]+s[2]+s[3]);
	solver.post(p[1]==s[0]+s[1]+s[4]+s[5]);
	solver.post(p[2]==s[0]+s[2]+s[4]+s[6]);
	solver.post(s[0]+s[1]==p[0]*p[1]);
	solver.post(s[0]+s[2]==p[0]*p[2]);
	solver.post(s[0]+s[4]==p[1]*p[2]);
	solver.post(distinctNotSound(s[0],p[0]*p[1]*p[2]));
	solver.post(s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6]+s[7]==1.0);

	// symmetry breaking

	solver.post(p[1]>=p[0]);
	solver.post(p[2]>=p[1]);

	for (UInt n = 0; n < 8; n++)
		solver.post(greaterNotSound(s[n],0.1));

	// labeling

/*	DomVarArray<Double> vars(solver,8);
	for (UInt i = 0; i < 8; i++)
		vars[i] = s[i];
*/
	DomVarArray<Double> vars(solver,3);
	for (UInt i = 0; i < 3; i++)
		vars[i] = p[i];

	cout << solver.solve() << endl;
	cout << "before search: " << s << endl;

	bool found = solver.solve( label(vars,selectVarLexRR(vars),
								  bisectHalfMin(vars)) );

	UInt solutions = 0;
	while (found)
	{
		cout << "solution " << ++solutions << ":" << p << " : " << s << endl;
		//cout << (s[7].domain().min()) << endl;
		found = solver.next();
	}
#endif
}

Void mutual4Real()
{
	ICPSolver& solver;

	DomVarArray<Double> p(solver,4,0,1);
	DomVarArray<Double> s(solver,16,0,1);

	solver.post(p[0]==s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6]+s[7]);
	solver.post(p[1]==s[0]+s[1]+s[2]+s[3]+s[8]+s[9]+s[10]+s[11]);
	solver.post(p[2]==s[0]+s[1]+s[4]+s[5]+s[8]+s[9]+s[12]+s[13]);
	solver.post(p[3]==s[0]+s[2]+s[4]+s[6]+s[8]+s[10]+s[12]+s[14]);
	solver.post((s[0]+s[1]+s[2]+s[3])==p[0]*p[1]);
	solver.post((s[0]+s[1]+s[4]+s[5])==p[0]*p[2]);
	solver.post((s[0]+s[2]+s[4]+s[6])==p[0]*p[3]);
	solver.post((s[0]+s[1]+s[8]+s[9])==p[1]*p[2]);
	solver.post((s[0]+s[8]+s[2]+s[10])==p[1]*p[3]);
	solver.post((s[0]+s[8]+s[4]+s[12])==p[2]*p[3]);
	solver.post((s[0]+s[1])==p[0]*p[1]*p[2]);
	solver.post((s[0]+s[2])==p[0]*p[1]*p[3]);
	solver.post((s[0]+s[4])==p[0]*p[2]*p[3]);
	solver.post((s[0]+s[8])==p[1]*p[2]*p[3]);
	solver.post(distinctNotSound(s[0],p[0]*p[1]*p[2]*p[3]));
	solver.post(s[0]+s[1]+s[2]+s[3]+s[4]+s[5]+s[6]+s[7]+s[8]+s[9]+s[10]+s[11]+s[12]+s[13]+s[14]+s[15]==1.0);

	// symmetry breaking

	solver.post(p[1]>=p[0]);
	solver.post(p[2]>=p[1]);
	solver.post(p[3]>=p[2]);

	// labeling

	cout << solver.solve() << endl;
	cout << "before search: " << s << endl;

	bool found = solver.solve( labelUntil(s,selectVarFFRR(s),
			bisectHalfMin(s),0.0001) );

	UInt solutions = 0;
	if (found)
	{
		cout << "solution " << ++solutions << ":" << s << endl;
		//found = solver.next();
	}
	cout << solver.stats() << endl;
	cout << solver.totalTime() << endl;
}

int main()
{
	//mutual4Real();
	gregReal();
	/*
	try {
	gregReal();
	} catch (const EHeapOverflow& e)
	{
		cerr << e << endl;
	}*/
	//debug();
	return 0;
}

