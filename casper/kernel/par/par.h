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
 

#ifndef CASPER_KERNEL_PAR_PAR_H_
#define CASPER_KERNEL_PAR_PAR_H_

#include <casper/util/pimpl.h>

namespace Casper {

/**
 *	Interface to a delayed expression.
 *
 *  \ingroup Expressions
 */
template<class T>
struct IConstPar
{
	/// Returns the current evaluation of the expression.
	virtual T 		value() const = 0;
};

template<class,class> struct ConstParView;

/**
 * A Par object is like a typed version of a logical variable. It provides copy-by-reference
 * and reversible policies to plain c++ types.
 */
template<class T>
struct Par : Util::PImplIdiom<Reversible<IConstPar<T>*> >
{
	public:

	typedef T						Eval;
	typedef Par<Eval> 				Self;

	typedef IConstPar<T> Iface;
	typedef Reversible<Iface*> 	RPIface;
	typedef Util::PImplIdiom<Reversible<IConstPar<T>*> > Super;

	/// The default constructor
	Par(State& state) :
		Super(new (state) RPIface(state,new (state) ConstParView<Eval,Eval>(state,Eval())))
	{}

	/// Builds a new parameter pointing to the data of \a s.
	Par(const Self& s) : Super(s) { }

	/// Constructor from a generic object.
	template<class T1>
	Par(State& state, const T1& t) :
		Super(new (state) RPIface(state, new (state) ConstParView<Eval,T1>(state,t))) {}

	~Par() {}

	/// Returns the value of the expression
	Eval value() const {	assert(Super::getPImpl()!=NULL); return Super::getImpl().get()->value();	}

	template<class T1>
	const Self& operator=(const T1& t)
	{
		Super::getImpl() = new (getState()) ConstParView<Eval,T1>(getState(),t);
		return *this;
	}

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
//	void attachOnUpdate(INotifiable*	f) { updateSL.attach(f); }
//	void detachOnUpdate(INotifiable*	f) { updateSL.detach(f); }

};

namespace Traits {
template<class T>
struct GetEval<Par<T> >
{	typedef	T	Type;	};
} // Traits


typedef Par<int>	IntPar;
typedef Par<bool>	BoolPar;
typedef Par<double>	DoublePar;

} // Casper

template<class T>
std::ostream& operator<<(std::ostream& os, const Casper::Par<T>& v)
{
	os << v.value();
	return os;
}

#endif /* CASPER_KERNEL_PAR_PAR_H_ */


#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_KERNEL_PAR_OP_PAR_H
#define CASPER_KERNEL_PAR_OP_PAR_H
#include <casper/kernel/par/op_par.h>
#endif
#endif

