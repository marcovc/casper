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

#ifndef _H_CASPER_KERNEL_VIEW_VALVIEW
#define _H_CASPER_KERNEL_VIEW_VALVIEW

#include <casper/kernel/expression.h>
#include <casper/kernel/view/arrayview.h>

namespace casper {

/********************
 * Value View
 *********************/

template<class Eval,class View>
struct ValView;

/**
 * 	ValView over a generic view. View must be convertible to Eval.
 * 	\ingroup Views
 **/

template<class Eval,class View>
struct ValView //: NoView
{	ValView(ICPSolver&, const View&) { assert(0); }	};

/**
 * 	ValView over a literal type.
 * 	\ingroup Views
 **/
template<class Eval>
struct ValView<Eval,Eval>
{
	ValView(ICPSolver& solver, const Eval& v) :
		v(v) {}

	Eval value() const { return v; }
	Bool setValue(const Eval& val) { return val == v;	}
	Bool ground() const { return true; }
	Void attach(INotifiable*) {}
	Void detach(INotifiable*) {}
	Eval getView()  const { return v; }
	const Eval	v;
};

/**
 * 	Int ValView over a UInt literal type.
 *  (for convenience)
 * 	\ingroup Views
 **/
template<>
struct ValView<Int,UInt>
{
	ValView(ICPSolver& solver, const UInt& v) :
		v(v) {}

	Int value() const { return static_cast<Int>(v); }
	Bool setValue(const Int& val) { return val == static_cast<Int>(v);	}
	Bool ground() const { return true; }
	Void attach(INotifiable*) {}
	Void detach(INotifiable*) {}
	UInt getView()  const { return v; }
	const UInt	v;
};

/**
 *	Identity : ValView over a ValView.
 *	\ingroup Views
 **/
template<class Eval,class View>
struct ValView<Eval,ValView<Eval,View> > : ValView<Eval,View>
{
	typedef ValView<Eval,View>	Super;
	using ValView<Eval,View>::attach;
	using ValView<Eval,View>::detach;

	ValView(ICPSolver& solver, const ValView<Eval,View>& v) :
		Super(v) {}

	Super getView()  const { return static_cast<const Super&>(*this); }
};

// TODO: reification here
template<class,class,class>			struct ValViewRel1;
template<class,class,class,class>	struct ValViewRel2;
template<class,class,class,class,class>	struct ValViewRel3;

/**
 * 	ValView over a Rel1 relation -> defer to ValViewRel1.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View>
struct ValView<Eval,Rel1<F,View> > : ValViewRel1<F,View,Eval>
{
	ValView(ICPSolver& solver, const Rel1<F,View>& r) :
		ValViewRel1<F,View,Eval>(solver,r.p1) {}
	// not sure if the below constructor is ever used
	ValView(ICPSolver& solver,const ValViewRel1<F,View,Eval>& v) :
		ValViewRel1<F,View,Eval>(v) {}
};

/**
 * 	ValView over a Rel2 relation -> defer to ValViewRel2.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View1,class View2>
struct ValView<Eval,Rel2<F,View1,View2> > :
	ValViewRel2<F,View1,View2,Eval>
{
	ValView(ICPSolver& solver, const Rel2<F,View1,View2>& r) :
		ValViewRel2<F,View1,View2,Eval>(solver,r.p1,r.p2) {}
	// not sure if the below constructor is ever used
	ValView(ICPSolver& solver, const ValViewRel2<F,View1,View2,Eval>& v) :
		ValViewRel2<F,View1,View2,Eval>(v) {}
};

/**
 * 	ValView over a Rel3 relation -> defer to ValViewRel3.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View1,class View2,class View3>
struct ValView<Eval,Rel3<F,View1,View2,View3> > :
	ValViewRel3<F,View1,View2,View3,Eval>
{
	ValView(ICPSolver& solver, const Rel3<F,View1,View2,View3>& r) :
		ValViewRel3<F,View1,View2,View3,Eval>(solver,r.p1,r.p2,r.p3) {}
};

/**
 * 	ValView over a ValExpr.
 * 	\ingroup Views
 **/
template<class Eval>
struct ValView<Eval,ValExpr<Eval> > : ValExpr<Eval>
{
	ValView(ICPSolver& solver, const ValExpr<Eval>& r) :
		ValExpr<Eval>(solver,r) {}
};


// conversion

template<class View,class Eval>
struct ValViewRel1<Cast<Eval>,View,Eval>
{
	typedef typename EvalTraits<View>::Eval	EvalFrom;

