 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_CP_VIEW_BND
#define _H_CASPER_CP_VIEW_BND

#include <casper/util/container/stdrange.h>
#include <casper/cp/view/common.h>
#include <casper/util/safeop.h>
#include <casper/util/io.h>
#include <casper/util/assertions.h>
#include <cmath>

namespace Casper {



namespace Traits {
template<class EvalT,class View>
struct GetEval<CP::BndView<EvalT,View> >
{	typedef EvalT	Type;	};
} // Traits

namespace CP {


struct Store;

#if 0
// converts a view to a checkview
template<class Eval,class View>
struct BndChkViewWrapper;

template<class> struct CChkView;
template<class> struct ChkView;

template<class View>
struct BndChkViewWrapper<bool,View>
{
	BndChkViewWrapper(Store& store,const View& v) :
		v(store,v) {}
	bool min() const
	{	return v.isTrue();	}
	bool max() const
	{	return v.canBeTrue();	}
	bool updateMin(const bool& val)
	{	return !val or v.isTrue() or v.setToTrue();	}
	bool updateMax(const bool& val)
	{	return val or !v.canBeTrue() or v.setToFalse();	}
	// TODO: improve:
	void range(bool& l,bool& u) const
	{	l = min(); u = max();	}
	bool updateRange(const bool& l, const bool& u)
	{	return updateMin(l) and updateMax(u);	}
	void attach(INotifiable* f) { v.attach(f);	}
	void detach(INotifiable* f) { v.detach(f);	}
	View getObj()  const { return v.getObj(); }

	CChkView<View>	v;
};


// specializations for bndview over boolean (reificable) expressions

/**
 * 	A BndView allows to access or update the bounds of an associated expression.
 * 	\ingroup BndViews
 **/
template<class Eval,class View>
struct BndView : BndChkViewWrapper<Eval,View>
{
	CASPER_ASSERT_CHKVIEW_EVAL(Eval)
	BndView(Store& store, const View& v) :
		BndChkViewWrapper<Eval,View>(store,v) {}
};

template<class F,class Expr1,class Eval>
struct BndViewRel1 : BndChkViewWrapper<Eval,Rel1<F,Expr1> >
{
	CASPER_ASSERT_CHKVIEW_EVAL(Eval)
	BndViewRel1(Store& store, const Expr1& v) :
		BndChkViewWrapper<Eval,Rel1<F,Expr1> >(store,rel<F>(v)) {}
};

template<class F,class Expr1,class Expr2,class Eval>
struct BndViewRel2 : BndChkViewWrapper<Eval,Rel2<F,Expr1,Expr2> >
{
	CASPER_ASSERT_CHKVIEW_EVAL(Eval)
	BndViewRel2(Store& store, const Expr1& v1, const Expr2& v2) :
		BndChkViewWrapper<Eval,Rel2<F,Expr1,Expr2> >(store,rel<F>(v1,v2)) {}
};

#else

template<class Eval,class R>
struct NoBndView
{
	NoBndView(Store& s,const R& r)
	{
		std::ostringstream os;
		os << r;
		throw Casper::Exception::UndefinedView(os.str().c_str(),"CP::BndView");
	}
	__attribute__((noreturn)) Eval min() const { assert(0); }
	__attribute__((noreturn)) Eval max() const { assert(0); }
	__attribute__((noreturn)) bool updateMin(const Eval& val) { assert(0); }
	__attribute__((noreturn)) bool updateMax(const Eval& val) { assert(0); }
	__attribute__((noreturn)) void range(Eval& v1,Eval& v2) const {	assert(0); }
	__attribute__((noreturn)) bool updateRange(const Eval& v1, const Eval& v2) {	assert(0);	}
	__attribute__((noreturn)) void attach(INotifiable*) { assert(0); }
	__attribute__((noreturn)) void detach(INotifiable*) { assert(0); }
	__attribute__((noreturn)) Eval getObj()  const { assert(0); }
};

template<class> struct CChkView;
template<class> struct ChkView;

template<class View>
struct NoBndView<bool,View>
{
	NoBndView(Store& store,const View& v) :
		v(store,v) {}
	bool min() const
	{	return v.isTrue();	}
	bool max() const
	{	return v.canBeTrue();	}
	bool updateMin(const bool& val)
	{	return !val or v.isTrue() or v.setToTrue();	}
	bool updateMax(const bool& val)
	{	return val or !v.canBeTrue() or v.setToFalse();	}
	// TODO: improve:
	void range(bool& l,bool& u) const
	{	l = min(); u = max();	}
	bool updateRange(const bool& l, const bool& u)
	{	return updateMin(l) and updateMax(u);	}
	void attach(INotifiable* f) { v.attach(f);	}
	void detach(INotifiable* f) { v.detach(f);	}
	View getObj()  const { return v.getObj(); }

