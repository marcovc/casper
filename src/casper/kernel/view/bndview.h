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

#ifndef _H_CASPER_KERNEL_VIEW_BNDVIEW
#define _H_CASPER_KERNEL_VIEW_BNDVIEW

#include <casper/kernel/container/range.h>
#include <casper/kernel/view/common.h>
#include <casper/kernel/safeop.h>
#include <casper/kernel/io.h>
#include <casper/kernel/assertions.h>
#include <cmath>

namespace casper {

/********************
 *  Bounds View
 *********************/

template<class EvalT,class View>
struct EvalTraits<BndView<EvalT,View> >
{	typedef	EvalT	Eval;	};


template<class T1,class T2>
struct GetCPSolver<BndView<T1,T2> >
{	ICPSolver& operator()(BndView<T1,T2> s)
	{ return s.solver(); }
};



// converts a view to a checkview
template<class Eval,class View>
struct ChkViewWrapper;

template<class> struct CChkView;

template<class View>
struct ChkViewWrapper<Bool,View>
{
	ChkViewWrapper(ICPSolver& solver,const View& v) :
		v(solver,v) {}
	Bool min() const
	{	return v.isTrue();	}
	Bool max() const
	{	return v.canBeTrue();	}
	Bool updateMin(const Bool& val)
	{	return !val or v.isTrue() or v.setToTrue();	}
	Bool updateMax(const Bool& val)
	{	return val or !v.canBeTrue() or v.setToFalse();	}
	// TODO: improve:
	Void range(Bool& l,Bool& u) const
	{	l = min(); u = max();	}
	Bool updateRange(const Bool& l, const Bool& u)
	{	return updateMin(l) and updateMax(u);	}
	Void attach(INotifiable* f) { v.attach(f);	}
	Void detach(INotifiable* f) { v.detach(f);	}
	View getView()  const { return v.getView(); }

	CChkView<View>	v;
};

// FIXME: this is needed because of the minicasper mess
template<class View>
struct ChkViewWrapper<Int,View>
{
	ChkViewWrapper(ICPSolver& solver,const View& v) :
		v(solver,v) {}
	Int min() const
	{	return static_cast<Int>(v.isTrue());	}
	Int max() const
	{	return static_cast<Int>(v.canBeTrue());	}
	Bool updateMin(const Int& val)
	{	return val==0 or v.isTrue() or v.setToTrue();	}
	Bool updateMax(const Int& val)
	{	return val!=0 or !v.canBeTrue() or v.setToFalse();	}
	// TODO: improve:
	Void range(Int& l,Int& u) const
	{	l = min(); u = max();	}
	Bool updateRange(const Int& l, const Int& u)
	{	return updateMin(l) and updateMax(u);	}
	Void attach(INotifiable* f) { v.attach(f);	}
	Void detach(INotifiable* f) { v.detach(f);	}
	View getView()  const { return v.getView(); }

	CChkView<View>	v;
};

// specializations for bndview over boolean (reificable) expressions

/**
 * 	BndView over a generic view.
 * 	\ingroup Views
 **/
template<class Eval,class View>
struct BndView : ChkViewWrapper<Eval,View>
{
	CASPER_ASSERT_CHKVIEW_EVAL(Eval)

	BndView(ICPSolver& solver, const View& v) :
		ChkViewWrapper<Eval,View>(solver,v) {}
};

template<class F,class View1,class Eval>
struct BndViewRel1 : ChkViewWrapper<Eval,Rel1<F,View1> >
{
	CASPER_ASSERT_CHKVIEW_EVAL(Eval)

	BndViewRel1(ICPSolver& solver, const View1& v) :
		ChkViewWrapper<Eval,Rel1<F,View1> >(solver,rel<F>(v)) {}
};

template<class F,class View1,class View2,class Eval>
struct BndViewRel2 : ChkViewWrapper<Eval,Rel2<F,View1,View2> >
{
	CASPER_ASSERT_CHKVIEW_EVAL(Eval)

	BndViewRel2(ICPSolver& solver, const View1& v1, const View2& v2) :
		ChkViewWrapper<Eval,Rel2<F,View1,View2> >(solver,rel<F>(v1,v2)) {}
};

/**
 * 	BndView over a literal type. Literal
 *  must support <= and >= operations.
 * 	\ingroup Views
 **/
template<class Eval>
struct BndView<Eval,Eval>
{
	BndView(ICPSolver& solver, const Eval& v) :
		mSolver(solver),v(v) {}

	Eval min() const { return v; }
	Eval max() const { return v; }
	Bool updateMin(const Eval& val)
	{ return val <= min(); }
	Bool updateMax(const Eval& val)
	{ return val >= max(); }
	Void range(Eval& v1,Eval& v2) const
	{	v1=v2=v; }
	Bool updateRange(const Eval& v1, const Eval& v2)
	{	return v1<=v and v<=v2;	}
//	Eval getMinInRange(Eval l,Eval u) const
//	{	return v;	}
	ICPSolver& solver() const {	return mSolver;	}
	Void attach(INotifiable*) {}
	Void detach(INotifiable*) {}
	Eval getView()  const { return v; }

	ICPSolver& mSolver;
	const Eval	v;
};

/**
 * 	Int BndView over a UInt literal type.
 *  (for convenience)
 * 	\ingroup Views
 **/
template<>
struct BndView<Int,UInt>
{
	BndView(ICPSolver& solver, const UInt& v) :
		v(v) {}

	Int min() const { return static_cast<Int>(v); }
	Int max() const { return static_cast<Int>(v); }
	Bool updateMin(const Int& val)
	{ return val <= min(); }
	Bool updateMax(const Int& val)
	{ return val >= max(); }
	Void range(Int& v1,Int& v2) const
	{	 v1 = v2 = static_cast<Int>(v);	}
	Bool updateRange(const Int& v1, const Int& v2)
	{	return v1<=static_cast<Int>(v) and
			   static_cast<Int>(v)<=v2;	}
	ICPSolver& solver() const {	return glbNullCPSolver;	}
	Void attach(INotifiable*) {}
	Void detach(INotifiable*) {}
	UInt getView()  const { return v; }
	const UInt	v;
};

#if 0
/**
 *	Identity : BndView over a BndView.
 *	\ingroup Views
 **/
template<class Eval,class View>
struct BndView<Eval,BndView<Eval,View> > : BndView<Eval,View>
{
	typedef BndView<Eval,View>	Super;
	using BndView<Eval,View>::attach;
	using BndView<Eval,View>::detach;

	BndView(ICPSolver& solver, const BndView<Eval,View>& v) :
		Super(v) {}

	Super getView()  const { return static_cast<const Super&>(*this); }
};
#endif



// TODO: reification here
template<class,class,class>			struct BndViewRel1;
template<class,class,class,class>	struct BndViewRel2;
template<class,class,class,class,class>	struct BndViewRel3;

/**
 *	BndView over a Rel1 relation -> defers to BndViewRel1
 *	\ingroup Views
 **/
template<class Eval,class F,class View>
struct BndView<Eval,Rel1<F,View> > : BndViewRel1<F,View,Eval>
{
	CASPER_ASSERT_BNDVIEW_EVAL(Eval)

