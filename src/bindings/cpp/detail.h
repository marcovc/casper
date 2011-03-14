/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2009 - Marco Correia <marco.v.correia@gmail.com>           *
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

#ifndef CASPER_BINDINGS_CPP_DETAIL_H_
#define CASPER_BINDINGS_CPP_DETAIL_H_

#include "symbol.h"
#include "exception.h"
#include <sstream>
#include <algorithm>
#include <set>


namespace Casperbind {
namespace cpp {
namespace Detail {

/// Generic ranges.
template<class T>
struct StdRange : Container
{
	T lower;
	T upper;

	StdRange(const T l,const T u) : lower(l),upper(u) {}
	~StdRange() { }
	Type getType(bool t = false) const { return Symbol::sRange; }
	ElemType getElemType() const { return Detail::getElemType<T>();	}
	const T& getLower() const { return lower; }
	const T& getUpper() const { return upper; }
	void setLower(const T& n) { lower = n; }
	void setUpper(const T& n) { upper = n; }
	const Symbol* clone() const {	return new StdRange(lower,upper);	}
	bool operator==(const StdRange& s) const { return lower==s.lower and upper==s.upper; }
	bool operator!=(const StdRange& s) const { return lower!=s.lower or upper!=s.upper; }
};

/// Generic sets.
template<class T, class Compare = std::less<T> >
struct Set : Container
{
	typedef std::set<T,Compare> Data;
	typedef typename Data::iterator Iterator;
	typedef typename Data::const_iterator ConstIterator;
	typedef typename Data::reverse_iterator RIterator;
	typedef typename Data::const_reverse_iterator ConstRIterator;

	Data data;

	Set() {}
	Set(const Set& s) : data(s.data) {}
	Set(const std::initializer_list<T>& i) : data(i) {}
	~Set() {}
	Iterator begin() { return data.begin(); }
	Iterator end() { return data.end(); }
	ConstIterator begin() const { return data.begin(); }
	ConstIterator end() const { return data.end(); }

	RIterator rbegin() { return data.rbegin(); }
	RIterator rend() { return data.rend(); }
	ConstRIterator rbegin() const { return data.rbegin(); }
	ConstRIterator rend() const { return data.rend(); }

	Type getType(bool t = false) const { return Symbol::sSet; }
	ElemType getElemType() const { return Detail::getElemType<T>();	}
	int getSize() const { return data.size(); }
	bool empty() const { return data.empty(); }
	void add(const T& t) { data.insert(t); }
	ConstIterator find(const T& t) const { return data.find(t);	}

	const Symbol* clone() const {	return new Set(*this);	}
};


/// Generic multidimensional arrays.
template<class T>
struct Array : Container
{
	int dims;
	int* sizes;
	T* data;

	Array(const int);
	Array(const int, const int*);
	Array(const int, const int*, const T*);
	Array(const Array& s);
	Array(const std::initializer_list<T>& i);
	~Array();
	Type getType(bool t = false) const
	{	return Symbol::sArray;	}
	ElemType getElemType() const { return Detail::getElemType<T>();	}
	int getSize() const;
	const Symbol* clone() const
	{	return new Array(dims, sizes, data); }
	const T& operator[](int i) const { return data[i]; }
	T& operator[](int i) { return data[i]; }
	T* getData() { return data;}
	const T* getData() const { return data; }
protected:
	void initialize(const int, const int*, const T*);
};

template<class T>
Array<T> makeArray(const T& arg1)
{ 	Array<T> r(1); r[0] = arg1; return r; }

template<class T>
Array<T> makeArray(const T& arg1,const T& arg2)
{  	Array<T> r(2); r[0] = arg1; r[1] = arg2; return r; }

template<class T>
Array<T> makeArray(const T& arg1,const T& arg2,const T& arg3)
{ 	Array<T> r(3); r[0] = arg1; r[1] = arg2; r[2] = arg3; return r;}


/**
 *  Creates a one-dimensional array
 *	\param dims Number of elements in the array.
 *	\param sizes StdVector with size \p dims specifying the size of each dimension.
 *	\note \p T must have a default constructor
 */
template<class T>
Array<T>::Array(const int size)
{
	this->dims = 1;
	this->sizes = new int[1];
	sizes[0] = size;
	this->data = new T[size];
}

/**
 *	\param dims Number of dimensions of the array.
 *	\param sizes StdVector with size \p dims specifying the size of each dimension.
 *	\note \p T must have a default constructor
 */
template<class T>
Array<T>::Array(const int dims, const int *sizes)
{
	this->dims = dims;
	this->sizes = new int[dims];
	std::copy(sizes, sizes + dims, this->sizes);
	int sd = getSize();
	this->data = new T[sd];
}

template<class T>
Array<T>::Array(const std::initializer_list<T>& i)
{
	this->dims = 1;
	this->sizes = new int[1];
	*this->sizes = std::distance(i.begin(),i.end());
	this->data = new T[*this->sizes];
	std::copy(i.begin(),i.end(),this->data);
}

template<class T>
void Array<T>::initialize(const int dims, const int *sizes, const T* data)
{
	this->dims = dims;
	this->sizes = new int[dims];
	std::copy(sizes, sizes + dims, this->sizes);
	int sd = getSize();
	this->data = new T[sd];
	for (int i = 0; i < sd; ++i)
		this->data[i] = data[i];
}

/**
 *	\param dims Number of dimensions of the array.
 *	\param sizes StdVector with size \p dims specifying the size of each dimension.
 *	\param data Pointer to a contiguous block of elements to store in the array.
 *	\note All vector arguments are copied to internal data structures (no
 *	pointers to user data are kept)
 */
template<class T>
Array<T>::Array(const int dims, const int *sizes, const T* data)
{ 	initialize(dims,sizes,data);	}

/**
 *  Copy (by value) constructor.
 */
template<class T>
Array<T>::Array(const Array& s)
{ initialize(s.dims,s.sizes,s.data);	}

template<class T>
Array<T>::~Array()
{
	delete[] sizes;
	delete[] data;
}

template<class T>
int Array<T>::getSize() const
{
	int sd = 1;
	for (int i = 0; i < dims; ++i)
		sd *= sizes[i];
	return sd;
}

template<class>
struct InsertIterator;

template<class T,class C>
struct InsertIterator<Set<T,C> >
{
  protected:
     Set<T,C>& container;
  public:
     explicit InsertIterator(Set<T,C>& s) : container(s) {}
     InsertIterator& operator=(const T& value)
     { container.add(value); return *this; }
     InsertIterator& operator* () { return *this; }
     InsertIterator& operator++ () { return *this; }
     InsertIterator operator++ (int) { return *this; }
};

template <class Container>
InsertIterator<Container> inserter(Container& s)
{ return InsertIterator<Container>(s); }



}


}
}

#endif /* CASPER_BINDINGS_CPP_DETAIL_H_ */
