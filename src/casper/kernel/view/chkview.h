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


#ifndef CASPER_KERNEL_VIEW_CHECKVIEW_H_
#define CASPER_KERNEL_VIEW_CHECKVIEW_H_

#include <casper/kernel/view/common.h>

namespace casper {

/* note: currently checkview is just a convenient wrapper around
 * bndview<P>, when P eval type is bool. The entailnment is
 * checked based on the bounds of the variables. This may not
 * be enough, so it could be useful to further specialize to
 * BndChkView and DomChkView, for example.
 */

template<class View>
struct ChkView
#ifdef CASPER_CPP0X
{	static_assert(sizeof(View)==0,"Undefined specialization of ChkView");	};
#else
;
#endif

template<class F,class Eval1,class View1>
struct ChkViewRel1
#ifdef CASPER_CPP0X
{	static_assert(sizeof(View1)==0,"Undefined specialization of ChkViewRel1");	};
#else
;
#endif

template<class F, class Eval1,class View1,class Eval2,class View2>
struct ChkViewRel2
#ifdef CASPER_CPP0X
{	static_assert(sizeof(View1)==0,"Undefined specialization of ChkViewRel2");	};
#else
;
#endif

template<class F, class Eval1,class View1,class Eval2,class View2,
				class Eval3,class View3>
struct ChkViewRel3
#ifdef CASPER_CPP0X
{	static_assert(sizeof(View1)==0,"Undefined specialization of ChkViewRel3");	};
#else
;
#endif

/**
 *	ChkView over a Rel1 relation -> defers to ChkViewRel1
 *	\ingroup Views
 **/
template<class F,class View>
struct ChkView<Rel1<F,View> > :
	ChkViewRel1<F,typename EvalTraits<View>::Eval,View>
{
	ChkView(ICPSolver& solver, const Rel1<F,View>& r) :
		ChkViewRel1<F,typename EvalTraits<View>::Eval,View>(solver,r.p1) {}
};

/**
 *	ChkView over a Rel2 relation -> defers to ChkViewRel2
 *	\ingroup Views
 **/
template<class F,class View1,class View2>
struct ChkView<Rel2<F,View1,View2> > :
		ChkViewRel2<F,typename EvalTraits<View1>::Eval,View1,
					  typename EvalTraits<View2>::Eval,View2>
{
	ChkView(ICPSolver& solver, const Rel2<F,View1,View2>& r) :
		ChkViewRel2<F,typename EvalTraits<View1>::Eval,View1,
					  typename EvalTraits<View2>::Eval,View2>(solver,r.p1,r.p2) {}
};

/**
 *	ChkView over a Rel3 relation -> defers to ChkViewRel3
 *	\ingroup Views
 **/
template<class F,class View1,class View2,class View3>
struct ChkView<Rel3<F,View1,View2,View3> > :
	ChkViewRel3<F,typename EvalTraits<View1>::Eval,View1,
					typename EvalTraits<View2>::Eval,View2,
					typename EvalTraits<View3>::Eval,View3>
{
	ChkView(ICPSolver& solver, const Rel3<F,View1,View2,View3>& r) :
		ChkViewRel3<F,typename EvalTraits<View1>::Eval,View1,
						typename EvalTraits<View2>::Eval,View2,
						typename EvalTraits<View3>::Eval,View3>(solver,r.p1,r.p2,r.p3)
						{}
};


// caches the value of the expression once it is set, and never
// asks it again from the viewed object
template<class View>
struct CChkView
{
	CChkView(ICPSolver& solver,const View& v) :
		v(solver,v),state(solver,2) {}
	Bool isTrue() const	// is it true?
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
	Bool canBeTrue() const 	// can it still be true?
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
	Bool setToTrue()
	{
		if (state!=2)
			return state==1;
		state=1;
		return v.setToTrue();
	}
	Bool setToFalse()
	{
		if (state!=2)
			return state==0;
		state=0;
		return v.setToFalse();
	}
	ICPSolver& solver() const {	return v.solver();	}

