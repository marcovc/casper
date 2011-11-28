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
template<class,class,class,class>			struct ParView1;
template<class,class,class,class,class,class>	struct ParView2;
template<class,class,class,class,class,class,class,class>	struct ParView3;
template<class,class,class,class,class,class,class,class,class,class>	struct ParView4;
template<class,class,class,class,class,class,class,class,class,class,class,class>	struct ParView5;

template<class Eval,class R>
struct NoParView : IPar<Eval>
{
	NoParView(State& s,const R& r)
	{
		std::ostringstream os;
		os << r;
		throw Casper::Exception::UndefinedView(os.str().c_str(),"ParView");
	}
	__attribute__((noreturn)) Eval value() const { assert(0); }
	__attribute__((noreturn)) void setValue(const Eval& val) { assert(0); }
	__attribute__((noreturn)) bool ground() const { assert(0); }
	__attribute__((noreturn)) R getObj()  const { assert(0); }
};


template<class Eval,class View>
struct ParView  : NoParView<Eval,View>
{
	ParView(State& s, const View& p1) :
		NoParView<Eval,View>(s,p1) {}
};

template<class F,class Eval1,class Expr1,class Eval>
struct ParView1 : NoParView<Eval,Rel1<F,Expr1> >
{
	ParView1(State& s,const Expr1& v) :
		NoParView<Eval,Rel1<F,Expr1> >(s,rel<F>(v)) {}
};

template<class F,class Eval1,class Expr1,class Eval2,class Expr2,class Eval>
struct ParView2 :  NoParView<Eval,Rel2<F,Expr1,Expr2> >
{
	ParView2(State& s,const Expr1& v1,const Expr2& v2) :
		NoParView<Eval,Rel2<F,Expr1,Expr2> >(s,rel<F>(v1,v2)) {}
};

template<class F,class Eval1,class Expr1,class Eval2,class Expr2,class Eval3,class Expr3,class Eval>
struct ParView3 :  NoParView<Eval,Rel3<F,Expr1,Expr2,Expr3> >
{
	ParView3(State& s,const Expr1& v1,const Expr2& v2,const Expr3& v3) :
		NoParView<Eval,Rel3<F,Expr1,Expr2,Expr3> >(s,rel<F>(v1,v2,v3)) {}
};

template<class F,class Eval1,class Expr1,class Eval2,class Expr2,class Eval3,class Expr3,
		 class Eval4,class Expr4, class Eval>
struct ParView4 :  NoParView<Eval,Rel4<F,Expr1,Expr2,Expr3,Expr4> >
{
	ParView4(State& s,const Expr1& v1,const Expr2& v2,const Expr3& v3,const Expr4& v4) :
		NoParView<Eval,Rel4<F,Expr1,Expr2,Expr3,Expr4> >(s,rel<F>(v1,v2,v3,v4)) {}
};

template<class F,class Eval1,class Expr1,class Eval2,class Expr2,class Eval3,class Expr3,
			class Eval4,class Expr4,class Eval5, class Expr5,class Eval>
struct ParView5 :  NoParView<Eval,Rel5<F,Expr1,Expr2,Expr3,Expr4,Expr5> >
{
	ParView5(State& s,const Expr1& v1,const Expr2& v2,const Expr3& v3,const Expr4& v4,const Expr5& v5) :
		NoParView<Eval,Rel5<F,Expr1,Expr2,Expr3,Expr4,Expr5> >(s,rel<F>(v1,v2,v3,v4,v5)) {}
};

// Extracts a given element of any array type
template<class T>
struct ParArrayView
{
	typedef typename Traits::GetElem<T>::Type	Elem;
	ParArrayView(State& s,const T& p) : p(p) {}
	Elem& operator[](int idx)
	{	return p[idx];	}
	const Elem& operator[](int idx) const
	{	return p[idx];	}
	T p;
};

