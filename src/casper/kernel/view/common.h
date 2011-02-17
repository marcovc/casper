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

#ifndef _H_CASPER_KERNEL_VIEW_COMMON
#define _H_CASPER_KERNEL_VIEW_COMMON

#include <casper/kernel/notifiable.h>
#include <casper/kernel/notifier.h>
#include <casper/kernel/solver.h>
//#include <casper/kernel/susplist.h>
//#include <casper/kernel/relation.h>
//#include <casper/kernel/memory.h>

namespace casper {

/** \defgroup Views Views
 * 	\ingroup Kernel
 * 	A View imposes a specific interface to generic objects, usually
 *  relations.
 */

/**
 *	Interface to an expression converging to
 *  a value of type \a T. The expression may be evaluated only when ground.
 *
 *  \ingroup Expressions
 */
template<class T>
struct IValExpr : INotifier
{
	/// Returns the evaluation of the expression. \pre Expression is ground.
	virtual T 		value() const = 0;
	/// Instantiates the expression to value \p value and propagates.
	virtual Bool	setValue(const T& value) = 0;
	/// \return \p True if expression is ground, \p False otherwise.
	virtual Bool 	ground() const = 0;
	/// Returns the ICPSolver& object associated with the expression.
	ICPSolver&		solver() const {	return mSolver; }

	///	Creates a new IValExpr.
	IValExpr(ICPSolver& solver) : mSolver(solver) {}
	bool notify() { assert(0); return true; }
	ICPSolver&				mSolver;
};

/**
 * Interface to a (contracting) bounded expression. Allows access/update of bounds.
 *
 * \ingroup Expressions
 */
template<class T>
struct IBndExpr : INotifier
{
	/// Returns the minimum of the bounded expression.
	virtual T min() const = 0;
	/// Returns the maximum of the bounded expression.
	virtual T max() const = 0;
	/// Returns the evaluation of the bounded expression. \pre min()==max()
	T value() const {	assert(min()==max()); return min();	}
	/// Returns if min()==max()
	Bool ground() const {	return min()==max();	}

	/**
	 * Assigns the minimum and maximum of the bounded expression to \p min and,
	 * \p max respectively.
	 **/
	virtual Void range(T& lb, T& ub) const
	{	lb = min(); ub = max();	}
	/// Updates the minimum of the bounded expression
	virtual Bool updateMin(const T&) = 0;
	/// Updates the maximum of the bounded expression
	virtual Bool updateMax(const T&) = 0;
	/// Updates the minimum and maximum of the bounded expression
	virtual Bool updateRange(const T& min, const T& max)
	{	return updateMin(min) && updateMax(max);	}
	/// Returns the ICPSolver& object associated with the bounded expression.
	ICPSolver&		solver() const {	return mSolver; }

	///	Creates a new IBndExpr.
	IBndExpr(ICPSolver& solver) : mSolver(solver) {}
	bool notify() { assert(0); return true; }

	ICPSolver&			mSolver;
};

/**
 * 	Interface to a DomExpr. Allows access to every possible value of a given
 *  expression, if necessary by introducing an auxilliary domain variable
 *  (\sa DomVar).
 *
 * \ingroup Expressions
 */
template<class Dom>
struct IDomExpr : INotifier
{
	IDomExpr(ICPSolver& solver) : mSolver(solver) {}
	virtual ~IDomExpr()	{}

	///	Returns a pointer to the domain of the expression.
	virtual Dom*		operator->()		=	0;
	///	Returns a pointer to the domain of the expression.
	virtual Dom*		operator->() const	=	0;
	///	Returns a reference to the domain of the expression.
	virtual Dom&		operator*()			=	0;
	///	Returns a reference to the domain of the expression.
	virtual Dom&		operator*() const	=	0;

	// temporary -> the ArrayView mess depends on this.
	Void	attach(INotifiable*	n)	{	(*this)->attachOnDomain(n);	}
	Void	detach(INotifiable*	n)	{	(*this)->detachOnDomain(n);	}