	CChkView<View>	v;
};


/**
 * 	BndView over a generic view. View must be convertible to Eval.
 * 	\ingroup BndViews
 **/

template<class Eval,class View>
struct BndView : NoBndView<Eval,View>
{
	BndView(Store& s,const View& v) :
		NoBndView<Eval,View>(s,v) {}
};


template<class F,class Expr1,class Eval>
struct BndViewRel1 : NoBndView<Eval,Rel1<F,Expr1> >
{
	BndViewRel1(Store& s,const Expr1& v) :
		NoBndView<Eval,Rel1<F,Expr1> >(s,rel<F>(v)) {}
};

template<class F,class Expr1,class Expr2,class Eval>
struct BndViewRel2 :  NoBndView<Eval,Rel2<F,Expr1,Expr2> >
{
	BndViewRel2(Store& s,const Expr1& v1,const Expr2& v2) :
		NoBndView<Eval,Rel2<F,Expr1,Expr2> >(s,rel<F>(v1,v2)) {}
};

template<class F,class Expr1,class Expr2,class Expr3,class Eval>
struct BndViewRel3 :  NoBndView<Eval,Rel3<F,Expr1,Expr2,Expr3> >
{
	BndViewRel3(Store& s,const Expr1& v1,const Expr2& v2,const Expr3& v3) :
		NoBndView<Eval,Rel3<F,Expr1,Expr2,Expr3> >(s,rel<F>(v1,v2,v3)) {}
};

#endif


/**
 * 	BndView over a literal type. Literal
 *  must support <= and >= operations.
 * 	\ingroup BndViews
 **/
template<class Eval>
struct BndView<Eval,Eval>
{
	BndView(Store& store, const Eval& v) :
		v(v) {}

	Eval min() const { return v; }
	Eval max() const { return v; }
	bool updateMin(const Eval& val)
	{ return val <= min(); }
	bool updateMax(const Eval& val)
	{ return val >= max(); }
	void range(Eval& v1,Eval& v2) const
	{	v1=v2=v; }
	bool updateRange(const Eval& v1, const Eval& v2)
	{	return v1<=v and v<=v2;	}
//	Eval getMinInRange(Eval l,Eval u) const
//	{	return v;	}
//	Store& store() const {	return mSolver;	}
	void attach(INotifiable*) {}
	void detach(INotifiable*) {}
	Eval getObj()  const { return v; }

	const Eval	v;
};

/**
 * 	BndView over a uint literal type (for convenience).
 * 	\ingroup BndViews
 **/
template<>
struct BndView<int,uint>
{
	BndView(Store& store, const uint& v) :
		v(v) {}

	int min() const { return static_cast<int>(v); }
	int max() const { return static_cast<int>(v); }
	bool updateMin(const int& val)
	{ return val <= min(); }
	bool updateMax(const int& val)
	{ return val >= max(); }
	void range(int& v1,int& v2) const
	{	 v1 = v2 = static_cast<int>(v);	}
	bool updateRange(const int& v1, const int& v2)
	{	return v1<=static_cast<int>(v) and
			   static_cast<int>(v)<=v2;	}
	void attach(INotifiable*) {}
	void detach(INotifiable*) {}
	uint getObj()  const { return v; }
	const uint	v;
};




// TODO: reification here
template<class,class,class>			struct BndViewRel1;
template<class,class,class,class>	struct BndViewRel2;
template<class,class,class,class,class>	struct BndViewRel3;

/*
 *	BndView over a Rel1 relation -> defers to BndViewRel1
 **/
template<class Eval,class F,class View>
struct BndView<Eval,Rel1<F,View> > : BndViewRel1<F,View,Eval>
{
	CASPER_ASSERT_BNDVIEW_EVAL(Eval)

	BndView(Store& store, const Rel1<F,View>& r) :
		BndViewRel1<F,View,Eval>(store,r.p1) {}
	// not sure if the below constructor is ever used
	BndView(Store& store,const BndViewRel1<F,View,Eval>& v) :
		BndViewRel1<F,View,Eval>(v) {}
};

/*
 *	BndView over a Rel2 relation -> defers to BndViewRel2
 **/
template<class Eval,class F,class Expr1,class Expr2>
struct BndView<Eval,Rel2<F,Expr1,Expr2> > :
	BndViewRel2<F,Expr1,Expr2,Eval>
{
	CASPER_ASSERT_BNDVIEW_EVAL(Eval)

	BndView(Store& store, const Rel2<F,Expr1,Expr2>& r) :
		BndViewRel2<F,Expr1,Expr2,Eval>(store,r.p1,r.p2) {}
	// not sure if the below constructor is ever used
	BndView(Store& store, const BndViewRel2<F,Expr1,Expr2,Eval>& v) :
		BndViewRel2<F,Expr1,Expr2,Eval>(v) {}
};

/*
 *	BndView over a Rel3 relation -> defers to BndViewRel3
 **/
template<class Eval,class F,class Expr1,class Expr2,class Expr3>
struct BndView<Eval,Rel3<F,Expr1,Expr2,Expr3> > :
	BndViewRel3<F,Expr1,Expr2,Expr3,Eval>
{
	CASPER_ASSERT_BNDVIEW_EVAL(Eval)

	BndView(Store& store, const Rel3<F,Expr1,Expr2,Expr3>& r) :
		BndViewRel3<F,Expr1,Expr2,Expr3,Eval>(store,r.p1,r.p2,r.p3) {}
};

/*
 *	BndView over a BndExpr
 **/
template<class Eval>
struct BndView<Eval,BndExpr<Eval> >
{
	BndView(Store& store, const BndExpr<Eval>& r) :
		r(r) {}

