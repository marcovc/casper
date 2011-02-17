 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_BNDFILTER
#define _H_CASPER_KERNEL_BNDFILTER

#include <casper/kernel/filter/common.h>
#include <casper/kernel/filter/basic.h>
#include <casper/kernel/view/bndview.h>
#include <casper/kernel/view/arrayview.h>
#include <casper/kernel/reversible.h>
#include <casper/kernel/safeop.h>
//#include <casper/kernel/expression.h>
#include <casper/kernel/relation.h>
#include <casper/kernel/assertions.h>

namespace casper {

template<class F,class,class V1> // TODO: default is to use FilterExpr1
struct BndFilterView1 : NoFilter
{	BndFilterView1(ICPSolver& s,const V1&) : NoFilter(s,*this) {}	};

template<class,class,class V1,class,class V2>
struct BndFilterView2 : NoFilter
{	BndFilterView2(ICPSolver& s,const V1&,const V2&) : NoFilter(s,*this) {}	};

template<class R,class E1,class V1,class E2,class V2,class E3,class V3>
struct BndFilterView3 : NoFilter
{	BndFilterView3(ICPSolver& s,const V1&,const V2&,const V3&) : NoFilter(s,*this) {}	};

template<class R,class E1,class V1,class E2,class V2,class E3,class V3,class E4,class V4>
struct BndFilterView4 : NoFilter
{	BndFilterView4(ICPSolver& s,const V1&,const V2&,const V3&,const V4&) : NoFilter(s,*this) {}	};

template<class R,class E1,class V1,class E2,class V2,class E3,class V3,class E4,class V4,class E5,class V5>
struct BndFilterView5 : NoFilter
{	BndFilterView5(ICPSolver& s,const V1&,const V2&,const V3&,const V4&,const V5&) : NoFilter(s,*this) {}	};

template<class Func,class Eval1,class View1>
struct BndFilterFact1
{
	static Filter create(ICPSolver& s,const View1& v1)
	{	return new (s.heap()) BndFilterView1<Func,Eval1,View1>(s,v1);}
};

template<class Func,class Eval1,class View1,class Eval2,class View2>
struct BndFilterFact2
{
	static Filter create(ICPSolver& s,const View1& v1,const View2& v2)
	{	return new (s.heap())
			BndFilterView2<Func,Eval1,View1,Eval2,View2>(s,v1,v2);}
};

template<class Func,class Eval1,class View1,class Eval2,class View2,
		 class Eval3,class View3>
struct BndFilterFact3
{
	static Filter create(ICPSolver& s,const View1& v1,const View2& v2,
						 const View3& v3)
	{	return new (s.heap())
			BndFilterView3<Func,Eval1,View1,Eval2,View2,Eval3,View3>(s,v1,v2,v3);}
};

template<class Func,class Eval1,class View1,class Eval2,class View2,
		 class Eval3,class View3,class Eval4,class View4>
struct BndFilterFact4
{
	static Filter create(ICPSolver& s,const View1& v1,const View2& v2,
						 const View3& v3,const View4& v4)
	{	return new (s.heap())
			BndFilterView4<Func,Eval1,View1,Eval2,View2,Eval3,View3,Eval4,View4>(s,v1,v2,v3,v4);}
};

template<class Func,class Eval1,class View1,class Eval2,class View2,
		 class Eval3,class View3,class Eval4,class View4,class Eval5,class View5>
struct BndFilterFact5
{
	static Filter create(ICPSolver& s,const View1& v1,const View2& v2,
						 const View3& v3,const View4& v4,const View5& v5)
	{	return new (s.heap())
			BndFilterView5<Func,Eval1,View1,Eval2,View2,Eval3,View3,Eval4,View4,Eval5,View5>(s,v1,v2,v3,v4,v5);}
};

template<class Rel>
struct BndFilterView; // default is to complain at compile time

// directional bounds arc consistency

template<class,class,class> // TODO: default is to use FilterExpr1
struct BndD1FilterView1;

template<class,class,class,class,class>
struct BndD1FilterView2; // TODO: default is to use FilterExpr2

template<class Func,class Eval1,class View1>
struct BndD1FilterFact1
{
	static Filter create(ICPSolver& s,const View1& v1)
	{	return new (s.heap()) BndD1FilterView1<Func,Eval1,View1>(s,v1);}
};

template<class Func,class Eval1,class View1,class Eval2,class View2>
struct BndD1FilterFact2
{
	static Filter create(ICPSolver& s,const View1& v1,const View2& v2)
	{	return new (s.heap())
			BndD1FilterView2<Func,Eval1,View1,Eval2,View2>(s,v1,v2);}
};

template<class Rel>
struct BndD1FilterView; // default is to complain at compile time

// happens for example when the negation for a relation is not defined
template<>
struct BndFilterView<Rel0<UnknownRel> > : NoFilter
{
	BndFilterView(ICPSolver& s,const Rel0<UnknownRel>&) : NoFilter(s,*this) {}
};

template<class F,class T1>
struct BndFilterView<Rel1<F,T1> > :
	public BndFilterView1<F,typename EvalTraits<T1>::Eval,T1>
{
	BndFilterView(ICPSolver& s,const Rel1<F,T1>& r) :
		BndFilterView1<F,typename EvalTraits<T1>::Eval,T1>(s,r.p1) {}
};

template<class F,class T1,class T2>
struct BndFilterView<Rel2<F,T1,T2> > :
	public BndFilterView2<F,typename EvalTraits<T1>::Eval,T1,
						 typename EvalTraits<T2>::Eval,T2>
{
	BndFilterView(ICPSolver& s,const Rel2<F,T1,T2>& r) :
		BndFilterView2<F,typename EvalTraits<T1>::Eval,T1,
					  typename EvalTraits<T2>::Eval,T2>(s,r.p1,r.p2) {}
};

template<class F,class T1,class T2,class T3>
struct BndFilterView<Rel3<F,T1,T2,T3> > :
	public BndFilterView3<F,typename EvalTraits<T1>::Eval,T1,
						 typename EvalTraits<T2>::Eval,T2,
						 typename EvalTraits<T3>::Eval,T3>
{
	BndFilterView(ICPSolver& s,const Rel3<F,T1,T2,T3>& r) :
		BndFilterView3<F,typename EvalTraits<T1>::Eval,T1,
					  typename EvalTraits<T2>::Eval,T2,
					  typename EvalTraits<T3>::Eval,T3>(s,r.p1,r.p2,r.p3) {}
};

template<class F,class T1,class T2,class T3,class T4>
struct BndFilterView<Rel4<F,T1,T2,T3,T4> > :
	public BndFilterView4<F,typename EvalTraits<T1>::Eval,T1,
						 	typename EvalTraits<T2>::Eval,T2,
						 	typename EvalTraits<T3>::Eval,T3,
						 	typename EvalTraits<T4>::Eval,T4>
{
	BndFilterView(ICPSolver& s,const Rel4<F,T1,T2,T3,T4>& r) :
		BndFilterView4<F,typename EvalTraits<T1>::Eval,T1,
					  	 typename EvalTraits<T2>::Eval,T2,
					  	 typename EvalTraits<T3>::Eval,T3,
					  	 typename EvalTraits<T4>::Eval,T4>(s,r.p1,r.p2,r.p3,r.p4) {}
};

template<class F,class T1,class T2,class T3,class T4,class T5>
struct BndFilterView<Rel5<F,T1,T2,T3,T4,T5> > :
	public BndFilterView5<F,typename EvalTraits<T1>::Eval,T1,
						 	typename EvalTraits<T2>::Eval,T2,
						 	typename EvalTraits<T3>::Eval,T3,
						 	typename EvalTraits<T4>::Eval,T4,
						 	typename EvalTraits<T5>::Eval,T5>
{
	BndFilterView(ICPSolver& s,const Rel5<F,T1,T2,T3,T4,T5>& r) :
		BndFilterView5<F,typename EvalTraits<T1>::Eval,T1,
					  	 typename EvalTraits<T2>::Eval,T2,
					  	 typename EvalTraits<T3>::Eval,T3,
					  	 typename EvalTraits<T4>::Eval,T4,
					  	 typename EvalTraits<T5>::Eval,T5>(s,r.p1,r.p2,r.p3,r.p4,r.p5) {}
};

template<class,class> class DomVar;
template<class,class> struct DomExpr;

struct BndFilterFact
{
	Filter operator()(ICPSolver& s,const Bool& v) const
	{	return BndFilterFact()(BndExpr<Bool>(s,v));	}

