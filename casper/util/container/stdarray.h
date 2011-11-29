 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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
 * Multidimensional generic array object.
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_STDARRAY
#define _H_CASPER_KERNEL_CONTAINER_STDARRAY

#include <casper/util/container/stdvector.h>

namespace Casper {

template<class,class,class>
struct Rel2;

struct Element;

namespace Util {


template<class,int>
struct StdArray;

namespace Detail {

template<class,int>
class ArrayTraits;

template<class,int>
struct Array2Traits;

template<class T, uint dims>
class ArrayDataTraits;

template<class T>
class ArrayDataTraits<T,1>
{
	public:
	typedef typename T::Data	Data;
	typedef typename T::Value	Value;

	// 1 args, uninitialized
	// FIXME: should initialize with Value() ?
	static Data* initData(IHeap& heap,uint p1)
	{	return new (heap) Data(heap,p1);  }

	// 2 args, last is initializer
	template<class T2>
	static Data* initData(IHeap& heap,uint p1,T2& p2)
	{
		Data* ret = initData(heap,p1);
		for (uint i = 0; i < p1; i++)
			T::initCell(&(*ret)[i],p2);
		return ret;
	}
	// 3 args, last is initializer
	template<class T2,class T3>
	static Data* initData(IHeap& heap,uint p1,T2& p2,T3& p3)
	{
		Data* ret = initData(heap,p1);
		for (uint i = 0; i < p1; i++)
			T::initCell(&(*ret)[i],p2,p3);
		return ret;
	}
	// 4 args, last is initializer
	template<class T2,class T3,class T4>
	static Data* initData(IHeap& heap,uint p1,T2& p2,T3& p3,T4& p4)
	{
		Data* ret = initData(heap,p1);
		for (uint i = 0; i < p1; i++)
			T::initCell(&(*ret)[i],p2,p3,p4);
		return ret;
	}
	// 5 args, last is initializer
	template<class T2,class T3,class T4,class T5>
	static Data* initData(IHeap& heap,uint p1,T2& p2,T3& p3,T4& p4,T5& p5)
	{
		Data* ret = initData(heap,p1);
		for (uint i = 0; i < p1; i++)
			T::initCell(&(*ret)[i],p2,p3,p4,p5);
		return ret;
	}
};

template<class T>
class ArrayDataTraits<T,2>
{
	public:

	typedef typename T::Data	Data;
	typedef typename T::Value	Value;

	// 2 args, uninitialized
	// FIXME: should initialize with Value() ?
	static Data* initData(IHeap& heap,uint p1, uint p2)
	{
		typedef typename T::Elem	Elem;
		Data* ret = new (heap) Data(heap,p1);
		for (uint i = 0; i < p1; i++)
			::new(&(*ret)[i]) Elem(heap,p2);
		return ret;
	}
	// 3 args, last is initializer
	template<class T2>
	static Data* initData(IHeap& heap,uint p1,uint p2,T2& p3)
	{
		Data* ret = initData(heap,p1,p2);
		for (uint i1 = 0; i1 < p1; i1++)
			for (uint i2 = 0; i2 < p2; i2++)
				T::initCell(&(*ret)[i1][i2],p3);
		return ret;
	}
	// 4 args, last 2 are initializers
	template<class T2,class T3>
	static Data* initData(IHeap& heap,uint p1,uint p2,T2& p3,T3& p4)
	{
		Data* ret = initData(heap,p1,p2);
		for (uint i1 = 0; i1 < p1; i1++)
			for (uint i2 = 0; i2 < p2; i2++)
				T::initCell(&(*ret)[i1][i2],p3,p4);
		return ret;
	}
	// 5 args, last 3 are initializers
	template<class T3,class T4,class T5>
	static Data* initData(IHeap& heap,uint p1,uint p2,T3& p3,T4& p4,T5& p5)
	{
		Data* ret = initData(heap,p1,p2);
		for (uint i1 = 0; i1 < p1; i1++)
			for (uint i2 = 0; i2 < p2; i2++)
				T::initCell(&(*ret)[i1][i2],p3,p4,p5);
		return ret;
	}
};

template<class T>
class ArrayDataTraits<T,3>
{
	public:

	typedef typename T::Data	Data;
	typedef typename T::Value	Value;

	// 3 args, uninitialized
	static Data* initData(IHeap& heap,uint p1, uint p2, uint p3)
	{
		typedef typename T::Elem	Elem;
		Data* ret = new (heap) Data(heap,p1);
		for (uint i = 0; i < p1; i++)
			::new(&(*ret)[i]) Elem(heap,p2,p3);
		return ret;
	}