	BndView(ICPSolver& solver, const Rel1<F,View>& r) :
		BndViewRel1<F,View,Eval>(solver,r.p1) {}
	// not sure if the below constructor is ever used
	BndView(ICPSolver& solver,const BndViewRel1<F,View,Eval>& v) :
		BndViewRel1<F,View,Eval>(v) {}
};

/**
 *	BndView over a Rel2 relation -> defers to BndViewRel2
 *	\ingroup Views
 **/
template<class Eval,class F,class View1,class View2>
struct BndView<Eval,Rel2<F,View1,View2> > :
	BndViewRel2<F,View1,View2,Eval>
{
	CASPER_ASSERT_BNDVIEW_EVAL(Eval)

	BndView(ICPSolver& solver, const Rel2<F,View1,View2>& r) :
		BndViewRel2<F,View1,View2,Eval>(solver,r.p1,r.p2) {}
	// not sure if the below constructor is ever used
	BndView(ICPSolver& solver, const BndViewRel2<F,View1,View2,Eval>& v) :
		BndViewRel2<F,View1,View2,Eval>(v) {}
};

/**
 *	BndView over a Rel3 relation -> defers to BndViewRel3
 *	\ingroup Views
 **/
template<class Eval,class F,class View1,class View2,class View3>
struct BndView<Eval,Rel3<F,View1,View2,View3> > :
	BndViewRel3<F,View1,View2,View3,Eval>
{
	CASPER_ASSERT_BNDVIEW_EVAL(Eval)

	BndView(ICPSolver& solver, const Rel3<F,View1,View2,View3>& r) :
		BndViewRel3<F,View1,View2,View3,Eval>(solver,r.p1,r.p2,r.p3) {}
};

/**
 *	BndView over a BndExpr
 *	\ingroup Views
 **/
template<class Eval>
struct BndView<Eval,BndExpr<Eval> >
{
	BndView(ICPSolver& solver, const BndExpr<Eval>& r) :
		r(r) {}

	Eval min() const { return r.min(); }
	Eval max() const { return r.max(); }
	Bool updateMin(const Eval& val)
	{ return r.updateMin(val); }
	Bool updateMax(const Eval& val)
	{ return r.updateMax(val); }
	Void range(Eval& v1,Eval& v2) const
	{ r.range(v1,v2);	 }
	Bool updateRange(const Eval& v1, const Eval& v2)
	{	return r.updateRange(v1,v2);	}
	ICPSolver& solver() const	{	return r.solver();	}
	Void attach(INotifiable* n) {	return r.attach(n);	}
	Void detach(INotifiable* n) {	return r.detach(n);	}
	BndExpr<Eval> 	getView()  const { return r; }
	BndExpr<Eval>	r;
};

/**
 *	BndView over a DomExpr
 *	\ingroup Views
 **/
template<class Eval,class Dom>
struct BndView<Eval,DomExpr<Eval,Dom> > :
	BndView<Eval,DomVar<Eval,Dom> >
{
	typedef BndView<Eval,DomVar<Eval,Dom> >	Super;
	BndView(ICPSolver& solver, const DomExpr<Eval,Dom>& r) :
		BndView<Eval,DomVar<Eval,Dom> >(solver,
					DomVar<Eval,Dom>(solver,r.operator->())) {}
	DomExpr<Eval,Dom> getView() const
	{	return DomExpr<Eval,Dom>(this->solver(),Super::getView()); }
};

#if 0
/**
 *	BndView over a DomView
 *	\ingroup Views
 **/
template<class Eval,class View,class Dom>
struct BndView<Eval,DomView<Eval,View,Dom> > : BndView<Eval,DomVar<Eval,Dom> >
{
	BndView(ICPSolver& solver, const DomView<Eval,View,Dom>& r) :
		BndView<Eval,DomVar<Eval,Dom> >(solver,DomVar<Eval,Dom>(solver,r.operator->())) {}
};
#endif

// ******** conversion **********

template<class View,class Eval>
struct BndViewRel1<Cast<Eval>,View,Eval>
{
	typedef typename EvalTraits<View>::Eval	EvalFrom;

	BndViewRel1(ICPSolver& solver, const View& v) :
		v(solver,v) {}

	Eval min() const { return detail::convLb<Eval>(v.min()); }
	Eval max() const { return detail::convUb<Eval>(v.max()); }
	Bool updateMin(const Eval& val)
	{ return v.updateMin(detail::convLb<EvalFrom>(val));	}
	Bool updateMax(const Eval& val)
	{ return v.updateMax(detail::convUb<EvalFrom>(val));	}
	Void range(Eval& v1,Eval& v2) const
	{ v1 = min(); v2=max(); }
	Bool updateRange(const Eval& v1, const Eval& v2)
	{	return v.updateRange(detail::convLb<EvalFrom>(v1),
							 detail::convUb<EvalFrom>(v2));	}

	Void attach(INotifiable* n) {	v.attach(n);	}
	Void detach(INotifiable* n) {	v.detach(n);	}
	Rel1<Cast<Eval>,View> getView()  const
	{ return Rel1<Cast<Eval>,View>(v.getView()); }

	BndView<EvalFrom,View>	v;
};


/**
 *	BndView over an if-then-else expression
 *	\ingroup Views
 **/
template<class Eval,class View1,class View2,class View3>
struct BndViewRel3<IfThenElse,View1,View2,View3,Eval>
{
	BndViewRel3(ICPSolver& solver, const View1& p1, const View2& p2, const View3& p3) :
		c1(solver,p1),p2(solver,p2),p3(solver,p3)
		{}
	Eval min() const
	{
		if (c1.isTrue())
			return p2.min();
		if (!c1.canBeTrue())
			return p3.min();
		return std::min(p2.min(),p3.min());
	}
	Eval max() const
	{
		if (c1.isTrue())
			return p2.max();
		if (!c1.canBeTrue())
			return p3.max();
		return std::max(p2.max(),p3.max());
	}
	Bool updateMin(const Eval& v)
	{
		if (c1.isTrue())
			return p2.updateMin(v);
		if (!c1.canBeTrue())
			return p3.updateMin(v);

		if (v > p2.max())
		{
			c1.setToFalse();
			p2.detach(pOwner);
			return p3.updateMin(v);
		}
		else
		if (v > p3.max())
		{
			c1.setToTrue();
			p3.detach(pOwner);
			return p2.updateMin(v);
		}
		return true;
	}
	Bool updateMax(const Eval& v)
	{
		if (c1.isTrue())
			return p2.updateMax(v);
		if (!c1.canBeTrue())
			return p3.updateMax(v);

		if (v < p2.min())
		{
			c1.setToFalse();
			p2.detach(pOwner);
			return p3.updateMax(v);
		}
		else
		if (v < p3.min())
		{
			c1.setToTrue();
			p3.detach(pOwner);
			return p2.updateMax(v);
		}
		return true;
	}
	Void range(Eval& v1,Eval& v2) const
	{	v1=min(); v2=max();	}
	Bool updateRange(const Eval& v1, const Eval& v2)
	{	return updateMin(v1) and updateMax(v2);	}

	void attach(INotifiable* f) { 	pOwner=f;c1.attach(f); p2.attach(f);	p3.attach(f); }
	void detach(INotifiable* f) {	c1.detach(f); p2.detach(f);	p3.detach(f); }
	Rel3<IfThenElse,View1,View2,View3> getView()  const
	{ 	return Rel3<IfThenElse,View1,View2,View3>(c1.getView(),p2.getView(),p3.getView());	}

