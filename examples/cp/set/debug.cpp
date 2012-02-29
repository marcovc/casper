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
#include <fstream>
#include <set>

using namespace Casper;
using namespace Casper::CP;

namespace Casper {

// Succeeds if g succeeds, fails otherwise. Does not backtrack on g after it
// succeeds.
struct Succeeds : IGoal
{
	Succeeds(State& state, const Goal& g) : explorer(state),state(state),g(g) {}
	Goal execute()
	{
		//DFSExplorer explorer(state);
		if (explorer.explore(g))
		{
			explorer.restart();
			return succeed();
		}
		explorer.restart();
		return fail();
		//return explorer.explore(g)?succeed():fail();
	}
	DFSExplorer explorer;
	State&	state;
	Goal g;
};

Goal succeeds(State& state, const Goal& g)
{	return new (state) Succeeds(state,g);	}

namespace CP {

template<class Eval>
struct MetaVar;

template<>
struct MetaVar<int>
{
	typedef Util::StdRange<int>	Universe;

	MetaVar(Solver& solver, const Universe& universe, bool range = false) :
		solver(solver),universe(universe),dom(solver,universe),
		ma(solver,universe.lb,universe.ub),mi(solver,universe.lb,universe.ub),
		range(range)
	{
		if (!range)
		{
			solver.post(cardinal(dom)>0);
			solver.post(maxEqual(dom,ma));
			solver.post(minEqual(dom,mi));
		}
		solver.post(ma>=mi);
	}

	const MetaVar& operator=(const MetaVar& m)
	{
		dom = m.dom;
		ma = m.ma;
		mi = m.mi;
		range = m.range;
		return *this;
	}

	Universe getUniverse() const
	{	return universe;	}

	BndExpr<bool> rangeContains(const int& s) const
	{
		return BndExpr<bool>(solver, s>=mi and s<=ma);
	}

	BndExpr<bool> 	rangeContains(const Var<int>& s) const
	{
		return BndExpr<bool>(solver, s>=mi and s<=ma);
	}

	BndExpr<bool> 	rangeContains(const MetaVar<int>& s) const
	{
		return BndExpr<bool>(solver, s.mi>=mi and s.ma<=ma);
	}
	Var<int> min() const
	{
		return mi;
	}

	Var<int> max() const
	{
		return ma;
	}

	BndExpr<bool> contains(const int& s) const
	{
		if (range)
			return rangeContains(s);
		return BndExpr<bool>(solver,member(s,dom));
	}

	BndExpr<bool> 	contains(const Var<int>& s) const
	{
		if (range)
			return rangeContains(s);
		return BndExpr<bool>(solver,member(s,dom));
	}

	BndExpr<bool> contains(const MetaVar<int >& v) const
	{
		if (range)
			return rangeContains(v);
		return BndExpr<bool>(solver,contained(v.dom,dom) and v.ma<=ma and v.mi>=mi);
	}

	BndExpr<bool> equals(const int& s) const
	{
		if (range)
			return BndExpr<bool>(solver,mi==ma and mi==s);
		Util::StdList<int> empty;
		Var<Set<int> > g(solver,list(s),empty);
		return BndExpr<bool>(solver,g==dom);
	}

	BndExpr<bool> equals(const Var<int >& s) const
	{
		if (range)
			return BndExpr<bool>(solver,mi==ma and mi==s);
		Util::StdList<int> empty;
		Var<Set<int> > g(solver,s.domain().begin(),s.domain().end(),empty.begin(),empty.end());
		return BndExpr<bool>(solver,dom==g);
	}

	Solver& solver;
	const Util::StdRange<int> universe;
	Var<Set<int> > dom;
	Var<int> ma;
	Var<int> mi;
	bool range;
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



template<class T, int dims = 1>
struct MetaVarArray : Util::StdArray<MetaVar<T>,dims>
{
	private:
	typedef MetaVarArray<T,dims>					Self;

	public:
	typedef Util::StdArray<MetaVar<T>,dims>		Super;

	/// Type of element stated in the array.
	typedef  typename Super::Elem		Elem;

	/// Element iterator.
	typedef typename Super::Iterator	Iterator;

	/// Creates a new MetaVarArray pointing to an existing MetaVarArray \p s.
	MetaVarArray(const MetaVarArray& s) : Super(s) { }

	/// Creates a new MetaVarArray pointing to an existing MetaVarArray \p s.
	MetaVarArray(Solver& solver, const Super& s) : Super(solver,s) {}

	/** Creates a new MetaVarArray with a copy of variables in the given
	 	range [\p b, \p e[.	*/
	MetaVarArray(Solver& solver, Iterator b, Iterator e) :
			Super(solver,b,e) {}

	/// Creates a new MetaVarArray with \p p1 variables.
	MetaVarArray(Solver& solver,uint p1) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(solver,p1,solver))
		{}

	MetaVarArray(Solver& solver,const Expr<Seq<T> >& expr);

	/** Creates a new MetaVarArray with \p p1 variables, and initializes each
		with the remaining parameter.	*/
	template<class T2>
	MetaVarArray(Solver& solver,uint p1,const T2& p2) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(solver,p1,p2,solver))
		{}

	/** Creates a new MetaVarArray with \p p1 variables, and initializes each
		with the remaining parameters.	*/
	template<class T2,class T3>
	MetaVarArray(Solver& solver,uint p1,const T2& p2,const T3& p3) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(solver,p1,p2,p3,solver))
		{}

