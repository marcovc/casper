/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_KERNEL_OBJ_BNDEXPR_H_
#define CASPER_KERNEL_OBJ_BNDEXPR_H_

#include <casper/kernel/spexpr/expr.h>

namespace Casper {
namespace Detail {

template<class T,class Eval>
struct Create<T,CP::BndExpr<Eval> >
{
	CP::BndExpr<Eval> operator()(CP::Store& store, const T& t);
};

template<class T,class Eval>
CP::BndExpr<Eval> Create<T,CP::BndExpr<Eval> >::operator()(CP::Store& store, const T& t)
{	return CP::BndExpr<Eval>(store,t);	}

//template<class Eval,class T>
//CP::BndExpr<Eval> ExprWrapper<Eval,T>::toCPBndExpr(CP::Store& store) const
//{	return Create<T,CP::BndExpr<Eval> >()(store,t);	}
//
//template<class T>
//CP::BndExpr<bool> ExprWrapper<bool,T>::toCPBndExpr(CP::Store& store) const
//{	return Create<T,CP::BndExpr<bool> >()(store,t);	}

} // Detail

namespace CP {

template<class Eval>
struct BndView<Eval,Expr<Eval> > : BndExpr<Eval>
{
	BndView(Store& store, const Expr<Eval>& e) :
		BndExpr<Eval>(e.toCPBndExpr(store)),expr(e) {}
	const Expr<Eval>& getObj() const {	return expr; }
	Expr<Eval> expr;
};

// element among Expr<Seq<Eval> >
template<class Eval>
struct BndViewRel2<Element,Expr<Seq<Eval> >,Expr<int>,Eval> :
		BndView<Eval,Expr<Eval> >
{
	CASPER_ASSERT_BNDVIEW_EVAL(Eval)

	typedef BndView<Eval,Expr<Eval> >	Super;
	typedef Var<Eval,typename Traits::GetDefaultDom<Eval>::Type> Elem;
	static Super getSuper(Store& s,const Expr<Seq<Eval> >& p1,const Expr<int>& p2)
	{
		if (ValView<int,Expr<int> >(s,p2).ground())
		{
			int idx = ValView<int,Expr<int> >(s,p2).value();

			ElementView<Expr<Seq<Eval> > > elem(p1);
			return Super(s,elem.get(idx));
		}

		Elem v(s,Detail::VarDomCreator<typename Elem::Dom>().unionOf(s,p1));
		s.post(elementEqual(p1,p2,v));
		return Super(s,v);
	}

	Rel2<Element,Expr<Seq<Eval> >,Expr<int> > getObj() const
	{	return Rel2<Element,Expr<Seq<Eval> >,Expr<int> >(v1,v2); }

	BndViewRel2(Store& store,const Expr<Seq<Eval> >& p1, const Expr<int>& p2) :
		Super(getSuper(store,p1,p2)),
		v1(p1),v2(p2)
	{}

	Expr<Seq<Eval> > v1;
	Expr<int> v2;
};

} // CP

}

#endif /* CASPER_KERNEL_OBJ_BNDEXPR_H_ */
