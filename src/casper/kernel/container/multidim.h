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

/**
 * \file
 * Multidimensional generic object.
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_MULTIDIM
#define _H_CASPER_KERNEL_CONTAINER_MULTIDIM

#include <casper/kernel/container/vector.h>

using namespace std;

namespace casper {
namespace detail {

template<class>
struct MultiDim;

template<class,int>
class MultiDimTraits;

//template<class,int>
//struct MultiDim2Traits;

template<class T, UInt dims>
class MultiDimDataTraits;

template<class T>
class MultiDimDataTraits<T,1>
{
	public:
	typedef typename T::Data	Data;

	// 1 args, uninitialized
	static Data* initData(Heap heap,UInt p1)
	{	return new (heap) Data(heap,p1);  }
	// 1 args, uninitialized, solver is needed
	static Data* initData(ICPSolver& solver,UInt p1)
	{	return new (solver.heap()) Data(solver.heap(),p1);  }

	// 2 args, last is initializer
	template<class T0,class T2>
	static Data* initData(T0 heap,UInt p1,T2 p2)
	{
		Data* ret = initData(heap,p1);
		for (UInt i = 0; i < p1; i++)
			T::Derived::initCell(&(*ret)[i],p2);
		return ret;
	}
	// 3 args, last is initializer
	template<class T0,class T2,class T3>
	static Data* initData(T0 heap,UInt p1,T2 p2,T3 p3)
	{
		Data* ret = initData(heap,p1);
		for (UInt i = 0; i < p1; i++)
			T::Derived::initCell(&(*ret)[i],p2,p3);
		return ret;
	}
	// 4 args, last is initializer
	template<class T0,class T2,class T3,class T4>
	static Data* initData(T0 heap,UInt p1,T2& p2,T3& p3,T4& p4)
	{
		Data* ret = initData(heap,p1);
		for (UInt i = 0; i < p1; i++)
			T::Derived::initCell(&(*ret)[i],p2,p3,p4);
		return ret;
	}
	// 5 args, last is initializer
	template<class T0,class T2,class T3,class T4,class T5>
	static Data* initData(T0 heap,UInt p1,T2 p2,T3 p3,T4 p4,T5 p5)
	{
		Data* ret = initData(heap,p1);
		for (UInt i = 0; i < p1; i++)
			T::Derived::initCell(&(*ret)[i],p2,p3,p4,p5);
		return ret;
	}

};

template<class T>
class MultiDimDataTraits<T,2>
{
	public:

	typedef typename T::Data	Data;

	// 2 args, uninitialized
	static Data* initData(Heap heap,UInt p1, UInt p2)
	{
		typedef typename T::Elem	Elem;
		Data* ret = new (heap) Data(heap,p1);
		for (UInt i = 0; i < p1; i++)
			::new(&(*ret)[i]) Elem(heap,p2);
		return ret;
	}

	// 2 args, uninitialized, ICPSolver& is needed
	static Data* initData(ICPSolver& solver,UInt p1, UInt p2)
	{
		typedef typename T::Elem	Elem;
		Data* ret = new (solver.heap()) Data(solver.heap(),p1);
		for (UInt i = 0; i < p1; i++)
			::new(&(*ret)[i]) Elem(solver,p2);
		return ret;
	}

	// 3 args, last is initializer
	template<class T0,class T2>
	static Data* initData(T0 heap,UInt p1,UInt p2,T2& p3)
	{
		Data* ret = initData(heap,p1,p2);
		for (UInt i1 = 0; i1 < p1; i1++)
			for (UInt i2 = 0; i2 < p2; i2++)
				T::Derived::initCell(&(*ret)[i1][i2],p3);
		return ret;
	}

	// 3 args, uninitialized
	template<class T0>
	static Data* initData(T0 heap,UInt p1,UInt p2,ICPSolver& p3)
	{
		Data* ret = initData(heap,p1,p2);
		for (UInt i1 = 0; i1 < p1; i1++)
			for (UInt i2 = 0; i2 < p2; i2++)
				T::Derived::initCell(&(*ret)[i1][i2],p3);
		return ret;
	}

	// 4 args, last 2 are initializers
	template<class T0,class T2,class T3>
	static Data* initData(T0 heap,UInt p1,UInt p2,T2 p3,T3 p4)
	{
		Data* ret = initData(heap,p1,p2);
		for (UInt i1 = 0; i1 < p1; i1++)
			for (UInt i2 = 0; i2 < p2; i2++)
				T::Derived::initCell(&(*ret)[i1][i2],p3,p4);
		return ret;
	}
	// 5 args, last 3 are initializers
	template<class T0,class T3,class T4,class T5>
	static Data* initData(T0 heap,UInt p1,UInt p2,T3 p3,T4 p4,T5 p5)
	{
		Data* ret = initData(heap,p1,p2);
		for (UInt i1 = 0; i1 < p1; i1++)
			for (UInt i2 = 0; i2 < p2; i2++)
				T::Derived::initCell(&(*ret)[i1][i2],p3,p4,p5);
		return ret;
	}
};

template<class T>
class MultiDimDataTraits<T,3>
{
	public:

	typedef typename T::Data	Data;

	// 3 args, uninitialized
	static Data* initData(Heap heap,UInt p1, UInt p2, UInt p3)
	{
		typedef typename T::Elem	Elem;
		Data* ret = new (heap) Data(heap,p1);
		for (UInt i = 0; i < p1; i++)
			::new(&(*ret)[i]) Elem(heap,p2,p3);
		return ret;
	}
	// 3 args, uninitialized, ICPSolver&
	static Data* initData(ICPSolver& solver,UInt p1, UInt p2, UInt p3)
	{
		typedef typename T::Elem	Elem;
		Data* ret = new (solver.heap()) Data(solver.heap(),p1);
		for (UInt i = 0; i < p1; i++)
			::new(&(*ret)[i]) Elem(solver,p2,p3);
		return ret;
	}

	// 4 args, last is initializer
	template<class T0,class T2>
	static Data* initData(T0 heap,UInt p1,UInt p2,UInt p3, T2& p4)
	{
		Data* ret = initData(heap,p1,p2,p3);
		for (UInt i1 = 0; i1 < p1; i1++)
			for (UInt i2 = 0; i2 < p2; i2++)
				for (UInt i3 = 0; i3 < p3; i3++)
					T::Derived::initCell(&(*ret)[i1][i2][i3],p4);
		return ret;
	}
	// 5 args, last 2 are initializers
	template<class T0,class T2,class T3>
	static Data* initData(T0 heap,UInt p1,UInt p2,UInt p3,T2 p4,T3 p5)
	{
		Data* ret = initData(heap,p1,p2,p3);
		for (UInt i1 = 0; i1 < p1; i1++)
			for (UInt i2 = 0; i2 < p2; i2++)
				for (UInt i3 = 0; i3 < p3; i3++)
					T::Derived::initCell(&(*ret)[i1][i2][i3],p4,p5);
		return ret;
	}
	// 6 args, last 3 are initializers
	template<class T0,class T3,class T4,class T5>
	static Data* initData(T0 heap,UInt p1,UInt p2,UInt p3,T3 p4,T4 p5,T5 p6)
	{
		Data* ret = initData(heap,p1,p2,p3);
		for (UInt i1 = 0; i1 < p1; i1++)
			for (UInt i2 = 0; i2 < p2; i2++)
				for (UInt i3 = 0; i3 < p3; i3++)
					T::Derived::initCell(&(*ret)[i1][i2][i3],p4,p5,p6);
		return ret;
	}
	// 7 args, last 4 are initializers
	template<class T0,class T3,class T4,class T5,class T6>
	static Data* initData(T0 heap,UInt p1,UInt p2,UInt p3,T3 p4,T4 p5,T5 p6,T6 p7)
	{
		Data* ret = initData(heap,p1,p2,p3);
		for (UInt i1 = 0; i1 < p1; i1++)
			for (UInt i2 = 0; i2 < p2; i2++)
				for (UInt i3 = 0; i3 < p3; i3++)
					T::Derived::initCell(&(*ret)[i1][i2][i3],p4,p5,p6,p7);
		return ret;
	}
};


// --- here
#ifndef SWIG
// NOT to be specialized by md derived classes
template<class Traits,int dims>
struct MDTraits
{
	typedef typename Traits::template Rebind<dims-1>::Type		Elem;
	typedef Vector<Elem>							Data;
};

template<class Traits>
struct MDTraits<Traits,1>
{
	typedef typename Traits::Term			Elem;
	typedef Vector<Elem>	Data;
};
#endif

/**
 * 	Multidimensional generic object.
 */
