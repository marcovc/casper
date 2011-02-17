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

#include "validate.h"

namespace casperbind {
namespace cpp {

const Symbol* validateIntSet(const Symbol& s)
{
	try {
		const Container& a = s;
		if (a.getElemType()==Container::sInt)
			return NULL;
		if (a.getElemType()==Container::sSymbol)
		{
			const SymbolSet& sa = s;
			for (SymbolSet::Iterator it = sa.begin(); it != sa.end(); ++it)
			{
				const Symbol* r = validateIntEval(*it);
				if (r!=NULL)
					return r;
			}
		}
		return &s;
	}
	catch (const ETypeConversion&)
	{	return &s; }
	return NULL;
}


const Symbol* validateIntArray(const Symbol& s)
{
	try {
		const Container& a = s;
		if (a.getElemType()==Container::sInt)
			return NULL;
		if (a.getElemType()==Container::sSymbol)
		{
			const SymbolArray& sa = s;
			for (int i = 0; i < sa.getSize(); ++i)
			{
				const Symbol* r = validateIntEval(sa[i]);
				if (r!=NULL)
					return r;
			}
		}
		return &s;
	}
	catch (const ETypeConversion&)
	{	return &s; }
	return NULL;
}

const Symbol* validateSetArray(const Symbol& s)
{
	try {
		const Container& a = s;
		if (a.getElemType()==Container::sSymbol)
		{
			const SymbolArray& sa = s;
			for (int i = 0; i < sa.getSize(); ++i)
			{
				const Symbol* r = validateSetEval(sa[i]);
				if (r!=NULL)
					return r;
			}
			return NULL;
		}
		return &s;
	}
	catch (const ETypeConversion&)
	{	return &s; }
}

const Symbol* validateBoolArray(const Symbol& s)
{
	try {
		const Container& a = s;
		if (a.getElemType()==Container::sInt)
			return NULL;
		if (a.getElemType()==Container::sSymbol)
		{
			const SymbolArray& sa = s;
			for (int i = 0; i < sa.getSize(); ++i)
			{
				const Symbol* r = validateBoolEval(sa[i]);
				if (r!=NULL)
					return r;
			}
		}
		return &s;
	}
	catch (const ETypeConversion&)
	{	return &s; }
	return NULL;
}

const Symbol* validateArray(const Symbol& s)
{	return (s.getType()==Symbol::sArray)?NULL:&s;	}

const Symbol* validateRange(const Symbol& s)
{	return (s.getType()==Symbol::sRange)?NULL:&s;	}

const Symbol* validateSet(const Symbol& s)
{	return (s.getType()==Symbol::sSet)?NULL:&s;	}

const Symbol* validateIntVariable(const Symbol& s)
{
	try
	{
		const Variable& v = s;
		if (v.getDomain().getType(true) == Symbol::sIntDomain)
			return validate(v.getDomain());
	} catch (const ETypeConversion&)
	{	return &s; }
	return &s;
}

const Symbol* validateSetVariable(const Symbol& s)
{
	try
	{
		const Variable& v = s;
		if (v.getDomain().getType(true) == Symbol::sIntSetDomain)
			return validate(v.getDomain());
	} catch (const ETypeConversion&)
	{	return &s; }
	return &s;
}

const Symbol* validateRealVariable(const Symbol& s)
{
	try
	{
		const Variable& v = s;
		if (v.getDomain().getType(true) == Symbol::sRealDomain)
			return validate(v.getDomain());
	} catch (const ETypeConversion&)
	{	return &s; }
	return &s;
}

const Symbol* validateVariable(const Symbol& s)
{
	if (validateIntVariable(s)!=NULL and
		validateSetVariable(s)!=NULL and
		validateRealVariable(s)!=NULL)
		return &s;
	return NULL;
}

const Symbol* validateInt(const Symbol& s)
{
	return (s.getType()==Symbol::sInt)?NULL:&s;
}

const Symbol* validateDouble(const Symbol& s)
{
	return (s.getType()==Symbol::sDouble)?NULL:&s;
}

const Symbol* validateString(const Symbol& s)
{
	return (s.getType()==Symbol::sString)?NULL:&s;
}

const Symbol* faultyExpr(const Symbol* p, const Symbol* c1, const Symbol* c2)
{
	if (c1 == NULL and c2 == NULL)
		return NULL;
	if (c1 != NULL and c2 != NULL)
		return p;
	if (c1 != NULL)
		return c1;
	if (c2 != NULL)
		return c2;
	assert(0);
}

const Symbol* faultyExpr(const Symbol* p, const Symbol* c1, const Symbol* c2,const Symbol* c3)
{
	int nsucc = (c1 == NULL) + (c2 == NULL) + (c3 == NULL);
	if (nsucc == 3)
		return NULL;
	if (nsucc <= 1)
		return p;
	if (c1 != NULL)
		return c1;
	if (c2 != NULL)
		return c2;
	if (c3 != NULL)
		return c3;
	assert(0);
}

// asserts that symbol \a s evaluates to an integer
const Symbol* validateIntEval(const Symbol& s)
{
	switch (s.getType())
	{
		case Symbol::sInt:
			return NULL;

		case Symbol::sPredicate:
		case Symbol::sDouble:
		case Symcol::sString:
		case Symbol::sInstance:
		case Symbol::sArray:
		case Symbol::sRange:
		case Symbol::sSet:
		case Symbol::sIntSetDomain:
			return &s;

		case Symbol::sVariable:
			return validateIntVariable(s);

		case Symbol::sSymbol:
			return validateIntEval(static_cast<const SharedSymbol&>(s).get());

		case Symbol::sExpression:
		{
			const Expression& e = s;
			switch (e.getOperator())
			{
				case Expression::eSym:
				case Expression::eAbs:
					return validateIntEval(e[0]);

				case Expression::eSub:
				case Expression::eDiv:
				case Expression::eMod:
				case Expression::ePow:
					return faultyExpr(&s,validateIntEval(e[0]),validateIntEval(e[1]));

				case Expression::eElem:
					return faultyExpr(&s,validateIntArray(e[0]),validateIntEval(e[1]));

				case Expression::eIfThenElse:
					return faultyExpr(&s,validateBoolEval(e[0]),validateIntEval(e[1]),validateIntEval(e[2]));

				case Expression::eAdd:
				case Expression::eMul:
				case Expression::eMin:
				case Expression::eMax:
				case Expression::eSumProduct:
				{
					const Symbol* lastnotok = NULL;
					for (int i = 0; i < e.getArity(); ++i)
					{
						const Symbol* r = validateIntEval(e[i]);
						if (r!=NULL)
						{
							if (lastnotok != NULL)
								return &s;
							lastnotok = r;
						}
						if (lastnotok != NULL)
							return lastnotok;
					}
					return NULL;
				}

				case Expression::eCard:
				case Expression::eIntersect:
				case Expression::eUnion:
				case Expression::eSymDiff:
					return &s;
			}
		}
	}
	return &s;
}

// asserts that expression \a s evaluates to a set
const Symbol* validateSetEval(const Symbol& s)
{
	switch (s.getType())
	{
		case Symbol::sSet:
			return NULL;

		case Symbol::sPredicate:
		case Symbol::sDouble:
		case Symbol::sInstance:
		case Symbol::sArray:
		case Symbol::sRange:
		case Symbol::sInt:
		case Symbol::sString:
		case Symbol::sIntSetDomain:
			return &s;

		case Symbol::sVariable:
			return validateSetVariable(s);

		case Symbol::sSymbol:
			return validateSetEval(static_cast<const SharedSymbol&>(s).get());

		case Symbol::sExpression:
		{
			const Expression& e = s;
			switch (e.getOperator())
			{
				case Expression::eSym:
				case Expression::eAbs:
				case Expression::eAdd:
				case Expression::eSub:
				case Expression::eMul:
				case Expression::eDiv:
				case Expression::eMod:
				case Expression::ePow:
				case Expression::eMin:
				case Expression::eMax:
				case Expression::eSumProduct:
					return &s;

				case Expression::eCard:
					return validateSetEval(e[0]);

				case Expression::eElem:
					return faultyExpr(&s,validateSetArray(e[0]),validateIntEval(e[1]));

				case Expression::eSymDiff:
					return faultyExpr(&s,validateSetEval(e[0]),validateSetEval(e[1]));

				case Expression::eIfThenElse:
					return faultyExpr(&s,validateBoolEval(e[0]),validateSetEval(e[1]),validateSetEval(e[2]));

				case Expression::eIntersect:
				case Expression::eUnion:
				{
					const Symbol* lastnotok = NULL;
					for (int i = 0; i < e.getArity(); ++i)
					{
						const Symbol* r = validateSetEval(e[i]);
						if (r!=NULL)
						{
							if (lastnotok != NULL)
								return &s;
							lastnotok = r;
						}
						if (lastnotok != NULL)
							return lastnotok;
					}
					return NULL;
				}
			}
			return &s;
		}
	}
	return &s;
}

// asserts that symbol \a s evaluates to a boolan
const Symbol* validateBoolEval(const Symbol& s)
{
	switch (s.getType())
	{
		case Symbol::sInt:
			if (static_cast<const int&>(s)>=0 and static_cast<const int&>(s)<=1)
				return NULL;
			else
				return &s;

		case Symbol::sExpression:
		case Symbol::sDouble:
		case Symbol::sString:
		case Symbol::sInstance:
		case Symbol::sArray:
		case Symbol::sRange:
		case Symbol::sSet:
		case Symbol::sIntSetDomain:
			return &s;

		case Symbol::sVariable:
			return validateIntVariable(s);

		case Symbol::sSymbol:
			return validateBoolEval(static_cast<const SharedSymbol&>(s).get());

		case Symbol::sPredicate:
		{
			const Predicate& e = s;
			const Symbol* (*fn)(const Symbol&) = NULL;
			switch (e.getFunctor())
			{
				case Predicate::pNot:
					return validateBoolEval(e[0]);

				case Predicate::pLess:
				case Predicate::pGreater:
				case Predicate::pLessEqual:
				case Predicate::pGreaterEqual:
					return faultyExpr(&s,validateIntEval(e[0]),validateIntEval(e[1]));

				case Predicate::pIf:
					return faultyExpr(&s,validateBoolEval(e[0]),validateBoolEval(e[1]));

				case Predicate::pContained:
				case Predicate::pMember:
					return faultyExpr(&s,validateSetEval(e[0]),validateSetEval(e[1]));

				case Predicate::pAnd:
				case Predicate::pOr:
				case Predicate::pXOr:
				case Predicate::pIff:
					fn = validateBoolEval;

				case Predicate::pEqual:
				case Predicate::pDistinct:
					if (validateBoolEval(e[0])==NULL)
						fn = validateBoolEval;
					else
					if (validateIntEval(e[0])==NULL)
						fn = validateIntEval;
					else
					if (validateSetEval(e[0])==NULL)
						fn = validateSetEval;
					else
						assert(0);

				{
					const Symbol* lastnotok = NULL;
					for (int i = 0; i < e.getArity(); ++i)
					{
						const Symbol* r = fn(e[i]);
						if (r!=NULL)
						{
							if (lastnotok != NULL)
								return &s;
							lastnotok = r;
						}
						if (lastnotok != NULL)
							return lastnotok;
					}
					return NULL;
				}
			}
		}
	}
	return &s;
}

const Symbol* validateExpression(const Symbol& s)
{
	try
	{
		const Symbol* ie = validateIntEval(s);
		const Symbol* de = validateSetEval(s);
		if (ie != NULL and de != NULL)
			return &s;
		return NULL;
	} catch (const ETypeConversion &)
	{	return &s; }
}

// does a very permissive validation of a given predicate
// it does not test for semantic coherence
const Symbol* validatePredicate(const Symbol& s)
{
	try
	{
		return validateBoolEval(s);
	} catch (const ETypeConversion &)
	{	return &s; }
}

const Symbol* validateInstance(const Symbol& s)
{
	if (s.getType()==Symbol::sInstance)
	{
		const Instance& i = s;

		// variables must be an array of variables
		try {
			const SymbolArray& a = i.getVariables();
			for (int i = 0; i < a.getSize(); ++i)
			{
				const Symbol* r = validateVariable(a[i]);
				if (r != NULL)
					return r;
			}
		} catch (const ETypeConversion&)
		{	return &i.getVariables();	}

		// constraints must be an array of constraints
		try {
			const SymbolArray& a = i.getConstraints();
			for (int i = 0; i < a.getSize(); ++i)
			{
				const Symbol* r = validatePredicate(a[i]);
				if (r != NULL)
					return r;
			}
		} catch (const ETypeConversion&)
		{	return &i.getVariables();	}
	}
	return NULL;
}

const Symbol* validateSharedSymbol(const Symbol& s)
{	return (s.getType()==Symbol::sSymbol)?NULL:&s;	}

const Symbol* validate(const Symbol& s)
{
	Symbol::assertVersionIn(0,1,0,0,1,0);

	switch (s.getType())
	{
		case Symbol::sArray:
			return validateArray(s);
		case Symbol::sRange:
			return validateRange(s);
		case Symbol::sSet:
			return validateSet(s);
		case Symbol::sVariable:
			return validateVariable(s);
		case Symbol::sInt:
			return validateInt(s);
		case Symbol::sDouble:
			return validateDouble(s);
		case Symbol::sString:
			return validateString(s);
		case Symbol::sExpression:
			return validateExpression(s);
		case Symbol::sPredicate:
			return validatePredicate(s);
		case Symbol::sInstance:
			return validateInstance(s);
		case Symbol::sSymbol:
			return validateSharedSymbol(s);
	}
	return &s;
}

}
}
