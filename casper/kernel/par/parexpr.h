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
 

#ifndef CASPER_KERNEL_PAR_PAREXPR_H_
#define CASPER_KERNEL_PAR_PAREXPR_H_

#include <casper/kernel/notify/notifier.h>
#include <casper/util/pimpl.h>
#include <casper/kernel/state/env.h>
#include <casper/kernel/rel/rel.h>

namespace Casper {

/********************
 * Expr
 *********************/

template<class,class> struct ConstParView;
template<class,class,class> struct ConstParView1;
template<class,class,class,class> struct ConstParView2;

/**
 *	Interface to a delayed expression.
 *
 *  \ingroup Expressions
 */
template<class T>
struct IConstPar : INotifier
{
	/// Returns the current evaluation of the expression.
	virtual T 		value() const = 0;
	/// Return CPSolver& object associated with the expression.
	Env&		getEnv() const {	return env; }

	/// Creates a new IMutExpr.
	IConstPar(Env& env) : env(env) {}
	bool notify() { assert(0); return true; }

	Env&				env;
};


/**
 * A delayed expression.
 *  \ingroup Expressions
 */
template<class Eval>
struct ParExpr : Util::PImplIdiom<IConstPar<Eval> >
{
	/// Expression type.
	typedef ParExpr<Eval>	Self;

	/// Parent type.
	typedef Util::PImplIdiom<IConstPar<Eval> > Super;

	/// Constructor from a generic object.
	template<class T1>
	ParExpr(Env& env, const T1& t) :
		Super(new (env) ConstParView<Eval,T1>(env,t)) {}

	/// Constructor from an unary relation.
	template<class Func,class T1>
	ParExpr(Env& env, const Rel1<Func,T1>& r) :
		Super(new (env) ConstParView1<Func,T1,Eval>(env,r.p1)) {}

	/// Constructor from a binary relation.
	template<class Func,class T1,class T2>
	ParExpr(Env& env, const Rel2<Func,T1,T2>& r) :
		Super(new (env) ConstParView2<Func,T1,T2,Eval>(env,r.p1,r.p2)) {}

	ParExpr(Env& env, const ParExpr<Eval>& t) : Super(t) {}

	ParExpr(IConstPar<Eval>* m) : Super(m)	{}

	/// Returns the current evaluation of the expression.
	Eval value() const { return Super::pImpl->value(); }

	/// Schedules notifiable \a f to be notified when expression changes.
	void attach(INotifiable* f) { Super::pImpl->attach(f); }

	/// Stops notifiable \a f to be notified when expression changes.
	void detach(INotifiable* f) { Super::pImpl->detach(f); }

	/// Return env object associated with the expression.
	Env&	getEnv() const { return Super::pImpl->getEnv(); }
};

namespace Traits {
template<class T>
struct GetEval<ParExpr<T> >
{	typedef T	Type;	};
} // Traits

template<class T>
struct GetPEnv<ParExpr<T> >
{	Env* operator()(ParExpr<T> s)
	{ return &s.getEnv(); }
};

typedef ParExpr<int>	IntParExpr;
typedef ParExpr<bool>	BoolParExpr;
typedef ParExpr<double>	DoubleParExpr;

} // Casper


template<class T>
std::ostream& operator<<(std::ostream& os, const Casper::ParExpr<T>& v)
{
	os << v.value();
	return os;
}

#endif /* CASPER_KERNEL_PAR_EXPR_H_ */

#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_KERNEL_PAR_OP_PAREXPR_H
#define CASPER_KERNEL_PAR_OP_PAREXPR_H
#include <casper/kernel/par/op_parexpr.h>
#endif
#endif