	/** Creates a new MetaVarArray with \p p1 variables, and initializes each
		with the remaining parameters.	*/
	template<class T2,class T3,class T4>
	MetaVarArray(Solver& solver,uint p1,const T2& p2,const T3& p3,const T4& p4) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(solver,p1,p2,p3,p4,solver))
		{}

	/** Creates a new MetaVarArray with \p p1 variables, and initializes each
		with the remaining parameters.	*/
	template<class T2,class T3,class T4,class T5>
	MetaVarArray(Solver& solver,uint p1,const T2& p2,const T3& p3,const T4& p4,const T5& p5) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(solver,p1,p2,p3,p4,p5,solver))
		{}

	/// Returns the Store object associated with this array.
	Store& getStore() const {	return this->operator[](0).solver;	}

	private:

	typedef typename Super::Data		Data;

	static void initCell(MetaVar<T>* mem, Solver& solver)
	{	::new(mem) MetaVar<T>(solver); }

	template<class T1>
	static void initCell(MetaVar<T>* mem, const T1& p1, Solver& solver)
	{	::new(mem) MetaVar<T>(solver,p1); }

	template<class T1,class T2>
	static void initCell(MetaVar<T>* mem, const T1& p1, const T2& p2, Solver& solver)
	{	::new(mem) MetaVar<T>(solver,p1,p2);	}

	template<class T1,class T2,class T3>
	static void initCell(MetaVar<T>* mem, const T1& p1, const T2& p2, const T3& p3, Solver& solver)
	{	::new(mem) MetaVar<T>(solver,p1,p2,p3);	}

	template<class T1,class T2,class T3,class T4>
	static void initCell(MetaVar<T>* mem, const T1& p1, const T2& p2, const T3& p3, const T4& p4, Solver& solver)
	{	::new(mem) MetaVar<T>(solver,p1,p2,p3,p4);	}

	friend class Util::Detail::ArrayTraits<T,dims-1>;
	friend class Util::Detail::ArrayDataTraits<Self,dims>;

	//Solver& solver;
};

} // CP

namespace Util {

template<>
struct Hash<CP::MetaVar<int> >
{
	size_t operator()(const CP::MetaVar<int>& t) const
	{	return reinterpret_cast<size_t>(&t.dom.domain().in()) +
			   reinterpret_cast<size_t>(&t.dom.domain().poss());	}
};

template<>
struct UseEqualOp<CP::MetaVar<int> >
{
	bool operator()(const CP::MetaVar<int>& f,const CP::MetaVar<int>& t) const
	{	return &f.dom.domain().in() == &t.dom.domain().in() and
			   &f.dom.domain().poss() == &t.dom.domain().poss();	}
};

} // Util

namespace Traits {

template<class Eval>
struct GetEval<CP::MetaVar<Eval> >
{	typedef Eval	Type;	};

template<class Eval,int dims>
struct GetEval<CP::MetaVarArray<Eval,dims> >
{	typedef Seq<Eval>	Type;	};


} // Traits