// Extracts a given element of any array type
template<class Array,class Index>
struct ParArrayView<Rel2<Element,Array,Index> >
{
	typedef typename Traits::GetElem<Array>::Type	Elem;
	typedef typename Traits::GetElem<Elem>::Type	EElem;
	ParArrayView(State& s,const Rel2<Element,Array,Index>& p) : a(s,p.p1),i(s,p.p2) {}
	EElem& operator[](int idx)
	{	return a[i.value()][idx];	}
	const EElem& operator[](int idx) const
	{	return a[i.value()][idx];	}
	ParArrayView<Array> a;
	Par<int> i;
};


/**
 * 	ParView over literal.
 * 	\ingroup Views
 **/
template<class Eval>
struct ParView<Eval,Eval> : IPar<Eval>
{
	ParView(State& state, const Eval& p1) :
		p1(p1) {}
	Eval value() const { return p1; }

	const Eval& getObj()  const { return p1; }

	const Eval	p1;
};

/**
 * 	ParView over literal.
 * 	\ingroup Views
 **/
template<>
struct ParView<int,unsigned int> : IPar<int>
{
	ParView(State& state, const unsigned int& p1) :
		p1(p1) {}
	int value() const { return static_cast<int>(p1); }

	const unsigned int& getObj()  const { return p1; }

	const unsigned int p1;
};

/**
 * 	ParView over std::function.
 * 	\ingroup Views
 **/
template<class Eval>
struct ParView<Eval,std::function<Eval()> > : IPar<Eval>
{
	ParView(State& state, const std::function<Eval()>& f) :
		f(f) {}
	Eval value() const { return f(); }

	const std::function<Eval()>& getObj()  const { return f; }

	std::function<Eval()> f;
};

/**
 * 	ParView over a Par.
 * 	\ingroup Views
 **/
template<class T>
struct ParView<T,Par<T> > : IPar<T>
{
	ParView(State& state,const Par<T>& p1) :
		p1(p1) {}
	T value() const { return p1.value(); }
	void setValue(const T& v)
	{	p1 = v;	}

	const Par<T>& getObj()  const { return p1; }
	Par<T>	p1;
};

/**
 * 	ParView over a Rel1 relation -> defer to ParView1.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View>
struct ParView<Eval,Rel1<F,View> > :
	ParView1<F,typename Traits::GetEval<View>::Type,View,Eval>
{
	typedef typename Traits::GetEval<View>::Type	Eval1;

	ParView(State& state, const Rel1<F,View>& r) :
		ParView1<F,Eval1,View,Eval>(state,r.p1) {}
	// not sure if the below constructor is ever used
	ParView(State& state,const ParView1<F,Eval1,View,Eval>& v) :
		ParView1<F,Eval1,View,Eval>(v) {}
};

/**
 * 	ParView over a Rel2 relation -> defer to ParView2.
 * 	\ingroup Views
 **/
template<class Eval,class F,class Expr1,class Expr2>
struct ParView<Eval,Rel2<F,Expr1,Expr2> > :
	ParView2<F,typename Traits::GetEval<Expr1>::Type,Expr1,
				typename Traits::GetEval<Expr2>::Type,Expr2,Eval>
{
	typedef typename Traits::GetEval<Expr1>::Type Eval1;
	typedef typename Traits::GetEval<Expr2>::Type Eval2;

	ParView(State& state, const Rel2<F,Expr1,Expr2>& r) :
		ParView2<F,Eval1,Expr1,Eval2,Expr2,Eval>(state,r.p1,r.p2) {}
	// not sure if the below constructor is ever used
	ParView(State& state, const ParView2<F,Eval1,Expr1,Eval2,Expr2,Eval>& v) :
		ParView2<F,Eval1,Expr1,Eval2,Expr2,Eval>(v) {}
};

/**
 * 	ParView over a Rel3 relation -> defer to ParView3.
 * 	\ingroup Views
 **/
