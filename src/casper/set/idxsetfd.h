/***************************************************************************
 *   Copyright (C) 2008 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#if defined(CASPER_IDXSETFD_HASH)
#include <casper/set/idxsetfd-hash.h>
#elif  defined(CASPER_IDXSETFD_HASH1)
#include <casper/set/idxsetfd-hash-1.h>
#elif defined(CASPER_IDXSETFD_HASHPOSS)
#include <casper/set/idxsetfd-hash+poss.h>
#elif defined(CASPER_IDXSETFD_HASHPOSSBITS)
#include <casper/set/idxsetfd-hash+poss+bits.h>
#endif


#ifndef CASPER_SET_IDXSETFD_H
#define CASPER_SET_IDXSETFD_H

namespace casper {
template<class> struct IdxSetFD;

namespace detail {
template<class T,class InputIterator>
Bool setSafeInsertRange(IdxSetFD<T>& d, InputIterator b, InputIterator e)
{	return d.insertRange(b,e);	}

template<class T,class InputIterator>
Bool setSafeEraseRange(IdxSetFD<T>& d, InputIterator b, InputIterator e)
{	return d.eraseRange(b,e);	}
};
};

#endif
