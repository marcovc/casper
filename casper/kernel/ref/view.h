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

#ifndef _H_CASPER_KERNEL_REF_VIEW
#define _H_CASPER_KERNEL_REF_VIEW

#include <casper/kernel/ref/ref.h>
#include <casper/kernel/ref/refarray.h>
#include <casper/kernel/ref/piteration.h>
 
namespace Casper {


/********************
 * Views over parameterized expressions
 *********************/

template<class>	struct Ref;
template<class,class,class,class>			struct RefView1;
template<class,class,class,class,class,class>	struct RefView2;
template<class,class,class,class,class,class,class,class>	struct RefView3;
template<class,class,class,class,class,class,class,class,class,class>	struct RefView4;
template<class,class,class,class,class,class,class,class,class,class,class,class>	struct RefView5;

template<class Eval,class R>
struct NoRefView : IRef<Eval>
{
	NoRefView(State& s,const R& r)
	{
		std::ostringstream os;
		os << r;
		throw Casper::Exception::UndefinedView(os.str().c_str(),"RefView");
	}
	Eval value() const;
	void setValue(const Eval& val);
	bool ground() const;
	R getObj()  const;
#if 0
	__attribute__((noreturn)) Eval value() const { assert(0); }
	__attribute__((noreturn)) void setValue(const Eval& val) { assert(0); }
	__attribute__((noreturn)) bool ground() const { assert(0); }
	__attribute__((noreturn)) R getObj()  const { assert(0); }
#endif
};


template<class Eval,class View>
struct RefView  : NoRefView<Eval,View>
{
	RefView(State& s, const View& p1) :
		NoRefView<Eval,View>(s,p1) {}
};

template<class F,class Eval1,class Expr1,class Eval>
struct RefView1 : NoRefView<Eval,Rel1<F,Expr1> >
{
	RefView1(State& s,const Expr1& v) :
		NoRefView<Eval,Rel1<F,Expr1> >(s,rel<F>(v)) {}
};

template<class F,class Eval1,class Expr1,class Eval2,class Expr2,class Eval>
struct RefView2 :  NoRefView<Eval,Rel2<F,Expr1,Expr2> >
{
	RefView2(State& s,const Expr1& v1,const Expr2& v2) :
		NoRefView<Eval,Rel2<F,Expr1,Expr2> >(s,rel<F>(v1,v2)) {}
};

template<class F,class Eval1,class Expr1,class Eval2,class Expr2,class Eval3,class Expr3,class Eval>
struct RefView3 :  NoRefView<Eval,Rel3<F,Expr1,Expr2,Expr3> >
{
	RefView3(State& s,const Expr1& v1,const Expr2& v2,const Expr3& v3) :
		NoRefView<Eval,Rel3<F,Expr1,Expr2,Expr3> >(s,rel<F>(v1,v2,v3)) {}
};

template<class F,class Eval1,class Expr1,class Eval2,class Expr2,class Eval3,class Expr3,
		 class Eval4,class Expr4, class Eval>
struct RefView4 :  NoRefView<Eval,Rel4<F,Expr1,Expr2,Expr3,Expr4> >
{
	RefView4(State& s,const Expr1& v1,const Expr2& v2,const Expr3& v3,const Expr4& v4) :
		NoRefView<Eval,Rel4<F,Expr1,Expr2,Expr3,Expr4> >(s,rel<F>(v1,v2,v3,v4)) {}
};

template<class F,class Eval1,class Expr1,class Eval2,class Expr2,class Eval3,class Expr3,
			class Eval4,class Expr4,class Eval5, class Expr5,class Eval>
struct RefView5 :  NoRefView<Eval,Rel5<F,Expr1,Expr2,Expr3,Expr4,Expr5> >
{
	RefView5(State& s,const Expr1& v1,const Expr2& v2,const Expr3& v3,const Expr4& v4,const Expr5& v5) :
		NoRefView<Eval,Rel5<F,Expr1,Expr2,Expr3,Expr4,Expr5> >(s,rel<F>(v1,v2,v3,v4,v5)) {}
};

// Extracts a given element of any array type
template<class T>
struct RefArrayView
{
	typedef typename Traits::GetElem<T>::Type	Elem;
	RefArrayView(State& s,const T& p) : p(p) {}
	Elem& operator[](int idx)
	{	return p[idx];	}
	const Elem& operator[](int idx) const
	{	return p[idx];	}
	T p;
};

// Extracts a given element of any array type
template<class Array,class Index>
struct RefArrayView<Rel2<Element,Array,Index> >
{
	typedef typename Traits::GetElem<Array>::Type	Elem;
	typedef typename Traits::GetElem<Elem>::Type	EElem;
	RefArrayView(State& s,const Rel2<Element,Array,Index>& p) : a(s,p.p1),i(s,p.p2) {}
	EElem& operator[](int idx)
	{	return a[i.value()][idx];	}
	const EElem& operator[](int idx) const
	{	return a[i.value()][idx];	}
	RefArrayView<Array> a;
	Ref<int> i;
};


/**
 * 	RefView over literal.
 * 	\ingroup Views
 **/
template<class Eval>
struct RefView<Eval,Eval> : IRef<Eval>
{
	RefView(State& state, const Eval& p1) :
		p1(p1) {}
	Eval value() const { return p1; }