template<class Eval,class F,class Expr1,class Expr2,class Expr3>
struct ParView<Eval,Rel3<F,Expr1,Expr2,Expr3> > :
	ParView3<F,typename Traits::GetEval<Expr1>::Type,Expr1,
				typename Traits::GetEval<Expr2>::Type,Expr2,
				typename Traits::GetEval<Expr3>::Type,Expr3,Eval>
{
	typedef typename Traits::GetEval<Expr1>::Type	Eval1;
	typedef typename Traits::GetEval<Expr2>::Type	Eval2;
	typedef typename Traits::GetEval<Expr3>::Type	Eval3;

	ParView(State& state, const Rel3<F,Expr1,Expr2,Expr3>& r) :
		ParView3<F,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3,Eval>(state,r.p1,r.p2,r.p3) {}
};

/**
 * 	ParView over a Rel4 relation -> defer to ParView4.
 * 	\ingroup Views
 **/
template<class Eval,class F,class Expr1,class Expr2,class Expr3,class Expr4>
struct ParView<Eval,Rel4<F,Expr1,Expr2,Expr3,Expr4> > :
	ParView4<F,typename Traits::GetEval<Expr1>::Type,Expr1,
			   typename Traits::GetEval<Expr2>::Type,Expr2,
			   typename Traits::GetEval<Expr3>::Type,Expr3,
			   typename Traits::GetEval<Expr4>::Type,Expr4,Eval>
{
	typedef typename Traits::GetEval<Expr1>::Type Eval1;
	typedef typename Traits::GetEval<Expr2>::Type Eval2;
	typedef typename Traits::GetEval<Expr3>::Type Eval3;
	typedef typename Traits::GetEval<Expr4>::Type Eval4;

	ParView(State& state, const Rel4<F,Expr1,Expr2,Expr3,Expr4>& r) :
		ParView4<F,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3,Eval4,Expr4,Eval>(state,r.p1,r.p2,r.p3,r.p4) {}
};


/**
 * 	ParView over a Rel5 relation -> defer to ParView5.
 * 	\ingroup Views
 **/
template<class Eval,class F,class Expr1,class Expr2,class Expr3,class Expr4,class Expr5>
struct ParView<Eval,Rel5<F,Expr1,Expr2,Expr3,Expr4,Expr5> > :
	ParView5<F,typename Traits::GetEval<Expr1>::Type,Expr1,
				typename Traits::GetEval<Expr2>::Type,Expr2,
				typename Traits::GetEval<Expr3>::Type,Expr3,
				typename Traits::GetEval<Expr4>::Type,Expr4,
				typename Traits::GetEval<Expr5>::Type,Expr5,Eval>
{
	typedef typename Traits::GetEval<Expr1>::Type Eval1;
	typedef typename Traits::GetEval<Expr2>::Type Eval2;
	typedef typename Traits::GetEval<Expr3>::Type Eval3;
	typedef typename Traits::GetEval<Expr4>::Type Eval4;
	typedef typename Traits::GetEval<Expr5>::Type Eval5;
	ParView(State& state, const Rel5<F,Expr1,Expr2,Expr3,Expr4,Expr5>& r) :
		ParView5<F,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3,Eval4,Expr4,Eval5,Expr5,Eval>(state,r.p1,r.p2,r.p3,r.p4,r.p5) {}
};

/**
 * 	ParView over symmetric.
 * 	\ingroup Views
 **/
template<class Obj1,class Eval>
struct ParView1<Sym,Eval,Obj1,Eval> : IPar<Eval>
{
	ParView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	Eval value() const { return -p1.value(); }

	Rel1<Sym,Obj1> getObj()  const { return -(p1.getObj()); }
	ParView<Eval,Obj1>	p1;
};

template<class Obj1,class Eval>
struct ParView1<Abs,Eval,Obj1,Eval> : IPar<Eval>
{
	ParView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	Eval value() const
	{
		Eval r = p1.value();
		return (r>=0)?r:-r;
	}

