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

#include "index.h"
#include "exception.h"
#include "symbol.h"

#include <map>
#include <iostream>

namespace casperbind {
namespace cpp {

Index::Index()
{
}

/*
const SharedSymbol* p;

// create a new SharedSymbol if not already
if (s.getType() != Symbol::sSymbol)
	p = new SharedSymbol(s);
else
	p = &static_cast<const SharedSymbol&>(s);
*/

void Index::add(SharedSymbol s, const std::string& k)
{
	if (keysTo.find(k)!=keysTo.end())
	{
		throw EDuplicateKey(s,k);
	}

	keysTo[k] = s;
	keysFrom[&s.get()].push_back(k);
}

const std::list<std::string>& Index::getKeys(const Symbol& s) const
{
	const Symbol* p = &s;
	if (s.getType() == Symbol::sSymbol)
		p = &static_cast<const SharedSymbol&>(s).get();
	KeysFrom::const_iterator it = keysFrom.find(p);
	if (it==keysFrom.end())
		throw ENoKeyForSymbol(s);

	return it->second;
}

bool Index::hasSymbol(std::string s) const
{
	KeysTo::const_iterator it = keysTo.find(s);
	return it!=keysTo.end();
}

const SharedSymbol& Index::getSymbol(const std::string& s) const
{
	KeysTo::const_iterator it = keysTo.find(s);
	if (it==keysTo.end())
		throw ENoSymbolForKey(s);

	return it->second;
}

bool Index::hasKeys(const Symbol& s) const
{
	const Symbol* p = &s;
	if (s.getType() == Symbol::sSymbol)
		p = &static_cast<const SharedSymbol&>(s).get();
	KeysFrom::const_iterator it = keysFrom.find(p);
	return it!=keysFrom.end();
}

}
}

