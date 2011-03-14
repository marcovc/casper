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
#include <casper/kernel/state/state.h>
#include <casper/kernel/state/trail.h>

namespace Casper {

template<class T> struct Reversible;


template<class T>
struct TrailAgent : ITrailAgent
{
	Reversible<T>* id;
	T data;
	ulonglong lastStateId;
	TrailAgent(Reversible<T>& obj,const T& data, ulonglong lastStateId) :
		ITrailAgent(),id(&obj),data(data),lastStateId(lastStateId) {}
	void restore()
	{
		id->data = data;
		id->lastStateID = lastStateId;
	}
};

namespace Detail {

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
	State&		state;
	ulonglong 	lastStateID;

	Reversible(State& state) : state(state),lastStateID(0) {}
	Reversible(State& state,const T& t) : data(t),  state(state),
											lastStateID(0) {}
	Reversible(const Reversible<T>& s) : data(s.data),
						   state(s.state),lastStateID(0) {}

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
	force_inline void  operator+=(const T& s);
	force_inline void  operator-=(const T& s);

	T& get() { return data;}
	const T& get() const { return data;}

	State& getState() const {	return state; }

	protected:
	force_inline void saveMe();
};


template<class T>
force_inline const Reversible<T>& Reversible<T>::operator=(const T& s)
{
	if (lastStateID != state.id())
		saveMe();
  	data = s;
	return *this;
}

template<class T>
force_inline const Reversible<T>& Reversible<T>::operator=(const Self& s)
{
	//if (s.data == data)
	//	return *this;

	if (lastStateID != state.id())
		saveMe();
	data = s.data;
	return *this;
}

template<class T>
force_inline Reversible<T>& Reversible<T>::operator++()
{
	if (lastStateID != state.id())
		saveMe();
	++data;
	return *this;
}

template<class T>
force_inline Reversible<T>  Reversible<T>::operator++(int)
{
	Self ret(*this);
	if (lastStateID != state.id())
		saveMe();
	++data;
	return ret;
}

template<class T>
force_inline Reversible<T>& Reversible<T>::operator--()
{
	if (lastStateID != state.id())
		saveMe();
	--data;
	return *this;
}

template<class T>
force_inline Reversible<T>  Reversible<T>::operator--(int)
{
	Self ret(*this);
	if (lastStateID != state.id())
		saveMe();
	--data;
	return ret;
}

template<class T>
force_inline void  Reversible<T>::operator+=(const T& s)
{
	if (lastStateID != state.id())
		saveMe();
	data+=s;
}

template<class T>
force_inline void  Reversible<T>::operator-=(const T& s)
{
	if (lastStateID != state.id())
		saveMe();
	data-=s;
}

template<class T>
force_inline void Reversible<T>::saveMe()
{
	state.record(new (state.getHeap()) TrailAgent<T>(*this,data,lastStateID));
	lastStateID = state.id();
}

template<class> struct Hash;

template<class T>
struct Hash<Reversible<T> >
{	uint operator()(const Reversible<T>& s)
	{	return Hash<T>()(s.get());	}
};


struct BacktrackSentinel
{
	BacktrackSentinel(State& ctx) : rCounter(ctx,0),iCounter(0) {}
	bool hasBacktracked() const	{ return rCounter < iCounter;	}
	void update() {	rCounter = rCounter+1; iCounter = rCounter; }
	Reversible<counter>	rCounter;
	counter	iCounter;
};

};

#endif
