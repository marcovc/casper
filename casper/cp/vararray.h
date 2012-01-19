 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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
 * 	\file
 *	Multidimensional array of variables.
 */

#ifndef _H_CASPER_CP_VARARRAY
#define _H_CASPER_CP_VARARRAY

#include <casper/util/container/stdarray.h>
#include <casper/cp/var.h>
//#include <casper/kernel/relation.h>
//#include <casper/kernel/expression.h>
#include <casper/kernel/view/iteration.h>

//using namespace std;

namespace Casper {
namespace CP {

template<class,int,class>
struct VarArray;

#if 0
namespace Detail {
template<class T,int Dims,class Dom>
struct VarArrayTraits
{
	typedef VarArray<T,Dims,Dom>	Derived;
	typedef Var<T,Dom>			Term;
	template<int dims2>
	struct Rebind {
		typedef VarArray<T,dims2,Dom> Type;
	};
	static const int dims = Dims;
};
};
#endif

/**
 * 	Multidimensional array of domain variables.
 *  \param T	The type of element in each domain.
 *  \param dims	The number of dimensions of the array.
 *  \param Dom	The type of the domain.
 *  \ingroup CPDataStructures
 */
template<class T, int dims = 1, class Dom1 = typename Traits::GetDefaultDom<T>::Type>
struct VarArray : Util::StdArray<Var<T,Dom1>,dims>
{
	typedef Dom1	Dom;

	private:
	typedef VarArray<T,dims,Dom>					Self;

	public:
	typedef Util::StdArray<Var<T,Dom1>,dims>		Super;

	/// Type of element stored in the array.
	typedef  typename Super::Elem		Elem;

	/// Element iterator.
	typedef typename Super::Iterator	Iterator;

	/// Creates a new VarArray pointing to an existing VarArray \p s.
	VarArray(const VarArray& s) : Super(s),store(s.store) { }

	/// Creates a new VarArray pointing to an existing VarArray \p s.
	VarArray(Store& store, const Super& s) : Super(s),store(store) {}

	/** Creates a new VarArray with a copy of variables in the given
	 	range [\p b, \p e[.	*/
	VarArray(Store& store, Iterator b, Iterator e) :
			Super(store,b,e),
			store(store) {}

	/// Creates a new VarArray with \p p1 variables.
	VarArray(Store& store,uint p1) : Super(store,p1),store(store)	{}

	/** Creates a new VarArray with \p p1 variables, and initializes each
		domain with the remaining parameter.	*/
	template<class T2>
	VarArray(Store& store,uint p1,const T2& p2) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(store,p1,p2,store)),
		store(store)	{}

	/** Creates a new VarArray with \p p1 variables, and initializes each
		domain with the remaining parameters.	*/
	template<class T2,class T3>
	VarArray(Store& store,uint p1,const T2& p2,const T3& p3) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(store,p1,p2,p3,store)),
		store(store)	{}

	/** Creates a new VarArray with \p p1 variables, and initializes each
		domain with the remaining parameters.	*/
	template<class T2,class T3,class T4>
	VarArray(Store& store,uint p1,const T2& p2,const T3& p3,const T4& p4) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(store,p1,p2,p3,p4,store)),
		store(store)	{}

	/** Creates a new VarArray with \p p1 variables, and initializes each
		domain with the remaining parameters.	*/
	template<class T2,class T3,class T4,class T5>
	VarArray(Store& store,uint p1,const T2& p2,const T3& p3,const T4& p4,const T5& p5) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(store,p1,p2,p3,p4,p5,store)),
		store(store)	{}

	/// Returns the Store object associated with this array.
	Store& getStore() const {	return store;	}

	/// Returns the State object associated with this array.
	State& getState() const {	return store;	}

	/// Returns \a true if all variables are ground, \a false otherwise.
	bool ground() const;

	private:

	typedef typename Super::Data		Data;

	// note: disabled for now
	static void initCell(Var<T,Dom>* mem, Store& store)
	{	/*::new(mem) Var<T,Dom>(solver);*/ }

	template<class T1>
	static void initCell(Var<T,Dom>* mem, const T1& p1, Store& store)
	{	::new(mem) Var<T,Dom>(store,p1); }

	template<class T1,class T2>
	static void initCell(Var<T,Dom>* mem, const T1& p1, const T2& p2, Store& store)
	{	::new(mem) Var<T,Dom>(store,p1,p2);	}

	template<class T1,class T2,class T3>
	static void initCell(Var<T,Dom>* mem, const T1& p1, const T2& p2, const T3& p3, Store& store)
	{	::new(mem) Var<T,Dom>(store,p1,p2,p3);	}

	template<class T1,class T2,class T3,class T4>
	static void initCell(Var<T,Dom>* mem, const T1& p1, const T2& p2, const T3& p3, const T4& p4, Store& store)
	{	::new(mem) Var<T,Dom>(store,p1,p2,p3,p4);	}

	friend class Util::Detail::ArrayTraits<T,dims-1>;
	friend class Util::Detail::ArrayDataTraits<Self,dims>;

	Store& store;
};

template<class T, int dims, class Dom>
bool VarArray<T,dims,Dom>::ground() const
{
	for (uint i = 0; i < this->count(); i++)
		if (!this->operator()(i).ground())
			return false;
	return true;
}

} // CP

// specialization for multidimesional (domvar)arrays
template<class Eval,int dims,class Dom>
struct IterationView<CP::VarArray<Eval,dims,Dom> >
{
	typedef IterationView<CP::VarArray<Eval,dims,Dom> >	Self;
	IterationView(const CP::VarArray<Eval,dims,Dom>& v) : idx(0),v(v),max(v.count()) {}
	IterationView(const CP::VarArray<Eval,dims,Dom>& v,uint idx) : idx(idx),v(v),max(v.count()) {}
	IterationView(const IterationView& s) : idx(s.idx),v(s.v),max(s.max) {}
	void		iterate()	{	assert(valid()); ++idx;	}
	const CP::Var<Eval,Dom>& 	value() const 	{	assert(valid()); return v(idx);	}
	bool		valid() const 	{	return idx < max;	}
	uint						idx;
	CP::VarArray<Eval,dims,Dom>	v;
	const uint					max;
};


template<class T,int Dims,class Dom>
struct GetPState<CP::VarArray<T,Dims,Dom> >
{	State* operator()(const CP::VarArray<T,Dims,Dom>& v)
	{ return &v.getState(); }
};

namespace Traits {

template<class T,int Dims,class Dom>
struct GetElem<CP::VarArray<T,Dims,Dom> >
{	typedef typename CP::VarArray<T,Dims,Dom>::Elem	Type;	};


template<class T,int Dims,class Dom>
struct GetTermElem<CP::VarArray<T,Dims,Dom> >
{	typedef CP::Var<T,Dom>	Type;	};

template<class T, int Dims, class Dom>
struct GetEval<CP::VarArray<T,Dims,Dom> >
{	typedef Seq<T>	Type;	};

template<class T,int dims,class D>
struct GetTypeStr<CP::VarArray<T,dims,D> >
{
	std::string operator()()
	{
		std::stringstream ss;
		ss << dims;
		return std::string("Casper::CP::VarArray<")+getTypeStr<T>()+","+ss.str()+">";
	}
};

} // Traits

} // Casper

#endif // _H_CASPER_CP_VARARRAY

#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_CP_REL_OP_VARARRAY_H
#define CASPER_CP_REL_OP_VARARRAY_H
#include <casper/cp/rel/op_vararray.h>
#endif
#endif