	template<class Dom>
	Filter operator()(DomVar<Bool,Dom> v) const
	{	return BndFilterFact()(BndExpr<Bool>(v.solver(),v));	}

	template<class Dom>
	Filter operator()(DomExpr<Bool,Dom> v) const
	{	return BndFilterFact()(BndExpr<Bool>(v.solver(),v));	}
	Filter operator()(BndExpr<Bool> v) const;
	Filter operator()(Filter f) const;

	template<class Func,class View1>
	Filter operator()(const Rel1<Func,View1>& v) const
	{
		typedef typename EvalTraits<View1>::Eval	Eval1;
		typedef BndFilterFact1<Func,Eval1,View1>	Fact;
		return Fact::create(getCPSolver(v),v.p1);
	}

	template<class Func,class View1,class View2>
	Filter operator()(const Rel2<Func,View1,View2>& v) const
	{
		typedef typename EvalTraits<View1>::Eval	Eval1;
		typedef typename EvalTraits<View2>::Eval	Eval2;
		typedef BndFilterFact2<Func,Eval1,View1,Eval2,View2>	Fact;
		return Fact::create(getCPSolver(v),v.p1,v.p2);
	}

	template<class Func,class View1,class View2,class View3>
	Filter operator()(const Rel3<Func,View1,View2,View3>& v) const
	{
		typedef typename EvalTraits<View1>::Eval	Eval1;
		typedef typename EvalTraits<View2>::Eval	Eval2;
		typedef typename EvalTraits<View3>::Eval	Eval3;
		typedef BndFilterFact3<Func,Eval1,View1,Eval2,View2,Eval3,View3>	Fact;
		return Fact::create(getCPSolver(v),v.p1,v.p2,v.p3);
	}

	template<class Func,class View1,class View2,class View3,class View4>
	Filter operator()(const Rel4<Func,View1,View2,View3,View4>& v) const
	{
		typedef typename EvalTraits<View1>::Eval	Eval1;
		typedef typename EvalTraits<View2>::Eval	Eval2;
		typedef typename EvalTraits<View3>::Eval	Eval3;
		typedef typename EvalTraits<View4>::Eval	Eval4;
		typedef BndFilterFact4<Func,Eval1,View1,Eval2,View2,Eval3,View3,Eval4,View4>	Fact;
		return Fact::create(getCPSolver(v),v.p1,v.p2,v.p3,v.p4);
	}

};

extern BndFilterFact Bnd;

struct BndD1FilterFact
{
	template<class Func,class View1>
	Filter operator()(const Rel1<Func,View1>& v) const
	{
		typedef typename EvalTraits<View1>::Eval	Eval1;
		typedef BndD1FilterFact1<Func,Eval1,View1>	Fact;
		return Fact::create(getCPSolver(v),v.p1);
	}

	template<class Func,class View1,class View2>
	Filter operator()(const Rel2<Func,View1,View2>& v) const
	{
		typedef typename EvalTraits<View1>::Eval	Eval1;
		typedef typename EvalTraits<View2>::Eval	Eval2;
		typedef BndD1FilterFact2<Func,Eval1,View1,Eval2,View2>	Fact;
		return Fact::create(getCPSolver(v),v.p1,v.p2);
	}

};

extern BndD1FilterFact BndD1;

/**
 * Enforces the conjunction of two constraints. It succeeds if both
 * constraints are satisfiable, fails otherwise.
 */
template<class View1,class View2>
struct BndFilterView2<And,Bool,View1,Bool,View2> : IFilter
{
	BndFilterView2(ICPSolver& s,const View1& p1, const View2& p2) :
		IFilter(s),p1(p1),p2(p2) {}

