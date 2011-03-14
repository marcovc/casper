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

#ifndef _H_CASPER_KERNEL_CONTAINER_ARRAY
#define _H_CASPER_KERNEL_CONTAINER_ARRAY

#include <casper/util/container/stdvector.h>
#include <casper/util/container/stdarray.h>

#include <casper/kernel/state/state.h>
#include <casper/kernel/reversible/array.h>
#include <casper/kernel/reversible/reversible.h>

using namespace std;

namespace Casper {

// fixed-size, non-reallocable, one-dimensional vector
template<class Elem>
struct Vector : public Util::StdVector<Reversible<Elem> >
{
	typedef Vector<Elem>				Self;
	typedef Util::StdVector<Reversible<Elem> >	Super;
	typedef	Reversible<Elem>			Value;
	typedef	Elem						RawValue;
	typedef	typename Super::Iterator	Iterator;

	Vector(State& state, uint size) :
			Super(state,size),s(state) {}
	Vector(State& state, const Vector& r) :
			Super(state,r),s(state) {}
	Vector(State& state, uint size, const Elem& e) :
			Super(state,size,Value(state,e)),s(state) {}
	Vector(State& state, Iterator b, Iterator e) :
			Super(state,b,e),s(state) {}

	const Self& operator=(const Self& s)
	{	Super::operator=(s);	return *this;	}

	const Self& operator=(const Elem& s)
	{	Super::operator=(s);	return *this;	}

	void initialize(uint idx, const Elem& e)
	{	Super::initialize(idx,Value(s,e));	}

	State& s;
};

#if 0
// fixed-size, non-reallocable, one-dimensional vector
template<class Elem>
struct FastRevVector : protected Util::StdVector<Elem>
{
	typedef Util::StdVector<Elem>		Super;
	typedef FastRevVector<Elem>	Self;
	typedef	Elem				Value;
	typedef	uint				Difference;
	typedef Value&				Reference;
	typedef Value*				Pointer;
	typedef const Value&		ConstReference;
	typedef const Value*		ConstPointer;

	typedef typename Super::Iterator 				Iterator;
	typedef typename Super::ReverseIterator 		ReverseIterator;
	typedef typename Super::ConstIterator 			ConstIterator;
	typedef typename Super::ConstReverseIterator 	ConstReverseIterator;

	FastRevVector(State& state,uint size) : Super(state.getHeap(),size),state(state),
											 lastStateID(0)
	{}

	FastRevVector(State& state, const FastRevVector& s) : Super(state.getHeap(),s),
													state(state),
													lastStateID(0)
	{}

	FastRevVector(State& state,uint size, const Elem& e) :
		Super(state.getHeap(),size,e),
		state(state),
		lastStateID(0)
	{}

	FastRevVector(State& state, Iterator b, Iterator e) :
		Super(state.getHeap(),b,e),
		state(state),
		lastStateID(0)
	{}

	Elem& operator[](uint i)
	{
		backup();
		return this->data[i];
	}

	const Elem& operator[](uint i) const
	{
		return this->data[i];
	}

	const Self& operator=(const Self& s)
	{
		backup();
		Super::operator=(s);
		return *this;
	}

	State& getState() const {	return state;	}

	private:
	void backup()
	{
		if (lastStateID != state.stateID())
		{
			state.record(new (state.getHeap()) TrailAgent<Super>(*this));
			lastStateID = state.stateID();
		}
	}
	State&	state;
	unsigned long long lastStateID;
};
#endif


/**
 * 	Reversible multidimensional generic array.
 * 	\ingroup Containers
 */
template<class T, int dims = 1>
struct Array : public Util::StdArray<Reversible<T>,dims>
{
	typedef Array<T,dims>			Self;
	typedef Reversible<T>			Value;
	typedef Util::StdArray<Value,dims>	Super;

	Array(const Array& s) : Super(s) { }

	Array(State& state,uint p1) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(state,p1,state))	{}

	template<class T2>
	Array(State& state,uint p1,const T2& p2) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(state,p1,p2,state))	{}

	template<class T3>
	Array(State& state,uint p1,uint p2,const T3& p3) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(state,p1,p2,p3,state))	{}

	template<class T4>
	Array(State& state,uint p1,uint p2,uint p3,const T4& p4) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(state,p1,p2,p3,p4,state)) {}

	template<class T5>
	Array(State& state,uint p1,uint p2,uint p3,uint p4,const T5& p5)
		: Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(state,p1,p2,p3,p4,p5,state))	{}

	const Self& operator=(const T& value)
	{
		int _count = (int)this->count();
		for (int i = 0; i < _count; i++)
			(*this)(i) = value;
		return *this;
	}

	//	protected:
	typedef typename Super::Data					Data;
	typedef typename Super::Elem					Elem;

	static void initCell(Reversible<T>* mem, State& state)
	{	::new(mem) Reversible<T>(state); }

	template<class T1>
	static void initCell(Reversible<T>* mem, T1 p1, State& state)
	{	::new(mem) Reversible<T>(state,p1); }

};

} // Casper

#endif
