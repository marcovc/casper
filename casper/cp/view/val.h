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

#ifndef _H_CASPER_CP_VIEW_VAL
#define _H_CASPER_CP_VIEW_VAL

#include <casper/cp/expr.h>
#include <casper/cp/view/array.h>

namespace Casper {
namespace CP {


/********************
 * Value View
 *********************/

#if 1
template<class Eval,class R>
struct NoValView
{
	NoValView(Store& s,const R& r)
	{
		std::ostringstream os;
		os << r;
		throw Casper::Exception::UndefinedView(os.str().c_str(),"CP::ValView");
	}
	Eval value() const {	throw 0;	}
	bool setValue(const Eval& val) {	throw 0;	}
	bool ground() const {	throw 0;	}
	void attach(INotifiable*) {	throw 0;	}
	void detach(INotifiable*) {	throw 0;	}
	Eval getObj()  const {	throw 0;	}
};
#else
template<class Eval,class R>
struct NoValView
{
	NoValView(Store& s,const R& r);
	Eval value() const;
	bool setValue(const Eval& val);
	bool ground() const;
	void attach(INotifiable*);
	void detach(INotifiable*);
	R getObj()  const;
};
#endif


template<class> struct CChkView;
template<class> struct ChkView;

// converts a view to a checkview
template<class View>
struct NoValView<bool,View>
{
	NoValView(Store& store,const View& v) :
		v(store,v) {}
	bool value() const { assert(ground()); return v.isTrue(); }
	bool setValue(const bool& val) { return val?v.setToTrue():v.setToFalse();	}
	bool ground() const { return v.isTrue() or !v.canBeTrue(); }
	void attach(INotifiable* f) { v.attach(f); }
	void detach(INotifiable* f) { v.detach(f); }
	View getObj()  const { return v.getObj(); }

	CChkView<View>	v;
};

/**
 * 	ValView over a generic view. View must be convertible to Eval.
 * 	\ingroup ValViews
 **/

template<class Eval,class View>
struct ValView : NoValView<Eval,View>
{
	ValView(Store& s,const View& v) :
		NoValView<Eval,View>(s,v) {}
};


template<class F,class Expr1,class Eval>
struct ValViewRel1 : NoValView<Eval,Rel1<F,Expr1> >
{
	ValViewRel1(Store& s,const Expr1& v) :
		NoValView<Eval,Rel1<F,Expr1> >(s,rel<F>(v)) {}
};

template<class F,class Expr1,class Expr2,class Eval>
struct ValViewRel2 :  NoValView<Eval,Rel2<F,Expr1,Expr2> >
{
	ValViewRel2(Store& s,const Expr1& v1,const Expr2& v2) :
		NoValView<Eval,Rel2<F,Expr1,Expr2> >(s,rel<F>(v1,v2)) {}
};

template<class F,class Expr1,class Expr2,class Expr3,class Eval>
struct ValViewRel3 :  NoValView<Eval,Rel3<F,Expr1,Expr2,Expr3> >
{
	ValViewRel3(Store& s,const Expr1& v1,const Expr2& v2,const Expr3& v3) :
		NoValView<Eval,Rel3<F,Expr1,Expr2,Expr3> >(s,rel<F>(v1,v2,v3)) {}
};


/**
 * 	ValView over a literal.
 * 	\ingroup ValViews
 **/
template<class Eval>
struct ValView<Eval,Eval>
{
	ValView(Store& store, const Eval& v) :
		v(v) {}

	Eval value() const { return v; }
	bool setValue(const Eval& val) { return val == v;	}
	bool ground() const { return true; }
	void attach(INotifiable*) {}
	void detach(INotifiable*) {}
	Eval getObj()  const { return v; }
	const Eval	v;
};

/**
 * 	ValView over a uint literal (for convenience).
 * 	\ingroup ValViews
 **/
template<>
struct ValView<int,uint>
{
	ValView(Store& store, const uint& v) :
		v(v) {}