	bool execute()
	{	return solver().post(Bnd(p1)) and solver().post(Bnd(p2));	}

	void attach(INotifiable*) {}
	void detach(INotifiable*) {}

	View1	p1;
	View2	p2;
};


/**
 * Enforces the conjunction of n constraints. It succeeds if all
 * constraints are satisfiable, fails otherwise.
 */
template<class View1>
struct BndFilterView1<And,Seq<Bool>,View1> : IFilter
{
	BndFilterView1(ICPSolver& s,const View1& p1) :
		IFilter(s),p1(s,p1) {}

	bool execute()
	{
		for (UInt i = 0; i < p1.size(); ++i)
			if (!solver.post(Bnd(p1[i])))
				return false;
		return true;
	}

	void attach(INotifiable*) {}
	void detach(INotifiable*) {}

	BndArrayView<Bool,View1>	p1;
};

/**
 * Enforces the disjunction of two constraints. It succeeds if at
 * least one of them is satisfiable, fails otherwise.
 */
template<class View1,class View2>
struct BndFilterView2<Or,Bool,View1,Bool,View2> : IFilter
{
	BndFilterView2(ICPSolver& s,const View1& p1, const View2& p2) :
		IFilter(s),c1(s,p1),c2(s,p2) {}

	bool execute()
	{
		if (c1.isTrue() or c2.isTrue())
			return true;
		if (!c1.canBeTrue())
			return c2.setToTrue();
		if (!c2.canBeTrue())
			return c1.setToTrue();
		return true;
	}
	void attach(INotifiable* s)
	{	c1.attach(s); c2.attach(s);}
	void detach(INotifiable* s)
	{	c1.detach(s); c2.detach(s);}

	CChkView<View1>	c1;
	CChkView<View2>	c2;
};


/**
 * Enforces the exclusive disjunction of two constraints. It succeeds if
 * exactly one of them is satisfiable, fails otherwise.
 */
template<class View1,class View2>
struct BndFilterView2<XOr,Bool,View1,Bool,View2> :
	BndFilterView2<Distinct,Bool,View1,Bool,View2>
{
	BndFilterView2(ICPSolver& s,const View1& p1, const View2& p2) :
		BndFilterView2<Distinct,Bool,View1,Bool,View2>(s,p1,p2) {}
};

///	Enforces equality
template<class Eval,class View1,class View2>
struct BndFilterView2<Equal,Eval,View1,Eval,View2> : IFilter
{
	BndFilterView2(ICPSolver& solver,const View1& p1,const View2& p2)
		: IFilter(solver),p1(solver,p1), p2(solver,p2)
		{}

	bool execute()
	{	return p1.updateMin(p2.min()) and p1.updateMax(p2.max()) and
			   p2.updateMin(p1.min()) and p2.updateMax(p1.max());}
	void attach(INotifiable* s)
	{	/*if (!p1.ground() and !p2.ground())*/ {p1.attach(s); p2.attach(s);}	}
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	BndView<Eval,View1>	p1;
	BndView<Eval,View2>	p2;
};

///	Enforces the GreaterEqual binary relation between two views.
template<class Eval,class View1,class View2>
struct BndFilterView2<GreaterEqual,Eval,View1,Eval,View2> : IFilter
{
	BndFilterView2(ICPSolver& solver, const View1& p1,const View2& p2) :
		IFilter(solver),p1(solver,p1), p2(solver,p2) {}

	bool execute()
	{	return p1.updateMin(p2.min()) && p2.updateMax(p1.max());	}
	void attach(INotifiable* s)
	{	/*if (!p1.ground() and !p2.ground())*/ { p1.attach(s); p2.attach(s);} }
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	BndView<Eval,View1> p1;
	BndView<Eval,View2> p2;
};

///	Enforces the LessEqual binary relation between two views.
template<class Eval1,class View1,class Eval2,class View2>
struct BndFilterView2<LessEqual,Eval1,View1,Eval2,View2> :
	BndFilterView2<GreaterEqual,Eval2,View2,Eval1,View1>
{
	BndFilterView2(ICPSolver& solver,const View1& p1, const View2& p2) :
		BndFilterView2<GreaterEqual,Eval2,View2,Eval1,View1>(solver,p2,p1) {}
};

///	Enforces the Greater binary relation between two views.
template<class Eval,class View1,class View2>
struct BndFilterView2<Greater,Eval,View1,Eval,View2> : IFilter
{
	BndFilterView2(ICPSolver& solver, const View1& p1,const View2& p2) :
		IFilter(solver),p1(solver,p1), p2(solver,p2) {}

