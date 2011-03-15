/***************************************************************************
 *   Copyright (C) 2007/2009 by Marco Correia                              *
 *   mvc@di.fct.unl.pt                               					   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#define CASPER_HASH_STD
#include <casper/kernel/reversible/hashmap.h>

#include <iostream>
#include <casper/real.h>

using namespace Casper;
using namespace std;


template<class T>
struct Center : IBndExpr<double>
{
	Center(CPSolver& solver,const T& v) :
		IBndExpr<double>(solver),v(solver,v) {}
	double min() const
	{
		double mi = Detail::divLb(Detail::addLb(v.min(),v.max()),2.0);
		double ma = Detail::divUb(Detail::addUb(v.min(),v.max()),2.0);
		if (mi>=v.min())
		{
	/*		cout << "v.min= " << v.min() << endl;
			cout << "v.max= " << v.max() << endl;
			cout << "ret " << mi << endl;
		*/	return mi;
		}
		else
		{
			assert(0);
			return ma;
		}
	}
	double max() const
	{	return min();	}
	bool updateMin(const double& v) {	return v<=max();}
	bool updateMax(const double& v) {	return v>=min();}
	void attach(INotifiable* f) { 	v.attach(f); }
	void detach(INotifiable* f) {	v.detach(f); }
	BndView<double,T> v;
};

template<class T>
BndExpr<double> center(const T& v)
{	return new (getState(v).getHeap()) Center<T>(getState(v),v);	}

// ------ newton -------

struct Newton : IFilter
{
	Newton(CPSolver& solver,const List<Var<double> >& vars,
						 const List<BndExpr<double> >& c,
						 const List<BndExpr<double> >& f,
						 const List<BndExpr<double> >& d) :
			IFilter(solver), vars(vars),c(c),f(f),d(d)
	{}

	bool execute()
	{
		List<Var<double> >::Iterator itv = vars.begin();
		List<BndExpr<double> >::Iterator itc = c.begin();
		List<BndExpr<double> >::Iterator itf = f.begin();
		List<BndExpr<double> >::Iterator itd = d.begin();
		for ( ; itv != vars.end(); ++itv, ++itc, ++itf, ++itd)
		{
			StdRange<double> rc(itc->min(),itc->max());
			StdRange<double> rf(itf->min(),itf->max());
			StdRange<double> rd(itd->min(),itd->max());
			double lb = Detail::subLb(rc.min(),itv->domain().max());
			double ub = Detail::subUb(rc.max(),itv->domain().min());
			Detail::divRange(rf,rd,lb,ub);
			if (lb>ub or
				!itv->domain().updateRange(Detail::subLb(rc.min(),ub),
										   Detail::subUb(rc.max(),lb)))
				return false;
		}
		return true;
	}

	void attach(INotifiable* f)
	{
		for (List<Var<double> >::Iterator itv = vars.begin();
				itv != vars.end(); ++itv)
			itv->domain().attachOnBounds(f);
	}
	void detach(INotifiable* f)
	{
		for (List<Var<double> >::Iterator itv = vars.begin();
				itv != vars.end(); ++itv)
			itv->domain().detachOnBounds(f);
	}
	Cost cost() const
	{	return linearHi; }

	List<Var<double> > vars;
	List<BndExpr<double> > c;
	List<BndExpr<double> > f;
	List<BndExpr<double> > d;
};
/*
Filter newton2(const List<Var<double> >& vars,
			  const List<BndExpr<double> >& c,
			  const List<BndExpr<double> >& f,
			  const List<BndExpr<double> >& d)
{
	CPSolver& solver(getState(vars));
	List<BndExpr<double> > vlst(solver.getHeap());
	for (List<Var<double> >::Iterator it = vars.begin();
				it != vars.end(); ++it)
		vlst.pushBack(BndExpr<double>(solver,*it));

	return new (solver.getHeap()) Newton(solver,vlst,c,f,d);
}*/

// ---

template<class T>
struct GetVars
{
	void operator()(const T& e,List<Var<double> >& l)
	{}
};

template<class T>
void getVars(const T& t,List<Var<double> >& l)
{	GetVars<T>()(t,l);	}

template<>
struct GetVars<Var<double> >
{
	void operator()(const Var<double>& e,List<Var<double> >& l)
	{	l.pushBack(e);	}
};

template<class Op,class T1>
struct GetVars<Rel1<Op,T1> >
{
	void operator()(const Rel1<Op,T1>& e,List<Var<double> >& l)
	{	getVars(e.p1,l);	}
};

template<class Op,class T1,class T2>
struct GetVars<Rel2<Op,T1,T2> >
{
	void operator()(const Rel2<Op,T1,T2>& e,List<Var<double> >& l)
	{	getVars(e.p1,l); getVars(e.p2,l);	}
};


