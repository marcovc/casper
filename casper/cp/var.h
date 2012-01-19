 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_CP_VAR
#define _H_CASPER_CP_VAR

#include <iostream>

#include <casper/kernel/common.h>
#include <casper/kernel/reversible/reversible.h>
#include <casper/kernel/notify/susplist.h>
#include <casper/kernel/rel/rel.h>

#include <casper/cp/store.h>
#include <casper/cp/traits.h>

namespace Casper {
namespace CP {


namespace Detail {

template<class T>
struct VarDomCreator
{
	T* operator()(Store& store)
	{ return new (store) T(); }

	T* operator()(Store& store,const T& s)
	{ return new (store) T(s);	}

	T* operator()(Store& store,T& s)
	{ return new (store) T(s);	}
};
};

//using namespace std;
/*
template<class,class>
class Var;

template<class T,class Dom>
ostream& operator<<(ostream& os, const Var<T,Dom>& f);
*/

namespace Traits {
template<class T>
struct GetDefaultDom;
} // Traits

/**
 * 	A domain variable holds a pointer to a domain of type \p DomT, and when
 *  instantiated will represent a value of type \p T.
 *  \ingroup CPDataStructures
 */
template<class T,
		 class DomT = typename Traits::GetDefaultDom<T>::Type>
class Var
{
	typedef T						Type;
	typedef Var<Type,DomT> 		Self;

	public:

	/// The type of the variable domain
	typedef DomT					Dom;
	typedef Dom*					PDom;
	typedef Reversible<PDom>		RPDom;

	/// Creates a new Var with a default domain.
	Var(Store& store) : pDom(store,Detail::VarDomCreator<Dom>()(store)) {}

	/// Creates a new variable pointing to the domain of \a s.
	Var(const Var<T,DomT>& s) : pDom(s.pDom)	{ }

	/// Creates a new variable with the domain pointing to \a s.
	Var(Store& store,PDom s) : pDom(store,s)	{ }

	/// Creates a new variable and initializes its domain with parameter \p p1.
	template<class T1>
	Var(Store& store, const T1& p1) :
		pDom(store,Detail::VarDomCreator<Dom>()(store,p1))	{}

	/** Creates a new variable and initializes its domain with parameters \p p1
	 *  and \p p2.	*/
	template<class T1,class T2>
	Var(Store& store, const T1& p1,const T2& p2) :
		pDom(store,Detail::VarDomCreator<Dom>()(store,p1,p2)) {}

	/** Creates a new variable and initializes its domain with parameters \p p1,
	 *  \p p2 and \p p3.	*/
	template<class T1,class T2,class T3>
	Var(Store& store, const T1& p1,const T2& p2,const T3& p3) :
		pDom(store,Detail::VarDomCreator<Dom>()(store,p1,p2,p3)) {}

	// TODO: more constructors

	/** Creates a new variable and initializes its domain with parameters \p p1,
	 *  \p p2,\p p3,\p p4.	*/
	template<class T1,class T2,class T3,class T4>
	Var(Store& store, const T1& p1,const T2& p2,const T3& p3,const T4& p4) :
		pDom(store,Detail::VarDomCreator<Dom>()(store,p1,p2,p3,p4)) {}

	/** Creates a new variable and initializes its domain with parameters \p p1,
	 *  \p p2,\p p3,\p p4,\p p5 and \p p6.	*/
	template<class T1,class T2,class T3,class T4,class T5,class T6>
	Var(Store& store, const T1& p1,const T2& p2,const T3& p3,
				 const T4& p4,const T5& p5,const T6& p6) :
		pDom(store,Detail::VarDomCreator<Dom>()(store,p1,p2,p3,p4,p5,p6)) {}

		/** Creates a new variable and initializes its domain with parameters \p p1,
	 *  \p p2,\p p3,\p p4,\p p5 and \p p6.	*/
	template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
	Var(Store& store, const T1& p1,const T2& p2,const T3& p3,
				 const T4& p4,const T5& p5,const T6& p6, const T7& p7, const T8& p8) :
		pDom(store,Detail::VarDomCreator<Dom>()(store,p1,p2,p3,
													  p4,p5,p6,p7,p8)) {}

#ifndef SWIG
	/// Creates a new variable and initializes its domain list \p lst.
	Var(Store& store, std::initializer_list<T> lst) :
		pDom(store,Detail::VarDomCreator<Dom>()(store,lst)) {}
#endif

	~Var() {}

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
//	const Self& operator=(const Dom& s)
//	{
//		pDom = PDom(new (solver().getHeap()) Dom(s));	// cheap copy
//		return *this;
//	}

	/// Returns the state manager for this object.
	State& getState() const {	return pDom.get()->getState();	}

	private:
	RPDom 	pDom;
};

namespace Traits {
template<class Eval,class DomT>
struct GetDom<Var<Eval,DomT> >
{
	typedef DomT	Type;
};
} // Traits

} // CP

namespace Traits {
template<class T,class D>
struct GetEval<CP::Var<T,D> >
{	typedef	T	Type;	};

template<class T,class D>
struct GetTypeStr<CP::Var<T,D> >
{
	std::string operator()()
	{	return std::string("Casper::CP::Var<")+getTypeStr<T>()+">"; }
};

} // Traits

template<class> struct GetPState;

template<class T,class Dom>
struct GetPState<CP::Var<T,Dom> >
{	State* operator()(const CP::Var<T,Dom>& v)
	{ return &v.getState(); }
};


} // Casper

template<class T,class Dom>
std::ostream& operator<<(std::ostream& os, const Casper::CP::Var<T,Dom>& v)
{
	os << v.domain();
	return os;
}

template<class T,class Dom>
std::istream& operator>>(std::istream& is, const Casper::CP::Var<T,Dom>& v)
{
	is >> v.domain();
	return is;
}

#endif

#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_CP_REL_OP_VAR_H
#define CASPER_CP_REL_OP_VAR_H
#include <casper/cp/rel/op_var.h>
#endif
#endif
