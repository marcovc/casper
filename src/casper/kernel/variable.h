 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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
 *	Domain variables.
 */

#ifndef _H_CASPER_KERNEL_VARIABLE
#define _H_CASPER_KERNEL_VARIABLE

#include <iostream>

#include <casper/kernel/common.h>
#include <casper/kernel/reversible.h>
#include <casper/kernel/susplist.h>

namespace casper {

namespace detail {

template<class T>
struct VarDomCreator
{
	T* operator()(ICPSolver& e)
	{ return new (e.heap()) T(); }

	T* operator()(ICPSolver& e,const T& s)
	{ return new (e.heap()) T(s);	}

	T* operator()(ICPSolver& e,T& s)
	{ return new (e.heap()) T(s);	}
};
};

using namespace std;
/*
template<class,class>
class DomVar;

template<class T,class Dom>
ostream& operator<<(ostream& os, const DomVar<T,Dom>& f);
*/
template<class T>
struct DefaultDomType;

/**
 * 	A domain variable holds a pointer to a domain of type \p DomT, and when
 *  instantiated will represent a value of type \p T.
 *  \ingroup Kernel
 */
template<class T,
		 class DomT = typename DefaultDomType<T>::Type>
class DomVar
{
	typedef T						Type;
	typedef DomVar<Type,DomT> 		Self;

	public:

	/// The type of the variable domain
	typedef DomT					Dom;
	typedef Dom*					PDom;
	typedef Reversible<PDom>		RPDom;

	/// Creates a new DomVar with a default domain.
	DomVar(ICPSolver& solver) : pDom(solver,detail::VarDomCreator<Dom>()(solver)) {}

	/// Creates a new variable pointing to the domain of \a s.
	DomVar(const DomVar<T,DomT>& s) : pDom(s.pDom)	{ }

#ifndef SWIG
	/// Creates a new variable pointing to the domain of \a s.
	/// \pre Dom1 is convertible to DomT
	template<class T1,class Dom1>
	DomVar(const DomVar<T1,Dom1>& s) : pDom(s.solver(),
									 static_cast<Dom*>(s.pDomain()))	{ }
#endif

	/// Creates a new variable with the domain pointing to \a s.
	DomVar(ICPSolver& solver,PDom s) : pDom(solver,s)	{ }

	/// Creates a new variable and initializes its domain with parameter \p p1.
	template<class T1>
	DomVar(ICPSolver& solver, const T1& p1) :
		pDom(solver,detail::VarDomCreator<Dom>()(solver,p1))	{}

	/** Creates a new variable and initializes its domain with parameters \p p1
	 *  and \p p2.	*/
	template<class T1,class T2>
	DomVar(ICPSolver& solver, const T1& p1,const T2& p2) :
		pDom(solver,detail::VarDomCreator<Dom>()(solver,p1,p2)) {}

	/** Creates a new variable and initializes its domain with parameters \p p1,
	 *  \p p2 and \p p3.	*/
	template<class T1,class T2,class T3>
	DomVar(ICPSolver& solver, const T1& p1,const T2& p2,const T3& p3) :
		pDom(solver,detail::VarDomCreator<Dom>()(solver,p1,p2,p3)) {}

	// TODO: more constructors

	/** Creates a new variable and initializes its domain with parameters \p p1,
	 *  \p p2,\p p3,\p p4.	*/
	template<class T1,class T2,class T3,class T4>
	DomVar(ICPSolver& solver, const T1& p1,const T2& p2,const T3& p3,const T4& p4) :
		pDom(solver,detail::VarDomCreator<Dom>()(solver,p1,p2,p3,p4)) {}

	/** Creates a new variable and initializes its domain with parameters \p p1,
	 *  \p p2,\p p3,\p p4,\p p5 and \p p6.	*/
	template<class T1,class T2,class T3,class T4,class T5,class T6>
	DomVar(ICPSolver& solver, const T1& p1,const T2& p2,const T3& p3,
				 const T4& p4,const T5& p5,const T6& p6) :
		pDom(solver,detail::VarDomCreator<Dom>()(solver,p1,p2,p3,
													  p4,p5,p6)) {}

		/** Creates a new variable and initializes its domain with parameters \p p1,
	 *  \p p2,\p p3,\p p4,\p p5 and \p p6.	*/
	template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
	DomVar(ICPSolver& solver, const T1& p1,const T2& p2,const T3& p3,
				 const T4& p4,const T5& p5,const T6& p6, const T7& p7, const T8& p8) :
		pDom(solver,detail::VarDomCreator<Dom>()(solver,p1,p2,p3,
													  p4,p5,p6,p7,p8)) {}

	~DomVar() {}

	///	Returns the Solver object associated with this variable.
	ICPSolver&		solver()	const	{	return domain().solver();	}