	bool execute()
	{
		return p1.updateMin(detail::succ(p2.min())) and
			   p2.updateMax(detail::pred(p1.max()));
	}
	void attach(INotifiable* s)
	{	/*if (!p1.ground() and !p2.ground())*/ {p1.attach(s); p2.attach(s);} }
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	BndView<Eval,View1> p1;
	BndView<Eval,View2> p2;
};


/**
	Filter that enforces bound consistenct on less constraint between
	two views.
*/
template<class Eval1,class Eval2,class View1,class View2>
struct BndFilterView2<Less,Eval1,View1,Eval2,View2> :
	BndFilterView2<Greater,Eval2,View2,Eval1,View1>
{
	BndFilterView2(ICPSolver& solver, const View1& p1, const View2& p2) :
		BndFilterView2<Greater,Eval2,View2,Eval1,View1>(solver,p2,p1) {}
};

/**
 *	BndView over a linear(coefs,vars) relation, where coefs are ground. From
 *  "Yuanlin,Yap, Arc-consistency on n-ary monotonic and linear constraints"
 *  TODO: improve using index deltas
 *  TODO: make sure it also works for reals
 */
template<class Eval,class View1,class View2,class View3>
struct BndFilterView3<Linear,Seq<Eval>,View1,Seq<Eval>,View2,Eval,View3> : IFilter
{
	BndFilterView3(ICPSolver& solver,const View1& v1, const View2& v2,
					const View3& v3) : IFilter(solver),
					c(solver,v1),x(solver,v2),v(solver,v3),
					f(solver,c[0].value()*x[0].getView())
	{
		for (UInt i = 1; i < c.size(); i++)
			f = f + c[i].value()*x[i].getView();
	}
	Bool execute()
	{
		Eval fmin = f.min();
		Eval fmax = f.max();
		if (!v.updateMin(fmin) or !v.updateMax(fmax))
			return false;
		fmin = fmin-v.max();
		fmax = fmax-v.min();

		for (UInt i = 0; i < c.size(); i++)
		{
			Eval lb = limits<Eval>::negInf();
			Eval ub = limits<Eval>::posInf();
			detail::mulRange(range(c[i].value(),c[i].value()),
							 range(x[i].min(),x[i].max()),lb,ub);
			Eval lb1 = x[i].min();
			Eval ub1 = x[i].max();
			detail::divRange(range(ub-fmax,lb-fmin),
							 range(c[i].value(),c[i].value()),lb1,ub1);
			Eval oldxmin = x[i].min();
			Eval oldxmax = x[i].max();
			if (oldxmin < lb1 or oldxmax > ub1)
			{
				if (!x[i].updateMin(lb1) or !x[i].updateMax(ub1))
					return false;
				fmin = f.min()-v.max();
				fmax = f.max()-v.min();
			}
		}
		return true;
	}
	Cost cost() const {	return linearLo; }
	void attach(INotifiable* s)
	{	x.attach(s); v.attach(s);	}
	void detach(INotifiable* s)
	{	x.detach(s); v.detach(s);	}

	ValArrayView<Eval,View1> c;
	BndArrayView<Eval,View2> x;
	BndView<Eval,View3>		 v;
	BndExpr<Eval>			 f;
};

#define GECODE_SUM
#ifdef GECODE_SUM
template<class Eval,class View1,class View2>
struct BndFilterView2<Sum,Seq<Eval>,View1,Eval,View2> : IFilter
{
	BndFilterView2(ICPSolver& solver,const View1& v1, const View2& v2) :
					IFilter(solver),
					x(solver,v1),v(solver,v2) {}

	Bool execute()
	{
		if (!v.ground())
			return true;

		setInQueue(1);
		// try to rewrite:
		UInt nground = 0;
		for (UInt i = 0; i < x.size(); ++i)
			if (x[i].min()==x[i].max())
				++nground;
		if (x.size()-nground == 3)
		{
			DomVarArray<Eval> newx(solver(),3);
			Eval s = 0;
			UInt c = 0;
			for (UInt i = 0; i < x.size(); ++i)
				if (x[i].min()!=x[i].max())
					newx[c++] = x[i].getView();
				else
					s += x[i].min();
			solver().post(newx[0]+newx[1]+newx[2]==v.value()-s);
			setInQueue(noQueue);
			detach(pOwner);
			return true;
		}
		else
		if (x.size()-nground == 2)
		{
			DomVarArray<Eval> newx(solver(),2);
			Eval s = 0;
			UInt c = 0;
			for (UInt i = 0; i < x.size(); ++i)
				if (x[i].min()!=x[i].max())
					newx[c++] = x[i].getView();
				else
					s += x[i].min();
			solver().post(newx[0]+newx[1]==v.value()-s);
			setInQueue(noQueue);
			detach(pOwner);
			return true;
		}
		else
		if (x.size()-nground == 1)
		{
			DomVarArray<Eval> newx(solver(),1);
			Eval s = 0;
			UInt c = 0;
			for (UInt i = 0; i < x.size(); ++i)
				if (x[i].min()!=x[i].max())
					newx[c++] = x[i].getView();
				else
					s += x[i].min();
			//solver().post(newx[0]==v.value()-s);
			setInQueue(noQueue);
			detach(pOwner);
			return newx[0].domain().updateRange(v.value()-s,v.value()-s);
		}

		Eval sl = 0;
		Eval su = 0;
		for (UInt i = 0; i < x.size(); ++i)
		{
			sl -= x[i].min();
			su -= x[i].max();
		}
		sl += v.value(); su += v.value();

		const int mod_sl = 1;
		const int mod_su = 2;

		int mod = mod_sl | mod_su;

		do
		{
			if (mod & mod_sl)
			{
				mod -= mod_sl;
				// Propagate upper bound
				for (UInt i = 0; i < x.size(); ++i)
				{
					const Eval xi_max = x[i].max();
					if (!x[i].updateMax(sl + x[i].min()))
					{
						setInQueue(noQueue);
						return false;
					}
					if (x[i].max()<xi_max)
					{
						su += xi_max - x[i].max();
						mod |= mod_su;
					}
				}
			}
			if (mod & mod_su)
			{
				mod -= mod_su;
				// Propagate lower bound
				for (UInt i = 0; i < x.size(); ++i)
				{
					const Eval xi_min = x[i].min();
					if (!x[i].updateMin(su + x[i].max()))
					{
						setInQueue(noQueue);
						return false;
					}
					if (x[i].min() > xi_min)
					{
						sl += xi_min - x[i].min();
						mod |= mod_sl;
					}
				}
			}
		} while (mod);
		if (sl == su)
		{
			setInQueue(IFilter::noQueue);
			detach(pOwner);
			return true;
		}
		setInQueue(IFilter::noQueue);
		return true;
	}
	Cost cost() const {	return linearLo; }
	void attach(INotifiable* s)
	{
		pOwner = s;
		x.attach(s);
		v.attach(s);
	}
	void detach(INotifiable* s)
	{
		x.detach(s);
		v.detach(s);
	}

	BndArrayView<Eval,View1> x;
	ValView<Eval,View2>		 v;
	INotifiable*			 pOwner;
};
#else

template<class Eval,class View1,class View2>
struct BndFilterView2<Sum,Seq<Eval>,View1,Eval,View2> : IFilter
{
	BndFilterView2(ICPSolver& solver,const View1& v1, const View2& v2) :
					IFilter(solver),
					x(solver,v1),v(solver,v2) {}

