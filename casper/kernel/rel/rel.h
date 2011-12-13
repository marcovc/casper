 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_REL
#define _H_CASPER_KERNEL_REL

#include <casper/kernel/common.h>
#include <casper/kernel/traits.h>
#include <iostream> // temp
#include <sstream> // temp
//#include <casper/kernel/memory.h>
//#include <cxxabi.h>	// not sure if this is portable
#include <casper/util/assertions.h>


namespace Casper {

/**
 *	\defgroup Relations
 *	\ingroup Kernel
 *
 * 	Represent an algebraic relation between one or more entities, usually
 *  variables, literals, sets of variables or literals, or other relations.
 *
 *  \note Relations capture expressions and predicates, i.e. the evaluation type
 *  of a relation is arbitrary.
 */

/**
 *  \defgroup EqualityRelations Equality Relations
 *  \ingroup Relations
 */

/**
 *  \defgroup InequalityRelations Inequality Relations
 *  \ingroup Relations
 */

/**
 *  \defgroup DisequalityRelations Distinctness Relations
 *  \ingroup Relations
 */

/**
 *  \defgroup ArithRelations Arithmetic Relations
 *  \ingroup Relations
 */

/**
 *  \defgroup LogicalRelations Logical Relations
 *  \ingroup Relations
 */

template<class>	struct Ref;
//template<class,int>	struct Util::StdArray;

template<class Functor> struct Rel0;
template<class Functor,class T1> struct Rel1;
template<class Functor,class T1,class T2> struct Rel2;
template<class Functor,class T1,class T2,class T3> struct Rel3;
template<class Functor,class T1,class T2,class T3,class T4> struct Rel4;

struct Element;

template<class Functor>
struct Rel0
{
	static const uint arity = 0;
	typedef Rel0<Functor>		Self;

	Rel0() {}
	Rel0(const Rel0<Functor>& p) {}
};

template<class Functor,class T1>
struct Rel1
{
	static const uint arity = 1;
	typedef T1						P1;
	typedef Rel1<Functor,T1>		Self;
	const P1						p1;
	Rel1(const P1& p1) : p1(p1) {}
	Rel1(const Self& p) : p1(p.p1) {}

	// for block syntax (see forAll)
	template<class T2>
	Rel2<Functor,T1,T2> operator()(const T2& p2) const;

};

template<class Functor,class T1>
Rel1<Functor,T1> rel(const T1& t1)
{	return Rel1<Functor,T1>(t1);	}

template<class Functor,class T1>
template<class T2>
Rel2<Functor,T1,T2> Rel1<Functor,T1>::operator()(const T2& p2) const
{	return rel<Functor>(p1,p2);	}

template<class Functor,class T1,class T2>
struct Rel2
{
	static const uint arity = 2;
	typedef Rel2<Functor,T1,T2>		Self;
	typedef T1	P1;
	typedef T2	P2;
	const P1	p1;
	const P2	p2;

	Rel2(const P1& p1,const P2& p2) : p1(p1),p2(p2) { }
	Rel2(const Self& p) : p1(p.p1),p2(p.p2) {}

	Rel2<Functor,T2,T1>	switch_args() const
	{	return Rel2<Functor,T2,T1>(p2,p1);	}

	template<class Arg>
	Rel2<Element,Self,Arg> operator[](const Arg& a) const
	{	return Rel2<Element,Self,Arg>(*this,a);	}

	// for block syntax (see forAll)
	template<class T3>
	Rel3<Functor,T1,T2,T3> operator()(const T3& p3) const;

private:
	void operator=(const Self&);
};

template<class Functor,class T1,class T2>
Rel2<Functor,T1,T2> rel(const T1& t1,const T2& t2)
{	return Rel2<Functor,T1,T2>(t1,t2);	}

template<class Functor,class T1,class T2,class T3>
struct Rel3
{
	static const uint arity = 3;
	typedef T1	P1;
	typedef T2	P2;
	typedef T3	P3;
	typedef Rel3<Functor,P1,P2,P3>		Self;

	const P1	p1;
	const P2	p2;
	const P3	p3;

	Rel3(const P1& p1,const P2& p2,const P3& p3) :
		p1(p1),p2(p2),p3(p3) {}

	Rel3(const Self& p) :
		p1(p.p1),p2(p.p2),p3(p.p3) {}

