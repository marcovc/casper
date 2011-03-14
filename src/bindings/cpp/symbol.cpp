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

#include "symbol.h"
#include "exception.h"

#include "variable.h"
#include "expression.h"
#include "predicate.h"
#include "instance.h"

namespace Casperbind {
namespace cpp {

const unsigned int Symbol::versionMajor = 0;
const unsigned int Symbol::versionMinor = 1;
const unsigned int Symbol::versionRelease = 0;

Symbol::operator bool() const
{
	return static_cast<bool>(skipPtrs<Detail::Constant<bool,sBool> >(*this,sBool,"bool"));
}

Symbol::operator int() const
{
	return static_cast<int>(skipPtrs<Detail::Constant<int,sInt> >(*this,sInt,"int"));
}

Symbol::operator double() const
{
	return static_cast<double>(skipPtrs<Detail::Constant<double,sDouble> >(*this,sDouble,"double"));
}

Symbol::operator const char*() const
{
	return static_cast<const char*>(skipPtrs<Detail::Constant<const char*,sString> >(*this,sString,"String"));
}

Symbol::operator const Container&() const
{
	Symbol::Type tf = getType(false);
	if (tf==Symbol::sArray or tf==Symbol::sRange or tf==Symbol::sSet)
		return static_cast<const Container&>(*this);
	if (tf==Symbol::sSymbol)
		return static_cast<const Container&>(static_cast<const SharedSymbol&>(*this).get());
	throw ETypeConversion(*this,"Container");
}

Symbol::operator const Variable&() const
{
	return skipPtrs<Variable>(*this,sVariable,"Variable");
/*
	if (getType(true)!=sVariable)
		throw ETypeConversion(*this,"Variable");
	if (getType()==sSymbol)
		return static_cast<const Variable&>(static_cast<const SharedSymbol&>(*this).get());
	return static_cast<const Variable&>(*this);*/
}

Symbol::operator const Expression&() const
{
	return skipPtrs<Expression>(*this,sExpression,"Expression");
}

Symbol::operator const Predicate&() const
{
	return skipPtrs<Predicate>(*this,sPredicate,"Predicate");
}

Symbol::operator const Annotation&() const
{
	return skipPtrs<Annotation>(*this,sAnnotation,"Annotation");
}

Symbol::operator const Instance&() const
{
	return skipPtrs<Instance>(*this,sInstance,"Instance");
}
/*
Symbol::operator const SymbolArray&() const
{
	if (getType()!=Array or
		static_cast<const Container&>(*this).getElemType()!=sSymbol)
		throw ETypeConversion(*this,"SymbolArray");
	return static_cast<const SymbolArray&>(*this);
}*/

void Symbol::assertVersionIn(unsigned int lbMajor,
						   unsigned int lbMinor,
						   unsigned int lbRelease,
						   unsigned int ubMajor,
						   unsigned int ubMinor,
						   unsigned int ubRelease)
{
	if (Symbol::versionMajor > ubMajor or Symbol::versionMajor < lbMajor)
		throw EVersion();
	if (Symbol::versionMajor == ubMajor)
	{
		if (Symbol::versionMinor > ubMinor)
			throw EVersion();
		if (Symbol::versionMinor == ubMinor and
			Symbol::versionRelease > ubRelease)
			throw EVersion();
	}
	if (Symbol::versionMajor == lbMajor)
	{
		if (Symbol::versionMinor < lbMinor)
			throw EVersion();
		if (Symbol::versionMinor == lbMinor and
			Symbol::versionRelease < lbRelease)
			throw EVersion();
	}
}

SharedSymbol::SharedSymbol(bool i) : pData(new bool(i)) {}
SharedSymbol::SharedSymbol(int i) : pData(new int(i)) {}
SharedSymbol::SharedSymbol(unsigned int i) : pData(new int(i)) {}
SharedSymbol::SharedSymbol(double i)  : pData(new double(i)) {}

}
}