// ---

template<class T>
struct ReplaceVarWithCenter
{
	BndExpr<double> operator()(const T& e, const Var<double>& s)
	{	return BndExpr<double>(s.solver(),e);	}
};

template<class T>
BndExpr<double> replaceVarWithCenter(const T& e,const Var<double>& s)
{
	return ReplaceVarWithCenter<T>()(e,s);
}

template<>
struct ReplaceVarWithCenter<Var<double> >
{
	BndExpr<double> operator()(const Var<double>& e, const Var<double>& s)
	{
		if (s.pDomain()==e.pDomain())
			return center(e);
		else
			return BndExpr<double>(s.solver(),e);
	}
};

template<class Op,class T1>
struct ReplaceVarWithCenter<Rel1<Op,T1> >
{
	BndExpr<double> operator()(const Rel1<Op,T1>& e, const Var<double>& s)
	{	return BndExpr<double>(s.solver(),rel<Op>(replaceVarWithCenter(e.p1,s)));	}
};

template<class Op,class T1,class T2>
struct ReplaceVarWithCenter<Rel2<Op,T1,T2> >
{
	BndExpr<double> operator()(const Rel2<Op,T1,T2>& e, const Var<double>& s)
	{	return BndExpr<double>(s.solver(),rel<Op>(replaceVarWithCenter(e.p1,s),
												  replaceVarWithCenter(e.p2,s)));	}
};

// ---

template<class T>
struct Derivative;

template<class T>
BndExpr<double> derivative(const T& v,const Var<double>& s)
{	return Derivative<T>()(v,s);	}

// c' = 0
template<>
struct Derivative<double>
{
	BndExpr<double> operator()(const double& e, const Var<double>& s)
	{	return BndExpr<double>(s.solver(),0.0);		}
};

// x' = 1 or 0
template<>
struct Derivative<Var<double> >
{
	BndExpr<double> operator()(const Var<double>& e, const Var<double>& s)
	{
		if (s.pDomain()==e.pDomain())
			return BndExpr<double>(s.solver(),1.0);
		else
			return BndExpr<double>(s.solver(),0.0);
	}
};

// (f+g)' = f' + g'
template<class T1,class T2>
struct Derivative<Rel2<Add,T1,T2> >
{
	BndExpr<double> operator()(const Rel2<Add,T1,T2>& e, const Var<double>& s)
	{
		return BndExpr<double>(s.solver(),derivative(e.p1,s) +
										  derivative(e.p2,s));
	}
};

// (f-g)' = f' - g'
template<class T1,class T2>
struct Derivative<Rel2<Sub,T1,T2> >
{
	BndExpr<double> operator()(const Rel2<Sub,T1,T2>& e, const Var<double>& s)
	{
		return BndExpr<double>(s.solver(),derivative(e.p1,s) -
										  derivative(e.p2,s));
	}
};

// (f*g)' = f'*g + f*g'
template<class T1,class T2>
struct Derivative<Rel2<Mul,T1,T2> >
{
	BndExpr<double> operator()(const Rel2<Mul,T1,T2>& e, const Var<double>& s)
	{
		return BndExpr<double>(s.solver(),derivative(e.p1,s)*e.p2 +
										  e.p1*derivative(e.p2,s));
	}
};

// ----

void eliminateDuplicates(List<Var<double> >& v)
{
	StdHashSet<Var<double>::PDom> occ;
	for (List<Var<double> >::Iterator it = v.begin();
			it != v.end(); ++it)
		occ.insert(it->pDomain());
	v.clear();
	for (auto it = occ.begin();
			it != occ.end(); ++it)
		v.pushBack(Var<double>((*it)->solver(),*it));
}

template<class T>
Filter newton(const T& t)
{
	CPSolver& solver(getState(t));
	List<Var<double> > vars(solver.getHeap());
	getVars(t,vars);
	eliminateDuplicates(vars);
	List<BndExpr<double> >  c(solver.getHeap());
	List<BndExpr<double> >  f(solver.getHeap());
	List<BndExpr<double> >  d(solver.getHeap());
	for (List<Var<double> >::Iterator it = vars.begin();
			it != vars.end(); ++it)
	{
		BndExpr<double> cc(solver,center(*it));
		BndExpr<double> ff(solver,replaceVarWithCenter(t,*it));
		BndExpr<double> dd(solver,derivative(t,*it));
		c.pushBack(cc);
		f.pushBack(ff);
		d.pushBack(dd);
	}
	return new (solver.getHeap()) Newton(solver,vars,c,f,d);
}

// ----

