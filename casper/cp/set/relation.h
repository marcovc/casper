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

#include <casper/cp/traits.h>

namespace Casper {


NEW_REL_1(cardinal,Cardinal)
NEW_REL_2(member,Member)
NEW_REL_2(notMember,NotMember)
NEW_REL_2(disjoint,Disjoint)

NEW_REL_2(contained,Contained)

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

namespace CP {
namespace Traits {
template<class View>
struct GetDom<Rel1<Cardinal,View> >
{	typedef typename Traits::GetDefaultDom<int>::Type	Type;	};
} // Traits
} // CP

namespace Traits {

template<class View>
struct GetEval<Rel1<Cardinal,View> >
{	typedef int		Type;	};

template<class View1,class View2>
struct GetEval<Rel2<Member,View1,View2> >
{	typedef bool	Type;	};

template<class View1,class View2>
struct GetEval<Rel2<NotMember,View1,View2> >
{	typedef bool	Type;	};

template<class View1,class View2>
struct GetEval<Rel2<Contained,View1,View2> >
{	typedef bool	Type;	};

template<class View1,class View2>
struct GetEval<Rel2<Disjoint,View1,View2> >
{	typedef bool	Type;	};

template<class View1,class View2>
struct GetEval<Rel2<SymDiff,View1,View2> >
{	typedef typename Casper::Traits::GetEval<View1>::Type	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<UnionEqual,T1,T2,T3> >
{	typedef	bool Type;	};

template<class T1,class T2>
struct GetEval<Rel2<UnionEqual,T1,T2> >
{	typedef	bool Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<Intersect,T1,T2,T3> >
{	typedef	bool Type;	};

template<class T1>
struct GetEval<Rel1<Partition,T1> >
{	typedef	bool Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<SumEqual,T1,T2,T3> >
{	typedef	bool Type;	};

template<class T1,class T2>
struct GetEval<Rel2<SumEqual,T1,T2> >
{	typedef	bool Type;	};

} // Traits
} // Casper

#endif /*CASPER_SET_RELATION_H_*/