	CChkView<View1> 	c1;
	BndView<Eval,View2>	p2;
	BndView<Eval,View3>	p3;
	INotifiable*		pOwner;
};

// ********** arithmetic ************

/**
 *	BndView over minimum of two values.
 *	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct BndViewRel2<Min,View1,View2,Eval>
{
	BndViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) {}
	Eval min() const
	{	return std::min(p1.min(),p2.min());	}
	Eval max() const
	{	return std::min(p1.max(),p2.max()); }

	Bool updateMin(const Eval& v)
	{	return p1.updateMin(v) and p2.updateMin(v);	}

	// only does anything if there is only one element with min > v
	Bool updateMax(const Eval& v)
	{
		if (p2.min() > v)
			return p1.updateMax(v);
		else
		if (p1.min() > v)
			return p2.updateMax(v);
		return true;
	}
	Void range(Eval& v1,Eval& v2) const
	{
		Eval i1,i2,a1,a2;
		p1.range(i1,a1);
		p2.range(i2,a2);
		v1 = std::min(i1,i2);
		v2 = std::min(a1,a2);
	}
	Bool updateRange(const Eval& v1, const Eval& v2)
	{	return updateMin(v1) and updateMax(v2);	}

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f); }
	Rel2<Min,View1,View2>	getView() const
	{	return 	Rel2<Min,View1,View2>(p1.getView(),p2.getView());	}

	BndView<Eval,View1>	p1;
	BndView<Eval,View2>	p2;
};

/**
 *	BndView over maximum of two values.
 *	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct BndViewRel2<Max,View1,View2,Eval>
{
	BndViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) {}
	Eval max() const
	{	return std::max(p1.max(),p2.max());	}
	Eval min() const
	{	return std::max(p1.min(),p2.min()); }

	Bool updateMax(const Eval& v)
	{	return p1.updateMax(v) and p2.updateMax(v);	}

	// only does anything if there is only one element with max < v
	Bool updateMin(const Eval& v)
	{
		if (p2.max() < v)
			return p1.updateMin(v);
		else
		if (p1.max() < v)
			return p2.updateMin(v);
		return true;
	}
	Void range(Eval& v1,Eval& v2) const
	{
		Eval i1,i2,a1,a2;
		p1.range(i1,a1);
		p2.range(i2,a2);
		v1 = std::max(i1,i2);
		v2 = std::max(a1,a2);
	}
	Bool updateRange(const Eval& v1, const Eval& v2)
	{	return updateMin(v1) and updateMax(v2);	}

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f); }
	Rel2<Max,View1,View2>	getView() const
	{	return 	Rel2<Max,View1,View2>(p1.getView(),p2.getView());	}

	BndView<Eval,View1>	p1;
	BndView<Eval,View2>	p2;
};

/**
 *	BndView over symmetric.
 *	\ingroup Views
 **/
template<class View,class Eval>
struct BndViewRel1<Sym,View,Eval>
{
	typedef typename EvalTraits<View>::Eval	ViewEval;

	BndViewRel1(ICPSolver& solver, const View& p1) :
		p1(solver,p1)	{}
	Eval min() const { return detail::convLb<Eval>(-p1.max()); }
	Eval max() const { return detail::convUb<Eval>(-p1.min()); }
	Bool updateMin(const Eval& v)
	{ return p1.updateMax(detail::convUb<ViewEval>(-v)); }
	Bool updateMax(const Eval& v)
	{ return p1.updateMin(detail::convLb<ViewEval>(-v)); }
	Void range(Eval& v1,Eval& v2) const
	{
		Eval i1,a1;
		p1.range(i1,a1);
		v1 = detail::convLb<Eval>(-a1);
		v2 = detail::convUb<Eval>(-i1);
	}
	Bool updateRange(const Eval& v1, const Eval& v2)
	{	return p1.updateRange(detail::convLb<ViewEval>(-v2),
							  detail::convUb<ViewEval>(-v1));	}

	void attach(INotifiable* f)	{	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }
	Rel1<Sym,View> getView()  const
	{ 	return Rel1<Sym,View>(p1.getView());	}

	BndView<ViewEval,View>	p1;
};

/**
 *	BndView over addition.
 *	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct BndViewRel2<Add,View1,View2,Eval>
{
/*	static_assert(IsEqual<typename EvalTraits<View1>::Eval,Eval>::value,
				  "'Add' operands must evaluate to the same type");
	static_assert(IsEqual<typename EvalTraits<View2>::Eval,Eval>::value,
				  "'Add' operands must evaluate to the same type");
*/
	BndViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) {}
	Eval min() const
	{ return detail::addLb<Eval>(p1.min(),p2.min()); }
	Eval max() const
	{ return detail::addUb<Eval>(p1.max(),p2.max()); }
	Void range(Eval& l, Eval& u) const
	{	l = min(); u = max();	}
	Bool updateRange(Eval l, Eval u)
	{	return updateMin(l) and updateMax(u);	}
	Bool updateMin(const Eval& v)
	{	return p1.updateMin(detail::subLb<Eval>(v,p2.max())) and
			   p2.updateMin(detail::subLb<Eval>(v,p1.max()));}
	Bool updateMax(const Eval& v)
	{ 	return p1.updateMax(detail::subUb<Eval>(v,p2.min())) and
			   p2.updateMax(detail::subUb<Eval>(v,p1.min()));}
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Add,View1,View2> getView()  const
	{ 	return Rel2<Add,View1,View2>(p1.getView(),p2.getView());	}

	BndView<Eval,View1>	p1;
	BndView<Eval,View2>	p2;
};


