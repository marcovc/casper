/***************************************************************************
 *   Copyright (C) 2007 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CASPER_KERNEL_UTIL_H_
#define CASPER_KERNEL_UTIL_H_

#include <stdlib.h>
#include <casper/kernel/common.h>
#include <casper/kernel/container/common.h>

namespace casper {
namespace detail {

// **** Random number helper functions ****

// generate random integer number in the interval [min,max]
Int randomInt(Int min, Int max);

// generate random real number in the interval [min,max]
template<class T>
T randomReal(T min, T max)
{	return min + static_cast<T>((max-min) * (rand() / RAND_MAX));	}

// generate random bool
Bool randomBool();



template<class InputIterator1,class Processor>
Bool copy(InputIterator1 b1, InputIterator1 e1,Processor pr)
{
	while (b1 != e1)
	{
		if (!pr(b1++))
			return false;
	}
	return true;
}


template<class InputIterator>
UInt count(InputIterator b, InputIterator e)
{
	UInt ret = 0;
	while (b != e)
	{
		++b;
		++ret;
	}
	return ret;
}

template<class InputIterator1,class InputIterator2,class Processor>
Bool setDifference(InputIterator1 b1, InputIterator1 e1,
				   InputIterator2 b2, InputIterator2 e2,
				   Processor pr)
{
	while (b1 != e1)
	{
		if (b2 == e2 or detail::less(*b1,*b2))
		{
			if (!pr(b1++))
				return false;
		}
		else
		if (!(detail::greater(*b1,*b2)))	// i.e. the same element
		{
			++b1;
			++b2;
		}
		else
		while (b2 != e2 and detail::greater(*b1,*b2))
			++b2;
	}
	return true;
}

template<class InputIterator1,class InputIterator2,class Processor>
Bool setIntersection(InputIterator1 b1, InputIterator1 e1,
			   		 InputIterator2 b2, InputIterator2 e2,
			   		 Processor pr)
{
	while ( b1 != e1 and b2 != e2)
	{
		if (detail::less(*b1,*b2))
			++b1;
		else
		if (detail::less(*b2,*b1))
			++b2;
		else
		{
			if (!pr(b1++))
				return false;
			++b2;
		}
	}
	return true;
}

template<class InputIterator1,class InputIterator2,class Processor>
Bool setUnion(InputIterator1 b1, InputIterator1 e1,
			  InputIterator2 b2, InputIterator2 e2,
			  Processor pr)
{
	for ( EVER )
	{
		if (b1 == e1)
		{
			if (b2 == e2)
				return true;
			if (!pr(b2++))
				return false;
		}
		else
		if (b2 == e2 or detail::less(*b1,*b2))
		{
			if (!pr(b1++))
				return false;
		}
		else
		if (detail::less(*b2,*b1))
		{
			if (!pr(b2++))
				return false;
		}
		else
		{
			++b1;
			if (!pr(b2++))
				return false;
		}
	}
	return true;
}


// returns true if both ranges intersect
template<class InputIterator1,class InputIterator2>
Bool intersect(InputIterator1 b1, InputIterator1 e1,
			   InputIterator2 b2, InputIterator2 e2)
{
	for ( EVER )
	{
		if (b1 == e1 or b2 == e2)
			return false;
		if (detail::less(*b1,*b2))
			++b1;
		else
		if (detail::less(*b2,*b1))
			++b2;
		else
			return true;
	}
}

// forks processing
template<class P1,class P2>
struct Fork
{
	Fork(const P1& p1, const P2& p2) : p1(p1),p2(p2) {}
	template<class InputIterator>
	Bool operator()(InputIterator it)
	{	return p1(it) and p2(it); }
	P1	p1;
	P2	p2;
};

template<class P1,class P2>
Fork<P1,P2> fork(const P1& p1, const P2& p2)
{	return Fork<P1,P2>(p1,p2);	}

// inserts the element referenced by the iterator
// in the List
template<class> struct List;

template<class T>
struct ListInserter
{
	ListInserter(List<T>& l) : l(l) {}
	template<class InputIterator>
	Bool operator()(InputIterator it)
	{	l.pushBack(*it);	return true; }
	List<T>& l;
};

template<class> struct SList;

// inserts the element referenced by the iterator
// in the SList
template<class T>
struct SListInserter
{
	SListInserter(SList<T>& l) : l(l) {}
	template<class InputIterator>
	Bool operator()(InputIterator it)
	{	l.pushFront(*it);	return true; }
	SList<T>& l;
};

template<class> struct Vector;

// inserts the element referenced by the iterator
// in the Vector
// \note Vector must have room for all inserts
template<class T>
struct VectorInserter
{
	VectorInserter(Vector<T>& l) : l(l),n(0) {}
	template<class InputIterator>
	Bool operator()(InputIterator it)
	{	l[n++] = *it;	return true; }
	Vector<T>& l;
	UInt	n;
};

// counts the number of elements
struct Counter
{
	Counter(UInt& s) : s(s) {}
	template<class InputIterator>
	Bool operator()(InputIterator)
	{	++s; return true;	}
	UInt& s;
};

struct Printer
{
	template<class InputIterator>
	Bool operator()(InputIterator it)
	{	std::cout << *it << " "; return true;	}
};

// like std::equal but checks if range b1..e1 is equal to b2..e2
// (std::equal assumes the size of both ranges is the same)
template<class InputIterator1, class InputIterator2>
Bool equal(InputIterator1 b1, InputIterator1 e1,
		   InputIterator2 b2, InputIterator2 e2)
{
	while (b1 != e1)
	{
		if (b2 == e2 or detail::distinct(*b1,*b2))
			return false;
		++b1;
		++b2;
	}
	return b2 == e2;
}

// checks if range b1..e1 is contained in b2..e2
template<class InputIterator1, class InputIterator2>
Bool contained(InputIterator1 b1, InputIterator1 e1,
		       InputIterator2 b2, InputIterator2 e2)
{
	while (b1 != e1)
	{
		if (b2 == e2 or detail::less(*b1,*b2))
			return false;
		if (detail::equal(*b1,*b2))
			++b1;
		++b2;
	}
	return b2 == e2;
}

}
}
#endif /*CASPER_KERNEL_UTIL_H_*/
