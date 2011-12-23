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

#ifndef CASPER_KERNEL_OBJ_ITERATION_H_
#define CASPER_KERNEL_OBJ_ITERATION_H_

#include <casper/kernel/expr.h>
#include <casper/kernel/spexpr/expr.h>

namespace Casper {
namespace Detail {

template<class FromObj,class ToEval>
struct ExprIterationAdaptor : IIterationExpr<Expr<ToEval> >
{
	typedef IIterationExpr<Expr<ToEval> > Super;
	typedef ExprIterationAdaptor<FromObj,ToEval> Self;
	typedef Expr<ToEval> Elem;
	ExprIterationAdaptor(const FromObj& e) : it(e) {}
	ExprIterationAdaptor(const ExprIterationAdaptor& s) : it(s.it) {}

	void		iterate()			{	it.iterate(); }
	Elem 		value() const 	{	return Elem(it.value()); }
	bool		valid() const 		{	return it.valid(); }
	IterationExpr<Elem>		copy() const
	{
		return static_cast<Super*>(new Self(*this));
	}

	IterationView<FromObj>	it;
};

template<class T,class Eval>
struct Create<T,IterationExpr<Expr<Eval> > >
{
	IterationExpr<Expr<Eval> > operator()(const T& t)
	{	return static_cast<IIterationExpr<Expr<Eval> >*>(new ExprIterationAdaptor<T,Eval>(t));	}
};

} // Detail


template<class Eval>
struct IterationView<Expr<Seq<Eval> > > : IterationExpr<Expr<Eval> >
{
	IterationView(const Expr<Seq<Eval> >& e) :
		IterationExpr<Expr<Eval> >(e.toIterationExpr()) {}
};


}

#endif /*CASPER_KERNEL_OBJ_ITERATION_H_*/
