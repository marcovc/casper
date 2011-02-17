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


#ifndef CASPER_BINDINGS_CPP_EXCEPTION_H_
#define CASPER_BINDINGS_CPP_EXCEPTION_H_

#include <string>
#include <exception>
#include <sstream>

namespace casperbind {
namespace cpp {

struct Symbol;

/**
 * 	Type conversion exception. Occurs when trying to convert between two
 * 	incompatible Symbols.
 *  \ingroup CasperBindingsCPP
 */
struct ETypeConversion
{
	const Symbol& s;
	const std::string d;
	ETypeConversion(const Symbol& s,const std::string& d) :
		s(s),d(d) {}
};

/**
 * 	Invalid structure exception. Triggered when the structure of a given Symbol
 *  is not as expected.
 *  \ingroup CasperBindingsCPP
 */
struct EStructure : std::exception
{
	const std::string expected;
	const Symbol& s;
	EStructure(const std::string& expected, const Symbol& s) : expected(expected),s(s){}
	~EStructure() throw() {}
	const char* what() const throw()
	{	return  "unexpected structure";	}
	const std::string& getExpected() const { return expected;}
	const Symbol& getSymbol() const { return s; }
};

/**
 * 	Implement exception. Triggered when casper does not implement symbol
 *  \ingroup CasperBindingsCPP
 */
struct EImplement : std::exception
{
	const Symbol& s;
	EImplement(const Symbol& s) : s(s){}
	const char* what() const throw()
	{	return "CaSPER does not implement symbol";}
	const Symbol& getSymbol() const { return s; }
};

/**
 * 	Version mismatch exception.
 *  \ingroup CasperBindingsCPP
 */
struct EVersion
{
	EVersion() {}
};


struct EDuplicateKey
{
	const Symbol& s;
	const std::string k;
	EDuplicateKey(const Symbol& s,const std::string& k) :
		s(s),k(k) {}
};

struct ENoKeyForSymbol
{
	const Symbol& s;
	ENoKeyForSymbol(const Symbol& s) :
		s(s) {}
};
struct ENoSymbolForKey
{
	const std::string s;
	ENoSymbolForKey(const std::string& s) :
		s(s) {}
};

}
}

#endif /* CASPER_BINDINGS_CPP_EXCEPTION_H_ */