	Bool execute()
	{
		// try to rewrite:
		UInt nground = 0;
		for (UInt i = 0; i < x.size(); ++i)
			if (x[i].min()==x[i].max())
				++nground;
		if (x.size()-nground == 3)
		{
			DomVarArray<Eval> newx(solver(),3);
			Eval s = 0;
			UInt c = 0;
			for (UInt i = 0; i < x.size(); ++i)
				if (x[i].min()!=x[i].max())
					newx[c++] = x[i].getView();
				else
					s += x[i].min();
			solver().post(newx[0]+newx[1]+newx[2]==v.value()-s);
			setInQueue(noQueue);
			detach(pOwner);
			return true;
		}
		else
		if (x.size()-nground == 2)
		{
			DomVarArray<Eval> newx(solver(),2);
			Eval s = 0;
			UInt c = 0;
			for (UInt i = 0; i < x.size(); ++i)
				if (x[i].min()!=x[i].max())
					newx[c++] = x[i].getView();
				else
					s += x[i].min();
			solver().post(newx[0]+newx[1]==v.value()-s);
			setInQueue(noQueue);
			detach(pOwner);
			return true;
		}
		else
		if (x.size()-nground == 1)
		{
			DomVarArray<Eval> newx(solver(),1);
			Eval s = 0;
			UInt c = 0;
			for (UInt i = 0; i < x.size(); ++i)
				if (x[i].min()!=x[i].max())
					newx[c++] = x[i].getView();
				else
					s += x[i].min();
			//solver().post(newx[0]==v.value()-s);
			setInQueue(noQueue);
			detach(pOwner);
			return newx[0].domain().updateRange(v.value()-s,v.value()-s);
		}

		Eval sl = 0;
		Eval su = 0;
		for (UInt i = 0; i < x.size(); ++i)
		{
			sl -= x[i].min();
			su -= x[i].max();
		}
		sl += v.value();
		su += v.value();

		// Propagate bounds
		for (UInt i = 0; i < x.size(); ++i)
		{
			const Eval oldMax = x[i].max();
			const Eval oldMin = x[i].min();
			if (!x[i].updateRange(su + oldMax,sl + oldMin))
				return false;
			const Eval newMax = x[i].max();
			const Eval newMin = x[i].min();
			if (newMax < oldMax)
				su += oldMax - newMax;
			if (newMin > oldMin)
				sl += oldMin - newMin;
			/*
			const Eval oldDiff = oldMax - oldMin;
			if (oldDiff > sl)
				su += oldDiff - sl;
			if (oldDiff > -su)
				sl += - su - oldDiff;*/
		}

	//	if (sl == su)
	//		detach(pOwner);
	//	setInQueue(IFilter::noQueue);
		return true;
	}
	Bool entailed() const {	return false; }
	Cost cost() const {	return linearLo; }
	void attach(INotifiable* s)
	{	pOwner = s; x.attach(s); v.attach(s);	}
	void detach(INotifiable* s)
	{	x.detach(s); v.detach(s);	}

	BndArrayView<Eval,View1> x;
	ValView<Eval,View2>		 v;
	INotifiable*			 pOwner;
};
#endif

/**
 *	BndView over a linear(vars) relation, where coefs are ground. From
 *  "Yuanlin,Yap, Arc-consistency on n-ary monotonic and linear constraints"
 *  TODO: improve using index deltas
 *  TODO: make sure it also works for reals
 */ /*
template<class Eval,class View2,class View3>
struct BndFilterView2<Linear,Seq<Eval>,View2,Eval,View3> : IFilter
{
	BndFilterView2(ICPSolver& solver,const View2& v2,const View3& v3) : IFilter(solver),
					x(solver,v2),v(solver,v3),
					f(solver,x[0])
	{
		for (UInt i = 1; i < c.size(); i++)
			f = f + x[i];
	}
	Bool execute()
	{
		Eval fmin = f.min();
		Eval fmax = f.max();
		if (!v.updateMin(fmin) or !v.updateMax(fmax))
			return false;
		fmin = fmin-v.max();
		fmax = fmax-v.min();

		for (UInt i = 0; i < x.size(); i++)
		{
			Eval lb = x[i].min();
			Eval ub = x[i].max();

			Eval lb1 = ub-fmax;
			Eval ub1 = lb-fmin;

			Eval oldxmin = x[i].min();
			Eval oldxmax = x[i].max();
			if (x[i].min() < x[i].max()-fmax or x[i].max() > x[i].min()-fmin)
			{
				if (!x[i].updateMin(lb1) or !x[i].updateMax(ub1))
					return false;
				fmin = f.min()-v.max();
				fmax = f.max()-v.min();
			}
		}
		return true;
	}
	Bool entailed() const {	assert(0);	return false; }
	Cost cost() const {	return linearLo; }
	void attach(INotifiable* s)
	{	x.attach(s); v.attach(s);	}
	void detach(INotifiable* s)
	{	x.detach(s); v.detach(s);	}

	ValArrayView<Eval,View1> c;
	BndArrayView<Eval,View2> x;
	BndView<Eval,View3>		 v;
	BndExpr<Eval>			 f;
};*/

// BndFilterView over a Filter
template<>
struct BndFilterView<Filter> : IFilter
{
	BndFilterView(ICPSolver& s,Filter f) : IFilter(s),pFilter(f.getPImpl()) {}

	bool execute()
	{	return pFilter->execute();	}
	Cost cost() const { return pFilter->cost(); }

	void attach(INotifiable* s)
	{	pFilter->attach(s); }
	void detach(INotifiable* s)
	{	pFilter->detach(s); }
	IFilter*	pFilter;
};

// BndFilterView over a BndExpr<Bool>
template<>
struct BndFilterView<BndExpr<Bool> > : IFilter
{
	BndFilterView(ICPSolver& s,BndExpr<Bool> e) : IFilter(s),rExpr(e.getImpl()) {}

	bool execute()
	{	return rExpr.updateMin(true);	}
	bool entailed() const
	{	return rExpr.min() == 1;	}
	Filter	operator!();

