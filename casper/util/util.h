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
#include <iostream>
#include <casper/util/container/common.h>

namespace Casper {
namespace Util {

#define EVER ; ;

// **** Random number helper functions ****

// generate random integer number in the interval [min,max]
int randomInt(int min, int max);

// generate random real number in the interval [min,max]
template<class T>
T randomReal(T min, T max)
{	return min + static_cast<T>((max-min) * (rand() / RAND_MAX));	}

// generate random bool
bool randomBool();



template<class InputIterator1,class Processor>
bool copy(InputIterator1 b1, InputIterator1 e1,Processor pr)
{
	while (b1 != e1)
	{
		if (!pr(b1++))
			return false;
	}
	return true;
}


template<class InputIterator>
uint count(InputIterator b, InputIterator e)
{
	uint ret = 0;
	while (b != e)
	{
		++b;
		++ret;
	}
	return ret;
}

template<class InputIterator1,class InputIterator2,class Processor>
bool setDifference(InputIterator1 b1, InputIterator1 e1,
				   InputIterator2 b2, InputIterator2 e2,
				   Processor pr)
{
	while (b1 != e1)
	{
		if (b2 == e2 or Detail::less(*b1,*b2))
		{
			if (!pr(b1++))
				return false;
		}
		else
		if (!(Detail::greater(*b1,*b2)))	// i.e. the same element
		{
			++b1;
			++b2;
		}
		else
		while (b2 != e2 and Detail::greater(*b1,*b2))
			++b2;
	}
	return true;
}

template<class InputIterator1,class InputIterator2,class Processor>
bool setIntersection(InputIterator1 b1, InputIterator1 e1,
			   		 InputIterator2 b2, InputIterator2 e2,
			   		 Processor pr)
{
	while ( b1 != e1 and b2 != e2)
	{
		if (Detail::less(*b1,*b2))
			++b1;
		else
		if (Detail::less(*b2,*b1))
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
bool setUnion(InputIterator1 b1, InputIterator1 e1,
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
		if (b2 == e2 or Detail::less(*b1,*b2))
		{
			if (!pr(b1++))
				return false;
		}
		else
		if (Detail::less(*b2,*b1))
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
bool intersect(InputIterator1 b1, InputIterator1 e1,
			   InputIterator2 b2, InputIterator2 e2)
{
	for ( EVER )
	{
		if (b1 == e1 or b2 == e2)
			return false;
		if (Detail::less(*b1,*b2))
			++b1;
		else
		if (Detail::less(*b2,*b1))
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
	bool operator()(InputIterator it)
	{	return p1(it) and p2(it); }
	P1	p1;
	P2	p2;
};

template<class P1,class P2>
Fork<P1,P2> fork(const P1& p1, const P2& p2)
{	return Fork<P1,P2>(p1,p2);	}

// inserts the element referenced by the iterator
// in the StdList
template<class> struct StdList;

template<class T>
struct ListInserter
{
	ListInserter(StdList<T>& l) : l(l) {}
	template<class InputIterator>
	bool operator()(InputIterator it)
	{	l.pushBack(*it);	return true; }
	StdList<T>& l;
};

template<class> struct StdSList;

// inserts the element referenced by the iterator
// in the StdSList
template<class T>
struct SListInserter
{
	SListInserter(StdSList<T>& l) : l(l) {}
	template<class InputIterator>
	bool operator()(InputIterator it)
	{	l.pushFront(*it);	return true; }
	StdSList<T>& l;
};

template<class> struct StdVector;

// inserts the element referenced by the iterator
// in the StdVector
// \note StdVector must have room for all inserts
template<class T>
struct VectorInserter
{
	VectorInserter(StdVector<T>& l) : l(l),n(0) {}
	template<class InputIterator>
	bool operator()(InputIterator it)
	{	l[n++] = *it;	return true; }
	StdVector<T>& l;
	uint	n;
};

// counts the number of elements
struct Counter
{
	Counter(uint& s) : s(s) {}
	template<class InputIterator>
	bool operator()(InputIterator)
	{	++s; return true;	}
	uint& s;
};

struct Printer
{
	template<class InputIterator>
	bool operator()(InputIterator it)
	{	std::cout << *it << " "; return true;	}
};

// like std::equal but checks if range b1..e1 is equal to b2..e2
// (std::equal assumes the size of both ranges is the same)
template<class InputIterator1, class InputIterator2>
bool equal(InputIterator1 b1, InputIterator1 e1,
		   InputIterator2 b2, InputIterator2 e2)
{
	while (b1 != e1)
	{
		if (b2 == e2 or Detail::distinct(*b1,*b2))
			return false;
		++b1;
		++b2;
	}
	return b2 == e2;
}

// checks if range b1..e1 is contained in b2..e2
template<class InputIterator1, class InputIterator2>
bool contained(InputIterator1 b1, InputIterator1 e1,
		       InputIterator2 b2, InputIterator2 e2)
{
	while (b1 != e1)
	{
		if (b2 == e2 or Detail::less(*b1,*b2))
			return false;
		if (Detail::equal(*b1,*b2))
			++b1;
		++b2;
	}
	return b2 == e2;
}

} // Util
} // Casper

#endif /*CASPER_KERNEL_UTIL_H_*/
