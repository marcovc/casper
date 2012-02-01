/***************************************************************************
 *   Copyright (C) 2007 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <casper/kernel.h>
#include <casper/cp/int.h>
#include <casper/cp/set.h>

#include <iostream>
#include <set>

using namespace Casper;
using namespace Casper::CP;

namespace Casper {
namespace CP {

template<class Eval>
struct MetaVar;

template<>
struct MetaVar<int>
{
	typedef Util::StdRange<int>	Universe;

	MetaVar(Solver& solver, const Universe& universe, Goal& eqvLabel) :
		solver(solver),universe(universe),dom(solver,universe)
	{
		solver.post(cardinal(dom)>0);
		eqvLabel = Goal(solver,eqvLabel and label(solver,{dom}));
	}

	Universe getUniverse() const
	{	return universe;	}

	BndExpr<bool> isContained(const MetaVar<int >& v) const
	{	return BndExpr<bool>(solver,contained(dom,v.dom)); }

	BndExpr<bool> rangeContains(const int& s) const
	{
		Var<int> ma(solver,universe.lb,universe.ub);
		Var<int> mi(solver,universe.lb,universe.ub);
		return BndExpr<bool>(solver, maxEqual(dom,ma) and minEqual(dom,mi) and s>=mi and s<=ma);
	}

	BndExpr<bool> contains(const int& s) const
	{	return BndExpr<bool>(solver,member(s,dom));	}

	BndExpr<bool> 	contains(const Var<int>& s) const
	{	return BndExpr<bool>(solver,member(s,dom)); }

	BndExpr<bool> equals(const int& s) const
	{
		Util::StdList<int> empty;
		Var<Set<int> > g(solver,list(s),empty);
		return BndExpr<bool>(solver,g==dom);
	}

	BndExpr<bool> equals(const Var<int >& s) const
	{
		Util::StdList<int> empty;
		Var<Set<int> > g(solver,s.domain().begin(),s.domain().end(),empty.begin(),empty.end());
		return BndExpr<bool>(solver,dom==g);
	}

	Solver& solver;
	const Util::StdRange<int> universe;
	Var<Set<int> > dom;
};

template<class Eval>
struct MetaVar<Set<Eval> >
{
	typedef Util::StdRange<Eval>	Universe;

	MetaVar(Solver& solver, const Util::StdRange<Eval>& universe, Goal& eqvLabel) :
		solver(solver),universe(universe),glb(solver,universe),lub(solver,universe)
	{
		solver.post(contained(glb,lub));
		eqvLabel = Goal(solver,eqvLabel and label(solver,{glb,lub}));
	}

	Universe getUniverse() const
	{	return universe;	}

	BndExpr<bool> isContained(const MetaVar<Set<Eval> >& v) const
	{	return BndExpr<bool>(solver,contained(lub,v.lub) and contained(v.glb,glb)); }

	BndExpr<bool> rangeContains(const typename Var<Set<Eval> >::Dom::Value& s) const
	{
		Util::StdList<int> empty;
		Var<Set<Eval> > a(solver,s.begin(),s.end(),empty.begin(),empty.end());
		return BndExpr<bool>(solver,contained(glb,a) and contained(a,lub));
	}

	BndExpr<bool> rangeContains(const Var<Set<Eval> >& s) const
	{	return BndExpr<bool>(solver,contained(glb,s) and contained(s,lub));	}

	BndExpr<bool> contains(const typename Var<Set<Eval> >::Dom::Value& s) const
	{	return rangeContains(s);	}

	BndExpr<bool> contains(const Var<Set<Eval> >& s) const
	{	return rangeContains(s);	}

	BndExpr<bool> equals(const Var<Set<Eval> >& s) const
	{
		Util::StdList<Eval> empty;
		Var<Set<Eval> > g(solver,s.domain().in().begin(),s.domain().in().end(),empty.begin(),empty.end());
		std::set<Eval> ls(s.domain().in().begin(),s.domain().in().end());
		ls.insert(s.domain().poss().begin(),s.domain().poss().end());
		Var<Set<Eval> > l(solver,ls.begin(),ls.end(),empty.begin(),empty.end());
		return BndExpr<bool>(solver,glb==g and lub==l);
	}

	Solver& solver;
	const Util::StdRange<Eval> universe;
	Var<Set<Eval> > glb;
	Var<Set<Eval> > lub;
};

} // CP

namespace Traits {

template<class Eval>
struct GetEval<CP::MetaVar<Eval> >
{	typedef Eval	Type;	};

} // Traits

namespace CP {
template<class,class> struct MetaDomView;

template<class Eval>
struct MetaDomView<Eval,MetaVar<Eval> >
{
	typedef typename MetaVar<Eval>::Universe	Universe;
	typedef typename Var<Eval>::Dom::Value	Value;

	MetaDomView(Solver& solver, const MetaVar<Eval>& v) :
		solver(solver),v(v) {}

	Universe getUniverse() const
	{	return v.getUniverse();	}

	BndExpr<bool>	isContained(const MetaVar<Eval>& s) const
	{	return v.isContained(s);	}

	BndExpr<bool> 	equals(const Value& s) const
	{	return v.equals(s);	}

	BndExpr<bool> 	contains(const Value& s) const
	{	return v.contains(s);	}

	BndExpr<bool> 	contains(const Var<Eval>& s) const
	{	return v.contains(s);	}

	MetaVar<Eval> getObj() const
	{	return v; }

	Solver&	solver;
	MetaVar<Eval>	v;
};

template<class,class,class> struct ComputeUniverse;

template<class Eval,class Func,class Obj1,class Obj2>
struct ComputeUniverse<Eval,Util::StdRange<Eval>,Rel2<Func,Obj1,Obj2> >
{
	Util::StdRange<Eval> operator()(const Rel2<Func,Obj1,Obj2>& obj)
	{
		Solver solver;
		Var<Eval> s1(solver,obj.p1.getUniverse());
		Var<Eval> s2(solver,obj.p2.getUniverse());
		BndExpr<Eval> b(solver,rel<Func>(s1,s2));
		return Util::StdRange<Eval>(b.min(),b.max());
	}
};

template<class Eval,class Func,class Obj1,class Obj2>
struct ComputeUniverse<Seq<Eval>,Util::StdRange<Eval>,Rel2<Func,Obj1,Obj2> >
{
	Util::StdRange<Eval> operator()(const Rel2<Func,Obj1,Obj2>& obj)
	{
		return obj.p1.getUniverse(); // hummm
	}
};

template<class Eval,class Func,class Obj1,class Obj2>
struct MetaDomView<Eval,Rel2<Func,Obj1,Obj2> >
{
	typedef typename Obj1::Universe	Universe;
	typedef typename Var<Eval>::Dom::Value	Value;

	MetaDomView(Solver& solver, const Rel2<Func,Obj1,Obj2>& r) :
		solver(solver),v1(solver,r.p1),v2(solver,r.p2)
	{}


	Universe getUniverse() const
	{
		return ComputeUniverse<Eval,Universe,Rel2<Func,Obj1,Obj2> >()(rel<Func>(v1.getObj(),v2.getObj()));
	}

	BndExpr<bool>	isContained(const Rel2<Func,Obj1,Obj2>& s) const
	{	return BndExpr<bool>(solver,v1.isContained(s.p1) and v2.isContained(s.p2));	}

	BndExpr<bool> 	equals(const Value& s) const
	{
		BndExpr<bool> posRet(solver,false);

		Solver posSolver;
		Var<Eval> s1(posSolver,v1.getUniverse());
		Var<Eval> s2(posSolver,v2.getUniverse());

		bool found = posSolver.post(rel<Func>(s1,s2)==s);
		found &= posSolver.solve(label(posSolver,{s1,s2}));
		while (found)
		{
			posRet = posRet or (v1.equals(s1.domain().value()) and v2.equals(s2.domain().value()));
			found = posSolver.next();
		}
		return posRet;
	}

	BndExpr<bool> 	contains(const Value& s) const
	{
		BndExpr<bool> posRet(solver,false);

		Solver posSolver;
		Var<Eval> s1(posSolver,v1.getUniverse());
		Var<Eval> s2(posSolver,v2.getUniverse());
		bool found = posSolver.post(rel<Func>(s1,s2)==s);
		found &= posSolver.solve(label(posSolver,{s1,s2}));
		while (found)
		{
			posRet = posRet or (v1.contains(s1.domain().value()) and v2.contains(s2.domain().value()));
			found = posSolver.next();
		}
		return posRet;
	}

	BndExpr<bool> 	contains(const Var<Eval>& s) const
	{
		BndExpr<bool> ret(solver,false);
		Solver posSolver;
		Var<Eval> s1(posSolver,v1.getUniverse());
		Var<Eval> s2(posSolver,v2.getUniverse());
		Var<Eval> aux(solver,getUniverse());

		bool found = posSolver.post(rel<Func>(s1,s2)==aux);
		found &= posSolver.solve(label(posSolver,{s1,s2,aux}));
		while (found)
		{
			ret = ret or (s==aux.domain().value() and v1.contains(s1.domain().value()) and v2.contains(s2.domain().value()));
			found = posSolver.next();
		}
		return ret;
	}

	Rel2<Func,Obj1,Obj2> getObj() const
	{	return rel<Func>(v1.getObj(),v2.getObj()); }

	Solver&	solver;
	MetaDomView<Eval,Obj1> v1;
	MetaDomView<Eval,Obj2> v2;
	mutable Util::StdList<Var<Eval> >	labelVars;
};


struct MetaFilterDD
{
	template<class Func,class Obj1,class Obj2>
	BndExpr<bool> operator()(Solver& solver, const Rel2<Func,Obj1,Obj2>& in,
							const Rel2<Func,Obj1,Obj2>& out, Goal& labeling)
	{
		typedef typename Casper::Traits::GetEval<Obj1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Obj2>::Type	Eval2;

		MetaDomView<Eval1,Obj1> in1(solver,in.p1);
		MetaDomView<Eval2,Obj2> in2(solver,in.p2);

		MetaDomView<Eval1,Obj1> out1(solver,out.p1);
		MetaDomView<Eval2,Obj2> out2(solver,out.p2);

		// contraction
		BndExpr<bool> ret(solver,out1.isContained(in1.getObj()) and out2.isContained(in2.getObj()));

		Solver posSolver;
		Var<Eval1> s1(posSolver,in1.getUniverse());
		Var<Eval2> s2(posSolver,in2.getUniverse());

		ChkExpr r(posSolver,rel<Func>(s1,s2));
		bool found = posSolver.solve(label(posSolver,{s1,s2}));
		while (found)
		{
			if (r.isTrue())
				ret = ret and ( (in1.contains(s1.domain().value()) and in2.contains(s2.domain().value())) ==
								(out1.contains(s1.domain().value()) and out2.contains(s2.domain().value()) ));
			else
			if (!r.canBeTrue())
				ret = ret and (not (out1.contains(s1.domain().value()) and out2.contains(s2.domain().value()) ));
			else
				assert(0);
			found = posSolver.next();
		}

		labeling = Goal(solver,labeling and in1.getLabelGoal() and
				in2.getLabelGoal() and out1.getLabelGoal() and out2.getLabelGoal());
		return ret;
	}

	template<class Func,class Obj1,class Obj2>
	BndExpr<bool> operator()(Solver& solver, const Rel2<Func,Obj1,Obj2>& in)
	{
		typedef typename Casper::Traits::GetEval<Obj1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Obj2>::Type	Eval2;

		MetaDomView<Eval1,Obj1> in1(solver,in.p1);
		MetaDomView<Eval2,Obj2> in2(solver,in.p2);

		BndExpr<bool> posRet(solver,false);
//		BndExpr<bool> inDom1(solver,false);
//		BndExpr<bool> inDom2(solver,false);

		Solver posSolver;
		Var<Eval1> s1(posSolver,in1.getUniverse());
		Var<Eval2> s2(posSolver,in2.getUniverse());

		bool found = posSolver.post(rel<Func>(s1,s2));
		found &= posSolver.solve(label(posSolver,{s1,s2}));
		while (found)
		{
			posRet = posRet or (in1.contains(s1.domain().value()) and in2.contains(s2.domain().value()));
	//		inDom1 = inDom1 or (in1.equals(s1.domain().value()));
	//		inDom2 = inDom2 or (in2.equals(s2.domain().value()));
			found = posSolver.next();
		}

		return BndExpr<bool>(solver,posRet /*and inDom1 and inDom2*/);
	}
};

