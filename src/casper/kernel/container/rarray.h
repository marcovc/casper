 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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
 * \file
 * Reversible multidimensional generic array.
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_RARRAY
#define _H_CASPER_KERNEL_CONTAINER_RARRAY

#include <casper/kernel/solver.h>
#include <casper/kernel/container/array.h>
#include <casper/kernel/reversible.h>

using namespace std;

namespace casper {

namespace detail {

// fixed-size, non-reallocable, one-dimensional vector
template<class Elem>
struct RevVector : public Vector<Reversible<Elem> >
{
	typedef RevVector<Elem>				Self;
	typedef Vector<Reversible<Elem> >	Super;
	typedef	Reversible<Elem>			Value;
	typedef	Elem						RawValue;
	typedef	typename Super::Iterator	Iterator;

	RevVector(ICPSolver& solver, UInt size) :
			Super(solver.heap(),size),s(solver) {}
	RevVector(ICPSolver& solver, const RevVector& r) :
			Super(solver.heap(),r),s(solver) {}
	RevVector(ICPSolver& solver, UInt size, const Elem& e) :
			Super(solver.heap(),size,Value(solver,e)),s(solver) {}
	RevVector(ICPSolver& solver, Iterator b, Iterator e) :
			Super(solver.heap(),b,e),s(solver) {}

	const Self& operator=(const Self& s)
	{	Super::operator=(s);	return *this;	}

	const Self& operator=(const Elem& s)
	{	Super::operator=(s);	return *this;	}

	void initialize(UInt idx, const Elem& e)
	{	Super::initialize(idx,Value(s,e));	}

	ICPSolver& s;
};

// fixed-size, non-reallocable, one-dimensional vector
template<class Elem>
struct FastRevVector : protected Vector<Elem>
{
	typedef Vector<Elem>		Super;
	typedef FastRevVector<Elem>	Self;
	typedef	Elem				Value;
	typedef	UInt				Difference;
	typedef Value&				Reference;
	typedef Value*				Pointer;
	typedef const Value&		ConstReference;
	typedef const Value*		ConstPointer;

	typedef typename Super::Iterator 				Iterator;
	typedef typename Super::ReverseIterator 		ReverseIterator;
	typedef typename Super::ConstIterator 			ConstIterator;
	typedef typename Super::ConstReverseIterator 	ConstReverseIterator;

	FastRevVector(ICPSolver& solver,UInt size) : Super(solver.heap(),size),mSolver(solver),
											 lastStateID(0)
	{}

	FastRevVector(ICPSolver& solver, const FastRevVector& s) : Super(solver.heap(),s),
													mSolver(solver),
													lastStateID(0)
	{}

	FastRevVector(ICPSolver& solver,UInt size, const Elem& e) :
		Super(solver.heap(),size,e),
		mSolver(solver),
		lastStateID(0)
	{}

	FastRevVector(ICPSolver& solver, Iterator b, Iterator e) :
		Super(solver.heap(),b,e),
		mSolver(solver),
		lastStateID(0)
	{}

	Elem& operator[](UInt i)
	{
		backup();
		return this->data[i];
	}

	const Elem& operator[](UInt i) const
	{
		return this->data[i];
	}

	const Self& operator=(const Self& s)
	{
		backup();
		Super::operator=(s);
		return *this;
	}

	ICPSolver& solver() const {	return mSolver;	}

	private:
	void backup()
	{
		if (lastStateID != mSolver.stateID())
		{
			mSolver.record(new (mSolver.heap()) TrailAgent<Super>(*this));
			lastStateID = mSolver.stateID();
		}
	}
	ICPSolver&	mSolver;
	unsigned long long lastStateID;
};



};

/**
 * 	Reversible multidimensional generic array.
 * 	\ingroup Containers
 */
template<class T, int dims = 1>
struct RevArray : public Array<Reversible<T>,dims>
{
	typedef RevArray<T,dims>	Self;
	typedef Reversible<T>		Value;
	typedef Array<Value,dims>	Super;

	RevArray(const RevArray& s) : Super(s) { }

	RevArray(ICPSolver& solver,UInt p1) :
		Super(detail::ArrayDataTraits<Self,dims>::initData(solver.heap(),p1,solver))	{}

	template<class T2>
	RevArray(ICPSolver& solver,UInt p1,const T2& p2) :
		Super(detail::ArrayDataTraits<Self,dims>::initData(solver.heap(),p1,p2,solver))	{}

	template<class T3>
	RevArray(ICPSolver& solver,UInt p1,UInt p2,const T3& p3) :
		Super(detail::ArrayDataTraits<Self,dims>::initData(solver.heap(),p1,p2,p3,solver))	{}

	template<class T4>
	RevArray(ICPSolver& solver,UInt p1,UInt p2,UInt p3,const T4& p4) :
		Super(detail::ArrayDataTraits<Self,dims>::initData(solver.heap(),p1,p2,p3,p4,solver)) {}

	template<class T5>
	RevArray(ICPSolver& solver,UInt p1,UInt p2,UInt p3,UInt p4,const T5& p5)
		: Super(detail::ArrayDataTraits<Self,dims>::initData(solver.heap(),p1,p2,p3,p4,p5,solver))	{}

	const Self& operator=(const T& value)
	{
		Int _count = (Int)this->count();
		for (Int i = 0; i < _count; i++)
			(*this)(i) = value;
		return *this;
	}

	//	protected:
	typedef typename Super::Data					Data;
	typedef typename Super::Elem					Elem;

	static void initCell(Reversible<T>* mem, ICPSolver& solver)
	{	::new(mem) Reversible<T>(solver); }

	template<class T1>
	static void initCell(Reversible<T>* mem, T1 p1, ICPSolver& solver)
	{	::new(mem) Reversible<T>(solver,p1); }

};

};


#endif
