/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2009 - Marco Correia <marco.v.correia@gmail.com>           	   *
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


#ifndef CASPER_CP_VIEW_CHK_H_
#define CASPER_CP_VIEW_CHK_H_

#include <casper/cp/view/common.h>
#include <casper/fwddecl.h>

namespace Casper {
namespace CP {

template<class R>
struct NoChkView
{
	NoChkView(const R& r)
	{
		std::ostringstream os;
		::operator<<(os,r);
		throw Casper::Exception::UndefinedView(os.str().c_str(),"CP::ChkView");
	}
	bool isTrue() const
	{	return true;	}
	bool canBeTrue() const
	{	return true;	}
	bool setToTrue()
	{	return true;	}
	bool setToFalse()
	{	return true;	}
	__attribute__((noreturn))  R getObj() const {}
	void attach(INotifiable*)	{}
	void detach(INotifiable*)	{}
};

template<class View>
struct ChkView : NoChkView<View>
{	ChkView(Store&,const View& r) : NoChkView<View>(r) {} };

template<class F,class Eval1,class Expr1>
struct ChkViewRel1 : NoChkView<Rel1<F,Expr1> >
{
	ChkViewRel1(Store&,const Expr1& v1) :
		NoChkView<Rel1<F,Expr1> >(rel<F>(v1)) {}
};

template<class F, class Eval1,class Expr1,class Eval2,class Expr2>
struct ChkViewRel2 : NoChkView<Rel2<F,Expr1,Expr2> >
{
	ChkViewRel2(Store&,const Expr1& v1,const Expr2& v2) :
		NoChkView<Rel2<F,Expr1,Expr2> >(rel<F>(v1,v2)) {}
};

template<class F, class Eval1,class Expr1,class Eval2,class Expr2,
				class Eval3,class Expr3>
struct ChkViewRel3 : NoChkView<Rel3<F,Expr1,Expr2,Expr3> >
{
	ChkViewRel3(Store&,const Expr1& v1,const Expr2& v2,const Expr3& v3) :
		NoChkView<Rel3<F,Expr1,Expr2,Expr3> >(rel<F>(v1,v2,v3)) {}
};


/*
 *	ChkView over a Rel1 relation -> defers to ChkViewRel1
 *	\ingroup ChkViews
 **/
template<class F,class View>
struct ChkView<Rel1<F,View> > :
	ChkViewRel1<F,typename Casper::Traits::GetEval<View>::Type,View>
{
	ChkView(Store& store, const Rel1<F,View>& r) :
		ChkViewRel1<F,typename Casper::Traits::GetEval<View>::Type,View>(store,r.p1) {}
};

/*
 *	ChkView over a Rel2 relation -> defers to ChkViewRel2
 *	\ingroup ChkViews
 **/
template<class F,class Expr1,class Expr2>
struct ChkView<Rel2<F,Expr1,Expr2> > :
		ChkViewRel2<F,typename Casper::Traits::GetEval<Expr1>::Type,Expr1,
					  typename Casper::Traits::GetEval<Expr2>::Type,Expr2>
{
	ChkView(Store& store, const Rel2<F,Expr1,Expr2>& r) :
		ChkViewRel2<F,typename Casper::Traits::GetEval<Expr1>::Type,Expr1,
					  typename Casper::Traits::GetEval<Expr2>::Type,Expr2>(store,r.p1,r.p2) {}
};

/*
 *	ChkView over a Rel3 relation -> defers to ChkViewRel3
 *	\ingroup ChkViews
 **/
template<class F,class Expr1,class Expr2,class Expr3>
struct ChkView<Rel3<F,Expr1,Expr2,Expr3> > :
	ChkViewRel3<F,typename Casper::Traits::GetEval<Expr1>::Type,Expr1,
					typename Casper::Traits::GetEval<Expr2>::Type,Expr2,
					typename Casper::Traits::GetEval<Expr3>::Type,Expr3>
{
	ChkView(Store& store, const Rel3<F,Expr1,Expr2,Expr3>& r) :
		ChkViewRel3<F,typename Casper::Traits::GetEval<Expr1>::Type,Expr1,
						typename Casper::Traits::GetEval<Expr2>::Type,Expr2,
						typename Casper::Traits::GetEval<Expr3>::Type,Expr3>(store,r.p1,r.p2,r.p3)
						{}
};


// caches the value of the expression once it is set, and never
// asks it again from the viewed object
template<class View>
struct CChkView
{
	CChkView(Store& store,const View& v) :
		v(store,v),state(store,2) {}
	bool isTrue() const	// is it true?
	{
		if (state!=2)
			return state;
		if (v.isTrue())
		{
			state = 1;
			return true;
		}
		return false;
	}
	bool canBeTrue() const 	// can it still be true?
	{
		if (state!=2)
			return state;
		if (!v.canBeTrue())
		{
			state = 0;
			return false;
		}
		return true;
	}
	bool setToTrue()
	{
		if (state!=2)
			return state==1;
		state=1;
		return v.setToTrue();
	}
	bool setToFalse()
	{
		if (state!=2)
			return state==0;
		state=0;
		return v.setToFalse();
	}
//	Store& store() const {	return v.store();	}

