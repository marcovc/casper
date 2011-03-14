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


#ifndef CASPER_BINDINGS_CPP_ANNOTATION_H_
#define CASPER_BINDINGS_CPP_ANNOTATION_H_

#include "symbol.h"
#include "container.h"
#include "expression.h"

#include <iostream>

namespace Casperbind {
namespace cpp {

/**
 * 	Symbol for annotations.
 *  \ingroup CasperBindingsCPP
 */
struct Annotation : Symbol
{
	String functor;
	SymbolArray terms;
	Annotation(String op) : functor(op),terms(0) {}
	/// Creates an n-ary annotation
	Annotation(String op, const SymbolArray& terms) : functor(op),terms(terms){}
	Type getType(bool) const { return Symbol::sAnnotation;}
	String getFunctor() const { return functor; }
	SymbolArray getTerms() const { return terms;}
	/// Returns the \p i 'th operand involved in the predicate
	const SharedSymbol& operator[](int i) const { return terms[i];	}

	int getArity() const { return terms.getSize(); }
	const Symbol* clone() const { return new Annotation(functor,terms);	}
};

}
}

#endif /* CASPER_BINDINGS_CPP_ANNOTATION_H_ */
