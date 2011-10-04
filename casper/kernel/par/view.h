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

#ifndef _H_CASPER_KERNEL_PAR_VIEW
#define _H_CASPER_KERNEL_PAR_VIEW

#include <casper/kernel/par/par.h>
#include <casper/kernel/par/piteration.h>

namespace Casper {


/********************
 * Views over parameterized expressions
 *********************/


template<class>	struct Par;
template<class,class,class>			struct ParView1;
template<class,class,class,class>	struct ParView2;
template<class,class,class,class,class>	struct ParView3;

/**
 * 	ParView over generic object. View must be convertible to Eval.
 * 	\ingroup Views
 **/
template<class Eval,class View>
struct ParView : IPar<Eval>
{
	ParView(State& state, const View& p1) :
		p1(p1) {}
	Eval value() const { return p1; }

//	void attach(INotifiable*) {}
//	void detach(INotifiable*) {}

	const View& getObj()  const { return p1; }

	const View	p1;
};

/**
 * 	ParView over a Par.
 * 	\ingroup Views
 **/
template<class Eval,class T>
struct ParView<Eval,Par<T> > : IPar<Eval>
{
	ParView(State& state,const Par<T>& p1) :
		p1(p1) {}
	Eval value() const { return p1.value(); }
	void setValue(const Eval& v)
	{	p1 = v;	}

//	void attach(INotifiable* f) { p1.attach(f); }
//	void detach(INotifiable* f) { p1.detach(f); }

	const Par<T>& getObj()  const { return p1; }
	Par<T>	p1;
};

/**
 * 	ParView over a Rel1 relation -> defer to ParView1.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View>
struct ParView<Eval,Rel1<F,View> > : ParView1<F,View,Eval>
{
//	using ParView1<F,View,Eval>::attach;
//	using ParView1<F,View,Eval>::detach;

	ParView(State& state, const Rel1<F,View>& r) :
		ParView1<F,View,Eval>(state,r.p1) {}
	// not sure if the below constructor is ever used
	ParView(State& state,const ParView1<F,View,Eval>& v) :
		ParView1<F,View,Eval>(v) {}
};

/**
 * 	ParView over a Rel2 relation -> defer to ParView2.
 * 	\ingroup Views
 **/
template<class Eval,class F,class Expr1,class Expr2>
struct ParView<Eval,Rel2<F,Expr1,Expr2> > :
	ParView2<F,Expr1,Expr2,Eval>
{
//	using ParView2<F,Expr1,Expr2,Eval>::attach;
//	using ParView2<F,Expr1,Expr2,Eval>::detach;

	ParView(State& state, const Rel2<F,Expr1,Expr2>& r) :
		ParView2<F,Expr1,Expr2,Eval>(state,r.p1,r.p2) {}
	// not sure if the below constructor is ever used
	ParView(State& state, const ParView2<F,Expr1,Expr2,Eval>& v) :
		ParView2<F,Expr1,Expr2,Eval>(v) {}
};

/**
 * 	ParView over a Rel3 relation -> defer to ParView3.
 * 	\ingroup Views
 **/
template<class Eval,class F,class Expr1,class Expr2,class Expr3>
struct ParView<Eval,Rel3<F,Expr1,Expr2,Expr3> > :
	ParView3<F,Expr1,Expr2,Expr3,Eval>
{
//	using ParView3<F,Expr1,Expr2,Expr3,Eval>::attach;
//	using ParView3<F,Expr1,Expr2,Expr3,Eval>::detach;

	ParView(State& state, const Rel3<F,Expr1,Expr2,Expr3>& r) :
		ParView3<F,Expr1,Expr2,Expr3,Eval>(state,r.p1,r.p2,r.p3) {}
};


/**
 * 	ParView over symmetric.
 * 	\ingroup Views
 **/
template<class View,class Eval>
struct ParView1<Sym,View,Eval> : IPar<Eval>
{
	ParView1(State& state, const View& p1) :
		p1(state,p1)	{}
	Eval value() const { return -p1.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); 	}
//	void detach(INotifiable* f)	{	p1.detach(f);  }