namespace CP {

Goal label(Store& store,const MetaVarArray<int>& x)
{
	VarArray<int> mima(store,x.size()*2);
	// count doms which are range only
	int nbSetDoms = 0;
	for (uint i = 0; i < x.size(); ++i)
		nbSetDoms += not x[i].range;
	VarArray<IntSet> doms(store,nbSetDoms);
	uint c = 0;
	for (uint i = 0; i < x.size(); ++i)
	{
		mima[i*2] = x[i].mi;
		mima[i*2+1] = x[i].ma;
		if (not x[i].range)
			doms[c++] = x[i].dom;
	}
	if (nbSetDoms==0)
		return Goal(store,label(store,mima,selectVarMinDom(store,mima)) );
	return Goal(store,label(store,doms,selectVarMinDom(store,doms)) and
					  label(store,mima,selectVarMinDom(store,mima)));
}

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

// ---

template<class Obj,class ToExtract,class Equal>
struct ExtractFromObj;

template<class Obj,class Equal>
struct ExtractFromObj<Obj,Obj,Equal>
{
	void operator()(const Obj& v, Util::StdList<Obj>& l) const
	{
		for (auto it = l.begin(); it != l.end(); ++it)
			if (Equal()(*it,v))
				return;
		l.pushBack(v);
	}
};

template<class Func,class Obj1,class ToExtract,class Equal>
struct ExtractFromObj<Rel1<Func,Obj1>,ToExtract,Equal>
{
	void operator()(const Rel1<Func,Obj1>& r, Util::StdList<ToExtract>& l) const
	{	ExtractFromObj<Obj1,ToExtract,Equal>()(r.p1,l);	}
};

template<class Func,class Obj1,class Obj2,class ToExtract,class Equal>
struct ExtractFromObj<Rel2<Func,Obj1,Obj2>,ToExtract,Equal>
{
	void operator()(const Rel2<Func,Obj1,Obj2>& r, Util::StdList<ToExtract>& l) const
	{
		ExtractFromObj<Obj1,ToExtract,Equal>()(r.p1,l);
		ExtractFromObj<Obj2,ToExtract,Equal>()(r.p2,l);
	}
};

template<class Eval,class Equal>
struct ExtractFromObj<MetaVarArray<Eval>,MetaVar<Eval>,Equal>
{
	void operator()(const MetaVarArray<Eval>& v, Util::StdList<MetaVar<Eval> >& l) const
	{
		for (uint i = 0; i < v.size(); ++i)
		{
			auto it = l.begin();
			for ( ; it != l.end(); ++it)
				if (Equal()(*it,v[i]))
					break;
			if (it == l.end())
				l.pushBack(v[i]);
		}
	}
};

// ---

template<class Obj,class From, class To>
struct ReplaceInObj
{
	typedef Obj	RetType;
	RetType operator()(const Obj& obj, const Util::StdHashMap<From,To>& m)
	{	return obj;	}
};

template<class From,class To>
struct ReplaceInObj<From,From,To>
{
	typedef To	RetType;
	RetType operator()(const From& from, const Util::StdHashMap<From,To>& m)
	{
		auto it = m.find(from);
		assert(it != m.end());
		return it->second;
	}
};

template<class Func,class Obj1,class From,class To>
struct ReplaceInObj<Rel1<Func,Obj1>,From,To>
{
	typedef Rel1<Func,typename ReplaceInObj<Obj1,From,To>::RetType> RetType;
	RetType operator()(const Rel1<Func,Obj1>& r, const Util::StdHashMap<From,To>& m)
	{
		ReplaceInObj<Obj1,From,To> rp1;
		return rel<Func>(rp1(r.p1,m));
	}
};

template<class Func,class Obj1,class Obj2,class From,class To>
struct ReplaceInObj<Rel2<Func,Obj1,Obj2>,From,To>
{
	typedef Rel2<Func,typename ReplaceInObj<Obj1,From,To>::RetType,
					  typename ReplaceInObj<Obj2,From,To>::RetType> RetType;
	RetType operator()(const Rel2<Func,Obj1,Obj2>& r, const Util::StdHashMap<From,To>& m)
	{
		ReplaceInObj<Obj1,From,To> rp1;
		ReplaceInObj<Obj2,From,To> rp2;
		return rel<Func>(rp1(r.p1,m),rp2(r.p2,m));
	}
};

template<class Eval>
struct ReplaceInObj<MetaVarArray<Eval>,MetaVar<Eval>,Var<Eval> >
{
	typedef VarArray<Eval>	RetType;
	RetType operator()(const MetaVarArray<Eval>& x, const Util::StdHashMap<MetaVar<Eval>,Var<Eval> >& m)
	{
		RetType r(x.getStore(),x.size());
		for (uint i = 0; i < x.size(); ++i)
		{
			auto it = m.find(x[i]);
			assert(it != m.end());
			r[i] = it->second;
		}
		return r;
	}
};

// ---


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
		std::cout << "contains(" << s << ")\n";
		Solver posSolver;
		Var<Eval> s1(posSolver,v1.getUniverse());
		Var<Eval> s2(posSolver,v2.getUniverse());
		bool found = posSolver.post(rel<Func>(s1,s2)==s);
		found &= posSolver.solve(label(posSolver,{s1,s2}));
		while (found)
		{
			std::cout << "found " << s1.domain().value() << "+" << s2.domain().value() << std::endl;
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
		Var<Eval> aux(posSolver,getUniverse());

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

	template<class Obj>
	BndExpr<bool> entailed(Solver& solver, const Obj& in)
	{
		Util::StdList<MetaVar<int> >	intMetaVarList;

		ExtractFromObj<Obj,MetaVar<int>,Casper::Util::UseEqualOp<MetaVar<int> > >()(in,intMetaVarList);

		BndExpr<bool> ret(solver,true);

		Solver solver1;
		VarArray<int> ivars = getSupports(solver1,intMetaVarList);
		Util::StdHashMap<MetaVar<int>,Var<int> > m;
		Util::StdArray<MetaVar<int>* > minv(ivars.size());
		uint c = 0;
		for (auto it = intMetaVarList.begin(); it != intMetaVarList.end(); ++it,++c)
		{
			m.insert(*it,ivars[c]);
			minv[c] = &*it;
		}
		ReplaceInObj<Obj,MetaVar<int>,Var<int> > rr;
		solver1.post(not rr(in,m));
		bool found1 = solver1.solve(label(solver1,ivars));
		while (found1)
		{
			BndExpr<bool> ored(solver,false);
			for (uint i = 0; i < ivars.size(); ++i)
			{
				ored = ored or (not minv[i]->contains(ivars[i].domain().value()));
			}
			ret = ret and ored;
			found1 = solver1.next();
		}
		return ret;
	}

	template<class Eval>
	VarArray<Eval> getSupports(Solver& solver, const Util::StdList<MetaVar<Eval> >& v)
	{
		VarArray<Eval> r(solver,v.size());
		uint c = 0;
		for (auto it = v.begin(); it != v.end(); ++it, ++c)
			r[c] = Var<Eval>(solver,it->getUniverse());
		return r;
	}

#if 0
	// O(dn^2) variables (supports)
	template<class Obj>
	BndExpr<bool> operator()(Solver& solver, const Obj& in, Util::StdList<Var<int> >& labeling)
	{
		Util::StdList<MetaVar<int> >	intMetaVarList;

		ExtractFromObj<Obj,MetaVar<int>,Casper::Util::UseEqualOp<MetaVar<int> > >()(in,intMetaVarList);

		BndExpr<bool> ret(solver,true);

		uint c = 0;
		for (auto it = intMetaVarList.begin(); it != intMetaVarList.end(); ++it, ++c)
		{
			Solver solver1;
			Var<int> intVar(solver1,it->getUniverse());
			bool found1 = solver1.solve(label(solver1,list(intVar)));
			while (found1)
			{
				VarArray<int> supports = getSupports(solver,intMetaVarList);
				supports[c] = Var<int>(solver,intVar.domain().value());

				BndExpr<bool> isSupported(solver,true);
				Util::StdHashMap<MetaVar<int>,Var<int> > m;
				uint c2 = 0;
				for (auto it2 = intMetaVarList.begin(); it2 != intMetaVarList.end(); ++it2,++c2)
				{
					m.insert(*it2,supports[c2]);
					if (c!=c2)
						//isSupported = isSupported and it2->contains(supports[c2]);
						solver.post(it2->contains(supports[c2]));
				}
				ReplaceInObj<Obj,MetaVar<int>,Var<int> > rr;
				if (ChkExpr(solver,rr(in,m)).canBeTrue())
				{
					//solver.post(rr(in,m));
					isSupported = isSupported and rr(in,m);
					ret = ret and (it->contains(intVar.domain().value())<=isSupported);
					labeling.insert(labeling.end(),supports.begin(),supports.end());
				}
				else
					ret = ret and (!it->contains(intVar.domain().value()));
				found1 = solver1.next();
			}
		}

		return ret;
	}
#else
	// O(dn^2) variables (supports)
	template<class Obj>
	BndExpr<bool> operator()(Solver& solver, const Obj& in, Util::StdList<Var<int> >& labeling)
	{
		Util::StdList<MetaVar<int> >	intMetaVarList;

		ExtractFromObj<Obj,MetaVar<int>,Casper::Util::UseEqualOp<MetaVar<int> > >()(in,intMetaVarList);

		typedef Util::StdList<int>					Solution;	// tuples of integers which are solutions
		typedef Util::StdList<Solution*>			Solutions;	// list of pointers to solutions
		typedef Util::StdHashMap<int,Solutions>		Support;	// value to list of supports of value
		typedef Util::StdArray<Support*>				Supports;	// in variable indexed array

		// collect solutions to constraint
		Supports supports(intMetaVarList.size());
		for (uint i = 0; i < supports.size(); ++i)
			supports[i] = new Support();
		Util::StdHashMap<MetaVar<int>,Var<int> > m;

		Solver solver1;
		VarArray<int> x = getSupports(solver1,intMetaVarList);
		uint c = 0;
		for (auto it = intMetaVarList.begin(); it != intMetaVarList.end(); ++it,++c)
			m.insert(*it,x[c]);
		bool found1 = solver1.post(ReplaceInObj<Obj,MetaVar<int>,Var<int> >()(in,m));
		found1 &= solver1.solve(label(solver1,x));
		while (found1)
		{
			Solution* solution = new Solution();
			uint c = 0;
			for (auto it = intMetaVarList.begin(); it != intMetaVarList.end(); ++it, ++c)
			{
				solution->pushBack(x[c].domain().value());
				auto mit = supports[c]->find(x[c].domain().value());
				if (mit == supports[c]->end())
					(*supports[c])[x[c].domain().value()] = Solutions();
				(*supports[c])[x[c].domain().value()].pushBack(solution);
			}
			found1 = solver1.next();
		}

		// post membership constraints
		BndExpr<bool> ret(solver,true);
		c = 0;
		for (auto it = intMetaVarList.begin(); it != intMetaVarList.end(); ++it, ++c)
		{
			for (auto valit = supports[c]->begin(); valit != supports[c]->end(); ++valit)
			{
				BndExpr<bool> ored(solver,not it->contains(valit->first));
				for (auto solit = valit->second.begin(); solit != valit->second.end(); ++solit)
				{
					BndExpr<bool> conj(solver,true);
					uint c2 = 0;
					auto invarit = intMetaVarList.begin();
					auto varvalit = (*solit)->begin();
					for ( ; varvalit != (*solit)->end(); ++varvalit, ++c2, ++invarit)
						if (c!=c2)
							conj = conj and invarit->contains(*varvalit);
					ored = ored or conj;
				}
				ret = ret and ored;
			}
			for (auto uit = it->getUniverse().begin(); uit != it->getUniverse().end(); ++uit)
			{
				auto valit = supports[c]->begin();
				for (	; valit != supports[c]->end(); ++valit)
					if (*uit == valit->first)
						break;
				if (valit==supports[c]->end())
					ret = ret and (not it->contains(*uit));
			}
		}
		return ret;
	}
#endif

	// O(dn^2) variables (supports)
	template<class Obj>
	BndExpr<bool> operator()(Solver& solver, const Obj& in, const Obj& out, Util::StdList<Var<int> >& labeling)
	{
		solver.post(this->operator()(solver,out,labeling));

		Util::StdList<MetaVar<int> >	intInMetaVarList;
		Util::StdList<MetaVar<int> >	intOutMetaVarList;

		ExtractFromObj<Obj,MetaVar<int>,Casper::Util::UseEqualOp<MetaVar<int> >  >()(in,intInMetaVarList);
		ExtractFromObj<Obj,MetaVar<int>,Casper::Util::UseEqualOp<MetaVar<int> >  >()(out,intOutMetaVarList);

		auto itin = intInMetaVarList.begin();
		auto itout = intOutMetaVarList.begin();

		for ( ; itin != intInMetaVarList.end() and itout != intOutMetaVarList.end(); ++itin,++itout)
			solver.post(itin->contains(*itout));

		BndExpr<bool> ret(solver,true);

		Util::StdHashMap<MetaVar<int>,Var<int> > m;

		Solver solver1;
		VarArray<int> x = getSupports(solver1,intInMetaVarList);
		uint c = 0;
		for (auto it = intInMetaVarList.begin(); it != intInMetaVarList.end(); ++it,++c)
			m.insert(*it,x[c]);
		bool found1 = solver1.post(ReplaceInObj<Obj,MetaVar<int>,Var<int> >()(in,m));
		found1 &= solver1.solve(label(solver1,x));
		while (found1)
		{
			uint co = 0;
			for (auto ito = intOutMetaVarList.begin(); ito != intOutMetaVarList.end(); ++ito, ++co)
			{
				BndExpr<bool> disj(solver,false);
				uint ci = 0;
				for (auto iti = intInMetaVarList.begin(); iti != intInMetaVarList.end(); ++iti, ++ci)
					disj = disj or (not iti->contains(x[ci].domain().value()));
				ret = ret and (disj or ito->contains(x[co].domain().value()));
			}
			found1 = solver1.next();
		}
		return ret;
	}

};


struct MetaFilterBB
{
	template<class Eval>
	VarArray<Eval> getSupports(Solver& solver, const Util::StdList<MetaVar<Eval> >& v)
	{
		VarArray<Eval> r(solver,v.size());
		uint c = 0;
		for (auto it = v.begin(); it != v.end(); ++it, ++c)
			r[c] = Var<Eval>(solver,it->getUniverse());
		return r;
	}

