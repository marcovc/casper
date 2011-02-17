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

#include "expression.h"

namespace casperbind {
namespace cpp {

Expression::Expression(Operator  op, SharedSymbol  op1) :
	optor(op),opnds(1)
{
	opnds[0] = op1;
}

Expression::Expression(Operator  op, SharedSymbol  op1,SharedSymbol  op2) :
	optor(op),opnds(2)
{
	opnds[0] = op1;
	opnds[1] = op2;
}

Expression::Expression(Operator  op, SharedSymbol  op1,
		SharedSymbol  op2,SharedSymbol  op3) :
							optor(op),opnds(3)
{
	opnds[0] = op1;
	opnds[1] = op2;
	opnds[2] = op3;
}

Expression::Expression(Operator  op, const SymbolArray& opd) :
	optor(op),opnds(opd.getSize())
{
	for (int i = 0; i < opd.getSize(); ++i)
		opnds[i] = opd[i];
}

SharedSymbol operator+(SharedSymbol o1, SharedSymbol o2)
{	return new Expression(Expression::eAdd,o1,o2);	}

SharedSymbol operator+(SharedSymbol o1, int o2)
{	return new Expression(Expression::eAdd,o1,SharedSymbol(o2));	}

SharedSymbol operator+(int o1, SharedSymbol o2)
{	return new Expression(Expression::eAdd,SharedSymbol(o1),o2);	}

SharedSymbol operator-(SharedSymbol o1, SharedSymbol o2)
{	return new Expression(Expression::eSub,o1,o2);	}

SharedSymbol operator-(SharedSymbol o1, int o2)
{	return new Expression(Expression::eSub,o1,SharedSymbol(o2));	}

SharedSymbol operator-(int o1, SharedSymbol o2)
{	return new Expression(Expression::eSub,SharedSymbol(o1),o2);	}

SharedSymbol operator*(SharedSymbol o1, SharedSymbol o2)
{	return new Expression(Expression::eMul,o1,o2);	}

SharedSymbol operator*(SharedSymbol o1, int o2)
{	return new Expression(Expression::eMul,o1,SharedSymbol(o2));	}

SharedSymbol operator*(int o1, SharedSymbol o2)
{	return new Expression(Expression::eMul,SharedSymbol(o1),o2);	}

SharedSymbol operator/(SharedSymbol o1, SharedSymbol o2)
{	return new Expression(Expression::eDiv,o1,o2);	}

SharedSymbol operator/(SharedSymbol o1, int o2)
{	return new Expression(Expression::eDiv,o1,SharedSymbol(o2));	}

SharedSymbol operator/(int o1, SharedSymbol o2)
{	return new Expression(Expression::eDiv,SharedSymbol(o1),o2);	}

SharedSymbol operator%(SharedSymbol o1, SharedSymbol o2)
{	return new Expression(Expression::eMod,o1,o2);	}

SharedSymbol operator%(SharedSymbol o1, int o2)
{	return new Expression(Expression::eMod,o1,SharedSymbol(o2));	}

SharedSymbol operator%(int o1, SharedSymbol o2)
{	return new Expression(Expression::eMod,SharedSymbol(o1),o2);	}

SharedSymbol operator-(SharedSymbol o1)
{	return new Expression(Expression::eSym,o1);	}

SharedSymbol abs(SharedSymbol o1)
{	return new Expression(Expression::eAbs,o1);	}

SharedSymbol pow(SharedSymbol o1, SharedSymbol o2)
{	return new Expression(Expression::ePow,o1,o2);	}

SharedSymbol pow(SharedSymbol o1, int o2)
{	return new Expression(Expression::ePow,o1,SharedSymbol(o2));	}

SharedSymbol pow(int o1, SharedSymbol o2)
{	return new Expression(Expression::ePow,SharedSymbol(o1),o2);	}

SharedSymbol min(SharedSymbol o1,SharedSymbol o2)
{	return new Expression(Expression::eMin,o1,o2);	}

SharedSymbol min(SharedSymbol o1, int o2)
{	return new Expression(Expression::eMin,o1,SharedSymbol(o2));	}

SharedSymbol min(int o1, SharedSymbol o2)
{	return new Expression(Expression::eMin,SharedSymbol(o1),o2);	}

SharedSymbol max(SharedSymbol o1,SharedSymbol o2)
{	return new Expression(Expression::eMax,o1,o2);	}

SharedSymbol max(SharedSymbol o1, int o2)
{	return new Expression(Expression::eMax,o1,SharedSymbol(o2));	}

SharedSymbol max(int o1, SharedSymbol o2)
{	return new Expression(Expression::eMax,SharedSymbol(o1),o2);	}

SharedSymbol element(SharedSymbol o1,SharedSymbol o2)
{	return new Expression(Expression::eElem,o1,o2);	}

SharedSymbol cardinality(SharedSymbol o1)
{	return new Expression(Expression::eCard,o1);	}

SharedSymbol intersect(SharedSymbol o1,SharedSymbol o2)
{	return new Expression(Expression::eIntersect,o1,o2);	}

SharedSymbol union_(SharedSymbol o1,SharedSymbol o2)
{	return new Expression(Expression::eUnion,o1,o2);	}

SharedSymbol symDiff(SharedSymbol o1,SharedSymbol o2)
{	return new Expression(Expression::eSymDiff,o1,o2);	}

SharedSymbol sumProduct(SharedSymbol o1,SharedSymbol o2)
{	return new Expression(Expression::eSumProduct,o1,o2);	}

}
}