	Rel1<Sym,View> getObj()  const { return -(p1.getObj()); }
	ParView<Eval,View>	p1;
};

template<class View,class Eval>
struct ParView1<Abs,View,Eval> : IPar<Eval>
{
	ParView1(State& state, const View& p1) :
		p1(state,p1)	{}
	Eval value() const
	{
		Eval r = p1.value();
		return (r>=0)?r:-r;
	}
//	void attach(INotifiable* f) { 	p1.attach(f); 	}
//	void detach(INotifiable* f)	{	p1.detach(f);  }

	Rel1<Abs,View> getObj()  const { return abs(p1.getObj()); }
	ParView<Eval,View>	p1;
};

// logical

/**
 * 	ParView over negation.
 * 	\ingroup Views
 **/
template<class View>
struct ParView1<Not,View,bool> : IPar<bool>
{
	ParView1(State& state, const View& p1) :
		p1(state,p1)	{}
	bool value() const { return !p1.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); 	}
//	void detach(INotifiable* f)	{	p1.detach(f);  }

	Rel1<Not,View> getObj()  const { return not p1.getObj(); }
	ParView<bool,View>	p1;
};

/**
 * 	ParView over conjunction.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct ParView2<And,Expr1,Expr2,bool> : IPar<bool>
{
	ParView2(State& state,const Expr1& p1,const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() && p2.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<And,Expr1,Expr2> getObj()  const
	{ return p1.getObj() and p2.getObj(); }
	ParView<bool,Expr1>	p1;
	ParView<bool,Expr2>	p2;
};

/**
 * 	ParView over disjunction.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct ParView2<Or,Expr1,Expr2,bool> : IPar<bool>
{
	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) 	{}
	bool value() const { return p1.value() || p2.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<Or,Expr1,Expr2> getObj()  const
	{ return p1.getObj() or p2.getObj(); }

	ParView<bool,Expr1>	p1;
	ParView<bool,Expr2>	p2;
};

// arithmetic

/**
 * 	ParView over addition.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2,class Eval>
struct ParView2<Add,Expr1,Expr2,Eval> : IPar<Eval>
{
	typedef typename Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Traits::GetEval<Expr2>::Type	View2Eval;

	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	Eval value() const { return p1.value() + p2.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<Add,Expr1,Expr2> getObj()  const
	{ return p1.getObj()+p2.getObj(); }

	ParView<View1Eval,Expr1>	p1;
	ParView<View2Eval,Expr2>	p2;
};

/**
 * 	ParView over subtraction.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2,class Eval>
struct ParView2<Sub,Expr1,Expr2,Eval> : IPar<Eval>
{
	typedef typename Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Traits::GetEval<Expr2>::Type	View2Eval;

	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	Eval value() const { return p1.value() - p2.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<Sub,Expr1,Expr2> getObj()  const
	{ return p1.getObj()-p2.getObj(); }

	ParView<View1Eval,Expr1>	p1;
	ParView<View2Eval,Expr2>	p2;
};

/**
 * 	ParView over multiplication.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2,class Eval>
struct ParView2<Mul,Expr1,Expr2,Eval> : IPar<Eval>
{
	typedef typename Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Traits::GetEval<Expr2>::Type	View2Eval;

	ParView2(State& state, const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	Eval value() const { return p1.value() * p2.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<Mul,Expr1,Expr2> getObj()  const
	{ return p1.getObj()*p2.getObj(); }

	ParView<View1Eval,Expr1>	p1;
	ParView<View2Eval,Expr2>	p2;
};

/**
 * 	ParView over equality relation.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct ParView2<Equal,Expr1,Expr2,bool> : IPar<bool>
{
	typedef typename Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Traits::GetEval<Expr2>::Type	View2Eval;

	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() == p2.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<Equal,Expr1,Expr2> getObj()  const
	{ return p1.getObj()==p2.getObj(); }

	ParView<View1Eval,Expr1>	p1;
	ParView<View2Eval,Expr2>	p2;
};

/**
 * 	ParView over inequality.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct ParView2<Greater,Expr1,Expr2,bool> : IPar<bool>
{
	typedef typename Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Traits::GetEval<Expr2>::Type	View2Eval;

	ParView2(State& state, const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() > p2.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<Greater,Expr1,Expr2> getObj()  const
	{ return p1.getObj()>p2.getObj(); }

	ParView<View1Eval,Expr1>	p1;
	ParView<View2Eval,Expr2>	p2;
};

/**
 * 	ParView over inequality.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct ParView2<GreaterEqual,Expr1,Expr2,bool> : IPar<bool>
{
	typedef typename Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Traits::GetEval<Expr2>::Type	View2Eval;

	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() >= p2.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<GreaterEqual,Expr1,Expr2> getObj()  const
	{ return p1.getObj()>=p2.getObj(); }

	ParView<View1Eval,Expr1>	p1;
	ParView<View2Eval,Expr2>	p2;
};

/**
 * 	ParView over inequality.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct ParView2<Less,Expr1,Expr2,bool> : IPar<bool>
{
	typedef typename Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Traits::GetEval<Expr2>::Type	View2Eval;

	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() < p2.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<Less,Expr1,Expr2> getObj()  const
	{ return p1.getObj()<p2.getObj(); }

	ParView<View1Eval,Expr1>	p1;
	ParView<View2Eval,Expr2>	p2;
};

/**
 * 	ParView over inequality.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct ParView2<LessEqual,Expr1,Expr2,bool> : IPar<bool>
{
	typedef typename Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Traits::GetEval<Expr2>::Type	View2Eval;

	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() <= p2.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}

	Rel2<LessEqual,Expr1,Expr2> getObj()  const
	{ return p1.getObj()<=p2.getObj(); }

	ParView<View1Eval,Expr1>	p1;
	ParView<View2Eval,Expr2>	p2;
};

/**
 * 	ParView over disequality.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct ParView2<Distinct,Expr1,Expr2,bool> : IPar<bool>
{
	typedef typename Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Traits::GetEval<Expr2>::Type	View2Eval;

	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() != p2.value(); }

//	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
//	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<Distinct,Expr1,Expr2> getObj()  const
	{ return p1.getObj()!=p2.getObj(); }

	ParView<View1Eval,Expr1>	p1;
	ParView<View2Eval,Expr2>	p2;
};

template<class View,class Eval>
struct ParView1<Round,View,Eval> : IPar<Eval>
{
	ParView1(State& state, const View& p1) :
		p1(state,p1)	{}
	Eval value() const { return ::round(p1.value()); }

//	void attach(INotifiable* f) { 	p1.attach(f); }
//	void detach(INotifiable* f) {	p1.detach(f); }

	Rel1<Round,View> getObj()  const { return round(p1.getObj()); }
	ParView<Eval,View>	p1;
};

template<class View,class Eval>
struct ParView1<Ceil,View,Eval> : IPar<Eval>
{
	ParView1(State& state, const View& p1) :
		p1(state,p1)	{}
	Eval value() const { return ::ceil(p1.value()); }

//	void attach(INotifiable* f) { 	p1.attach(f); }
//	void detach(INotifiable* f) {	p1.detach(f); }

	Rel1<Ceil,View> getObj()  const { return ceil(p1.getObj()); }
	ParView<Eval,View>	p1;
};

template<class View,class Eval>
struct ParView1<Floor,View,Eval> : IPar<Eval>
{
	ParView1(State& state, const View& p1) :
		p1(state,p1)	{}
	Eval value() const { return ::floor(p1.value()); }

//	void attach(INotifiable* f) { 	p1.attach(f); }
//	void detach(INotifiable* f) {	p1.detach(f); }

	Rel1<Floor,View> getObj()  const { return floor(p1.getObj()); }
	ParView<Eval,View>	p1;
};

template<class Eval,class View>
struct ParView1<Cast<Eval>,View,Eval> : IPar<Eval>
{
	typedef typename Traits::GetEval<View>::Type	ViewEval;

	ParView1(State& state, const View& p1) :
		p1(state,p1)	{}
	Eval value() const { return static_cast<Eval>(p1.value()); }

//	void attach(INotifiable* f) { 	p1.attach(f); }
//	void detach(INotifiable* f) {	p1.detach(f); }

	Rel1<Round,View> getObj()  const { return cast<Eval>(p1.getObj()); }
	ParView<ViewEval,View>	p1;
};

// FIXME: Attach is not correct, must fix when ParArray is ready
template<class Array,class Index,class Eval>
struct ParView2<Element,Array,Index,Eval> : IPar<Eval>
{
	typedef typename Traits::GetEval<Index>::Type	IndexEval;
	typedef typename Traits::GetElem<Array>::Type	ArrayElem;
	ParView2(State& state, const Array& s, const Index& idx) :
		state(state),s(s),idx(state,idx) {}
	Eval value() const { return ParView<Eval,ArrayElem>(state,s[idx.value()]).value();	}
	void setValue(const Eval& v)
	{	ParView<Eval,ArrayElem>(state,s[idx.value()]).setValue(v); }

//	void attach(INotifiable* f) { 	idx.attach(f); }
//	void detach(INotifiable* f) {	idx.detach(f); }

	State& state;
	Array	s;
	ParView<IndexEval,Index> idx;
};

// FIXME: Attach is not correct, must fix when ParArray is ready
template<class Set, class Expr>
struct ParView3<ArgMax,Par<int>,Set,Expr,int> : IPar<int>
{
	typedef typename Traits::GetEval<Expr>::Type	ExprEval;

	ParView3(State& state, const Par<int>& v, const Set& s, const Expr& e) :
		v(v),s(s),e(state,e) {}
	int value() const
	{
		int bestIdx;
		ExprEval bestEval = limits<ExprEval>::negInf();
		for (Casper::Detail::PIteration<Par<int>,Set,bool> it(v,s,true);
				it.valid(); it.iterate())
		{
			if (e.value() > bestEval)
			{
				bestEval = e.value();
				bestIdx = v.value();
			}
		}
		return bestIdx;
	}

//	void attach(INotifiable* f) { 	e.attach(f); }
//	void detach(INotifiable* f) {	e.detach(f); }

	Par<int> v;
	Set	s;
	ParView<ExprEval,Expr> e;
};

// FIXME: Attach is not correct, must fix when ParArray is ready
template<class Set, class Expr>
struct ParView3<ArgMin,Par<int>,Set,Expr,int> : IPar<int>
{
	typedef typename Traits::GetEval<Expr>::Type	ExprEval;

	ParView3(State& state, const Par<int>& v, const Set& s, const Expr& e) :
		v(v),s(s),e(state,e) {}
	int value() const
	{
		int bestIdx;
		ExprEval bestEval = limits<ExprEval>::posInf();
		for (Casper::Detail::PIteration<Par<int>,Set,bool> it(v,s,true);
				it.valid(); it.iterate())
		{
			if (e.value() < bestEval)
			{
				bestEval = e.value();
				bestIdx = v.value();
			}
		}
		return bestIdx;
	}

//	void attach(INotifiable* f) { 	e.attach(f); }
//	void detach(INotifiable* f) {	e.detach(f); }

	Par<int> v;
	Set	s;
	ParView<ExprEval,Expr> e;
};

// FIXME: Attach is not correct, must fix when ParArray is ready
template<class Set, class Expr,class Eval>
struct ParView3<Max,Par<int>,Set,Expr,Eval> : IPar<Eval>
{
	ParView3(State& state, const Par<int>& v, const Set& s, const Expr& e) :
		v(v),s(s),e(state,e) {}
	Eval value() const
	{
		Eval bestEval = limits<Eval>::negInf();
		for (Casper::Detail::PIteration<Par<int>,Set,bool> it(v,s,true);
				it.valid(); it.iterate())
			if (e.value() > bestEval)
				bestEval = e.value();
		return bestEval;
	}

//	void attach(INotifiable* f) { 	e.attach(f); }
//	void detach(INotifiable* f) {	e.detach(f); }

	Par<int> v;
	Set	s;
	ParView<Eval,Expr> e;
};

// FIXME: Attach is not correct, must fix when ParArray is ready
template<class Set, class Expr,class Eval>
struct ParView3<Min,Par<int>,Set,Expr,Eval> : IPar<Eval>
{
	ParView3(State& state, const Par<int>& v, const Set& s, const Expr& e) :
		v(v),s(s),e(state,e) {}
	Eval value() const
	{
		Eval bestEval = limits<Eval>::posInf();
		for (Casper::Detail::PIteration<Par<int>,Set,bool> it(v,s,true);
				it.valid(); it.iterate())
			if (e.value() < bestEval)
				bestEval = e.value();
		return bestEval;
	}

//	void attach(INotifiable* f) { 	e.attach(f); }
//	void detach(INotifiable* f) {	e.detach(f); }

	Par<int> v;
	Set	s;
	ParView<Eval,Expr> e;
};

// FIXME: Attach is not correct, must fix when ParArray is ready
template<class Set, class Expr,class Eval>
struct ParView3<MaxDiff,Par<int>,Set,Expr,Eval> : IPar<Eval>
{
	ParView3(State& state, const Par<int>& v, const Set& s, const Expr& e) :
		v(v),s(s),e(state,e) {}
	Eval value() const
	{
		Eval bestMin = limits<Eval>::posInf();
		Eval bestMax = limits<Eval>::negInf();
		for (Casper::Detail::PIteration<Par<int>,Set,bool> it(v,s,true);
				it.valid(); it.iterate())
		{
			if (e.value() < bestMin)
				bestMin = e.value();
			if (e.value() > bestMax)
				bestMax = e.value();
		}
		return bestMax-bestMin;
	}

//	void attach(INotifiable* f) { 	e.attach(f); }
//	void detach(INotifiable* f) {	e.detach(f); }

	Par<int> v;
	Set	s;
	ParView<Eval,Expr> e;
};

template<class Set, class Expr>
struct ParView3<ForSome,Par<int>,Set,Expr,bool> : IPar<bool>
{
	typedef typename Traits::GetEval<Expr>::Type	ExprEval;

	ParView3(State& state, const Par<int>& v, const Set& s, const Expr& e) :
		v(v),s(s),e(state,e) {}
	bool value() const
	{
		for (Casper::Detail::PIteration<Par<int>,Set,bool> it(v,s,true);
				it.valid(); it.iterate())
			if (e.value())
				return true;
		return false;
	}

//	void attach(INotifiable* f) { 	e.attach(f); }
//	void detach(INotifiable* f) {	e.detach(f); }

	Par<int> v;
	Set	s;
	ParView<ExprEval,Expr> e;
};


template<class Eval>
struct ParView2<RandInRange,Eval,Eval,Eval> : IPar<Eval>
{
	ParView2(State& state, const Eval& lb, const Eval& ub) :
		lb(lb),ub(ub) {}
	Eval value() const
	{
		return static_cast<Eval>(::rand()/static_cast<double>(RAND_MAX) * (ub-lb) + lb);
	}

	const Eval lb;
	const Eval ub;
};

};

#endif /*_H_CASPER_KERNEL_VIEW_PAR */