	// O(2n^2) variables (supports)
	template<class Obj>
	BndExpr<bool> operator()(Solver& solver, const Obj& in, Goal& labeling)
	{
		Util::StdList<MetaVar<int> >	intMetaVarList;

		ExtractFromObj<Obj,MetaVar<int>,Casper::Util::UseEqualOp<MetaVar<int> >  >()(in,intMetaVarList);

		BndExpr<bool> ret(solver,true);

		uint c = 0;
		for (auto it = intMetaVarList.begin(); it != intMetaVarList.end(); ++it, ++c)
		{
			VarArray<int> ubSupports = getSupports(solver,intMetaVarList);
			VarArray<int> lbSupports = getSupports(solver,intMetaVarList);

			ubSupports[c] = it->min();
			lbSupports[c] = it->max();

			BndExpr<bool> ubIsSupported(solver,true);
			BndExpr<bool> lbIsSupported(solver,true);
			Util::StdHashMap<MetaVar<int>,Var<int> > ubM;
			Util::StdHashMap<MetaVar<int>,Var<int> > lbM;
			uint c2 = 0;
			for (auto it2 = intMetaVarList.begin(); it2 != intMetaVarList.end(); ++it2,++c2)
			{
				ubM.insert(*it2,ubSupports[c2]);
				lbM.insert(*it2,lbSupports[c2]);
				if (c!=c2)
				{
					//ubIsSupported = ubIsSupported and it2->rangeContains(ubSupports[c2]);
					//lbIsSupported = lbIsSupported and it2->rangeContains(lbSupports[c2]);
					solver.post(it2->rangeContains(ubSupports[c2]));
					solver.post(it2->rangeContains(lbSupports[c2]));
				}
			}
			ReplaceInObj<Obj,MetaVar<int>,Var<int> > rr;
			ubIsSupported = ubIsSupported and rr(in,ubM);
			lbIsSupported = lbIsSupported and rr(in,lbM);
			ret = ret and lbIsSupported;
			ret = ret and ubIsSupported;
			labeling = Goal(solver,labeling and label(solver,lbSupports) and
												label(solver,ubSupports));
		}

		return ret;
	}