	void attach(INotifiable* f) { 	v.attach(f);}
	void detach(INotifiable* f) {	v.detach(f);}

	View getObj()  const
	{ 	return v.getObj();	}

	ChkView<View>	v;

	mutable Reversible<int>	state;

};

// -------- reification ---------

/**
 * 	ChkView over a Boolean literal.
 * 	\ingroup ChkViews
 */
template<>
struct ChkView<bool>
{
	ChkView(Store& store, const bool& b) :
			b(b) {}
	bool isTrue() const	// is it true?
	{	return b;	}
	bool canBeTrue() const 	// can it still be true?
	{	return b;	}
	bool setToTrue()
	{	return b;	}
	bool setToFalse()
	{	return !b;	}
	//Store& store() const {	return mSolver;	}

	void attach(INotifiable* f) { 	}
	void detach(INotifiable* f) {	}

	bool getObj()  const
	{ 	return b;	}

	const bool b;
};

// chkview over bndexpr
template<class Eval>
struct ChkView<BndExpr<Eval> >
{
	ChkView(Store& store, const BndExpr<Eval>& p1) :
			v(p1) {}
	bool isTrue() const	// is it true?
	{	return v.min();	}
	bool canBeTrue() const 	// can it still be true?
	{	return v.max();	}
	bool setToTrue()
	{	return v.updateMin(true);	}
	bool setToFalse()
	{	return v.updateMax(false);	}
//	Store& store() const {	return v.store();	}

	void attach(INotifiable* f) { 	v.attach(f);}
	void detach(INotifiable* f) {	v.detach(f);}

	BndExpr<Eval> getObj()  const
	{ 	return v;	}

	BndExpr<Eval>	v;
};

/**
 * 	ChkView over a Boolean CP variable.
 * 	\ingroup ChkViews
 */
template<class Dom>
struct ChkView<Var<bool,Dom> >
{
	ChkView(Store& store, const Var<bool,Dom>& p1) :
			v(store,p1) {}
	bool isTrue() const	// is it true?
	{	return v.min();	}
	bool canBeTrue() const 	// can it still be true?
	{	return v.max();	}
	bool setToTrue()
	{	return v.updateMin(true);	}
	bool setToFalse()
	{	return v.updateMax(false);	}
//	Store& store() const {	return v.store();	}

	void attach(INotifiable* f) { 	v.attach(f);}
	void detach(INotifiable* f) {	v.detach(f);}

	Var<bool,Dom> getObj()  const
	{ 	return v.getObj();	}

	BndView<bool,Var<bool,Dom> >	v;
};

/**
 * 	ChkView over the negation of an expression.
 * 	\ingroup ChkViews
 */
template<class Expr1>
struct ChkViewRel1<Not,bool,Expr1>
{
	ChkViewRel1(Store& store, const Expr1& p1) :
			v(store,p1) {}
	bool isTrue() const	// is it true?
	{	return !v.canBeTrue();	}
	bool canBeTrue() const 	// can it still be true?
	{	return !v.isTrue();	}
	bool setToTrue()
	{	return v.setToFalse();	}
	bool setToFalse()
	{	return v.setToTrue();	}
//	Store& store() const {	return v.store();	}

	void attach(INotifiable* f) { 	v.attach(f);}
	void detach(INotifiable* f) {	v.detach(f);}

	Rel1<Not,Expr1> getObj()  const
	{ 	return rel<Not>(v.getObj());	}

	ChkView<Expr1>	v;
};

/**
 * 	ChkView over the conjunction of two expressions.
 * 	\ingroup ChkViews
 */
template<class Expr1,class Expr2>
struct ChkViewRel2<And,bool,Expr1,bool,Expr2>
{
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
			store(store),p1(store,p1),p2(store,p2) {}
	bool isTrue() const	// is it true?
	{	return p1.isTrue() and p2.isTrue();	}
	bool canBeTrue() const 	// can it still be true?
	{	return p1.canBeTrue() and p2.canBeTrue();	}
	bool setToTrue()
	{	return p1.setToTrue() and p2.setToTrue();	}
	bool setToFalse()
	{
		detach(pOwner);
		return store.post(rel<Or>(rel<Not>(p1.getObj()),rel<Not>(p2.getObj())));
	}