	// 4 args, last is initializer
	template<class T2>
	static Data* initData(IHeap& heap,uint p1,uint p2,uint p3, T2& p4)
	{
		Data* ret = initData(heap,p1,p2,p3);
		for (uint i1 = 0; i1 < p1; i1++)
			for (uint i2 = 0; i2 < p2; i2++)
				for (uint i3 = 0; i3 < p3; i3++)
					T::initCell(&(*ret)[i1][i2][i3],p4);

		return ret;
	}

	// 5 args, last 2 are initializers
	template<class T2,class T3>
	static Data* initData(IHeap& heap,uint p1,uint p2,uint p3,T2& p4,T3& p5)
	{
		Data* ret = initData(heap,p1,p2,p3);
		for (uint i1 = 0; i1 < p1; i1++)
			for (uint i2 = 0; i2 < p2; i2++)
				for (uint i3 = 0; i3 < p3; i3++)
					T::initCell(&(*ret)[i1][i2][i3],p4,p5);
		return ret;
	}
	// 5 args, last 3 are initializers
	template<class T3,class T4,class T5>
	static Data* initData(IHeap& heap,uint p1,uint p2,uint p3,T3& p4,T4& p5,T5& p6)
	{
		Data* ret = initData(heap,p1,p2,p3);
		for (uint i1 = 0; i1 < p1; i1++)
			for (uint i2 = 0; i2 < p2; i2++)
				for (uint i3 = 0; i3 < p3; i3++)
					T::initCell(&(*ret)[i1][i2][i3],p4,p5,p6);
		return ret;
	}
};

} // Detail

/**
 * 	Multidimensional generic array.
 * \ingroup Containers
 */
template<class T,
		 int dims = 1>
struct StdArray
{
	typedef StdArray<T,dims>										Self;
	typedef typename Detail::ArrayTraits<T,dims-1>::Type		Elem;
	typedef typename Detail::ArrayTraits<T,dims-1>::TermType	TermElem;
	typedef StdVector<Elem> 									Data;
	typedef Data* 												PData;
	typedef typename Data::Iterator								Iterator;
	typedef typename Data::ConstIterator						ConstIterator;
	typedef Elem	Value;

	/// Creates a new array pointing to memory of the existing array \p s.
	StdArray(const StdArray& s) : pData(s.pData),mustFree(false) { }

	/// Creates a new array pointing to \p pData.
	StdArray(PData pData) : pData(pData),mustFree(false) {}

	/// Creates a new array initialized with the range [b,e[
	StdArray(IHeap& heap, Iterator b, Iterator e) :
			pData(new (heap) Data(heap,b,e)),mustFree(!heap.doesGC()) {}

	/// Creates a new array with a copy of the existing array \p s.
	StdArray(IHeap& heap,const StdArray& s) :
		pData(new (heap) Data(heap,s.begin(),s.end())),mustFree(!heap.doesGC()) {}

	/// Creates a new (uninitialized) array with room for \p n elements.
	StdArray(IHeap& heap,uint n) :
			pData(new (heap) Data(heap,n)),mustFree(!heap.doesGC())	{}

	/** Creates a new array with \p p1 elements and initializes each element
	 *  with the parameter \p p2.	*/
	template<class T2>
	StdArray(IHeap& heap,uint p1,const T2& p2)  :
			pData(Detail::ArrayDataTraits<Self,dims>::initData(heap,p1,p2)),
			mustFree(!heap.doesGC())
	{}

	/** Creates a new array with \p p1 elements and initializes each element
	 *  with the parameter \p p2.	*/
	template<class T2>
	StdArray(IHeap& heap,uint p1, T2& p2)  :
			pData(Detail::ArrayDataTraits<Self,dims>::initData(heap,p1,p2)),
			mustFree(!heap.doesGC())
	{}

	/** Creates a new 2D array with \p p1 X \p p2 elements and initializes
	 *  each element with the parameter \p p3.	*/
	template<class T2,class T3>
	StdArray(IHeap& heap,uint p1,const T2& p2,const T3& p3)  :
			pData(Detail::ArrayDataTraits<Self,dims>::initData(heap,p1,p2,p3)),
			mustFree(!heap.doesGC())
	{}

	/** Creates a new 2D array with \p p1 X \p p2 elements and initializes
	 *  each element with the parameter \p p3.	*/
	template<class T2,class T3>
	StdArray(IHeap& heap,uint p1,const T2& p2,T3& p3)  :
			pData(Detail::ArrayDataTraits<Self,dims>::initData(heap,p1,p2,p3)),
			mustFree(!heap.doesGC())
	{}