/**
 *	BndView over subtraction.
 *	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct BndViewRel2<Sub,View1,View2,Eval>
{
//	typedef typename EvalTraits<View1>::Eval	View1Eval;
//	typedef typename EvalTraits<View2>::Eval	View2Eval;

	BndViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) {}
	Eval min() const { return detail::subLb(p1.min(),p2.max()); }
	Eval max() const { return detail::subUb(p1.max(),p2.min()); }
	Void range(Eval& l, Eval& u) const
	{	l = min(); u = max();	}
	Bool updateMin(const Eval& v)
	{ return p1.updateMin(detail::addLb(v,p2.min())) &&
			 p2.updateMax(detail::subUb(p1.max(),v)); }
	Bool updateMax(const Eval& v)
	{ return p1.updateMax(detail::addUb(v,p2.max())) &&
			 p2.updateMin(detail::subLb(p1.min(),v)); }
	Bool updateRange(const Eval& lb, const Eval& ub)
	{ return p1.updateRange(detail::addLb(lb,p2.min()),
							detail::addUb(ub,p2.max())) and
			 p2.updateRange(detail::subLb(p1.min(),ub),
							detail::subUb(p1.max(),lb));
	}
	void attach(INotifiable* f)	{ 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

#if 1
	Rel2<Sub,View1,View2> getView()  const
	{ 	return Rel2<Sub,View1,View2>(p1.getView(),p2.getView());	}
#else
	Rel2<Sub,typename BndView<Eval,View1>::Viewed,typename BndView<Eval,View2>::Viewed> getView()  const
	{ 	return Rel2<Sub,typename BndView<Eval,View1>::Viewed,typename BndView<Eval,View2>::Viewed>(p1.getView(),p2.getView());	}
#endif
	BndView<Eval,View1>	p1;
	BndView<Eval,View2>	p2;
};


#if 1
template<class Eval,class View>
struct BndViewRel3<InRange,View,Eval,Eval,Eval>
{
	typedef BndViewRel3<InRange,View,Eval,Eval,Eval> Self;

	BndViewRel3(ICPSolver& solver,const View& p,Eval lb,Eval ub) :
		v(solver,p),lb(lb),ub(ub) {}

	Eval min() const
	{	return std::max(v.min(),lb);	}
	Eval max() const
	{	return std::min(v.max(),ub);	}
	Void range(Eval& l, Eval& u) const
	{
		v.range(l,u);
		l = std::max(l,lb);
		u = std::min(u,ub);
	}

	Bool updateMin(const Eval& m)
	{	return m <= ub and v.updateMin(std::max(lb,m));	}
	Bool updateMax(const Eval& m)
	{	return m >= lb and v.updateMax(std::min(ub,m));	}

	Bool updateRange(const Eval& l, const Eval& u)
	{	return u >= lb and l <= ub and v.updateRange(std::max(lb,l),std::min(ub,u));	}

	void attach(INotifiable* f) { 	v.attach(f);	}
	void detach(INotifiable* f) {	v.detach(f);	}

	Rel3<InRange,View,Eval,Eval> getView()  const
	{ 	return Rel3<InRange,View,Eval,Eval>(v.getView(),lb,ub);	}

	BndView<Eval,View>			v;
	const Eval	lb;
	const Eval	ub;
};
#else
template<class Eval,class View>
struct BndViewRel3<InRange,View,Eval,Eval,Eval>
{
	typedef BndViewRel3<InRange,View,Eval,Eval,Eval> Self;

	BndViewRel3(ICPSolver& solver,const View& p,Eval lb,Eval ub) : v(solver,p),
		lb(solver,std::max(lb,v.min())),ub(solver,std::min(ub,v.max())) {}

	Eval min() const
	{
		lb = std::max(v.min(),lb.get());
		return lb;
	}
	Eval max() const
	{
		ub = std::min(v.max(),ub.get());
		return ub;
	}
	Void range(Eval& l, Eval& u) const
	{
		v.range(l,u);
		l = std::max(l,lb.get());
		u = std::min(u,ub.get());
		lb = l;
		ub = u;
	}

	Bool updateMin(const Eval& m)
	{
		if (m > ub)
			return false;
		if (m <= lb)
			return true;
		if (!v.updateMin(m))
			return false;
		if (min() < m)
			lb.solver().post(v.getView() >= m);
		lb = m;
		return true;
	}
	Bool updateMax(const Eval& m)
	{
		if (m < lb)
			return false;
		if (m >= ub)
			return true;
		if (!v.updateMax(m))
			return false;
		if (max() > m)
			lb.solver().post(v.getView() <= m);
		ub = m;
		return true;
	}

	Bool updateRange(const Eval& l, const Eval& u)
	{
		if (u < lb or l > ub)
			return false;
		if (l <= lb and u >= ub)
			return true;
		if (!v.updateRange(l,u))
			return false;
		if (min() < l)
			lb.solver().post(v.getView() >= l);
		if (max() > u)
			lb.solver().post(v.getView() <= u);
		lb = l;
		ub = u;
		return true;
	}

	void attach(INotifiable* f) { 	v.attach(f);	}
	void detach(INotifiable* f) {	v.detach(f);	}

	View getView()  const
	{ 	return v.getView();	}

	BndView<Eval,View>			v;
	mutable Reversible<Eval>	lb;
	mutable Reversible<Eval>	ub;
};
#endif

/**
 *	BndView over multiplication.
 *	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct BndViewRel2<Mul,View1,View2,Eval>
{
	BndViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) {}

	inline Eval min() const;
	inline Eval max() const;
	Void range(Eval& min, Eval& max) const;

	Bool updateMin(const Eval& m);
	Bool updateMax(const Eval& m);

	Bool updateRange(const Eval& min, const Eval& max);

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<Mul,View1,View2> getView()  const
	{ 	return Rel2<Mul,View1,View2>(p1.getView(),p2.getView());	}

	BndView<Eval,View1>	p1;
	BndView<Eval,View2>	p2;
};

namespace detail {

template<class T>
struct TermElemTraits
{	typedef typename EvalTraits<T>::Eval Eval;	};

template<class T>
struct TermElemTraits<Range<T> >
{	typedef T	Eval;	};

template<class P1,class P2,class Eval>
Void mulRange(const P1& p1, const P2& p2,Eval& lb, Eval& ub)
{
	using detail::mulUb;
	using detail::mulLb;
	using detail::isNeg;
	using detail::isPos;

	Eval xl,xu;
	Eval yl,yu;

	p1.range(xl,xu);
	p2.range(yl,yu);

	if (isPos(xl))	// P * X
	{
		if (isPos(yl))	// P * P
		{ lb = mulLb(xl,yl); ub = mulUb(xu,yu); }
		else
		if (isNeg(yu)) // P * N
		{ lb = mulLb(xu,yl); ub = mulUb(xl,yu);	}
		else	// P * X
		{ lb = mulLb(xu,yl); ub = mulUb(xu,yu);	}
	}
	else
	if (isNeg(xu))	// N * X
	{
		if (isNeg(yu))	// N * N
		{ lb = mulLb(xu,yu); ub = mulUb(xl,yl);	}
		else
		if (isPos(yl))	// N * P
		{ lb = mulLb(xl,yu); ub = mulUb(xu,yl);	}
		else //  N * X
		{ lb = mulLb(xl,yu); ub = mulUb(xl,yl);	}
	}
	else
	if (isPos(yl))	// X * P
	{	lb = mulLb(yu,xl); ub = mulUb(yu,xu);	}
	else
	if (isNeg(yu))	// X * N
	{	lb = mulLb(yl,xu); ub = mulUb(yl,xl);	}
	else	// X * X
	{
		lb = std::min(mulLb(xl,yu),mulLb(xu,yl));
		ub = std::max(mulUb(xl,yl),mulUb(xu,yu));
	}
}

};

template<class View1,class View2,class Eval>
Void BndViewRel2<Mul,View1,View2,Eval>::range(Eval& lb, Eval& ub) const
{	
	lb = limits<Eval>::negInf();
	ub = limits<Eval>::posInf();
	detail::mulRange(p1,p2,lb,ub);	
}

template<class View1,class View2,class Eval>
inline Eval BndViewRel2<Mul,View1,View2,Eval>::min() const
{
	using detail::mulLb;
	using detail::isNeg;
	using detail::isPos;

	Eval xl,xu;
	Eval yl,yu;

	p1.range(xl,xu);
	p2.range(yl,yu);

	if (isPos(xl))	// P * X
	{
		if (isPos(yl))	// P * P
			return mulLb(xl,yl);
		else // P * N or P * X
			return mulLb(xu,yl);
	}
	else
	if (isNeg(xu))	// N * X
	{
		if (isNeg(yu))	// N * N
			return mulLb(xu,yu);
		else // N * P or N * X
			return mulLb(xl,yu);
	}
	else
	if (isPos(yl))	// X * P
		return mulLb(yu,xl);
	else
	if (isNeg(yu))	// X * N
		return mulLb(yl,xu);
	else	// X * X
	return std::min(mulLb(xl,yu),mulLb(xu,yl));
}

template<class View1,class View2,class Eval>
inline Eval BndViewRel2<Mul,View1,View2,Eval>::max() const
{
	using detail::mulUb;
	using detail::isNeg;
	using detail::isPos;

	Eval xl,xu;
	Eval yl,yu;

	p1.range(xl,xu);
	p2.range(yl,yu);

	if (isPos(xl))	// P * X
	{
		if (isNeg(yu)) // P * N
			return mulUb(xl,yu);
		else 	// P * P or P * X
			return mulUb(xu,yu);
	}
	else
	if (isNeg(xu))	// N * X
	{
		if (isPos(yl)) // N * P
			return mulUb(xu,yl);
		else	// N * N or N * X
			return mulUb(xl,yl);
	}
	else
	if (isPos(yl))	// X * P
		return mulUb(yu,xu);
	else
	if (isNeg(yu))	// X * N
		return mulUb(yl,xl);
	else	// X * X
	return std::max(mulUb(xl,yl),mulUb(xu,yu));
}

namespace detail {
template<class P1,class P2,class Eval> void divRange(const P1&,const P2&,
													 Eval&,Eval&);
template<class Eval,class P1,class P2> Eval divMin(const P1&,const P2&);
template<class Eval,class P1,class P2> Eval divMax(const P1&,const P2&);

};

template<class T> struct Range;

template<class View1,class View2,class Eval>
Bool BndViewRel2<Mul,View1,View2,Eval>::updateMin(const Eval& zl)
{
	using detail::divLb;
	using detail::divUb;
	using detail::isNeg;
	using detail::isPos;

	Eval xl,xu;
	Eval yl,yu;

	p1.range(xl,xu);
	p2.range(yl,yu);

	if (zl > 0) // ? * ? = P
	{
		if (isPos(xl))	// P * ?
		{
			if (isPos(yl) or !isNeg(yu))	// P * P or P * X
				return p1.updateMin(divLb(zl,yu)) and p2.updateMin(divLb(zl,xu));
			else
				return false;
		}
		else
		if (isNeg(xu))	// N * ?
		{
			if (isNeg(yu) or !isPos(yl))	// N * N or N * X	[OK]
				return p1.updateMax(divUb(zl,yl)) and p2.updateMax(divUb(zl,xl));
			else
				return false;
		}
		else		// X * ?
		{
			if (isPos(yl))	// X * P
				return p2.updateMin(divLb(zl,xu)) and p1.updateMin(divLb(zl,yu));
			else
			if (isNeg(yu))	// X * N
				return p2.updateMax(divUb(zl,xl)) and p1.updateMax(divUb(zl,yl));
		}
	}
	else
	if (zl <= 0) // ? * ? = X
	{
		if (isPos(xl))	// P * ?
		{
			if (isNeg(yu))	// P * N
				return p1.updateMax(divUb(zl,yu)) and p2.updateMin(divLb(zl,xl));
			else
			if (!isPos(yl))	// P * X
				return p2.updateMin(divLb(zl,xl));
		}
		else
		if (isNeg(xu))	// N * ?
		{
			if (isPos(yl))	// N * P
				return p2.updateMax(divUb(zl,xu)) and p1.updateMin(divLb(zl,yl));
			else
			if (!isNeg(yu))	// N * X
				return p2.updateMax(divUb(zl,xu));
		}
		else		// X * ?
		{
			if (isPos(yl))	// X * P
				return p1.updateMin(divLb(zl,yl));
			else
			if (isNeg(yu))	// X * N
				return p1.updateMax(divUb(zl,yu));
		}
	}
	return true;
}

template<class View1,class View2,class Eval>
Bool BndViewRel2<Mul,View1,View2,Eval>::updateMax(const Eval& zu)
{
	using detail::divLb;
	using detail::divUb;
	using detail::isNeg;
	using detail::isPos;

	Eval xl,xu;
	Eval yl,yu;

	p1.range(xl,xu);
	p2.range(yl,yu);

	if (zu >= 0) // ? * ? = X
	{
		if (isPos(xl))	// P * ?
		{
			if (isPos(yl))	// P * P
				return p1.updateMax(divUb(zu,yl)) and p2.updateMax(divUb(zu,xl));
			else
			if (!isNeg(yu))	// P * X
				return p2.updateMax(divUb(zu,xl));
		}
		else
		if (isNeg(xu))	// N * ?
		{
			if (isNeg(yu))	// N * N
				return p1.updateMin(divLb(zu,yu)) and p2.updateMin(divLb(zu,xu));
			else
			if (!isPos(yl))	// N * X
				return p2.updateMin(divLb(zu,xu));
		}
		else		// X * ?
		{
			if (isPos(yl))	// X * P
				return p1.updateMax(divUb(zu,yl));
			else
			if (isNeg(yu))	// X * N
				return p1.updateMin(divLb(zu,yu));
		}
	}
	else
	if (zu < 0) // ? * ? = N
	{
		if (isPos(xl))	// P * ?
		{
			if (isNeg(yu) or !isPos(yl))	// P * N or P * X
				return p1.updateMin(divLb(zu,yl)) and p2.updateMax(divUb(zu,xu));
			else	// P * P
				return false;
		}
		else
		if (isNeg(xu))	// N * ?
		{
			if (isPos(yl) or !isNeg(yu))	// N * P or N * X
				return p2.updateMin(divLb(zu,xl)) and p1.updateMax(divUb(zu,yu));
			else	// N * N
				return false;
		}
		else		// X * ?
		{
			if (isPos(yl))	// X * P
				return p2.updateMin(divLb(zu,xl)) and p1.updateMax(divUb(zu,yu));
			else
			if (isNeg(yu))	// X * N
				return p1.updateMin(divLb(zu,yl)) and p2.updateMax(divUb(zu,xu));
		}
	}
	return true;
}

template<class View1,class View2,class Eval>
Bool BndViewRel2<Mul,View1,View2,Eval>::updateRange(const Eval& zl,
													const Eval& zu)
{
	using detail::divLb;
	using detail::divUb;
	using detail::isNeg;
	using detail::isPos;

	Eval xl,xu;
	Eval yl,yu;

	p1.range(xl,xu);
	p2.range(yl,yu);

	if (zl > 0) // ? * ? = P
	{
		if (isPos(xl))	// P * ?
		{
			if (isPos(yl))	// P * P
				return p1.updateRange(divLb(zl,yu),divUb(zu,yl)) and
					   p2.updateRange(divLb(zl,xu),divUb(zu,xl));
			else
			if (isNeg(yu))	// P * N
				return false;
			else			// P * X
				return p1.updateMin(divLb(zl,yu)) and
					   p2.updateRange(divLb(zl,xu),divUb(zu,xl));
		}
		else
		if (isNeg(xu))	// N * ?
		{
			if (isPos(yl))	// N * P
				return false;
			else
			if (isNeg(yu))	// N * N
				return p1.updateRange(divLb(zu,yu),divUb(zl,yl)) and
					   p2.updateRange(divLb(zu,xu),divUb(zl,xl));
			else			// N * X	[OK]
				return p1.updateMax(divUb(zl,yl)) and
					   p2.updateRange(divLb(zu,xu),divUb(zl,xl));
		}
		else		// X * ?
		{
			if (isPos(yl))	// X * P
				return p2.updateMin(divLb(zl,xu)) and
					   p1.updateRange(divLb(zl,yu),divUb(zu,yl));
			else
			if (isNeg(yu))	// X * N
				return p2.updateMax(divUb(zl,xl)) and
					   p1.updateRange(divLb(zu,yu),divUb(zl,yl));
			else			// X * X
				return true;
		}
	}
	else
	if (zu < 0) // ? * ? = N
	{
		if (isPos(xl))	// P * ?
		{
			if (isPos(yl))	// P * P
				return false;
			else
			if (isNeg(yu))	// P * N
				return p1.updateRange(divLb(zu,yl),divUb(zl,yu)) and
					   p2.updateRange(divLb(zl,xl),divUb(zu,xu));
			else			// P * X
				return p1.updateMin(divLb(zu,yl)) and
					   p2.updateRange(divLb(zl,xl),divUb(zu,xu));
		}
		else
		if (isNeg(xu))	// N * ?
		{
			if (isPos(yl))	// N * P
				return p2.updateRange(divLb(zu,xl),divUb(zl,xu)) and
					   p1.updateRange(divLb(zl,yl),divUb(zu,yu));
			else
			if (isNeg(yu))	// N * N
				return false;
			else			// N * X
				return p2.updateRange(divLb(zu,xl),divUb(zl,xu)) and
					   p1.updateMax(divUb(zu,yu));
		}
		else		// X * ?
		{
			if (isPos(yl))	// X * P
				return p2.updateMin(divLb(zu,xl)) and
					   p1.updateRange(divLb(zl,yl),divUb(zu,yu));
			else
			if (isNeg(yu))	// X * N
				return p1.updateRange(divLb(zu,yl),divUb(zl,yu)) and
					   p2.updateMax(divUb(zu,xu));
			else			// X * X
				return true;
		}
	}
	else		// ? * ? = X
	{
		if (isPos(xl))	// P * ?
		{
			if (isPos(yl))	// P * P
				return p1.updateMax(divUb(zu,yl)) and p2.updateMax(divUb(zu,xl));
			else
			if (isNeg(yu))	// P * N
				return p1.updateMax(divUb(zl,yu)) and p2.updateMin(divLb(zl,xl));
			else			// P * X
				return p2.updateRange(divLb(zl,xl),divUb(zu,xl));
		}
		else
		if (isNeg(xu))	// N * ?
		{
			if (isPos(yl))	// N * P	[OK]
				return p2.updateMax(divUb(zl,xu)) and p1.updateMin(divLb(zl,yl));
			else
			if (isNeg(yu))	// N * N	[OK]
				return p1.updateMin(divLb(zu,yu)) and p2.updateMin(divLb(zu,xu));
			else			// N * X	[OK]
				return p2.updateRange(divLb(zu,xu),divUb(zl,xu));
		}
		else		// X * ?
		{
			if (isPos(yl))	// X * P
				return p1.updateRange(divLb(zl,yl),divUb(zu,yl));
			else
			if (isNeg(yu))	// X * N
				return p1.updateRange(divLb(zu,yu),divUb(zl,yu));
			else			// X * X
				return true;
		}
	}
}

#if 0
template<class View1,class View2,class Eval>
Bool BndViewRel2<Mul,View1,View2,Eval>::updateRange(const Eval& lb,
													const Eval& ub)
{
	Range<Eval> r(lb,ub);
	Eval l1,u1;
	p1.range(l1,u1);
	if (!detail::IsIntegral<Eval>::value or lb*ub>0 or p2.min()*p2.max()>0)
	{
		if (p2.min()==0 and p2.max()==0)
			return false;
		detail::divRange(r,p2,l1,u1);
		if (!p1.updateRange(l1,u1))
			return false;
	}
	Eval l2,u2;
	p2.range(l2,u2);
	if (!detail::IsIntegral<Eval>::value or lb*ub>0 or p1.min()*p1.max()>0)
	{
		if (p1.min()==0 and p1.max()==0)
			return false;
		detail::divRange(r,p1,l2,u2);
		if (!p2.updateRange(l2,u2))
			return false;
	}
	return true;
}
#endif

/**
 *	BndView over division.
 *	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct BndViewRel2<Div,View1,View2,Eval>
{
//	typedef typename EvalTraits<View1>::Eval	View1Eval;
//	typedef typename EvalTraits<View2>::Eval	View2Eval;

	BndViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) /*,
		r(solver)*/ {}

	Eval min() const;
	Eval max() const;
	Void range(Eval& min, Eval& max) const;

	// these could be better ...
	Bool updateMin(const Eval& m) { return updateRange(m,max());}
	Bool updateMax(const Eval& m) { return updateRange(min(),m);}

	Bool updateRange(const Eval& min, const Eval& max);

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<Div,View1,View2> getView()  const
	{ 	return Rel2<Div,View1,View2>(p1.getView(),p2.getView());	}

	BndView<Eval,View1> 		p1;
	BndView<Eval,View2> 		p2;
};