	ValViewRel1(ICPSolver& solver, const View& v) :
		v(solver,v) {}

	Eval value() const { return static_cast<Eval>(v.value()); }
	Bool setValue(const Eval& val)
	{ return v.setValue(static_cast<EvalFrom>(val));	}
	Bool ground() const { return v.ground(); }
	Void attach(INotifiable* n) {	v.attach(n);	}
	Void detach(INotifiable* n) {	v.detach(n);	}
	ValView<EvalFrom,View>	v;
};

/**
 * 	ValView over symmetric.
 * 	\ingroup Views
 **/
template<class View,class Eval>
struct ValViewRel1<Sym,View,Eval>
{
	ValViewRel1(ICPSolver& solver, const View& p1) :
		p1(solver,p1)	{}
	Eval value() const { return -p1.value(); }
	Bool setValue(const Eval& val) { return p1.setValue(-val);	}
	Bool ground() const { return p1.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); 	}
	void detach(INotifiable* f) {	p1.detach(f); }
	Rel1<Sym,View>	getView() const {	return Rel1<Sym,View>(p1.getView());}
	ValView<Eval,View>	p1;
};

/**
 * 	ValView over negation.
 * 	\ingroup Views
 **/
template<class View>
struct ValViewRel1<Not,View,Bool>
{
	ValViewRel1(ICPSolver& solver,const View& p1) :
		p1(solver,p1)	{}
	Bool value() const { return !p1.value(); }
	Bool setValue(const Bool& val) { return p1.setValue(!val);	}
	Bool ground() const { return p1.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); 	}
	void detach(INotifiable* f)	{	p1.detach(f);	}
	Rel1<Not,View>	getView() const {	return Rel1<Not,View>(p1.getView());}
	ValView<Bool,View>	p1;
};

// logical