	int value() const { return static_cast<int>(v); }
	bool setValue(const int& val) { return val == static_cast<int>(v);	}
	bool ground() const { return true; }
	void attach(INotifiable*) {}
	void detach(INotifiable*) {}
	uint getObj()  const { return v; }
	const uint	v;
};

/**
 *	Identity : ValView over a ValView.
 *	\ingroup Views
 **/
//template<class Eval,class View>
//struct ValView<Eval,ValView<Eval,View> > : ValView<Eval,View>
//{
//	typedef ValView<Eval,View>	Super;
//	using ValView<Eval,View>::attach;
//	using ValView<Eval,View>::detach;
//
//	ValView(Store& store, const ValView<Eval,View>& v) :
//		Super(v) {}
//
//	Super getObj()  const { return static_cast<const Super&>(*this); }
//};

// TODO: reification here


template<class,class,class>			struct ValViewRel1;
template<class,class,class,class>	struct ValViewRel2;
template<class,class,class,class,class>	struct ValViewRel3;


/*
 * 	ValView over a Rel1 relation -> defer to ValViewRel1.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View>
struct ValView<Eval,Rel1<F,View> > : ValViewRel1<F,View,Eval>
{
	ValView(Store& store, const Rel1<F,View>& r) :
		ValViewRel1<F,View,Eval>(store,r.p1) {}
	// not sure if the below constructor is ever used
	ValView(Store& store,const ValViewRel1<F,View,Eval>& v) :
		ValViewRel1<F,View,Eval>(v) {}
};

/*
 * 	ValView over a Rel2 relation -> defer to ValViewRel2.
 * 	\ingroup Views
 **/
template<class Eval,class F,class Expr1,class Expr2>
struct ValView<Eval,Rel2<F,Expr1,Expr2> > :
	ValViewRel2<F,Expr1,Expr2,Eval>
{
	ValView(Store& store, const Rel2<F,Expr1,Expr2>& r) :
		ValViewRel2<F,Expr1,Expr2,Eval>(store,r.p1,r.p2) {}
	// not sure if the below constructor is ever used
	ValView(Store& store, const ValViewRel2<F,Expr1,Expr2,Eval>& v) :
		ValViewRel2<F,Expr1,Expr2,Eval>(v) {}
};

/*
 * 	ValView over a Rel3 relation -> defer to ValViewRel3.
 * 	\ingroup Views
 **/
template<class Eval,class F,class Expr1,class Expr2,class Expr3>
struct ValView<Eval,Rel3<F,Expr1,Expr2,Expr3> > :
	ValViewRel3<F,Expr1,Expr2,Expr3,Eval>
{
	ValView(Store& store, const Rel3<F,Expr1,Expr2,Expr3>& r) :
		ValViewRel3<F,Expr1,Expr2,Expr3,Eval>(store,r.p1,r.p2,r.p3) {}
};

/*
 * 	ValView over a ValExpr.
 * 	\ingroup Views
 **/
template<class Eval>
struct ValView<Eval,ValExpr<Eval> > : ValExpr<Eval>
{
	ValView(Store& store, const ValExpr<Eval>& r) :
		ValExpr<Eval>(store,r) {}
};


// conversion

/**
 * 	ValView over a type cast expression.
 *  \ingroup ValViews
 */
template<class View,class Eval>
struct ValViewRel1<Cast<Eval>,View,Eval>
{
	typedef typename Casper::Traits::GetEval<View>::Type	EvalFrom;

	ValViewRel1(Store& store, const View& v) :
		v(store,v) {}

