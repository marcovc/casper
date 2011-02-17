
/***************************************************************************
 *   Copyright (C) 2007 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <casper/set/config.h>

#include <casper/kernel.h>
#include <casper/set.h>
#include <casper/int.h>

#include <iostream>
#include <fstream>

#include <map>

using namespace casper;
using namespace std;

const int nc1 = 3;
const int nc2 = nc1*nc1;
const int nc3 = nc2*nc1;
const int nc4 = nc3*nc1;
const int nc5 = nc4*nc1;
const int nc6 = nc5*nc1;
const int nc7 = nc6*nc1;
const int nc8 = nc7*nc1;

map<char,int> m;

int getConsist(int i1,int i2,int i3, int i4)
{	return i1*nc3+i2*nc2+i3*nc1+i4;	}

int getConsist(int i1,int i2,int i3, int i4, int i5, int i6, int i7, int i8)
{	return i1*nc7+i2*nc6+i3*nc5+i4*nc4+i5*nc3+i6*nc2+i7*nc1+i8;	}

int getIndexConsist(int index,int consist,int arity)
{	return static_cast<int>(consist/std::pow((double)nc1,(double)arity-index-1)) % nc1;	}

struct Consist
{
	DomVar<IntSet> equiv;
	DomVar<IntSet> weaker;
	DomVar<IntSet> stronger;
	Consist(ICPSolver& solver, int nelems) :
		equiv(solver,range(0,nelems-1)),
		weaker(solver,range(0,nelems-1)),
		stronger(solver,range(0,nelems-1))
	{
		solver.post( intersect(weaker,stronger,equiv));
	}
	ICPSolver& solver() const { return equiv.solver(); }
	Filter isStronger(int c1, int c2) const
	{
		return member(c1,weaker) <= member(c2,weaker) and
			   member(c2,stronger) <= member(c1,stronger);
	}

	Filter isSStronger(int c1, int c2)
	{
		return member(c1,weaker) <= (member(c2,weaker) and notMember(c2,stronger)) and
			   member(c2,stronger) <= (member(c1,stronger) and notMember(c1,weaker));
	}

	Filter isEquivalent(int c1, int c2) const
	{	return member(c1,equiv) == member(c2,equiv);	}
};

Bool postIsDerivedProp(const Consist& view,const Consist& prop)
{
	for (int a1 = 0; a1 < 3; ++a1)
	for (int a2 = 0; a2 < 3; ++a2)
	for (int a3 = 0; a3 < 3; ++a3)
	for (int a4 = 0; a4 < 3; ++a4)
	for (int a5 = 0; a5 < 3; ++a5)
	for (int a6 = 0; a6 < 3; ++a6)
	for (int a7 = 0; a7 < 3; ++a7)
	for (int a8 = 0; a8 < 3; ++a8)
	{
		if (!view.solver().post( member(getConsist(a3,a4,a5,a6),prop.weaker) ==
								 member(getConsist(a1,a2,a3,a4,a5,a6,a7,a8),view.weaker)) or
			!view.solver().post( member(getConsist(a3,a4,a5,a6),prop.stronger) ==
								 member(getConsist(a1,a2,a3,a4,a5,a6,a7,a8),view.stronger)))
			return false;

	}
	return true;
}

//struct PropToView : IFilter
//{
//	typedef DomVar<IntSet>::Dom	Dom;
//	typedef DomVar<IntSet>::Dom::DeltasIterator DeltasIterator;
//	typedef InItView<IntSet,Dom>		In;
//	typedef PossItView<IntSet,Dom>		Poss;
//	typedef UnionItView<Int,In,Poss> 	LUB;
//	typedef DiffItView<Int,In,LUB> 		DiffInLUB;
//
//	PropToView(const Consist& view, const Consist& prop) :
//		IFilter(view.solver()),
//		first(view.solver(),true),
//		vw(view.solver(),view.weaker),
//		vs(view.solver(),view.stronger),
//		pw(view.solver(),prop.weaker),
//		ps(view.solver(),prop.stronger),
//		pwGLBDeltasIt(pw->glbDeltas().begin()),
//		pwLUBDeltasIt(pw->lubDeltas().begin()),
//		psGLBDeltasIt(ps->glbDeltas().begin()),
//		psLUBDeltasIt(ps->lubDeltas().begin()) {}
//
//	bool exactPi(int c)
//	{
//		return (getIndexConsistency(0,c,4)>=getIndexConsistency(1,c,4) and
//			    getIndexConsistency(3,c,4)>=getIndexConsistency(2,c,4)) or
//			   (getIndexConsistency(0,c,4)<=getIndexConsistency(1,c,4) and
//			    getIndexConsistency(3,c,4)<=getIndexConsistency(2,c,4));
//	}
//	bool postPropWeaker(int c)
//	{
//		if (not exactPi(c))
//			return true;
//		t3 = maxc(getIndexConsistency(0,c,4),getIndexConsistency(1,c,4));
//		t4 = maxc(getIndexConsistency(2,c,4),getIndexConsistency(3,c,4));
//		for (int d1 = 0; d1 < 3; d1++)
//		  for (int d2 = 0; d2 < 3; d2++)
//			for (int d3 = t3+1; d3 < 3; d3++)
//			  for (int d4 = t4+1; d4 < 3; d4++)
//				for (int d5 = 0; d5 < 3; d5++)
//				  for (int d6 = 0; d6 < 3; d6++)
//					if (!solver.post(view.isStronger(getConsist(d1,d2,d3,d4,d5,d6),
//												     getConsist(d1,d2,t3,t4,d5,d6))))
//						return false;
//		return true;
//	}
//	bool execute()
//	{
//		if (first)
//		{
//			first = false;
//			for (Dom::IIterator it = pw->beginIn(); it != pw->endIn(); ++it)
//				if (!postPropWeaker(*it))
//					return false;
//			for (Dom::IIterator it = ps->beginIn(); it != ps->endIn(); ++it)
//				if (exactPi(*it) and !solver().post(member(toView(*it),vs.getView())))
//					return false;
//
//			In	inps(*ps);
//			LUB lubpw(*pw);
//			for (DiffInLUB diff(inps,lubpw); diff.valid(); diff.iterate())
//				if (exactPi(diff.value()) and
//					!(solver().post(member(toView(diff.value()),vs.getView())) and
//					  solver().post(notMember(toView(diff.value()),vw.getView()))))
//					return false;
//			In	inpw(*pw);
//			LUB lubps(*ps);
//			for (DiffInLUB diff(inpw,lubps); diff.valid(); diff.iterate())
//				if (exactPi(diff.value()) and
//					!(solver().post(member(toView(diff.value()),vw.getView())) and
//					  solver().post(notMember(toView(diff.value()),vs.getView()))))
//					return false;
//		}
//		for (pwGLBDeltasIt = pw->glbDeltas().beginFrom(pwGLBDeltasIt);
//				pwGLBDeltasIt != pw->glbDeltas().end(); ++pwGLBDeltasIt)
//			if (exactPi(*pwGLBDeltasIt) and
//				!solver().post(member(toView(*pwGLBDeltasIt),vw.getView())))
//				return false;
//		for (psGLBDeltasIt = ps->glbDeltas().beginFrom(psGLBDeltasIt);
//				psGLBDeltasIt != ps->glbDeltas().end(); ++psGLBDeltasIt)
//			if (exactPi(*psGLBDeltasIt) and
//				!solver().post(member(toView(*psGLBDeltasIt),vs.getView())))
//				return false;
//		for (pwLUBDeltasIt = pw->lubDeltas().beginFrom(pwLUBDeltasIt);
//				pwLUBDeltasIt != pw->lubDeltas().end(); ++pwLUBDeltasIt)
//			if (exactPi(*pwLUBDeltasIt) and
//				!solver().post(notMember(toView(*pwLUBDeltasIt),vw.getView())))
//				return false;
//		for (psLUBDeltasIt = ps->lubDeltas().beginFrom(psLUBDeltasIt);
//				psLUBDeltasIt != ps->lubDeltas().end(); ++psLUBDeltasIt)
//			if (exactPi(*psLUBDeltasIt) and
//				!solver().post(notMember(toView(*psLUBDeltasIt),vs.getView())))
//				return false;
//		return true;
//	}
//
//	Reversible<Bool>	first;
//	DomView<IntSet,DomVar<IntSet> >	vw;
//	DomView<IntSet,DomVar<IntSet> >	vs;
//	DomView<IntSet,DomVar<IntSet> >	pw;
//	DomView<IntSet,DomVar<IntSet> >	ps;
//
//	DeltasIterator 		pwGLBDeltasIt;
//	DeltasIterator 		pwLUBDeltasIt;
//	DeltasIterator 		psGLBDeltasIt;
//	DeltasIterator 		psLUBDeltasIt;
//};
//
//Filter derived(const Consist& view, const Consist& prop)
//{	return new (view.solver()) PropToView(view,prop);	}

bool postViewConstraints(const Consist& view, const char* filename)
{
	// read file
	ifstream f(filename);
	while (!f.eof())
	{
		char a1,a2,a3,a4,a5,a6,a7,a8;
		f >> ws >> a1 >> ws >> a2 >> ws >> a3 >> ws >> a4 >> ws >> a5 >> ws
		  >> a6 >> ws >> a7 >> ws >> a8 >> ws;
		char b1,b2,b3,b4,b5,b6,b7,b8;
		f >> ws >> b1 >> ws >> b2 >> ws >> b3 >> ws >> b4 >> ws >> b5 >> ws
		  >> b6 >> ws >> b7 >> ws >> b8 >> ws;
		int c1 = getConsist(a1,a2,a3,a4,a5,a6,a7,a8);
		int c2 = getConsist(b1,b2,b3,b4,b5,b6,b7,b8);
		if (!view.solver().post(view.isStronger(c1,c2)))
			return false;
	}
	// add implicit rules due to mononicity
	int a[8];
	for (a[0] = 0; a[0] < 3; ++a[0])
	for (a[1] = 0; a[1] < 3; ++a[1])
	for (a[2] = 0; a[2] < 3; ++a[2])
	for (a[3] = 0; a[3] < 3; ++a[3])
	for (a[4] = 0; a[4] < 3; ++a[4])
	for (a[5] = 0; a[5] < 3; ++a[5])
	for (a[6] = 0; a[6] < 3; ++a[6])
	for (a[7] = 0; a[7] < 3; ++a[7])
	{
		int c1 = getConsist(a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]);
		for (int i = 0; i < 8; ++i)
			if (a[i]<2)
			{
				int b[8];
				memcpy(b,a,sizeof(int)*8);
				b[i] = a[i]+1;
				int c2 = getConsist(b[0],b[1],b[2],b[3],b[4],b[5],b[6],b[7]);
				if (!view.solver().post(view.isStronger(c1,c2)))
					return false;
			}
	}
	return true;
}

bool postPropConstraints(const Consist& prop, const char* filename)
{
	// read file
	ifstream f(filename);
	while (!f.eof())
	{
		char a1,a2,a3,a4;
		f >> ws >> a1 >> ws >> a2 >> ws >> a3 >> ws >> a4 >> ws;
		char b1,b2,b3,b4;
		f >> ws >> b1 >> ws >> b2 >> ws >> b3 >> ws >> b4 >> ws;
		int c1 = getConsist(a1,a2,a3,a4);
		int c2 = getConsist(b1,b2,b3,b4);
		if (!prop.solver().post(prop.isStronger(c1,c2)))
			return false;
	}
	// add implicit rules due to mononicity
	int a[4];
	for (a[0] = 0; a[0] < 3; ++a[0])
	for (a[1] = 0; a[1] < 3; ++a[1])
	for (a[2] = 0; a[2] < 3; ++a[2])
	for (a[3] = 0; a[3] < 3; ++a[3])
	{
		int c1 = getConsist(a[0],a[1],a[2],a[3]);
		for (int i = 0; i < 4; ++i)
			if (a[i]<2)
			{
				int b[4];
				memcpy(b,a,sizeof(int)*4);
				b[i] = a[i]+1;
				int c2 = getConsist(b[0],b[1],b[2],b[3]);
				if (!prop.solver().post(prop.isStronger(c1,c2)))
					return false;
			}
	}
	return true;
}

string c2str(int c, int arity)
{
	string ret="";
	for (int i = 0; i < arity; ++i)
	{
		int ci = getIndexConsist(i,c,arity);
		if (ci == 0)
			ret += "p";
		else
		if (ci == 1)
			ret += "d";
		else
		if (ci == 2)
			ret += "b";
	}
	return ret;
}

bool isStrictlyStronger(int c1, int c2,int arity)
{
  bool found = false;
  for (int i = 0; i < arity; ++i)
  {
    if (getIndexConsist(i,c1,arity) > getIndexConsist(i,c2,arity))
      return false;
    if (getIndexConsist(i,c1,arity) < getIndexConsist(i,c2,arity))
      found = true;
  }
  return found;
}

bool isStrictlyWeaker(int c1, int c2,int arity)
{
  bool found = false;
  for (int i = 0; i<arity; ++i)
  {
    if (getIndexConsist(i,c1,arity) > getIndexConsist(i,c2,arity))
    	found = true;
    if (getIndexConsist(i,c1,arity) < getIndexConsist(i,c2,arity))
    	return false;
  }
  return found;
}

template<class InputIterator>
void printBest(InputIterator b,InputIterator e,int arity)
{
	InputIterator be = b;
	while (be != e)
	{
		bool keep = true;
		for (InputIterator it = b; it != e; ++it)
			if (isStrictlyStronger(*it,*be,arity))
			{
				keep = false;
				break;
			}
		if (keep)
			cout << c2str(*be,arity) << " ";
		++be;
	}
	cout << endl;
}

template<class InputIterator>
void printWorst(InputIterator b,InputIterator e,int arity)
{
	InputIterator be = b;
	while (be != e)
	{
		bool keep = true;
		for (InputIterator it = b; it != e; ++it)
			if (isStrictlyWeaker(*it,*be,arity))
			{
				keep = false;
				break;
			}
		if (keep)
			cout << c2str(*be,arity) << " ";
		++be;
	}
	cout << endl;
}

void print(const Consist& c, int arity)
{
	cout << "lower bound (only best shown):\n";
	printBest(c.weaker.domain().beginIn(),c.weaker.domain().endIn(),arity);
	cout << "upper bound (only worst shown):\n";
	printWorst(c.stronger.domain().beginIn(),c.stronger.domain().endIn(),arity);
	cout << "equiv:\n";
	cout << "from (best):\n";
	printBest(c.equiv.domain().beginIn(),c.equiv.domain().endIn(),arity);
	cout << "to (worst):\n";
	printWorst(c.equiv.domain().beginIn(),c.equiv.domain().endIn(),arity);
}

int main(int argc, const char** argv)
{
	m['p']=0;
	m['d']=1;
	m['b']=2;
	const int p=0;
	const int d=1;
	const int b=2;

	ICPSolver& solver;
	Consist** views = new Consist*[argc-2];
	for (int i = 0; i < argc-2; ++i)
		views[i] = new Consist(solver,nc8);
	Consist prop(solver,nc4);

	cout << "posting view constraints\n";
	for (int i = 1; i < argc-1; ++i)
		if (!postViewConstraints(*views[i-1],argv[i]))
		{
			cout << "no solution for view " << (i-1) << endl;
			return 1;
		}
	cout << "posting prop constraints\n";
	if (!postPropConstraints(prop,argv[argc-1]))
	{
		cout << "no solution for prop" << endl;
		return 1;
	}
	cout << "posting derived propagator constraints\n";
	if (!postIsDerivedProp(*views[argc-3],prop))
		return 1;

	cout << "posting query\n";
	solver.post(member(getConsist(d,b,b,b,b,b,b,d),views[0]->equiv));
	//solver.post(member(getConsist(b,b,b,b),prop.weaker));

	cout << "initial propagation\n";
	cout << solver.solve() << endl;

	print(*views[0],8);
	print(prop,4);

	cout << solver.stats() << endl;
	cout << solver.totalTime() << endl;
}