	void attach(INotifiable* f) { 	v.attach(f);}
	void detach(INotifiable* f) {	v.detach(f);}

	View getView()  const
	{ 	return v.getView();	}

	ChkView<View>	v;

	mutable Reversible<Int>	state;

};

// -------- reification ---------

// chkview over constast
template<>
struct ChkView<Bool>
{
	ChkView(ICPSolver& solver, const Bool& b) :
			mSolver(solver),b(b) {}
	Bool isTrue() const	// is it true?
	{	return b;	}
	Bool canBeTrue() const 	// can it still be true?
	{	return b;	}
	Bool setToTrue()
	{	return b;	}
	Bool setToFalse()
	{	return !b;	}
	ICPSolver& solver() const {	return mSolver;	}

	void attach(INotifiable* f) { 	}
	void detach(INotifiable* f) {	}

	Bool getView()  const
	{ 	return b;	}

	ICPSolver& mSolver;
	const Bool b;
};

// chkview over bndexpr
template<class Eval>
struct ChkView<BndExpr<Eval> >
{
	ChkView(ICPSolver& solver, const BndExpr<Eval>& p1) :
			v(p1) {}
	Bool isTrue() const	// is it true?
	{	return v.min();	}
	Bool canBeTrue() const 	// can it still be true?
	{	return v.max();	}
	Bool setToTrue()
	{	return v.updateMin(true);	}
	Bool setToFalse()
	{	return v.updateMax(false);	}
	ICPSolver& solver() const {	return v.solver();	}

	void attach(INotifiable* f) { 	v.attach(f);}
	void detach(INotifiable* f) {	v.detach(f);}

	BndExpr<Eval> getView()  const
	{ 	return v;	}

	BndExpr<Eval>	v;
};

// chkview over domvar
template<class Dom>
struct ChkView<DomVar<Bool,Dom> >
{
	ChkView(ICPSolver& solver, const DomVar<Bool,Dom>& p1) :
			v(solver,p1) {}
	Bool isTrue() const	// is it true?
	{	return v.min();	}
	Bool canBeTrue() const 	// can it still be true?
	{	return v.max();	}
	Bool setToTrue()
	{	return v.updateMin(true);	}
	Bool setToFalse()
	{	return v.updateMax(false);	}
	ICPSolver& solver() const {	return v.solver();	}

	void attach(INotifiable* f) { 	v.attach(f);}
	void detach(INotifiable* f) {	v.detach(f);}

	DomVar<Bool,Dom> getView()  const
	{ 	return v.getView();	}

	BndView<Bool,DomVar<Bool,Dom> >	v;
};

// reification of not
template<class View1>
struct ChkViewRel1<Not,Bool,View1>
{
	ChkViewRel1(ICPSolver& solver, const View1& p1) :
			v(solver,p1) {}
	Bool isTrue() const	// is it true?
	{	return !v.canBeTrue();	}
	Bool canBeTrue() const 	// can it still be true?
	{	return !v.isTrue();	}
	Bool setToTrue()
	{	return v.setToFalse();	}
	Bool setToFalse()
	{	return v.setToTrue();	}
	ICPSolver& solver() const {	return v.solver();	}

	void attach(INotifiable* f) { 	v.attach(f);}
	void detach(INotifiable* f) {	v.detach(f);}

	View1 getView()  const
	{ 	return v.getView();	}

	ChkView<View1>	v;
};

// reification of and
template<class View1,class View2>
struct ChkViewRel2<And,Bool,View1,Bool,View2>
{
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
			p1(solver,p1),p2(solver,p2) {}
	Bool isTrue() const	// is it true?
	{	return p1.isTrue() and p2.isTrue();	}
	Bool canBeTrue() const 	// can it still be true?
	{	return p1.canBeTrue() and p2.canBeTrue();	}
	Bool setToTrue()
	{	return p1.setToTrue() and p2.setToTrue();	}
	Bool setToFalse()
	{
		detach(pOwner);
		return solver().post(rel<Or>(!p1.getView(),!p2.getView()));
	}
	ICPSolver& solver() const {	return p1.solver();	}

