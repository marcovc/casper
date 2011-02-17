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
#include <casper/kernel/container/hashmap.h>

#include <iostream>
#include <casper/real.h>

using namespace casper;
using namespace std;


template<class T>
struct Center : IBndExpr<Double>
{
	Center(ICPSolver& solver,const T& v) :
		IBndExpr<Double>(solver),v(solver,v) {}
	Double min() const
	{
		Double mi = detail::divLb(detail::addLb(v.min(),v.max()),2.0);
		Double ma = detail::divUb(detail::addUb(v.min(),v.max()),2.0);
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
	Double max() const
	{	return min();	}
	Bool updateMin(const Double& v) {	return v<=max();}
	Bool updateMax(const Double& v) {	return v>=min();}
	void attach(INotifiable* f) { 	v.attach(f); }
	void detach(INotifiable* f) {	v.detach(f); }
	BndView<Double,T> v;
};

template<class T>
BndExpr<Double> center(const T& v)
{	return new (getCPSolver(v).heap()) Center<T>(getCPSolver(v),v);	}

// ------ newton -------

struct Newton : IFilter
{
	Newton(ICPSolver& solver,const detail::List<DomVar<Double> >& vars,
						 const detail::List<BndExpr<Double> >& c,
						 const detail::List<BndExpr<Double> >& f,
						 const detail::List<BndExpr<Double> >& d) :
			IFilter(solver), vars(vars),c(c),f(f),d(d)
	{}

	Bool execute()
	{
		detail::List<DomVar<Double> >::Iterator itv = vars.begin();
		detail::List<BndExpr<Double> >::Iterator itc = c.begin();
		detail::List<BndExpr<Double> >::Iterator itf = f.begin();
		detail::List<BndExpr<Double> >::Iterator itd = d.begin();
		for ( ; itv != vars.end(); ++itv, ++itc, ++itf, ++itd)
		{
			Range<Double> rc(itc->min(),itc->max());
			Range<Double> rf(itf->min(),itf->max());
			Range<Double> rd(itd->min(),itd->max());
			Double lb = detail::subLb(rc.min(),itv->domain().max());
			Double ub = detail::subUb(rc.max(),itv->domain().min());
			detail::divRange(rf,rd,lb,ub);
			if (lb>ub or
				!itv->domain().updateRange(detail::subLb(rc.min(),ub),
										   detail::subUb(rc.max(),lb)))
				return false;
		}
		return true;
	}

	Void attach(INotifiable* f)
	{
		for (detail::List<DomVar<Double> >::Iterator itv = vars.begin();
				itv != vars.end(); ++itv)
			itv->domain().attachOnBounds(f);
	}
	Void detach(INotifiable* f)
	{
		for (detail::List<DomVar<Double> >::Iterator itv = vars.begin();
				itv != vars.end(); ++itv)
			itv->domain().detachOnBounds(f);
	}
	Cost cost() const
	{	return linearHi; }