/**
 * 	ValView over conjunction.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ValViewRel2<And,View1,View2,Bool>
{
	ValViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) {}
	Bool value() const { return p1.value() && p2.value(); }
	Bool setValue(const Bool& val)
	{
		if (val)
			return p1.setValue(true) and p2.setValue(true);
		if (p1.ground())
			return !p1.value() or p2.setValue(false);
		if (p2.ground())
			return !p2.value() or p1.setValue(false);
		return true;
	}
	Bool ground() const
	{
		return (p1.ground() and p2.ground()) or
			   (p1.ground() and !p1.value()) or
			   (p2.ground() and !p2.value());
	}
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}
	Rel2<And,View1,View2>	getView() const
	{	return Rel2<And,View1,View2>(p1.getView(),p2.getView());}
	ValView<Bool,View1>	p1;
	ValView<Bool,View2>	p2;
};

/**
 * 	ValView over disjunction.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ValViewRel2<Or,View1,View2,Bool>
{
	ValViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) 	{}
	Bool value() const { return p1.value() or p2.value(); }
	Bool setValue(const Bool& val)
	{
		if (!val)
			return p1.setValue(false) and p2.setValue(false);
		if (p1.ground())
			return p1.value() or p2.setValue(true);
		if (p2.ground())
			return p2.value() or p1.setValue(true);
		return true;
	}
	Bool ground() const
	{
		return (p1.ground() and p2.ground()) or
			   (p1.ground() and p1.value()) or
			   (p2.ground() and p2.value());
	}

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Or,View1,View2>	getView() const
	{	return Rel2<Or,View1,View2>(p1.getView(),p2.getView());}

	ValView<Bool,View1>	p1;
	ValView<Bool,View2>	p2;
};

/**
 * 	ValView over XOR.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ValViewRel2<XOr,View1,View2,Bool>
{
	ValViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) 	{}
	Bool value() const { return p1.value() ^ p2.value(); }
	Bool setValue(const Bool& val)
	{
		if (p1.ground())
			return p2.setValue(val ^ p1.value());
		if (p2.ground())
			return p1.setValue(val ^ p2.value());
		return true;
	}
	Bool ground() const { return p1.ground() and p2.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<XOr,View1,View2>	getView() const
	{	return Rel2<XOr,View1,View2>(p1.getView(),p2.getView());}

	ValView<Bool,View1>	p1;
	ValView<Bool,View2>	p2;
};

// arithmetic

/**
 * 	ValView over addition.
 * 	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct ValViewRel2<Add,View1,View2,Eval>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	typedef typename EvalTraits<View2>::Eval	View2Eval;

	ValViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) {}
	Eval value() const { return p1.value() + p2.value(); }
	Bool setValue(const Eval& val)
	{
		if (p1.ground())
			return p2.setValue(val - p1.value());
		if (p2.ground())
			return p1.setValue(val - p2.value());
		return true;
	}
	Bool ground() const { return p1.ground() && p2.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Add,View1,View2>	getView() const
	{	return Rel2<Add,View1,View2>(p1.getView(),p2.getView());}

	ValView<View1Eval,View1>	p1;
	ValView<View2Eval,View2>	p2;
};

/**
 * 	ValView over subtraction.
 * 	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct ValViewRel2<Sub,View1,View2,Eval>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	typedef typename EvalTraits<View2>::Eval	View2Eval;

	ValViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) {}
	Eval value() const { return p1.value() - p2.value(); }
	Bool setValue(const Eval& val)
	{
		if (p1.ground())
			return p2.setValue(val + p1.value());
		if (p2.ground())
			return p1.setValue(val + p2.value());
		return true;
	}
	Bool ground() const { return p1.ground() and p2.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}
	Rel2<Sub,View1,View2>	getView() const
	{	return Rel2<Sub,View1,View2>(p1.getView(),p2.getView());}

	ValView<View1Eval,View1>	p1;
	ValView<View2Eval,View2>	p2;
};

/**
 * 	ValView over multiplication.
 * 	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct ValViewRel2<Mul,View1,View2,Eval>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	typedef typename EvalTraits<View2>::Eval	View2Eval;

	ValViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) {}
	Eval value() const { return p1.value() * p2.value(); }
	Bool ground() const { return p1.ground() && p2.ground(); }
	Bool setValue(const Eval& val)
	{
		if (p1.ground())
			return p2.setValue(val / p1.value());
		if (p2.ground())
			return p1.setValue(val / p2.value());
		return true;
	}
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Mul,View1,View2>	getView() const
	{	return Rel2<Mul,View1,View2>(p1.getView(),p2.getView());}

	ValView<View1Eval,View1>	p1;
	ValView<View2Eval,View2>	p2;
};

/**
 * 	ValView over BndExpr.
 * 	\ingroup Views
 **/
template<class Eval>
struct ValView<Eval,BndExpr<Eval> >
{
	ValView(ICPSolver& solver,const BndExpr<Eval>& p1) :
		p1(p1)	{}
	Eval value() const { assert(ground()); return p1.min(); }
	Bool setValue(const Eval& val)
	{	return p1.updateRange(val,val);	}
	Bool ground() const { return p1.min()==p1.max(); }
	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }
	BndExpr<Eval>	getView() const {	return p1; }

	BndExpr<Eval>	p1;
};

/**
 * 	ValView over DomExpr.
 * 	\ingroup Views
 **/
template<class Eval,class Dom>
struct ValView<Eval,DomExpr<Eval,Dom> >
{
	ValView(ICPSolver& solver,const DomExpr<Eval,Dom>& p1) :
		p1(p1)	{}
	Eval value() const { assert(ground()); return p1->value(); }
	Bool setValue(const Eval& val)
	{	return p1->setValue(val);	}

