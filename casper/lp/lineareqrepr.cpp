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
 
#include <casper/lp/lineareqrepr.h>

namespace Casper {
namespace LP {


LinearEqRepr operator==(const LinearExprRepr& lhs, const LinearExprRepr& rhs)
{	return LinearEqRepr(LinearEqRepr::Equal, lhs-rhs);	}

LinearEqRepr operator==(const LinearExprRepr& lhs, const Var& rhs)
{	return LinearEqRepr(LinearEqRepr::Equal, lhs-LinearExprRepr(rhs));	}

LinearEqRepr operator==(const LinearExprRepr& lhs, double rhs)
{	return LinearEqRepr(LinearEqRepr::Equal, lhs-LinearExprRepr(rhs));	}


LinearEqRepr operator>=(const LinearExprRepr& lhs, const LinearExprRepr& rhs)
{	return LinearEqRepr(LinearEqRepr::LessEqual, rhs-lhs);	}

LinearEqRepr operator>=(const LinearExprRepr& lhs, const Var& rhs)
{	return LinearEqRepr(LinearEqRepr::LessEqual, LinearExprRepr(rhs)-lhs);	}

LinearEqRepr operator>=(const LinearExprRepr& lhs, double rhs)
{	return LinearEqRepr(LinearEqRepr::LessEqual, LinearExprRepr(rhs)-lhs);	}


LinearEqRepr operator<=(const LinearExprRepr& lhs, const LinearExprRepr& rhs)
{	return LinearEqRepr(LinearEqRepr::LessEqual, lhs-rhs);	}

LinearEqRepr operator<=(const LinearExprRepr& lhs, const Var& rhs)
{	return LinearEqRepr(LinearEqRepr::LessEqual, lhs-LinearExprRepr(rhs));	}

LinearEqRepr operator<=(const LinearExprRepr& lhs, double rhs)
{	return LinearEqRepr(LinearEqRepr::LessEqual, lhs-LinearExprRepr(rhs));	}

LinearEqRepr operator!(const LinearEqRepr& leq)
{
	throw Exception::NonLinear();
}

}
}

std::ostream& operator<<(std::ostream& os, const Casper::LP::LinearEqRepr& eq)
{
	os << eq.getExpr();
	switch (eq.getOperator())
	{
		case Casper::LP::LinearEqRepr::Equal:
			os << " == ";
			break;
		case Casper::LP::LinearEqRepr::LessEqual:
			os << " <= ";
			break;
	}
	os << "0";
	return os;
}


