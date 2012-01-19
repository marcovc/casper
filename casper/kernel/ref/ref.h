/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_KERNEL_REF_REF_H_
#define CASPER_KERNEL_REF_REF_H_

#include <casper/util/pimpl.h>
#include <casper/kernel/reversible/reversible.h>
#include <casper/kernel/rel/rel.h>
#include <casper/kernel/notify/susplist.h>
#include <casper/util/container/stdarray.h>

namespace Casper {

template<class> struct Ref;

/** \defgroup DataStructures Data Structures
 * 	\ingroup Kernel
 * 	Kernel data structures.
 */

/**
 *	An IRef subclassed object is like a typed version of a logical variable. It provides copy-by-reference
 *  and reversible policies to plain c++ types.
 *
 *  \ingroup DataStructures
 */
template<class T>
struct IRef
{
	/// Returns the current evaluation of the expression.
	virtual T 		value() const = 0;

	virtual void setValue(const T& value)
	{	throw Casper::Exception::InvalidOperation("cannot set value for current par expression"); }
};

template<class,class> struct RefView;

/**
 * A Ref object is like a typed version of a logical variable. It provides copy-by-reference
 * and reversible policies to plain c++ types.
 * \ingroup DataStructures
 */
template<class T>
struct Ref : Util::PImplIdiom<Reversible<IRef<T>*> >
{
	public:

	typedef T						Eval;
	typedef Ref<Eval> 				Self;

	typedef IRef<T> Iface;
	typedef IRef<T>* PIface;
	typedef Reversible<PIface> 	RPIface;
	typedef Util::PImplIdiom<RPIface> Super;

	/// The default constructor
	Ref(State& state) :
		Super(new (state) RPIface(state,new (state) RefView<Eval,Eval>(state,Eval())))
	{}

	Ref(State& state, PIface pimpl) :
		Super(new (state) RPIface(state,pimpl)) {}

	/// Builds a new parameter pointing to the data of \a s.
	Ref(const Self& s) : Super(s) {}

	/// Constructor from a generic object.
	template<class T1>
	Ref(State& state, const T1& t) :
		Super(new (state) RPIface(state, new (state) RefView<Eval,T1>(state,t))) {}

	~Ref() {}

	/// Returns the value of the expression
	Eval value() const {	assert(Super::getPImpl()!=NULL); return Super::getImpl().get()->value();	}

	/// Updates *this to point at t.
	template<class T1>
	const Self& operator=(const T1& t)
	{
		Super::getImpl() = new (getState()) RefView<Eval,T1>(getState(),t);
		return *this;
	}

	/// Updates *this to point at t.
	const Self& operator=(const Self& t)
	{
		static_cast<Super&>(*this) = t;
		return *this;
	}

	void setValue(const Eval& v)
	{
		assert(Super::getPImpl()!=NULL);
		Super::getImpl().get()->setValue(v);
	}

//	const Self& operator=(const Self& t)
//	{
//		Super::getPImpl() = t.getPImpl();
//		return *this;
//	}

	State& getState() const {	return this->getImpl().getState();	}

	const Self& operator++()
	{	*this = value()+1; return *this;	}

	bool operator==(const Self& other) const
	{	return value() == other.value(); }

	// no because it takes precedence to constraints posted on variable
	//operator const Data&() const { return *pData; }

	// useful for reversible pointers
	// (const version not necessary because of automatic conversion to const T)
	//Data operator->()
	//{	return *pData;	}

	/// Registers notifiable \a f on value update event
//	void attach(INotifiable*	f) { }
//	void detach(INotifiable*	f) { }
};


namespace Traits {
template<class> struct GetEval;
template<class> struct GetElem;

template<class T>
struct GetEval<Ref<T> >
{	typedef	T	Type;	};

template<class T>
struct GetTypeStr<Ref<T> >
{
	std::string operator()()
	{
		return std::string("Casper::Ref<")+getTypeStr<T>()+">";
	}
};

} // Traits

template<class Eval>
struct GetPState<Ref<Eval> >
{	State* operator()(const Ref<Eval>& p) { return &p.getState(); } };

typedef Ref<int>			IntRef;
typedef Ref<bool>			BoolRef;
typedef Ref<double>			DoubleRef;

} // Casper

template<class T>
std::ostream& operator<<(std::ostream& os, const Casper::Ref<T>& v)
{
	os << v.value();
	return os;
}

#endif /* CASPER_KERNEL_REF_REF_H_ */


#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_KERNEL_PAR_OP_REF_H
#define CASPER_KERNEL_PAR_OP_REF_H
#include <casper/kernel/ref/op_ref.h>
#endif
#endif

