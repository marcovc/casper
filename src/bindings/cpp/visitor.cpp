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
 
#include "visitor.h"
#include "print.h"

using namespace Casperbind::cpp;

void visitTopDown(const Symbol& s, IVisitor& v)
{
	SharedSymbol::assertVersionIn(0,0,0,0,1,0);

	// no shared symbols of shared symbols
	assert(s.getType() != Symbol::sSymbol);

	v(s);

	using namespace Casperbind::cpp;
	switch (s.getType())
	{
		case Symbol::sSet:
			if (static_cast<const Container&>(s).getElemType()==Container::sSymbol)
			{
				const SymbolSet& ss = s;
				for (SymbolSet::ConstIterator it = ss.begin(); it != ss.end(); ++it)
					visitTopDown(*it,v);
			}
			break;
		case Symbol::sArray:
			if (static_cast<const Container&>(s).getElemType()==Container::sSymbol)
			{
				const SymbolArray& ss = s;
				for (int i = 0; i < ss.getSize(); ++i)
					visitTopDown(ss[i],v);
			}
			break;
		case Symbol::sRange:
			if (static_cast<const Container&>(s).getElemType()==Container::sSymbol)
			{
				const SymbolRange& ss = s;
				visitTopDown(ss.getLower(),v);
				visitTopDown(ss.getUpper(),v);
			}
			break;

		case Symbol::sVariable:
			visitTopDown(static_cast<const Variable&>(s).getDomain(),v);
			break;
		case Symbol::sExpression:
			for (int i = 0; i < static_cast<const Expression&>(s).getArity(); ++i)
				visitTopDown(static_cast<const Expression&>(s)[i],v);
			break;
		case Symbol::sPredicate:
			for (int i = 0; i < static_cast<const Predicate&>(s).getArity(); ++i)
				visitTopDown(static_cast<const Predicate&>(s)[i],v);
			break;
		case Symbol::sInstance:
			visitTopDown(static_cast<const Instance&>(s).getVariables(),v);
			visitTopDown(static_cast<const Instance&>(s).getConstraints(),v);
			break;
		default:;
	}
}

struct MatchTypePred
{
	Symbol::Type type;
	MatchTypePred(Symbol::Type type) : type(type) {}
	bool operator()(const Symbol& s) const
	{	return s.getType() == type; }
};

template<class MatchPred>
struct CollectVisitor : IVisitor
{
	MatchPred match;
	std::set<const Symbol*>& collection;
	CollectVisitor(const MatchPred& match, std::set<const Symbol*>& collection) :
		match(match),collection(collection) {}
	void operator()(const Symbol& s)
	{
		if (match(s))
			collection.insert(&s);
	}
};

void collectType(const Symbol& s,Symbol::Type type, std::set<const Symbol*>& collection)
{
	CollectVisitor<MatchTypePred> visitor(MatchTypePred(type),collection);
	visitTopDown(s,visitor);
}