	void attach(INotifiable* f) { 	pOwner=f; p1.attach(f); p2.attach(f);}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);}

	Rel2<And,Expr1,Expr2> getObj()  const
	{ 	return Rel2<And,Expr1,Expr2>(p1.getObj(),p2.getObj());	}

	Store&			store;
	ChkView<Expr1>	p1;
	ChkView<Expr2>	p2;
	INotifiable*	pOwner;
};

/**
 * 	ChkView over the disjunction of two expressions.
 * 	\ingroup ChkViews
 */
template<class Expr1,class Expr2>
struct ChkViewRel2<Or,bool,Expr1,bool,Expr2>
{
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
			store(store),p1(store,p1),p2(store,p2) {}
	bool isTrue() const	// is it true?
	{	return p1.isTrue() or p2.isTrue();	}
	bool canBeTrue() const 	// can it still be true?
	{	return p1.canBeTrue() or p2.canBeTrue();	}
	bool setToTrue()
	{
		detach(pOwner);
		return store.post(rel<Or>(p1.getObj(),p2.getObj()));
	}
	bool setToFalse()
	{
		detach(pOwner);
		//return store().post(!p1.getObj() and !p2.getObj());
		return p1.setToFalse() and p2.setToFalse();
	}
//	Store& store() const {	return p1.store();	}

	void attach(INotifiable* f) { 	pOwner=f; p1.attach(f); p2.attach(f);}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);}

	Rel2<Or,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Or,Expr1,Expr2>(p1.getObj(),p2.getObj());	}

	Store&			store;
	ChkView<Expr1>	p1;
	ChkView<Expr2>	p2;
	INotifiable*	pOwner;
};

/**
 * 	ChkView over a exclusive disjunction of two expressions.
 * 	\ingroup ChkViews
 */
template<class Expr1,class Expr2>
struct ChkViewRel2<XOr,bool,Expr1,bool,Expr2> :
	ChkViewRel2<Distinct,bool,Expr1,bool,Expr2>
{
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
		ChkViewRel2<Distinct,bool,Expr1,bool,Expr2>(store,p1,p2) {}
};

/**
 * 	ChkView over the equality constraint.
 * 	\ingroup ChkViews
 */
template<class Eval,class Expr1,class Expr2>
struct ChkViewRel2<Equal,Eval,Expr1,Eval,Expr2>
{
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
			store(store),p1(store,p1),p2(store,p2) {}
	bool isTrue() const	// is it true?
	{	return p1.max()==p2.min() and p1.min()==p2.max();	}
	bool canBeTrue() const 	// can it still be true?
	{	return p1.max()>=p2.min() and p1.min()<=p2.max();	}
	bool setToTrue()
	{
		detach(pOwner);
		return store.post(rel<Equal>(p1.getObj(),p2.getObj()));
	}
	bool setToFalse()
	{
		detach(pOwner);
		return store.post(rel<Distinct>(p1.getObj(),p2.getObj()));
	}
//	Store& store() const {	return p1.store();	}

	void attach(INotifiable* f) { 	pOwner=f; p1.attach(f); p2.attach(f);}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);}

	Rel2<Equal,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Equal,Expr1,Expr2>(p1.getObj(),p2.getObj());	}

	Store&				store;
	BndView<Eval,Expr1>	p1;
	BndView<Eval,Expr2>	p2;
	INotifiable*	pOwner;
};

/**
 * 	ChkView over the distinct constraint.
 * 	\ingroup ChkViews
 */
template<class Eval,class Expr1,class Expr2>
struct ChkViewRel2<Distinct,Eval,Expr1,Eval,Expr2> :
	ChkViewRel1<Not,bool,Rel2<Equal,Expr1,Expr2> >
{
	typedef ChkViewRel1<Not,bool,Rel2<Equal,Expr1,Expr2> >	Super;
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
		Super(store,rel<Equal>(p1,p2)) {}
	Rel2<Distinct,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Distinct,Expr1,Expr2>(Super::getObj().p1.p1,Super::getObj().p1.p2);	}

};

/**
 * 	ChkView over the inequality constraint.
 * 	\ingroup ChkViews
 */