	Eval value() const { return static_cast<Eval>(v.value()); }
	bool setValue(const Eval& val)
	{ return v.setValue(static_cast<EvalFrom>(val));	}
	bool ground() const { return v.ground(); }
	void attach(INotifiable* n) {	v.attach(n);	}
	void detach(INotifiable* n) {	v.detach(n);	}
	Rel1<Cast<Eval>,View> getObj()  const { return rel<Cast<Eval> >(v.getObj()); }
	ValView<EvalFrom,View>	v;
};

/**
 * 	ValView over the symmetric of an expression.
 * 	\ingroup ValViews
 **/
template<class View,class Eval>
struct ValViewRel1<Sym,View,Eval>
{
	ValViewRel1(Store& store, const View& p1) :
		p1(store,p1)	{}
	Eval value() const { return -p1.value(); }
	bool setValue(const Eval& val) { return p1.setValue(-val);	}
	bool ground() const { return p1.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); 	}
	void detach(INotifiable* f) {	p1.detach(f); }
	Rel1<Sym,View>	getObj() const {	return Rel1<Sym,View>(p1.getObj());}
	ValView<Eval,View>	p1;
};

/**
 * 	ValView over the negation of an expression.
 * 	\ingroup ValViews
 **/
template<class View>
struct ValViewRel1<Not,View,bool>
{
	ValViewRel1(Store& store,const View& p1) :
		p1(store,p1)	{}
	bool value() const { return !p1.value(); }
	bool setValue(const bool& val) { return p1.setValue(!val);	}
	bool ground() const { return p1.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); 	}
	void detach(INotifiable* f)	{	p1.detach(f);	}
	Rel1<Not,View>	getObj() const {	return Rel1<Not,View>(p1.getObj());}
	ValView<bool,View>	p1;
};

// logical

/**
 * 	ValView over the conjunction of two expressions.
 * 	\ingroup ValViews
 **/
template<class Expr1,class Expr2>
struct ValViewRel2<And,Expr1,Expr2,bool>
{
	ValViewRel2(Store& store, const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}
	bool value() const { return p1.value() && p2.value(); }
	bool setValue(const bool& val)
	{
		if (val)
			return p1.setValue(true) and p2.setValue(true);
		if (p1.ground())
			return !p1.value() or p2.setValue(false);
		if (p2.ground())
			return !p2.value() or p1.setValue(false);
		return true;
	}
	bool ground() const
	{
		return (p1.ground() and p2.ground()) or
			   (p1.ground() and !p1.value()) or
			   (p2.ground() and !p2.value());
	}
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}
	Rel2<And,Expr1,Expr2>	getObj() const
	{	return Rel2<And,Expr1,Expr2>(p1.getObj(),p2.getObj());}
	ValView<bool,Expr1>	p1;
	ValView<bool,Expr2>	p2;
};

/**
 * 	ValView over the disjunction of two expressions.
 * 	\ingroup ValViews
 **/
template<class Expr1,class Expr2>
struct ValViewRel2<Or,Expr1,Expr2,bool>
{
	ValViewRel2(Store& store,const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) 	{}
	bool value() const { return p1.value() or p2.value(); }
	bool setValue(const bool& val)
	{
		if (!val)
			return p1.setValue(false) and p2.setValue(false);
		if (p1.ground())
			return p1.value() or p2.setValue(true);
		if (p2.ground())
			return p2.value() or p1.setValue(true);
		return true;
	}
	bool ground() const
	{
		return (p1.ground() and p2.ground()) or
			   (p1.ground() and p1.value()) or
			   (p2.ground() and p2.value());
	}

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Or,Expr1,Expr2>	getObj() const
	{	return Rel2<Or,Expr1,Expr2>(p1.getObj(),p2.getObj());}

	ValView<bool,Expr1>	p1;
	ValView<bool,Expr2>	p2;
};

/**
 * 	ValView over the exclusive disjunction of two expressions.
 * 	\ingroup ValViews
 **/
template<class Expr1,class Expr2>
struct ValViewRel2<XOr,Expr1,Expr2,bool>
{
	ValViewRel2(Store& store,const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) 	{}
	bool value() const { return p1.value() ^ p2.value(); }
	bool setValue(const bool& val)
	{
		if (p1.ground())
			return p2.setValue(val ^ p1.value());
		if (p2.ground())
			return p1.setValue(val ^ p2.value());
		return true;
	}
	bool ground() const { return p1.ground() and p2.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<XOr,Expr1,Expr2>	getObj() const
	{	return Rel2<XOr,Expr1,Expr2>(p1.getObj(),p2.getObj());}

	ValView<bool,Expr1>	p1;
	ValView<bool,Expr2>	p2;
};

// arithmetic

/**
 * 	ValView over the addition of two expressions.
 * 	\ingroup ValViews
 **/
template<class Expr1,class Expr2,class Eval>
struct ValViewRel2<Add,Expr1,Expr2,Eval>
{
	typedef typename Casper::Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Casper::Traits::GetEval<Expr2>::Type	View2Eval;

