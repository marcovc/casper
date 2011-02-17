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

/** \defgroup Expressions Expressions
 * 	\ingroup Kernel
 * 	Different kind of expressions may be associated with a given relation.
 */

#ifndef _H_CASPER_KERNEL_EXPRESSION
#define _H_CASPER_KERNEL_EXPRESSION

#include <casper/kernel/view/common.h>
#include <casper/kernel/io.h>

namespace minicasper {
struct MExpr;
};

namespace casper {

template<class T>
struct EvalTraits<ValExpr<T> >
{	typedef	T	Eval;	};

template<class T>
struct EvalTraits<BndExpr<T> >
{	typedef	T	Eval;	};

template<class T,class D>
struct EvalTraits<DomExpr<T,D> >
{	typedef	T	Eval;	};

template<class T>
struct EvalTraits<MutExpr<T> >
{	typedef	T	Eval;	};


/********************
 *  Bounds Expression
 *********************/

/**
 * A (contracting) bounded expression. Allows access/update of bounds.
 *
 * \note This class is just a reference to an instance of IBndExpr class.
 *
 * \ingroup Expressions
 */
template<class Eval>
struct BndExpr : detail::PImplIdiom<IBndExpr<Eval> >
{
	typedef BndExpr<Eval>	Self;
	typedef detail::PImplIdiom<IBndExpr<Eval> > Super;

	/// Creates a bounded expression which references an existing implementation.
	BndExpr(IBndExpr<Eval>* pImpl) : Super(pImpl) {}

	/// Creates a new bounded expression from a generic type \p T1.
	template<class T1>
	BndExpr(ICPSolver& solver, const T1& t);

	BndExpr(ICPSolver&, const BndExpr<Eval>& t) : Super(t) {}

//#ifdef CM_IFACE
	/// Creates a new bounded expression from a mini expression \p e
	BndExpr(minicasper::MExpr e);
//#endif

	/// Replaces the referenced bounded expression with a new expression.
	const BndExpr<Eval>& operator=(BndExpr<Eval> s)
	{
		((Super&)*this) = s;
		return *this;
	}

	/// Replaces the referenced bounded expression with a new expression.
	template<class T1>
	const BndExpr<Eval>& operator=(const T1& s)
	{
		((Super&)*this) = Self(solver(),s);
		return *this;
	}

	/// Returns the minimum of the bounded expression.
	Eval min() const { return Super::pImpl->min(); }
	/// Returns the maximum of the bounded expression.
	Eval max() const { return Super::pImpl->max(); }
	/**
	 * Assigns the minimum and maximum of the bounded expression to \p min and,
	 * \p max respectively.
	 **/
	Void range(Eval& min, Eval& max) const
	{ 	Super::pImpl->range(min,max); }
	/// Updates the minimum of the bounded expression
	Bool updateMin(const Eval& v) { return Super::pImpl->updateMin(v); }
	/// Updates the maximum of the bounded expression
	Bool updateMax(const Eval& v) { return Super::pImpl->updateMax(v); }
	/// Updates the minimum and maximum of the bounded expression
	Bool updateRange(const Eval& min,const Eval& max)
	{ return Super::pImpl->updateRange(min,max); }
	/// Returns the ICPSolver& object associated with the bounded expression.
	ICPSolver&	solver() const { return Super::pImpl->solver(); }

	/// Schedules \p f to be notified as soon as the bounded expression changes.
	void attach(INotifiable* f) { Super::pImpl->attach(f); }
	/// Stops \p f to be notified as soon as the bounded expression changes.
	void detach(INotifiable* f) { Super::pImpl->detach(f); }

};

/// Creates a new bounded expression from a generic type \p T1.
template<class Eval>
template<class T1>
BndExpr<Eval>::BndExpr(ICPSolver& solver, const T1& t) :
	Super(new (solver.heap())
			detail::BndExprWrapper<Eval,T1>(solver,t)) {}

template<class T>
struct GetCPSolver<BndExpr<T> >
{	ICPSolver& operator()(BndExpr<T> s)
	{ return s.solver(); }
};

template<class T>
std::ostream& operator<<(std::ostream& os, const casper::BndExpr<T>& f)
{	return casper::detail::RangePrinter<T,casper::BndExpr<T> >()(os,f);	};

/********************
 * Value Expression
 *********************/


/**
 *	An expression converging to a value of type \a T. The expression may be
 *  evaluated only when ground.
 *
 *  \note This class is just a reference to an instance of IValExpr class.
 *
 *  \ingroup Expressions
 */
template<class Eval>
struct ValExpr : detail::PImplIdiom<IValExpr<Eval> >
{
	/// Expression type.
	typedef ValExpr<Eval>	Self;

	/// Parent type.
	typedef detail::PImplIdiom<IValExpr<Eval> > Super;

	/// Creates a value expression which references an existing implementation.
	ValExpr(IValExpr<Eval>* pImpl) : Super(pImpl) {}

	/// Creates a new value expression from a generic object.
	template<class T1>
	ValExpr(ICPSolver& solver, const T1& t) :
		Super(new (solver.heap()) detail::ValExprWrapper<Eval,T1>(solver,t)) {}

	ValExpr(ICPSolver& solver, const ValExpr<Eval>& t) : Super(t) {}

	/// Returns the evaluation of the expression. \pre Expression is ground.
	Eval value() const { return Super::pImpl->value(); }

	/// \return \p True if expression is ground, \p False otherwise.
	Bool ground() const { return Super::pImpl->ground(); }

	/// Schedules \p f to be notified as soon as the expression becomes ground.
	void attach(INotifiable* f) { Super::pImpl->attach(f); }

	/// Stops \p f to be notified as soon as the expression becomes ground.
	void detach(INotifiable* f) { Super::pImpl->detach(f); }


