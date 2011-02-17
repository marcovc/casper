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

#include <casper/kernel/container/common.h>
#include <casper/kernel/delta.h>
#include <casper/kernel/util.h>

namespace casper {
namespace detail {
	/*
struct ProxyFlag : INotifier
{
	ProxyFlag(INotifiable* pOwner, Reversible<Bool>& flag) :
		pOwner(pOwner),flag(flag) {}
	Bool notify(Event event)
	{
		flag = true;
		return pOwner->notify(event);
	}
	INotifiable* 		pOwner;
	Reversible<Bool>& 	flag;
};

ProxyFlag* proxyFlag(INotifiable* pOwner, Reversible<Bool>& flag)
{	return new (flag.solver().heap()) ProxyFlag(pOwner,flag);	}
*/


template<class Dom>
struct ProxyDelta : INotifiable,DeltaStack<typename Dom::Delta>
{
	ProxyDelta(Dom& d) :
		DeltaStack<typename Dom::Delta>(d.solver()),
		pOwner(NULL),d(d) {}
	Void setOwner(INotifiable* p) { pOwner = p;	}
	Bool notify()
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
	Bool operator()(InputIterator2 it)
	{
		if (b == e or detail::distinct(*it,*b))
			return false;
		++b;
		return true;
	}
	InputIterator b;
	InputIterator e;
};


template<class InputIterator>
Void debugRange(InputIterator b, InputIterator e)
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
Void setLUB(const Dom& d,detail::Vector<typename Dom::Value>& lub)
{
	detail::setUnion(d.beginIn(),d.endIn(),
					 d.beginPoss(),d.endPoss(),
					 detail::VectorInserter<typename Dom::Value>(lub));
}



};
};


#endif /*_H_CASPER_SET_UTIL*/