	/** Creates a new 3D array with \p p1 X \p p2 X \p p3 elements and
	 *  initializes each element with the parameter \p p4.	*/
	template<class T2,class T3,class T4>
	StdArray(IHeap& heap,uint p1,const T2& p2,const T3& p3,const T4& p4) :
		 	pData(Detail::ArrayDataTraits<Self,dims>::initData(heap,p1,p2,p3,p4)),
		 	mustFree(!heap.doesGC())
	{}

	/** Creates a new 3D array with \p p1 X \p p2 X \p p3 elements and
	 *  initializes each element with the parameter \p p4.	*/
	template<class T2,class T3,class T4>
	StdArray(IHeap& heap,uint p1,const T2& p2,const T3& p3,T4& p4) :
		 	pData(Detail::ArrayDataTraits<Self,dims>::initData(heap,p1,p2,p3,p4)),
		 	mustFree(!heap.doesGC())
	{}

	/// Creates a new array initialized with the range [b,e[ in stdHeap.
	StdArray(Iterator b, Iterator e) :
			pData(new (stdHeap) Data(stdHeap,b,e)),
			mustFree(true){}

#ifndef _MSC_VER
	/// Creates a new array from an initializer list
	StdArray(const std::initializer_list<Elem>& l) :
		pData(new (stdHeap) Data(l)),
		mustFree(true) {}
	/// Creates a new array from an initializer list
	StdArray(IHeap& heap,const std::initializer_list<Elem>& l) :
		pData(new (heap) Data(l)),
		mustFree(true) {}
#endif

	/// Creates a new (uninitialized) array with room for \p n elements in stdHeap.
	StdArray(uint n) :
			pData(new (stdHeap) Data(stdHeap,n)),
			mustFree(true) {}

	/** Creates a new array with \p p1 elements in stdHeap and initializes
	 *  each element with the parameter \p p2.	*/
	template<class T2>
	StdArray(uint p1,const T2& p2)  :
			pData(Detail::ArrayDataTraits<Self,dims>::initData(stdHeap,p1,p2)),
			mustFree(true)
	{}

	/** Creates a new 2D array with \p p1 X \p p2 elements in stdHeap and
	 *  initializes each element with the parameter \p p3.	*/
	template<class T3>
	StdArray(uint p1,uint p2,const T3& p3)  :
			pData(Detail::ArrayDataTraits<Self,dims>::initData(stdHeap,p1,p2,p3)),
			mustFree(true)
	{}

	/** Creates a new 3D array with \p p1 X \p p2 X \p p3 elements in
	 *  stdHeap and initializes each element with the parameter \p p4.	*/
	template<class T4>
	StdArray(uint p1,uint p2,uint p3,const T4& p4) :
		 	pData(Detail::ArrayDataTraits<Self,dims>::initData(stdHeap,p1,p2,p3,p4)),
		 	mustFree(true)
	{}

	/// Free memory
	~StdArray()
	{
		if (mustFree)
		{
			IHeap& heap = pData->getHeap();
			pData->~Data();
			heap.deallocate(pData);
		}
		//else
		//	pData->~Data();
	}

	///	Returns an Iterator pointing to the first element of the array.
 	Iterator begin()	{	return pData->begin();	}

 	///	Returns an Iterator pointing to the end of the array.
 	Iterator end()	{	return pData->end();	}

 	///	Returns a ConstIterator pointing to the first element of the array.
 	ConstIterator begin() const	{	return pData->begin();	}

 	///	Returns a ConstIterator pointing to the end of the array.
 	ConstIterator end()	const {	return pData->end();	}

  	/**	Returns the size of a given dimension of the array. The optional
  	 *  argument \p dim specifies the dimension to consider
  	 * (0 is the first dimension). */
	unsigned int size(unsigned int dim = 0) const
	{
		if (dim == 0)
			return pData->size();
		else
			return Detail::ArrayTraits<T,dims-1>::size(*this,dim-1);
	}

	/** Returns the number of cells in the array. The optional
  	 *  argument \p dim specifies the dimension to consider
  	 * (0 is the first dimension). */
	unsigned int count(unsigned int dim = 0) const
	{
		unsigned int ret=1;
		while (dim < dims)
			ret *= size(dim++);
		return ret;
	}

	bool operator==(const Self& s) const
	{	return *pData == *pData;	}

	///	Returns the element at position \p i.
	Elem& operator[](int i)
	{	assert(i >= 0 && i < (int)size()); return pData->operator[](i); }

	///	Returns the element at position \p i.
	const Elem& operator[](int i) const
	{	assert(i >= 0 && i < (int)size()); return pData->operator[](i); }