	void attach(INotifiable* s)
	{	rExpr.attach(s); }
	void detach(INotifiable* s)
	{	rExpr.detach(s); }
	IBndExpr<Bool>&	rExpr;
};


#if 0
/**
	Bound expression from filter (reification)
*/
template<class View>
struct BndReify : IBndExpr<Bool>
{
	private:
	struct EntailDemon : INotifiable
	{
		BndReify&	rOwner;
		IFilter&	f;
		Bool		val;
		EntailDemon(BndReify* pOwner,IFilter& f,Bool val) :
			rOwner(*pOwner),f(f),val(val)
		{	f.attach(this);	}
		bool notify()
		{
			if (rOwner.isTrue != 2)
				return true;
			if (f.entailed())
			{
				rOwner.set(val);
				return rOwner.pOwner->notify();
			}
			return true;
		}
	};
	public:
	BndReify(ICPSolver& solver, const View& p1) :
						IBndExpr<Bool>(solver),
						v1(p1),
						pos(p1),
						neg(!pos),
						isTrue(solver,2),
						pOwner(NULL) {}
	Bool min() const
	{	return  isTrue==1;	}
	Bool max() const
	{	return  isTrue!=0;	}
	Bool updateMin(const Bool& v)
	{	return !v || isTrue==1 || (isTrue==2 && postPositive());	}
	Bool updateMax(const Bool& v)
	{	return  v || isTrue==0 || (isTrue==2 && postNegative());	}
	void attach(INotifiable* f)
	{
		pOwner = f;
		new (solver().heap()) EntailDemon(this,pos.getImpl(),true);
		new (solver().heap()) EntailDemon(this,neg.getImpl(),false);
	}
	void detach(INotifiable* f)
	{ 	pos.detach(this); neg.detach(this);	}
	View  getView()  const { return v1;	}

	private:
	Void set(Bool val)
	{ isTrue=val; /*detach(pOwner);*/ }
	Bool postPositive()
	{ set(true); return solver().post(pos); }
	Bool postNegative()
	{ set(false); return solver().post(neg); }

	View	v1;
	Filter	pos;
	Filter	neg;
	Reversible<Int> isTrue;
	INotifiable* 	pOwner;
};
#else

/**
	\note Simplest implementation for reification
*/ /*
template<class View>
struct BndReify : IBndExpr<Bool>
{
	BndReify(ICPSolver& solver, const View& p1) :
						IBndExpr<Bool>(solver),
						v1(p1),
						pos(p1),
						neg(!pos) {}
	Bool min() const { return pos.entailed(); }
	Bool max() const { return !neg.entailed(); }
	Bool updateMin(const Bool& v)
	{ 	return !v or solver().post(pos);	}
	Bool updateMax(const Bool& v)
	{ return v or solver().post(neg); }
	void attach(INotifiable* f) { pos.attach(f); neg.attach(f); 	}
	void detach(INotifiable* f) { pos.detach(f); neg.detach(f); 	}
	View  getView()  const { return v1;	}
	View	v1;
	Filter	pos;
	Filter	neg;
};*/

/*
template<class View>
struct BndReify
{
	BndReify(ICPSolver& solver, const View& p1) :
						v1(p1),
						pos(p1),
						neg(!pos),
						pOwner(NULL),
						assigned(solver,2) {}

	ICPSolver& solver() const {	return assigned.solver();	}
	Bool min() const
	{
		if (assigned!=2)
			return assigned;
		return pos.entailed();
	}
	Bool max() const
	{
		if (assigned!=2)
			return assigned;
		return !neg.entailed();
	}
	Bool updateMin(const Bool& v)
	{
		if (!v or assigned==1) return true;
		if (assigned==0) return false;
		assigned = 1;
		if (pOwner != NULL) detach(pOwner);
		return solver().post(pos);
	}
	Bool updateMax(const Bool& v)
	{
		if (v or assigned==0) return true;
		if (assigned==1) return false;
		assigned = 0;
		if (pOwner != NULL) detach(pOwner);
		return solver().post(neg);
	}
	Void range(Bool& l, Bool& u) const
	{
		if (assigned!=2)
			l = u = assigned;
		else
		{
			l = pos.entailed();
			u = !neg.entailed();
		}
	}
	Bool updateRange(const Bool& l,const Bool& r)
	{	return updateMin(l) and updateMax(r);	}

	void attach(INotifiable* f)
	{	pOwner = f; pos.attach(f); neg.attach(f);}
	void detach(INotifiable* f)
	{	assert(f == pOwner); pos.detach(f); neg.detach(f);}
	View  getView()  const { return v1;	}
	View	v1;
	Filter	pos;
	Filter	neg;
	INotifiable*	pOwner;
	Reversible<Int>	assigned;
};*/
#endif

/*
template<class F,class View1,class Eval>
struct BndViewRel1 : BndReify<Rel1<F,View1> >
{
	typedef Rel1<F,View1> Rel;
	typedef BndReify<Rel> Super;
	BndViewRel1(ICPSolver& solver, const View1& p1) :
		Super(solver,Rel(p1)) {}
};

template<class F, class View1,class View2,class Eval>
struct BndViewRel2 : BndReify<Rel2<F,View1,View2> >
{
	typedef Rel2<F,View1,View2> Rel;
	typedef BndReify<Rel> Super;
	BndViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		Super(solver,Rel(p1,p2)) {}
};
*/

/**
	Bound expression from filter (reification)
*//*
template<class Eval>
struct BndView<Eval,Filter> : BndReify<Filter>
{
	typedef BndReify<Filter> Super;

	BndView(ICPSolver& solver, const Filter& v) :
						Super(solver,v) {}
};*/


// factory for Element: optimize when index is ground
template<class Eval,class ArrayView,class IdxView,class EvalView>
struct BndFilterFact3<Element,Seq<Eval>,ArrayView,Int,IdxView,Eval,EvalView>
{
    static Filter create( ICPSolver& s,const ArrayView& p1,const IdxView& p2,
    					  const EvalView& p3)
    {
    	if (ValView<Int,IdxView>(s,p2).ground())
    	{
    		DomArrayView<Eval,ArrayView> b(s,p1);
    		DomVar<Eval> aux(s,&(*b[ValView<Int,IdxView>(s,p2).value()]));
    		return aux==p3;
    	}
    	else
    		return new (s.heap())
    			BndFilterView3<Element,Seq<Eval>,ArrayView,
    									Int,IdxView,
    									Eval,EvalView>(s,p1,p2,p3);
    }

};

#if 0
template<bool Cond,class IfTrue,class IfFalse>
struct ITE
{	typedef IfTrue Eval;	};

template<class IfTrue,class IfFalse>
struct ITE<false,IfTrue,IfFalse>
{	typedef IfFalse Eval;	};

template<class View1,class View2,class Eval>
struct ElementOverGroundIdx : BndView<Eval,typename ElemTraits<View1>::Elem>
{
	typedef	BndView<Eval,typename ElemTraits<View1>::Elem>	Super;

