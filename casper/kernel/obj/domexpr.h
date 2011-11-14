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
 

#ifndef CASPER_KERNEL_OBJ_DOMEXPR_H_
#define CASPER_KERNEL_OBJ_DOMEXPR_H_

#include <casper/kernel/obj/expr.h>

namespace Casper {
namespace Detail {

template<class T,class Eval>
struct Create<T,CP::DomExpr<Eval> >
{
	CP::DomExpr<Eval> operator()(CP::Store& store, const T& t)
	{	return CP::DomExpr<Eval>(store,t);	}
};

template<class Eval>
struct Create<Goal,CP::DomExpr<Eval> >
{
	CP::DomExpr<Eval> operator()(CP::Store& store, const Goal& t)
	{	throw Casper::Exception::TypeCoercion("Goal",typeid(CP::DomExpr<Eval>).name()); }
};

} // Detail

namespace CP {

template<class Eval>
struct DomView<Eval,Expr<Eval> > : DomExpr<Eval>
{
	DomView(Store& store, const Expr<Eval>& e) :
		DomExpr<Eval>(e.toDomExpr(store)),expr(e) {}
	const Expr<Eval>& getObj() const {	return expr; }
	Expr<Eval> expr;
};

} // CP

}

#endif /* CASPER_KERNEL_OBJ_DOMEXPR_H_ */