	Rel1<Abs,Obj1> getObj()  const { return abs(p1.getObj()); }
	ParView<Eval,Obj1>	p1;
};

// logical

/**
 * 	ParView over negation.
 * 	\ingroup Views
 **/
template<class Obj1>
struct ParView1<Not,bool,Obj1,bool> : IPar<bool>
{
	ParView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	bool value() const { return !p1.value(); }

	Rel1<Not,Obj1> getObj()  const { return not p1.getObj(); }
	ParView<bool,Obj1>	p1;
};

/**
 * 	ParView over conjunction.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct ParView2<And,bool,Expr1,bool,Expr2,bool> : IPar<bool>
{
	ParView2(State& state,const Expr1& p1,const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() && p2.value(); }

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
struct ParView2<Or,bool,Expr1,bool,Expr2,bool> : IPar<bool>
{
	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) 	{}
	bool value() const { return p1.value() || p2.value(); }

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
struct ParView2<Add,Eval,Expr1,Eval,Expr2,Eval> : IPar<Eval>
{
	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	Eval value() const { return p1.value() + p2.value(); }

	Rel2<Add,Expr1,Expr2> getObj()  const
	{ return p1.getObj()+p2.getObj(); }

	ParView<Eval,Expr1>	p1;
	ParView<Eval,Expr2>	p2;
};

/**
 * 	ParView over subtraction.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2,class Eval>
struct ParView2<Sub,Eval,Expr1,Eval,Expr2,Eval> : IPar<Eval>
{
	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	Eval value() const { return p1.value() - p2.value(); }

	Rel2<Sub,Expr1,Expr2> getObj()  const
	{ return p1.getObj()-p2.getObj(); }

	ParView<Eval,Expr1>	p1;
	ParView<Eval,Expr2>	p2;
};

/**
 * 	ParView over multiplication.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2,class Eval>
struct ParView2<Mul,Eval,Expr1,Eval,Expr2,Eval> : IPar<Eval>
{
	ParView2(State& state, const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	Eval value() const { return p1.value() * p2.value(); }

	Rel2<Mul,Expr1,Expr2> getObj()  const
	{ return p1.getObj()*p2.getObj(); }

	ParView<Eval,Expr1>	p1;
	ParView<Eval,Expr2>	p2;
};

/**
 * 	ParView over equality relation.
 * 	\ingroup Views
 **/
template<class Eval,class Expr1,class Expr2>
struct ParView2<Equal,Eval,Expr1,Eval,Expr2,bool> : IPar<bool>
{
	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() == p2.value(); }

	Rel2<Equal,Expr1,Expr2> getObj()  const
	{ return p1.getObj()==p2.getObj(); }

	ParView<Eval,Expr1>	p1;
	ParView<Eval,Expr2>	p2;
};

/**
 * 	ParView over inequality.
 * 	\ingroup Views
 **/
template<class Eval,class Expr1,class Expr2>
struct ParView2<Greater,Eval,Expr1,Eval,Expr2,bool> : IPar<bool>
{
	ParView2(State& state, const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() > p2.value(); }

	Rel2<Greater,Expr1,Expr2> getObj()  const
	{ return p1.getObj()>p2.getObj(); }

	ParView<Eval,Expr1>	p1;
	ParView<Eval,Expr2>	p2;
};

/**
 * 	ParView over inequality.
 * 	\ingroup Views
 **/
template<class Eval,class Expr1,class Expr2>
struct ParView2<GreaterEqual,Eval,Expr1,Eval,Expr2,bool> : IPar<bool>
{
	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() >= p2.value(); }

	Rel2<GreaterEqual,Expr1,Expr2> getObj()  const
	{ return p1.getObj()>=p2.getObj(); }

	ParView<Eval,Expr1>	p1;
	ParView<Eval,Expr2>	p2;
};

/**
 * 	ParView over inequality.
 * 	\ingroup Views
 **/