	static Super getSuper(ICPSolver& s,const View1& p1,const View2& p2)
	{
		if (ValExpr<Int>(s,p2).ground())
		{
			UInt val = ValExpr<Int>(s,p2).value();
			typedef typename ElemTraits<View1>::Elem	SElem;
			IterationView<SElem,View1>	it(p1);
			for (;  it.valid() and val > 0; it.iterate())
				--val;
			return Super(s,it.value());
		}
	}
	typedef Rel2<Element,View1,View2>	Viewed;
	Viewed getView()  const {	return viewed;	}

	ElementOverGroundIdx(ICPSolver& solver,const View1& p1, const View2& p2) :
		Super(getSuper(solver,p1,p2)),
		viewed(p1,p2){}

	Viewed viewed;
};
#endif

// element as expression: may introduce one auxiliary variable
template<class View1,class View2,class Eval>
struct BndViewRel2<Element,View1,View2,Eval> :
	BndView<Eval,DomVar<Eval,typename DefaultDomType<Eval>::Type> >
{
	CASPER_ASSERT_BNDVIEW_EVAL(Eval)

	template<class E,class V> struct Helper	{
		V operator()(ICPSolver& s, const E& e)
		{ return V(s,e);}
	};
	template<class V> struct Helper<V,V>	{
		const V& operator()(ICPSolver& s,const V& e)
		{ return e;}
	};

	typedef DomVar<Eval,typename DefaultDomType<Eval>::Type> Elem;
	typedef BndView<Eval,Elem>	Super;
	static Super getSuper(ICPSolver& s,const View1& p1,const View2& p2)
	{
		if (ValView<Int,View2>(s,p2).ground())
		{
			UInt val = ValView<Int,View2>(s,p2).value();
			typedef typename ElemTraits<View1>::Elem EElem;
			IterationView<View1>	it(p1);
			for (;  it.valid() and val > 0; it.iterate())
				--val;
			return Super(s,Helper<EElem,Elem>()(s,it.value()));
		}

		Elem v(s,detail::VarDomCreator<typename Elem::Dom>().unionOf(s,p1));
		s.post(element(p1,p2,v));
		return Super(s,v);
	}
	// temp
#if 0
	typedef Rel2<Element,View1,View2>	Viewed;
	Viewed getView()  const
	{	return Viewed(viewed.p1,idx.getView());	}
#else
	typedef Elem	Viewed;
	Viewed getView() const
	{	return Super::getView(); }
#endif
	BndViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		Super(getSuper(solver,p1,p2)),
		viewed(viewed),idx(solver,p2)
	{}

	Viewed viewed;
	ValView<Int,View2> idx;
};

#if 0
// element as expression: introduces one auxiliary variable
template<class View1,class View2,class Eval>
struct BndViewRel2<Element,View1,View2,Eval> :
	ITE<ExprMatch<View2,expression::Ground>::value,
		ElementOverGroundIdx<View1,View2,Eval>,
		ElementOverNonGroundIdx<View1,View2,Eval> >::Eval
{
	typedef typename
		ITE<ExprMatch<View2,expression::Ground>::value,
			ElementOverGroundIdx<View1,View2,Eval>,
			ElementOverNonGroundIdx<View1,View2,Eval> >::Eval	Super;

	BndViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		Super(solver,p1,p2) {}
};

// element as expression: introduces one auxiliary variable
template<class View1,class View2,class Eval>
struct BndViewRel2<Element,View1,View2,Eval> :
	BndView<Eval,BndExpr<Eval> >
{
	typedef BndView<Eval,BndExpr<Eval> >	Super;

	static BndExpr<Eval> getBndExpr(ICPSolver& s,const View1& p1,const View2& p2)
	{
		if (ValExpr<Int>(s,p2).ground())
		{
			UInt val = ValExpr<Int>(s,p2).value();
			typedef typename ElemTraits<View1>::Elem	SElem;
			IterationView<SElem,View1>	it(p1);
			for (;  it.valid() and val > 0; it.iterate())
				--val;
			return BndExpr<Eval>(s,it.value());
		}
		else
		{
			typedef typename DomVar<Eval>::Dom	Dom;
			DomVar<Eval> v(s,detail::VarDomCreator<Dom>().unionOf(s,p1));
			s.post(element(p1,p2,v));
			return BndExpr<Eval>(s,v);
		}
	}

	typedef Rel2<Element,View1,View2>	Viewed;
	Viewed getView()  const {	return viewed;	}

	BndViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		Super(solver,getBndExpr(solver,p1,p2)),
		viewed(p1,p2){}

	Viewed viewed;
};
#endif

/*
template<class T1,class T2>
struct ComposedFilter : IFilter
{
	ComposedFilter(ICPSolver& solver, const T1& pos, const T2& neg) :
		IFilter(solver),pos(solver,pos),neg(neg) {}
	bool execute()
	{	return pos.execute();	}
	bool entailed() const
	{	return pos.entailed();	}
	Filter operator!() {	return Filter(neg);	}
	void attach(INotifiable* pOwner)	{	pos.attach(pOwner);	}
	void detach(INotifiable* pOwner)	{ 	pos.detach(pOwner);	}

	BndFilterView<T1> pos;
	T2 neg;
};

template<class T1,class T2>
Filter compose(const T1& t1, const T2& t2)
{	return new (getCPSolver(t1).heap()) ComposedFilter<T1,T2>(getCPSolver(t1),t1,t2);}
*/

#if 1
struct FastCache;

template<class Eval,class View>
struct BndViewRel1<FastCache,View,Eval>
{
	BndViewRel1(ICPSolver& solver,const View& p) : v(solver,p),
		cachedMin(solver,limits<Eval>::min()),
		cachedMax(solver,limits<Eval>::max())
		{}