/*
template<class Func,class Eval1,class Eval2>
struct MetaFilterBB2
{
	BndExpr<bool> operator()(Solver& solver, const MetaVar<Eval1>& in1, const MetaVar<Eval2>& in2,
				const MetaVar<Eval1>& out1, const MetaVar<Eval2>& out2)
	{
		// contraction
		BndExpr<bool> ret(solver,out1.isContained(in1) and out2.isContained(in2));

		Solver posSolver;
		Var<Eval1> s1(in1.inUniverse(posSolver));
		Var<Eval2> s2(in2.inUniverse(posSolver));
		ChkExpr r(posSolver,rel<Func>(s1,s2));
		bool found = posSolver.solve(label(posSolver,{s1,s2}));
		while (found)
		{
			if (r.isTrue())
				ret = ret and ( (in1.rangeContains(s1.domain().value()) and in2.rangeContains(s2.domain().value())) <=
								(out1.rangeContains(s1.domain().value()) and out2.rangeContains(s2.domain().value()) ));
			else
			if (!r.canBeTrue())
				ret = ret and ( (in1.rangeContains(s1.domain().value()) and in2.rangeContains(s2.domain().value())) >=
								(out1.rangeContains(s1.domain().value()) and out2.rangeContains(s2.domain().value()) ));
			else
				assert(0);
			found = posSolver.next();
		}

		return ret;
	}
};*/

} // CP
} // Casper

