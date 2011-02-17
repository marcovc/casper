 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_ORDERING
#define _H_CASPER_KERNEL_ORDERING

namespace casper {

template<class T>
struct IOrdering
{
	virtual ~IOrdering() {}
	virtual MutExpr<T> getEval() const  = 0;
};

template<class T>
struct OrderInc : public IOrdering<T>
{
	template<class P>
	OrderInc(ICPSolver& solver, const P& e) : e(solver,e) {}
	MutExpr<T> getEval() const {	return e;	}
	MutExpr<T>	e;
};

template<class T>
struct OrderDec : public IOrdering<T>
{
	template<class P>
	OrderDec(ICPSolver& solver, const P& e) : e(solver,e) {}
	MutExpr<T> getEval() const {	return e;	}
	MutExpr<T>	e;
};

template<class T>
OrderInc<typename EvalTraits<T>::Eval> orderInc(const T& e)
{
	return OrderInc<typename EvalTraits<T>::Eval>(getCPSolver(e),e);
}

template<class T>
OrderDec<typename EvalTraits<T>::Eval> orderDec(const T& e)
{
	return OrderDec<typename EvalTraits<T>::Eval>(getCPSolver(e),e);
}

};

#endif /*_H_CASPER_KERNEL_ORDERING*/
