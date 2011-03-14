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

#include <casper/kernel/par/parexpr.h>

namespace Casper {


/********************
 * Views over parameterized expressions
 *********************/


template<class>	class Par;
template<class,class,class>			struct ParViewRel1;
template<class,class,class,class>	struct ParViewRel2;
template<class,class,class,class,class>	struct ParViewRel3;

/**
 * 	ParView over generic object. View must be convertible to Eval.
 * 	\ingroup Views
 **/
template<class Eval,class View>
struct ParView : IParExpr<Eval>
{
	ParView(Env& env, const View& p1) :
		IParExpr<Eval>(env),p1(p1) {}
	Eval value() const { return p1; }

	void attach(INotifiable*) {}
	void detach(INotifiable*) {}

	const View& getObj()  const { return p1; }

	const View	p1;
};

/**
 * 	ParView over a Par.
 * 	\ingroup Views
 **/
template<class Eval,class T>
struct ParView<Eval,Par<T> > : IParExpr<Eval>
{
	ParView(Env& env,const Par<T>& p1) :
		IParExpr<Eval>(env),p1(p1) {}
	Eval value() const { return p1.value(); }
	void attach(INotifiable* f) { p1.attachOnUpdate(f); }
	void detach(INotifiable* f) { p1.detachOnUpdate(f); }
	const Par<T>& getObj()  const { return p1; }
	Par<T>	p1;
};

/**
 * 	ParView over a Rel1 relation -> defer to ParViewRel1.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View>
struct ParView<Eval,Rel1<F,View> > : ParViewRel1<F,View,Eval>
{
	using ParViewRel1<F,View,Eval>::attach;
	using ParViewRel1<F,View,Eval>::detach;

	ParView(Env& env, const Rel1<F,View>& r) :
		ParViewRel1<F,View,Eval>(env,r.p1) {}
	// not sure if the below constructor is ever used
	ParView(Env& env,const ParViewRel1<F,View,Eval>& v) :
		ParViewRel1<F,View,Eval>(v) {}
};

/**
 * 	ParView over a Rel2 relation -> defer to ParViewRel2.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View1,class View2>
struct ParView<Eval,Rel2<F,View1,View2> > :
	ParViewRel2<F,View1,View2,Eval>
{
	using ParViewRel2<F,View1,View2,Eval>::attach;
	using ParViewRel2<F,View1,View2,Eval>::detach;

	ParView(Env& env, const Rel2<F,View1,View2>& r) :
		ParViewRel2<F,View1,View2,Eval>(env,r.p1,r.p2) {}
	// not sure if the below constructor is ever used
	ParView(Env& env, const ParViewRel2<F,View1,View2,Eval>& v) :
		ParViewRel2<F,View1,View2,Eval>(v) {}
};

/**
 * 	ParView over a Rel3 relation -> defer to ParViewRel3.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View1,class View2,class View3>
struct ParView<Eval,Rel3<F,View1,View2,View3> > :
	ParViewRel3<F,View1,View2,View3,Eval>
{
	using ParViewRel3<F,View1,View2,View3,Eval>::attach;
	using ParViewRel3<F,View1,View2,View3,Eval>::detach;

	ParView(Env& env, const Rel3<F,View1,View2,View3>& r) :
		ParViewRel3<F,View1,View2,View3,Eval>(env,r.p1,r.p2,r.p3) {}
};

/**
 * 	ParView over a MutExpr.
 * 	\ingroup Views
 **/
template<class Eval>
struct ParView<Eval,ParExpr<Eval> > : ParExpr<Eval>
{
	using ParExpr<Eval>::attach;
	using ParExpr<Eval>::detach;

	ParView(Env& env, const ParExpr<Eval>& r) :
		ParExpr<Eval>(env,r) {}
};


/**
 * 	ParView over symmetric.
 * 	\ingroup Views
 **/
template<class View,class Eval>
struct ParViewRel1<Sym,View,Eval> : IParExpr<Eval>
{
	ParViewRel1(Env& env, const View& p1) :
		IParExpr<Eval>(env),p1(env,p1)	{}
	Eval value() const { return -p1.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); 	}
	void detach(INotifiable* f)	{	p1.detach(f);  }
	Rel1<Sym,View> getObj()  const { return -(p1.getObj()); }
	ParView<Eval,View>	p1;
};

// logical

/**
 * 	ParView over negation.
 * 	\ingroup Views
 **/
template<class View>
struct ParViewRel1<Not,View,bool> : IParExpr<bool>
{
	ParViewRel1(Env& env, const View& p1) :
		IParExpr<bool>(env),p1(env,p1)	{}
	bool value() const { return !p1.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); 	}
	void detach(INotifiable* f)	{	p1.detach(f);  }
	Rel1<Not,View> getObj()  const { return not p1.getObj(); }
	ParView<bool,View>	p1;
};

