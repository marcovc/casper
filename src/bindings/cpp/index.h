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

#ifndef CASPER_BINDINGS_CPP_XINFO_H_
#define CASPER_BINDINGS_CPP_XINFO_H_

#include "symbol.h"
#include <map>
#include <list>

namespace Casperbind {
namespace cpp {

struct Index
{
	typedef std::map<const Symbol*,std::list<std::string> > KeysFrom;
	typedef std::map<std::string,SharedSymbol> KeysTo;
	KeysFrom keysFrom;
	KeysTo keysTo;

	Index();
	~Index() {}

	void add(SharedSymbol s, const std::string& k);

	const std::list<std::string>& getKeys(const Symbol& s) const;
	const SharedSymbol& getSymbol(const std::string& s) const;

	bool hasKeys(const Symbol& s) const;
	bool hasSymbol(std::string s) const;
};

}
}


#endif /* CASPER_BINDINGS_CPP_XINFO_H_ */
