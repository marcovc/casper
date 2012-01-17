 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_KERNEL_OBJ_ELEMENT_H_
#define CASPER_KERNEL_OBJ_ELEMENT_H_

#include <casper/kernel/expr.h>
#include <casper/kernel/spexpr/expr.h>

namespace Casper {
namespace Detail {

template<class FromObj,class ToEval>
struct ElementExprAdaptor : IElementExpr<Expr<ToEval> >
{
	typedef Expr<ToEval> Elem;
	ElementExprAdaptor(const FromObj& e) { throw 0; }
	ElementExprAdaptor(const ElementExprAdaptor& s) { throw 0; }

	Elem 		get(uint i) const 	{	throw 0; }
	ElementExpr<Elem>		copy() const { throw 0; }
};

template<class FromObj>
struct ElementExprAdaptor<FromObj,typename Traits::GetEval<typename Traits::GetElem<FromObj>::Type>::Type>
			: IElementExpr<Expr<typename Traits::GetEval<typename Traits::GetElem<FromObj>::Type>::Type> >
{
	typedef typename Traits::GetElem<FromObj>::Type	FromElem;
	typedef typename Traits::GetEval<FromElem>::Type	ToEval;
	typedef IElementExpr<Expr<ToEval> > Super;
	typedef ElementExprAdaptor<FromObj,ToEval> Self;
	typedef Expr<ToEval> Elem;
	ElementExprAdaptor(const FromObj& e) : it(e) {}
	ElementExprAdaptor(const ElementExprAdaptor& s) : it(s.it) {}

	Elem 		get(uint i) const 	{	return it.get(i);  }
	ElementExpr<Elem>		copy() const
	{
		return static_cast<Super*>(new Self(*this));
	}

	ElementView<FromObj,FromElem>	it;
};

template<class T,class Eval>
struct Create<T,ElementExpr<Expr<Eval> > >
{
	ElementExpr<Expr<Eval> > operator()(const T& t)
	{	return static_cast<IElementExpr<Expr<Eval> >*>(new ElementExprAdaptor<T,Eval>(t));	}
};

} // Detail


template<class Eval>
struct ElementView<Expr<Seq<Eval> >,Expr<Eval> > : ElementExpr<Expr<Eval> >
{
	ElementView(const Expr<Seq<Eval> >& e) :
		ElementExpr<Expr<Eval> >(e.toElementExpr()) {}
};

template<class Eval>
struct ElementView<Expr<Seq<Eval> >,Expr<Seq<Eval> > > : ElementExpr<Expr<Seq<Eval> > >
{
	ElementView(const Expr<Seq<Eval> >& e) :
		ElementExpr<Expr<Seq<Eval> > >(e.toElementSeqExpr()) {}
};

}

#endif /*CASPER_KERNEL_OBJ_ELEMENT_H_*/