	const Eval& getObj()  const { return p1; }

	const Eval	p1;
};

/**
 * 	RefView over literal.
 * 	\ingroup Views
 **/
template<>
struct RefView<int,unsigned int> : IRef<int>
{
	RefView(State& state, const unsigned int& p1) :
		p1(p1) {}
	int value() const { return static_cast<int>(p1); }

	const unsigned int& getObj()  const { return p1; }

	const unsigned int p1;
};

/**
 * 	RefView over std::function.
 * 	\ingroup Views
 **/
template<class Eval>
struct RefView<Eval,std::function<Eval()> > : IRef<Eval>
{
	RefView(State& state, const std::function<Eval()>& f) :
		f(f) {}
	Eval value() const { return f(); }

	const std::function<Eval()>& getObj()  const { return f; }

	std::function<Eval()> f;
};

/**
 * 	RefView over a Ref.
 * 	\ingroup Views
 **/
template<class T>
struct RefView<T,Ref<T> > : IRef<T>
{
	RefView(State& state,const Ref<T>& p1) :
		p1(p1) {}
	T value() const { return p1.value(); }
	void setValue(const T& v)
	{	p1 = v;	}

	const Ref<T>& getObj()  const { return p1; }
	Ref<T>	p1;
};

/**
 * 	RefView over a Rel1 relation -> defer to RefView1.
 * 	\ingroup Views
 **/
template<class Eval,class F,class View>
struct RefView<Eval,Rel1<F,View> > :
	RefView1<F,typename Traits::GetEval<View>::Type,View,Eval>
{
	typedef typename Traits::GetEval<View>::Type	Eval1;

	RefView(State& state, const Rel1<F,View>& r) :
		RefView1<F,Eval1,View,Eval>(state,r.p1) {}
	// not sure if the below constructor is ever used
	RefView(State& state,const RefView1<F,Eval1,View,Eval>& v) :
		RefView1<F,Eval1,View,Eval>(v) {}
};

/**
 * 	RefView over a Rel2 relation -> defer to RefView2.
 * 	\ingroup Views
 **/
template<class Eval,class F,class Expr1,class Expr2>
struct RefView<Eval,Rel2<F,Expr1,Expr2> > :
	RefView2<F,typename Traits::GetEval<Expr1>::Type,Expr1,
				typename Traits::GetEval<Expr2>::Type,Expr2,Eval>
{
	typedef typename Traits::GetEval<Expr1>::Type Eval1;
	typedef typename Traits::GetEval<Expr2>::Type Eval2;

	RefView(State& state, const Rel2<F,Expr1,Expr2>& r) :
		RefView2<F,Eval1,Expr1,Eval2,Expr2,Eval>(state,r.p1,r.p2) {}
	// not sure if the below constructor is ever used
	RefView(State& state, const RefView2<F,Eval1,Expr1,Eval2,Expr2,Eval>& v) :
		RefView2<F,Eval1,Expr1,Eval2,Expr2,Eval>(v) {}
};

/**
 * 	RefView over a Rel3 relation -> defer to RefView3.
 * 	\ingroup Views
 **/
template<class Eval,class F,class Expr1,class Expr2,class Expr3>
struct RefView<Eval,Rel3<F,Expr1,Expr2,Expr3> > :
	RefView3<F,typename Traits::GetEval<Expr1>::Type,Expr1,
				typename Traits::GetEval<Expr2>::Type,Expr2,
				typename Traits::GetEval<Expr3>::Type,Expr3,Eval>
{
	typedef typename Traits::GetEval<Expr1>::Type	Eval1;
	typedef typename Traits::GetEval<Expr2>::Type	Eval2;
	typedef typename Traits::GetEval<Expr3>::Type	Eval3;

	RefView(State& state, const Rel3<F,Expr1,Expr2,Expr3>& r) :
		RefView3<F,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3,Eval>(state,r.p1,r.p2,r.p3) {}
};

/**
 * 	RefView over a Rel4 relation -> defer to RefView4.
 * 	\ingroup Views
 **/
template<class Eval,class F,class Expr1,class Expr2,class Expr3,class Expr4>
struct RefView<Eval,Rel4<F,Expr1,Expr2,Expr3,Expr4> > :
	RefView4<F,typename Traits::GetEval<Expr1>::Type,Expr1,
			   typename Traits::GetEval<Expr2>::Type,Expr2,
			   typename Traits::GetEval<Expr3>::Type,Expr3,
			   typename Traits::GetEval<Expr4>::Type,Expr4,Eval>
{
	typedef typename Traits::GetEval<Expr1>::Type Eval1;
	typedef typename Traits::GetEval<Expr2>::Type Eval2;
	typedef typename Traits::GetEval<Expr3>::Type Eval3;
	typedef typename Traits::GetEval<Expr4>::Type Eval4;

	RefView(State& state, const Rel4<F,Expr1,Expr2,Expr3,Expr4>& r) :
		RefView4<F,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3,Eval4,Expr4,Eval>(state,r.p1,r.p2,r.p3,r.p4) {}
};


/**
 * 	RefView over a Rel5 relation -> defer to RefView5.
 * 	\ingroup Views
 **/
template<class Eval,class F,class Expr1,class Expr2,class Expr3,class Expr4,class Expr5>
struct RefView<Eval,Rel5<F,Expr1,Expr2,Expr3,Expr4,Expr5> > :
	RefView5<F,typename Traits::GetEval<Expr1>::Type,Expr1,
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
	RefView(State& state, const Rel5<F,Expr1,Expr2,Expr3,Expr4,Expr5>& r) :
		RefView5<F,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3,Eval4,Expr4,Eval5,Expr5,Eval>(state,r.p1,r.p2,r.p3,r.p4,r.p5) {}
};