	/// Returns \a true if the variable is ground, \a false otherwise.
	bool	ground() const { return domain().ground(); }

	/// Returns the domain of the variable
	Dom& domain() const {return *pDom.get();}

	/// Returns a pointer to the domain of the variable
	PDom& pDomain() {return pDom.get();}

	/// Returns a pointer to the domain of the variable (const version)
	const PDom& pDomain() const {return pDom.get();}

	//friend ostream& operator<< <>(ostream& os, const Self& f);

	/** Makes the variable domain point to the domain of \a s. This is a
	 * 	reversible assignment.	*/
	const Self& operator=(const Self& s)
	{
		pDomain() = s.pDomain();	// cheap copy
		return *this;
	}

	/** Makes the variable domain point to a copy of the domain of \a s.
	 * 	This is a reversible assignment.	*/
	const Self& operator=(const Dom& s)
	{
		pDom = PDom(new (solver().heap()) Dom(s));	// cheap copy
		return *this;
	}

	private:
	RPDom 	pDom;
};


template<class T,class Dom>
std::ostream& operator<<(std::ostream& os, const casper::DomVar<T,Dom>& v)
{
	os << v.domain();
	return os;
}

template<class T,class Dom>
std::istream& operator>>(std::istream& is, const casper::DomVar<T,Dom>& v)
{
	is >> v.domain();
	return is;
}

template<class T,class Dom>
struct GetCPSolver<DomVar<T,Dom> >
{	ICPSolver& operator()(const DomVar<T,Dom>& v)
	{ return v.solver(); }
};

/*
template<class>	class MutVar;
template<class T> ostream& operator<<(ostream& os, const MutVar<T>& f);
*/

/**
 * Mutable Variable
 */
template<class T>
class MutVar
{
	public:

	typedef T						Data;
	typedef MutVar<Data> 			Self;


	/// The type of the variable domain
	typedef Reversible<Data>		RData;
	typedef RData*					PRData;

	/// The default constructor
	MutVar(ICPSolver& solver) : mSolver(solver),
							pData(new (solver.heap()) RData(solver)),
							updateSL(solver) {}

	/// Builds a new variable pointing to the data of \a s.
	MutVar(const Self& s) : mSolver(s.solver()),
							pData(s.pData),
							updateSL(s.updateSL)	{ }

	/// A constructor with one parameter
	MutVar(ICPSolver& solver, const T& p1) : mSolver(solver),
		pData(new (solver.heap()) RData(solver,p1)),
		updateSL(solver)	{}
/*
	/// A constructor with one generic parameter
	template<class T1>
	MutVar(Solver solver, const T1& p1) : mSolver(solver),
		pData(new (solver.heap()) RData(solver,p1)),
		updateSL(solver)	{}

	/// A constructor with two generic parameters
	template<class T1,class T2>
	MutVar(Solver solver, const T1& p1,const T2& p2) : mSolver(solver),
		pData(new (solver.heap()) RData(solver,p1,p2)),
		updateSL(solver) {}

	// TODO: more constructors

	/// A constructor with six generic parameters
	template<class T1,class T2,class T3,class T4,class T5,class T6>
	MutVar(Solver solver, const T1& p1,const T2& p2,const T3& p3,
				 const T4& p4,const T5& p5,const T6& p6) :  mSolver(solver),
		pData(new (solver.heap()) RData(solver,p1,p2,p3,p4,p5,p6)),
		updateSL(solver) {}
*/
	~MutVar() {}

	ICPSolver&		solver()	const	{	return mSolver;	}

	/// Returns the value of the variable
	const Data& value() const {return (*pData).get();}

	/// Returns a pointer to the value of the variable
	//PRData& pValue() {return pData;}

	//operator const Data&() const { return value();	}

	//friend ostream& operator<< <>(ostream& os, const Self& f);

	/// Makes the variable domain point to the value of \a s
	Bool operator=(const Self& s)
	{
		*pData = s.value();
		return updateSL.notifyAll();
	}

	/// Assign \a s to the variable
	Bool operator=(const Data& s)
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
	Void attachOnUpdate(INotifiable*	f) { updateSL.attach(f); }
	Void detachOnUpdate(INotifiable*	f) { updateSL.detach(f); }

	private:
	ICPSolver&					mSolver;
	PRData 					pData;
	EventSuspList			updateSL;
};

template<class T>
struct GetCPSolver<MutVar<T> >
{	ICPSolver& operator()(const MutVar<T>& v)
	{ return v.solver(); }
};

template<class T>
std::ostream& operator<<(std::ostream& os, const casper::MutVar<T>& v)
{
	os << v.value();
	return os;
}

};

#endif

namespace casper {

#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_KERNEL_MACRO_VARIABLE_H
#define CASPER_KERNEL_MACRO_VARIABLE_H
#include <casper/kernel/macro/variable.h>
#endif
#endif

}