template<class Eval,class Expr1,class Expr2>
struct ChkViewRel2<GreaterEqual,Eval,Expr1,Eval,Expr2>
{
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
			store(store),p1(store,p1),p2(store,p2) {}
	bool isTrue() const	// is it true?
	{	return p1.min()>=p2.max();	}
	bool canBeTrue() const 	// can it still be true?
	{	return p1.max()>=p2.min();	}
	bool setToTrue()
	{
		detach(pOwner);
		return store.post(rel<GreaterEqual>(p1.getObj(),p2.getObj()));
	}
	bool setToFalse()
	{
		detach(pOwner);
		return store.post(rel<Less>(p1.getObj(),p2.getObj()));
	}
//	Store& store() const {	return getState(p1,p2);	}

	void attach(INotifiable* f) { 	pOwner=f; p1.attach(f); p2.attach(f);}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);}

	Rel2<GreaterEqual,Expr1,Expr2> getObj()  const
	{ 	return Rel2<GreaterEqual,Expr1,Expr2>(p1.getObj(),p2.getObj());	}

	Store&				store;
	BndView<Eval,Expr1>	p1;
	BndView<Eval,Expr2>	p2;
	INotifiable*	pOwner;
};

/**
 * 	ChkView over the inequality constraint.
 * 	\ingroup ChkViews
 */
template<class Eval,class Expr1,class Expr2>
struct ChkViewRel2<LessEqual,Eval,Expr1,Eval,Expr2> :
	ChkViewRel2<GreaterEqual,Eval,Expr2,Eval,Expr1>
{
	typedef ChkViewRel2<GreaterEqual,Eval,Expr2,Eval,Expr1>	Super;
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
			Super(store,p2,p1) {}
	Rel2<LessEqual,Expr1,Expr2> getObj()  const
	{ 	return Rel2<LessEqual,Expr1,Expr2>(Super::getObj().p2,Super::getObj().p1);	}

};

/**
 * 	ChkView over the inequality constraint.
 * 	\ingroup ChkViews
 */
template<class Eval,class Expr1,class Expr2>
struct ChkViewRel2<Less,Eval,Expr1,Eval,Expr2> :
	ChkViewRel1<Not,bool,Rel2<GreaterEqual,Expr1,Expr2> >
{
	typedef ChkViewRel1<Not,bool,Rel2<GreaterEqual,Expr1,Expr2> >	Super;
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
		Super(store,rel<GreaterEqual>(p1,p2)) {}
	Rel2<Less,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Less,Expr1,Expr2>(Super::getObj().p1.p1,Super::getObj().p1.p2);	}
};

/**
 * 	ChkView over the inequality constraint.
 * 	\ingroup ChkViews
 */
template<class Eval,class Expr1,class Expr2>
struct ChkViewRel2<Greater,Eval,Expr1,Eval,Expr2> :
	ChkViewRel1<Not,bool,Rel2<LessEqual,Expr1,Expr2> >
{
	typedef ChkViewRel1<Not,bool,Rel2<LessEqual,Expr1,Expr2> >	Super;
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
		Super(store,rel<LessEqual>(p1,p2)) {}
	Rel2<Greater,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Greater,Expr1,Expr2>(Super::getObj().p1.p1,Super::getObj().p1.p2);	}

};

// FIXME: rewrite the following checkers
/**
 * 	ChkView over the n-ary distinct constraint.
 * 	\ingroup ChkViews
 */
template<class Eval,class Expr1>
struct ChkViewRel1<Distinct,Seq<Eval>,Expr1>
{
	ChkViewRel1(Store& store, const Expr1& p1) : store(store),p1(p1) {}
	bool isTrue() const	// is it true?
	{	return false; }
	bool canBeTrue() const 	// can it still be true?
	{	return true; }
	bool setToTrue()
	{
		return store.post(rel<Distinct>(p1));
	}
	bool setToFalse()
	{
		assert(0); return false;
	}
//	Store& store() const {	return getState(p1);	}

	void attach(INotifiable* f) { 	}
	void detach(INotifiable* f) {	}

	Rel1<Distinct,Expr1> getObj()  const
	{ 	return Rel1<Distinct,Expr1>(p1);	}

	Store&	store;
	Expr1 p1;
};

/**
 * 	ChkView over the InTable constraint.
 * 	\ingroup ChkViews
 */
template<class Eval,class Expr1,class Expr2>
struct ChkViewRel2<InTable,Seq<Eval>,Expr1,Seq<Eval>,Expr2>
{
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) : store(store),p1(p1),p2(p2) {}
	bool isTrue() const	// is it true?
	{	return false; }
	bool canBeTrue() const 	// can it still be true?
	{	return true; }
	bool setToTrue()
	{
		return store.post(rel<InTable>(p1,p2));
	}
	bool setToFalse()
	{
		assert(0); return false;
	}