template<class Traits>
struct MultiDim
{
 	static const  int	dims = Traits::dims;
	typedef typename Traits::Derived				Derived;
	typedef MultiDim								Self;
	typedef typename MDTraits<Traits,dims>::Data	Data;
	typedef Data* 									PData;
	typedef typename MDTraits<Traits,dims>::Elem	Elem;
	typedef typename Data::Iterator					Iterator;
	typedef typename Data::ConstIterator			ConstIterator;

	/// Creates a new MultiDom pointing to memory of the existing MultiDim \p s.
	MultiDim(const MultiDim& s) : pData(s.pData) { }

	/// Creates a new MultiDim pointing to \p pData.
	MultiDim(PData pData) : pData(pData) {}

	/// Creates a new MultiDim initialized with the range [b,e[
	MultiDim(Heap heap, Iterator b, Iterator e) :
			pData(new (heap) Data(heap,b,e)) {}
	//MultiDim(ICPSolver& solver, Iterator b, Iterator e) :
	//		pData(new (solver.heap()) Data(solver,b,e)) {}

	/// Creates a new (uninitialized) MultiDim with room for \p n elements.
	MultiDim(Heap heap,UInt n) :
			pData(new (heap) Data(heap,n))	{}
	//MultiDim(ICPSolver& solver,UInt n) :
	//		pData(new (solver.heap()) Data(solver.heap(),n))	{}

