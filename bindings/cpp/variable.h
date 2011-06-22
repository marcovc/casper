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


#ifndef CASPER_BINDINGS_CPP_VARIABLE_H_
#define CASPER_BINDINGS_CPP_VARIABLE_H_

#include "symbol.h"
#include "container.h"

namespace Casperbind {
namespace cpp {

/**
 * 	Represents a CSP variable.
 *	\ingroup CasperBindingsCPP
 */
struct Variable : Symbol
{
	SharedSymbol pDomain;

	/// Create a variable with the given domain
	Variable(SharedSymbol d) : pDomain(d) {}
	//Variable(const Symbol& v) : pDomain(v) {}
	~Variable() {}
	Type getType(bool i = false) const { return Symbol::sVariable;}
	Symbol* clone() const { return new Variable(pDomain); }
	const SharedSymbol& getDomain() const { return pDomain; }
	Container::ElemType getVariableType() const
	{
		if (pDomain.getType(true)==Symbol::sInt)
			return Container::sInt;
		if (pDomain.getType(true)==Symbol::sBool)
			return Container::sBool;
		if (pDomain.getType(true)==Symbol::sDouble)
			return Container::sDouble;
		if (pDomain.getType(true)==Symbol::sString)
			return Container::sString;
		return static_cast<const Container&>(pDomain.get()).getElemType();
	}
};


}
}

#endif /* CASPER_BINDINGS_CPP_VARIABLE_H_ */
