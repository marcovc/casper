/***************************************************************************
 *   Copyright (C) 2008 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CASPER_KERNEL_SETREL_H_
#define CASPER_KERNEL_SETREL_H_

namespace Casper {


NEW_REL_1(cardinal,Cardinal)
NEW_REL_2(member,Member)
NEW_REL_2(notMember,NotMember)

NEW_REL_2(disjoint,Disjoint)
NEW_FN_1(disjoint,Disjoint)

NEW_REL_2(contained,Contained)


ALIAS(Mul,Intersect)
NEW_FN_2(intersect,Intersect)
NEW_REL_3(intersectEqual,IntersectEqual)


ALIAS(Add,Union)
NEW_FN_2(union_,Union)
NEW_REL_3(unionEqual,UnionEqual)
NEW_FN_2(unionEqual,UnionEqual)

NEW_REL_1(partition,Partition)

NEW_FN_3(sumEqual,SumEqual)

//NEW_REL_2(symDiff,SymDiff)

namespace Traits {

template<class View>
struct GetEval<Rel1<Cardinal,View> >
{	typedef int		Type;	};

template<class Expr1,class Expr2>
struct GetEval<Rel2<Member,Expr1,Expr2> >
{	typedef bool	Type;	};

template<class Expr1,class Expr2>
struct GetEval<Rel2<NotMember,Expr1,Expr2> >
{	typedef bool	Type;	};

template<class Expr1,class Expr2>
struct GetEval<Rel2<Contained,Expr1,Expr2> >
{	typedef bool	Type;	};

template<class Expr1,class Expr2>
struct GetEval<Rel2<Disjoint,Expr1,Expr2> >
{	typedef bool	Type;	};

template<class Expr1>
struct GetEval<Rel1<Disjoint,Expr1> >
{	typedef bool	Type;	};

//template<class Expr1,class Expr2>
//struct GetEval<Rel2<SymDiff,Expr1,Expr2> >
//{	typedef typename Casper::Traits::GetEval<Expr1>::Type	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<UnionEqual,T1,T2,T3> >
{	typedef	bool Type;	};

template<class T1,class T2>
struct GetEval<Rel2<UnionEqual,T1,T2> >
{	typedef	bool Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<IntersectEqual,T1,T2,T3> >
{	typedef	bool Type;	};

template<class T1>
struct GetEval<Rel1<Partition,T1> >
{	typedef	bool Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<SumEqual,T1,T2,T3> >
{	typedef	bool Type;	};


} // Traits
} // Casper

#endif /*CASPER_KERNEL_SETREL_H_*/
