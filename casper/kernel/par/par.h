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

#include <casper/kernel/notify/susplist.h>

namespace Casper {

/**
 * A Par object represents a parameter in a model. It provides copy-by-reference
 * and reversible policies to plain c++ types.
 */
template<class T>
class Par
{
	public:

	typedef T						Data;
	typedef Par<Data> 				Self;


	/// The type of the variable domain
	typedef Reversible<Data>		RData;
	typedef RData*					PRData;

	/// The default constructor
	Par(Env& env) : env(env),
					pData(new (env) RData(env)),
					updateSL(env) {}

	/// Builds a new parameter pointing to the data of \a s.
	Par(const Self& s) : env(s.getEnv()),
							pData(s.pData),
							updateSL(s.updateSL)	{ }

	/// A constructor with one parameter
	Par(Env& env, const T& p1) : env(env),
		pData(new (env) RData(env,p1)),updateSL(env)	{}

	~Par() {}

	Env&		getEnv()	const	{	return env;	}

	/// Returns the value of the variable
	const Data& value() const {return (*pData).get();}

	/// Returns a pointer to the value of the variable
	//PRData& pValue() {return pData;}

	//operator const Data&() const { return value();	}

	//friend ostream& operator<< <>(ostream& os, const Self& f);

	/// Makes the variable domain point to the value of \a s
	bool operator=(const Self& s)
	{
		*pData = s.value();
		return updateSL.notifyAll();
	}

	/// Assign \a s to the variable
	bool operator=(const Data& s)
	{
		*pData = s;
		return updateSL.notifyAll();
	}

	const Self& operator++()
	{	(*pData)=(*pData).get()+1;	return *this;	}


	// no because it takes precedence to constraints posted on variable
	//operator const Data&() const { return *pData; }

	// useful for reversible pointers
	// (const version not necessary because of automatic conversion to const T)
	//Data operator->()
	//{	return *pData;	}

	/// Registers notifiable \a f on value update event
	void attachOnUpdate(INotifiable*	f) { updateSL.attach(f); }
	void detachOnUpdate(INotifiable*	f) { updateSL.detach(f); }

	private:
	Env&				env;
	PRData 				pData;
	EventSuspList		updateSL;
};

namespace Traits {
template<class T>
struct GetEval<Par<T> >
{	typedef	T	Type;	};
} // Traits

template<class T>
struct GetPEnv<Par<T> >
{	Env* operator()(const Par<T>& v)
	{ return &v.getEnv(); }
};

typedef Par<int>	IntPar;
typedef Par<bool>	BoolPar;

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