namespace detail {

// Continuous case. TODO
template<class Eval>
struct DivRange;

template<class P1,class P2,class Eval>
Void divRange(const P1& p1, const P2& p2, Eval& lb, Eval& ub)
{	DivRange<Eval>()(p1,p2,lb,ub);	}

// NOTE: lb and ub are also IN parameters (will be used for pruning
// in cases the division evaluates to two intervals).
// If the denominator is the singleton [0] it raises an exception,
// otherwise this implementation is consistent with what is said here:
// http://grouper.ieee.org/groups/1788/
template<class Eval>
struct DivRange
{
	template<class P1,class P2>
	Void operator()(const P1& p1, const P2& p2, Eval& lb, Eval& ub)
	{
		using detail::isPos;
		using detail::isNeg;
		using detail::divLb;
		using detail::divUb;

		const Eval zero = static_cast<Eval>(0);

		Eval a1,a2;
		Eval b1,b2;

		p1.range(a1,a2);
		p2.range(b1,b2);

		if (isPos(b1))	// ? / [+,+]
		{
			if (!isNeg(a1))	// [0+,+] / [+,+]
			{
				lb = divLb(a1,b2);
				ub = divUb(a2,b1);
			}
			else
			if (!isPos(a2))	// [-,-0] / [+,+]
			{
				lb = divLb(a1,b1);
				ub = divUb(a2,b2);
			}
			else
			{				// [-,+] / [+,+]
				lb = divLb(a1,b1);
				ub = divUb(a2,b1);
			}
		}
		else
		if (isNeg(b2))	// ? / [-,-]
		{
			if (!isNeg(a1))	// [0+,+] / [-,-]
			{
				lb = divLb(a2,b2);
				ub = divUb(a1,b1);
			}
			else
			if (!isPos(a2))	// [-,-0] / [-,-]
			{
				lb = divLb(a2,b1);
				ub = divUb(a1,b2);
			}
			else
			{				// [-,+] / [-,-]
				lb = divLb(a2,b2);
				ub = divUb(a1,b2);
			}
		}
		else	// ? / [-0,0+]
		{
			if (b1==zero)	// ? / [0,0+]
			{
				if (b2==zero) // ? / [0]
					throw EDivisionByZero();
				if (isNeg(a2)) // [-,-] / [0,0+]
					ub = divUb(a2,b2);
				else
				if (isPos(a1)) // [+,+] / [0,0+]
					lb = divLb(a1,b2);
			}
			else
			if (b2==zero)	// ? / [-0,0]
			{
				if (isNeg(a2)) // [-,-] / [0,0+]
					lb = divLb(a2,b1);
				else
				if (isPos(a1)) // [+,+] / [0,0+]
					ub = divUb(a1,b1);
			}
			else	// ? / [-,+]
			{
				if (isNeg(a2))	// [-,-] / [-,+]
				{
					if (divLb(a2,b1)>ub)
						ub = divUb(a2,b2);
					if (divUb(a2,b2)<lb)
						lb = divLb(a2,b1);
				}
				else
				if (isPos(a1))	// [+,+] / [-,+]
				{
					if (divUb(a1,b1)<lb)
						lb = divLb(a1,b2);
					if (divLb(a1,b2)>ub)
						ub = divUb(a1,b1);
				}
			}
		}
	}
};
}

