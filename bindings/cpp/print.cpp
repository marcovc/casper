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

#include "print.h"
#include "visitor.h"

using namespace Casperbind::cpp;

SymStream& operator<<(SymStream& os, const Casperbind::cpp::Index& s);


template<class T>
SymStream& printArrayContents(SymStream& os, const T& s)
{
	int tcells = 1;
	int ncells[s.dims];
	for (int i = s.dims-1; i >= 0; --i)
	{
		tcells *= s.sizes[i];
		ncells[i] = tcells;
	}
	for (int i = 0; i < tcells; ++i)
	{
		os << s.data[i];
		bool sep = false;
		for (int d = 1; d < s.dims; ++d)
			if ((i+1) % ncells[d] == 0 and i+1<tcells-1)
			{
				sep = true;
				for (int sp = 0; sp < s.dims-d; ++sp)
					os << "|";
			}
		if (!sep and i < tcells-1)
			os << ",";
	}
	return os;
}

template<class InputIterator>
SymStream& printSetContents(SymStream& os, InputIterator b, InputIterator e)
{
	os << "{";
	if (b!=e)
	{
		os << *b;
		while (++b!=e)
			os << "," << *b;
	}
	os << "}";
	return os;
}

SymStream& printAddress(SymStream& os, const Symbol& s)
{
	if (os.index.hasKeys(s))
	{
		if (os.index.getKeys(s).size()>1)
		{
			os << "{";
			std::list<std::string>::const_iterator it = os.index.getKeys(s).begin();
			os << *it;
			++it;
			for ( ; it != os.index.getKeys(s).end(); ++it)
				os << "," << *it;
			os << "}";
		}
		else
			os << os.index.getKeys(s).front();
	}
	else
		os << &s;
	return os;
}

SymStream& printArray(SymStream& os, const Symbol& s)
{
	if (os.showIdentifier())
		os << "array";
	if (os.showAddress())
	{
		if (os.showIdentifier())
			os << "_";
		printAddress(os,s);
	}
	if (!os.showArrayContents())
		return os;
	if (os.showIdentifier() or os.showAddress())
		os << ":";
	switch (static_cast<const Container&>(s).getElemType())
	{
		case Container::sBool:
		{
			const BoolArray& ia = s;
			if (os.showDelimiters())
				return printArrayContents(os << "[",ia) << "]";
			else
				return printArrayContents(os,ia);
		}
		case Container::sInt:
		{
			const IntArray& ia = s;
			if (os.showDelimiters())
				return printArrayContents(os << "[",ia) << "]";
			else
				return printArrayContents(os,ia);
		}
		case Container::sSymbol:
		{
			const SymbolArray& ia = s;
			if (os.showDelimiters())
				return printArrayContents(os << "[",ia) << "]";
			else
				return printArrayContents(os,ia);
		}
		default:
			throw EImplement(s);
	}
	return os;
}

SymStream& printRange(SymStream& os, const Symbol& s)
{
	os << "range_";
	printAddress(os,s);
	if (!os.showRangeContents())
		return os;
	os << ":";
	switch (static_cast<const Container&>(s).getElemType())
	{
		case Container::sInt:
		{
			const IntRange& ia = s;
			return os << "[" << ia.getLower() << ".." << ia.getUpper() << "]";
		}
		case Container::sDouble:
		{
			const RealRange& ia = s;
			return os << "[" << ia.getLower() << ";" << ia.getUpper() << "]";
		}
		case Container::sSymbol:
		{
			const SymbolRange& ia = s;
			return os << "[" << ia.getLower() << ";" << ia.getUpper() << "]";
		}

		default:
			throw EImplement(s);
	}
	return os;
}


SymStream& printSet(SymStream& os, const Symbol& s)
{
	os << "set_";
	printAddress(os,s);
	if (!os.showSetContents())
		return os;
	os << ":";
	switch (static_cast<const Container&>(s).getElemType())
	{
		case Container::sBool:
			return printSetContents(os,static_cast<const BoolSet&>(s).begin(),
									   static_cast<const BoolSet&>(s).end());
		case Container::sInt:
			return printSetContents(os,static_cast<const IntSet&>(s).begin(),
									   static_cast<const IntSet&>(s).end());
		case Container::sSymbol:
			return printSetContents(os,static_cast<const SymbolSet&>(s).begin(),
									   static_cast<const SymbolSet&>(s).end());
		default:
			throw EImplement(s);
	}
}


