 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_RELATION
#define _H_CASPER_KERNEL_RELATION

#include <casper/kernel/common.h>
#include <iostream> // temp
//#include <casper/kernel/memory.h>
//#include <cxxabi.h>	// not sure if this is portable

namespace casper {

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

template<class,class>	class DomVar;
template<class>	class MutVar;
template<class,int>	struct Array;

struct Element;

template<class Functor>
struct Rel0
{
	static const UInt arity = 0;
	typedef Rel0<Functor>		Self;

	Rel0() {}
	Rel0(const Rel0<Functor>& p) {}
};

template<class Functor,class T1>
struct Rel1
{
	static const UInt arity = 1;
	typedef T1						P1;
	typedef Rel1<Functor,T1>		Self;
	const P1						p1;
	Rel1(const P1& p1) : p1(p1) {}
	Rel1(const Self& p) : p1(p.p1) {}
};

template<class Functor,class T1>
Rel1<Functor,T1> rel(const T1& t1)
{	return Rel1<Functor,T1>(t1);	}

template<class Functor,class T1,class T2>
struct Rel2
{
	static const UInt arity = 2;
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

private:
	void operator=(const Self&);
};

template<class Functor,class T1,class T2>
Rel2<Functor,T1,T2> rel(const T1& t1,const T2& t2)
{	return Rel2<Functor,T1,T2>(t1,t2);	}

template<class Functor,class T1,class T2,class T3>
struct Rel3
{
	static const UInt arity = 3;
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
};

template<class Functor,class T1,class T2,class T3>
Rel3<Functor,T1,T2,T3> rel(const T1& t1,const T2& t2,const T3& t3)
{	return Rel3<Functor,T1,T2,T3>(t1,t2,t3);	}

template<class Functor,class T1,class T2,class T3,class T4>
struct Rel4
{
	static const UInt arity = 4;
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

template<class Functor,class T1,class T2,class T3,class T4,class T5>
struct Rel5
{
	static const UInt arity = 5;
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

#define NEW_REL_0(FunName,ObjName) \
struct ObjName; \
NEW_FN_0(FunName,ObjName)

#define NEW_REL_1(FunName,ObjName) \
struct ObjName; \
NEW_FN_1(FunName,ObjName)

#define NEW_REL_2(FunName,ObjName) \
struct ObjName; \
NEW_FN_2(FunName,ObjName)

#define NEW_REL_3(FunName,ObjName) \
struct ObjName; \
NEW_FN_3(FunName,ObjName)

#define NEW_REL_4(FunName,ObjName) \
struct ObjName; \
NEW_FN_4(FunName,ObjName)

#define NEW_REL_5(FunName,ObjName) \
struct ObjName; \
NEW_FN_5(FunName,ObjName)

// ---

#ifdef CASPER_NO_OPERATORS
struct Equal;
struct Distinct;
struct Less;
struct LessEqual;
struct Greater;
struct GreaterEqual;
struct Not;
struct Add;
struct Sub;
struct Mul;
struct Div;
struct Sym;
struct Element;
struct And;
struct Or;
struct XOr;
struct Ground;
struct Abs;
struct Exp;
struct Log;
struct Min;
struct Max;
struct Mod;
struct Linear;
struct Sum;
struct IfThenElse;
struct Sqr;
struct ForAll2;
struct All;
struct Pow;
struct InRange;
struct Cache;
struct InTable;
struct NotInTable;
struct SumProduct;

#else

NEW_REL_1(domain,Domain)

NEW_REL_2(equal,Equal)
NEW_REL_2(distinct,Distinct)       	// !=
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
NEW_REL_3(element,Element)						// a[b]==c

NEW_REL_2(and_,And)						// logical and
NEW_REL_2(or_,Or)						// logical or
NEW_REL_2(xor_,XOr)						// logical or

NEW_REL_1(ground,Ground)
//NEW_REL_2(select,Select)

NEW_REL_1(abs,Abs)
NEW_REL_1(exp,Exp)
NEW_REL_1(sqr,Sqr)
NEW_REL_1(log,Log)
NEW_REL_1(min,Min)
NEW_REL_1(max,Max)

NEW_REL_2(min,Min)
NEW_REL_2(max,Max)

NEW_REL_2(mod,Mod)

NEW_REL_2(linear,Linear)
NEW_REL_3(linear,Linear)

NEW_REL_2(sumProduct,SumProduct)

NEW_REL_1(sum,Sum)
NEW_REL_2(sum,Sum)

NEW_REL_3(ifThenElse,IfThenElse)

NEW_REL_2(forAll2,ForAll2)
NEW_REL_1(cache,Cache)
NEW_REL_3(cache,Cache)
NEW_REL_1(fastCache,FastCache)
NEW_REL_1(virtualView,VirtualView)
NEW_REL_3(inRange,InRange)

NEW_REL_4(all,All)

NEW_REL_2(inTable,InTable)
NEW_REL_2(notInTable,NotInTable)

NEW_REL_5(cumulative,Cumulative)
#endif

// operators

// comment this if you never seen a loop in the compilation process
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

// cast constraints
// TODO: add more (round,floor,ceil)
template<class AsT> struct Cast {	typedef AsT	As;	};

template<class As,class T>
Rel1<Cast<As>,T> cast(const T& t)
{	return Rel1<Cast<As>,T>(t);	}

struct UnknownRel;

template<class T,class D>
struct EvalTraits<DomVar<T,D> >
{	typedef	T	Eval;	};

template<class T>
struct EvalTraits<MutVar<T> >
{	typedef	T	Eval;	};

template<class Func,class T>
struct EvalTraits<Rel1<Func,T> >
{	typedef	typename EvalTraits<T>::Eval	Eval;	};

template<class T>
struct EvalTraits<Rel1<Not,T> >
{	typedef	Bool	Eval;	};

// FIXME: this does not work for example for  -(a or b)
template<>
struct EvalTraits<Rel1<Sym,Bool> >
{	typedef	Int	Eval;	};

template<class EvalTo,class T>
struct EvalTraits<Rel1<Cast<EvalTo>,T> >
{	typedef	EvalTo	Eval;	};

//  Special traits class for binary relations, requires T1 and T2 to be
//  already evaluated.
//	\note Default case is a simplification (works most times)
template<class Func,class T1,class T2>
struct Rel2Traits
{	typedef	T1 Eval;	};

//	entry point to Rel2Traits
template<class Func,class T1,class T2>
struct EvalTraits<Rel2<Func,T1,T2> >
{
	typedef	typename Rel2Traits<Func,
								typename EvalTraits<T1>::Eval,
								typename EvalTraits<T2>::Eval>::Eval
			Eval;
};

template<class T1,class T2>
struct Rel2Traits<Equal,T1,T2>
{	typedef	Bool	Eval;	};

template<class T1,class T2>
struct Rel2Traits<Distinct,T1,T2>
{	typedef	Bool	Eval;	};

template<class T1,class T2>
struct Rel2Traits<Less,T1,T2>
{	typedef	Bool	Eval;	};

template<class T1,class T2>
struct Rel2Traits<LessEqual,T1,T2>
{	typedef	Bool	Eval;	};

template<class T1,class T2>
struct Rel2Traits<Greater,T1,T2>
{	typedef	Bool	Eval;	};

template<class T1,class T2>
struct Rel2Traits<GreaterEqual,T1,T2>
{	typedef	Bool	Eval;	};

template<class T1,class T2>
struct Rel2Traits<And,T1,T2>
{	typedef	Bool	Eval;	};

template<class T1,class T2>
struct Rel2Traits<Or,T1,T2>
{	typedef	Bool	Eval;	};

template<class T1,class T2>
struct Rel2Traits<XOr,T1,T2>
{	typedef	Bool	Eval;	};

template<class T2>
struct Rel2Traits<Add,Bool,T2>
{	typedef	T2	Eval;	};

template<>
struct Rel2Traits<Add,Bool,Bool>
{	typedef	Int	Eval;	};

template<class T2>
struct Rel2Traits<Sub,Bool,T2>
{	typedef	T2	Eval;	};

template<>
struct Rel2Traits<Sub,Bool,Bool>
{	typedef	Int	Eval;	};

template<class T1,class T2,class T3>
struct EvalTraits<Rel3<Linear,T1,T2,T3> >
{	typedef	typename EvalTraits<T2>::Eval Eval;	};

template<class T1,class T2,class T3>
struct EvalTraits<Rel3<IfThenElse,T1,T2,T3> >
{	typedef	typename EvalTraits<T2>::Eval Eval;	};

template<class T1,class T2>
struct EvalTraits<Rel2<ForAll2,T1,T2> >
{	typedef	Seq<typename EvalTraits<T2>::Eval>	Eval;	};

template<class T1,class T2,class T3>
struct EvalTraits<Rel3<InRange,T1,T2,T3> >
{	typedef	typename EvalTraits<T1>::Eval	Eval;	};

template<class T1,class T2,class T3>
struct EvalTraits<Rel3<Cache,T1,T2,T3> >
{	typedef	typename EvalTraits<T1>::Eval	Eval;	};

template<class T1,class T2,class T3,class T4>
struct EvalTraits<Rel4<All,T1,T2,T3,T4> >
{	typedef	Seq<typename EvalTraits<T4>::Eval>	Eval;	};

template<class T1>
struct EvalTraits<Rel1<Max,T1> >
{	typedef	typename EvalTraits<typename ElemTraits<T1>::Elem>::Eval 	Eval;	};

template<class T1>
struct EvalTraits<Rel1<Min,T1> >
{	typedef	typename EvalTraits<typename ElemTraits<T1>::Elem>::Eval 	Eval;	};

template<class T1,class T2>
struct EvalTraits<Rel2<Sum,T1,T2> >
{	typedef	typename EvalTraits<T2>::Eval	Eval;	};

template<class T1>
struct EvalTraits<Rel1<Sum,T1> >
{	typedef	typename EvalTraits<typename ElemTraits<T1>::Elem>::Eval	Eval;	};

template<class T1,class T2>
struct EvalTraits<Rel2<InTable,T1,T2> >
{	typedef	Bool Eval;	};

template<class T1,class T2>
struct EvalTraits<Rel2<NotInTable,T1,T2> >
{	typedef	Bool Eval;	};

template<class T1,class T2>
struct EvalTraits<Rel2<SumProduct,T1,T2> >
{	typedef	typename EvalTraits<typename ElemTraits<T2>::Elem>::Eval	Eval;	};

// extract domain from view

template<class> struct DefaultDomType;

template<class View>
struct DomTraits
{
	typedef typename DefaultDomType<typename EvalTraits<View>::Eval>::Type	 	Dom;
};

template<class Eval,class DomT>
struct DomTraits<DomVar<Eval,DomT> >
{	typedef DomT	Dom;	};

template<class T1,class T2>
struct DomTraits<Rel2<Add,T1,T2> >
{	typedef typename DomTraits<T1>::Dom	Dom;	};

template<class T1,class T2>
struct DomTraits<Rel2<Sub,T1,T2> >
{	typedef typename DomTraits<T1>::Dom	Dom;	};

template<class T1,class T2>
struct DomTraits<Rel2<Mul,T1,T2> >
{	typedef typename DomTraits<T1>::Dom	Dom;	};

template<class T1,class T2>
struct DomTraits<Rel2<Pow,T1,T2> >
{	typedef typename DomTraits<T1>::Dom	Dom;	};

template<class T1,class T2>
struct DomTraits<Rel2<Div,T1,T2> >
{	typedef typename DomTraits<T1>::Dom	Dom;	};

template<class T1,class T2>
struct DomTraits<Rel2<Min,T1,T2> >
{	typedef typename DomTraits<T1>::Dom	Dom;	};

template<class T1,class T2>
struct DomTraits<Rel2<Max,T1,T2> >
{	typedef typename DomTraits<T1>::Dom	Dom;	};

template<class T1>
struct DomTraits<Rel1<Sym,T1> >
{	typedef typename DomTraits<T1>::Dom	Dom;	};

template<class T1>
struct DomTraits<Rel1<Abs,T1> >
{	typedef typename DomTraits<T1>::Dom	Dom;	};

template<class T1>
struct DomTraits<Rel1<Exp,T1> >
{	typedef typename DomTraits<T1>::Dom	Dom;	};

template<class T1>
struct DomTraits<Rel1<Log,T1> >
{	typedef typename DomTraits<T1>::Dom	Dom;	};

template<class ArrayView,class IdxView>
struct DomTraits<Rel2<Element,ArrayView,IdxView> >
{	typedef typename DomTraits<typename ElemTraits<ArrayView>::Elem>::Dom	Dom;	};

template<class T1,class T2>
struct DomTraits<Rel2<ForAll2,T1,T2> >
{	typedef	typename DomTraits<T2>::Dom Dom;	};

template<class T1,class T2,class T3>
struct DomTraits<Rel3<InRange,T1,T2,T3> >
{	typedef	typename DomTraits<T1>::Dom Dom;	};

template<class T1,class T2,class T3>
struct DomTraits<Rel3<Cache,T1,T2,T3> >
{	typedef	typename DomTraits<T1>::Dom Dom;	};

template<class T1,class T2,class T3,class T4>
struct DomTraits<Rel4<All,T1,T2,T3,T4> >
{	typedef	typename DomTraits<T4>::Dom Dom;	};

// ----

template<class View1,class View2,class View3,class View4>
struct ElemTraits<Rel4<All,View1,View2,View3,View4> >
{	typedef View4	Elem;	};

template<class View1,class View2>
struct ElemTraits<Rel2<Element,View1,View2> >
{	typedef typename ElemTraits<View1>::Elem	Elem;	};

// ----

namespace expression {
struct Domain;
struct Bound;
struct Value;
struct Mutable;
struct Relation;
struct Filter;
struct Ground;
};

template<class,class> class DomVar;
template<class,class> struct DomExpr;
template<class> struct BndExpr;
template<class> struct ValExpr;
template<class> struct MutExpr;
template<class,class,class> struct DomView;
template<class,class> struct BndView;
template<class,class> struct ValView;
template<class,class> struct MutView;
struct Filter;

template<class View,class Expr>
struct ExprMatch
{	static const bool value = false;	};

template<class Eval,class Dom>
struct ExprMatch<DomVar<Eval,Dom>,expression::Domain>
{	static const bool value = true;	};

template<class Eval,class Dom>
struct ExprMatch<DomExpr<Eval,Dom>,expression::Domain>
{	static const bool value = true;	};

template<class Eval,class View,class Dom>
struct ExprMatch<DomView<Eval,View,Dom>,expression::Domain>
{	static const bool value = true;	};

template<class Eval>
struct ExprMatch<BndExpr<Eval>,expression::Bound>
{	static const bool value = true;	};

template<class Eval,class View>
struct ExprMatch<BndView<Eval,View>,expression::Bound>
{	static const bool value = true;	};

template<class Eval>
struct ExprMatch<ValExpr<Eval>,expression::Value>
{	static const bool value = true;	};

template<class Eval,class View>
struct ExprMatch<ValView<Eval,View>,expression::Value>
{	static const bool value = true;	};

template<class Eval>
struct ExprMatch<MutExpr<Eval>,expression::Mutable>
{	static const bool value = true;	};

template<class Eval,class View>
struct ExprMatch<MutView<Eval,View>,expression::Mutable>
{	static const bool value = true;	};

template<class Func,class View>
struct ExprMatch<Rel1<Func,View>,expression::Relation>
{	static const bool value = true;	};

template<class Func,class View1,class View2>
struct ExprMatch<Rel2<Func,View1,View2>,expression::Relation>
{	static const bool value = true;	};

template<>
struct ExprMatch<Filter,expression::Filter>
{	static const bool value = true;	};

/*
template<class Func,class View,class Test>
struct ExprMatch<Rel1<Func,View>,Test>
{	static const bool value = true;	};

template<class Func,class View1,class View2,class Test>
struct ExprMatch<Rel2<Func,View1,View2>,Test>
{	static const bool value = ExprMatch<View1,Test>::value or
							  ExprMatch<View2,Test>::value;	};
*/
template<class Func,class View>
struct ExprMatch<Rel1<Func,View>,expression::Ground>
{	static const bool value = ExprMatch<View,expression::Value>::value or
							  ExprMatch<View,expression::Mutable>::value;	};

template<class View>
bool isDomExpr(const View& v)
{	return ExprMatch<View,expression::Domain>::value;	}

template<class View>
bool isBndExpr(const View& v)
{	return ExprMatch<View,expression::Bound>::value;	}

template<class View>
bool isValExpr(const View& v)
{	return ExprMatch<View,expression::Value>::value;	}

template<class View>
bool isMutExpr(const View& v)
{	return ExprMatch<View,expression::Mutable>::value;	}

template<class View>
bool isFilter(const View& v)
{	return ExprMatch<View,expression::Filter>::value;	}

template<class View>
bool isRelation(const View& v)
{	return ExprMatch<View,expression::Relation>::value;	}

template<class View>
bool isGround(const View& v)
{	return ExprMatch<View,expression::Ground>::value;	}

/*
		if (isDomExpr(indexView.getCRef()))
		{
			DomView<Int,IdxView> v(solver(),indexView.getCRef());
			if (v->ground())
				pDom = &array[v->value()];
			else
				assert(0);	// TODO: use elementDom
		}
		else
		if (isBndExpr(indexView.getCRef()))
		{
			BndView<Int,IdxView> v(solver(),indexView.getCRef());
			if (v.min()==v.max())
				pDom = &array[v.min()];
			else
			{
				pDom = new Dom(solver());
				solver().post(elementBnd(array,index,DomVar<Eval,Dom>(solver(),pDom)));
			}
		}
		else

		if (index.min()==index.max())
			Super::v = array[index.min()];
		else
			this->solver().post(elementBnd(array,index,Super::v));
  */


/*
template<class T>
struct RelSig;

template<class Func, class T>
struct RelSig<Rel1<Func,T> >
{	typedef Rel1<Func,typename EvalTraits<T>::EvalType>	Type;	};

template<class Func, class T1,class T2>
struct RelSig<Rel2<Func,T1,T2> >
{	typedef Rel2<Func,typename EvalTraits<T1>::EvalType,
					  typename EvalTraits<T2>::EvalType>	Type;	};

template<class Func, class T1,class T2,class T3>
struct RelSig<Rel3<Func,T1,T2,T3> >
{	typedef Rel3<Func,typename EvalTraits<T1>::EvalType,
					  typename EvalTraits<T2>::EvalType,
					  typename EvalTraits<T3>::EvalType>	Type;	};
*/

/*
template<class F>
std::ostream& operator<<(std::ostream& os, const casper::Rel0<F>& r)
{
	int     status;
	char   *realname;
	realname = abi::__cxa_demangle(typeid(r).name(), 0, 0, &status);
	os << realname;
	return os;
}

template<class F,class T1>
std::ostream& operator<<(std::ostream& os, const casper::Rel1<F,T1>& r)
{
	int     status;
	char   *realname;
	realname = abi::__cxa_demangle(typeid(r).name(), 0, 0, &status);
	os << realname;
	return os;
}

template<class F,class T1,class T2>
std::ostream& operator<<(std::ostream& os, const casper::Rel2<F,T1,T2>& r)
{
	int     status;
	char   *realname;
	realname = abi::__cxa_demangle(typeid(r).name(), 0, 0, &status);
	os << realname;
	return os;
}

template<class F,class T1,class T2,class T3>
std::ostream& operator<<(std::ostream& os, const casper::Rel3<F,T1,T2,T3>& r)
{
	int     status;
	char   *realname;
	realname = abi::__cxa_demangle(typeid(r).name(), 0, 0, &status);
	os << realname;
	return os;
}

template<class F,class T1,class T2,class T3,class T4>
std::ostream& operator<<(std::ostream& os, const casper::Rel4<F,T1,T2,T3,T4>& r)
{
	int     status;
	char   *realname;
	realname = abi::__cxa_demangle(typeid(r).name(), 0, 0, &status);
	os << realname;
	return os;
}
*/


};


#endif

namespace casper {

#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_KERNEL_MACRO_RELATION_H
#define CASPER_KERNEL_MACRO_RELATION_H
#include <casper/kernel/macro/relation.h>
#endif
#endif

}
