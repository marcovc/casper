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
 * Fixed size, non-reallocable, one-dimensional vector.
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_VECTOR
#define _H_CASPER_KERNEL_CONTAINER_VECTOR

#include <casper/kernel/memory.h>
#include <string.h>

using namespace std;

namespace casper {

namespace detail {

/**
 * Fixed size, non-reallocable, one-dimensional vector.
 * \ingroup Containers
 **/
template<class Elem>
struct Vector
{
	typedef Vector<Elem>	Self;
	typedef	Elem			Value;
	typedef	ptrdiff_t		Difference;
	typedef Value&			Reference;
	typedef Value*			Pointer;
	typedef const Value&	ConstReference;
	typedef const Value*	ConstPointer;

	class Iterator;
	friend class Iterator;
	class Iterator : public std::iterator<bidirectional_iterator_tag,
									 	  Value,
									 	  Difference>
	{
		protected:
		typedef	Value*	PValue;
		friend struct Vector;
		public:
		Iterator(PValue pCurElem) : pCurElem(pCurElem) {}
		Iterator(const Iterator& s) : pCurElem(s.pCurElem) {}
		bool operator==(const Iterator& s) const
		{ return pCurElem == s.pCurElem; }
		bool operator!=(const Iterator& s) const
		{ return pCurElem != s.pCurElem; }
		Reference operator*()
		{ return *pCurElem; }
		ConstReference operator*() const
		{ return *pCurElem; }
		Pointer operator->()
		{ return pCurElem; }
		ConstPointer operator->() const
		{ return pCurElem; }
		operator PValue() const {	return pCurElem;}
		Iterator& operator++() // pre-increment
		{ ++pCurElem; return *this; }
		Iterator operator++(int) //post-increment
		{ Iterator ret(pCurElem); ++pCurElem; return ret; }
		const Iterator& operator=(const Iterator& s)
		{ pCurElem = s.pCurElem;	return *this; }
		Iterator& operator--() // pre-decrement
		{ --pCurElem; return *this; }
		Iterator operator--(int) //post-decrement
		{ Iterator ret(pCurElem); --pCurElem; return ret; }
		protected:
		PValue pCurElem;
	};

	typedef Iterator 					ConstIterator;
	typedef reverse_iterator<Iterator> 	ReverseIterator;
	typedef ReverseIterator 			ConstReverseIterator;

	Vector(UInt size) :
			mHeap(stdHeap),
			_size(size),
			data((Elem*)stdHeap.allocate(_size*sizeof(Elem)))
		{}

	Vector(Heap heap,UInt size) :
		mHeap(heap),
		_size(size),
		data((Elem*)heap.allocate(_size*sizeof(Elem)))
	{}

	Vector(Heap heap, const Vector& s)	:
		mHeap(heap),
		_size(s._size),
		data((Elem*)heap.allocate(_size*sizeof(Elem)))
	{	memcpy(data,s.data,sizeof(Elem)*_size);}

	Vector(const Vector& s)	:
		mHeap(s.heap()),
		_size(s._size),
		data((Elem*)mHeap.allocate(_size*sizeof(Elem)))
	{	memcpy(data,s.data,sizeof(Elem)*_size);}

	Vector(Heap heap,UInt size, const Elem& e) :
		mHeap(heap),
		_size(size),
		data((Elem*)heap.allocate(_size*sizeof(Elem)))
	{
		for (UInt i = 0; i < _size; i++)
			::new(&data[i]) Elem(e);
	}

	Vector(Heap heap, Iterator b, Iterator e) :
		mHeap(heap),
		_size(e-b),
		data((Elem*)heap.allocate(_size*sizeof(Elem)))
	{
		UInt i = 0;
		while (b != e)
			::new(&data[i++]) Elem(*(b++));
	}

	~Vector()
	{
		for (UInt i = 0; i < _size; i++)
			data[i].~Elem();
		mHeap.deallocate(data);
	}

	void initialize(UInt idx,const Elem& e)
	{	::new(&data[idx]) Elem(e);	}

	/*void destroy()
	{	mHeap.deallocate(data);		}*/

 	Iterator begin()	{	return Iterator(data);	}
 	Iterator end()	{	return Iterator(data+_size);	}
 	ConstIterator begin() const	{	return Iterator(data);	}
 	ConstIterator end()	const {	return Iterator(data+_size);	}

	unsigned int size() const
	{	return _size;	}

	inline Elem& operator[](UInt i)
	{	assert(i<_size); return data[i]; }

	inline const Elem& operator[](UInt i) const
	{	assert(i<_size); return data[i]; }

	Bool empty() const {	return _size == 0;	}

	const Self& operator=(const Self& s)
	{
		assert( _size == s._size );
		for (UInt i = 0; i < _size; i++)
			data[i] = s.data[i];
		return *this;
	}

	const Self& operator=(const Elem& s)
	{
		for (UInt i = 0; i < _size; i++)
			data[i] = s;
		return *this;
	}

	Bool operator==(const Self& s) const
	{
		for (UInt i = 0; i < _size; i++)
			if (data[i]!=s.data[i])
				return false;
		return true;
	}

	Heap heap() const {	return mHeap;	}

	Heap	mHeap;
	UInt	_size;
	Elem* 	data;
};

template<class T>
std::ostream& operator<<(std::ostream& os, const casper::detail::Vector<T>& v)
{
	os << "{ ";
	if (!v.empty())
	{
 		os << v[0];
		for (unsigned int n = 1; n < v.size(); n++)
			os << ", " << v[n];
	}
	os << " }";
	return os;
}

};

template<class> struct EvalTraits;

template<class T>
struct EvalTraits<detail::Vector<T> >
{	typedef Seq<T>	Eval;	};

};


#endif // _H_CASPER_KERNEL_CONTAINER_VECTOR