	// O(dn^2) variables (supports)
	template<class Obj>
	BndExpr<bool> operator()(Solver& solver, const Obj& in, const Obj& out, Goal& labeling)
	{
		solver.post(this->operator()(solver,out,labeling));

		Util::StdList<MetaVar<int> >	intInMetaVarList;
		Util::StdList<MetaVar<int> >	intOutMetaVarList;

		ExtractFromObj<Obj,MetaVar<int>,Casper::Util::UseEqualOp<MetaVar<int> >  >()(in,intInMetaVarList);
		ExtractFromObj<Obj,MetaVar<int>,Casper::Util::UseEqualOp<MetaVar<int> >  >()(out,intOutMetaVarList);

		auto itin = intInMetaVarList.begin();
		auto itout = intOutMetaVarList.begin();

		for ( ; itin != intInMetaVarList.end() and itout != intOutMetaVarList.end(); ++itin,++itout)
			solver.post(itin->contains(*itout));

		BndExpr<bool> ret(solver,true);

		uint c = 0;
		for (auto it = intOutMetaVarList.begin(); it != intOutMetaVarList.end(); ++it, ++c)
		{
			Solver solver1;
			Var<int> intVar(solver1,it->getUniverse());
			bool found1 = solver1.solve(label(solver1,list(intVar)));
			while (found1)
			{
				VarArray<int> supports = getSupports(solver,intInMetaVarList);
				supports[c] = Var<int>(solver,intVar.domain().value());

				BndExpr<bool> isSupported(solver,true);
				Util::StdHashMap<MetaVar<int>,Var<int> > m;
				uint c2 = 0;
				for (auto it2 = intInMetaVarList.begin(); it2 != intInMetaVarList.end(); ++it2,++c2)
				{
					m.insert(*it2,supports[c2]);
					if (c!=c2)
						isSupported = isSupported and it2->rangeContains(supports[c2]);
				}
				ReplaceInObj<Obj,MetaVar<int>,Var<int> > rr;
				if (ChkExpr(solver,rr(in,m)).canBeTrue())
				{
					solver.post(rr(in,m));
					ret = ret and (it->rangeContains(intVar.domain().value())<=isSupported);
					auto it2 = intInMetaVarList.begin();
					for (uint c3 = 0; c3 < c; ++c3)
						++it2;
					ret = ret and ((it2->contains(intVar.domain().value()) and
									isSupported)<=it->contains(intVar.domain().value()));
					labeling = Goal(solver,labeling and label(solver,supports));
				}
				else
					ret = ret and (!it->contains(intVar.domain().value()));
				found1 = solver1.next();
			}
		}

		return ret;
	}

};

} // CP
} // Casper

std::ostream& operator<<(std::ostream& os, const Casper::CP::MetaVar<int>& v)
{
	if (v.range)
		os << "[" << v.mi << ".." << v.ma << "]";
	else
		os << v.dom;
	return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const Casper::CP::MetaVar<Set<T> >& v)
{
	os << "[" << v.glb << ".." << v.lub << "]";
	return os;
}

BndExpr<bool> strictlyContains(Solver& solver, const MetaVarArray<int>& x1, const MetaVarArray<int>& x2)
{
	BndExpr<bool> ret(solver,true);
	assert(x1.size()==x2.size());
	// contains
	for (uint i = 0; i < x1.size(); ++i)
		ret = ret and x1[i].contains(x2[i]);
	// strictly contains
	BndExpr<bool> ored(solver,false);
	for (uint i = 0; i < x1.size(); ++i)
		ored = ored or (not x2[i].contains(x1[i]));
	return BndExpr<bool>(solver,ret and ored);
}

BndExpr<bool> strictlyRangeContains(Solver& solver, const MetaVarArray<int>& x1, const MetaVarArray<int>& x2)
{
	BndExpr<bool> ret(solver,true);
	assert(x1.size()==x2.size());
	// contains
	for (uint i = 0; i < x1.size(); ++i)
		ret = ret and x1[i].contains(x2[i]);
	// strictly contains
	BndExpr<bool> ored(solver,false);
	for (uint i = 0; i < x1.size(); ++i)
		ored = ored or (not x2[i].rangeContains(x1[i]));
	return BndExpr<bool>(solver,ret and ored);
}

struct BoundDomainRules
{
	typedef Util::StdRange<int> 	Range;
	typedef Util::StdList<Range>	Dom;
	typedef Util::StdPair<Dom>		DomPair;
	typedef Util::StdList<DomPair>	DomPairs;
	template<class InputIterator1, class InputIterator2>
	void store(InputIterator1 beginObj, InputIterator1 endObj,
				InputIterator2 beginTarget, InputIterator2 endTarget)
	{
		Dom objDom;
		for (auto it = beginObj; it != endObj; ++it)
			objDom.pushBack(Range(it->min().domain().value(),it->max().domain().value()));
		Dom targetDom;
		for (auto it = beginTarget; it != endTarget; ++it)
			targetDom.pushBack(Range(it->min().domain().value(),it->max().domain().value()));
		domPairs.pushBack(DomPair(objDom,targetDom));
	}
	bool equal(const Dom& d1, const Dom& d2) const
	{	return d1 == d2;	}