std::ostream& operator<<(std::ostream& os, const Casper::CP::MetaVar<int>& v)
{
	os << v.dom;
	return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const Casper::CP::MetaVar<Set<T> >& v)
{
	os << "[" << v.glb << ".." << v.lub << "]";
	return os;
}

void test()
{
	Util::StdRange<int> universe(0,3);
	Solver solver;
	Goal labeling(succeed());
	MetaVar<int> xi(solver,universe,labeling);
	MetaVar<int> yi(solver,universe,labeling);
	//MetaVar<int> zi(solver,universe,labeling);
	//MetaVar<int> xo(solver,universe,labeling);
	//MetaVar<int> yo(solver,universe,labeling);

	//auto c1 = rel<Equal>(rel<Add>(xi,yi),zi);
	auto c1 = rel<Equal>(xi,yi);

	bool found = true;

	found &= solver.post(MetaFilterDD()(solver,c1));

	//MetaDomView<int,Rel2<Add,MetaVar<int>,MetaVar<int> > > v(solver,rel<Add>(xi,yi));

	//found &= solver.post(xi.equals(IntSetVar(solver,list(1,2),list<int>())));
	//found &= solver.post(yi.equals(IntSetVar(solver,list(1),list(2))));
	//auto setOne = IntSetVar(solver,list(1),list<int>());
	//found &= solver.post(yo.equals(IntSetVar(solver,list(1),list(2))));

	//found &= solver.post(not yo.contains(setOne));

	std::cout << "solving\n";
	found &= solver.solve(labeling);
	while (found)
	{
		std::cout << c1 << std::endl;
		//std::cout << xi << " " << yi << " : " << xo << " " << yo << std::endl;
		found = solver.next();
	}
	std::cout << solver.getStats() << std::endl;
}

int main()
{
	test();
	return 0;
}