	bool notify() { assert(0); return true; }
	/// Return ICPSolver& object associated with the expression.
	ICPSolver&		solver() const {	return mSolver; }

	ICPSolver&				mSolver;
};

/**
 *	Interface to a mutable expression. A mutable expression is a readonly
 *  expression which evaluation may change over time.
 *
 *  \ingroup Expressions
 */
template<class T>
struct IMutExpr : INotifier
{
	/// Returns the current evaluation of the expression.
	virtual T 		value() const = 0;
	/// Return ICPSolver& object associated with the expression.
	ICPSolver&		solver() const {	return mSolver; }

	/// Creates a new IMutExpr.
	IMutExpr(ICPSolver& solver) : mSolver(solver) {}
	bool notify() { assert(0); return true; }

	ICPSolver&				mSolver;
};


namespace detail {


// wrappers
template<class Eval,class View>
struct ValExprWrapper : IValExpr<Eval>
{
	ValExprWrapper(ICPSolver& solver,const View& v) :
		IValExpr<Eval>(solver),v(solver,v) {}

	Eval	value() const	{	return v.value();	}
	Bool	setValue(const Eval& value) { return v.setValue(value);	}
	Bool 	ground() const 	{	return v.ground();	}

	Void attach(INotifiable* f)
	{	v.attach(f);	}
	Void detach(INotifiable* f)
	{	v.detach(f);	}
	ValView<Eval,View>	v;
};

template<class Eval,class View>
struct BndExprWrapper : IBndExpr<Eval>
{
	BndExprWrapper(ICPSolver& solver,const View& v) :
		IBndExpr<Eval>(solver),v(solver,v) {}

	Eval min() const	{	return v.min();	}
	Eval max() const 	{	return v.max();	}
	Void range(Eval& lb, Eval& ub) const
	{	v.range(lb,ub);	}
	Bool updateMin(const Eval& t)
	{	return v.updateMin(t);	}
	Bool updateMax(const Eval& t)
	{	return v.updateMax(t);	}
	Bool updateRange(const Eval& min, const Eval& max)
	{	return v.updateRange(min,max);	}
	ICPSolver&		solver() const
	{	return v.solver(); }
	Void attach(INotifiable* f)
	{	v.attach(f);	}
	Void detach(INotifiable* f)
	{	v.detach(f);	}
	BndView<Eval,View>	v;
};


template<class Eval,class View,class Dom>
struct DomExprWrapper : IDomExpr<Dom>
{
	DomExprWrapper(ICPSolver& solver,const View& v) :
		IDomExpr<Dom>(solver),v(solver,v) {}

	Dom*		operator->()
	{	return v.operator->();	}
	Dom*		operator->() const
	{	return v.operator->();	}
	Dom&		operator*()
	{	return v.operator*();	}
	Dom&		operator*() const
	{	return v.operator*();	}

	Void attach(INotifiable* f)
	{	v.attach(f);	}
	Void detach(INotifiable* f)
	{	v.detach(f);	}
	DomView<Eval,View,Dom>	v;
};

}

template<class T>	struct ValExpr;
template<class T>	struct BndExpr;
template<class E,class D>	struct DomExpr;
template<class T>	struct MutExpr;

template<class,class>				struct ValView;
template<class,class,class>			struct ValViewRel1;
template<class,class,class,class>	struct ValViewRel2;

template<class,class>				struct BndView;
template<class,class,class>			struct BndViewRel1;
template<class,class,class,class>	struct BndViewRel2;

template<class,class,class>			struct DomView;
//template<class,class,class>			struct DomViewRel1;
//template<class,class,class,class>	struct DomViewRel2;

template<class,class>				struct MutView;
template<class,class,class>			struct MutViewRel1;
template<class,class,class,class>	struct MutViewRel2;

};

#endif // _H_CASPER_KERNEL_VIEW_COMMON
