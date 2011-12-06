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

#ifndef _H_CASPER_KERNEL_PARARRAY
#define _H_CASPER_KERNEL_PARARRAY


//using namespace std;

namespace Casper {

template<class> struct Expr;


/**
 * 	Multidimensional array of parameters.
 *  \param T	The type of element.
 *  \param dims	The number of dimensions of the array.
 *  \ingroup DataStructures
 */
template<class T, int dims = 1>
struct ParArray : Util::StdArray<Par<T>,dims>
{
	private:
	typedef ParArray<T,dims>					Self;

	public:
	typedef Util::StdArray<Par<T>,dims>		Super;

	/// Type of element stated in the array.
	typedef  typename Super::Elem		Elem;

	/// Element iterator.
	typedef typename Super::Iterator	Iterator;

	/// Creates a new ParArray pointing to an existing ParArray \p s.
	ParArray(const ParArray& s) : Super(s) { }

	/// Creates a new ParArray pointing to an existing ParArray \p s.
	ParArray(State& state, const Super& s) : Super(state,s) {}

	/** Creates a new ParArray with a copy of variables in the given
	 	range [\p b, \p e[.	*/
	ParArray(State& state, Iterator b, Iterator e) :
			Super(state,b,e) {}

	/// Creates a new ParArray with \p p1 variables.
	ParArray(State& state,uint p1) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(state,p1,state))
		{}

	ParArray(State& state,const Expr<Seq<T> >& expr);

	/** Creates a new ParArray with \p p1 variables, and initializes each
		with the remaining parameter.	*/
	template<class T2>
	ParArray(State& state,uint p1,const T2& p2) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(state,p1,p2,state))
		{}

	/** Creates a new ParArray with \p p1 variables, and initializes each
		with the remaining parameters.	*/
	template<class T2,class T3>
	ParArray(State& state,uint p1,const T2& p2,const T3& p3) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(state,p1,p2,p3,state))
		{}

	/** Creates a new ParArray with \p p1 variables, and initializes each
		with the remaining parameters.	*/
	template<class T2,class T3,class T4>
	ParArray(State& state,uint p1,const T2& p2,const T3& p3,const T4& p4) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(state,p1,p2,p3,p4,state))
		{}

	/** Creates a new ParArray with \p p1 variables, and initializes each
		with the remaining parameters.	*/
	template<class T2,class T3,class T4,class T5>
	ParArray(State& state,uint p1,const T2& p2,const T3& p3,const T4& p4,const T5& p5) :
		Super(Util::Detail::ArrayDataTraits<Self,dims>::initData(state,p1,p2,p3,p4,p5,state))
		{}

	/// Returns the State object associated with this array.
//	State& getState() const {	return state;	}

	private:

	typedef typename Super::Data		Data;

	static void initCell(Par<T>* mem, State& state)
	{	::new(mem) Par<T>(state); }

	template<class T1>
	static void initCell(Par<T>* mem, const T1& p1, State& state)
	{	::new(mem) Par<T>(state,p1); }

	template<class T1,class T2>
	static void initCell(Par<T>* mem, const T1& p1, const T2& p2, State& state)
	{	::new(mem) Par<T>(state,p1,p2);	}

	template<class T1,class T2,class T3>
	static void initCell(Par<T>* mem, const T1& p1, const T2& p2, const T3& p3, State& state)
	{	::new(mem) Par<T>(state,p1,p2,p3);	}

	template<class T1,class T2,class T3,class T4>
	static void initCell(Par<T>* mem, const T1& p1, const T2& p2, const T3& p3, const T4& p4, State& state)
	{	::new(mem) Par<T>(state,p1,p2,p3,p4);	}

	friend class Util::Detail::ArrayTraits<T,dims-1>;
	friend class Util::Detail::ArrayDataTraits<Self,dims>;

	//State& state;
};




// specialization for multidimensional (par)arrays
template<class Eval,int dims>
struct IterationView<ParArray<Eval,dims> >
{
	typedef IterationView<ParArray<Eval,dims> >	Self;
	IterationView(const ParArray<Eval,dims>& v) : idx(0),v(v),max(v.count()) {}
	IterationView(const ParArray<Eval,dims>& v,uint idx) : idx(idx),v(v),max(v.count()) {}
	IterationView(const IterationView& s) : idx(s.idx),v(s.v),max(s.max) {}
	void		iterate()	{	assert(valid()); ++idx;	}
	const Par<Eval>& 	value() const 	{	assert(valid()); return v(idx);	}
	bool		valid() const 	{	return idx < max;	}
//	Self		copy() const {	return Self(*this); }
	uint						idx;
	ParArray<Eval,dims>	v;
	const uint					max;
};

template<class T,int Dims>
struct GetPState<ParArray<T,Dims> >
{	State* operator()(const ParArray<T,Dims>& v)
	{ return &v.getState(); }
};

namespace Traits {

template<class T, int Dims>
struct GetEval<ParArray<T,Dims> >
{	typedef Seq<T>	Type;	};

template<class T,int Dims>
struct GetElem<ParArray<T,Dims> >
{	typedef typename ParArray<T,Dims>::Elem	Type;	};


template<class T,int Dims>
struct GetTermElem<ParArray<T,Dims> >
{	typedef Par<T>	Type;	};

} // Traits


typedef ParArray<int>		IntParArray;
typedef ParArray<bool>		BoolParArray;
typedef ParArray<double>	DoubleParArray;

} // Casper

#endif // _H_CASPER_CP_VARARRAY

/*
#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_CP_REL_OP_VARARRAY_H
#define CASPER_CP_REL_OP_VARARRAY_H
#include <casper/cp/rel/op_vararray.h>
#endif
#endif
*/
