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


#ifndef CASPER_BINDINGS_CPP_EXPRESSION_H_
#define CASPER_BINDINGS_CPP_EXPRESSION_H_

#include "symbol.h"
#include "container.h"
#include "detail.h"

namespace Casperbind {
namespace cpp {

namespace Detail {
template<class T,Symbol::Type type>
struct Constant : Symbol
{
	const T data;
	Type getType(bool i = false) const { return type;}
	Constant(const T& data) : data(data) {}
	const Symbol* clone() const { return new Constant(data);	}
	operator T() const { return data; }
	bool operator==(const Constant& s) const { return data==s.data; }
	bool operator!=(const Constant& s) const { return data!=s.data; }
};
}

/**
 * Symbol for a boolean (bool) constant
 * \ingroup CasperBindingsCPP
 */
typedef Detail::Constant<bool,Symbol::sBool>			Bool;

/**
 * Symbol for an integral (int) constant
 * \ingroup CasperBindingsCPP
 */
typedef Detail::Constant<int,Symbol::sInt>			Int;

/**
 *  Symbol for a real (double) constant.
 *  \ingroup CasperBindingsCPP
 */
typedef Detail::Constant<double,Symbol::sDouble>	Double;

/**
 *  Symbol for a string constant.
 *  \ingroup CasperBindingsCPP
 */
typedef Detail::Constant<const char*,Symbol::sString>	String;

/**
 * 	Symbol for arithmetic expressions. Currently only integer or real-valued
 *  expressions are allowed. For boolean expressions \see Predicate.
 *  The set of expressions with arity 1 is { eSym, eAbs, eCard }
 *  The set of expressions with arity 2 is
 *  { eSub, eDiv, eMod, ePow, eElem, eSymDiff }
 *  The set of expressions with arity 3 is
 *  { eIfThenElse }
 *  The set of expressions with arity >= 2 is
 *  { eAdd, eMul, eMin, eMax, eIntersect, eUnion }
 *  \ingroup CasperBindingsCPP
 */
struct Expression : Symbol
{
	/// Type of operation.
	enum Operator { eAdd,eSub,eMul,eDiv,eMod, eSym, eAbs, ePow, eMin, eMax,
		eIfThenElse, eElem, eCard, eIntersect, eUnion, eSymDiff, eSumProduct };
	Operator optor;
	SymbolArray opnds;
	/// Creates a unary expression
	Expression(Operator op, SharedSymbol op1);
	/// Creates a binary expression
	Expression(Operator op, SharedSymbol op1,SharedSymbol op2);
	/// Creates a ternary expression
	Expression(Operator op, SharedSymbol op1,SharedSymbol op2,SharedSymbol op3);
	/// Creates a nary expression
	Expression(Operator op, const SymbolArray& opnds);
	~Expression() {}
	Type getType(bool i = false) const { return Symbol::sExpression;}
	Operator getOperator() const { return optor; }
	const SymbolArray& getOperands() const { return opnds;}
	int getArity() const { return opnds.getSize(); }
	const Symbol* clone() const { return new Expression(optor,opnds);	}
	/// Returns the \p i 'th operand involved in the expression
	const SharedSymbol& operator[](int i) const { return opnds[i];	}
};

SharedSymbol operator+(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator+(SharedSymbol o1, int o2);
SharedSymbol operator+(int o1, SharedSymbol o2);

SharedSymbol operator-(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator-(SharedSymbol o1, int o2);
SharedSymbol operator-(int o1, SharedSymbol o2);

SharedSymbol operator*(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator*(SharedSymbol o1, int o2);
SharedSymbol operator*(int o1, SharedSymbol o2);

SharedSymbol operator/(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator/(SharedSymbol o1, int o2);
SharedSymbol operator/(int o1, SharedSymbol o2);

SharedSymbol operator%(SharedSymbol o1, SharedSymbol o2);
SharedSymbol operator%(SharedSymbol o1, int o2);
SharedSymbol operator%(int o1, SharedSymbol o2);

SharedSymbol operator-(SharedSymbol o1);

SharedSymbol abs(SharedSymbol o1);

SharedSymbol pow(SharedSymbol o1, SharedSymbol o2);
SharedSymbol pow(SharedSymbol o1, int o2);
SharedSymbol pow(int o1, SharedSymbol o2);

SharedSymbol min(SharedSymbol o1, SharedSymbol o2);
SharedSymbol min(SharedSymbol o1, int o2);
SharedSymbol min(int o1, SharedSymbol o2);

SharedSymbol max(SharedSymbol o1, SharedSymbol o2);
SharedSymbol max(SharedSymbol o1, int o2);
SharedSymbol max(int o1, SharedSymbol o2);

SharedSymbol element(SharedSymbol o1,SharedSymbol o2);
SharedSymbol element(SharedSymbol o1,int o2);

SharedSymbol cardinality(SharedSymbol o1);
SharedSymbol intersect(SharedSymbol o1,SharedSymbol o2);
SharedSymbol union_(SharedSymbol o1,SharedSymbol o2);
SharedSymbol symDiff(SharedSymbol o1,SharedSymbol o2);
SharedSymbol sumProduct(SharedSymbol o1,SharedSymbol o2);

}
}

#endif /* CASPER_BINDINGS_CPP_EXPRESSION_H_ */