	ValViewRel2(Store& store,const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}
	Eval value() const { return p1.value() + p2.value(); }
	bool setValue(const Eval& val)
	{
		if (p1.ground())
			return p2.setValue(val - p1.value());
		if (p2.ground())
			return p1.setValue(val - p2.value());
		return true;
	}
	bool ground() const { return p1.ground() && p2.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Add,Expr1,Expr2>	getObj() const
	{	return Rel2<Add,Expr1,Expr2>(p1.getObj(),p2.getObj());}

	ValView<View1Eval,Expr1>	p1;
	ValView<View2Eval,Expr2>	p2;
};

/**
 * 	ValView over the subtraction of two expressions.
 * 	\ingroup ValViews
 **/
template<class Expr1,class Expr2,class Eval>
struct ValViewRel2<Sub,Expr1,Expr2,Eval>
{
	typedef typename Casper::Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Casper::Traits::GetEval<Expr2>::Type	View2Eval;

	ValViewRel2(Store& store, const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}
	Eval value() const { return p1.value() - p2.value(); }
	bool setValue(const Eval& val)
	{
		if (p1.ground())
			return p2.setValue(val + p1.value());
		if (p2.ground())
			return p1.setValue(val + p2.value());
		return true;
	}
	bool ground() const { return p1.ground() and p2.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}
	Rel2<Sub,Expr1,Expr2>	getObj() const
	{	return Rel2<Sub,Expr1,Expr2>(p1.getObj(),p2.getObj());}

	ValView<View1Eval,Expr1>	p1;
	ValView<View2Eval,Expr2>	p2;
};

/**
 * 	ValView over the multiplication of two expressions.
 * 	\ingroup ValViews
 **/
template<class Expr1,class Expr2,class Eval>
struct ValViewRel2<Mul,Expr1,Expr2,Eval>
{
	typedef typename Casper::Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Casper::Traits::GetEval<Expr2>::Type	View2Eval;

	ValViewRel2(Store& store, const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}
	Eval value() const { return p1.value() * p2.value(); }
	bool ground() const { return p1.ground() && p2.ground(); }
	bool setValue(const Eval& val)
	{
		if (p1.ground())
			return p2.setValue(val / p1.value());
		if (p2.ground())
			return p1.setValue(val / p2.value());
		return true;
	}
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Mul,Expr1,Expr2>	getObj() const
	{	return Rel2<Mul,Expr1,Expr2>(p1.getObj(),p2.getObj());}

	ValView<View1Eval,Expr1>	p1;
	ValView<View2Eval,Expr2>	p2;
};


/*
 * 	ValView over BndExpr.
 * 	\ingroup Views
 **/
template<class Eval>
struct ValView<Eval,BndExpr<Eval> >
{
	ValView(Store& store,const BndExpr<Eval>& p1) :
		p1(p1)	{}
	Eval value() const { assert(ground()); return p1.min(); }
	bool setValue(const Eval& val)
	{	return p1.updateRange(val,val);	}
	bool ground() const { return p1.min()==p1.max(); }
	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }
	BndExpr<Eval>	getObj() const {	return p1; }

	BndExpr<Eval>	p1;
};

/*
 * 	ValView over DomExpr.
 * 	\ingroup Views
 **/
template<class Eval,class Dom>
struct ValView<Eval,DomExpr<Eval,Dom> >
{
	ValView(Store& store,const DomExpr<Eval,Dom>& p1) :
		p1(p1)	{}
	Eval value() const { assert(ground()); return p1->value(); }
	bool setValue(const Eval& val)
	{	return p1->setValue(val);	}

