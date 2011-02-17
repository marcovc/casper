 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_KERNEL_CONTAINER_COMMON_H_
#define CASPER_KERNEL_CONTAINER_COMMON_H_

#include <assert.h>
#include <functional>
#include <casper/kernel/memory.h>

namespace casper {
namespace detail {

template<class T,class Compare = std::less<T> >
struct TestRel
{
	static bool equal(const T& t1, const T& t2)
	{	return !Compare()(t1,t2) and !Compare()(t2,t1);	}
	static bool distinct(const T& t1, const T& t2)
	{	return Compare()(t1,t2) or Compare()(t2,t1);	}
	static bool less(const T& t1, const T& t2)
	{	return Compare()(t1,t2);	}
	static bool greater(const T& t1, const T& t2)
	{	return Compare()(t2,t1);	}
	static bool lessEqual(const T& t1, const T& t2)
	{	return !Compare()(t2,t1);	}
	static bool greaterEqual(const T& t1, const T& t2)
	{	return !Compare()(t1,t2);	}

	// allow comparing objects with different types
	template<class K>
	static bool equal(const T& t1, const K& t2)
	{	return !Compare()(t1,t2) and !Compare()(t2,t1);	}
	template<class K>
	static bool equal(const K& t1, const T& t2)
	{	return !Compare()(t1,t2) and !Compare()(t2,t1);	}
	template<class K>
	static bool distinct(const K& t1, const T& t2)
	{	return Compare()(t1,t2) or Compare()(t2,t1);	}
	template<class K>
	static bool distinct(const T& t1, const K& t2)
	{	return Compare()(t1,t2) or Compare()(t2,t1);	}
	template<class K>
	static bool less(const K& t1, const T& t2)
	{	return Compare()(t1,t2);	}
	template<class K>
	static bool less(const T& t1, const K& t2)
	{	return Compare()(t1,t2);	}
	template<class K>
	static bool greater(const K& t1, const T& t2)
	{	return Compare()(t2,t1);	}
	template<class K>
	static bool greater(const T& t1, const K& t2)
	{	return Compare()(t2,t1);	}
	template<class K>
	static bool lessEqual(const K& t1, const T& t2)
	{	return !Compare()(t2,t1);	}
	template<class K>
	static bool lessEqual(const T& t1, const K& t2)
	{	return !Compare()(t2,t1);	}
	template<class K>
	static bool greaterEqual(const K& t1, const T& t2)
	{	return !Compare()(t1,t2);	}
	template<class K>
	static bool greaterEqual(const T& t1, const K& t2)
	{	return !Compare()(t1,t2);	}
};

template<class T>
Bool	equal(const T& t1, const T& t2)
{	return TestRel<T>::equal(t1,t2);	}

template<class T>
Bool	distinct(const T& t1, const T& t2)
{	return TestRel<T>::distinct(t1,t2);	}

template<class T>
Bool	lessEqual(const T& t1, const T& t2)
{	return TestRel<T>::lessEqual(t1,t2);	}

template<class T>
Bool	greaterEqual(const T& t1, const T& t2)
{	return TestRel<T>::greaterEqual(t1,t2);	}

template<class T>
Bool	less(const T& t1, const T& t2)
{	return TestRel<T>::less(t1,t2);	}

template<class T>
Bool	greater(const T& t1, const T& t2)
{	return TestRel<T>::greater(t1,t2);	}

template<class InputIterator>
Bool	isStrictOrdered(InputIterator b,InputIterator e)
{
	if (b==e)
		return true;
	InputIterator next = b;
	++next;
	while (next != e)
	{
		if (greaterEqual(*b,*next))
			return false;
		++b;
		++next;
	}
	return true;
}

// Iterator adaptor which skips some values of the underlying iterator
template<class Traits>
struct FilterIterator : Traits::UnderIterator
{
	typedef typename Traits::UnderIterator	Super;
	typedef	typename Traits::Value	Value;
	typedef const Value&	ConstReference;
	typedef const Value*	ConstPointer;

	FilterIterator() : Super()  {}
	FilterIterator(const Super& s) : Super(s)
	{	assert(Traits::valid(*this));	}
	FilterIterator(const FilterIterator& s) : Super(s) {}

	FilterIterator& operator++() // pre-increment
	{
		// the following assertion may be also triggered when erasing an
		// iterator and then incrementing it.
		// assert(Traits::valid(*this));
		do {
			Super::operator++();
		} while (!Traits::valid(*this));
		return *this;
	}

	FilterIterator operator++(int) //post-increment
	{
		assert(Traits::valid(*this));
		FilterIterator ret(*this);
		++(*this);
		return ret;
	}

	const FilterIterator& operator=(const FilterIterator& i)
	{ 	static_cast<Super&>(*this) = i; return *this; }

	ConstReference operator*() const
	{ return Traits::dereference(*this); }
	ConstPointer operator->() const
	{ return &Traits::dereference(*this); }
};

struct StaticMemHolder
{
	StaticMemHolder(UInt size) : mHeap(size) {}
	Heap	heap() const {	return const_cast<IHeap*>(static_cast<const IHeap*>(&mHeap)); }
	StaticHeap	mHeap;
};

/*
template<class InputIterator1, class InputIterator2>
struct ZipIterator
{
	typedef typename InputIterator1::Reference	Reference;
	typedef typename InputIterator1::Pointer	Pointer;

	ZipIterator(InputIterator1 it1, InputIterator2 it2) : it1(it1),it2(it2)
	{...}

	bool operator==(const ZipIterator& s) const
	{ return it1 == s.it1 and it2 == s.it2 and active == s.active; }

	bool operator!=(const ZipIterator& s) const
	{ return it1 != s.it1 or it2 != s.it2 or active != s.active; }

	Reference operator*() const
	{
		if (active == 0)
			return *it1;
		else
			return *it2;
	}

	Pointer operator->() const
	{	return &operator*();	}

	ZipIterator& operator++() // pre-increment
	{	if }
Iterator operator++(int) //post-increment
{ assert(curNode); Iterator ret(*this); curNode = curNode->next; return ret; }
const Iterator& operator=(const Iterator& i)
{ curNode = i.curNode;	return *this; }
	private:
	InputIterator1 it1;
	InputIterator2 it2;
	Bool active;
};*/

template<class Container>
struct ElemInContainer
{
	typedef typename ElemTraits<Container>::Elem Elem;
	ElemInContainer(const Container& c) : c(c) {}
	Bool operator()(const Elem& t) const
	{	return c.find(t)!=c.end(); }
	const Container& c;
};

template<class Container>
ElemInContainer<Container> elemInContainer(const Container& c)
{	return ElemInContainer<Container>(c);	}

};

};

#endif /*CASPER_KERNEL_CONTAINER_COMMON_H_*/