	Bool ground() const { return p1->ground(); }
	void attach(INotifiable* f) { 	p1->attachOnGround(f); }
	void detach(INotifiable* f) {	p1->detachOnGround(f); }
	DomExpr<Eval,Dom>	getView() const {	return p1; }

	DomExpr<Eval,Dom>	p1;
};

#if 0
/**
 * 	ValView over BndView. \deprecated
 * 	\ingroup Views
 **/
template<class Eval,class View>
struct ValView<Eval,BndView<Eval,View> > : IValExpr<Eval>
{
	ValView(ICPSolver& solver,const BndView<Eval,View>& p1) :
		IValExpr<Eval>(solver),p1(p1)	{}
	Eval value() const { assert(ground()); return p1.min(); }
	Bool ground() const { return p1.min()==p1.max(); }
	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }
	BndView<Eval,View>	getView() const	{	return p1; }

	BndView<Eval,View>	p1;
};
#endif

/**
 * 	ValView over element.
 * 	\ingroup Views
 **/
#if 0
template<class View1,class View2,class Eval>
struct ValViewRel2<Element,View1,View2,Eval> : IValExpr<Eval>
{
	typedef Rel2<Element,View1,View2>	Rel;
	ValViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		IValExpr<Eval>(solver),v(solver,Rel(p1,p2))
		//array(solver,p1),index(solver,p2)
		{}
	Eval value() const { assert(ground()); return v->value(); /*return array[index.value()]->min();*/ }
	Bool ground() const
	{	return v->ground(); /*return index.ground() && array[index.value()]->min()==array[index.value()]->max();*/ }
	void attach(INotifiable* f) { 	v->attachOnGround(f); /*array.attach(f);index.attach(f);*/ }
	void detach(INotifiable* f) {	v->detachOnGround(f); /*array.detach(f);index.detach(f);*/ }

	DomExpr<Eval> v;
	//DomArrayView<Eval,View1>	array;	// TODO: replace by ValArrayView
	//ValView<Eval,View2>			index;
};
#else
template<class View1,class View2,class Eval>
struct ValViewRel2<Element,View1,View2,Eval>
{
	typedef Rel2<Element,View1,View2>	Rel;
	ValViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		array(solver,p1),index(solver,p2)
		{}
	Eval value() const { assert(ground()); return array[index.value()].value(); }
	Bool setValue(const Eval& val)
	{
		if (!index.ground())
			return true;
		return array[index.value()].setValue(val);
	}

	Bool ground() const
	{	return index.ground() && array[index.value()].value()==array[index.value()].value(); }
	void attach(INotifiable* f) { 	array.attach(f);index.attach(f); }
	void detach(INotifiable* f) {	array.detach(f);index.detach(f); }
	Rel2<Element,View1,View2>	getView() const
	{	return Rel2<Element,View1,View2>(array.getView(),index.getView());}

	ValArrayView<Eval,View1>	array;
	ValView<Eval,View2>			index;
};
#endif

/**
 * 	ValView over absolute value.
 * 	\ingroup Views
 **/
template<class View1,class Eval>
struct ValViewRel1<Abs,View1,Eval>
{
	typedef typename EvalTraits<View1>::Eval	View1Eval;
	ValViewRel1(ICPSolver& solver, const View1& p1) :
		p1(solver,p1) {}

	Eval value() const { assert(ground()); return std::abs(p1.value()); }
	Bool setValue(const Eval& val)
	{
		if (val<0)
			return false;
		if (p1.ground())
			return val == std::abs(p1.value());
		return true;
	}
	Bool ground() const { return p1.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }
	Rel1<Abs,View1>	getView() const
	{	return Rel1<Abs,View1>(p1.getView());}

	ValView<View1Eval,View1>	p1;
};

template<class Eval,class View>
std::ostream& operator<<(std::ostream& os, const casper::ValView<Eval,View>& v)
{
	if (v.ground())
		os << v.value();
	else
		os << "[not ground]";
	return os;
}

};

#endif /*_H_CASPER_KERNEL_VIEW_VALVIEW */