	bool contained(const Dom& d1, const Dom& d2) const
	{
		auto it1 = d1.begin();
		auto it2 = d2.begin();
		for ( ; it1 != d1.end(); ++it1,++it2)
			if (it1->min()<it2->min() or it1->max()>it2->max())
				break;
		if (it1 == d1.end())
		{
			assert(it2 == d2.end());
			return true;
		}
		assert(it2 != d2.end());
		return false;
	}

	void simplify()
	{
		std::cout << "merging " << domPairs.size() << " nogoods to ... ";
		for (auto it1 = domPairs.begin(); it1 != domPairs.end(); )
		{
			bool found = false;
			for (auto it2 = ++typename DomPairs::Iterator(it1); it2 != domPairs.end(); )
			{
				if (equal(it1->second,it2->second))
				{
					if (contained(it1->first,it2->first))
					{
						domPairs.erase(it1++);
						found = true;
						break;
					}
					else
					if (contained(it2->first,it1->first))
					{
						domPairs.erase(it2++);
						continue;
					}
				}
				++it2;
			}
			if (!found)
				++it1;
		}
		std::cout << domPairs.size() << " nogoods\n";
	}

	void simplify2()
	{
		std::cout << "merging " << domPairs.size() << " nogoods to ... ";
		for (auto it1 = domPairs.begin(); it1 != domPairs.end(); )
		{
			bool found = false;
			for (auto it2 = ++typename DomPairs::Iterator(it1); it2 != domPairs.end(); )
			{
				if (contained(it1->first,it2->first))
				{
					auto itd1 = it1->second.begin();
					auto its1 = it1->first.begin();
					auto itd2 = it2->second.begin();
					for ( ; itd1 != it1->second.end(); ++itd1, ++itd2, ++its1)
						if ((itd1->min()!=itd2->min() or itd1->max()!=itd2->max()) and
							(itd1->min()!=its1->min() or itd1->max()!=its1->max()))
							break;
					if (itd1 == it1->second.end())
					{
						domPairs.erase(it1++);
						found = true;
						break;
					}
				}
				else
				if (contained(it2->first,it1->first))
				{
					auto itd2 = it2->second.begin();
					auto its2 = it2->first.begin();
					auto itd1 = it1->second.begin();
					for ( ; itd1 != it1->second.end(); ++itd1, ++itd2, ++its2)
						if ((itd2->min()!=itd1->min() or itd2->max()!=itd1->max()) and
							(itd2->min()!=its2->min() or itd2->max()!=its2->max()))
							break;
					if (itd1 == it1->second.end())
					{
						domPairs.erase(it2++);
						continue;
					}
				}
				++it2;
			}
			if (!found)
				++it1;
		}
		std::cout << domPairs.size() << " nogoods\n";
	}

	void print()
	{
		for (auto it = domPairs.begin(); it != domPairs.end(); ++it)
			std::cout << it->first << " --> " << it->second << std::endl;
	}

	DomPairs domPairs;
};

void distinctMult()
{
	Util::StdRange<int> universe(-2,2);
	Solver solver;
	Util::StdList<Var<int> > labeling;

	MetaVarArray<int> xi(solver,3,universe,true);
	MetaVarArray<int> xoT(solver,3,universe,true);
	//MetaVarArray<int> xoL(solver,3,universe,true);

	bool found = true;

	auto ci1 = rel<Equal>(rel<Mul>(xi[0],xi[1]),xi[2]);
	auto ci2 = rel<Distinct>(xi);

	auto co1 = rel<Equal>(rel<Mul>(xoT[0],xoT[1]),xoT[2]);
	auto co2 = rel<Distinct>(xoT);

	auto ciT = ci1 and ci2;
	auto coT = co1 and co2;

	found &= solver.post(MetaFilterDD()(solver,ciT,coT,labeling));

	found &= solver.post(MetaFilterDD()(solver,ci1,labeling));
	found &= solver.post(MetaFilterDD()(solver,ci2,labeling));

	found &= solver.post(strictlyRangeContains(solver,xi,xoT));

	std::cout << "solving\n";
	MetaVarArray<int> toLabel(solver,6,universe);
	for (uint i = 0; i < 3; ++i)
	{
		toLabel[i*2] = xi[i];
		toLabel[i*2+1] = xoT[i];
		//toLabel[i*3+2] = xoL[i];
	}

	found &= solver.solve(label(solver,toLabel) and
						succeeds(solver,label(solver,labeling)));

	BoundDomainRules rules;

	while (found)
	{
		rules.store(xi.begin(), xi.end(), xoT.begin(),xoT.end());
		//std::cout << xi << " " << xoT << " " << xoL << std::endl;
		std::cout << "local: " << xi << "\t>> global: " << xoT << std::endl;

		found = solver.next();
	}
	std::cout << solver.getStats() << std::endl;
	rules.simplify();
	rules.print();
}

