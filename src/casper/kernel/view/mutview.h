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

#ifndef _H_CASPER_KERNEL_VIEW_MUTVIEW
#define _H_CASPER_KERNEL_VIEW_MUTVIEW

#include <casper/kernel/expression.h>

namespace casper {


/********************
 * Mutable View
 *********************/


template<class>	class MutVar;
template<class,class,class>			struct MutViewRel1;
template<class,class,class,class>	struct MutViewRel2;
template<class,class,class,class,class>	struct MutViewRel3;

/**
 * 	MutView over generic object. View must be convertible to Eval.
 * 	\ingroup Views
 **/
template<class Eval,class View>
struct MutView : IMutExpr<Eval>
{
	MutView(ICPSolver& solver, const View& p1) :
		IMutExpr<Eval>(solver),p1(p1) {}
	Eval value() const { return p1; }

	Void attach(INotifiable*) {}
	Void detach(INotifiable*) {}

	const View& getView()  const { return p1; }

	const View	p1;
};

/**
 * 	MutView over a MutVar.
 * 	\ingroup Views
 **/
template<class Eval,class T>
struct MutView<Eval,MutVar<T> > : IMutExpr<Eval>
{
	MutView(ICPSolver& solver,const MutVar<T>& p1) :
		IMutExpr<Eval>(solver),p1(p1) {}
	Eval value() const { return p1.value(); }
	Void attach(INotifiable* f) { p1.attachOnUpdate(f); }
	Void detach(INotifiable* f) { p1.detachOnUpdate(f); }
	const MutVar<T>& getView()  const { return p1; }
	MutVar<T>	p1;
};

/**
 * 	MutView over a Rel1 relation -> defer to MutViewRel1.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View>
struct MutView<Eval,Rel1<F,View> > : MutViewRel1<F,View,Eval>
{
	using MutViewRel1<F,View,Eval>::attach;
	using MutViewRel1<F,View,Eval>::detach;

	MutView(ICPSolver& solver, const Rel1<F,View>& r) :
		MutViewRel1<F,View,Eval>(solver,r.p1) {}
	// not sure if the below constructor is ever used
	MutView(ICPSolver& solver,const MutViewRel1<F,View,Eval>& v) :
		MutViewRel1<F,View,Eval>(v) {}
};

/**
 * 	MutView over a Rel2 relation -> defer to MutViewRel2.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View1,class View2>
struct MutView<Eval,Rel2<F,View1,View2> > :
	MutViewRel2<F,View1,View2,Eval>
{
	using MutViewRel2<F,View1,View2,Eval>::attach;
	using MutViewRel2<F,View1,View2,Eval>::detach;

	MutView(ICPSolver& solver, const Rel2<F,View1,View2>& r) :
		MutViewRel2<F,View1,View2,Eval>(solver,r.p1,r.p2) {}
	// not sure if the below constructor is ever used
	MutView(ICPSolver& solver, const MutViewRel2<F,View1,View2,Eval>& v) :
		MutViewRel2<F,View1,View2,Eval>(v) {}
};

/**
 * 	MutView over a Rel3 relation -> defer to MutViewRel3.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View1,class View2,class View3>
struct MutView<Eval,Rel3<F,View1,View2,View3> > :
	MutViewRel3<F,View1,View2,View3,Eval>
{
	using MutViewRel3<F,View1,View2,View3,Eval>::attach;
	using MutViewRel3<F,View1,View2,View3,Eval>::detach;

	MutView(ICPSolver& solver, const Rel3<F,View1,View2,View3>& r) :
		MutViewRel3<F,View1,View2,View3,Eval>(solver,r.p1,r.p2,r.p3) {}
};

/**
 * 	MutView over a MutExpr.
 * 	\ingroup Views
 **/
template<class Eval>
struct MutView<Eval,MutExpr<Eval> > : MutExpr<Eval>
{
	using MutExpr<Eval>::attach;
	using MutExpr<Eval>::detach;

	MutView(ICPSolver& solver, const MutExpr<Eval>& r) :
		MutExpr<Eval>(solver,r) {}
};

/**
 * 	MutView over groundness testing.
 * 	\ingroup Views
 **/
template<class View>
struct MutViewRel1<Ground,View,Bool> : IMutExpr<Bool>
{
	typedef typename EvalTraits<View>::Eval	ViewEval;
	MutViewRel1(ICPSolver& solver, const View& p1) :
		IMutExpr<Bool>(solver),p1(solver,p1) {}
	Bool value() const
	{	return p1.ground();	}
	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f)	{	p1.detach(f); }
	View getView()  const { return p1.getView(); }
	ValView<ViewEval,View>	p1;
};

/**
 * 	MutView over symmetric.
 * 	\ingroup Views
 **/
template<class View,class Eval>
struct MutViewRel1<Sym,View,Eval> : IMutExpr<Eval>
{
	MutViewRel1(ICPSolver& solver, const View& p1) :
		IMutExpr<Eval>(solver),p1(solver,p1)	{}
	Eval value() const { return -p1.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); 	}
	void detach(INotifiable* f)	{	p1.detach(f);  }
	View getView()  const { return p1.getView(); }
	MutView<Eval,View>	p1;
};