template<class Eval,class Expr1,class Expr2>
struct ParView2<Less,Eval,Expr1,Eval,Expr2,bool> : IPar<bool>
{
	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() < p2.value(); }

	Rel2<Less,Expr1,Expr2> getObj()  const
	{ return p1.getObj()<p2.getObj(); }

	ParView<Eval,Expr1>	p1;
	ParView<Eval,Expr2>	p2;
};

/**
 * 	ParView over inequality.
 * 	\ingroup Views
 **/
template<class Eval,class Expr1,class Expr2>
struct ParView2<LessEqual,Eval,Expr1,Eval,Expr2,bool> : IPar<bool>
{
	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() <= p2.value(); }

	Rel2<LessEqual,Expr1,Expr2> getObj()  const
	{ return p1.getObj()<=p2.getObj(); }

	ParView<Eval,Expr1>	p1;
	ParView<Eval,Expr2>	p2;
};

/**
 * 	ParView over disequality.
 * 	\ingroup Views
 **/
template<class Eval,class Expr1,class Expr2>
struct ParView2<Distinct,Eval,Expr1,Eval,Expr2,bool> : IPar<bool>
{
	ParView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() != p2.value(); }

	Rel2<Distinct,Expr1,Expr2> getObj()  const
	{ return p1.getObj()!=p2.getObj(); }

	ParView<Eval,Expr1>	p1;
	ParView<Eval,Expr2>	p2;
};

template<class Eval1,class Obj1,class Eval>
struct ParView1<Round,Eval1,Obj1,Eval> : IPar<Eval>
{
	ParView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	Eval value() const { return ::round(p1.value()); }

	Rel1<Round,Obj1> getObj()  const { return round(p1.getObj()); }
	ParView<Eval1,Obj1>	p1;
};

template<class Eval1,class Obj1,class Eval>
struct ParView1<Ceil,Eval1,Obj1,Eval> : IPar<Eval>
{
	ParView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	Eval value() const { return ::ceil(p1.value()); }

	Rel1<Ceil,Obj1> getObj()  const { return ceil(p1.getObj()); }
	ParView<Eval1,Obj1>	p1;
};

template<class Eval1,class Obj1,class Eval>
struct ParView1<Floor,Eval1,Obj1,Eval> : IPar<Eval>
{
	ParView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	Eval value() const { return ::floor(p1.value()); }

	Rel1<Floor,Obj1> getObj()  const { return floor(p1.getObj()); }
	ParView<Eval1,Obj1>	p1;
};

template<class Eval1,class Eval,class Obj1>
struct ParView1<Cast<Eval>,Eval1,Obj1,Eval> : IPar<Eval>
{
	ParView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	Eval value() const { return static_cast<Eval>(p1.value()); }

	Rel1<Round,Obj1> getObj()  const { return cast<Eval>(p1.getObj()); }
	ParView<Eval1,Obj1>	p1;
};

template<class Eval,class T>
ParView<Eval,T> newParView(State& state,const T& t)
{	return ParView<Eval,T>(state,t);	}

template<class ArrayEval,class IndexEval,class Array,class Index,class Eval>
struct ParView2<Element,ArrayEval,Array,IndexEval,Index,Eval> : IPar<Eval>
{
	typedef typename Traits::GetElem<Array>::Type	ArrayElem;
	ParView2(State& state, const Array& s, const Index& idx) :
		state(state),s(state,s),idx(state,idx) {}
	//Eval value() const { return ParView<Eval,ArrayElem>(state,s[idx.value()]).value();	}
	Eval value() const { return  newParView<Eval>(state,s[idx.value()]).value();	}
	void setValue(const Eval& v)
	{
		//ParView<Eval,ArrayElem>(state,s[idx.value()]).setValue(v);
		newParView<Eval>(state,s[idx.value()]).setValue(v);
	}