	Eval min() const { return r.min(); }
	Eval max() const { return r.max(); }
	bool updateMin(const Eval& val)
	{ return r.updateMin(val); }
	bool updateMax(const Eval& val)
	{ return r.updateMax(val); }
	void range(Eval& v1,Eval& v2) const
	{ r.range(v1,v2);	 }
	bool updateRange(const Eval& v1, const Eval& v2)
	{	return r.updateRange(v1,v2);	}
//	Store& store() const	{	return r.store();	}
	void attach(INotifiable* n) {	return r.attach(n);	}
	void detach(INotifiable* n) {	return r.detach(n);	}
	BndExpr<Eval> 	getObj()  const { return r; }
	BndExpr<Eval>	r;
};

/*
 *	BndView over a DomExpr
 **/
template<class Eval,class Dom>
struct BndView<Eval,DomExpr<Eval,Dom> > :
	BndView<Eval,Var<Eval,Dom> >
{
	typedef BndView<Eval,Var<Eval,Dom> >	Super;
	BndView(Store& store, const DomExpr<Eval,Dom>& r) :
		BndView<Eval,Var<Eval,Dom> >(store,
					Var<Eval,Dom>(store,r.operator->())),store(store) {}
	DomExpr<Eval,Dom> getObj() const
	{	return DomExpr<Eval,Dom>(store,Super::getObj()); }
	Store& store;
};

#if 0
/**
 *	BndView over a DomView
 *	\ingroup Views
 **/
template<class Eval,class View,class Dom>
struct BndView<Eval,DomView<Eval,View,Dom> > : BndView<Eval,Var<Eval,Dom> >
{
	BndView(Store& store, const DomView<Eval,View,Dom>& r) :
		BndView<Eval,Var<Eval,Dom> >(store,Var<Eval,Dom>(store,r.operator->())) {}
};
#endif

// ******** conversion **********

/**
 * 	BndView over a type cast  expression.
 * 	\ingroup BndViews
 */
template<class View,class Eval>
struct BndViewRel1<Cast<Eval>,View,Eval>
{
	typedef typename Casper::Traits::GetEval<View>::Type	EvalFrom;

	BndViewRel1(Store& store, const View& v) :
		v(store,v) {}

	Eval min() const { return Util::convLb<Eval>(v.min()); }
	Eval max() const { return Util::convUb<Eval>(v.max()); }
	bool updateMin(const Eval& val)
	{ return v.updateMin(Util::convLb<EvalFrom>(val));	}
	bool updateMax(const Eval& val)
	{ return v.updateMax(Util::convUb<EvalFrom>(val));	}
	void range(Eval& v1,Eval& v2) const
	{ v1 = min(); v2=max(); }
	bool updateRange(const Eval& v1, const Eval& v2)
	{	return v.updateRange(Util::convLb<EvalFrom>(v1),
							 Util::convUb<EvalFrom>(v2));	}

	void attach(INotifiable* n) {	v.attach(n);	}
	void detach(INotifiable* n) {	v.detach(n);	}
	Rel1<Cast<Eval>,View> getObj()  const
	{ return Rel1<Cast<Eval>,View>(v.getObj()); }

	BndView<EvalFrom,View>	v;
};


/**
 *	BndView over an IfThenElse expression.
 *	\ingroup BndViews
 **/
template<class Eval,class Expr1,class Expr2,class Expr3>
struct BndViewRel3<IfThenElse,Expr1,Expr2,Expr3,Eval>
{
	BndViewRel3(Store& store, const Expr1& p1, const Expr2& p2, const Expr3& p3) :
		c1(store,p1),p2(store,p2),p3(store,p3)
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
	bool updateMin(const Eval& v)
	{
		if (c1.isTrue())
			return p2.updateMin(v);
		if (!c1.canBeTrue())
			return p3.updateMin(v);

		if (v > p2.max())
		{
			p2.detach(pOwner);
			return p3.updateMin(v) and c1.setToFalse() and pOwner->notify();
		}
		else
		if (v > p3.max())
		{
			p3.detach(pOwner);
			return p2.updateMin(v) and c1.setToTrue() and pOwner->notify();
		}
		return true;
	}
	bool updateMax(const Eval& v)
	{
		if (c1.isTrue())
			return p2.updateMax(v);
		if (!c1.canBeTrue())
			return p3.updateMax(v);

		if (v < p2.min())
		{
			p2.detach(pOwner);
			return p3.updateMax(v) and c1.setToFalse() and pOwner->notify();
		}
		else
		if (v < p3.min())
		{
			p3.detach(pOwner);
			return p2.updateMax(v) and c1.setToTrue() and pOwner->notify();
		}
		return true;
	}
	void range(Eval& v1,Eval& v2) const
	{	v1=min(); v2=max();	}
	bool updateRange(const Eval& v1, const Eval& v2)
	{	return updateMin(v1) and updateMax(v2);	}

	void attach(INotifiable* f) { 	pOwner=f;c1.attach(f); p2.attach(f);	p3.attach(f); }
	void detach(INotifiable* f) {	c1.detach(f); p2.detach(f);	p3.detach(f); }
	Rel3<IfThenElse,Expr1,Expr2,Expr3> getObj()  const
	{ 	return Rel3<IfThenElse,Expr1,Expr2,Expr3>(c1.getObj(),p2.getObj(),p3.getObj());	}

	CChkView<Expr1> 	c1;
	BndView<Eval,Expr2>	p2;
	BndView<Eval,Expr3>	p3;
	INotifiable*		pOwner;
};

// ********** arithmetic ************

/**
 *	BndView over the minimum of two expressions.
 *	\ingroup BndViews
 **/
template<class Expr1,class Expr2,class Eval>
struct BndViewRel2<Min,Expr1,Expr2,Eval>
{
	BndViewRel2(Store& store, const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}
	Eval min() const
	{	return std::min(p1.min(),p2.min());	}
	Eval max() const
	{	return std::min(p1.max(),p2.max()); }

