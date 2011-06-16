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

#ifndef _H_CASPER_CP_EXPRESSION
#define _H_CASPER_CP_EXPRESSION

#include <casper/cp/view/common.h>
#include <casper/cp/traits.h>
#include <casper/util/io.h>


namespace Casper {
namespace CP {


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
	virtual bool	setValue(const T& value) = 0;
	/// \return \p True if expression is ground, \p False otherwise.
	virtual bool 	ground() const = 0;
	/// Returns the Store object associated with the expression.
	Store&		getStore() const {	return store; }

	///	Creates a new IValExpr.
	IValExpr(Store& store) : store(store) {}
	bool notify() { assert(0); return true; }
	Store&				store;
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
	bool ground() const {	return min()==max();	}

	/**
	 * Assigns the minimum and maximum of the bounded expression to \p min and,
	 * \p max respectively.
	 **/
	virtual void range(T& lb, T& ub) const
	{	lb = min(); ub = max();	}
	/// Updates the minimum of the bounded expression
	virtual bool updateMin(const T&) = 0;
	/// Updates the maximum of the bounded expression
	virtual bool updateMax(const T&) = 0;
	/// Updates the minimum and maximum of the bounded expression
	virtual bool updateRange(const T& min, const T& max)
	{	return updateMin(min) && updateMax(max);	}
	/// Returns the Store object associated with the bounded expression.
	Store&		getStore() const {	return store; }

	///	Creates a new IBndExpr.
	IBndExpr(Store& store) : store(store) {}
	bool notify() { assert(0); return true; }

	Store&			store;
};

/**
 * 	Interface to a DomExpr. Allows access to every possible value of a given
 *  expression, if necessary by introducing an auxilliary domain variable
 *  (\sa Var).
 *
 * \ingroup Expressions
 */
template<class Dom>
struct IDomExpr : INotifier
{
	IDomExpr(Store& store) : store(store) {}
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
	void	attach(INotifiable*	n)	{	(*this)->attachOnDomain(n);	}
	void	detach(INotifiable*	n)	{	(*this)->detachOnDomain(n);	}

	bool notify() { assert(0); return true; }
	/// Return Store object associated with the expression.
	Store&		getStore() const {	return store; }

	Store&				store;
};

namespace Detail {


// wrappers
template<class Eval,class View>
struct ValExprWrapper : IValExpr<Eval>
{
	ValExprWrapper(Store& store,const View& v) :
		IValExpr<Eval>(store),v(store,v) {}

	Eval	value() const	{	return v.value();	}
	bool	setValue(const Eval& value) { return v.setValue(value);	}
	bool 	ground() const 	{	return v.ground();	}

	void attach(INotifiable* f)
	{	v.attach(f);	}
	void detach(INotifiable* f)
	{	v.detach(f);	}
	ValView<Eval,View>	v;
};

template<class Eval,class View>
struct BndExprWrapper : IBndExpr<Eval>
{
	BndExprWrapper(Store& store,const View& v) :
		IBndExpr<Eval>(store),v(store,v) {}

	Eval min() const	{	return v.min();	}
	Eval max() const 	{	return v.max();	}
	void range(Eval& lb, Eval& ub) const
	{	v.range(lb,ub);	}
	bool updateMin(const Eval& t)
	{	return v.updateMin(t);	}
	bool updateMax(const Eval& t)
	{	return v.updateMax(t);	}
	bool updateRange(const Eval& min, const Eval& max)
	{	return v.updateRange(min,max);	}
	void attach(INotifiable* f)
	{	v.attach(f);	}
	void detach(INotifiable* f)
	{	v.detach(f);	}
	BndView<Eval,View>	v;
};


template<class Eval,class View,class Dom>
struct DomExprWrapper : IDomExpr<Dom>
{
	DomExprWrapper(Store& store,const View& v) :
		IDomExpr<Dom>(store),v(store,v) {}

	Dom*		operator->()
	{	return v.operator->();	}
	Dom*		operator->() const
	{	return v.operator->();	}
	Dom&		operator*()
	{	return v.operator*();	}
	Dom&		operator*() const
	{	return v.operator*();	}

	void attach(INotifiable* f)
	{	v.attach(f);	}
	void detach(INotifiable* f)
	{	v.detach(f);	}
	DomView<Eval,View,Dom>	v;
};

} // Detail


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
struct BndExpr : Util::PImplIdiom<IBndExpr<Eval> >
{
	typedef BndExpr<Eval>	Self;
	typedef Util::PImplIdiom<IBndExpr<Eval> > Super;

	/// Creates a bounded expression which references an existing implementation.
	BndExpr(IBndExpr<Eval>* pImpl) : Super(pImpl) {}

	/// Creates a new bounded expression from a generic type \p T1.
	template<class T1>
	BndExpr(Store& store, const T1& t);

	BndExpr(Store&, const BndExpr<Eval>& t) : Super(t) {}

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
		((Super&)*this) = Self(getStore(),s);
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
	void range(Eval& min, Eval& max) const
	{ 	Super::pImpl->range(min,max); }
	/// Updates the minimum of the bounded expression
	bool updateMin(const Eval& v) { return Super::pImpl->updateMin(v); }
	/// Updates the maximum of the bounded expression
	bool updateMax(const Eval& v) { return Super::pImpl->updateMax(v); }
	/// Updates the minimum and maximum of the bounded expression
	bool updateRange(const Eval& min,const Eval& max)
	{ return Super::pImpl->updateRange(min,max); }
	/// Returns the Store object associated with the bounded expression.
	Store&	getStore() const { return Super::pImpl->getStore(); }

