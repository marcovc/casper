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

#ifndef _H_CASPER_KERNEL_PAR_VIEW
#define _H_CASPER_KERNEL_PAR_VIEW

#include <casper/kernel/par/par.h>

namespace Casper {


/********************
 * Views over parameterized expressions
 *********************/


template<class>	class Par;
template<class,class,class>			struct ConstParView1;
template<class,class,class,class>	struct ConstParView2;
template<class,class,class,class,class>	struct ConstParView3;

/**
 * 	ConstParView over generic object. View must be convertible to Eval.
 * 	\ingroup Views
 **/
template<class Eval,class View>
struct ConstParView : IConstPar<Eval>
{
	ConstParView(State& state, const View& p1) :
		p1(p1) {}
	Eval value() const { return p1; }

//	void attach(INotifiable*) {}
//	void detach(INotifiable*) {}

	const View& getObj()  const { return p1; }

	const View	p1;
};

/**
 * 	ConstParView over a Par.
 * 	\ingroup Views
 **/
template<class Eval,class T>
struct ConstParView<Eval,Par<T> > : IConstPar<Eval>
{
	ConstParView(State& state,const Par<T>& p1) :
		p1(p1) {}
	Eval value() const { return p1.value(); }
//	void attach(INotifiable* f) { p1.attachOnUpdate(f); }
//	void detach(INotifiable* f) { p1.detachOnUpdate(f); }
	const Par<T>& getObj()  const { return p1; }
	Par<T>	p1;
};

/**
 * 	ConstParView over a Rel1 relation -> defer to ConstParView1.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View>
struct ConstParView<Eval,Rel1<F,View> > : ConstParView1<F,View,Eval>
{
//	using ConstParView1<F,View,Eval>::attach;
//	using ConstParView1<F,View,Eval>::detach;

	ConstParView(State& state, const Rel1<F,View>& r) :
		ConstParView1<F,View,Eval>(state,r.p1) {}
	// not sure if the below constructor is ever used
	ConstParView(State& state,const ConstParView1<F,View,Eval>& v) :
		ConstParView1<F,View,Eval>(v) {}
};

/**
 * 	ConstParView over a Rel2 relation -> defer to ConstParView2.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View1,class View2>
struct ConstParView<Eval,Rel2<F,View1,View2> > :
	ConstParView2<F,View1,View2,Eval>
{
//	using ConstParView2<F,View1,View2,Eval>::attach;
//	using ConstParView2<F,View1,View2,Eval>::detach;

	ConstParView(State& state, const Rel2<F,View1,View2>& r) :
		ConstParView2<F,View1,View2,Eval>(state,r.p1,r.p2) {}
	// not sure if the below constructor is ever used
	ConstParView(State& state, const ConstParView2<F,View1,View2,Eval>& v) :
		ConstParView2<F,View1,View2,Eval>(v) {}
};

/**
 * 	ConstParView over a Rel3 relation -> defer to ConstParView3.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View1,class View2,class View3>
struct ConstParView<Eval,Rel3<F,View1,View2,View3> > :
	ConstParView3<F,View1,View2,View3,Eval>
{
//	using ConstParView3<F,View1,View2,View3,Eval>::attach;
//	using ConstParView3<F,View1,View2,View3,Eval>::detach;

	ConstParView(State& state, const Rel3<F,View1,View2,View3>& r) :
		ConstParView3<F,View1,View2,View3,Eval>(state,r.p1,r.p2,r.p3) {}
};


/**
 * 	ConstParView over symmetric.
 * 	\ingroup Views
 **/
template<class View,class Eval>
struct ConstParView1<Sym,View,Eval> : IConstPar<Eval>
{
	ConstParView1(State& state, const View& p1) :
		p1(state,p1)	{}
	Eval value() const { return -p1.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); 	}
//	void detach(INotifiable* f)	{	p1.detach(f);  }
	Rel1<Sym,View> getObj()  const { return -(p1.getObj()); }
	ConstParView<Eval,View>	p1;
};

// logical

/**
 * 	ConstParView over negation.
 * 	\ingroup Views
 **/
template<class View>
struct ConstParView1<Not,View,bool> : IConstPar<bool>
{
	ConstParView1(State& state, const View& p1) :
		p1(state,p1)	{}
	bool value() const { return !p1.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); 	}
//	void detach(INotifiable* f)	{	p1.detach(f);  }
	Rel1<Not,View> getObj()  const { return not p1.getObj(); }
	ConstParView<bool,View>	p1;
};