	/// Returns the ICPSolver& object associated with the expression.
	ICPSolver&	solver() const { return Super::pImpl->solver(); }
};

template<class T>
std::ostream& operator<<(std::ostream& os, const casper::ValExpr<T>& f)
{
	os << f.value();
	return os;
};

template<class T>
struct GetCPSolver<ValExpr<T> >
{	ICPSolver& operator()(ValExpr<T> s)
	{ return s.solver(); }
};

/********************
 * Domain Expression
 ********************/

template<class T>
struct DefaultDomType;

/**
 * 	Allows access to every possible value of a given expression, if necessary
 *  by introducing an auxilliary domain variable (\sa DomVar).
 *
 *  \note This class is just a reference to an instance of IDomExpr class.
 *
 * \ingroup Expressions
 */
template<class Eval,class DomT = typename DefaultDomType<Eval>::Type>
struct DomExpr : detail::PImplIdiom<IDomExpr<DomT> >
{
	/// Domain type.
	typedef	DomT	Dom;

	/// Expression type.
	typedef DomExpr<Eval,Dom>	Self;

	/// Parent type.
	typedef detail::PImplIdiom<IDomExpr<Dom> > Super;

	/// Creates a domain expression which references an existing implementation.
	DomExpr(IDomExpr<DomT>* pImpl) : Super(pImpl) {}

	//#ifdef CM_IFACE
		/// Creates a new bounded expression from a mini expression \p e
		DomExpr(minicasper::MExpr e);
	//#endif

	/// Constructor from a generic object.
	template<class T1>
	DomExpr(ICPSolver& solver, const T1& t) :
		Super(new (solver.heap()) detail::DomExprWrapper<Eval,T1,Dom>(solver,t)) {}

	DomExpr(ICPSolver& solver, const DomExpr<Eval,Dom>& t) : Super(t) {}

	/// Replaces the referenced domain expression with a new expression.
	const DomExpr<Eval,Dom>& operator=(const DomExpr<Eval,Dom>& s)
	{
		((Super&)*this) = s;
		return *this;
	}

	/// Replaces the referenced domain expression with a new expression.
	template<class T1>
	const DomExpr<Eval,Dom>& operator=(const T1& s)
	{
		((Super&)*this) = Self(solver(),s);
		return *this;
	}

	///	Returns a pointer to the domain of the expression.
	Dom*		operator->()		{	return Super::pImpl->operator->();	}
	///	Returns a pointer to the domain of the expression.
	Dom*		operator->() const	{	return Super::pImpl->operator->();	}
	///	Returns a reference to the domain of the expression.
	Dom&		operator*()			{	return Super::pImpl->operator*();	}
	///	Returns a reference to the domain of the expression.
	Dom&	operator*() const		{	return Super::pImpl->operator*();	}

	/// Returns the ICPSolver& object associated with this expression.
	ICPSolver&	solver() const { return Super::pImpl->solver(); }
};

template<class Eval,class Dom>
std::ostream& operator<<(std::ostream& os, const casper::DomExpr<Eval,Dom>& f)
{
	os << *f;
	return os;
};

template<class> struct DomTraits;

template<class Eval,class DomT>
struct DomTraits<DomExpr<Eval,DomT> >
{	typedef DomT	Dom;	};

template<class E,class D>
struct GetCPSolver<DomExpr<E,D> >
{	ICPSolver& operator()(DomExpr<E,D> s)
	{ return s.solver(); }
};


/********************
 * Mutable Expression
 *********************/

/**
 *	A mutable expression is a readonly expression which
 *  evaluation may change over time.
 *
 *  \ingroup Expressions
 */
template<class Eval>
struct MutExpr : detail::PImplIdiom<IMutExpr<Eval> >
{
	/// Expression type.
	typedef MutExpr<Eval>	Self;

	/// Parent type.
	typedef detail::PImplIdiom<IMutExpr<Eval> > Super;

	/// Constructor from a generic object.
	template<class T1>
	MutExpr(ICPSolver& solver, const T1& t) :
		Super(new (solver.heap()) MutView<Eval,T1>(solver,t)) {}

	/// Constructor from an unary relation.
	template<class Func,class T1>
	MutExpr(ICPSolver& solver, const Rel1<Func,T1>& r) :
		Super(new (solver.heap())
			MutViewRel1<Func,T1,Eval>(solver,r.p1)) {}

	/// Constructor from a binary relation.
	template<class Func,class T1,class T2>
	MutExpr(ICPSolver& solver, const Rel2<Func,T1,T2>& r) :
		Super(new (solver.heap())
			MutViewRel2<Func,T1,T2,Eval>(solver,r.p1,r.p2)) {}

	MutExpr(ICPSolver& solver, const MutExpr<Eval>& t) : Super(t) {}

	MutExpr(IMutExpr<Eval>* m) : Super(m)	{}

	/// Returns the current evaluation of the expression.
	Eval value() const { return Super::pImpl->value(); }

	/// Schedules notifiable \a f to be notified when expression changes.
	void attach(INotifiable* f) { Super::pImpl->attach(f); }

	/// Stops notifiable \a f to be notified when expression changes.
	void detach(INotifiable* f) { Super::pImpl->detach(f); }

	/// Return ICPSolver& object associated with the expression.
	ICPSolver&	solver() const { return Super::pImpl->solver(); }
};

template<class T>
struct GetCPSolver<MutExpr<T> >
{	ICPSolver& operator()(MutExpr<T> s)
	{ return s.solver(); }
};

};


#endif

namespace casper {
#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_KERNEL_MACRO_EXPRESSION_H
#define CASPER_KERNEL_MACRO_EXPRESSION_H
#include <casper/kernel/macro/expression.h>
#endif
#endif
}