	BndViewRel1(ICPSolver& solver,const View& p,const Eval& mi, const Eval& ma) : v(solver,p),
		cachedMin(solver,mi),cachedMax(solver,ma)
		{}

	Eval min() const
	{
		//return cachedMin;
		return std::max((Eval)cachedMin,v.min());
	}
	Eval max() const
	{
		//return cachedMax;
		return std::min((Eval)cachedMax,v.max());
	}
	Void range(Eval& min, Eval& max) const
	{
		//min = cachedMin;
		//max = cachedMax;
		min = std::max((Eval)cachedMin,v.min());
		max = std::min((Eval)cachedMax,v.max());
	}

	Bool updateMin(const Eval& m)
	{
		if (m>cachedMin)
		{
			cachedMin = m;
			return m <= cachedMax and v.updateMin(cachedMin);
		}
		return true;
	}
	Bool updateMax(const Eval& m)
	{
		if (m<cachedMax)
		{
			cachedMax = m;
			return m >= cachedMin and v.updateMax(cachedMax);
		}
		return true;
	}

	Bool updateRange(const Eval& lb, const Eval& ub)
	{
		Bool ch = false;
		if (lb>cachedMin)
		{
			cachedMin = lb;
			ch = true;
		}
		if (ub<cachedMax)
		{
			cachedMax = ub;
			ch = true;
		}
		return cachedMin<=cachedMax and
				(!ch or v.updateRange(cachedMin,cachedMax));
	}

	void attach(INotifiable* f)
	{
		v.attach(f);
		cachedMin = std::max((Eval)cachedMin,v.min());
		cachedMax = std::min((Eval)cachedMax,v.max());
	}
	void detach(INotifiable* f) {	v.detach(f);	}

	View getView()  const
	{ 	return v.getView();	}

	BndView<Eval,View>			v;
	Reversible<Eval>			cachedMin;
	Reversible<Eval>			cachedMax;
};
#endif

template<class Eval,class View>
struct BndViewRel1<Cache,View,Eval>
{
	BndViewRel1(ICPSolver& solver,const View& p) : v(solver,p),
		cachedMin(solver,limits<Eval>::min()),
		cachedMax(solver,limits<Eval>::max()),
		syncRange(new SyncRange(solver,this))
		{}

	BndViewRel1(ICPSolver& solver,const View& p,const Eval& mi, const Eval& ma) : v(solver,p),
		cachedMin(solver,mi),cachedMax(solver,ma),
		syncRange(new SyncRange(solver,this))
		{}

	struct SyncRange : IFilter
	{
		SyncRange(ICPSolver& solver,BndViewRel1* pOwner) : IFilter(solver),rOwner(*pOwner) {}
		Bool execute()
		{	return rOwner.v.updateRange(rOwner.cachedMin,rOwner.cachedMax) and rOwner.pParent->notify();	}
		Cost cost() const { return constantLo;}
		void attach(INotifiable* f) { rOwner.v.attach(f); }
		void detach(INotifiable* f) { rOwner.v.detach(f); }
		BndViewRel1& rOwner;
	};

	Eval min() const
	{
		//return cachedMin;
		return std::max((Eval)cachedMin,v.min());
	}
	Eval max() const
	{
		//return cachedMax;
		return std::min((Eval)cachedMax,v.max());
	}
	Void range(Eval& min, Eval& max) const
	{
		//min = cachedMin;
		//max = cachedMax;
		min = std::max((Eval)cachedMin,v.min());
		max = std::min((Eval)cachedMax,v.max());
	}

	Bool updateMin(const Eval& m)
	{
		if (m>cachedMin)
		{
			cachedMin = m;
			return m <= cachedMax and v.updateMin(cachedMin) and pParent->notify();
		}
		return true;
	}
	Bool updateMax(const Eval& m)
	{
		if (m<cachedMax)
		{
			cachedMax = m;
			return m >= cachedMin and v.updateMax(cachedMax) and pParent->notify();
		}
		return true;
	}

	Bool updateRange(const Eval& lb, const Eval& ub)
	{
		Bool ch = false;
		if (lb>cachedMin)
		{
			cachedMin = lb;
			ch = true;
		}
		if (ub<cachedMax)
		{
			cachedMax = ub;
			ch = true;
		}
		return cachedMin<=cachedMax and
				(!ch or
				 (v.updateRange(cachedMin,cachedMax) and pParent->notify()));
	}

	void attach(INotifiable* f)
	{
		pParent = f;
		cachedMin.solver().post(syncRange);
		cachedMin = std::max((Eval)cachedMin,v.min());
		cachedMax = std::min((Eval)cachedMax,v.max());
	}
	void detach(INotifiable* f) {	v.detach(f);	}

	View getView()  const
	{ 	return v.getView();	}

	BndView<Eval,View>			v;
	Reversible<Eval>			cachedMin;
	Reversible<Eval>			cachedMax;
	Filter						syncRange;
	INotifiable*				pParent;
};

template<class Eval,class View>
struct BndViewRel3<Cache,View,Eval,Eval,Eval> :
			BndViewRel1<Cache,View,Eval>
{
	typedef BndViewRel1<Cache,View,Eval> Super;
	BndViewRel3(ICPSolver& solver,const View& p,const Eval& mi, const Eval& ma) :
			Super(solver,p,mi,ma) {}
};

};

#endif /*_H_CASPER_KERNEL_BNDFILTER */