/**
 * 	RefView over symmetric.
 * 	\ingroup Views
 **/
template<class Obj1,class Eval>
struct RefView1<Sym,Eval,Obj1,Eval> : IRef<Eval>
{
	RefView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	Eval value() const { return -p1.value(); }

	Rel1<Sym,Obj1> getObj()  const { return -(p1.getObj()); }
	RefView<Eval,Obj1>	p1;
};

template<class Obj1,class Eval>
struct RefView1<Abs,Eval,Obj1,Eval> : IRef<Eval>
{
	RefView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	Eval value() const
	{
		Eval r = p1.value();
		return (r>=0)?r:-r;
	}

	Rel1<Abs,Obj1> getObj()  const { return abs(p1.getObj()); }
	RefView<Eval,Obj1>	p1;
};

// logical

/**
 * 	RefView over negation.
 * 	\ingroup Views
 **/
template<class Obj1>
struct RefView1<Not,bool,Obj1,bool> : IRef<bool>
{
	RefView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	bool value() const { return !p1.value(); }

	Rel1<Not,Obj1> getObj()  const { return not p1.getObj(); }
	RefView<bool,Obj1>	p1;
};

/**
 * 	RefView over conjunction.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct RefView2<And,bool,Expr1,bool,Expr2,bool> : IRef<bool>
{
	RefView2(State& state,const Expr1& p1,const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() && p2.value(); }

	Rel2<And,Expr1,Expr2> getObj()  const
	{ return p1.getObj() and p2.getObj(); }
	RefView<bool,Expr1>	p1;
	RefView<bool,Expr2>	p2;
};

/**
 * 	RefView over disjunction.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct RefView2<Or,bool,Expr1,bool,Expr2,bool> : IRef<bool>
{
	RefView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) 	{}
	bool value() const { return p1.value() || p2.value(); }

	Rel2<Or,Expr1,Expr2> getObj()  const
	{ return p1.getObj() or p2.getObj(); }

	RefView<bool,Expr1>	p1;
	RefView<bool,Expr2>	p2;
};

// arithmetic

/**
 * 	RefView over addition.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2,class Eval>
struct RefView2<Add,Eval,Expr1,Eval,Expr2,Eval> : IRef<Eval>
{
	RefView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	Eval value() const { return p1.value() + p2.value(); }

	Rel2<Add,Expr1,Expr2> getObj()  const
	{ return p1.getObj()+p2.getObj(); }

	RefView<Eval,Expr1>	p1;
	RefView<Eval,Expr2>	p2;
};

/**
 * 	RefView over subtraction.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2,class Eval>
struct RefView2<Sub,Eval,Expr1,Eval,Expr2,Eval> : IRef<Eval>
{
	RefView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	Eval value() const { return p1.value() - p2.value(); }

	Rel2<Sub,Expr1,Expr2> getObj()  const
	{ return p1.getObj()-p2.getObj(); }

	RefView<Eval,Expr1>	p1;
	RefView<Eval,Expr2>	p2;
};

/**
 * 	RefView over multiplication.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2,class Eval>
struct RefView2<Mul,Eval,Expr1,Eval,Expr2,Eval> : IRef<Eval>
{
	RefView2(State& state, const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	Eval value() const { return p1.value() * p2.value(); }

	Rel2<Mul,Expr1,Expr2> getObj()  const
	{ return p1.getObj()*p2.getObj(); }

	RefView<Eval,Expr1>	p1;
	RefView<Eval,Expr2>	p2;
};

/**
 * 	RefView over equality relation.
 * 	\ingroup Views
 **/
