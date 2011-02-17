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

#include "predicate.h"

namespace casperbind {
namespace cpp {

/*
Predicate::Predicate(Functor f, const SymbolArray& t) :
	functor(f),terms(t.getSize())
{
	for (int i = 0; i < t.getSize(); ++i)
		terms[i] = t[i];
}
*/

SharedSymbol operator==(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pEqual,o1,o2);	}

SharedSymbol operator==(SharedSymbol o1, int o2)
{	return new Predicate(Predicate::pEqual,o1,SharedSymbol(o2));	}

SharedSymbol operator==(int o1, SharedSymbol o2)
{	return new Predicate(Predicate::pEqual,SharedSymbol(o1),o2);	}

SharedSymbol operator==(SharedSymbol o1, bool o2)
{	return new Predicate(Predicate::pEqual,o1,SharedSymbol(o2));	}

SharedSymbol operator==(bool o1, SharedSymbol o2)
{	return new Predicate(Predicate::pEqual,SharedSymbol(o1),o2);	}


SharedSymbol operator!=(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pDistinct,o1,o2);	}

SharedSymbol operator!=(SharedSymbol o1, int o2)
{	return new Predicate(Predicate::pDistinct,o1,SharedSymbol(o2));	}

SharedSymbol operator!=(int o1, SharedSymbol o2)
{	return new Predicate(Predicate::pDistinct,SharedSymbol(o1),o2);	}

SharedSymbol operator!=(SharedSymbol o1, bool o2)
{	return new Predicate(Predicate::pDistinct,o1,SharedSymbol(o2));	}

SharedSymbol operator!=(bool o1, SharedSymbol o2)
{	return new Predicate(Predicate::pDistinct,SharedSymbol(o1),o2);	}


SharedSymbol operator<(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pLess,o1,o2);	}

SharedSymbol operator<(SharedSymbol o1, int o2)
{	return new Predicate(Predicate::pLess,o1,SharedSymbol(o2));	}

SharedSymbol operator<(int o1, SharedSymbol o2)
{	return new Predicate(Predicate::pLess,SharedSymbol(o1),o2);	}

SharedSymbol operator<(SharedSymbol o1, bool o2)
{	return new Predicate(Predicate::pLess,o1,SharedSymbol(o2));	}

SharedSymbol operator<(bool o1, SharedSymbol o2)
{	return new Predicate(Predicate::pLess,SharedSymbol(o1),o2);	}


SharedSymbol operator<=(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pLessEqual,o1,o2);	}

SharedSymbol operator<=(SharedSymbol o1, int o2)
{	return new Predicate(Predicate::pLessEqual,o1,SharedSymbol(o2));	}

SharedSymbol operator<=(int o1, SharedSymbol o2)
{	return new Predicate(Predicate::pLessEqual,SharedSymbol(o1),o2);	}

SharedSymbol operator<=(SharedSymbol o1, bool o2)
{	return new Predicate(Predicate::pLessEqual,o1,SharedSymbol(o2));	}

SharedSymbol operator<=(bool o1, SharedSymbol o2)
{	return new Predicate(Predicate::pLessEqual,SharedSymbol(o1),o2);	}


SharedSymbol operator>(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pGreater,o1,o2);	}

SharedSymbol operator>(SharedSymbol o1, int o2)
{	return new Predicate(Predicate::pGreater,o1,SharedSymbol(o2));	}

SharedSymbol operator>(int o1, SharedSymbol o2)
{	return new Predicate(Predicate::pGreater,SharedSymbol(o1),o2);	}

SharedSymbol operator>(SharedSymbol o1, bool o2)
{	return new Predicate(Predicate::pGreater,o1,SharedSymbol(o2));	}

SharedSymbol operator>(bool o1, SharedSymbol o2)
{	return new Predicate(Predicate::pGreater,SharedSymbol(o1),o2);	}

SharedSymbol operator>=(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pGreaterEqual,o1,o2);	}

SharedSymbol operator>=(SharedSymbol o1, int o2)
{	return new Predicate(Predicate::pGreaterEqual,o1,SharedSymbol(o2));	}

SharedSymbol operator>=(int o1, SharedSymbol o2)
{	return new Predicate(Predicate::pGreaterEqual,SharedSymbol(o1),o2);	}

SharedSymbol operator>=(SharedSymbol o1, bool o2)
{	return new Predicate(Predicate::pGreaterEqual,o1,SharedSymbol(o2));	}

SharedSymbol operator>=(bool o1, SharedSymbol o2)
{	return new Predicate(Predicate::pGreaterEqual,SharedSymbol(o1),o2);	}

SharedSymbol operator!(SharedSymbol o1)
{	return new Predicate(Predicate::pNot,o1);	}

SharedSymbol operator&&(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pAnd,o1,o2);	}

SharedSymbol operator&&(SharedSymbol o1, bool o2)
{	return new Predicate(Predicate::pAnd,o1,SharedSymbol(o2));	}

SharedSymbol operator&&(bool o1, SharedSymbol o2)
{	return new Predicate(Predicate::pAnd,SharedSymbol(o1),o2);	}

SharedSymbol operator||(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pOr,o1,o2);	}

SharedSymbol operator||(SharedSymbol o1, bool o2)
{	return new Predicate(Predicate::pOr,o1,SharedSymbol(o2));	}

SharedSymbol operator||(bool o1, SharedSymbol o2)
{	return new Predicate(Predicate::pOr,SharedSymbol(o1),o2);	}

SharedSymbol operator^(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pXOr,o1,o2);	}

SharedSymbol operator^(SharedSymbol o1, bool o2)
{	return new Predicate(Predicate::pXOr,o1,SharedSymbol(o2));	}

SharedSymbol operator^(bool o1, SharedSymbol o2)
{	return new Predicate(Predicate::pXOr,SharedSymbol(o1),o2);	}

SharedSymbol channel(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pChannel,o1,o2);	}

SharedSymbol contained(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pContained,o1,o2);	}

SharedSymbol member(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pMember,o1,o2);	}

SharedSymbol inTable(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pInTable,o1,o2);	}

SharedSymbol notInTable(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pNotInTable,o1,o2);	}

SharedSymbol disjoint(SharedSymbol o1, SharedSymbol o2)
{	return new Predicate(Predicate::pDisjoint,o1,o2);	}

}
}