// logical

/**
 * 	MutView over negation.
 * 	\ingroup Views
 **/
template<class View>
struct MutViewRel1<Not,View,Bool> : IMutExpr<Bool>
{
	MutViewRel1(ICPSolver& solver, const View& p1) :
		IMutExpr<Bool>(solver),p1(solver,p1)	{}
	Bool value() const { return !p1.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); 	}
	void detach(INotifiable* f)	{	p1.detach(f);  }
	View getView()  const { return p1.getView(); }
	MutView<Bool,View>	p1;
};

/**
 * 	MutView over conjunction.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct MutViewRel2<And,View1,View2,Bool> : IMutExpr<Bool>
{
	MutViewRel2(ICPSolver& solver,const View1& p1,const View2& p2) :
		IMutExpr<Bool>(solver),p1(solver,p1),p2(solver,p2) {}
	Bool value() const { return p1.value() && p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}
	Rel2<And,View1,View2> getView()  const
	{ return p1.getView() and p2.getView(); }
	MutView<Bool,View1>	p1;
	MutView<Bool,View2>	p2;
};

/**
 * 	MutView over disjunction.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct MutViewRel2<Or,View1,View2,Bool> : IMutExpr<Bool>
{
	MutViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		IMutExpr<Bool>(solver),p1(solver,p1),p2(solver,p2) 	{}
	Bool value() const { return p1.value() || p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Or,View1,View2> getView()  const
	{ return p1.getView() or p2.getView(); }

	MutView<Bool,View1>	p1;
	MutView<Bool,View2>	p2;
};

// arithmetic

/**
 * 	MutView over addition.
 * 	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct MutViewRel2<Add,View1,View2,Eval> : IMutExpr<Eval>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	typedef typename EvalTraits<View2>::Eval	View2Eval;

	MutViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		IMutExpr<Eval>(solver),p1(solver,p1),p2(solver,p2) {}
	Eval value() const { return p1.value() + p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Add,View1,View2> getView()  const
	{ return p1.getView()+p2.getView(); }

	MutView<View1Eval,View1>	p1;
	MutView<View2Eval,View2>	p2;
};

/**
 * 	MutView over subtraction.
 * 	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct MutViewRel2<Sub,View1,View2,Eval> : IMutExpr<Eval>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	typedef typename EvalTraits<View2>::Eval	View2Eval;

	MutViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		IMutExpr<Eval>(solver),p1(solver,p1),p2(solver,p2) {}
	Eval value() const { return p1.value() - p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Sub,View1,View2> getView()  const
	{ return p1.getView()-p2.getView(); }

	MutView<View1Eval,View1>	p1;
	MutView<View2Eval,View2>	p2;
};

/**
 * 	MutView over multiplication.
 * 	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct MutViewRel2<Mul,View1,View2,Eval> : IMutExpr<Eval>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	typedef typename EvalTraits<View2>::Eval	View2Eval;

	MutViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		IMutExpr<Eval>(solver),p1(solver,p1),p2(solver,p2) {}
	Eval value() const { return p1.value() * p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Mul,View1,View2> getView()  const
	{ return p1.getView()*p2.getView(); }

	MutView<View1Eval,View1>	p1;
	MutView<View2Eval,View2>	p2;
};

/**
 * 	MutView over equality relation.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct MutViewRel2<Equal,View1,View2,Bool> : IMutExpr<Bool>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	typedef typename EvalTraits<View2>::Eval	View2Eval;

	MutViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		IMutExpr<Bool>(solver),p1(solver,p1),p2(solver,p2) {}
	Bool value() const { return p1.value() == p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Equal,View1,View2> getView()  const
	{ return p1.getView()==p2.getView(); }

	MutView<View1Eval,View1>	p1;
	MutView<View2Eval,View2>	p2;
};

/**
 * 	MutView over inequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct MutViewRel2<Greater,View1,View2,Bool> : IMutExpr<Bool>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	typedef typename EvalTraits<View2>::Eval	View2Eval;

	MutViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		IMutExpr<Bool>(solver),p1(solver,p1),p2(solver,p2) {}
	Bool value() const { return p1.value() > p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Greater,View1,View2> getView()  const
	{ return p1.getView()>p2.getView(); }

	MutView<View1Eval,View1>	p1;
	MutView<View2Eval,View2>	p2;
};

/**
 * 	MutView over inequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct MutViewRel2<GreaterEqual,View1,View2,Bool> : IMutExpr<Bool>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	typedef typename EvalTraits<View2>::Eval	View2Eval;

	MutViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		IMutExpr<Bool>(solver),p1(solver,p1),p2(solver,p2) {}
	Bool value() const { return p1.value() >= p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<GreaterEqual,View1,View2> getView()  const
	{ return p1.getView()>=p2.getView(); }

	MutView<View1Eval,View1>	p1;
	MutView<View2Eval,View2>	p2;
};

/**
 * 	MutView over inequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct MutViewRel2<Less,View1,View2,Bool> : IMutExpr<Bool>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	typedef typename EvalTraits<View2>::Eval	View2Eval;

	MutViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		IMutExpr<Bool>(solver),p1(solver,p1),p2(solver,p2) {}
	Bool value() const { return p1.value() < p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<Less,View1,View2> getView()  const
	{ return p1.getView()<p2.getView(); }

	MutView<View1Eval,View1>	p1;
	MutView<View2Eval,View2>	p2;
};

/**
 * 	MutView over inequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct MutViewRel2<LessEqual,View1,View2,Bool> : IMutExpr<Bool>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	typedef typename EvalTraits<View2>::Eval	View2Eval;

	MutViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		IMutExpr<Bool>(solver),p1(solver,p1),p2(solver,p2) {}
	Bool value() const { return p1.value() <= p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<LessEqual,View1,View2> getView()  const
	{ return p1.getView()<=p2.getView(); }

	MutView<View1Eval,View1>	p1;
	MutView<View2Eval,View2>	p2;
};

/**
 * 	MutView over disequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct MutViewRel2<Distinct,View1,View2,Bool> : IMutExpr<Bool>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	typedef typename EvalTraits<View2>::Eval	View2Eval;

	MutViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		IMutExpr<Bool>(solver),p1(solver,p1),p2(solver,p2) {}
	Bool value() const { return p1.value() != p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<Distinct,View1,View2> getView()  const
	{ return p1.getView()!=p2.getView(); }

	MutView<View1Eval,View1>	p1;
	MutView<View2Eval,View2>	p2;
};

/**
 * 	BndView over MutVar.
 * 	\ingroup Views
 **/
