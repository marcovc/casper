/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2010-2010 - Marco Correia <marco.v.correia@gmail.com>           *
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
 
#include "cpp.h"
#include "eval.h"

namespace casperbind {
namespace cpp {

Eval getFirstElemEval(const Symbol& s)
{
	const Container& c = s;

	switch (c.getElemType())
	{
		case Container::sBool:
			return eBool;
		case Container::sInt:
			return eInt;
		case Container::sDouble:
			return eReal;
		case Container::sSymbol:
		{
			// FIXME
			switch (c.getType(true))
			{
				case Symbol::sArray:
				{
					const SymbolArray& a = c;
					return getEval(a[0]);
				}
				case Symbol::sRange:
				{
					const SymbolRange& a = c;
					return getEval(a.getLower());
				}
			}
			assert(0);
		}

	}
	assert(0);
}

Eval getEval(const Symbol& s)
{
	switch (s.getType(true))
	{
		case Symbol::sBool:
			return eBool;
		case Symbol::sInt:
			return eInt;
		case Symbol::sDouble:
			return eReal;

		case Symbol::sRange:
		{
			switch (getFirstElemEval(s))
			{
				case eInt:
					return eIntRange;
				case eReal:
					return eRealRange;
				case eIntRange:
				case eIntSet:
					return eIntSetRange;
				case eBoolSet:
					return eBoolSetRange;
			}
			assert(0);
		}

		case Symbol::sArray:
		{
			switch (getFirstElemEval(s))
			{
				case eBool:
					return eBoolArray;
				case eInt:
					return eIntArray;
				case eReal:
					return eRealArray;
				case eBoolVar:
					return eBoolVarArray;
				case eIntVar:
					return eIntVarArray;
				case eRealVar:
					return eRealVarArray;
				case eIntSetVar:
					return eIntSetVarArray;
				case eBoolSetVar:
					return eBoolSetVarArray;
			}
			assert(0);
		}
		case Symbol::sSet:
		{
			switch (getFirstElemEval(s))
			{
				case eBool:
					return eBoolSet;
				case eInt:
					return eIntSet;
			}
			assert(0);
		}
		case Symbol::sVariable:
		{
			const Variable& v = s;
			Eval e = getEval(v.getDomain());
			if (e == eBoolArray or e == eBoolSet or e == eBool)
				return eBoolVar;
			if (e == eIntArray or e == eIntSet or e == eIntRange or e == eInt)
				return eIntVar;
			if (e == eRealRange or e == eReal)
				return eRealVar;
			if (e == eIntSetRange)
				return eIntSetVar;
			if (e == eBoolSetRange)
				return eBoolSetVar;
			assert(0);
		}

		case Symbol::sExpression:
		{
			const Expression& e = s;
			Eval ev;
			switch (e.getOperator())
			{
				case Expression::eAdd:
				case Expression::eSub:
				case Expression::eMul:
				case Expression::eDiv:
				case Expression::eMod:
				case Expression::eSym:
				case Expression::eAbs:
				case Expression::ePow:
				case Expression::eMin:
				case Expression::eMax:
				case Expression::eIntersect:
				case Expression::eUnion:
				case Expression::eSymDiff:
					ev = getEval(e[0]);
					break;

				case Expression::eIfThenElse:
					ev = getEval(e[1]);
					break;
				case Expression::eCard:
					ev = eInt;
					break;
				case Expression::eElem:
					ev = getFirstElemEval(e[0]);
					break;
				case Expression::eSumProduct:
					ev = getFirstElemEval(e[1]);
					break;
			}

			switch (ev)
			{
				case eBool:
				case eBoolVar:
					return eBoolExpr;
				case eInt:
				case eIntVar:
					return eIntExpr;
				case eReal:
				case eRealVar:
					return eRealExpr;
				case eIntSet:
				case eIntSetVar:
					return eIntSetExpr;
				case eBoolSet:
				case eBoolSetVar:
					return eBoolSetExpr;
			}
			assert(0);
			break;
		}

		case Symbol::sPredicate:
		case Symbol::sInstance:
			return eBoolExpr;
	}
	throw EImplement(s);
}

}
}