SymStream& printVariable(SymStream& os, const Symbol& s)
{
	os << "var_";
	printAddress(os,s);
	if (!os.showVariableContents())
		return os;
	const Variable& v = s;
	os << ":" << v.getDomain();
	return os;
}

SymStream& printBool(SymStream& os, const Symbol& s)
{
	return os << static_cast<bool>(s);
}

SymStream& printInt(SymStream& os, const Symbol& s)
{
	return os << static_cast<int>(s);
}

SymStream& printDouble(SymStream& os, const Symbol& s)
{
	return os << static_cast<double>(s);
}

SymStream& printString(SymStream& os, const Symbol& s)
{
	return os << static_cast<const char*>(s);
}

SymStream& printExpression(SymStream& os, const Symbol& s)
{
	const Expression& e = s;
	switch (e.getOperator())
	{
		case Expression::eSym:
			return os << "sym(" << e[0] << ")";
		case Expression::eAbs:
			return os << "abs(" << e[0] << ")";
		case Expression::eAdd:
			return os << "add(" << e[0] << "," << e[1] << ")";
		case Expression::eSub:
			return os << "sub(" << e[0] << "," << e[1] << ")";
		case Expression::eMul:
			return os << "mul(" << e[0] << "," << e[1] << ")";
		case Expression::eDiv:
			return os << "div(" << e[0] << "," << e[1] << ")";
		case Expression::eMod:
			return os << "mod(" << e[0] << "," << e[1] << ")";
		case Expression::ePow:
			return os << "pow(" << e[0] << "," << e[1] << ")";
		case Expression::eMin:
			return os << "min(" << e[0] << "," << e[1] << ")";
		case Expression::eMax:
			return os << "max(" << e[0] << "," << e[1] << ")";
		case Expression::eIfThenElse:
			return os << "ifThenElse(" << e[0] << "," << e[1] << "," << e[2] << ")";
		case Expression::eElem:
			return os << e[0] << "[" << e[1] << "]";
		case Expression::eCard:
			return os << "card(" << e[0] << ")";
		case Expression::eIntersect:
			return os << "intersect(" << e[0] << "," << e[1] << ")";
		case Expression::eUnion:
			return os << "union(" << e[0] << "," << e[1] << ")";
		case Expression::eSymDiff:
			return os << "symdiff(" << e[0] << "," << e[1] << ")";
		case Expression::eSumProduct:
			return os << "sumProduct(" << e[0] << "," << e[1] << ")";
	}
	throw EImplement(s);
}

SymStream& printPredicate(SymStream& os, const Symbol& s)
{
	os << "con_";
	printAddress(os,s);
	if (!os.showPredicateContents())
		return os;
	os.showPredicateContents()=false;
	os << ":";
	const Predicate& e = s;
	switch (e.getFunctor())
	{
		case Predicate::pEqual:
			os << "equal(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pLess:
			os << "less(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pGreater:
			os << "greater(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pLessEqual:
			os << "lessEqual(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pGreaterEqual:
			os << "greaterEqual(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pDistinct:
			os << "distinct(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pNot:
			os << "not(" << e[0] << ")";
			break;
		case Predicate::pAnd:
			os << "and(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pOr:
			os << "or(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pXOr:
			os << "xor(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pIff:
			os << "iff(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pIf:
			os << "if(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pContained:
			os << "contained(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pMember:
			os << "member(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pInTable:
			os << "inTable(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pNotInTable:
			os << "notInTable(" << e[0] << "," << e[1] << ")";
			break;
		case Predicate::pDisjoint:
			os << "disjoint(" << e << ")";
			break;
		case Predicate::pCumulative:
			os << "cumulative(" << e << ")";
			break;
		case Predicate::pPartition:
			os << "partition(" << e << ")";
			break;
	}
	os.showPredicateContents()=true;
	return os;
}

SymStream& printAnnotation(SymStream& os, const Symbol& s)
{
	os << "ann_";
	printAddress(os,s);
	if (!os.showAnnotationContents())
		return os;
	os.showAnnotationContents()=false;
	os << ":";
	const Annotation& a = s;
	os << a.getFunctor();
	if (a.getArity()>0)
		os << "(" << a[0];
	for (int i = 1; i < a.getArity(); ++i)
		os << "," << a[i];
	if (a.getArity()>0)
		os << ")";
	os.showAnnotationContents()=true;
	return os;
}