void golomb()
{
	Util::StdRange<int> universeM(0,6);
	Util::StdRange<int> universeD(1,6);
	Solver solver;
	Util::StdList<Var<int> > labeling;

	MetaVarArray<int> mi(solver,3,universeM,true);
	MetaVarArray<int> di(solver,3,universeD,true);

	MetaVarArray<int> moT(solver,3,universeM,true);
	MetaVarArray<int> doT(solver,3,universeD,true);

//	MetaVarArray<int> moL(solver,3,universe,true);
//	MetaVarArray<int> doL(solver,3,universe,true);

	bool found = true;

	auto ci1 = rel<Less>(mi[0],mi[1]);
	auto ci2 = rel<Less>(mi[1],mi[2]);
	auto ci3 = rel<Equal>(rel<Sub>(mi[1],mi[0]),di[0]);
	auto ci4 = rel<Equal>(rel<Sub>(mi[2],mi[1]),di[1]);
	auto ci5 = rel<Equal>(rel<Sub>(mi[2],mi[0]),di[2]);
	auto ci6 = rel<Distinct>(di);

	auto coT = 	  rel<Less>(moT[0],moT[1]) and
				  rel<Less>(moT[1],moT[2]) and
				  rel<Equal>(rel<Sub>(moT[1],moT[0]),doT[0]) and
				  rel<Equal>(rel<Sub>(moT[2],moT[1]),doT[1]) and
				  rel<Equal>(rel<Sub>(moT[2],moT[0]),doT[2]) and
				  rel<Distinct>(doT);

	auto ciT = ci1 and ci2 and ci3 and ci4 and ci5 and ci6;

	found &= solver.post(MetaFilterDD()(solver,ciT,coT,labeling));
/*
	auto co1 = rel<Less>(moL[0],moL[1]);
	auto co2 = rel<Less>(moL[1],moL[2]);
	auto co3 = rel<Equal>(rel<Sub>(moL[1],moL[0]),doL[0]);
	auto co4 = rel<Equal>(rel<Sub>(moL[2],moL[1]),doL[1]);
	auto co5 = rel<Equal>(rel<Sub>(moL[2],moL[0]),doL[2]);
	auto co6 = rel<Distinct>(doL);
*/
	found &= solver.post(MetaFilterDD()(solver,ci1,labeling));
	found &= solver.post(MetaFilterDD()(solver,ci2,labeling));
	found &= solver.post(MetaFilterDD()(solver,ci3,labeling));
	found &= solver.post(MetaFilterDD()(solver,ci4,labeling));
	found &= solver.post(MetaFilterDD()(solver,ci5,labeling));
	found &= solver.post(MetaFilterDD()(solver,ci6,labeling));

	found &= solver.post(strictlyRangeContains(solver,mi,moT));

	std::cout << "solving\n";
	MetaVarArray<int> toLabel(solver,12,universeD);
	for (uint i = 0; i < 3; ++i)
	{
		toLabel[i*4] = mi[i];
		toLabel[i*4+1] = moT[i];
		toLabel[i*4+2] = di[i];
		toLabel[i*4+3] = doT[i];
	}

	found &= solver.solve(label(solver,toLabel) and
						succeeds(solver,label(solver,labeling)));
	BoundDomainRules rules;
	while (found)
	{
		Util::StdList<MetaVar<int> > aux1;
		Util::StdList<MetaVar<int> > aux2;
		for (uint i = 0; i < 3; ++i)
		{
			aux1.pushBack(mi[i]);
			aux2.pushBack(moT[i]);
		}
		for (uint i = 0; i < 3; ++i)
		{
			aux1.pushBack(di[i]);
			aux2.pushBack(doT[i]);
		}
		rules.store(aux1.begin(),aux1.end(),aux2.begin(),aux2.end());
		//std::cout << xi << " " << xoT << " " << xoL << std::endl;
		std::cout << "local: " << mi << "\t>> global: " << moT << std::endl;
		std::cout << "diffs: " << di << "\t>> diffs: " << doT << std::endl << std::endl;
		found = solver.next();
	}
	std::cout << solver.getStats() << std::endl;
	rules.simplify();
	rules.print();
	rules.simplify2();
	std::ofstream f("rules.data");
	for (auto it1 = rules.domPairs.begin(); it1 != rules.domPairs.end(); ++it1)
	{
		std::cout << it1->first << " --> " << it1->second;
		bool sym = false;
		for (auto it2 = rules.domPairs.begin(); it2 != rules.domPairs.end();  ++it2)
		{
			// check if source differences are equal
			auto itd1 = it1->first.begin();
			auto itd2 = it2->first.begin();
			uint c = 0;
			while (c<3)	{	++itd1; ++itd2;	++c; }
			for ( ; c < 6; ++c,++itd1,++itd2)
				if (itd1->min()!=itd2->min() or
					itd1->max()!=itd2->max())
					break;
			if (c<6)
				continue;
			// check if target differences are equal
			itd1 = it1->second.begin();
			itd2 = it2->second.begin();
			c = 0;
			while (c<3)	{	++itd1; ++itd2;	++c; }
			for ( ; c < 6; ++c,++itd1,++itd2)
				if (itd1->min()!=itd2->min() or
					itd1->max()!=itd2->max())
					break;
			if (c<6)
				continue;

			// check if source marks are value symmetric
			itd1 = it1->first.begin();
			itd2 = it2->first.begin();
			c = 0;
			for ( ; c < 3; ++c,++itd1,++itd2)
				if (itd1->min()-1!=itd2->min() or
					itd1->max()-1!=itd2->max())
					break;
			if (c<3)
				continue;
			// check if target marks are value symmetric
			itd1 = it1->second.begin();
			itd2 = it2->second.begin();
			c = 0;
			for ( ; c < 3; ++c,++itd1,++itd2)
				if (itd1->min()-1!=itd2->min() or
					itd1->max()-1!=itd2->max())
					break;
			if (c<3)
				continue;
			sym = true;
			std::cout << " (symmetric)";
			break;
		}
		std::cout << std::endl;

		for (auto dit = it1->first.begin() ; dit != it1->first.end(); ++dit)
			f << dit->min() << " " << dit->max() << " ";
		for (auto dit = it1->second.begin() ; dit != it1->second.end(); ++dit)
			f << dit->min() << " " << dit->max() << " ";
		f << std::endl;
	}
	f.close();
	//rules.print();
}

struct Implies : IFilter
{
	Implies(Solver& solver, const BndExpr<bool>& cond, const BndExpr<bool>& impl,
			const IntVarArray& marks, const IntVarArray& diffs,
			const Util::StdArray<int,2>& boundsCond, const Util::StdArray<int,2>& boundsImpl) :
		IFilter(solver),cond(solver,cond),impl(solver,impl),
		marks(marks),diffs(diffs),boundsCond(boundsCond),boundsImpl(boundsImpl){}