	detail::List<DomVar<Double> > vars;
	detail::List<BndExpr<Double> > c;
	detail::List<BndExpr<Double> > f;
	detail::List<BndExpr<Double> > d;
};
/*
Filter newton2(const detail::List<DomVar<Double> >& vars,
			  const detail::List<BndExpr<Double> >& c,
			  const detail::List<BndExpr<Double> >& f,
			  const detail::List<BndExpr<Double> >& d)
{
	ICPSolver& solver(getCPSolver(vars));
	detail::List<BndExpr<Double> > vlst(solver.heap());
	for (detail::List<DomVar<Double> >::Iterator it = vars.begin();
				it != vars.end(); ++it)
		vlst.pushBack(BndExpr<Double>(solver,*it));

	return new (solver.heap()) Newton(solver,vlst,c,f,d);
}*/

// ---

template<class T>
struct GetVars
{
	void operator()(const T& e,detail::List<DomVar<Double> >& l)
	{}
};

template<class T>
void getVars(const T& t,detail::List<DomVar<Double> >& l)
{	GetVars<T>()(t,l);	}

template<>
struct GetVars<DomVar<Double> >
{
	void operator()(const DomVar<Double>& e,detail::List<DomVar<Double> >& l)
	{	l.pushBack(e);	}
};

template<class Op,class T1>
struct GetVars<Rel1<Op,T1> >
{
	void operator()(const Rel1<Op,T1>& e,detail::List<DomVar<Double> >& l)
	{	getVars(e.p1,l);	}
};

template<class Op,class T1,class T2>
struct GetVars<Rel2<Op,T1,T2> >
{
	void operator()(const Rel2<Op,T1,T2>& e,detail::List<DomVar<Double> >& l)
	{	getVars(e.p1,l); getVars(e.p2,l);	}
};


// ---

template<class T>
struct ReplaceDomVarWithCenter
{
	BndExpr<Double> operator()(const T& e, const DomVar<Double>& s)
	{	return BndExpr<Double>(s.solver(),e);	}
};

template<class T>
BndExpr<Double> replaceDomVarWithCenter(const T& e,const DomVar<Double>& s)
{
	return ReplaceDomVarWithCenter<T>()(e,s);
}

template<>
struct ReplaceDomVarWithCenter<DomVar<Double> >
{
	BndExpr<Double> operator()(const DomVar<Double>& e, const DomVar<Double>& s)
	{
		if (s.pDomain()==e.pDomain())
			return center(e);
		else
			return BndExpr<Double>(s.solver(),e);
	}
};

template<class Op,class T1>
struct ReplaceDomVarWithCenter<Rel1<Op,T1> >
{
	BndExpr<Double> operator()(const Rel1<Op,T1>& e, const DomVar<Double>& s)
	{	return BndExpr<Double>(s.solver(),rel<Op>(replaceDomVarWithCenter(e.p1,s)));	}
};

template<class Op,class T1,class T2>
struct ReplaceDomVarWithCenter<Rel2<Op,T1,T2> >
{
	BndExpr<Double> operator()(const Rel2<Op,T1,T2>& e, const DomVar<Double>& s)
	{	return BndExpr<Double>(s.solver(),rel<Op>(replaceDomVarWithCenter(e.p1,s),
												  replaceDomVarWithCenter(e.p2,s)));	}
};

// ---

template<class T>
struct Derivative;

template<class T>
BndExpr<Double> derivative(const T& v,const DomVar<Double>& s)
{	return Derivative<T>()(v,s);	}

// c' = 0
template<>
struct Derivative<Double>
{
	BndExpr<Double> operator()(const Double& e, const DomVar<Double>& s)
	{	return BndExpr<Double>(s.solver(),0.0);		}
};

// x' = 1 or 0
template<>
struct Derivative<DomVar<Double> >
{
	BndExpr<Double> operator()(const DomVar<Double>& e, const DomVar<Double>& s)
	{
		if (s.pDomain()==e.pDomain())
			return BndExpr<Double>(s.solver(),1.0);
		else
			return BndExpr<Double>(s.solver(),0.0);
	}
};

// (f+g)' = f' + g'
template<class T1,class T2>
struct Derivative<Rel2<Add,T1,T2> >
{
	BndExpr<Double> operator()(const Rel2<Add,T1,T2>& e, const DomVar<Double>& s)
	{
		return BndExpr<Double>(s.solver(),derivative(e.p1,s) +
										  derivative(e.p2,s));
	}
};

// (f-g)' = f' - g'
template<class T1,class T2>
struct Derivative<Rel2<Sub,T1,T2> >
{
	BndExpr<Double> operator()(const Rel2<Sub,T1,T2>& e, const DomVar<Double>& s)
	{
		return BndExpr<Double>(s.solver(),derivative(e.p1,s) -
										  derivative(e.p2,s));
	}
};

// (f*g)' = f'*g + f*g'
template<class T1,class T2>
struct Derivative<Rel2<Mul,T1,T2> >
{
	BndExpr<Double> operator()(const Rel2<Mul,T1,T2>& e, const DomVar<Double>& s)
	{
		return BndExpr<Double>(s.solver(),derivative(e.p1,s)*e.p2 +
										  e.p1*derivative(e.p2,s));
	}
};

// ----

void eliminateDuplicates(detail::List<DomVar<Double> >& v)
{
	detail::HashSet<DomVar<Double>::PDom> occ;
	for (detail::List<DomVar<Double> >::Iterator it = v.begin();
			it != v.end(); ++it)
		occ.insert(it->pDomain());
	v.clear();
	for (auto it = occ.begin();
			it != occ.end(); ++it)
		v.pushBack(DomVar<Double>((*it)->solver(),*it));
}

template<class T>
Filter newton(const T& t)
{
	ICPSolver& solver(getCPSolver(t));
	detail::List<DomVar<Double> > vars(solver.heap());
	getVars(t,vars);
	eliminateDuplicates(vars);
	detail::List<BndExpr<Double> >  c(solver.heap());
	detail::List<BndExpr<Double> >  f(solver.heap());
	detail::List<BndExpr<Double> >  d(solver.heap());
	for (detail::List<DomVar<Double> >::Iterator it = vars.begin();
			it != vars.end(); ++it)
	{
		BndExpr<Double> cc(solver,center(*it));
		BndExpr<Double> ff(solver,replaceDomVarWithCenter(t,*it));
		BndExpr<Double> dd(solver,derivative(t,*it));
		c.pushBack(cc);
		f.pushBack(ff);
		d.pushBack(dd);
	}
	return new (solver.heap()) Newton(solver,vars,c,f,d);
}

// ----

void i1()
{
	CPSolver solver;
	DomVarArray<Double> x(solver,10,-2.0,2.0);

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

	UInt solutions = 0;
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
	DomVarArray<Double> x(solver,3,0,10e5);

	solver.post(newton(x[1]*x[1]*x[2]*x[2]+x[1]*x[1]+x[2]*x[2]+13.0-24.0*x[1]*x[2]));
	solver.post(newton(x[0]*x[0]*x[2]*x[2]+x[0]*x[0]+x[2]*x[2]+13.0-24.0*x[0]*x[2]));
	solver.post(newton(x[0]*x[0]*x[1]*x[1]+x[0]*x[0]+x[1]*x[1]+13.0-24.0*x[0]*x[1]));

	cout << solver.valid() << endl;
	cout << "before search: " << x << endl;

	bool found = solver.solve( label(x,selectVarLex(x),
								  selectHalfMin(x)) );

	cout.precision(17);

	UInt solutions = 0;
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
	DomVarArray<Double> x(solver,5,0,10e8);

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

	UInt solutions = 0;
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
	DomVarArray<Double> x(solver,1,-2,2);
	//solver.post(newton(x[0]-pow(x[0],3.0)));
	solver.post(newton(x[0]-x[0]*x[0]*x[0]));
	Bool found = solver.solve(label(x,selectVarLex(x),
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