	///	Returns the element at position \p i.
	Elem& operator[](uint i)
	{	assert(i < size()); return pData->operator[](i); }

	///	Returns the element at position \p i.
	const Elem& operator[](uint i) const
	{	assert(i < size()); return pData->operator[](i); }

	// FIXME: dangerous, T1 is too general...
	/** Returns an Element relation involving the array, and the index value
		\a t*/
	template<class T1>
	Rel2<Element,Self,T1>	operator[](const T1& t) const
	{	return	Rel2<Element,Self,T1>(*this,t);	}

	///	Returns \a true if the size of the array is 0.
	bool empty() const {	return pData->empty();	}

	///	Sets all elements of the array to \p value.
	const Self& operator=(const T& value)
	{
		int _size = (int)size();
		for (int i = 0; i < _size; i++)
			(*this)[i] = value;
		return *this;
	}

	///	Copies all elements from \s replacing existing elements.
	const Self& operator=(const Self& s)
	{
		if (mustFree)
			delete pData;
		pData = new (s.getHeap()) Data(s.getHeap(),s.begin(),s.end());
		return *this;
	}

	/// Returns element at position \p i in the last level, using row major
	/// index ordering.
	const T& operator()(int i) const
	{	return Detail::ArrayTraits<T,dims-1>::access(*this,i);	}

	/// Returns element at position \p i in the last level, using row major
	/// index ordering.
	T& operator()(int i)
	{	return Detail::ArrayTraits<T,dims-1>::access(*this,i);	}

	IHeap& getHeap() const {	return pData->getHeap();	}

	protected:
	template<class T1>
	static void initCell(T* mem, T1& p1)	{	::new(mem) T(p1);	}
	template<class T1,class T2>
	static void initCell(T* mem, T1& p1,T2& p2)	{	::new(mem) T(p1,p2);	}
	template<class T1,class T2,class T3>
	static void initCell(T* mem, T1& p1,T2& p2,T3& p3)	{	::new(mem) T(p1,p2,p3);	}
	template<class T1,class T2,class T3,class T4>
	static void initCell(T* mem, T1& p1,T2& p2,T3& p3,T4& p4)	{	::new(mem) T(p1,p2,p3,p4);	}

	friend class Detail::ArrayTraits<T,dims-1>;
	friend class Detail::ArrayDataTraits<Self,dims>;

	PData		pData;
	const bool 	mustFree;
};


namespace Detail {

template<class T, int dims>
class ArrayTraits
{
public:
	typedef StdArray<T,dims>	Type;
	typedef typename ArrayTraits<T,dims-1>::TermType	TermType;

	static unsigned int size(const StdArray<T,dims+1>& a,
					  		 unsigned int dim)
	{	return a[0].size(dim);	}
	static const T& access(const StdArray<T,dims+1>& a,int i)
	{
		unsigned int aux = a.count(1);
		return a[i/aux](i%aux);
	}
	static T& access(StdArray<T,dims+1>& a,int i)
	{
		unsigned int aux = a.count(1);
		return a[i/aux](i%aux);
	}

};

template<class T>
class ArrayTraits<T,0>
{
public:
	typedef T	Type;
	typedef T	TermType;
	static unsigned int size(const StdArray<T,1>&,unsigned int)
	{	assert(0); return 0; }
	static const T& access(const StdArray<T,1>& a,int i)
	{	return a[i];	}
	static T& access(StdArray<T,1>& a,int i)
	{	return a[i];	}

};


} // Detail

typedef StdArray<int>		StdIntArray;
typedef StdArray<int,2>		StdIntArray2;
typedef StdArray<int,3>		StdIntArray3;
typedef StdArray<bool>		StdBoolArray;
typedef StdArray<bool,2>	StdBoolArray2;
typedef StdArray<bool,3>	StdBoolArray3;
typedef StdArray<string>	StdStringArray;

} // Util


} // Casper

template<class T, int I>
std::ostream& operator<<(std::ostream& os, const Casper::Util::StdArray<T,I>& v)
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

template<class T, int I>
std::istream& operator>>(std::istream& is, Casper::Util::StdArray<T,I>& v)
{
	char sep;
	is >> std::ws >> sep;
	assert(sep == '{');
	if (!v.empty())
	{
 		is >> std::ws >> v[0];
		for (unsigned int n = 1; n < v.size(); n++)
		{
			is >> std::ws >> sep >> std::ws >> v[n];
			assert(sep == ',');
		}
	}
	is >> std::ws >> sep;
	assert(sep == '}');
	return is;
}

#endif