template<class InputIterator>
SymStream& printSetVertically(SymStream& os, InputIterator b, InputIterator e)
{
	assert(b!=e);
	while (b!=e)
		os << *(b++) << std::endl;
	return os;
}

SymStream& printInstance(SymStream& os, const Symbol& s)
{
	const Instance& i = s;
	std::set<const Symbol*> rangeSet;
	std::set<const Symbol*> arraySet;
	std::set<const Symbol*> setSet;
	std::set<const Symbol*> variableSet;
	std::set<const Symbol*> predicateSet;

	const SymbolArray& constraints = i.getConstraints();
	collectType(constraints,Symbol::sRange,rangeSet);
	for (int i = 0; i < constraints.getSize(); ++i)
		collectType(constraints[i],Symbol::sArray,arraySet);
	collectType(constraints,Symbol::sSet,setSet);
	collectType(constraints,Symbol::sVariable,variableSet);
	collectType(constraints,Symbol::sPredicate,predicateSet);
	os.showAll(true);
	if (!rangeSet.empty())
	{
		os << "+++ ranges +++" << std::endl;
		printSetVertically(os,rangeSet.begin(),rangeSet.end());
		os << std::endl;
	}
	os.showRangeContents() = false;
	if (!setSet.empty())
	{
		os << "+++ sets +++" << std::endl;
		printSetVertically(os,setSet.begin(),setSet.end());
		os << std::endl;
	}
	os.showSetContents() = false;
	if (!arraySet.empty())
	{
		os << "+++ arrays +++" << std::endl;
		printSetVertically(os,arraySet.begin(),arraySet.end());
		os << std::endl;
	}
	os.showArrayContents() = false;
	if (!variableSet.empty())
	{
		os << "+++ variables +++" << std::endl;
		printSetVertically(os,variableSet.begin(),variableSet.end());
		os << std::endl;
	}
	os.showVariableContents() = false;
	if (!predicateSet.empty())
	{
		os << "+++ constraints +++" << std::endl;
		printSetVertically(os,predicateSet.begin(),predicateSet.end());
		os << std::endl;
	}
	os.showAll(true);
/*	os << "variables:\n";
	os << i.getVariables() << std::endl;
	os << "constraints:\n";
	os << i.getConstraints() << std::endl;
	os << "index:\n";
	os << i.getIndex() << std::endl;
*/	return os;
}

SymStream& printSharedSymbol(SymStream& os, const Symbol& s)
{
//	const SharedSymbol& ss = s;
//	os << ss.get();

	os << static_cast<const SharedSymbol&>(s).get();

/*	if (os.index.hasSymbol(s))
		os << "_" << os.index.getKey(s);
	else
		os << "_" << &s;*/
	return os;
}

SymStream& operator<<(SymStream& os, const Symbol& s)
{
	SharedSymbol::assertVersionIn(0,0,0,0,1,0);

	using namespace Casperbind::cpp;
	switch (s.getType())
	{
		case Symbol::sArray:
			return printArray(os,s);
		case Symbol::sRange:
			return printRange(os,s);
		case Symbol::sSet:
			return printSet(os,s);
		case Symbol::sVariable:
			return printVariable(os,s);
		case Symbol::sBool:
			return printBool(os,s);
		case Symbol::sInt:
			return printInt(os,s);
		case Symbol::sDouble:
			return printDouble(os,s);
		case Symbol::sString:
			return printString(os,s);
		case Symbol::sExpression:
			return printExpression(os,s);
		case Symbol::sPredicate:
			return printPredicate(os,s);
		case Symbol::sInstance:
			return printInstance(os,s);
		case Symbol::sSymbol:
			return printSharedSymbol(os,s);
		case Symbol::sAnnotation:
			return printAnnotation(os,s);
	}
	return os;
}


SymStream& operator<<(SymStream& os, const Casperbind::cpp::Index& s)
{
	typedef Casperbind::cpp::Index::KeysTo KeysTo;
	os << "{";
	for (KeysTo::const_iterator it = s.keysTo.begin();
			it != s.keysTo.end(); ++it)
		os << it->first << ":" << it->second << ",";
	os << "}";
	return os;
}