template<class View1,class View2,class Eval>
Eval BndViewRel2<Div,View1,View2,Eval>::min() const
{
	using detail::isPos;
	using detail::isNeg;
	using detail::divLb;
	using detail::divUb;

	const Eval zero = static_cast<Eval>(0);

	Eval a1,a2;
	Eval b1,b2;

	p1.range(a1,a2);
	p2.range(b1,b2);

	if (isPos(b1))	// ? / [+,+]
	{
		if (!isNeg(a1))	// [0+,+] / [+,+]
			return divLb(a1,b2);
		else
		if (!isPos(a2))	// [-,-0] / [+,+]
			return divLb(a1,b1);
		else
			return divLb(a1,b1);	// [-,+] / [+,+]
	}
	else
	if (isNeg(b2))	// ? / [-,-]
	{
		if (!isNeg(a1))	// [0+,+] / [-,-]
			return divLb(a2,b2);
		else
		if (!isPos(a2))	// [-,-0] / [-,-]
			return divLb(a2,b1);
		else
			return divLb(a2,b2); // [-,+] / [-,-]
	}
	else	// ? / [-0,0+]
	{
		if (b1==zero)	// ? / [0,0+]
		{
			if (b2==zero) // ? / [0]
				throw EDivisionByZero();
			if (isPos(a1)) // [+,+] / [0,0+]
				return divLb(a1,b2);
		}
		else
		if (b2==zero)	// ? / [-0,0]
		{
			if (isNeg(a2)) // [-,-] / [0,0+]
				return divLb(a2,b1);
		}
	}
	return limits<Eval>::negInf();
}