template<class Eval,class Expr1,class Expr2>
struct RefView2<Equal,Eval,Expr1,Eval,Expr2,bool> : IRef<bool>
{
	RefView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() == p2.value(); }

	Rel2<Equal,Expr1,Expr2> getObj()  const
	{ return rel<Equal>(p1.getObj(),p2.getObj()); }

	RefView<Eval,Expr1>	p1;
	RefView<Eval,Expr2>	p2;
};

/**
 * 	RefView over inequality.
 * 	\ingroup Views
 **/
template<class Eval,class Expr1,class Expr2>
struct RefView2<Greater,Eval,Expr1,Eval,Expr2,bool> : IRef<bool>
{
	RefView2(State& state, const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() > p2.value(); }

	Rel2<Greater,Expr1,Expr2> getObj()  const
	{ return rel<Greater>(p1.getObj(),p2.getObj()); }

	RefView<Eval,Expr1>	p1;
	RefView<Eval,Expr2>	p2;
};

/**
 * 	RefView over inequality.
 * 	\ingroup Views
 **/
template<class Eval,class Expr1,class Expr2>
struct RefView2<GreaterEqual,Eval,Expr1,Eval,Expr2,bool> : IRef<bool>
{
	RefView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() >= p2.value(); }

	Rel2<GreaterEqual,Expr1,Expr2> getObj()  const
	{ return rel<GreaterEqual>(p1.getObj(),p2.getObj()); }

	RefView<Eval,Expr1>	p1;
	RefView<Eval,Expr2>	p2;
};