	void attach(INotifiable* f) { 	pOwner=f; p1.attach(f); p2.attach(f);}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);}

	Rel2<And,View1,View2> getView()  const
	{ 	return Rel2<And,View1,View2>(p1.getView(),p2.getView());	}

	ChkView<View1>	p1;
	ChkView<View2>	p2;
	INotifiable*	pOwner;
};

// reification of or
template<class View1,class View2>
struct ChkViewRel2<Or,Bool,View1,Bool,View2>
{
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
			p1(solver,p1),p2(solver,p2) {}
	Bool isTrue() const	// is it true?
	{	return p1.isTrue() or p2.isTrue();	}
	Bool canBeTrue() const 	// can it still be true?
	{	return p1.canBeTrue() or p2.canBeTrue();	}
	Bool setToTrue()
	{
		detach(pOwner);
		return solver().post(rel<Or>(p1.getView(),p2.getView()));
	}
	Bool setToFalse()
	{
		detach(pOwner);
		//return solver().post(!p1.getView() and !p2.getView());
		return p1.setToFalse() and p2.setToFalse();
	}
	ICPSolver& solver() const {	return p1.solver();	}

	void attach(INotifiable* f) { 	pOwner=f; p1.attach(f); p2.attach(f);}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);}

	Rel2<Or,View1,View2> getView()  const
	{ 	return Rel2<Or,View1,View2>(p1.getView(),p2.getView());	}

	ChkView<View1>	p1;
	ChkView<View2>	p2;
	INotifiable*	pOwner;
};

// reification of xor
template<class View1,class View2>
struct ChkViewRel2<XOr,Bool,View1,Bool,View2> :
	ChkViewRel2<Distinct,Bool,View1,Bool,View2>
{
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
		ChkViewRel2<Distinct,Bool,View1,Bool,View2>(solver,p1,p2) {}
};

// reification of equal
template<class Eval,class View1,class View2>
struct ChkViewRel2<Equal,Eval,View1,Eval,View2>
{
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
			p1(solver,p1),p2(solver,p2) {}
	Bool isTrue() const	// is it true?
	{	return p1.max()==p2.min() and p1.min()==p2.max();	}
	Bool canBeTrue() const 	// can it still be true?
	{	return p1.max()>=p2.min() and p1.min()<=p2.max();	}
	Bool setToTrue()
	{
		detach(pOwner);
		return p1.solver().post(rel<Equal>(p1.getView(),p2.getView()));
	}
	Bool setToFalse()
	{
		detach(pOwner);
		return p1.solver().post(rel<Distinct>(p1.getView(),p2.getView()));
	}
	ICPSolver& solver() const {	return p1.solver();	}

	void attach(INotifiable* f) { 	pOwner=f; p1.attach(f); p2.attach(f);}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);}

	Rel2<Equal,View1,View2> getView()  const
	{ 	return Rel2<Equal,View1,View2>(p1.getView(),p2.getView());	}

	BndView<Eval,View1>	p1;
	BndView<Eval,View2>	p2;
	INotifiable*	pOwner;
};

// reification of distinct

template<class Eval,class View1,class View2>
struct ChkViewRel2<Distinct,Eval,View1,Eval,View2> :
	ChkViewRel1<Not,Bool,Rel2<Equal,View1,View2> >
{
	typedef ChkViewRel1<Not,Bool,Rel2<Equal,View1,View2> >	Super;
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
		Super(solver,rel<Equal>(p1,p2)) {}
};

// reification of greater equal