	template<class T4>
	Rel4<Functor,T1,T2,T3,T4> operator()(const T4& t4);
};

template<class Functor,class T1,class T2,class T3>
Rel3<Functor,T1,T2,T3> rel(const T1& t1,const T2& t2,const T3& t3)
{	return Rel3<Functor,T1,T2,T3>(t1,t2,t3);	}

template<class Functor,class T1,class T2>
template<class T3>
Rel3<Functor,T1,T2,T3> Rel2<Functor,T1,T2>::operator()(const T3& p3) const
{	return rel<Functor>(p1,p2,p3);	}

template<class Functor,class T1,class T2,class T3,class T4>
struct Rel4
{
	static const uint arity = 4;
	typedef T1	P1;
	typedef T2	P2;
	typedef T3	P3;
	typedef T4	P4;
	typedef Rel4<Functor,P1,P2,P3,P4>	Self;

	const P1	p1;
	const P2	p2;
	const P3	p3;
	const P4	p4;

	Rel4(const P1& p1,const P2& p2,const P3& p3,const P4& p4) :
		p1(p1),p2(p2),p3(p3),p4(p4) {}

	Rel4(const Self& p) :	p1(p.p1),p2(p.p2),p3(p.p3),p4(p.p4) {}
};

template<class Functor,class T1,class T2,class T3,class T4>
Rel4<Functor,T1,T2,T3,T4> rel(const T1& t1,const T2& t2,const T3& t3,
								const T4& t4)
{	return Rel4<Functor,T1,T2,T3,T4>(t1,t2,t3,t4);	}

template<class Functor,class T1,class T2,class T3>
template<class T4>
Rel4<Functor,T1,T2,T3,T4> Rel3<Functor,T1,T2,T3>::operator()(const T4& p4)
{	return rel<Functor>(p1,p2,p3,p4);	}

template<class Functor,class T1,class T2,class T3,class T4,class T5>
struct Rel5
{
	static const uint arity = 5;
	typedef T1	P1;
	typedef T2	P2;
	typedef T3	P3;
	typedef T4	P4;
	typedef T5	P5;
	typedef Rel5<Functor,P1,P2,P3,P4,P5>	Self;

	const P1	p1;
	const P2	p2;
	const P3	p3;
	const P4	p4;
	const P5	p5;

	Rel5(const P1& p1,const P2& p2,const P3& p3,const P4& p4,const P5& p5) :
		p1(p1),p2(p2),p3(p3),p4(p4),p5(p5) {}