//	Store& store() const {	return getState(p1);	}

	void attach(INotifiable* f) { 	}
	void detach(INotifiable* f) {	}

	Rel2<InTable,Expr1,Expr2> getObj()  const
	{ 	return Rel2<InTable,Expr1,Expr2>(p1,p2);	}

	Store&	store;
	Expr1 p1;
	Expr2 p2;
};

/**
 * 	ChkView over the SumEqual constraint.
 * 	\ingroup ChkViews
 */
template<class Eval,class Expr1,class Expr2>
struct ChkViewRel2<SumEqual,Seq<Eval>,Expr1,Eval,Expr2>
{
	ChkViewRel2(Store& store, const Expr1& p1, const Expr2& p2) : store(store),p1(p1),p2(p2) {}
	bool isTrue() const	// is it true?
	{	return false; }
	bool canBeTrue() const 	// can it still be true?
	{	return true; }
	bool setToTrue()
	{
		return store.post(rel<SumEqual>(p1,p2));
	}
	bool setToFalse()
	{
		assert(0); return false;
	}
//	Store& store() const {	return getState(p1);	}

	void attach(INotifiable* f) { 	}
	void detach(INotifiable* f) {	}

	Rel2<SumEqual,Expr1,Expr2> getObj()  const
	{ 	return Rel2<SumEqual,Expr1,Expr2>(p1);	}

	Store&	store;
	Expr1 p1;
	Expr2 p2;
};

// This class adapts bnd views over boolean expressions to work as checkers
// note: this is not enabled for all checkers to avoid complex compiler error
// messages when user makes a mistake. To use it see example below (IfThenElse).
template<class Rel>
struct BndView2ChkView
{
	BndView2ChkView(Store& s,const Rel& rel) : s(s),v(s,rel) {}
	bool isTrue() const	// is it true?
	{	return v.min();	}
	bool canBeTrue() const 	// can it still be true?
	{	return v.max();	}
	bool setToTrue()
	{	v.detach(pOwner); return s.post(true==v.getObj());	}
	bool setToFalse()
	{	v.detach(pOwner); return s.post(false==v.getObj());	}

	void attach(INotifiable* f) { 	pOwner=f; v.attach(f);}
	void detach(INotifiable* f) {	v.detach(f); }

	Rel getObj()  const
	{ 	return v.getObj();	}

	Store& s;
	BndView<bool,Rel> v;
	INotifiable* pOwner;
};

/**
 * 	ChkView over the IfThenElse Boolean constraint.
 * 	\ingroup ChkViews
 */
template<class T1,class T2,class T3>
struct ChkViewRel3<IfThenElse,bool,T1,bool,T2,bool,T3> :
		BndView2ChkView<Rel3<IfThenElse,T1,T2,T3> >
{
	ChkViewRel3(Store& s, const T1& t1,const T2& t2, const T3& t3) :
		BndView2ChkView<Rel3<IfThenElse,T1,T2,T3> >(s,rel<IfThenElse>(t1,t2,t3)) {}
};

/**
 * 	ChkView over the subscript constraint (element).
 * 	\ingroup ChkViews
 */
template<class T1,class T2>
struct ChkViewRel2<Element,Seq<bool>,T1,int,T2> :
		BndView2ChkView<Rel2<Element,T1,T2> >
{
	ChkViewRel2(Store& s, const T1& t1,const T2& t2) :
		BndView2ChkView<Rel2<Element,T1,T2> >(s,rel<Element>(t1,t2)) {}
};

/**
 * 	ChkView over a cast expression.
 * 	\ingroup ChkViews
 */
template<class Expr1>
struct ChkViewRel1<Cast<bool>,int,Expr1>
{
	ChkViewRel1(Store& store, const Expr1& p1) :
			v(store,p1) {}
	bool isTrue() const	// is it true?
	{	return v.max()==v.min() and v.max()==1;	}
	bool canBeTrue() const 	// can it still be true?
	{	return v.max()>=1 and v.min()<=1;	}
	bool setToTrue()
	{	return v.updateRange(1,1);	}
	bool setToFalse()
	{
		if (v.min()==1)
			return v.updateMin(2);
		if (v.max()==1)
			return v.updateMax(0);
		return true;
	}

	void attach(INotifiable* f) { 	v.attach(f);}
	void detach(INotifiable* f) {	v.detach(f);}

	Rel1<Cast<bool>,Expr1> getObj()  const
	{ 	return rel<cast<bool>>(v.getObj());	}

	BndView<int,Expr1>	v;
};

} // CP
} // Casper

#endif /* CASPER_CP_VIEW_CHK_H_ */