template<class View1,class View2,class Eval>
Eval BndViewRel2<Div,View1,View2,Eval>::max() const
{
	using detail::isPos;
	using detail::isNeg;
	using detail::divLb;
	using detail::divUb;

	const Eval zero = static_cast<Eval>(0);

	Eval a1,a2;
	Eval b1,b2;

	p1.range(a1,a2);
	p2.range(b1,b2);

	if (isPos(b1))	// ? / [+,+]
	{
		if (!isNeg(a1))	// [0+,+] / [+,+]
			return divUb(a2,b1);
		else
		if (!isPos(a2))	// [-,-0] / [+,+]
			return divUb(a2,b2);
		else
			return divUb(a2,b1); // [-,+] / [+,+]
	}
	else
	if (isNeg(b2))	// ? / [-,-]
	{
		if (!isNeg(a1))	// [0+,+] / [-,-]
			return divUb(a1,b1);
		else
		if (!isPos(a2))	// [-,-0] / [-,-]
			return divUb(a1,b2);
		else
			return divUb(a1,b2); // [-,+] / [-,-]
	}
	else	// ? / [-0,0+]
	{
		if (b1==zero)	// ? / [0,0+]
		{
			if (b2==zero) // ? / [0]
				throw EDivisionByZero();
			if (isNeg(a2)) // [-,-] / [0,0+]
				return divUb(a2,b2);
		}
		else
		if (b2==zero)	// ? / [-0,0]
		{
			if (isPos(a1)) // [+,+] / [0,0+]
				return divUb(a1,b1);
		}
	}
	return limits<Eval>::posInf();
}


template<class View1,class View2,class Eval>
Void BndViewRel2<Div,View1,View2,Eval>::range(Eval& lb, Eval& ub) const
{
	lb = limits<Eval>::negInf();
	ub = limits<Eval>::posInf();
	detail::divRange(p1,p2,lb,ub);
}


template<class View1,class View2,class Eval>
Bool BndViewRel2<Div,View1,View2,Eval>::updateRange(const Eval& lb,
													const Eval& ub)
{
	Range<Eval> r(lb,ub);
	Eval mmin,mmax;
	range(mmin,mmax);
	if (lb>mmax or ub<mmin)
		return false;
	if (lb<=mmin and ub>=mmax)
		return true;

	Eval l1,u1;
	p1.range(l1,u1);
	detail::mulRange(p2,r,l1,u1);

	if (!p1.updateRange(l1,u1))
		return false;

	Eval l2,u2;
	p2.range(l2,u2);
	detail::divRange(p1,r,l2,u2);

	if (!p2.updateRange(l2,u2))
		return false;

	return true;
}

/**
 *	BndView over mod(x,y).
 *  \warning Works with positive intervals only.
 *  \note Does not compute the tightest interval. I don't think
 *  the smallest interval can be obtained in constant time.
 *  \todo Generalize to negative intervals.
 *	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct BndViewRel2<Mod,View1,View2,Eval>
{
	BndViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		p1(solver,p1),p2(solver,p2) {}
	Eval min() const;
	Eval max() const;
	Bool updateMin(const Eval& v);
	Bool updateMax(const Eval& v);
	Void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	Bool updateRange(const Eval& l, const Eval& u)
	{	return updateMin(l) and updateMax(u);	}
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f); }
	Rel2<Mod,View1,View2> getView()  const
	{ 	return Rel2<Mod,View1,View2>(p1.getView(),p2.getView());	}

	BndView<Eval,View1>	p1;
	BndView<Eval,View2>	p2;
};

template<class View1,class View2,class Eval>
Eval BndViewRel2<Mod,View1,View2,Eval>::min() const
{
	assert(p1.min()>=0);
	assert(p2.min()>0);

	Eval Dmin = p1.min();
	Eval Dmax = p1.max();

	Eval dmin = p2.min();
	Eval dmax = p2.max();

	return std::max(
			static_cast<Eval>(0),
			static_cast<Eval>(Dmin-
								(Dmax/dmin)*dmax));
}

template<class View1,class View2,class Eval>
Eval BndViewRel2<Mod,View1,View2,Eval>::max() const
{
	assert(p1.min()>=0);
	assert(p2.min()>0);

	Eval Dmin = p1.min();
	Eval Dmax = p1.max();

	Eval dmin = p2.min();
	Eval dmax = p2.max();

	Eval qmin = Dmin/dmax;
	Eval qmax = Dmax/dmin;

	// find a better dmin fast
	Eval bdmin = static_cast<Eval>(std::ceil(static_cast<Double>(Dmin+1)/(qmin+1)));
	// use only when is safe
	if (dmin>=qmax)	// FIXME: could it be bdmin>=qmax?
		dmin = std::max(dmin,bdmin);

	assert(dmin<=dmax);

	//if (Dmax/(qmin+1)>dmin and Dmax/(qmin+1)<dmax)
	//	return std::min(Dmax*(1-qmin)/(qmin+1),dmax-1);

	return std::min(Dmax-qmin*dmin,dmax-1);
}

template<class View1,class View2,class Eval>
Bool BndViewRel2<Mod,View1,View2,Eval>::updateMin(const Eval& v)
{
	Eval rmin = min();
	Eval rmax = max();

	if (v <= rmin)
		return true;
	if (v > rmax)
		return false;

	Eval Dmin = p1.min();
	Eval Dmax = p1.max();

	Eval dmin = p2.min();
	Eval dmax = p2.max();

	const Eval lq = std::ceil(static_cast<Double>(Dmin-rmax)/dmax);
	const Eval rq = std::floor(static_cast<Double>(Dmax-v)/dmin);

	Bool r = p2.updateMin(v+1);

	r &= p1.updateMin(lq*dmin + v) and
		 p1.updateMax(rq*dmax+rmax);

	if (rq != 0)
		r &= p2.updateMin((Dmin-rmax)/rq);

	if (lq != 0)
		r &= p2.updateMax((Dmax-v)/lq);

	return r;
}

template<class View1,class View2,class Eval>
Bool BndViewRel2<Mod,View1,View2,Eval>::updateMax(const Eval& v)
{
	Eval rmin = min();
	Eval rmax = max();

	if (v >= rmax)
		return true;
	if (v < rmin)
		return false;

	Eval Dmin = p1.min();
	Eval Dmax = p1.max();

	Eval dmin = p2.min();
	Eval dmax = p2.max();

	const Eval lq = std::ceil(static_cast<Double>(Dmin-v)/dmax);
	const Eval rq = std::floor(static_cast<Double>(Dmax-min())/dmin);

	Bool r = p1.updateMin(lq*dmin+rmin) and
		 	 p1.updateMax(rq*dmax+v);
	if (rq != 0)
		r &= p2.updateMin((Dmin-v)/rq);
	if (lq != 0)
		r &= p2.updateMax((Dmax-rmin)/lq);
	return r;
}

/**
 *	BndView over absolute value.
 *	\ingroup Views
 **/
template<class View1,class Eval>
struct BndViewRel1<Abs,View1,Eval>
{
	BndViewRel1(ICPSolver& solver, const View1& p1) :
		p1(solver,p1) {}
	Eval min() const
	{
		if (p1.min() > 0)
			return p1.min();
		else
		if (p1.max() < 0)
			return -p1.max();
		else
			return 0;
	}
	Eval max() const
	{	return std::max((Eval)p1.max(),(Eval)-p1.min());	}