template<class Eval,class View1,class View2>
struct ChkViewRel2<GreaterEqual,Eval,View1,Eval,View2>
{
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
			p1(solver,p1),p2(solver,p2) {}
	Bool isTrue() const	// is it true?
	{	return p1.min()>=p2.max();	}
	Bool canBeTrue() const 	// can it still be true?
	{	return p1.max()>=p2.min();	}
	Bool setToTrue()
	{
		detach(pOwner);
		return solver().post(rel<GreaterEqual>(p1.getView(),p2.getView()));
	}
	Bool setToFalse()
	{
		detach(pOwner);
		return solver().post(rel<Less>(p1.getView(),p2.getView()));
	}
	ICPSolver& solver() const {	return getCPSolver(p1,p2);	}

	void attach(INotifiable* f) { 	pOwner=f; p1.attach(f); p2.attach(f);}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);}

	Rel2<GreaterEqual,View1,View2> getView()  const
	{ 	return Rel2<GreaterEqual,View1,View2>(p1.getView(),p2.getView());	}

	BndView<Eval,View1>	p1;
	BndView<Eval,View2>	p2;
	INotifiable*	pOwner;
};

// reification of lessEqual

template<class Eval,class View1,class View2>
struct ChkViewRel2<LessEqual,Eval,View1,Eval,View2> :
	ChkViewRel2<GreaterEqual,Eval,View2,Eval,View1>
{
	typedef ChkViewRel2<GreaterEqual,Eval,View2,Eval,View1>	Super;
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
			Super(solver,p2,p1) {}
};

// reification of less

template<class Eval,class View1,class View2>
struct ChkViewRel2<Less,Eval,View1,Eval,View2> :
	ChkViewRel1<Not,Bool,Rel2<GreaterEqual,View1,View2> >
{
	typedef ChkViewRel1<Not,Bool,Rel2<GreaterEqual,View1,View2> >	Super;
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
		Super(solver,rel<GreaterEqual>(p1,p2)) {}
};

// reification of greater

template<class Eval,class View1,class View2>
struct ChkViewRel2<Greater,Eval,View1,Eval,View2> :
	ChkViewRel1<Not,Bool,Rel2<LessEqual,View1,View2> >
{
	typedef ChkViewRel1<Not,Bool,Rel2<LessEqual,View1,View2> >	Super;
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) :
		Super(solver,rel<LessEqual>(p1,p2)) {}
};

// FIXME: rewrite the following checkers

template<class Eval,class View1>
struct ChkViewRel1<Distinct,Seq<Eval>,View1>
{
	ChkViewRel1(ICPSolver& solver, const View1& p1) : p1(p1) {}
	Bool isTrue() const	// is it true?
	{	return false; }
	Bool canBeTrue() const 	// can it still be true?
	{	return true; }
	Bool setToTrue()
	{
		return solver().post(rel<Distinct>(p1));
	}
	Bool setToFalse()
	{
		assert(0); return false;
	}
	ICPSolver& solver() const {	return getCPSolver(p1);	}

	void attach(INotifiable* f) { 	}
	void detach(INotifiable* f) {	}

	Rel1<Distinct,View1> getView()  const
	{ 	return Rel1<Distinct,View1>(p1);	}

	View1 p1;
};

template<class Eval,class View1,class View2>
struct ChkViewRel2<InTable,Seq<Eval>,View1,Seq<Eval>,View2>
{
	ChkViewRel2(ICPSolver& solver, const View1& p1,const View2& p2) : p1(p1),p2(p2) {}
	Bool isTrue() const	// is it true?
	{	return false; }
	Bool canBeTrue() const 	// can it still be true?
	{	return true; }
	Bool setToTrue()
	{
		return solver().post(rel<InTable>(p1,p2));
	}
	Bool setToFalse()
	{
		assert(0); return false;
	}
	ICPSolver& solver() const {	return getCPSolver(p1);	}

	void attach(INotifiable* f) { 	}
	void detach(INotifiable* f) {	}

	Rel2<InTable,View1,View2> getView()  const
	{ 	return Rel2<InTable,View1,View2>(p1,p2);	}

	View1 p1;
	View2 p2;
};

}

#endif /* CASPER_KERNEL_VIEW_CHECKVIEW_H_ */