	State& state;
	ParArrayView<Array>	s;
	ParView<IndexEval,Index> idx;
};

template<class Set, class Cond,class ExprEval,class Expr>
struct ParView4<ArgMax,int,Par<int>,Seq<int>,Set,bool,Cond,ExprEval,Expr,int> : IPar<int>
{
	ParView4(State& state, const Par<int>& v, const Set& s, const Cond& c,const Expr& e) :
		v(v),s(s),c(c),e(state,e) {}
	int value() const
	{
		int bestIdx;
		ExprEval bestEval = limits<ExprEval>::negInf();
		for (Casper::Detail::PIteration<Par<int>,Set,Cond> it(v,s,c);
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

	Par<int> v;
	Set	s;
	Cond c;
	ParView<ExprEval,Expr> e;
};

template<class Set, class ExprEval,class Expr>
struct ParView3<ArgMax,int,Par<int>,Seq<int>,Set,ExprEval,Expr,int> :
	ParView4<ArgMax,int,Par<int>,Seq<int>,Set,bool,bool,ExprEval,Expr,int>
{
	typedef ParView4<ArgMax,int,Par<int>,Seq<int>,Set,bool,bool,ExprEval,Expr,int> Super;

	ParView3(State& state, const Par<int>& v, const Set& s, const Expr& e) :
		Super(state,v,s,true,e) {}
};

template<class Set, class Cond,class ExprEval,class Expr>
struct ParView4<ArgMin,int,Par<int>,Seq<int>,Set,bool,Cond,ExprEval,Expr,int> : IPar<int>
{
	ParView4(State& state, const Par<int>& v, const Set& s, const Cond& c,const Expr& e) :
		v(v),s(s),c(c),e(state,e) {}
	int value() const
	{
		int bestIdx;
		ExprEval bestEval = limits<ExprEval>::posInf();
		for (Casper::Detail::PIteration<Par<int>,Set,Cond> it(v,s,c);
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

	Par<int> v;
	Set	s;
	Cond c;
	ParView<ExprEval,Expr> e;
};

template<class Set, class ExprEval,class Expr>
struct ParView3<ArgMin,int,Par<int>,Seq<int>,Set,ExprEval,Expr,int> :
	ParView4<ArgMin,int,Par<int>,Seq<int>,Set,bool,bool,ExprEval,Expr,int>
{
	typedef ParView4<ArgMin,int,Par<int>,Seq<int>,Set,bool,bool,ExprEval,Expr,int> Super;

	ParView3(State& state, const Par<int>& v, const Set& s, const Expr& e) :
		Super(state,v,s,true,e) {}
};

template<class Set, class Cond,class Expr,class Eval>
struct ParView4<Max,int,Par<int>,Seq<int>,Set,bool,Cond,Eval,Expr,Eval> : IPar<Eval>
{
	ParView4(State& state, const Par<int>& v, const Set& s, const Cond& c,const Expr& e) :
		v(v),s(s),c(c),e(state,e) {}
	Eval value() const
	{
		Eval bestEval = limits<Eval>::negInf();
		for (Casper::Detail::PIteration<Par<int>,Set,Cond> it(v,s,c);
				it.valid(); it.iterate())
			if (e.value() > bestEval)
				bestEval = e.value();
		return bestEval;
	}

	Par<int> v;
	Set	s;
	Cond c;
	ParView<Eval,Expr> e;
};


template<class Set, class Expr,class Eval>
struct ParView3<Max,int,Par<int>,Seq<int>,Set,Eval,Expr,Eval> :
	ParView4<Max,int,Par<int>,Seq<int>,Set,bool,bool,Eval,Expr,Eval>
{
	typedef ParView4<Max,int,Par<int>,Seq<int>,Set,bool,bool,Eval,Expr,Eval>	Super;
	ParView3(State& state, const Par<int>& v, const Set& s, const Expr& e) :
		Super(state,v,s,true,e) {}
};

template<class Set, class Cond, class Expr,class Eval>
struct ParView4<Min,int,Par<int>,Seq<int>,Set,bool,Cond,Eval,Expr,Eval> : IPar<Eval>
{
	ParView4(State& state, const Par<int>& v, const Set& s, const Cond& c, const Expr& e) :
		v(v),s(s),c(c),e(state,e) {}
	Eval value() const
	{
		Eval bestEval = limits<Eval>::posInf();
		for (Casper::Detail::PIteration<Par<int>,Set,Cond> it(v,s,c);
				it.valid(); it.iterate())
			if (e.value() < bestEval)
				bestEval = e.value();
		return bestEval;
	}

	Par<int> v;
	Set	s;
	Cond c;
	ParView<Eval,Expr> e;
};

template<class Set, class Expr,class Eval>
struct ParView3<Min,int,Par<int>,Seq<int>,Set,Eval,Expr,Eval> :
	ParView4<Min,int,Par<int>,Seq<int>,Set,bool,bool,Eval,Expr,Eval>
{
	typedef ParView4<Min,int,Par<int>,Seq<int>,Set,bool,bool,Eval,Expr,Eval>	Super;
	ParView3(State& state, const Par<int>& v, const Set& s, const Expr& e) :
		Super(state,v,s,true,e) {}
};

// FIXME: Attach is not correct, must fix when ParArray is ready
template<class Set, class Expr,class Eval>
struct ParView3<MaxDiff,int,Par<int>,Seq<int>,Set,Eval,Expr,Eval> : IPar<Eval>
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

	Par<int> v;
	Set	s;
	ParView<Eval,Expr> e;
};

template<class Set, class Expr>
struct ParView3<ForSome,int,Par<int>,Seq<int>,Set,bool,Expr,bool> : IPar<bool>
{
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

	Par<int> v;
	Set	s;
	ParView<bool,Expr> e;
};


template<class Eval>
struct ParView2<RandInRange,Eval,Eval,Eval,Eval,Eval> : IPar<Eval>
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


template<class Set, class Cond,class Expr,class Eval>
struct ParView4<Sum,int,Par<int>,Seq<int>,Set,bool,Cond,Eval,Expr,Eval> : IPar<Eval>
{
	ParView4(State& state, const Par<int>& v, const Set& s, const Cond& c, const Expr& e) :
		v(v),s(s),cond(c),e(state,e) {}
	Eval value() const
	{
		Eval sum = 0;
		for (Casper::Detail::PIteration<Par<int>,Set,Cond> it(v,s,cond);
				it.valid(); it.iterate())
			sum += e.value();
		return sum;
	}
	Par<int> v;
	Set	s;
	Cond cond;
	ParView<Eval,Expr> e;
};


template<class Set, class Expr,class Eval>
struct ParView3<Sum,int,Par<int>,Seq<int>,Set,Eval,Expr,Eval> :
	ParView4<Sum,int,Par<int>,Seq<int>,Set,bool,bool,Eval,Expr,Eval>
{
	ParView3(State& state, const Par<int>& v, const Set& s, const Expr& e) :
		ParView4<Sum,int,Par<int>,Seq<int>,Set,bool,bool,Eval,Expr,Eval>(state,v,s,true,e) {}
};

template<class Cond,class IfTrue, class IfFalse,class Eval>
struct ParView3<IfThenElse,bool,Cond,Eval,IfTrue,Eval,IfFalse,Eval> : IPar<Eval>
{
	ParView3(State& state, const Cond& c, const IfTrue& t, const IfFalse& f) :
		c(state,c),t(state,t),f(state,f) {}
	Eval value() const
	{
		if (c.value())
			return t.value();
		else
			return f.value();
	}

	ParView<bool,Cond> c;
	ParView<Eval,IfTrue>  t;
	ParView<Eval,IfFalse> f;
};

};

#endif /*_H_CASPER_KERNEL_VIEW_PAR */