	bool ground() const { return p1->ground(); }
	void attach(INotifiable* f) { 	p1->attachOnGround(f); }
	void detach(INotifiable* f) {	p1->detachOnGround(f); }
	DomExpr<Eval,Dom>	getObj() const {	return p1; }

	DomExpr<Eval,Dom>	p1;
};

/**
 * 	ValView over the subscript relation (element).
 * 	\ingroup ValViews
 */
template<class Expr1,class Expr2,class Eval>
struct ValViewRel2<Element,Expr1,Expr2,Eval>
{
	typedef Rel2<Element,Expr1,Expr2>	Rel;
	ValViewRel2(Store& store,const Expr1& p1, const Expr2& p2) :
		array(store,p1),index(store,p2)
		{}
	Eval value() const
	{
		assert(ground());
		if (index.value()< CASPER_ELEMENT_ARRAY_BASE or
			index.value() >= static_cast<int>(array.size())+CASPER_ELEMENT_ARRAY_BASE)
			throw Exception::IndexOutOfBounds();
		return array[index.value()].value();
	}
	bool setValue(const Eval& val)
	{
		if (!index.ground())
			return true;
		if (index.value()< CASPER_ELEMENT_ARRAY_BASE or
			index.value() >= static_cast<int>(array.size())+CASPER_ELEMENT_ARRAY_BASE)
			throw Exception::IndexOutOfBounds();
		return array[index.value()].setValue(val);
	}

	bool ground() const
	{
		return index.ground() and
				(index.value()< CASPER_ELEMENT_ARRAY_BASE or
				 index.value() >= static_cast<int>(array.size())+CASPER_ELEMENT_ARRAY_BASE or
				 array[index.value()].ground());
	}
	void attach(INotifiable* f) { 	array.attach(f);index.attach(f); }
	void detach(INotifiable* f) {	array.detach(f);index.detach(f); }
	Rel2<Element,Expr1,Expr2>	getObj() const
	{	return Rel2<Element,Expr1,Expr2>(array.getObj(),index.getObj());}

	ValArrayView<Eval,Expr1>	array;
	ValView<int,Expr2>			index;
};

/**
 * 	ValView over the absolute value of an expression.
 * 	\ingroup ValViews
 **/
template<class Expr1,class Eval>
struct ValViewRel1<Abs,Expr1,Eval>
{
	typedef typename Casper::Traits::GetEval<Expr1>::Type	View1Eval;
	ValViewRel1(Store& store, const Expr1& p1) :
		p1(store,p1) {}

	Eval value() const { assert(ground()); return std::abs(p1.value()); }
	bool setValue(const Eval& val)
	{
		if (val<0)
			return false;
		if (p1.ground())
			return val == std::abs(p1.value());
		return true;
	}
	bool ground() const { return p1.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f); }
	Rel1<Abs,Expr1>	getObj() const
	{	return Rel1<Abs,Expr1>(p1.getObj());}

	ValView<View1Eval,Expr1>	p1;
};

/**
 * 	ValView over a parameterized expression.
 * 	\ingroup ValViews
 **/
template<class Eval>
struct ValView<Eval,Casper::Ref<Eval> >
{
	ValView(Store& store, const Casper::Ref<Eval>& p1) :
		state(p1.getState()),val(p1.value()) {}
	Eval value() const {	return val;}
	bool setValue(const Eval& v)
	{	return v == val;	}

	bool ground() const { return true; }
	void attach(INotifiable* f) {}
	void detach(INotifiable* f) {}
	Casper::Ref<Eval>	getObj() const
	{	return Casper::Ref<Eval>(state,val);	}

	State&	state;
	const Eval	val;
};


template<class> struct CChkView;


/**
 *	ValView over an IfThenElse expression.
 *	\ingroup ValViews
 **/