	/// Schedules \p f to be notified as soon as the bounded expression changes.
	void attach(INotifiable* f) { Super::pImpl->attach(f); }
	/// Stops \p f to be notified as soon as the bounded expression changes.
	void detach(INotifiable* f) { Super::pImpl->detach(f); }

};

/// Creates a new bounded expression from a generic type \p T1.
template<class Eval>
template<class T1>
BndExpr<Eval>::BndExpr(Store& store, const T1& t) :
	Super(new (store) Detail::BndExprWrapper<Eval,T1>(store,t)) {}


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
struct ValExpr : Util::PImplIdiom<IValExpr<Eval> >
{
	/// Expression type.
	typedef ValExpr<Eval>	Self;

	/// Parent type.
	typedef Util::PImplIdiom<IValExpr<Eval> > Super;

	/// Creates a value expression which references an existing implementation.
	ValExpr(IValExpr<Eval>* pImpl) : Super(pImpl) {}

	/// Creates a new value expression from a generic object.
	template<class T1>
	ValExpr(Store& store, const T1& t) :
		Super(new (store) Detail::ValExprWrapper<Eval,T1>(store,t)) {}

	ValExpr(Store& store, const ValExpr<Eval>& t) : Super(t) {}

	/// Returns the evaluation of the expression. \pre Expression is ground.
	Eval value() const { return Super::pImpl->value(); }

	/// Attempts to fix the value of the expression to \p v.
	/// \return \p False if the expression cannot be equal to \p v
	/// \note Calling this method does not guarantee that the expression will become ground after.
	bool setValue(const Eval& v) {	return Super::pImpl->setValue(v);	}

	/// \return \p True if expression is ground, \p False otherwise.
	bool ground() const { return Super::pImpl->ground(); }

	/// Schedules \p f to be notified as soon as the expression becomes ground.
	void attach(INotifiable* f) { Super::pImpl->attach(f); }

	/// Stops \p f to be notified as soon as the expression becomes ground.
	void detach(INotifiable* f) { Super::pImpl->detach(f); }


	/// Returns the Store object associated with the expression.
	Store&	getStore() const { return Super::pImpl->getStore(); }
};


/********************
 * Domain Expression
 ********************/

namespace Traits {
template<class T>
struct GetDefaultDom;
} // Traits

/**
 * 	Allows access to every possible value of a given expression, if necessary
 *  by introducing an auxilliary domain variable (\sa Var).
 *
 *  \note This class is just a reference to an instance of IDomExpr class.
 *
 * \ingroup Expressions
 */
template<class Eval,class DomT = typename Traits::GetDefaultDom<Eval>::Type>
struct DomExpr : Util::PImplIdiom<IDomExpr<DomT> >
{
	/// Domain type.
	typedef	DomT	Dom;

	/// Expression type.
	typedef DomExpr<Eval,Dom>	Self;

	/// Parent type.
	typedef Util::PImplIdiom<IDomExpr<Dom> > Super;

	/// Creates a domain expression which references an existing implementation.
	DomExpr(IDomExpr<DomT>* pImpl) : Super(pImpl) {}

	/// Constructor from a generic object.
	template<class T1>
	DomExpr(Store& store, const T1& t) :
		Super(new (store) Detail::DomExprWrapper<Eval,T1,Dom>(store,t)) {}

	DomExpr(Store& store, const DomExpr<Eval,Dom>& t) : Super(t) {}

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
		((Super&)*this) = Self(getStore(),s);
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

	/// Returns the Store object associated with this expression.
	Store&	getStore() const { return Super::pImpl->getStore(); }
};

namespace Traits {
template<class Eval,class DomT>
struct GetDom<DomExpr<Eval,DomT> >
{	typedef DomT	Type;
};
} // Traits

typedef BndExpr<int>	IntBndExpr;
typedef BndExpr<bool>	BoolBndExpr;
typedef BndExpr<double>	DoubleBndExpr;
typedef BndExpr<float>	FloatBndExpr;

typedef ValExpr<int>	IntValExpr;
typedef ValExpr<bool>	BoolValExpr;


} // CP

template<class> struct GetPEnv;
template<class T>
struct GetPEnv<CP::BndExpr<T> >
{	Env* operator()(CP::BndExpr<T> s)
	{ return &s.getStore().getEnv(); }
};

template<class T>
struct GetPEnv<CP::ValExpr<T> >
{	Env* operator()(CP::ValExpr<T> s)
	{ return &s.getStore().getEnv(); }
};

template<class E,class D>
struct GetPEnv<CP::DomExpr<E,D> >
{	Env* operator()(CP::DomExpr<E,D> s)
	{ return &s.getStore().getEnv();}
};

namespace Traits {

template<class T>
struct GetEval<CP::ValExpr<T> >
{	typedef T	Type;	};

template<class T>
struct GetEval<CP::BndExpr<T> >
{	typedef T	Type;	};

template<class T,class D>
struct GetEval<CP::DomExpr<T,D> >
{	typedef T	Type;	};

}

} // Casper

template<class T>
std::ostream& operator<<(std::ostream& os, const Casper::CP::BndExpr<T>& f)
{	return Casper::Util::RangePrinter<T,Casper::CP::BndExpr<T> >()(os,f);	};

template<class T>
std::ostream& operator<<(std::ostream& os, const Casper::CP::ValExpr<T>& f)
{
	os << f.value();
	return os;
};


template<class Eval,class Dom>
std::ostream& operator<<(std::ostream& os, const Casper::CP::DomExpr<Eval,Dom>& f)
{
	os << *f;
	return os;
};

#endif

#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_CP_REL_OP_EXPR_H
#define CASPER_CP_REL_OP_EXPR_H
#include <casper/cp/rel/op_expr.h>
#endif
#endif