/***************************************************************************
 *   Copyright (C) 2007/2007 by Marco Correia                              *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _H_CASPER_SET_UTIL
#define _H_CASPER_SET_UTIL

#include <casper/kernel/reversible/reversible.h>
#include <casper/cp/delta.h>
#include <casper/util/util.h>

namespace Casper {
namespace CP {
namespace Detail {
	/*
struct ProxyFlag : INotifier
{
	ProxyFlag(INotifiable* pOwner, Reversible<bool>& flag) :
		pOwner(pOwner),flag(flag) {}
	bool notify(Event event)
	{
		flag = true;
		return pOwner->notify(event);
	}
	INotifiable* 		pOwner;
	Reversible<bool>& 	flag;
};

ProxyFlag* proxyFlag(INotifiable* pOwner, Reversible<bool>& flag)
{	return new (flag.store().getHeap()) ProxyFlag(pOwner,flag);	}
*/


template<class Dom>
struct ProxyDelta : INotifiable,DeltaStack<typename Dom::Delta>
{
	ProxyDelta(Dom& d) :
		DeltaStack<typename Dom::Delta>(d.store()),
		pOwner(NULL),d(d) {}
	void setOwner(INotifiable* p) { pOwner = p;	}
	bool notify()
	{
		assert(pOwner);
		push(d.delta());
		return pOwner->notify();
	}
	INotifiable* 		pOwner;
	Dom&				d;
};

template<class Dom>
struct SetInserter;

template<class Dom>
SetInserter<Dom> setInserter(Dom& d)
{	return SetInserter<Dom>(d);	}

template<class Dom>
struct SetPItEraser;

template<class Dom>
SetPItEraser<Dom> setPItEraser(Dom& d)
{	return SetPItEraser<Dom>(d);	}

template<class Dom>
struct SetEraser;

template<class Dom>
struct SetSafeEraser;

template<class Dom>
SetEraser<Dom> setEraser(Dom& d)
{	return SetEraser<Dom>(d);	}

// removes the element referenced by the iterator
// from the specified domain
template<class InputIterator>
struct RangeEqual
{
	RangeEqual(InputIterator b,InputIterator e) : b(b),e(e) {}
	template<class InputIterator2>
	bool operator()(InputIterator2 it)
	{
		if (b == e or Casper::Util::Detail::distinct(*it,*b))
			return false;
		++b;
		return true;
	}
	InputIterator b;
	InputIterator e;
};


template<class InputIterator>
void debugRange(InputIterator b, InputIterator e)
{
	assert(b != e);
	std::cout << *b;
	++b;
	while (b != e)
	{
		std::cout << " " << *b;
		++b;
	}
	std::cout << std::endl;
}


template<class Dom>
void setLUB(const Dom& d,Util::StdVector<typename Dom::Elem>& lub)
{
	Casper::Util::setUnion(d.beginIn(),d.endIn(),
					 d.beginPoss(),d.endPoss(),
					 Casper::Util::VectorInserter<typename Dom::Elem>(lub));
}



} // Detail
} // CP
} // Casper


#endif /*_H_CASPER_SET_UTIL*/
