/***************************************************************************
 *   Copyright (C) 2008 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CASPER_SET_RELATION_H_
#define CASPER_SET_RELATION_H_

namespace casper {


NEW_REL_1(cardinal,Cardinal)

template<class View>
struct EvalTraits<Rel1<Cardinal,View> >
{	typedef Int	Eval;	};

template<class View>
struct DomTraits<Rel1<Cardinal,View> >
{	typedef typename DefaultDomType<Int>::Type	Dom;	};


NEW_REL_2(member,Member)

template<class View1,class View2>
struct EvalTraits<Rel2<Member,View1,View2> >
{	typedef Bool	Eval;	};

NEW_REL_2(notMember,NotMember)

template<class View1,class View2>
struct EvalTraits<Rel2<NotMember,View1,View2> >
{	typedef Bool	Eval;	};

NEW_REL_2(contained,Contained)

template<class View1,class View2>
struct EvalTraits<Rel2<Contained,View1,View2> >
{	typedef Bool	Eval;	};

NEW_REL_2(disjoint,Disjoint)

template<class View1,class View2>
struct EvalTraits<Rel2<Disjoint,View1,View2> >
{	typedef Bool	Eval;	};


NEW_REL_1(disjoint,Disjoint)

ALIAS(Mul,Intersect)
NEW_FN_2(intersect,Intersect)
NEW_FN_3(intersect,Intersect)


ALIAS(Add,Union)
NEW_FN_2(union_,Union)
NEW_REL_3(unionEqual,UnionEqual)
NEW_FN_2(unionEqual,UnionEqual)

NEW_REL_1(partition,Partition)

NEW_REL_2(sumEqual,SumEqual)
NEW_FN_3(sumEqual,SumEqual)

NEW_REL_2(symDiff,SymDiff)

template<class View1,class View2>
struct EvalTraits<Rel2<SymDiff,View1,View2> >
{	typedef typename EvalTraits<View1>::Eval	Eval;	};

template<class T1,class T2,class T3>
struct EvalTraits<Rel3<UnionEqual,T1,T2,T3> >
{	typedef	Bool Eval;	};

template<class T1,class T2>
struct EvalTraits<Rel2<UnionEqual,T1,T2> >
{	typedef	Bool Eval;	};

template<class T1,class T2,class T3>
struct EvalTraits<Rel3<Intersect,T1,T2,T3> >
{	typedef	Bool Eval;	};

template<class T1>
struct EvalTraits<Rel1<Partition,T1> >
{	typedef	Bool Eval;	};

template<class T1,class T2,class T3>
struct EvalTraits<Rel3<SumEqual,T1,T2,T3> >
{	typedef	Bool Eval;	};

template<class T1,class T2>
struct EvalTraits<Rel2<SumEqual,T1,T2> >
{	typedef	Bool Eval;	};

};

#endif /*CASPER_SET_RELATION_H_*/
