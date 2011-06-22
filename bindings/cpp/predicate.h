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


#ifndef CASPER_BINDINGS_CPP_PREDICATE_H_
#define CASPER_BINDINGS_CPP_PREDICATE_H_

#include "symbol.h"
#include "container.h"
#include "detail.h"

namespace Casperbind {
namespace cpp {

/**
 * 	Symbol for predicates. For expressions with non-boolean
 *  evaluation \see Expression.
 *  The set of predicates with arity 1 is { pNot }
 *  The set of predicates with arity 2 is
 *  { pLess, pLessEqual, pGreater, pGreaterEqual, pIf, pContained, pMember }
 *  The set of predicates with arity >= 2 is
 *  { pEqual, pDistinct, pAnd, pOr, pXOr, pIff }
 *
 *  \ingroup CasperBindingsCPP
 */
struct Predicate : Symbol
{
	/// Available predicate functors.
	enum Functor { pEqual,pDistinct,pLess,pLessEqual,pGreater,pGreaterEqual,
					pNot, pAnd, pOr, pXOr, pIff, pIf, pContained, pMember,
					pInTable, pNotInTable, pDisjoint,pCumulative,pPartition,
					pChannel
				 };
	Functor functor;
	SymbolArray terms;
	/// Creates an n-ary predicate
	Predicate(Functor op, const SymbolArray& terms) : functor(op),terms(terms) {}
	Predicate(Functor op, SharedSymbol o1) : functor(op),terms(makeSymbolArray(o1)) {}
	Predicate(Functor op, SharedSymbol o1, SharedSymbol o2) : functor(op),terms(makeSymbolArray(o1,o2)) {}
	Predicate(Functor op, SharedSymbol o1, SharedSymbol o2, SharedSymbol o3) : functor(op),terms(makeSymbolArray(o1,o2,o3)) {}
	Type getType(bool) const { return Symbol::sPredicate;}
	Functor getFunctor() const { return functor; }
	SymbolArray getTerms() const { return terms;}
	/// Returns the \p i 'th operand involved in the predicate
	const SharedSymbol& operator[](int i) const { return terms[i];	}

	int getArity() const { return terms.getSize(); }
	const Symbol* clone() const { return new Predicate(functor,terms);	}
};

/// \ingroup CasperBindingsCPP
typedef Predicate Constraint;

SharedSymbol operator==(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator==(SharedSymbol o1, int o2);
SharedSymbol operator==(int o1, SharedSymbol o2);
SharedSymbol operator==(SharedSymbol o1, bool o2);
SharedSymbol operator==(bool o1, SharedSymbol o2);

SharedSymbol operator!=(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator!=(SharedSymbol o1, int o2);
SharedSymbol operator!=(int o1, SharedSymbol o2);
SharedSymbol operator!=(SharedSymbol o1, bool o2);
SharedSymbol operator!=(bool o1, SharedSymbol o2);

SharedSymbol operator<(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator<(SharedSymbol o1, int o2);
SharedSymbol operator<(int o1, SharedSymbol o2);
SharedSymbol operator<(SharedSymbol o1, bool o2);
SharedSymbol operator<(bool o1, SharedSymbol o2);

SharedSymbol operator<=(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator<=(SharedSymbol o1, int o2);
SharedSymbol operator<=(int o1, SharedSymbol o2);
SharedSymbol operator<=(SharedSymbol o1, bool o2);
SharedSymbol operator<=(bool o1, SharedSymbol o2);

SharedSymbol operator>(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator>(SharedSymbol o1, int o2);
SharedSymbol operator>(int o1, SharedSymbol o2);
SharedSymbol operator>(SharedSymbol o1, bool o2);
SharedSymbol operator>(bool o1, SharedSymbol o2);

SharedSymbol operator>=(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator>=(SharedSymbol o1, int o2);
SharedSymbol operator>=(int o1, SharedSymbol o2);
SharedSymbol operator>=(SharedSymbol o1, bool o2);
SharedSymbol operator>=(bool o1, SharedSymbol o2);

SharedSymbol operator!(SharedSymbol o1);

SharedSymbol operator&&(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator&&(SharedSymbol o1, bool o2);
SharedSymbol operator&&(bool o1, SharedSymbol o2);

SharedSymbol operator||(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator||(SharedSymbol o1, bool o2);
SharedSymbol operator||(bool o1, SharedSymbol o2);

SharedSymbol operator^(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator^(SharedSymbol o1, bool o2);
SharedSymbol operator^(bool o1, SharedSymbol o2);

SharedSymbol channel(SharedSymbol o1, SharedSymbol o2);
SharedSymbol contained(SharedSymbol o1, SharedSymbol o2);
SharedSymbol member(SharedSymbol o1, SharedSymbol o2);
SharedSymbol inTable(SharedSymbol o1, SharedSymbol o2);
SharedSymbol notInTable(SharedSymbol o1, SharedSymbol o2);
SharedSymbol disjoint(SharedSymbol o1, SharedSymbol o2);

}

}

#endif /* CASPER_BINDINGS_CPP_PREDICATE_H_ */