/**
 * 	ConstParView over conjunction.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ConstParView2<And,View1,View2,bool> : IConstPar<bool>
{
	ConstParView2(State& state,const View1& p1,const View2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() && p2.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}
	Rel2<And,View1,View2> getObj()  const
	{ return p1.getObj() and p2.getObj(); }
	ConstParView<bool,View1>	p1;
	ConstParView<bool,View2>	p2;
};

/**
 * 	ConstParView over disjunction.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ConstParView2<Or,View1,View2,bool> : IConstPar<bool>
{
	ConstParView2(State& state,const View1& p1, const View2& p2) :
		p1(state,p1),p2(state,p2) 	{}
	bool value() const { return p1.value() || p2.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Or,View1,View2> getObj()  const
	{ return p1.getObj() or p2.getObj(); }

	ConstParView<bool,View1>	p1;
	ConstParView<bool,View2>	p2;
};

// arithmetic

/**
 * 	ConstParView over addition.
 * 	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct ConstParView2<Add,View1,View2,Eval> : IConstPar<Eval>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ConstParView2(State& state,const View1& p1, const View2& p2) :
		p1(state,p1),p2(state,p2) {}
	Eval value() const { return p1.value() + p2.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Add,View1,View2> getObj()  const
	{ return p1.getObj()+p2.getObj(); }

	ConstParView<View1Eval,View1>	p1;
	ConstParView<View2Eval,View2>	p2;
};

/**
 * 	ConstParView over subtraction.
 * 	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct ConstParView2<Sub,View1,View2,Eval> : IConstPar<Eval>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ConstParView2(State& state,const View1& p1, const View2& p2) :
		p1(state,p1),p2(state,p2) {}
	Eval value() const { return p1.value() - p2.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Sub,View1,View2> getObj()  const
	{ return p1.getObj()-p2.getObj(); }

	ConstParView<View1Eval,View1>	p1;
	ConstParView<View2Eval,View2>	p2;
};

/**
 * 	ConstParView over multiplication.
 * 	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct ConstParView2<Mul,View1,View2,Eval> : IConstPar<Eval>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ConstParView2(State& state, const View1& p1, const View2& p2) :
		p1(state,p1),p2(state,p2) {}
	Eval value() const { return p1.value() * p2.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Mul,View1,View2> getObj()  const
	{ return p1.getObj()*p2.getObj(); }

	ConstParView<View1Eval,View1>	p1;
	ConstParView<View2Eval,View2>	p2;
};

/**
 * 	ConstParView over equality relation.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ConstParView2<Equal,View1,View2,bool> : IConstPar<bool>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ConstParView2(State& state,const View1& p1, const View2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() == p2.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Equal,View1,View2> getObj()  const
	{ return p1.getObj()==p2.getObj(); }

	ConstParView<View1Eval,View1>	p1;
	ConstParView<View2Eval,View2>	p2;
};

/**
 * 	ConstParView over inequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ConstParView2<Greater,View1,View2,bool> : IConstPar<bool>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ConstParView2(State& state, const View1& p1, const View2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() > p2.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Greater,View1,View2> getObj()  const
	{ return p1.getObj()>p2.getObj(); }

	ConstParView<View1Eval,View1>	p1;
	ConstParView<View2Eval,View2>	p2;
};

/**
 * 	ConstParView over inequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ConstParView2<GreaterEqual,View1,View2,bool> : IConstPar<bool>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ConstParView2(State& state,const View1& p1, const View2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() >= p2.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<GreaterEqual,View1,View2> getObj()  const
	{ return p1.getObj()>=p2.getObj(); }

	ConstParView<View1Eval,View1>	p1;
	ConstParView<View2Eval,View2>	p2;
};

/**
 * 	ConstParView over inequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ConstParView2<Less,View1,View2,bool> : IConstPar<bool>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ConstParView2(State& state,const View1& p1, const View2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() < p2.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<Less,View1,View2> getObj()  const
	{ return p1.getObj()<p2.getObj(); }

	ConstParView<View1Eval,View1>	p1;
	ConstParView<View2Eval,View2>	p2;
};

/**
 * 	ConstParView over inequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ConstParView2<LessEqual,View1,View2,bool> : IConstPar<bool>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ConstParView2(State& state,const View1& p1, const View2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() <= p2.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<LessEqual,View1,View2> getObj()  const
	{ return p1.getObj()<=p2.getObj(); }

	ConstParView<View1Eval,View1>	p1;
	ConstParView<View2Eval,View2>	p2;
};

/**
 * 	ConstParView over disequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ConstParView2<Distinct,View1,View2,bool> : IConstPar<bool>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ConstParView2(State& state,const View1& p1, const View2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() != p2.value(); }
//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<Distinct,View1,View2> getObj()  const
	{ return p1.getObj()!=p2.getObj(); }

	ConstParView<View1Eval,View1>	p1;
	ConstParView<View2Eval,View2>	p2;
};


};

#endif /*_H_CASPER_KERNEL_VIEW_PAR */