template<class Eval,class Expr1,class Expr2,class Expr3>
struct ValViewRel3<IfThenElse,Expr1,Expr2,Expr3,Eval>
{
	ValViewRel3(Store& store, const Expr1& p1, const Expr2& p2, const Expr3& p3) :
		c1(store,p1),p2(store,p2),p3(store,p3)
		{}
	Eval value() const
	{
		if (c1.isTrue())
			return p2.value();
		if (!c1.canBeTrue())
			return p3.value();
		if (p2.value()==p3.value())
			return p2.value();
		assert(0);
		return 0; // FIXME: should throw
	}
	bool setValue(const Eval& v)
	{
		if (c1.isTrue())
			return p2.setValue(v);
		if (!c1.canBeTrue())
			return p3.setValue(v);
		if (p2.ground() and p3.ground())
		{
			if (p2.value()==v and p3.value()!=v)
				return c1.setToTrue();
			if (p3.value()==v and p2.value()!=v)
				return c1.setToFalse();
			if (p2.value()!=v and p3.value()!=v)
				return false;
		}
		return true;
	}
	bool ground() const
	{
		return (c1.isTrue() and p2.ground()) or
			   (!c1.canBeTrue() and p3.ground()) or
			   (p2.ground() and p3.ground() and p2.value()==p3.value());
	}

	void attach(INotifiable* f) { 	c1.attach(f); p2.attach(f);	p3.attach(f); }
	void detach(INotifiable* f) {	c1.detach(f); p2.detach(f);	p3.detach(f); }
	Rel3<IfThenElse,Expr1,Expr2,Expr3> getObj()  const
	{ 	return Rel3<IfThenElse,Expr1,Expr2,Expr3>(c1.getObj(),p2.getObj(),p3.getObj());	}

	CChkView<Expr1> 	c1;
	ValView<Eval,Expr2>	p2;
	ValView<Eval,Expr3>	p3;
};

/**
 * 	ValView over the modulo operation.
 * 	\ingroup ValViews
 **/
template<class Expr1,class Expr2,class Eval>
struct ValViewRel2<Mod,Expr1,Expr2,Eval>
{
	typedef typename Casper::Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Casper::Traits::GetEval<Expr2>::Type	View2Eval;

	ValViewRel2(Store& store,const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}
	Eval value() const { return p1.value() % p2.value(); }
	bool setValue(const Eval& val)
	{
		if (p1.ground() and p2.ground())
			return val == value();
		return true;
	}
	bool ground() const { return p1.ground() && p2.ground(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Mod,Expr1,Expr2>	getObj() const
	{	return Rel2<Mod,Expr1,Expr2>(p1.getObj(),p2.getObj());}

	ValView<View1Eval,Expr1>	p1;
	ValView<View2Eval,Expr2>	p2;
};

template<class Eval,class View>
struct ValViewRel3<InRange,View,Eval,Eval,Eval>
{
	typedef ValViewRel3<InRange,View,Eval,Eval,Eval> Self;

	ValViewRel3(Store& store,const View& p,Eval lb,Eval ub) :
		v(store,p),lb(lb),ub(ub) {}

	Eval value() const
	{
		if (lb==ub)
			return lb;
		if (v.ground())
			return v.value();
		throw Exception::NotGround();
	}
	bool setValue(const Eval& v)
	{
		if (v<lb or v>ub)
			return false;
		return v.setValue(v);
	}
	bool ground() const
	{
		return lb==ub or v.ground();
	}

	void attach(INotifiable* f) { 	v.attach(f);	}
	void detach(INotifiable* f) {	v.detach(f);	}

	Rel3<InRange,View,Eval,Eval> getObj()  const
	{ 	return Rel3<InRange,View,Eval,Eval>(v.getObj(),lb,ub);	}

	ValView<Eval,View>			v;
	const Eval	lb;
	const Eval	ub;
};

} // CP


} // Casper

template<class Eval,class View>
std::ostream& operator<<(std::ostream& os, const Casper::CP::ValView<Eval,View>& v)
{
	if (v.ground())
		os << v.value();
	else
		os << "[not ground]";
	return os;
}

#endif /*_H_CASPER_CP_VIEW_VAL */
