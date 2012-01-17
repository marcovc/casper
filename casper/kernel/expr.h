 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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


#ifndef _H_CASPER_KERNEL_EXPRESSION
#define _H_CASPER_KERNEL_EXPRESSION

#include <casper/kernel/view/iteration.h>
#include <casper/kernel/view/element.h>
#include <casper/kernel/traits.h>


namespace Casper {

template<class Elem> struct IterationExpr;
template<class> struct IterationView;

template<class Elem> struct ElementExpr;
template<class,class> struct ElementView;

namespace Detail {

template<class Elem>
struct IIterationExpr
{
	/// Iterates the current pointer to the expression
	virtual void		iterate()	= 0;
	/// Dereferences the current pointer
	virtual Elem 		value() const 	= 0;
	/// Returns if the current pointer is still valid
	virtual bool		valid() const 	= 0;
	/// Returns a copy of *this
	virtual IterationExpr<Elem>		copy() const = 0;
};

// wrappers
template<class Elem,class View>
struct IterationExprWrapper : IIterationExpr<Elem>
{
	IterationExprWrapper(const View& v) :
		v(v) {}

	void		iterate()	{	v.iterate(); }
	Elem	 	value() const 	{	return v.value(); }
	bool		valid() const 	{	return v.valid(); }
	IterationExpr<Elem>		copy() const
	{	return static_cast<IIterationExpr<Elem>*>(new IterationView<View>(v)); }

	IterationView<View>	v;
};

template<class Elem>
struct IElementExpr
{
	/// Returns the ith element
	virtual Elem 		get(uint i) const 	= 0;
	/// Returns a copy of *this
	virtual ElementExpr<Elem>		copy() const = 0;
};

// wrappers
template<class Elem,class View>
struct ElementExprWrapper : IElementExpr<Elem>
{
	ElementExprWrapper(const View& v) :
		v(v) {}

	Elem	 	get(uint i) const 	{	return v.get(i); }
	ElementExpr<Elem>		copy() const
	{	return static_cast<IElementExpr<Elem>*>(new ElementView<View,Elem>(v)); }

	ElementView<View,Elem>	v;
};


} // Detail


/********************
 *  Iteration Expression
 *********************/

/**
 * An IterationExrpression provides a way to iterate through a generic iterable
 * expression.
 *
 * \ingroup Views
 */
template<class Elem>
struct IterationExpr : Util::SPImplIdiom<Detail::IIterationExpr<Elem> >
{
	typedef IterationExpr<Elem>	Self;
	typedef Util::SPImplIdiom<Detail::IIterationExpr<Elem> > Super;

	/// Creates an iteration expression which references an existing implementation.
	IterationExpr(Detail::IIterationExpr<Elem>* pImpl) : Super(pImpl) {}

	/// Creates a new iteration expression from a generic type \p T1.
	template<class T1>
	IterationExpr(const T1& t);

	/// Copy constructor. \note The IterationView protocol does not allow shallow copies.
	IterationExpr(const IterationExpr<Elem>& t) : Super(t.copy()) {}

	/// Replaces the referenced bounded expression with a new expression.
	const IterationExpr<Elem>& operator=(IterationExpr<Elem> s)
	{
		((Super&)*this) = s;
		return *this;
	}

	/// Iterates the current pointer to the expression
	void		iterate()	{	Super::getImpl().iterate(); }
	/// Dereferences the current pointer
	Elem 		value() const 	{	return Super::getImpl().value(); }
	/// Returns if the current pointer is still valid
	bool		valid() const 	{	return Super::getImpl().valid(); }
	/// Returns a copy of *this
	Self		copy() const {	return Super::getImpl().copy(); }

};

/// Creates a new iteration expression from a generic type \p T1.
template<class Elem>
template<class T1>
IterationExpr<Elem>::IterationExpr(const T1& t) :
	Super(new Detail::IterationExprWrapper<Elem,T1>(t)) {}



/********************
 *  Element Expression
 *********************/

/**
 * An ElementExrpression provides a way to access an element of a generic indexable
 * expression.
 *
 * \ingroup Views
 */
template<class Elem>
struct ElementExpr : Util::SPImplIdiom<Detail::IElementExpr<Elem> >
{
	typedef ElementExpr<Elem>	Self;
	typedef Util::SPImplIdiom<Detail::IElementExpr<Elem> > Super;

	/// Creates an element expression which references an existing implementation.
	ElementExpr(Detail::IElementExpr<Elem>* pImpl) : Super(pImpl) {}

	/// Creates a new element expression from a generic type \p T1.
	template<class T1>
	ElementExpr(const T1& t);

	/// Copy constructor. \note The ElementView protocol does not allow shallow copies.
	ElementExpr(const ElementExpr<Elem>& t) : Super(t.copy()) {}

	/// Replaces the referenced bounded expression with a new expression.
	const ElementExpr<Elem>& operator=(ElementExpr<Elem> s)
	{
		((Super&)*this) = s;
		return *this;
	}

	/// Returns element at position i
	Elem 		get(uint i) const 	{	return Super::getImpl().get(i); }
	/// Returns a copy of *this
	Self		copy() const {	return Super::getImpl().copy(); }

};

/// Creates a new iteration expression from a generic type \p T1.
template<class Elem>
template<class T1>
ElementExpr<Elem>::ElementExpr(const T1& t) :
	Super(new Detail::ElementExprWrapper<Elem,T1>(t)) {}


namespace Traits {

template<class Elem>
struct GetEval<ElementExpr<Elem> >
{	typedef typename GetEval<Elem>::Type	Type;	};

}

} // Casper

template<class T>
std::ostream& operator<<(std::ostream& os, const Casper::ElementExpr<T>& f)
{
	os << "[ElementExpr]";
	return os;
};

#endif

