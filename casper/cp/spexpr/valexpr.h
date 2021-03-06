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
 

#ifndef CASPER_KERNEL_OBJ_VALEXPR_H_
#define CASPER_KERNEL_OBJ_VALEXPR_H_

#include <casper/kernel/spexpr/expr.h>

namespace Casper {
namespace Detail {


template<class T,class Eval>
struct Create<T,CP::ValExpr<Eval> >
{
	CP::ValExpr<Eval> operator()(CP::Store& store, const T& t);
};

template<class T,class Eval>
CP::ValExpr<Eval> Create<T,CP::ValExpr<Eval> >::operator()(CP::Store& store, const T& t)
{	return CP::ValExpr<Eval>(store,t);	}

//template<class Eval,class T>
//CP::ValExpr<Eval> ExprWrapper<Eval,T>::toCPValExpr(CP::Store& store) const
//{	return Create<T,CP::ValExpr<Eval> >()(store,t);	}
//
//template<class T>
//CP::ValExpr<bool> ExprWrapper<bool,T>::toCPValExpr(CP::Store& store) const
//{	return Create<T,CP::ValExpr<bool> >()(store,t);	}

} // Detail

namespace CP {

template<class Eval>
struct ValView<Eval,Expr<Eval> > : ValExpr<Eval>
{
	ValView(Store& store, const Expr<Eval>& e) :
		ValExpr<Eval>(e.toCPValExpr(store)),expr(e) {}
	const Expr<Eval>& getObj() const {	return expr; }
	Expr<Eval> expr;
};

} // CP

}

#endif /* CASPER_KERNEL_OBJ_VALEXPR_H_ */
