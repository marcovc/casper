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

	MetaVar(Solver& solver, const Universe& universe) :
		solver(solver),universe(universe),dom(solver,universe),
		ma(solver,universe.lb,universe.ub),mi(solver,universe.lb,universe.ub)
	{
		solver.post(cardinal(dom)>0);
		solver.post(maxEqual(dom,ma));
		solver.post(minEqual(dom,mi));
	}

	const MetaVar& operator=(const MetaVar& m)
	{
		dom = m.dom;
		ma = m.ma;
		mi = m.mi;
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
	{	return BndExpr<bool>(solver,member(s,dom));	}

	BndExpr<bool> 	contains(const Var<int>& s) const
	{	return BndExpr<bool>(solver,member(s,dom)); }

	BndExpr<bool> contains(const MetaVar<int >& v) const
	{	return BndExpr<bool>(solver,contained(v.dom,dom) and v.ma<=ma and v.mi>=mi); }

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
	Var<int> ma;
	Var<int> mi;
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
	VarArray<IntSet> doms(store,x.size());
	for (uint i = 0; i < x.size(); ++i)
	{
		mima[i*2] = x[i].mi;
		mima[i*2+1] = x[i].ma;
		doms[i] = x[i].dom;
	}
	return Goal(store,label(store,doms,selectVarMinDom(store,doms)) );
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
						isSupported = isSupported and it2->contains(supports[c2]);
						//solver.post(it2->contains(supports[c2]));
				}
				ReplaceInObj<Obj,MetaVar<int>,Var<int> > rr;
				if (ChkExpr(solver,rr(in,m)).canBeTrue())
				{
					solver.post(rr(in,m));
					//isSupported = isSupported and rr(in,m);
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
						isSupported = isSupported and it2->contains(supports[c2]);
						//solver.post(it2->contains(supports[c2]));
				}
				ReplaceInObj<Obj,MetaVar<int>,Var<int> > rr;
				if (ChkExpr(solver,rr(in,m)).canBeTrue())
				{
					std::cout << rr(in,m) << std::endl;
					solver.post(rr(in,m));
					//isSupported = isSupported and rr(in,m);
					auto it2 = intInMetaVarList.begin();
					for (uint c3 = 0; c3 < c; ++c3)
						++it2;
					ret = ret and (it->contains(intVar.domain().value())>=(it2->contains(intVar.domain().value()) and isSupported));
					//ret = ret and ((it2->contains(intVar.domain().value()) and
					//				isSupported)<=it->contains(intVar.domain().value()));
					labeling.insert(labeling.end(),supports.begin(),supports.end());
				}
				else
					ret = ret and (!it->contains(intVar.domain().value()));
				found1 = solver1.next();
			}
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

#if 0
void golomb()
{
	const int nMarks = 4;
	const int maxRulerSize = 6;

	const int nDiffVars = nMarks*(nMarks-1)/2;

	Util::StdRange<int> universe(0,maxRulerSize);
	Solver solver;
	Goal labeling(succeed());

	MetaVarArray<int> xi(solver,nMarks,universe);
	MetaVarArray<int> xo(solver,nMarks,universe);
	MetaVarArray<int> xo2(solver,nMarks,universe);
	MetaVarArray<int> ddo(solver,nDiffVars,universe);

	Goal supLabeling(succeed());
	bool found = true;

	// Good is entailed

	auto cGood = rel<Greater>(ddo[nDiffVars-2],ddo[nDiffVars-1]);
	found &= solver.post(MetaFilterDD().entailed(solver,cGood));

	// Propagating Good is effective

	auto cGoodIn = rel<Greater>(ddo2[nDiffVars-2],ddo2[nDiffVars-1]);
	found &= solver.post(MetaFilterBB()(solver,xo,xo2,supLabeling));
	found &= solver.post(strictlyContains(solver,xo,xo2));

	// Problem model

	auto c1i = rel<Distinct>(xi);
	auto c1o = rel<Distinct>(xo);

	found &= solver.post(MetaFilterBB()(solver,c1i,c1o,supLabeling));

	auto c2o = rel<Distinct>(ddo);

	found &= solver.post(MetaFilterBB()(solver,c2o,supLabeling));

	uint cc = 0;
	for (uint i = 0; i < nMarks; ++i)
		for (uint j = i+1; j < nMarks; ++j)
		{
			auto co = rel<Equal>(rel<Sub>(xo[j],xo[i]),ddo[cc]);
			found &= solver.post(MetaFilterBB()(solver,co,supLabeling));
			++cc;
		}

	for (uint i = 1; i < nMarks; ++i)
	{
		auto ci = rel<Less>(xi[i-1],xi[i]);
		auto co = rel<Less>(xo[i-1],xo[i]);
		found &= solver.post(MetaFilterBB()(solver,ci,co,supLabeling));
	}

	std::cout << "solving\n";
	found &= solver.solve(label(solver,xi) and label(solver,xo) and
							label(solver,xo2) and
							label(solver,ddo) and
							succeeds(solver,supLabeling));
	while (found)
	{
		//std::cout << c1 << " " << c21 << " " << c22 << std::endl;
		std::cout << xi << " --> " << xo << std::endl;
		std::cout << xi << " [/\\Good] --> " << xo2 << std::endl;
		//std::cout << " >> " << ddo << std::endl;
		//std::cout << xi << " " << yi <<  " " << zi << " [" << xysup << "||" << zisup << "]" << std::endl;
		found = solver.next();
	}
	std::cout << solver.getStats() << std::endl;

}
#endif

void test()
{
	Util::StdRange<int> universe(0,1);
	Solver solver;
	Util::StdList<Var<int> > labeling;

	MetaVarArray<int> xi(solver,3,universe);
	MetaVarArray<int> xoT(solver,3,universe);
//	MetaVarArray<int> xoL(solver,3,universe);

	bool found = true;

	auto ciT = rel<Equal>(rel<Mul>(xi[0],xi[1]),xi[2]); /* and
				rel<Distinct>(xi);*/

	auto coT = rel<Equal>(rel<Mul>(xoT[0],xoT[1]),xoT[2]);/* and
			  rel<Distinct>(xoT);*/

	found &= solver.post(MetaFilterDD()(solver,ciT,coT,labeling));

/*	auto ci1 = rel<Equal>(rel<Mul>(xi[0],xi[1]),xi[2]);
	auto ci2 = rel<Distinct>(xi);

	auto co1 = rel<Equal>(rel<Mul>(xoL[0],xoL[1]),xoL[2]);
	auto co2 = rel<Distinct>(xoL);

	found &= solver.post(MetaFilterDD()(solver,ci1,co1,labeling));
	found &= solver.post(MetaFilterDD()(solver,ci2,co2,labeling));

	found &= solver.post(strictlyRangeContains(solver,xoL,xoT));
*/
	std::cout << "solving\n";
	MetaVarArray<int> toLabel(solver,6,universe);
	for (uint i = 0; i < 3; ++i)
	{
		toLabel[i*2] = xi[i];
		toLabel[i*2+1] = xoT[i];
//		toLabel[i*3+2] = xoL[i];
	}

	found &= solver.solve(label(solver,toLabel) and
						succeeds(solver,label(solver,labeling)));
	while (found)
	{
		//std::cout << xi << " " << xoT << " " << xoL << std::endl;
		std::cout << xi << " " << xoT << std::endl;

		found = solver.next();
	}
	std::cout << solver.getStats() << std::endl;
}

int main()
{
	test();
	//golomb();
	return 0;
}