/**
 * 	RefView over inequality.
 * 	\ingroup Views
 **/
template<class Eval,class Expr1,class Expr2>
struct RefView2<Less,Eval,Expr1,Eval,Expr2,bool> : IRef<bool>
{
	RefView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() < p2.value(); }

	Rel2<Less,Expr1,Expr2> getObj()  const
	{ return rel<Less>(p1.getObj(),p2.getObj()); }

	RefView<Eval,Expr1>	p1;
	RefView<Eval,Expr2>	p2;
};

/**
 * 	RefView over inequality.
 * 	\ingroup Views
 **/
template<class Eval,class Expr1,class Expr2>
struct RefView2<LessEqual,Eval,Expr1,Eval,Expr2,bool> : IRef<bool>
{
	RefView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() <= p2.value(); }

	Rel2<LessEqual,Expr1,Expr2> getObj()  const
	{ return rel<LessEqual>(p1.getObj(),p2.getObj()); }

	RefView<Eval,Expr1>	p1;
	RefView<Eval,Expr2>	p2;
};

/**
 * 	RefView over disequality.
 * 	\ingroup Views
 **/
template<class Eval,class Expr1,class Expr2>
struct RefView2<Distinct,Eval,Expr1,Eval,Expr2,bool> : IRef<bool>
{
	RefView2(State& state,const Expr1& p1, const Expr2& p2) :
		p1(state,p1),p2(state,p2) {}
	bool value() const { return p1.value() != p2.value(); }

	Rel2<Distinct,Expr1,Expr2> getObj()  const
	{ return rel<Distinct>(p1.getObj(),p2.getObj()); }

	RefView<Eval,Expr1>	p1;
	RefView<Eval,Expr2>	p2;
};

template<class Eval1,class Obj1,class Eval>
struct RefView1<Round,Eval1,Obj1,Eval> : IRef<Eval>
{
	RefView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	Eval value() const { return ::round(p1.value()); }

	Rel1<Round,Obj1> getObj()  const { return round(p1.getObj()); }
	RefView<Eval1,Obj1>	p1;
};

template<class Eval1,class Obj1,class Eval>
struct RefView1<Ceil,Eval1,Obj1,Eval> : IRef<Eval>
{
	RefView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	Eval value() const { return ::ceil(p1.value()); }

	Rel1<Ceil,Obj1> getObj()  const { return ceil(p1.getObj()); }
	RefView<Eval1,Obj1>	p1;
};

template<class Eval1,class Obj1,class Eval>
struct RefView1<Floor,Eval1,Obj1,Eval> : IRef<Eval>
{
	RefView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	Eval value() const { return ::floor(p1.value()); }

	Rel1<Floor,Obj1> getObj()  const { return floor(p1.getObj()); }
	RefView<Eval1,Obj1>	p1;
};

template<class Eval1,class Eval,class Obj1>
struct RefView1<Cast<Eval>,Eval1,Obj1,Eval> : IRef<Eval>
{
	RefView1(State& state, const Obj1& p1) :
		p1(state,p1)	{}
	Eval value() const { return static_cast<Eval>(p1.value()); }

	Rel1<Round,Obj1> getObj()  const { return cast<Eval>(p1.getObj()); }
	RefView<Eval1,Obj1>	p1;
};

template<class Eval,class T>
RefView<Eval,T> newRefView(State& state,const T& t)
{	return RefView<Eval,T>(state,t);	}

