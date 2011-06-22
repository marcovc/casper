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

#ifndef CASPER_BINDINGS_CPP_ANNOTATIONS_H_
#define CASPER_BINDINGS_CPP_ANNOTATIONS_H_

#include "symbol.h"
#include "annotation.h"
#include <map>
#include <list>
#include "print.h" // tmp
#include <iostream>

namespace Casperbind {
namespace cpp {

struct Annotations
{
	typedef std::list<Annotation>::const_iterator Iterator;
	typedef std::map<const Symbol*,std::list<Annotation> > KeysFrom;
	KeysFrom keysFrom;

	Annotations() {}
	~Annotations() {}

	void add(SharedSymbol s, const Annotation& a)
	{	keysFrom[&s.get()].push_back(a); }

	bool hasAnnotations(SharedSymbol s) const
	{	return keysFrom.find(&s.get())!=keysFrom.end(); }

	bool hasAnnotation(SharedSymbol s, const String& ann) const
	{
		if (keysFrom.find(&s.get())==keysFrom.end())
			return false;
		for (Iterator it = begin(s); it != end(s); ++it)
			if (std::string(it->getFunctor()) == std::string(ann))
				return true;
		return false;
	}

	const Annotation& getAnnotation(SharedSymbol s, const String& ann) const
	{
		if (keysFrom.find(&s.get())==keysFrom.end())
			throw ENoKeyForSymbol(s);
		for (Iterator it = begin(s); it != end(s); ++it)
			if (std::string(it->getFunctor()) == std::string(ann))
				return *it;
		assert(0);
	}

	Iterator begin(SharedSymbol s) const
	{
		KeysFrom::const_iterator it = keysFrom.find(&s.get());
		if (it == keysFrom.end())
			throw ENoKeyForSymbol(s);
		return it->second.begin();
	}
	Iterator end(SharedSymbol s) const
	{
		KeysFrom::const_iterator it = keysFrom.find(&s.get());
		if (it == keysFrom.end())
			throw ENoKeyForSymbol(s);
		return it->second.end();
	}
};

}
}


#endif /* CASPER_BINDINGS_CPP_ANNOTATIONS_H_ */
