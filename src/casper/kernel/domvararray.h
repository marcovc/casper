 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

/**
 * 	\file
 *	Multidimensional array of variables.
 */

#ifndef _H_CASPER_KERNEL_DOMVARARRAY
#define _H_CASPER_KERNEL_DOMVARARRAY

#include <casper/kernel/container/array.h>
#include <casper/kernel/variable.h>
#include <casper/kernel/relation.h>
#include <casper/kernel/expression.h>
#include <casper/kernel/view/iteration.h>

using namespace std;

namespace casper {

template<class,int,class>
struct DomVarArray;

namespace detail {
template<class T,int Dims,class Dom>
struct DomVarArrayTraits
{
	typedef DomVarArray<T,Dims,Dom>	Derived;
	typedef DomVar<T,Dom>			Term;
	template<int dims2>
	struct Rebind {
		typedef DomVarArray<T,dims2,Dom> Type;
	};
	static const int dims = Dims;
};
};

/**	Multidimensional array of domain variables.
 *  \param T	The type of element in each domain.
 *  \param dims	The number of dimensions of the array.
 *  \param Dom	The type of the domain.
 *  \ingroup Kernel
 */
template<class T, int dims = 1, class Dom1 = typename DefaultDomType<T>::Type>
struct DomVarArray :
	casper::Array<DomVar<T,Dom1>,dims>
	//casper::detail::MultiDim<casper::detail::DomVarArrayTraits<T,dims,Dom1> >
{
	typedef Dom1	Dom;

	private:
	typedef DomVarArray<T,dims,Dom>					Self;
//	typedef detail::DomVarArrayTraits<T,dims,Dom> 	Traits;


	public:
	typedef casper::Array<DomVar<T,Dom1>,dims>		Super;
	//typedef detail::MultiDim<Traits>				Super;

	/// Type of element stored in the array.
	typedef  typename Super::Elem		Elem;

	/// Element iterator.
	typedef typename Super::Iterator	Iterator;

	/// Creates a new DomVarArray pointing to an existing DomVarArray \p s.
	DomVarArray(const DomVarArray& s) : Super(s),mSolver(s.solver()) { }

	/// Creates a new DomVarArray pointing to an existing DomVarArray \p s.
	DomVarArray(ICPSolver& solver, const Super& s) : Super(s),mSolver(solver) {}

	/** Creates a new DomVarArray with a copy of variables in the given
	 	range [\p b, \p e[.	*/
	DomVarArray(ICPSolver& solver, Iterator b, Iterator e) :
			Super(solver.heap(),b,e),
			mSolver(solver) {}

	/// Creates a new DomVarArray with \p p1 variables.
	DomVarArray(ICPSolver& solver,UInt p1) : Super(solver.heap(),p1),mSolver(solver)	{}

	/** Creates a new DomVarArray with \p p1 variables, and initializes each
		domain with the remaining parameter.	*/
	template<class T2>
	DomVarArray(ICPSolver& solver,UInt p1,const T2& p2) :
		Super(detail::ArrayDataTraits<Self,dims>::initData(solver.heap(),p1,p2,solver)),
		mSolver(solver)	{}

	/** Creates a new DomVarArray with \p p1 variables, and initializes each
		domain with the remaining parameters.	*/
	template<class T2,class T3>
	DomVarArray(ICPSolver& solver,UInt p1,const T2& p2,const T3& p3) :
		Super(detail::ArrayDataTraits<Self,dims>::initData(solver.heap(),p1,p2,p3,solver)),
		mSolver(solver)	{}

	/** Creates a new DomVarArray with \p p1 variables, and initializes each
		domain with the remaining parameters.	*/
	template<class T2,class T3,class T4>
	DomVarArray(ICPSolver& solver,UInt p1,const T2& p2,const T3& p3,const T4& p4) :
		Super(detail::ArrayDataTraits<Self,dims>::initData(solver.heap(),p1,p2,p3,p4,solver)),
		mSolver(solver)	{}

	/** Creates a new DomVarArray with \p p1 variables, and initializes each
		domain with the remaining parameters.	*/
	template<class T2,class T3,class T4,class T5>
	DomVarArray(ICPSolver& solver,UInt p1,const T2& p2,const T3& p3,const T4& p4,const T5& p5) :
		Super(detail::ArrayDataTraits<Self,dims>::initData(solver.heap(),p1,p2,p3,p4,p5,solver)),
		mSolver(solver)	{}

	/// Returns the ICPSolver& object associated with this array.
	ICPSolver& solver() const {	return mSolver;	}

	// FIXME: dangerous, T1 is too general...
	/** Returns an Element relation involving the array, the index value
		\a t, and the corresponding variable.	*/
/*	template<class T1>
	Rel2<Element,Self,T1>	operator[](const T1& t) const
	{	return	Rel2<Element,Self,T1>(*this,t);	}

	///	Returns element at position \p i.
	Elem& operator[](const Int& i)
	{	return Super::operator[](i); }

	///	Returns element at position \p i.
	Elem& operator[](const UInt& i)
	{	return Super::operator[](i); }

	///	Returns element at position \p i.
	const Elem& operator[](const Int& i) const
	{	return Super::operator[](i); }

	///	Returns element at position \p i.
	const Elem& operator[](const UInt& i) const
	{	return Super::operator[](i); }

	///	Returns element at position \p i.
	template<class T1>
	const Elem& operator[](Reversible<T1> i) const
	{	return Super::operator[](i.get());	}

	///	Returns element at position \p i.
	template<class T1>
	Elem& operator[](Reversible<T1> i)
	{	return Super::operator[](i.get());	}
*/

	/// Returns \a true if all variables are ground, \a false otherwise.
	Bool ground() const;

	private:

	typedef typename Super::Data		Data;

	// note: disabled for now
	static void initCell(DomVar<T,Dom>* mem, ICPSolver& solver)
	{	/*::new(mem) DomVar<T,Dom>(solver);*/ }

	template<class T1>
	static void initCell(DomVar<T,Dom>* mem, const T1& p1, ICPSolver& solver)
	{	::new(mem) DomVar<T,Dom>(solver,p1); }

	template<class T1,class T2>
	static void initCell(DomVar<T,Dom>* mem, const T1& p1, const T2& p2, ICPSolver& solver)
	{	::new(mem) DomVar<T,Dom>(solver,p1,p2);	}

	template<class T1,class T2,class T3>
	static void initCell(DomVar<T,Dom>* mem, const T1& p1, const T2& p2, const T3& p3, ICPSolver& solver)
	{	::new(mem) DomVar<T,Dom>(solver,p1,p2,p3);	}

	template<class T1,class T2,class T3,class T4>
	static void initCell(DomVar<T,Dom>* mem, const T1& p1, const T2& p2, const T3& p3, const T4& p4, ICPSolver& solver)
	{	::new(mem) DomVar<T,Dom>(solver,p1,p2,p3,p4);	}

	friend class detail::ArrayTraits<T,dims-1>;
	friend class detail::ArrayDataTraits<Self,dims>;

	ICPSolver& mSolver;
};

template<class T,Int Dims,class Dom>
struct GetCPSolver<DomVarArray<T,Dims,Dom> >
{	ICPSolver& operator()(const DomVarArray<T,Dims,Dom>& v)
	{ return v.solver(); }
};

template<class T, int dims, class Dom>
Bool DomVarArray<T,dims,Dom>::ground() const
{
	for (UInt i = 0; i < this->size(); i++)
		if (!this->operator[](i).ground())
			return false;
	return true;
}

template<class T, int Dims, class Dom>
struct EvalTraits<DomVarArray<T,Dims,Dom> >
{	typedef Seq<T>	Eval;	};

template<class ArrayView, class IdxView>
struct EvalTraits<Rel2<Element,ArrayView,IdxView> >
{	typedef	typename EvalTraits<typename ElemTraits<ArrayView>::Elem>::Eval	Eval;	};

template<class T,int Dims,class Dom>
struct ElemTraits<DomVarArray<T,Dims,Dom> >
{	typedef DomVar<T,Dom>	Elem;	};

// specialization for multidimesional (domvar)arrays
template<class Eval,int dims,class Dom>
struct IterationView<DomVarArray<Eval,dims,Dom> >
{
	typedef IterationView<DomVarArray<Eval,dims,Dom> >	Self;
	IterationView(const DomVarArray<Eval,dims,Dom>& v) : idx(0),v(v),max(v.count()) {}
	IterationView(const DomVarArray<Eval,dims,Dom>& v,UInt idx) : idx(idx),v(v),max(v.count()) {}
	IterationView(const IterationView& s) : idx(s.idx),v(s.v),max(s.max) {}
	Void		iterate()	{	assert(valid()); ++idx;	}
	const DomVar<Eval,Dom>& 	value() const 	{	assert(valid()); return v(idx);	}
	Bool		valid() const 	{	return idx < max;	}
	Self		next() const {	return Self(v,idx+1); }
	UInt						idx;
	DomVarArray<Eval,dims,Dom>	v;
	const UInt					max;
};

};


#endif // _H_CASPER_KERNEL_DOMVARARRAY

namespace casper {
#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_KERNEL_MACRO_DOMVARARRAY_H
#define CASPER_KERNEL_MACRO_DOMVARARRAY_H
#include <casper/kernel/macro/domvararray.h>
#endif
#endif
};