	bool execute()
	{
		if (cond.isTrue())
		{
			if (not impl.isTrue())
			{
				std::cout << "propagating rule " << std::endl;
				return impl.setToTrue();
			}
			else
			{
				std::cout << "rule is already satisfied: " << std::endl;
//				std::cout << boundsCond << " --> " << boundsImpl << std::endl;
//				std::cout << marks << " / " << diffs << std::endl;
			}
		}
		return true;
	}
	Cost cost() const
	{	return IFilter::Cost::exponentialLo;	}

	void attach(INotifiable* f)
	{	cond.attach(f);}
	void detach(INotifiable* f)
	{	cond.detach(f);}

	ChkExpr cond;
	ChkExpr impl;
	IntVarArray marks;
	IntVarArray diffs;
	Util::StdArray<int,2> boundsCond;
	Util::StdArray<int,2> boundsImpl;
};

bool postImplies(Solver& solver,  const BndExpr<bool>& cond, const BndExpr<bool>& impl,
		const IntVarArray& marks, const IntVarArray& diffs,
					const Util::StdArray<int,2>& boundsCond, const Util::StdArray<int,2>& boundsImpl)
{	return solver.post(new (solver) Implies(solver,cond,impl,marks,diffs,boundsCond,boundsImpl));	}

void golombInt(uint n,uint m)
{
	using namespace std;
	const int ndiffs = n * (n-1) / 2;

	Solver solver;

    IntVarArray	marks(solver,n,0,m);
    IntVarArray	diffs(solver,ndiffs,1,m);
    Util::StdArray<int,2> markDiffs(m,m,0);

    uint c = 0;
    for (uint i = 0; i < n; ++i)
        for (uint j = i+1; j < n; ++j)
        {
        	markDiffs[j][i] = c;
        	solver.post(marks[j]-marks[i]==diffs[c++]);
        }

    solver.post(distinct(diffs));

    for (uint i = 0; i < n-1; i++)
    	solver.post(marks[i] < marks[i+1]);

    marks[0].domain() = 0;

#if 0
	// load rules
	ifstream f("rules.data");
	while (!f.eof())
	{
		Util::StdArray<int,2> boundsCond(solver,6,2);
		Util::StdArray<int,2> boundsImpl(solver,6,2);
		for (uint i = 0; i < 6; ++i)
			f >> boundsCond[i][0] >> boundsCond[i][1];
		for (uint i = 0; i < 6; ++i)
			f >> boundsImpl[i][0] >> boundsImpl[i][1];
		f >> ws;

		int maxOfMin = limits<int>::min();
		for (uint i = 0; i < 3; ++i)
			maxOfMin = std::max(maxOfMin,boundsCond[i][0]);

		for (int x = 0; x <  n-2; ++x)
			for (int v = 0; v < m-maxOfMin; ++v)
			{
				BndExpr<bool> ruleCond(solver,true);
				BndExpr<bool> ruleImpl(solver,true);
				for (uint i = 0; i < 3; ++i)
				{
					ruleCond = ruleCond and (marks[i+x]>=boundsCond[i][0]+v and marks[i+x]<=boundsCond[i][1]+v);
					if (boundsImpl[i][0] > boundsCond[i][0])
						ruleImpl = ruleImpl and marks[i+x]>=boundsImpl[i][0]+v;
					if (boundsImpl[i][1] < boundsCond[i][1])
						ruleImpl = ruleImpl and marks[i+x]<=boundsImpl[i][1]+v;
				}
				int diffIdx = markDiffs[1+x][0+x];
				ruleCond = ruleCond and (diffs[diffIdx]>=boundsCond[3][0] and diffs[diffIdx]<=boundsCond[3][1]);
				if (boundsImpl[3][0] > boundsCond[3][0])
					ruleImpl = ruleImpl and diffs[diffIdx]>=boundsImpl[3][0];
				if (boundsImpl[3][1] < boundsCond[3][1])
					ruleImpl = ruleImpl and diffs[diffIdx]<=boundsImpl[3][1];
				diffIdx = markDiffs[2+x][1+x];
				ruleCond = ruleCond and (diffs[diffIdx]>=boundsCond[4][0] and diffs[diffIdx]<=boundsCond[4][1]);
				if (boundsImpl[4][0] > boundsCond[4][0])
					ruleImpl = ruleImpl and diffs[diffIdx]>=boundsImpl[4][0];
				if (boundsImpl[4][1] < boundsCond[4][1])
					ruleImpl = ruleImpl and diffs[diffIdx]<=boundsImpl[4][1];
				diffIdx = markDiffs[2+x][0+x];
				ruleCond = ruleCond and (diffs[diffIdx]>=boundsCond[5][0] and diffs[diffIdx]<=boundsCond[5][1]);
				if (boundsImpl[5][0] > boundsCond[5][0])
					ruleImpl = ruleImpl and diffs[diffIdx]>=boundsImpl[5][0];
				if (boundsImpl[5][1] < boundsCond[5][1])
					ruleImpl = ruleImpl and diffs[diffIdx]<=boundsImpl[5][1];
				//bool r = solver.post(ruleCond <= ruleImpl);
				bool r = postImplies(solver,ruleCond,ruleImpl,marks,diffs,boundsCond,boundsImpl);
				//solver.post(ruleCond <= debugCond(solver,marks,diffs, boundsCond, boundsImpl));
				assert(r);
			}
	}
#endif
	bool found = solver.valid();
	std::cout << marks << std::endl << diffs << std::endl;
	solver.setExplorer(bbMinimize(solver,marks[n-1]));
	//marks[n-1].domain() = m;
	found &= solver.solve(label(solver,marks,selectVarMinDom(solver,marks)));
	uint nbSolutions = 0;
	while (found)
	{
		cout << "solution " << ++nbSolutions << " : " << marks << endl;
		//break;
		found = solver.next();
	}
	cout << solver.getStats() << endl;
	cout << solver.getCPUTimer() << endl;
}

int main()
{
	//distinctMult();
	//golomb();
	golombInt(8,100);
	return 0;
}