	bool updateMin(const Eval& v)
	{	return p1.updateMin(v) and p2.updateMin(v);	}

	// only does anything if there is only one element with min > v
	bool updateMax(const Eval& v)
	{
		if (p2.min() > v)
			return p1.updateMax(v);
		else
		if (p1.min() > v)
			return p2.updateMax(v);
		return true;
	}
	void range(Eval& v1,Eval& v2) const
	{
		Eval i1,i2,a1,a2;
		p1.range(i1,a1);
		p2.range(i2,a2);
		v1 = std::min(i1,i2);
		v2 = std::min(a1,a2);
	}
	bool updateRange(const Eval& v1, const Eval& v2)
	{	return updateMin(v1) and updateMax(v2);	}

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f); }
	Rel2<Min,Expr1,Expr2>	getObj() const
	{	return 	Rel2<Min,Expr1,Expr2>(p1.getObj(),p2.getObj());	}

	BndView<Eval,Expr1>	p1;
	BndView<Eval,Expr2>	p2;
};

/**
 *	BndView over the maximum of two expressions.
 *	\ingroup BndViews
 **/
template<class Expr1,class Expr2,class Eval>
struct BndViewRel2<Max,Expr1,Expr2,Eval>
{
	BndViewRel2(Store& store, const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}
	Eval max() const
	{	return std::max(p1.max(),p2.max());	}
	Eval min() const
	{	return std::max(p1.min(),p2.min()); }

	bool updateMax(const Eval& v)
	{	return p1.updateMax(v) and p2.updateMax(v);	}

	// only does anything if there is only one element with max < v
	bool updateMin(const Eval& v)
	{
		if (p2.max() < v)
			return p1.updateMin(v);
		else
		if (p1.max() < v)
			return p2.updateMin(v);
		return true;
	}
	void range(Eval& v1,Eval& v2) const
	{
		Eval i1,i2,a1,a2;
		p1.range(i1,a1);
		p2.range(i2,a2);
		v1 = std::max(i1,i2);
		v2 = std::max(a1,a2);
	}
	bool updateRange(const Eval& v1, const Eval& v2)
	{	return updateMin(v1) and updateMax(v2);	}

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f); }
	Rel2<Max,Expr1,Expr2>	getObj() const
	{	return 	Rel2<Max,Expr1,Expr2>(p1.getObj(),p2.getObj());	}

	BndView<Eval,Expr1>	p1;
	BndView<Eval,Expr2>	p2;
};

/**
 *	BndView over the symmetric of an expression.
 *	\ingroup BndViews
 **/
template<class View,class Eval>
struct BndViewRel1<Sym,View,Eval>
{
	typedef typename Casper::Traits::GetEval<View>::Type	ViewEval;

	BndViewRel1(Store& store, const View& p1) :
		p1(store,p1)	{}
	Eval min() const { return Util::convLb<Eval>(-p1.max()); }
	Eval max() const { return Util::convUb<Eval>(-p1.min()); }
	bool updateMin(const Eval& v)
	{ return p1.updateMax(Util::convUb<ViewEval>(-v)); }
	bool updateMax(const Eval& v)
	{ return p1.updateMin(Util::convLb<ViewEval>(-v)); }
	void range(Eval& v1,Eval& v2) const
	{
		Eval i1,a1;
		p1.range(i1,a1);
		v1 = Util::convLb<Eval>(-a1);
		v2 = Util::convUb<Eval>(-i1);
	}
	bool updateRange(const Eval& v1, const Eval& v2)
	{	return p1.updateRange(Util::convLb<ViewEval>(-v2),
							  Util::convUb<ViewEval>(-v1));	}

	void attach(INotifiable* f)	{	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }
	Rel1<Sym,View> getObj()  const
	{ 	return Rel1<Sym,View>(p1.getObj());	}

	BndView<ViewEval,View>	p1;
};

/**
 *	BndView over the addition of two expressions.
 *	\ingroup BndViews
 **/
template<class Expr1,class Expr2,class Eval>
struct BndViewRel2<Add,Expr1,Expr2,Eval>
{
/*	static_assert(IsEqual<typename Casper::Traits::GetEval<Expr1>::Type,Eval>::value,
				  "'Add' operands must evaluate to the same type");
	static_assert(IsEqual<typename Casper::Traits::GetEval<Expr2>::Type,Eval>::value,
				  "'Add' operands must evaluate to the same type");
*/
	BndViewRel2(Store& store,const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}
	Eval min() const
	{ return Util::addLb<Eval>(p1.min(),p2.min()); }
	Eval max() const
	{ return Util::addUb<Eval>(p1.max(),p2.max()); }
	void range(Eval& l, Eval& u) const
	{	l = min(); u = max();	}
	bool updateRange(Eval l, Eval u)
	{	return updateMin(l) and updateMax(u);	}
	bool updateMin(const Eval& v)
	{	return p1.updateMin(Util::subLb<Eval>(v,p2.max())) and
			   p2.updateMin(Util::subLb<Eval>(v,p1.max()));}
	bool updateMax(const Eval& v)
	{ 	return p1.updateMax(Util::subUb<Eval>(v,p2.min())) and
			   p2.updateMax(Util::subUb<Eval>(v,p1.min()));}
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Add,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Add,Expr1,Expr2>(p1.getObj(),p2.getObj());	}

	BndView<Eval,Expr1>	p1;
	BndView<Eval,Expr2>	p2;
};


/**
 *	BndView over the subtraction of two expressions.
 *	\ingroup BndViews
 **/