/**
 * 	ParView over conjunction.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ParViewRel2<And,View1,View2,bool> : IParExpr<bool>
{
	ParViewRel2(Env& env,const View1& p1,const View2& p2) :
		IParExpr<bool>(env),p1(env,p1),p2(env,p2) {}
	bool value() const { return p1.value() && p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}
	Rel2<And,View1,View2> getObj()  const
	{ return p1.getObj() and p2.getObj(); }
	ParView<bool,View1>	p1;
	ParView<bool,View2>	p2;
};

/**
 * 	ParView over disjunction.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ParViewRel2<Or,View1,View2,bool> : IParExpr<bool>
{
	ParViewRel2(Env& env,const View1& p1, const View2& p2) :
		IParExpr<bool>(env),p1(env,p1),p2(env,p2) 	{}
	bool value() const { return p1.value() || p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Or,View1,View2> getObj()  const
	{ return p1.getObj() or p2.getObj(); }

	ParView<bool,View1>	p1;
	ParView<bool,View2>	p2;
};

// arithmetic

/**
 * 	ParView over addition.
 * 	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct ParViewRel2<Add,View1,View2,Eval> : IParExpr<Eval>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ParViewRel2(Env& env,const View1& p1, const View2& p2) :
		IParExpr<Eval>(env),p1(env,p1),p2(env,p2) {}
	Eval value() const { return p1.value() + p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Add,View1,View2> getObj()  const
	{ return p1.getObj()+p2.getObj(); }

	ParView<View1Eval,View1>	p1;
	ParView<View2Eval,View2>	p2;
};

/**
 * 	ParView over subtraction.
 * 	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct ParViewRel2<Sub,View1,View2,Eval> : IParExpr<Eval>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ParViewRel2(Env& env,const View1& p1, const View2& p2) :
		IParExpr<Eval>(env),p1(env,p1),p2(env,p2) {}
	Eval value() const { return p1.value() - p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Sub,View1,View2> getObj()  const
	{ return p1.getObj()-p2.getObj(); }

	ParView<View1Eval,View1>	p1;
	ParView<View2Eval,View2>	p2;
};

/**
 * 	ParView over multiplication.
 * 	\ingroup Views
 **/
template<class View1,class View2,class Eval>
struct ParViewRel2<Mul,View1,View2,Eval> : IParExpr<Eval>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ParViewRel2(Env& env, const View1& p1, const View2& p2) :
		IParExpr<Eval>(env),p1(env,p1),p2(env,p2) {}
	Eval value() const { return p1.value() * p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Mul,View1,View2> getObj()  const
	{ return p1.getObj()*p2.getObj(); }

	ParView<View1Eval,View1>	p1;
	ParView<View2Eval,View2>	p2;
};

/**
 * 	ParView over equality relation.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ParViewRel2<Equal,View1,View2,bool> : IParExpr<bool>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ParViewRel2(Env& env,const View1& p1, const View2& p2) :
		IParExpr<bool>(env),p1(env,p1),p2(env,p2) {}
	bool value() const { return p1.value() == p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Equal,View1,View2> getObj()  const
	{ return p1.getObj()==p2.getObj(); }

	ParView<View1Eval,View1>	p1;
	ParView<View2Eval,View2>	p2;
};

/**
 * 	ParView over inequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ParViewRel2<Greater,View1,View2,bool> : IParExpr<bool>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ParViewRel2(Env& env, const View1& p1, const View2& p2) :
		IParExpr<bool>(env),p1(env,p1),p2(env,p2) {}
	bool value() const { return p1.value() > p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Greater,View1,View2> getObj()  const
	{ return p1.getObj()>p2.getObj(); }

	ParView<View1Eval,View1>	p1;
	ParView<View2Eval,View2>	p2;
};

/**
 * 	ParView over inequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ParViewRel2<GreaterEqual,View1,View2,bool> : IParExpr<bool>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ParViewRel2(Env& env,const View1& p1, const View2& p2) :
		IParExpr<bool>(env),p1(env,p1),p2(env,p2) {}
	bool value() const { return p1.value() >= p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<GreaterEqual,View1,View2> getObj()  const
	{ return p1.getObj()>=p2.getObj(); }

	ParView<View1Eval,View1>	p1;
	ParView<View2Eval,View2>	p2;
};

/**
 * 	ParView over inequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ParViewRel2<Less,View1,View2,bool> : IParExpr<bool>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ParViewRel2(Env& env,const View1& p1, const View2& p2) :
		IParExpr<bool>(env),p1(env,p1),p2(env,p2) {}
	bool value() const { return p1.value() < p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<Less,View1,View2> getObj()  const
	{ return p1.getObj()<p2.getObj(); }

	ParView<View1Eval,View1>	p1;
	ParView<View2Eval,View2>	p2;
};

/**
 * 	ParView over inequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ParViewRel2<LessEqual,View1,View2,bool> : IParExpr<bool>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ParViewRel2(Env& env,const View1& p1, const View2& p2) :
		IParExpr<bool>(env),p1(env,p1),p2(env,p2) {}
	bool value() const { return p1.value() <= p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<LessEqual,View1,View2> getObj()  const
	{ return p1.getObj()<=p2.getObj(); }

	ParView<View1Eval,View1>	p1;
	ParView<View2Eval,View2>	p2;
};

/**
 * 	ParView over disequality.
 * 	\ingroup Views
 **/
template<class View1,class View2>
struct ParViewRel2<Distinct,View1,View2,bool> : IParExpr<bool>
{
	typedef typename Traits::GetEval<View1>::Type	View1Eval;
	typedef typename Traits::GetEval<View2>::Type	View2Eval;

	ParViewRel2(Env& env,const View1& p1, const View2& p2) :
		IParExpr<bool>(env),p1(env,p1),p2(env,p2) {}
	bool value() const { return p1.value() != p2.value(); }
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<Distinct,View1,View2> getObj()  const
	{ return p1.getObj()!=p2.getObj(); }

	ParView<View1Eval,View1>	p1;
	ParView<View2Eval,View2>	p2;
};


};

#endif /*_H_CASPER_KERNEL_VIEW_PAR */