	Rel5(const Self& p) :	p1(p.p1),p2(p.p2),p3(p.p3),p4(p.p4),p5(p.p5) {}
};

template<class Functor,class T1,class T2,class T3,class T4,class T5>
Rel5<Functor,T1,T2,T3,T4,T5> rel(const T1& t1,const T2& t2,const T3& t3,
								const T4& t4,const T5& t5)
{	return Rel5<Functor,T1,T2,T3,T4,T5>(t1,t2,t3,t4,t5);	}

// ---

#define ALIAS(OldName,NewName) \
typedef OldName NewName;

#ifdef CASPER_NO_OPERATORS

#define NEW_FN_0(FunName,ObjName)
#define NEW_FN_1(FunName,ObjName)
#define NEW_FN_2(FunName,ObjName)
#define NEW_FN_3(FunName,ObjName)
#define NEW_FN_4(FunName,ObjName)
#define NEW_FN_5(FunName,ObjName)

#else
#define NEW_FN_0(FunName,ObjName) \
Rel0<ObjName > \
FunName() \
{ return Rel0<ObjName >(); }

#define NEW_FN_1(FunName,ObjName) \
template<class T1> \
Rel1<ObjName,T1> \
FunName(const T1& p1) \
{ return Rel1<ObjName,T1>(p1); }

#define NEW_FN_2(FunName,ObjName) \
template<class T1,class T2> \
Rel2<ObjName,T1,T2> \
FunName(const T1& p1,const T2& p2) \
{ return Rel2<ObjName,T1,T2>(p1,p2); }

#define NEW_FN_3(FunName,ObjName) \
template<class T1,class T2,class T3> \
Rel3<ObjName,T1,T2,T3> \
FunName(const T1& p1,const T2& p2,const T3& p3) \
{ return Rel3<ObjName,T1,T2,T3>(p1,p2,p3); }

#define NEW_FN_4(FunName,ObjName) \
template<class T1,class T2,class T3,class T4> \
Rel4<ObjName,T1,T2,T3,T4> \
FunName(const T1& p1,const T2& p2,const T3& p3,const T4& p4) \
{ return Rel4<ObjName,T1,T2,T3,T4>(p1,p2,p3,p4); }

#define NEW_FN_5(FunName,ObjName) \
template<class T1,class T2,class T3,class T4,class T5> \
Rel5<ObjName,T1,T2,T3,T4,T5> \
FunName(const T1& p1,const T2& p2,const T3& p3,const T4& p4,const T5& p5) \
{ return Rel5<ObjName,T1,T2,T3,T4,T5>(p1,p2,p3,p4,p5); }
#endif

template<class>
struct FuncToStr;

#define FUNC2STR(FunName,ObjName) \
template<> \
struct FuncToStr<ObjName> \
{	const char* operator()() const { return # FunName; } };

#define NEW_REL_0(FunName,ObjName) \
struct ObjName; \
NEW_FN_0(FunName,ObjName) \
FUNC2STR(FunName,ObjName)

#define NEW_REL_1(FunName,ObjName) \
struct ObjName; \
NEW_FN_1(FunName,ObjName) \
FUNC2STR(FunName,ObjName)

#define NEW_REL_2(FunName,ObjName) \
struct ObjName; \
NEW_FN_2(FunName,ObjName) \
FUNC2STR(FunName,ObjName)

#define NEW_REL_3(FunName,ObjName) \
struct ObjName; \
NEW_FN_3(FunName,ObjName) \
FUNC2STR(FunName,ObjName)

#define NEW_REL_4(FunName,ObjName) \
struct ObjName; \
NEW_FN_4(FunName,ObjName) \
FUNC2STR(FunName,ObjName)

#define NEW_REL_5(FunName,ObjName) \
struct ObjName; \
NEW_FN_5(FunName,ObjName) \
FUNC2STR(FunName,ObjName)

// ---



NEW_REL_2(equal,Equal)
NEW_REL_2(distinct,Distinct)       	// !=
NEW_FN_1(distinct,Distinct)
NEW_REL_2(less,Less)       			// <
NEW_REL_2(lessEqual,LessEqual)     	// <=
NEW_REL_2(greater,Greater)       		// <
NEW_REL_2(greaterEqual,GreaterEqual)   // <=

NEW_REL_1(not_,Not)     		// !

NEW_REL_2(add,Add)        				// +
NEW_REL_2(sub,Sub)        				// -
NEW_REL_2(mul,Mul)        				// *
NEW_REL_2(pow,Pow)        				// *
NEW_REL_2(div,Div)						// /
NEW_REL_1(sym,Sym)						// - (symmetric)
//NEW_REL_1(element,Element)						// subscript operator []
NEW_REL_2(element,Element)						// subscript operator a[b]
NEW_REL_3(elementEqual,ElementEqual)						// a[b]==c

NEW_REL_2(and_,And)						// logical and
NEW_REL_2(or_,Or)						// logical or
NEW_REL_2(xor_,XOr)						// logical or

NEW_REL_1(round,Round)
NEW_REL_1(ceil,Ceil)
NEW_REL_1(floor,Floor)

//NEW_REL_2(select,Select)

NEW_REL_1(abs,Abs)
NEW_REL_1(exp,Exp)
NEW_REL_1(sqr,Sqr)
NEW_REL_1(log,Log)
NEW_REL_1(min,Min)
NEW_REL_1(max,Max)

NEW_FN_2(min,Min)
NEW_FN_2(max,Max)

NEW_REL_2(mod,Mod)

NEW_REL_3(sumProductEqual,SumProductEqual)

NEW_REL_2(sumProduct,SumProduct)

NEW_REL_1(sum,Sum)
NEW_FN_3(sum,Sum)
NEW_FN_4(sum,Sum)

NEW_REL_2(sumEqual,SumEqual)

NEW_REL_2(ifThen,IfThen)
NEW_REL_3(ifThenElse,IfThenElse)

NEW_REL_1(cache,Cache)
NEW_FN_3(cache,Cache)
NEW_REL_1(fastCache,FastCache)
NEW_REL_1(virtualView,VirtualView)
NEW_REL_3(inRange,InRange)

NEW_REL_4(all,All)

NEW_REL_2(inTable,InTable)
NEW_REL_2(notInTable,NotInTable)

NEW_REL_5(cumulative,Cumulative)

NEW_FN_3(min,Min)
NEW_FN_3(max,Max)
NEW_REL_3(maxDiff,MaxDiff)
NEW_REL_3(argMin,ArgMin)
NEW_REL_3(argMax,ArgMax)

NEW_FN_4(min,Min)
NEW_FN_4(max,Max)
NEW_FN_4(argMin,ArgMin)
NEW_FN_4(argMax,ArgMax)

NEW_REL_3(forSome,ForSome)

NEW_REL_2(assign,Assign)
NEW_REL_2(selectFirst,SelectFirst)
NEW_FN_3(selectFirst,SelectFirst)

NEW_REL_3(countEqual,CountEqual)

NEW_REL_2(randInRange,RandInRange)


// operators

// comment this if you never seen a compiler looping forever
template< class T>
T operator!(const Rel1<Not,Rel1<Not,T> >& rh)
{ 	return rh.p1.p1;	}


// this might be more complex than it looks like:
/*
template<class Func,class T1,class T2>
template<class Arg>
Rel2<Element,Rel2<Func,T1,T2>,Arg>
Rel2<Func,T1,T2>::operator[](const Arg& arg) const
{	return Rel2<Element,Self,Arg>(*this,arg);	}
*/

template<class AsT> struct Cast {	typedef AsT	As;	};

template<class As,class T>
Rel1<Cast<As>,T> cast(const T& t)
{	return Rel1<Cast<As>,T>(t);	}

template<>
struct FuncToStr<Cast<int> >
{
	const char* operator()() const
	{	return "int" ;}
};

template<>
struct FuncToStr<Cast<bool> >
{
	const char* operator()() const
	{	return "bool" ;}
};

template<>
struct FuncToStr<Cast<double> >
{
	const char* operator()() const
	{	return "double" ;}
};

template<>
struct FuncToStr<Cast<float> >
{
	const char* operator()() const
	{	return "float" ;}
};

struct UnknownRel;

namespace Traits {

template<class Func,class T>
struct GetEval<Rel1<Func,T> >
{	typedef	typename Traits::GetEval<T>::Type	Type;	};

template<class T>
struct GetEval<Rel1<Not,T> >
{	typedef bool	Type;	};

template<class EvalTo,class T>
struct GetEval<Rel1<Cast<EvalTo>,T> >
{	typedef	EvalTo	Type;	};

//  Binary relations
//	\note Default case is a simplification (works most times)

template<class Func,class T1,class T2>
struct GetEval<Rel2<Func,T1,T2> >
{	typedef	typename Traits::GetEval<T1>::Type Type;	};

template<class T1,class T2>
struct GetEval<Rel2<Equal,T1,T2> >
{	typedef	bool	Type;	};

template<class T1,class T2>
struct GetEval<Rel2<Distinct,T1,T2> >
{	typedef	bool	Type;	};

template<class View>
struct GetEval<Rel1<Distinct,View> >
{	typedef	bool	Type;	};

template<class T1,class T2>
struct GetEval<Rel2<Less,T1,T2> >
{	typedef	bool	Type;	};

template<class T1,class T2>
struct GetEval<Rel2<LessEqual,T1,T2> >
{	typedef	bool	Type;	};

template<class T1,class T2>
struct GetEval<Rel2<Greater,T1,T2> >
{	typedef	bool	Type;	};

template<class T1,class T2>
struct GetEval<Rel2<GreaterEqual,T1,T2> >
{	typedef	bool	Type;	};

template<class T1,class T2>
struct GetEval<Rel2<And,T1,T2> >
{	typedef	bool	Type;	};

template<class T1,class T2>
struct GetEval<Rel2<Or,T1,T2> >
{	typedef	bool	Type;	};

template<class T1,class T2>
struct GetEval<Rel2<XOr,T1,T2> >
{	typedef	bool	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<SumProductEqual,T1,T2,T3> >
{	typedef bool Type;	};

template<class T1,class T2>
struct GetEval<Rel2<IfThen,T1,T2> >
{	typedef	typename Traits::GetEval<T2>::Type Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<IfThenElse,T1,T2,T3> >
{	typedef	typename Traits::GetEval<T2>::Type Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<InRange,T1,T2,T3> >
{	typedef typename Traits::GetEval<T1>::Type	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<Cache,T1,T2,T3> >
{	typedef typename Traits::GetEval<T1>::Type	Type;	};

template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<All,T1,T2,T3,T4> >
{	typedef Seq<typename Traits::GetEval<T4>::Type>	Type;	};

template<class T>
struct GetSetElem;

template<class T1>
struct GetEval<Rel1<Max,T1> >
{	typedef typename Traits::GetSetElem<typename Traits::GetEval<typename Traits::GetTermElem<T1>::Type>::Type>::Type 	Type;	};

template<class T1>
struct GetEval<Rel1<Min,T1> >
{	typedef typename Traits::GetSetElem<typename Traits::GetEval<typename Traits::GetTermElem<T1>::Type>::Type>::Type 	Type;	};

template<class T1,class T2>
struct GetEval<Rel2<SumEqual,T1,T2> >
{	typedef bool	Type;	};

template<class T1>
struct GetEval<Rel1<Sum,T1> >
{	typedef	typename Traits::GetEval<typename Traits::GetTermElem<T1>::Type>::Type	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<Sum,T1,T2,T3> >
{	typedef	typename Traits::GetEval<T3>::Type	Type;	};

template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<Sum,T1,T2,T3,T4> >
{	typedef	typename Traits::GetEval<T4>::Type	Type;	};

template<class T1,class T2>
struct GetEval<Rel2<InTable,T1,T2> >
{	typedef	bool Type;	};

template<class T1,class T2>
struct GetEval<Rel2<NotInTable,T1,T2> >
{	typedef	bool Type;	};

template<class T1,class T2>
struct GetEval<Rel2<SumProduct,T1,T2> >
{	typedef	typename Traits::GetEval<typename Traits::GetTermElem<T2>::Type>::Type	Type;	};

template<class T1,class T2,class T3,class T4,class T5>
struct GetEval<Rel5<Cumulative,T1,T2,T3,T4,T5> >
{	typedef	bool Type;	};

template<class Expr1,class Expr2,class Expr3,class Expr4>
struct GetElem<Rel4<All,Expr1,Expr2,Expr3,Expr4> >
{	typedef Expr4	Type;	};

template<class Expr1,class Expr2>
struct GetElem<Rel2<Element,Expr1,Expr2> >
{	typedef typename Traits::GetElem<typename Traits::GetElem<Expr1>::Type>::Type	Type;	};

template<class Expr1,class Expr2,class Expr3,class Expr4>
struct GetTermElem<Rel4<All,Expr1,Expr2,Expr3,Expr4> >
{	typedef Expr4	Type;	};

template<class Expr1,class Expr2>
struct GetTermElem<Rel2<Element,Expr1,Expr2> >
{	typedef typename Traits::GetTermElem<Expr1>::Type	Type;	};

template<class ArrayView, class IdxView>
struct GetEval<Rel2<Element,ArrayView,IdxView> >
{
	typedef typename Traits::GetEval<typename Traits::GetElem<ArrayView>::Type>::Type	Type;
	CASPER_ASSERT_EVAL(Type);
};

template<class ArrayView, class IdxView,class EqView>
struct GetEval<Rel3<ElementEqual,ArrayView,IdxView,EqView> >
{	typedef bool Type;	};

template<class T>
struct GetEval<Seq<Seq<T> > >
{	typedef typename GetEval<Seq<T> >::Type	Type;	};

template<class T>
struct GetTermElem<Seq<T> >
{	typedef typename GetTermElem<T>::Type	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<ArgMin,T1,T2,T3> >
{	typedef typename Traits::GetEval<T1>::Type	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<ArgMax,T1,T2,T3> >
{	typedef typename Traits::GetEval<T1>::Type	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<Min,T1,T2,T3> >
{	typedef typename Traits::GetEval<T3>::Type	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<Max,T1,T2,T3> >
{	typedef typename Traits::GetEval<T3>::Type	Type;	};

template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<ArgMin,T1,T2,T3,T4> >
{	typedef typename Traits::GetEval<T1>::Type	Type;	};

template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<ArgMax,T1,T2,T3,T4> >
{	typedef typename Traits::GetEval<T1>::Type	Type;	};

template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<Min,T1,T2,T3,T4> >
{	typedef typename Traits::GetEval<T4>::Type	Type;	};

template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<Max,T1,T2,T3,T4> >
{	typedef typename Traits::GetEval<T4>::Type	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<MaxDiff,T1,T2,T3> >
{	typedef typename Traits::GetEval<T3>::Type	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<ForSome,T1,T2,T3> >
{	typedef bool	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<CountEqual,T1,T2,T3> >
{	typedef bool	Type;	};

// if the user wants an int he must use cast (as in c++)
template<class T>
struct GetEval<Rel1<Round,T> >
{	typedef typename Traits::GetEval<T>::Type	Type;	};

template<class T>
struct GetEval<Rel2<RandInRange,T,T> >
{	typedef T Type;	};

template<class Expr1, class Expr2>
struct GetEval<Rel2<Assign,Expr1,Expr2> >
{	typedef bool	Type;	};

template<class Expr1, class Expr2>
struct GetEval<Rel2<SelectFirst,Expr1,Expr2> >
{	typedef bool	Type;	};

template<class Expr1, class Expr2,class Expr3>
struct GetEval<Rel3<SelectFirst,Expr1,Expr2,Expr3> >
{	typedef bool	Type;	};

} // Traits


template<class> struct GetPState;
struct State;

template<class Func,class T1>
struct GetPState<Rel1<Func,T1> >
{
	State* operator()(const Rel1<Func,T1>& s)
	{ return getPState(s.p1); }
};

template<class Func,class T1,class T2>
struct GetPState<Rel2<Func,T1,T2> >
{
	State* operator()(const Rel2<Func,T1,T2>& r)
	{	return getPState(r.p1,r.p2); }
};

template<class Func,class T1,class T2,class T3>
struct GetPState<Rel3<Func,T1,T2,T3> >
{
	State*	operator()(const Rel3<Func,T1,T2,T3>& r)
	{	return getPState(r.p1,r.p2,r.p3);	}
};

template<class Func,class T1,class T2,class T3,class T4>
struct GetPState<Rel4<Func,T1,T2,T3,T4> >
{
	State*	operator()(const Rel4<Func,T1,T2,T3,T4>& r)
	{	return getPState(r.p1,r.p2,r.p3,r.p4);}
};

template<class Func,class T1,class T2,class T3,class T4,class T5>
struct GetPState<Rel5<Func,T1,T2,T3,T4,T5> >
{
	State*	operator()(const Rel5<Func,T1,T2,T3,T4,T5>& r)
	{	return getPState(r.p1,r.p2,r.p3,r.p4,r.p5);}
};

template<class ArrayT,class IndexT>
struct IterationView<Rel2<Element,ArrayT,IndexT> > :
	IterationView<typename ArrayT::Elem>
{
	typedef IterationView<typename ArrayT::Elem> Super;
	typedef Rel2<Element,ArrayT,IndexT> RelT;
	IterationView(const RelT& v) : Super(v.p1[v.p2.value()]) {}
};

};

template<class F>
std::ostream& operator<<(std::ostream& os, const Casper::Rel0<F>& r)
{
	os << Casper::FuncToStr<F>()();
	return os;
}

template<class F,class T1>
std::ostream& operator<<(std::ostream& os, const Casper::Rel1<F,T1>& r)
{
	os << Casper::FuncToStr<F>()() << "(" << r.p1 << ")";
	return os;
}

template<class F,class T1,class T2>
std::ostream& operator<<(std::ostream& os, const Casper::Rel2<F,T1,T2>& r)
{
	os << Casper::FuncToStr<F>()() << "(" << r.p1 << "," << r.p2 << ")";
	return os;
}

template<class F,class T1,class T2,class T3>
std::ostream& operator<<(std::ostream& os, const Casper::Rel3<F,T1,T2,T3>& r)
{
	os << Casper::FuncToStr<F>()() << "(" << r.p1 << "," << r.p2 << "," << r.p3 << ")";
	return os;
}

template<class F,class T1,class T2,class T3,class T4>
std::ostream& operator<<(std::ostream& os, const Casper::Rel4<F,T1,T2,T3,T4>& r)
{
	os << Casper::FuncToStr<F>()() << "(" << r.p1 << "," << r.p2
	   << "," << r.p3 << "," << r.p4 << ")";
	return os;
}

template<class F,class T1,class T2,class T3,class T4,class T5>
std::ostream& operator<<(std::ostream& os, const Casper::Rel5<F,T1,T2,T3,T4,T5>& r)
{
	os << Casper::FuncToStr<F>()() << "(" << r.p1 << "," << r.p2
	   << "," << r.p3 << "," << r.p4 << "," << r.p5 << ")";
	return os;
}

#endif


#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_KERNEL_REL_OP_REL_H
#define CASPER_KERNEL_REL_OP_REL_H
#include <casper/kernel/rel/op_rel.h>
#endif
#endif