	Bool updateMin(const Eval& v)
	{
		if (v <= 0)
			return true;
		if (p1.min() >= 0)
			return p1.updateMin(v);
		if (p1.max() <= 0)
			return p1.updateMax(-v);
		return true;
	}
	Bool updateMax(const Eval& v)
	{
		if (v < 0)
			return false;
		return p1.updateRange(-v,v);
	}
	Void range(Eval& l,Eval& u) const
	{
		Eval mi,ma;
		p1.range(mi,ma);
		if (mi>0)
		{	l = mi;	u = ma;	}
		else
		if (ma<0)
		{	l = -ma; u = -mi; }
		else
		{	l = 0; u = std::max(-mi,ma); }
	}
	Bool updateRange(const Eval& l, const Eval& u)
	{
		if (u < 0)
			return false;
		if (p1.min() >= 0)
			return p1.updateRange(std::max(l,0),u);
		if (p1.max() <= 0)
			return p1.updateRange(-u,std::min(-l,0));
		return p1.updateRange(-u,u);
	}

	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }
	Rel1<Abs,View1> getView()  const
	{ 	return Rel1<Abs,View1>(p1.getView());	}


	BndView<Eval,View1>	p1;
};

/**
 *	BndView over square.
 *	\ingroup Views
 **/
template<class View1,class Eval>
struct BndViewRel1<Sqr,View1,Eval>
{
//	typedef typename EvalTraits<View1>::Eval	View1Eval;

	BndViewRel1(ICPSolver& solver, const View1& p1) :
		p1(solver,p1) {}
	Eval min() const
	{
		const Eval pmin = p1.min();
		const Eval pmax = p1.max();
		if (pmin>0)
			return detail::sqrLb(pmin);
		else
		if (pmax<0)
			return detail::sqrLb(pmax);
		else
			return static_cast<Eval>(0);
	}
	Eval max() const
	{	return std::max(detail::sqrUb(p1.min()),detail::sqrUb(p1.max()));	}

	Bool updateMin(const Eval& v)
	{
		if (v <= 0)
			return true;
		const Eval r = detail::sqrtLb(v);
		if (p1.min() > -r)
			return p1.updateMin(r);
		else
		if (p1.max() < r)
			return p1.updateMax(-r);
		return true;
	}
	Bool updateMax(const Eval& v)
	{	return v >= 0 and p1.updateRange(-detail::sqrtUb(v),detail::sqrtUb(v));	}
	Void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	Bool updateRange(const Eval& l, const Eval& u)
	{	return updateMin(l) and updateMax(u);	}

	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }
	Rel1<Sqr,View1> getView()  const
	{ 	return Rel1<Sqr,View1>(p1.getView());	}

	BndView<Eval,View1>	p1;
};

/**
 *	BndView over exponential.
 *	\ingroup Views
 **/
template<class View1,class Eval>
struct BndViewRel1<Exp,View1,Eval>
{
//	typedef typename EvalTraits<View1>::Eval	View1Eval;

	BndViewRel1(ICPSolver& solver, const View1& p1) :
		p1(solver,p1) {}
	Eval min() const 	{	return detail::expLb<Eval>(p1.min());	}
	Eval max() const 	{	return detail::expUb<Eval>(p1.max());	}

	Bool updateMin(const Eval& v)
	{	return !detail::isPos(v) or detail::logLb<Eval>(v);	}
	Bool updateMax(const Eval& v)
	{	return p1.updateMax(detail::logUb<Eval>(v));	}
	Void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	Bool updateRange(const Eval& l, const Eval& u)
	{	return updateMin(l) and updateMax(u);	}

	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }
	Rel1<Exp,View1> getView()  const
	{ 	return Rel1<Exp,View1>(p1.getView());	}

	BndView<Eval,View1>	p1;
};

/**
 *	BndView over logarithm.
 *	\ingroup Views
 **/
template<class View1,class Eval>
struct BndViewRel1<Log,View1,Eval>
{
//	typedef typename EvalTraits<View1>::Eval	View1Eval;

	BndViewRel1(ICPSolver& solver, const View1& p1) :
		p1(solver,p1) {}
	Eval min() const
	{
		return !detail::isPos(p1.min()) ?
			limits<Eval>::negInf() :
					detail::logLb<Eval>(p1.min());
	}
	Eval max() const 	{	return detail::logUb<Eval>(p1.max());	}
	Bool updateMin(const Eval& v)
	{	return p1.updateMin(detail::expLb<Eval>(v));	}
	Bool updateMax(const Eval& v)
	{	return p1.updateMax(detail::expUb<Eval>(v));	}
	Void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	Bool updateRange(const Eval& l, const Eval& u)
	{	return updateMin(l) and updateMax(u);	}

	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }

	Rel1<Log,View1> getView()  const
	{ 	return Rel1<Log,View1>(p1.getView());	}

	BndView<Eval,View1>	p1;
};


/*
 *	Fake virtual calls
 */
template<class Eval>
struct IVirtualBndView
{
	virtual Eval min() const = 0;
	virtual Eval max() const = 0;
	virtual Bool updateMin(const Eval& v) = 0;
	virtual Bool updateMax(const Eval& v) = 0;
	virtual Void range(Eval& l,Eval& u) const = 0;
	virtual Bool updateRange(const Eval& l, const Eval& u) = 0;
	virtual void attach(INotifiable* f) = 0;
	virtual void detach(INotifiable* f) = 0;
};

template<class Eval,class View>
struct VirtualBndViewWrapper : IVirtualBndView<Eval>
{
	VirtualBndViewWrapper(ICPSolver& solver,const View& v) : p1(solver,v) {}
	Eval min() const { return p1.min(); }
	Eval max() const { return p1.max(); }
	Bool updateMin(const Eval& v) { return p1.updateMin(v); }
	Bool updateMax(const Eval& v) { return p1.updateMax(v); }
	Void range(Eval& l,Eval& u) const { p1.range(l,u);}
	Bool updateRange(const Eval& l, const Eval& u) { return p1.updateRange(l,u);}
	void attach(INotifiable* f) { p1.attach(f); }
	void detach(INotifiable* f) { p1.detach(f); }
	BndView<Eval,View> p1;
};

struct VirtualView;

template<class View1,class Eval>
struct BndViewRel1<VirtualView,View1,Eval>
{
	BndViewRel1(ICPSolver& solver, const View1& p1) :
		p1(new VirtualBndViewWrapper<Eval,View1>(solver,p1)),v1(p1) {}

	Eval min() const { return p1->min(); }
	Eval max() const { return p1->max(); }
	Bool updateMin(const Eval& v) { return p1->updateMin(v); }
	Bool updateMax(const Eval& v) { return p1->updateMax(v); }
	Void range(Eval& l,Eval& u) const { p1->range(l,u);}
	Bool updateRange(const Eval& l, const Eval& u) { return p1->updateRange(l,u);}
	void attach(INotifiable* f) { p1->attach(f); }
	void detach(INotifiable* f) { p1->detach(f); }

	Rel1<VirtualView,View1> getView()  const
	{ 	return Rel1<VirtualView,View1>(v1);	}
	IVirtualBndView<Eval>*	p1;
	View1 v1;
};

template<class Eval,class View>
std::ostream& operator<<(std::ostream& os, const casper::BndView<Eval,View>& v)
{	return casper::detail::RangePrinter<Eval,casper::BndView<Eval,View> >()(os,v);	}


};


#endif // _H_CASPER_KERNEL_VIEW_BNDVIEW

namespace casper {
#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_KERNEL_MACRO_BNDVIEW_H
#define CASPER_KERNEL_MACRO_BNDVIEW_H
#include <casper/kernel/macro/bndview.h>
#endif
#endif
};