template<class Expr1,class Expr2,class Eval>
struct BndViewRel2<Sub,Expr1,Expr2,Eval>
{
//	typedef typename Casper::Traits::GetEval<Expr1>::Type	View1Eval;
//	typedef typename Casper::Traits::GetEval<Expr2>::Type	View2Eval;

	BndViewRel2(Store& store,const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}
	Eval min() const { return Util::subLb(p1.min(),p2.max()); }
	Eval max() const { return Util::subUb(p1.max(),p2.min()); }
	void range(Eval& l, Eval& u) const
	{	l = min(); u = max();	}
	bool updateMin(const Eval& v)
	{ return p1.updateMin(Util::addLb(v,p2.min())) &&
			 p2.updateMax(Util::subUb(p1.max(),v)); }
	bool updateMax(const Eval& v)
	{ return p1.updateMax(Util::addUb(v,p2.max())) &&
			 p2.updateMin(Util::subLb(p1.min(),v)); }
	bool updateRange(const Eval& lb, const Eval& ub)
	{ return p1.updateRange(Util::addLb(lb,p2.min()),
							Util::addUb(ub,p2.max())) and
			 p2.updateRange(Util::subLb(p1.min(),ub),
							Util::subUb(p1.max(),lb));
	}
	void attach(INotifiable* f)	{ 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

#if 1
	Rel2<Sub,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Sub,Expr1,Expr2>(p1.getObj(),p2.getObj());	}
#else
	Rel2<Sub,typename BndView<Eval,Expr1>::Viewed,typename BndView<Eval,Expr2>::Viewed> getObj()  const
	{ 	return Rel2<Sub,typename BndView<Eval,Expr1>::Viewed,typename BndView<Eval,Expr2>::Viewed>(p1.getObj(),p2.getObj());	}
#endif
	BndView<Eval,Expr1>	p1;
	BndView<Eval,Expr2>	p2;
};


#if 1
template<class Eval,class View>
struct BndViewRel3<InRange,View,Eval,Eval,Eval>
{
	typedef BndViewRel3<InRange,View,Eval,Eval,Eval> Self;

	BndViewRel3(Store& store,const View& p,Eval lb,Eval ub) :
		v(store,p),lb(lb),ub(ub) {}

	Eval min() const
	{	return std::max(v.min(),lb);	}
	Eval max() const
	{	return std::min(v.max(),ub);	}
	void range(Eval& l, Eval& u) const
	{
		v.range(l,u);
		l = std::max(l,lb);
		u = std::min(u,ub);
	}

	bool updateMin(const Eval& m)
	{	return m <= ub and v.updateMin(std::max(lb,m));	}
	bool updateMax(const Eval& m)
	{	return m >= lb and v.updateMax(std::min(ub,m));	}

	bool updateRange(const Eval& l, const Eval& u)
	{	return u >= lb and l <= ub and v.updateRange(std::max(lb,l),std::min(ub,u));	}

	void attach(INotifiable* f) { 	v.attach(f);	}
	void detach(INotifiable* f) {	v.detach(f);	}

	Rel3<InRange,View,Eval,Eval> getObj()  const
	{ 	return Rel3<InRange,View,Eval,Eval>(v.getObj(),lb,ub);	}

	BndView<Eval,View>			v;
	const Eval	lb;
	const Eval	ub;
};
#else
template<class Eval,class View>
struct BndViewRel3<InRange,View,Eval,Eval,Eval>
{
	typedef BndViewRel3<InRange,View,Eval,Eval,Eval> Self;

	BndViewRel3(Store& store,const View& p,Eval lb,Eval ub) : v(store,p),
		lb(store,std::max(lb,v.min())),ub(store,std::min(ub,v.max())) {}

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
	void range(Eval& l, Eval& u) const
	{
		v.range(l,u);
		l = std::max(l,lb.get());
		u = std::min(u,ub.get());
		lb = l;
		ub = u;
	}

	bool updateMin(const Eval& m)
	{
		if (m > ub)
			return false;
		if (m <= lb)
			return true;
		if (!v.updateMin(m))
			return false;
		if (min() < m)
			lb.store().post(v.getObj() >= m);
		lb = m;
		return true;
	}
	bool updateMax(const Eval& m)
	{
		if (m < lb)
			return false;
		if (m >= ub)
			return true;
		if (!v.updateMax(m))
			return false;
		if (max() > m)
			lb.store().post(v.getObj() <= m);
		ub = m;
		return true;
	}

	bool updateRange(const Eval& l, const Eval& u)
	{
		if (u < lb or l > ub)
			return false;
		if (l <= lb and u >= ub)
			return true;
		if (!v.updateRange(l,u))
			return false;
		if (min() < l)
			lb.store().post(v.getObj() >= l);
		if (max() > u)
			lb.store().post(v.getObj() <= u);
		lb = l;
		ub = u;
		return true;
	}

	void attach(INotifiable* f) { 	v.attach(f);	}
	void detach(INotifiable* f) {	v.detach(f);	}

	View getObj()  const
	{ 	return v.getObj();	}

	BndView<Eval,View>			v;
	mutable Reversible<Eval>	lb;
	mutable Reversible<Eval>	ub;
};
#endif


/**
 *	BndView over the multiplication of two expressions.
 *	\ingroup BndViews
 **/
template<class Expr1,class Expr2,class Eval>
struct BndViewRel2<Mul,Expr1,Expr2,Eval>
{
	BndViewRel2(Store& store, const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}