void i1()
{
	CPSolver solver;
	VarArray<double> x(solver,10,-2.0,2.0);

	solver.post(newton(0.25428722 + 0.183247757*x[3]*x[2]*x[8]-x[0]));
	solver.post(newton(0.37842197 + 0.16275449*x[0]*x[9]*x[5]-x[1]));
	solver.post(newton(0.27162577 + 0.16955071*x[0]*x[1]*x[9]-x[2]));
	solver.post(newton(0.19807914 + 0.15585316*x[6]*x[0]*x[5]-x[3]));
	solver.post(newton(0.44166728 + 0.19950920*x[6]*x[5]*x[2]-x[4]));
	solver.post(newton(0.14654113 + 0.18922793*x[7]*x[4]*x[9]-x[5]));
	solver.post(newton(0.42937161 + 0.21180486*x[1]*x[4]*x[7]-x[6]));
	solver.post(newton(0.07056438 + 0.17081208*x[0]*x[6]*x[5]-x[7]));
	solver.post(newton(0.34504906 + 0.19612740*x[9]*x[5]*x[7]-x[8]));
	solver.post(newton(0.42651102 + 0.21466544*x[3]*x[7]*x[0]-x[9]));

	cout << solver.valid() << endl;
	cout << "before search: " << x << endl;

	bool found = solver.solve( label(x,selectVarLex(x),
								  selectHalfMin(x)) );

	cout.precision(17);

	uint solutions = 0;
	while (found)
	{
		cout << "solution " << ++solutions << ":" << x << endl;
		found = solver.next();
	}
	cout << solver.stats() << endl;
}

void cyclo()
{
	CPSolver solver;
	VarArray<double> x(solver,3,0,10e5);

	solver.post(newton(x[1]*x[1]*x[2]*x[2]+x[1]*x[1]+x[2]*x[2]+13.0-24.0*x[1]*x[2]));
	solver.post(newton(x[0]*x[0]*x[2]*x[2]+x[0]*x[0]+x[2]*x[2]+13.0-24.0*x[0]*x[2]));
	solver.post(newton(x[0]*x[0]*x[1]*x[1]+x[0]*x[0]+x[1]*x[1]+13.0-24.0*x[0]*x[1]));

	cout << solver.valid() << endl;
	cout << "before search: " << x << endl;

	bool found = solver.solve( label(x,selectVarLex(x),
								  selectHalfMin(x)) );

	cout.precision(17);

	uint solutions = 0;
	while (found)
	{
		cout << "solution " << ++solutions << ":" << x << endl;
		found = solver.next();
	}
	cout << solver.stats() << endl;
}

void chem()
{
	const double r = 10;
	const double r5 = 0.193;
	const double r6 = 0.002597/sqrt(40);
	const double r7 = 0.003448/sqrt(40);
	const double r8 = 0.00001799/40;
	const double r9 = 0.0002155/sqrt(40);
	const double r10 = 0.00003846/40;

	CPSolver solver;
	VarArray<double> x(solver,5,0,10e8);

	solver.post(newton(3.0*x[4]-x[0]*(x[1]+1.0)));
	solver.post(newton(x[1]*(2.0*x[0]+x[2]*x[2]+r8+2.0*r10*x[1]+r7*x[2]+r9*x[3])+x[0]-r*x[4]));
	solver.post(newton(x[2]*(2.0*x[1]*x[2]+2.0*r5*x[2]+r6+r7*x[1])-8.0*x[4]));
	solver.post(newton(x[3]*(r9*x[1]+2.0*x[3])-4.0*r*x[4]));
	solver.post(newton(x[1]*(x[0]+r10*x[1]+x[2]*x[2]+r8+r7*x[2]+r9*x[3])+x[0]+x[2]*(r5*x[2]+r6)+x[3]*x[3]-1.0));

	cout << solver.valid() << endl;
	cout << "before search: " << x << endl;

	bool found = solver.solve( label(x,selectVarLex(x),
								  selectHalfMin(x)) );

	cout.precision(17);

	uint solutions = 0;
	while (found)
	{
		cout << "solution " << ++solutions << ":" << x << endl;
		found = solver.next();
	}
	cout << solver.stats() << endl;
}


int debug()
{
	CPSolver solver ;
	cout.precision(20);
	VarArray<double> x(solver,1,-2,2);
	//solver.post(newton(x[0]-pow(x[0],3.0)));
	solver.post(newton(x[0]-x[0]*x[0]*x[0]));
	bool found = solver.solve(label(x,selectVarLex(x),
			  selectHalfMin(x)));
	while (found)
	{
		cout << x << endl;
		found = solver.next();
	}
	return 0;
}


int main()
{
	//i1();
	cyclo();
	//chem();
	//debug();
	return 0;
}