	/** Creates a new MultiDim with \p p1 elements and initializes each element
	 *  with the parameter \p p2.	*/
	template<class T0,class T2>
	MultiDim(T0 heap,UInt p1,T2 p2)  :
			pData(MultiDimDataTraits<Self,dims>::initData(heap,p1,p2))
	{}

	/** Creates a new 2D MultiDim with \p p1 X \p p2 elements and initializes
	 *  each element with the parameter \p p3.	*/
	template<class T0,class T2,class T3>
	MultiDim(T0 heap,UInt p1,T2 p2,T3 p3)  :
			pData(MultiDimDataTraits<Self,dims>::initData(heap,p1,p2,p3))
	{}

	/** Creates a new 3D MultiDim with \p p1 X \p p2 X \p p3 elements and
	 *  initializes each element with the parameter \p p4.	*/
	template<class T0,class T2,class T3,class T4>
	MultiDim(T0 heap,UInt p1,T2 p2,T3 p3,T4 p4) :
		 	pData(MultiDimDataTraits<Self,dims>::initData(heap,p1,p2,p3,p4))
	{}

	/** Creates a new 3D MultiDim with \p p1 X \p p2 X \p p3 elements and
	 *  initializes each element with the parameter \p p4.	*/
	template<class T0,class T2,class T3,class T4,class T5>
	MultiDim(T0 heap,UInt p1,T2 p2,T3 p3,T4 p4,T5 p5) :
		 	pData(MultiDimDataTraits<Self,dims>::initData(heap,p1,p2,p3,p4,p5))
	{}

	/** Creates a new 3D MultiDim with \p p1 X \p p2 X \p p3 elements and
	 *  initializes each element with the parameters \p p4 \p p5.	*/
	template<class T0,class T2,class T3,class T4,class T5,class T6>
	MultiDim(T0 heap,UInt p1,T2 p2,T3 p3,T4 p4,T5 p5,T6 p6) :
		 	pData(MultiDimDataTraits<Self,dims>::initData(heap,p1,p2,p3,p4,p5,p6))
	{}

	/// Creates a new MultiDim initialized with the range [b,e[ in stdHeap.
	MultiDim(Iterator b, Iterator e) :
			pData(new (stdHeap) Data(stdHeap,b,e)) {}

	/// Creates a new (uninitialized) MultiDim with room for \p n elements in stdHeap.
	MultiDim(UInt n) : MultiDim(stdHeap,n) {}

	/** Creates a new MultiDim with \p p1 elements in stdHeap and initializes
	 *  each element with the parameter \p p2.	*/
	template<class T2>
	MultiDim(UInt p1,T2 p2)  : MultiDim(stdHeap,p1,p2) {}

	/** Creates a new 2D MultiDim with \p p1 X \p p2 elements in stdHeap and
	 *  initializes each element with the parameter \p p3.	*/
	template<class T3>
	MultiDim(UInt p1,UInt p2,T3 p3)  : MultiDim(stdHeap,p1,p2,p3) {}

	/** Creates a new 3D MultiDim with \p p1 X \p p2 X \p p3 elements in
	 *  stdHeap and initializes each element with the parameter \p p4.	*/
	template<class T4>
	MultiDim(UInt p1,UInt p2,UInt p3,T4 p4) : MultiDim(stdHeap,p1,p2,p3,p4) {}