	inline Eval min() const;
	inline Eval max() const;
	void range(Eval& min, Eval& max) const;

	bool updateMin(const Eval& m);
	bool updateMax(const Eval& m);

	bool updateRange(const Eval& min, const Eval& max);

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<Mul,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Mul,Expr1,Expr2>(p1.getObj(),p2.getObj());	}

	BndView<Eval,Expr1>	p1;
	BndView<Eval,Expr2>	p2;
};

namespace Detail {

//template<class T>
//struct TermCasper::Traits::GetElem
//::Typedef typename Casper::Traits::GetEval<T>::Type Eval;	};
//
//template<class T>
//struct TermCasper::Traits::GetElem<StdRange<T> >::Typeedef T	Eval;	};

template<class P1,class P2,class Eval>
void mulRange(const P1& p1, const P2& p2,Eval& lb, Eval& ub)
{
	using Util::mulUb;
	using Util::mulLb;
	using Util::isNeg;
	using Util::isPos;

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

template<class Expr1,class Expr2,class Eval>
void BndViewRel2<Mul,Expr1,Expr2,Eval>::range(Eval& lb, Eval& ub) const
{	
	lb = limits<Eval>::negInf();
	ub = limits<Eval>::posInf();
	Detail::mulRange(p1,p2,lb,ub);
}

template<class Expr1,class Expr2,class Eval>
inline Eval BndViewRel2<Mul,Expr1,Expr2,Eval>::min() const
{
	using Util::mulLb;
	using Util::isNeg;
	using Util::isPos;

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

template<class Expr1,class Expr2,class Eval>
inline Eval BndViewRel2<Mul,Expr1,Expr2,Eval>::max() const
{
	using Util::mulUb;
	using Util::isNeg;
	using Util::isPos;

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

namespace Detail {
template<class P1,class P2,class Eval> void divRange(const P1&,const P2&,
													 Eval&,Eval&);
template<class Eval,class P1,class P2> Eval divMin(const P1&,const P2&);
template<class Eval,class P1,class P2> Eval divMax(const P1&,const P2&);

};

template<class T> struct StdRange;

template<class Expr1,class Expr2,class Eval>
bool BndViewRel2<Mul,Expr1,Expr2,Eval>::updateMin(const Eval& zl)
{
	using Util::divLb;
	using Util::divUb;
	using Util::isNeg;
	using Util::isPos;

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

template<class Expr1,class Expr2,class Eval>
bool BndViewRel2<Mul,Expr1,Expr2,Eval>::updateMax(const Eval& zu)
{
	using Util::divLb;
	using Util::divUb;
	using Util::isNeg;
	using Util::isPos;

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

template<class Expr1,class Expr2,class Eval>
bool BndViewRel2<Mul,Expr1,Expr2,Eval>::updateRange(const Eval& zl,
													const Eval& zu)
{
	using Util::divLb;
	using Util::divUb;
	using Util::isNeg;
	using Util::isPos;

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
template<class Expr1,class Expr2,class Eval>
bool BndViewRel2<Mul,Expr1,Expr2,Eval>::updateRange(const Eval& lb,
													const Eval& ub)
{
	StdRange<Eval> r(lb,ub);
	Eval l1,u1;
	p1.range(l1,u1);
	if (!Detail::IsIntegral<Eval>::value or lb*ub>0 or p2.min()*p2.max()>0)
	{
		if (p2.min()==0 and p2.max()==0)
			return false;
		Detail::divRange(r,p2,l1,u1);
		if (!p1.updateRange(l1,u1))
			return false;
	}
	Eval l2,u2;
	p2.range(l2,u2);
	if (!Detail::IsIntegral<Eval>::value or lb*ub>0 or p1.min()*p1.max()>0)
	{
		if (p1.min()==0 and p1.max()==0)
			return false;
		Detail::divRange(r,p1,l2,u2);
		if (!p2.updateRange(l2,u2))
			return false;
	}
	return true;
}
#endif

/**
 *	BndView over the division of two expressions.
 *	\ingroup BndViews
 **/
template<class Expr1,class Expr2,class Eval>
struct BndViewRel2<Div,Expr1,Expr2,Eval>
{
//	typedef typename Casper::Traits::GetEval<Expr1>::Type	View1Eval;
//	typedef typename Casper::Traits::GetEval<Expr2>::Type	View2Eval;

	BndViewRel2(Store& store, const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) /*,
		r(store)*/ {}

	Eval min() const;
	Eval max() const;
	void range(Eval& min, Eval& max) const;

	// these could be better ...
	bool updateMin(const Eval& m) { return updateRange(m,max());}
	bool updateMax(const Eval& m) { return updateRange(min(),m);}

	bool updateRange(const Eval& min, const Eval& max);

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<Div,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Div,Expr1,Expr2>(p1.getObj(),p2.getObj());	}

	BndView<Eval,Expr1> 		p1;
	BndView<Eval,Expr2> 		p2;
};

namespace Detail {

// Continuous case. TODO
template<class Eval>
struct DivRange;

template<class P1,class P2,class Eval>
void divRange(const P1& p1, const P2& p2, Eval& lb, Eval& ub)
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
	void operator()(const P1& p1, const P2& p2, Eval& lb, Eval& ub)
	{
		using Util::isPos;
		using Util::isNeg;
		using Util::divLb;
		using Util::divUb;

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
					throw Exception::DivisionByZero();
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

template<class Expr1,class Expr2,class Eval>
Eval BndViewRel2<Div,Expr1,Expr2,Eval>::min() const
{
	using Util::isPos;
	using Util::isNeg;
	using Util::divLb;
	using Util::divUb;

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
				throw Exception::DivisionByZero();
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

template<class Expr1,class Expr2,class Eval>
Eval BndViewRel2<Div,Expr1,Expr2,Eval>::max() const
{
	using Util::isPos;
	using Util::isNeg;
	using Util::divLb;
	using Util::divUb;

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
				throw Exception::DivisionByZero();
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


template<class Expr1,class Expr2,class Eval>
void BndViewRel2<Div,Expr1,Expr2,Eval>::range(Eval& lb, Eval& ub) const
{
	lb = limits<Eval>::negInf();
	ub = limits<Eval>::posInf();
	Detail::divRange(p1,p2,lb,ub);
}


template<class Expr1,class Expr2,class Eval>
bool BndViewRel2<Div,Expr1,Expr2,Eval>::updateRange(const Eval& lb,
													const Eval& ub)
{
	Util::StdRange<Eval> r(lb,ub);
	Eval mmin,mmax;
	range(mmin,mmax);
	if (lb>mmax or ub<mmin)
		return false;
	if (lb<=mmin and ub>=mmax)
		return true;

	Eval l1,u1;
	p1.range(l1,u1);
	Detail::mulRange(p2,r,l1,u1);

	if (!p1.updateRange(l1,u1))
		return false;

	Eval l2,u2;
	p2.range(l2,u2);
	Detail::divRange(p1,r,l2,u2);

	if (!p2.updateRange(l2,u2))
		return false;

	return true;
}

/**
 *	BndView over the modulo of an expression.
 *  \warning Works with positive intervals only.
 *  \note Does not compute the tightest interval. I don't think
 *  the smallest interval can be obtained in constant time.
 *  \todo Generalize to negative intervals.
 *	\ingroup BndViews
 **/
template<class Expr1,class Expr2,class Eval>
struct BndViewRel2<Mod,Expr1,Expr2,Eval>
{
	BndViewRel2(Store& store, const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}
	Eval min() const;
	Eval max() const;
	bool updateMin(const Eval& v);
	bool updateMax(const Eval& v);
	void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	bool updateRange(const Eval& l, const Eval& u)
	{	return updateMin(l) and updateMax(u);	}
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f); }
	Rel2<Mod,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Mod,Expr1,Expr2>(p1.getObj(),p2.getObj());	}

	BndView<Eval,Expr1>	p1;
	BndView<Eval,Expr2>	p2;
};

template<class Expr1,class Expr2,class Eval>
Eval BndViewRel2<Mod,Expr1,Expr2,Eval>::min() const
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

template<class Expr1,class Expr2,class Eval>
Eval BndViewRel2<Mod,Expr1,Expr2,Eval>::max() const
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
	Eval bdmin = static_cast<Eval>(std::ceil(static_cast<double>(Dmin+1)/(qmin+1)));
	// use only when is safe
	if (dmin>=qmax)	// FIXME: could it be bdmin>=qmax?
		dmin = std::max(dmin,bdmin);

	assert(dmin<=dmax);

	//if (Dmax/(qmin+1)>dmin and Dmax/(qmin+1)<dmax)
	//	return std::min(Dmax*(1-qmin)/(qmin+1),dmax-1);

	return std::min(Dmax-qmin*dmin,dmax-1);
}

template<class Expr1,class Expr2,class Eval>
bool BndViewRel2<Mod,Expr1,Expr2,Eval>::updateMin(const Eval& v)
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

	const Eval lq = std::ceil(static_cast<double>(Dmin-rmax)/dmax);
	const Eval rq = std::floor(static_cast<double>(Dmax-v)/dmin);

	bool r = p2.updateMin(v+1);

	r &= p1.updateMin(lq*dmin + v) and
		 p1.updateMax(rq*dmax+rmax);

	if (rq != 0)
		r &= p2.updateMin((Dmin-rmax)/rq);

	if (lq != 0)
		r &= p2.updateMax((Dmax-v)/lq);

	return r;
}

template<class Expr1,class Expr2,class Eval>
bool BndViewRel2<Mod,Expr1,Expr2,Eval>::updateMax(const Eval& v)
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

	const Eval lq = std::ceil(static_cast<double>(Dmin-v)/dmax);
	const Eval rq = std::floor(static_cast<double>(Dmax-min())/dmin);

	bool r = p1.updateMin(lq*dmin+rmin) and
		 	 p1.updateMax(rq*dmax+v);
	if (rq != 0)
		r &= p2.updateMin((Dmin-v)/rq);
	if (lq != 0)
		r &= p2.updateMax((Dmax-rmin)/lq);
	return r;
}

/**
 *	BndView over the absolute value of an expression.
 *	\ingroup BndViews
 **/
template<class Expr1,class Eval>
struct BndViewRel1<Abs,Expr1,Eval>
{
	BndViewRel1(Store& store, const Expr1& p1) :
		p1(store,p1) {}
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

	bool updateMin(const Eval& v)
	{
		if (v <= 0)
			return true;
		if (p1.min() >= 0)
			return p1.updateMin(v);
		if (p1.max() <= 0)
			return p1.updateMax(-v);
		return true;
	}
	bool updateMax(const Eval& v)
	{
		if (v < 0)
			return false;
		return p1.updateRange(-v,v);
	}
	void range(Eval& l,Eval& u) const
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
	bool updateRange(const Eval& l, const Eval& u)
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
	Rel1<Abs,Expr1> getObj()  const
	{ 	return Rel1<Abs,Expr1>(p1.getObj());	}


	BndView<Eval,Expr1>	p1;
};

/**
 *	BndView over the square of an expression.
 *	\ingroup BndViews
 **/
template<class Expr1,class Eval>
struct BndViewRel1<Sqr,Expr1,Eval>
{
//	typedef typename Casper::Traits::GetEval<Expr1>::Type	View1Eval;

	BndViewRel1(Store& store, const Expr1& p1) :
		p1(store,p1) {}
	Eval min() const
	{
		const Eval pmin = p1.min();
		const Eval pmax = p1.max();
		if (pmin>0)
			return Util::sqrLb(pmin);
		else
		if (pmax<0)
			return Util::sqrLb(pmax);
		else
			return static_cast<Eval>(0);
	}
	Eval max() const
	{	return std::max(Util::sqrUb(p1.min()),Util::sqrUb(p1.max()));	}

	bool updateMin(const Eval& v)
	{
		if (v <= 0)
			return true;
		const Eval r = Util::sqrtLb(v);
		if (p1.min() > -r)
			return p1.updateMin(r);
		else
		if (p1.max() < r)
			return p1.updateMax(-r);
		return true;
	}
	bool updateMax(const Eval& v)
	{	return v >= 0 and p1.updateRange(-Util::sqrtUb(v),Util::sqrtUb(v));	}
	void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	bool updateRange(const Eval& l, const Eval& u)
	{	return updateMin(l) and updateMax(u);	}

	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }
	Rel1<Sqr,Expr1> getObj()  const
	{ 	return Rel1<Sqr,Expr1>(p1.getObj());	}

	BndView<Eval,Expr1>	p1;
};


/**
 *	BndView over the exponential of an expression.
 *	\ingroup BndViews
 **/
template<class Expr1,class Eval>
struct BndViewRel1<Exp,Expr1,Eval>
{
//	typedef typename Casper::Traits::GetEval<Expr1>::Type	View1Eval;

	BndViewRel1(Store& store, const Expr1& p1) :
		p1(store,p1) {}
	Eval min() const 	{	return Util::expLb<Eval>(p1.min());	}
	Eval max() const 	{	return Util::expUb<Eval>(p1.max());	}

	bool updateMin(const Eval& v)
	{	return !Util::isPos(v) or Util::logLb<Eval>(v);	}
	bool updateMax(const Eval& v)
	{	return p1.updateMax(Util::logUb<Eval>(v));	}
	void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	bool updateRange(const Eval& l, const Eval& u)
	{	return updateMin(l) and updateMax(u);	}

	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }
	Rel1<Exp,Expr1> getObj()  const
	{ 	return Rel1<Exp,Expr1>(p1.getObj());	}

	BndView<Eval,Expr1>	p1;
};

/**
 *	BndView over the logarithm of an expression.
 *	\ingroup BndViews
 **/
template<class Expr1,class Eval>
struct BndViewRel1<Log,Expr1,Eval>
{
//	typedef typename Casper::Traits::GetEval<Expr1>::Type	View1Eval;

	BndViewRel1(Store& store, const Expr1& p1) :
		p1(store,p1) {}
	Eval min() const
	{
		return !Util::isPos(p1.min()) ?
			limits<Eval>::negInf() :
			Util::logLb<Eval>(p1.min());
	}
	Eval max() const 	{	return Util::logUb<Eval>(p1.max());	}
	bool updateMin(const Eval& v)
	{	return p1.updateMin(Util::expLb<Eval>(v));	}
	bool updateMax(const Eval& v)
	{	return p1.updateMax(Util::expUb<Eval>(v));	}
	void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	bool updateRange(const Eval& l, const Eval& u)
	{	return updateMin(l) and updateMax(u);	}

	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }

	Rel1<Log,Expr1> getObj()  const
	{ 	return Rel1<Log,Expr1>(p1.getObj());	}

	BndView<Eval,Expr1>	p1;
};


/**
 * 	BndView over a parameterized expression.
 * 	\ingroup BndViews
 **/
template<class Eval>
struct BndView<Eval,Par<Eval> >
{
	BndView(Store& store, const Par<Eval>& p1) :
		state(p1.getState()),val(p1.value())	{}
	Eval min() const {	return val;}
	Eval max() const {	return val;}
	bool updateMin(const Eval& v) 	{ return v <= min(); }
	bool updateMax(const Eval& v)   { return v >= max(); }
	void range(Eval& l,Eval& u) const {	l = u = val; }
	bool updateRange(const Eval& l, const Eval& u)
	{	return l <= val and val <= u;	}

	void attach(INotifiable* f) {}
	void detach(INotifiable* f) 	{}
	Par<Eval>	getObj() const
	{	return Par<Eval>(state,val);	}

	State&	state;
	Eval	val;
};


} // CP
} // Casper

template<class Eval,class View>
std::ostream& operator<<(std::ostream& os, const Casper::CP::BndView<Eval,View>& v)
{	return Casper::Util::RangePrinter<Eval,Casper::CP::BndView<Eval,View> >()(os,v);	}


#endif // _H_CASPER_CP_VIEW_BND
//
//namespace Casper {
//#ifndef CASPER_NO_OPERATORS
//#ifndef CASPER_CP_RELGEN_BNDVIEW_H
//#define CASPER_KERNEL_MACRO_BNDVIEW_H
//#include <casper/kernel/macro/bndview.h>
//#endif
//#endif
//};
