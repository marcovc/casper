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

#ifndef CASPER_BINDINGS_CPP_CONTAINERS_H_
#define CASPER_BINDINGS_CPP_CONTAINERS_H_

#include "symbol.h"

namespace Casperbind {
namespace cpp {

/**
 * 	Base class for all container-like classes in cpp bindings
 * 	\ingroup CasperBindingsCPP
 */
struct Container : Symbol
{
	/// Containers can hold elements of the following types
	enum ElemType { sBool = Symbol::sBool,
					sInt = Symbol::sInt,
					sDouble = Symbol::sDouble,
					sString = Symbol::sString,
					sSymbol = Symbol::sSymbol };

	/// Returns the type of element stored in the container
	virtual ElemType getElemType() const = 0;
};

namespace Detail {
template<class>
struct ElemTypeTraits;

template<>
struct ElemTypeTraits<bool>
{	static const Container::ElemType type; };

template<>
struct ElemTypeTraits<int>
{	static const Container::ElemType type; };

template<>
struct ElemTypeTraits<double>
{	static const Container::ElemType type; };

template<>
struct ElemTypeTraits<const char*>
{	static const Container::ElemType type; };

template<>
struct ElemTypeTraits<SharedSymbol>
{	static const Container::ElemType type; };

template<class T>
Container::ElemType getElemType()
{	return ElemTypeTraits<T>::type;	}

};

template<class T>
Symbol::operator const Array<T>&() const
{
	const Array<T>& r = skipPtrs<Array<T> >(*this,sArray,"Array");
	assert(r.getElemType() == Detail::getElemType<T>());
	return r;
}


}
}

#include "detail.h"

namespace Casperbind {
namespace cpp {


/**
 * Heterogeneous array of (shared) objects
 * \ingroup CasperBindingsCPP
 */

typedef Array<SharedSymbol>	SymbolArray;

/**
 * Heterogeneous set of (shared) objects
 * \ingroup CasperBindingsCPP
 */

typedef Detail::Set<SharedSymbol,Detail::LtSharedSymbol>	SymbolSet;

/**
 * Closed interval of something.Set<SharedSymbol>
 * \ingroup CasperBindingsCPP
 */
typedef StdRange<SharedSymbol> SymbolRange;

/**
 * Multidimensional array of booleans
 * \ingroup CasperBindingsCPP
 */
typedef Array<bool> BoolArray;

/**
 * Multidimensional array of integers
 * \ingroup CasperBindingsCPP
 */
typedef Array<int> IntArray;

/**
 * Multidimensional array of reals
 * \ingroup CasperBindingsCPP
 */
typedef Array<double> RealArray;

/**
 * Set of booleans
 * \ingroup CasperBindingsCPP
 */
typedef Detail::Set<bool> BoolSet;

/**
 * Set of integers
 * \ingroup CasperBindingsCPP
 */
typedef Detail::Set<int> IntSet;

/**
 * Set of realse
 * \ingroup CasperBindingsCPP
 */
typedef Detail::Set<double> RealSet;

//TODO
typedef StdRange<SymbolSet> SetRange;


/**
 * Closed integer interval.
 * \ingroup CasperBindingsCPP
 */
typedef StdRange<int> IntRange;



/**
 * Closed real interval.
 * \ingroup CasperBindingsCPP
 */
typedef StdRange<double> RealRange;


template<class T>
Symbol::operator Array<T>() const
{
	//const Array<T>& r = skipPtrs<Array<T> >(*this,sArray,"Array");
	//assert(r.getElemType() == Detail::getElemType<T>());
	//return r;

	const Array<T>& r = skipPtrs<Array<T> >(*this,sArray,"Array");
	if (r.getElemType() == Detail::getElemType<T>())
		return r;
	if (r.getElemType()==Container::sSymbol)
	{
		const SymbolArray& r1 = skipPtrs<SymbolArray>(*this,sArray,"Array");
		Array<T> r(r1.getSize());
		for (int i = 0; i < r1.getSize(); ++i)
			r[i] = r1[i];
		return r;
	}
	assert(0);
}

inline SymbolArray makeSymbolArray(SharedSymbol arg1)
{ 	return Detail::makeArray<SharedSymbol>(arg1); }

inline SymbolArray makeSymbolArray(SharedSymbol arg1,SharedSymbol arg2)
{ 	return Detail::makeArray<SharedSymbol>(arg1,arg2); }

inline SymbolArray makeSymbolArray(SharedSymbol arg1,SharedSymbol arg2,SharedSymbol arg3)
{ 	return Detail::makeArray<SharedSymbol>(arg1,arg2,arg3); }

}
}


#endif /* CASPER_BINDINGS_CPP_CONTAINERS_H_ */
