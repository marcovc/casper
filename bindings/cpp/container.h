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
//#include "detail.h"

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


}
}


#endif /* CASPER_BINDINGS_CPP_CONTAINERS_H_ */