template<class Eval>
struct BndView<Eval,MutVar<Eval> >
{
	BndView(ICPSolver& solver, const MutVar<Eval>& p1) :
		val(p1.value()),mSolver(solver)	{}
	ICPSolver& solver() const {	return mSolver;	}
	Eval min() const {	return val;}
	Eval max() const {	return val;}
	Bool updateMin(const Eval& v) 	{ return v <= min(); }
	Bool updateMax(const Eval& v)   { return v >= max(); }
	Void range(Eval& l,Eval& u) const {	l = u = val; }
	Bool updateRange(const Eval& l, const Eval& u)
	{	return l <= val and val <= u;	}

	void attach(INotifiable* f) {}
	void detach(INotifiable* f) 	{}
	MutVar<Eval>	getView() const
	{	return MutVar<Eval>(solver(),val);	}

	Eval			val;
	ICPSolver&			mSolver;
};

/**
 * 	ValView over MutVar.
 * 	\ingroup Views
 **/
template<class Eval>
struct ValView<Eval,MutVar<Eval> >
{
	ValView(ICPSolver& solver, const MutVar<Eval>& p1) :
		val(p1.value()),mSolver(solver)	{}
	Eval value() const {	return val;}
	Bool setValue(const Eval& v)
	{	return v == val;	}

	Bool ground() const { return true; }
	void attach(INotifiable* f) {}
	void detach(INotifiable* f) {}
	MutVar<Eval>	getView() const
	{	return MutVar<Eval>(mSolver,val);	}

	const Eval	val;
	ICPSolver&	mSolver;
};

/**
 * 	BndView over MutExpr.
 * 	\ingroup Views
 **/
template<class Eval>
struct BndView<Eval,MutExpr<Eval> >
{
	BndView(ICPSolver& solver, const MutExpr<Eval>& p1) :
		val(p1.value())	{}
	ICPSolver& solver() const {	return mSolver;	}
	Eval min() const {	return val;}
	Eval max() const {	return val;}
	Bool updateMin(const Eval& v) 	{ return v <= min(); }
	Bool updateMax(const Eval& v)   { return v >= max(); }
	Void range(Eval& l,Eval& u) const {	l = u = val; }
	Bool updateRange(const Eval& l, const Eval& u)
	{	return l <= val and val <= u;	}

	void attach(INotifiable* f) {}
	void detach(INotifiable* f) 	{}
	MutExpr<Eval>	getView() const
	{	return MutExpr<Eval>(solver(),val);	}

	Eval			val;
	ICPSolver&			mSolver;
};

/**
 * 	ValView over MutExpr.
 * 	\ingroup Views
 **/
template<class Eval>
struct ValView<Eval,MutExpr<Eval> >
{
	ValView(ICPSolver& solver, const MutExpr<Eval>& p1) :
		val(p1.value())	{}
	Eval value() const {	return val;}
	Bool setValue(const Eval& v)
	{	return v == val;	}

	Bool ground() const { return true; }
	void attach(INotifiable* f) {}
	void detach(INotifiable* f) {}
	MutExpr<Eval>	getView() const
	{	return MutExpr<Eval>(this->solver(),val);	}

	Eval	val;
};

};

#endif /*_H_CASPER_KERNEL_VIEW_MUTVIEW */