	/**
	 * 	Frees memory.
	 */
	~MultiDim()
	{	// TESTME: not sure it is correct
		pData->~Data();
		heap().deallocate(pData);

/*		if (!heap().doesGC())
		{
			Heap h(heap());
			pData->~Data();
			h.deallocate(pData);
		}
*/	}

	///	Returns an Iterator pointing to the first element of the MultiDim.
 	Iterator begin()	{	return pData->begin();	}

 	///	Returns an Iterator pointing to the end of the MultiDim.
 	Iterator end()	{	return pData->end();	}

 	///	Returns a ConstIterator pointing to the first element of the MultiDim.
 	ConstIterator begin() const	{	return pData->begin();	}

 	///	Returns a ConstIterator pointing to the end of the MultiDim.
 	ConstIterator end()	const {	return pData->end();	}

  	/**	Returns the size of a given dimension of the MultiDim. The optional
  	 *  argument \p dim specifies the dimension to consider
  	 * (0 is the first dimension). */
	unsigned int size(unsigned int dim = 0) const
	{
		if (dim == 0)
			return pData->size();
		else
			return detail::MultiDimTraits<Traits,dims>::size(*this,dim-1);
	}

	/** Returns the number of cells in the MultiDim. The optional
  	 *  argument \p dim specifies the dimension to consider
  	 * (0 is the first dimension). */
	unsigned int count(unsigned int dim = 0) const
	{
		unsigned int ret=1;
		while (dim < (UInt)dims)
			ret *= size(dim++);
		return ret;
	}

	///	Returns the element at position \p i.
	Elem& operator[](Int i)
	{	assert(i >= 0 && i < (Int)size()); return pData->operator[](i); }

	///	Returns the element at position \p i.
	const Elem& operator[](Int i) const
	{	assert(i >= 0 && i < (Int)size()); return pData->operator[](i); }

	///	Returns the element at position \p i.
	Elem& operator[](UInt i)
	{	assert(i < size()); return pData->operator[](i); }

	///	Returns the element at position \p i.
	const Elem& operator[](UInt i) const
	{	assert(i < size()); return pData->operator[](i); }

	///	Returns \a true if the size of the MultiDim is 0.
	Bool empty() const {	return pData->empty();	}

	///	Sets all elements of the MultiDim to \p value.
	const Self& operator=(const Derived& value)
	{
		Int _size = (Int)size();
		for (Int i = 0; i < _size; i++)
			(*this)[i] = value;
		return *this;
	}

	///	Copies all elements from \s replacing existing elements.
	const Self& operator=(const Self& s)
	{	*pData = *s.pData;	return *this;	}

	/// Returns element at position \p i in the last level, using row major
	/// index ordering.
	const typename Traits::Term& operator()(int i) const
	{	return detail::MultiDimTraits<Traits,dims>::access(*this,i);	}

	Heap heap() const {	return pData->heap();	}

	protected:
	static void initCell(Derived* mem, Derived val)
	{	 *mem = val; /*::new(mem) T(val);*/	}
	friend class detail::MultiDimTraits<Traits,dims>;
	friend class detail::MultiDimDataTraits<Self,dims>;

	PData	pData;
};

//template<class Traits>
//static const int MultiDim<Traits>::dims = Traits::dims;

template<class Traits,int dims>
class MultiDimTraits
{
public:
	static unsigned int size(const MultiDim<Traits>& a,
					  		 unsigned int dim)
	{	return a[0].size(dim);	}
	static const typename Traits::Term&
		access(const MultiDim<Traits>& a,
						 int i)
	{
		unsigned int aux = a.count(1);
		return a[i/aux](i%aux);
	}
};

template<class Traits>
class MultiDimTraits<Traits,1>
{
public:
	static unsigned int size(const MultiDim<Traits>&,unsigned int)
	{	assert(0); return 0; }
	static const typename Traits::Term&
		access(const MultiDim<Traits>& a,int i)
	{	return a[i];	}
};

template<class Traits>
std::ostream& operator<<(std::ostream& os, const casper::detail::MultiDim<Traits>& v)
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

template<class Traits>
std::istream& operator>>(std::istream& is, casper::detail::MultiDim<Traits>& v)
{
	char sep;
	is >> ws >> sep;
	assert(sep == '{');
	if (!v.empty())
	{
 		is >> ws >> v[0];
		for (unsigned int n = 1; n < v.size(); n++)
		{
			is >> ws >> sep >> ws >> v[n];
			assert(sep == ',');
		}
	}
	is >> ws >> sep;
	assert(sep == '}');
	return is;
}

};

};



#endif