template<class ArrayEval,class IndexEval,class Array,class Index,class Eval>
struct RefView2<Element,ArrayEval,Array,IndexEval,Index,Eval> : IRef<Eval>
{
	typedef typename Traits::GetElem<Array>::Type	ArrayElem;
	RefView2(State& state, const Array& s, const Index& idx) :
		state(state),s(state,s),idx(state,idx) {}
	//Eval value() const { return RefView<Eval,ArrayElem>(state,s[idx.value()]).value();	}
	Eval value() const { return  newRefView<Eval>(state,s[idx.value()]).value();	}
	void setValue(const Eval& v)
	{
		//RefView<Eval,ArrayElem>(state,s[idx.value()]).setValue(v);
		newRefView<Eval>(state,s[idx.value()]).setValue(v);
	}


	State& state;
	RefArrayView<Array>	s;
	RefView<IndexEval,Index> idx;
};

template<class Set, class Cond,class ExprEval,class Expr>
struct RefView4<ArgMax,int,Ref<int>,Seq<int>,Set,bool,Cond,ExprEval,Expr,int> : IRef<int>
{
	RefView4(State& state, const Ref<int>& v, const Set& s, const Cond& c,const Expr& e) :
		v(v),s(s),c(c),e(state,e) {}
	int value() const
	{
		int bestIdx;
		ExprEval bestEval = limits<ExprEval>::negInf();
		for (Casper::Detail::PIteration<Ref<int>,Set,Cond> it(v,s,c);
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

	Ref<int> v;
	Set	s;
	Cond c;
	RefView<ExprEval,Expr> e;
};

template<class Set, class Cond,class ExprEval,class Expr>
struct RefView4<ArgMax,int,Ref<int>,Seq<int>,Set,bool,Cond,Seq<ExprEval>,Expr,int> : IRef<int>
{
	RefView4(State& state, const Ref<int>& v, const Set& s, const Cond& c,const Expr& e) :
		v(v),s(s),c(c),e(state,e) {}
	int value() const
	{
		int bestIdx;
		Util::StdVector<ExprEval> bestEvals(e.size());
		bestEvals = limits<ExprEval>::negInf();
		for (Casper::Detail::PIteration<Ref<int>,Set,Cond> it(v,s,c);
				it.valid(); it.iterate())
		{
			for (uint i = 0; i < e.size(); ++i)
				if (e[i].value() < bestEvals[i])
					break;
				else
				if (e[i].value() > bestEvals[i])
				{
					for (uint j = i; j < e.size(); ++j)
						bestEvals[j] = e[j].value();
					bestIdx = v.value();
					break;
				}
		}
		return bestIdx;
	}

	Ref<int> v;
	Set	s;
	Cond c;
	RefArray<ExprEval> e;
};

template<class Set, class ExprEval,class Expr>
struct RefView3<ArgMax,int,Ref<int>,Seq<int>,Set,ExprEval,Expr,int> :
	RefView4<ArgMax,int,Ref<int>,Seq<int>,Set,bool,bool,ExprEval,Expr,int>
{
	typedef RefView4<ArgMax,int,Ref<int>,Seq<int>,Set,bool,bool,ExprEval,Expr,int> Super;

	RefView3(State& state, const Ref<int>& v, const Set& s, const Expr& e) :
		Super(state,v,s,true,e) {}
};

template<class Set, class Cond,class ExprEval,class Expr>
struct RefView4<ArgMin,int,Ref<int>,Seq<int>,Set,bool,Cond,ExprEval,Expr,int> : IRef<int>
{
	RefView4(State& state, const Ref<int>& v, const Set& s, const Cond& c,const Expr& e) :
		v(v),s(s),c(c),e(state,e) {}
	int value() const
	{
		int bestIdx;
		ExprEval bestEval = limits<ExprEval>::posInf();
		for (Casper::Detail::PIteration<Ref<int>,Set,Cond> it(v,s,c);
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

	Ref<int> v;
	Set	s;
	Cond c;
	RefView<ExprEval,Expr> e;
};

template<class Set, class Cond,class ExprEval,class Expr>
struct RefView4<ArgMin,int,Ref<int>,Seq<int>,Set,bool,Cond,Seq<ExprEval>,Expr,int> : IRef<int>
{
	RefView4(State& state, const Ref<int>& v, const Set& s, const Cond& c,const Expr& e) :
		v(v),s(s),c(c),e(state,e) {}
	int value() const
	{
		int bestIdx;
		Util::StdVector<ExprEval> bestEvals(e.size());
		bestEvals = limits<ExprEval>::posInf();
		for (Casper::Detail::PIteration<Ref<int>,Set,Cond> it(v,s,c);
				it.valid(); it.iterate())
		{
			for (uint i = 0; i < e.size(); ++i)
				if (e[i].value() > bestEvals[i])
					break;
				else
				if (e[i].value() < bestEvals[i])
				{
					for (uint j = i; j < e.size(); ++j)
						bestEvals[j] = e[j].value();
					bestIdx = v.value();
					break;
				}
		}
		return bestIdx;
	}

	Ref<int> v;
	Set	s;
	Cond c;
	RefArray<ExprEval> e;
};

template<class Set, class ExprEval,class Expr>
struct RefView3<ArgMin,int,Ref<int>,Seq<int>,Set,ExprEval,Expr,int> :
	RefView4<ArgMin,int,Ref<int>,Seq<int>,Set,bool,bool,ExprEval,Expr,int>
{
	typedef RefView4<ArgMin,int,Ref<int>,Seq<int>,Set,bool,bool,ExprEval,Expr,int> Super;

	RefView3(State& state, const Ref<int>& v, const Set& s, const Expr& e) :
		Super(state,v,s,true,e) {}
};

template<class Set, class Cond,class Expr,class Eval>
struct RefView4<Max,int,Ref<int>,Seq<int>,Set,bool,Cond,Eval,Expr,Eval> : IRef<Eval>
{
	RefView4(State& state, const Ref<int>& v, const Set& s, const Cond& c,const Expr& e) :
		v(v),s(s),c(c),e(state,e) {}
	Eval value() const
	{
		Eval bestEval = limits<Eval>::negInf();
		for (Casper::Detail::PIteration<Ref<int>,Set,Cond> it(v,s,c);
				it.valid(); it.iterate())
			if (e.value() > bestEval)
				bestEval = e.value();
		return bestEval;
	}

	Ref<int> v;
	Set	s;
	Cond c;
	RefView<Eval,Expr> e;
};


template<class Set, class Expr,class Eval>
struct RefView3<Max,int,Ref<int>,Seq<int>,Set,Eval,Expr,Eval> :
	RefView4<Max,int,Ref<int>,Seq<int>,Set,bool,bool,Eval,Expr,Eval>
{
	typedef RefView4<Max,int,Ref<int>,Seq<int>,Set,bool,bool,Eval,Expr,Eval>	Super;
	RefView3(State& state, const Ref<int>& v, const Set& s, const Expr& e) :
		Super(state,v,s,true,e) {}
};

template<class Set, class Cond, class Expr,class Eval>
struct RefView4<Min,int,Ref<int>,Seq<int>,Set,bool,Cond,Eval,Expr,Eval> : IRef<Eval>
{
	RefView4(State& state, const Ref<int>& v, const Set& s, const Cond& c, const Expr& e) :
		v(v),s(s),c(c),e(state,e) {}
	Eval value() const
	{
		Eval bestEval = limits<Eval>::posInf();
		for (Casper::Detail::PIteration<Ref<int>,Set,Cond> it(v,s,c);
				it.valid(); it.iterate())
			if (e.value() < bestEval)
				bestEval = e.value();
		return bestEval;
	}

	Ref<int> v;
	Set	s;
	Cond c;
	RefView<Eval,Expr> e;
};

template<class Set, class Expr,class Eval>
struct RefView3<Min,int,Ref<int>,Seq<int>,Set,Eval,Expr,Eval> :
	RefView4<Min,int,Ref<int>,Seq<int>,Set,bool,bool,Eval,Expr,Eval>
{
	typedef RefView4<Min,int,Ref<int>,Seq<int>,Set,bool,bool,Eval,Expr,Eval>	Super;
	RefView3(State& state, const Ref<int>& v, const Set& s, const Expr& e) :
		Super(state,v,s,true,e) {}
};

// FIXME: Attach is not correct, must fix when RefArray is ready
template<class Set, class Expr,class Eval>
struct RefView3<MaxDiff,int,Ref<int>,Seq<int>,Set,Eval,Expr,Eval> : IRef<Eval>
{
	RefView3(State& state, const Ref<int>& v, const Set& s, const Expr& e) :
		v(v),s(s),e(state,e) {}
	Eval value() const
	{
		Eval bestMin = limits<Eval>::posInf();
		Eval bestMax = limits<Eval>::negInf();
		for (Casper::Detail::PIteration<Ref<int>,Set,bool> it(v,s,true);
				it.valid(); it.iterate())
		{
			if (e.value() < bestMin)
				bestMin = e.value();
			if (e.value() > bestMax)
				bestMax = e.value();
		}
		return bestMax-bestMin;
	}

	Ref<int> v;
	Set	s;
	RefView<Eval,Expr> e;
};

template<class Set, class Expr>
struct RefView3<ForSome,int,Ref<int>,Seq<int>,Set,bool,Expr,bool> : IRef<bool>
{
	RefView3(State& state, const Ref<int>& v, const Set& s, const Expr& e) :
		v(v),s(s),e(state,e) {}
	bool value() const
	{
		for (Casper::Detail::PIteration<Ref<int>,Set,bool> it(v,s,true);
				it.valid(); it.iterate())
			if (e.value())
				return true;
		return false;
	}

	Ref<int> v;
	Set	s;
	RefView<bool,Expr> e;
};


template<class Eval>
struct RefView2<RandInRange,Eval,Eval,Eval,Eval,Eval> : IRef<Eval>
{
	RefView2(State& state, const Eval& lb, const Eval& ub) :
		lb(lb),ub(ub) {}
	Eval value() const
	{
		return static_cast<Eval>(::rand()/static_cast<double>(RAND_MAX) * (ub-lb) + lb);
	}

	const Eval lb;
	const Eval ub;
};


template<class Set, class Cond,class Expr,class Eval>
struct RefView4<Sum,int,Ref<int>,Seq<int>,Set,bool,Cond,Eval,Expr,Eval> : IRef<Eval>
{
	RefView4(State& state, const Ref<int>& v, const Set& s, const Cond& c, const Expr& e) :
		v(v),s(s),cond(c),e(state,e) {}
	Eval value() const
	{
		Eval sum = 0;
		for (Casper::Detail::PIteration<Ref<int>,Set,Cond> it(v,s,cond);
				it.valid(); it.iterate())
			sum += e.value();
		return sum;
	}
	Ref<int> v;
	Set	s;
	Cond cond;
	RefView<Eval,Expr> e;
};


template<class Set, class Expr,class Eval>
struct RefView3<Sum,int,Ref<int>,Seq<int>,Set,Eval,Expr,Eval> :
	RefView4<Sum,int,Ref<int>,Seq<int>,Set,bool,bool,Eval,Expr,Eval>
{
	RefView3(State& state, const Ref<int>& v, const Set& s, const Expr& e) :
		RefView4<Sum,int,Ref<int>,Seq<int>,Set,bool,bool,Eval,Expr,Eval>(state,v,s,true,e) {}
};

template<class Cond,class IfTrue, class IfFalse,class Eval>
struct RefView3<IfThenElse,bool,Cond,Eval,IfTrue,Eval,IfFalse,Eval> : IRef<Eval>
{
	RefView3(State& state, const Cond& c, const IfTrue& t, const IfFalse& f) :
		c(state,c),t(state,t),f(state,f) {}
	Eval value() const
	{
		if (c.value())
			return t.value();
		else
			return f.value();
	}

	RefView<bool,Cond> c;
	RefView<Eval,IfTrue>  t;
	RefView<Eval,IfFalse> f;
};

};

#endif /*_H_CASPER_KERNEL_VIEW_REF */
