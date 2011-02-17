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

#ifndef _H_CASPER_KERNEL_REVERSIBLE
#define _H_CASPER_KERNEL_REVERSIBLE

#include <casper/kernel/common.h>
#include <casper/kernel/solver.h>


namespace casper {

template<class T> struct Reversible;


template<class T>
struct TrailAgent : ITrailAgent
{
	Reversible<T>* id;
	T data;
	ULongLong lastStateId;
	TrailAgent(Reversible<T>& obj,const T& data, ULongLong lastStateId) :
		ITrailAgent(),id(&obj),data(data),lastStateId(lastStateId) {}
	void restore()
	{
		id->data = data;
		id->lastStateID = lastStateId;
	}
};

namespace detail {

template<class T>
struct PointerTraits
{};

template<class T>
struct PointerTraits<T*>
{	typedef	T	Data;	};

};


/**
 *  \note Reversible pointers are not deleted. It is the client's responsability
 *  to delete the data pointed by a reversible pointer.
 */

template<class T>
struct Reversible
{
	typedef Reversible<T>			Self;
	T	data;
	ICPSolver&	mSolver;
	ULongLong 	lastStateID;

	Reversible(ICPSolver& solver) : mSolver(solver),lastStateID(0) {}
	Reversible(ICPSolver& solver,const T& t) : data(t),  mSolver(solver),
											lastStateID(0) {}
	Reversible(const Reversible<T>& s) : data(s.data),
						   mSolver(s.mSolver),lastStateID(0) {}

	~Reversible()	{}

	force_inline const Self& operator=(const T& s);
	force_inline const Self& operator=(const Self& s);

	operator const T&() const { return data; }

	// useful for reversible pointers
	// (const version not necessary because of automatic conversion to const T)
	T operator->()
	{	return data;	}

	force_inline Self& operator++();
	force_inline Self  operator++(int);
	force_inline Self& operator--();
	force_inline Self  operator--(int);
	force_inline Void  operator+=(const T& s);
	force_inline Void  operator-=(const T& s);

	T& get() { return data;}
	const T& get() const { return data;}

	ICPSolver& solver() const {	return mSolver; }

	protected:
	force_inline void saveMe();
};


template<class T>
force_inline const Reversible<T>& Reversible<T>::operator=(const T& s)
{
	if (lastStateID != mSolver.stateID())
		saveMe();
  	data = s;
	return *this;
}

template<class T>
force_inline const Reversible<T>& Reversible<T>::operator=(const Self& s)
{
	//if (s.data == data)
	//	return *this;

	if (lastStateID != mSolver.stateID())
		saveMe();
	data = s.data;
	return *this;
}

template<class T>
force_inline Reversible<T>& Reversible<T>::operator++()
{
	if (lastStateID != mSolver.stateID())
		saveMe();
	++data;
	return *this;
}

template<class T>
force_inline Reversible<T>  Reversible<T>::operator++(int)
{
	Self ret(*this);
	if (lastStateID != mSolver.stateID())
		saveMe();
	++data;
	return ret;
}

template<class T>
force_inline Reversible<T>& Reversible<T>::operator--()
{
	if (lastStateID != mSolver.stateID())
		saveMe();
	--data;
	return *this;
}

template<class T>
force_inline Reversible<T>  Reversible<T>::operator--(int)
{
	Self ret(*this);
	if (lastStateID != mSolver.stateID())
		saveMe();
	--data;
	return ret;
}

template<class T>
force_inline Void  Reversible<T>::operator+=(const T& s)
{
	if (lastStateID != mSolver.stateID())
		saveMe();
	data+=s;
}

template<class T>
force_inline Void  Reversible<T>::operator-=(const T& s)
{
	if (lastStateID != mSolver.stateID())
		saveMe();
	data-=s;
}

template<class T>
force_inline void Reversible<T>::saveMe()
{
	mSolver.record(new (mSolver.heap()) TrailAgent<T>(*this,data,lastStateID));
	lastStateID = mSolver.stateID();
}

template<class> struct Hash;

template<class T>
struct Hash<Reversible<T> >
{	UInt operator()(const Reversible<T>& s)
	{	return Hash<T>()(s.get());	}
};

};

#endif
