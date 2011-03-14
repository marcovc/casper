/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2009 - Sergio Silva <email>           				   		   *
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

#ifndef CASPER_BINDINGS_CPP_SYMBOL_H_
#define CASPER_BINDINGS_CPP_SYMBOL_H_

#include <boost/shared_ptr.hpp>
#include "exception.h"

/**
 * 	Bindings (API's for different programming languages) for the
 * 	CaSPER library.
 */
namespace Casperbind {

/**
 * 	C++ bindings. Provides a structured set of c++ classes describing CSP
 * 	instances.
 */
namespace cpp {

// required forward declarations


struct Container;

struct Variable;
struct Expression;
struct Predicate;
struct Instance;
struct Annotation;

namespace Detail {
template<class> struct Array;
template<class> struct StdRange;
template<class,class> struct Set;

}


/**
 * 	Base class for every class in cpp bindings.
 *  \ingroup CasperBindingsCPP
 */
struct Symbol
{
	/// Possible types of Symbol
	enum Type {sArray, sRange, sSet,
		 sVariable, sBool, sInt, sDouble,sString,
		sExpression,sPredicate,sInstance,sAnnotation,
		sSymbol};

	/**
	 *  Returns the specific type of the Symbol. If skipPtrs is true then
	 *  the routine ignores any indirections caused by existing pointers.
	 */
	virtual Type getType(bool skipPtrs = false) const = 0;

	/**
	 *   Returns a (shallow) copy of the Symbol.
	 *   TODO: introduce a bool flag for deep copies if required
	 */
	virtual const Symbol* clone() const = 0;

	//bool operator==(const Symbol& s) const { return this == &s; }
	//bool operator!=(const Symbol& s) const { return this != &s; }

	/// \name Conversion functions. Useful for extracting the Symbol type.
	// \{
	operator bool() const;
	operator int() const;
	operator double() const;
	operator const char*() const;

	operator const Container&() const;

	operator const Variable&() const;
	operator const Expression&() const;
	operator const Predicate&() const;
	operator const Instance&() const;

	operator const Annotation&() const;

	template<class T>
	operator const Array<T>&() const;
	template<class T,class C>
	operator const Detail::Set<T,C>&() const;
	template<class T>
	operator const StdRange<T>&() const;

	// creates a copy (temp)
	template<class T>
	operator Array<T>() const;

	// \}

	/**
	 * 	Asserts that the current Symbol language version is
	 * 	between the given version interval. Throws an EVersion
	 *  exception if this is not the case.
	 */

	static void assertVersionIn(unsigned int lbMajor,
							  unsigned int lbMinor,
							  unsigned int lbRelease,
							  unsigned int ubMajor,
							  unsigned int ubMinor,
							  unsigned int ubRelease);

	/// Current release version
	static const unsigned int versionRelease;
	/// Current minor version
	static const unsigned int versionMinor;
	/// Current major version
	static const unsigned int versionMajor;
};

template<class T>
const T& skipPtrs(const Symbol& s, const Symbol::Type type, const std::string& stype);
/*
template<class T>
Symbol::operator const Array<T>&() const
{
	const Array<T>& r = skipPtrs<Array<T> >(*this,sArray,"Array");
	ass r.getElemType() == Detail::getElemType<T>();
}*/

template<class T,class C>
Symbol::operator const Detail::Set<T,C>&() const
{
	return skipPtrs<Detail::Set<T,C> >(*this,sSet,"Set");
}

template<class T>
Symbol::operator const StdRange<T>&() const
{
	return skipPtrs<StdRange<T> >(*this,sRange,"StdRange");
}

/**
 * Used for Symbols which may be referenced by multiple objects.
 * \ingroup CasperBindingsCPP
 */
struct SharedSymbol : Symbol
{
	boost::shared_ptr<const Symbol>	pData;
	SharedSymbol() : pData() {}

	// these routines avoid creating SharedSymbols of SharedSymbols
	boost::shared_ptr<const Symbol> getPtrSymbol(const Symbol& s) const
	{
		if (s.getType()==Symbol::sSymbol)
			return static_cast<const SharedSymbol&>(s).pData;
		else
			return boost::shared_ptr<const Symbol>(s.clone());
	}
	boost::shared_ptr<const Symbol> getPtrSymbol(const Symbol* s) const
	{
		if (s->getType()==Symbol::sSymbol)
			return static_cast<const SharedSymbol*>(s)->pData;
		else
			return boost::shared_ptr<const Symbol>(s);
	}

	template<class Deleter>
	boost::shared_ptr<const Symbol> getPtrSymbol(const Symbol* s,Deleter d) const
	{
		if (s->getType()==Symbol::sSymbol)
			return static_cast<const SharedSymbol*>(s)->pData;
		else
			return boost::shared_ptr<const Symbol>(s,d);
	}

	explicit SharedSymbol(const Symbol& s) : pData(getPtrSymbol(s)) {}
	SharedSymbol(const SharedSymbol& s) : pData(s.pData) {}
	SharedSymbol(const Symbol* s) : pData(getPtrSymbol(s)) {}

	SharedSymbol(bool i);
	SharedSymbol(int i);
	SharedSymbol(unsigned int i);
	SharedSymbol(double i);

	template<class Deleter>
	SharedSymbol(const Symbol* s, Deleter d) : pData(getPtrSymbol(s,d)) {}


	~SharedSymbol() {}
	const SharedSymbol& operator=(const SharedSymbol& s)
	{	pData = s.pData; return *this; }
	//bool operator==(const SharedSymbol& s) const { return *pData == *s.pData; }
	//bool operator!=(const SharedSymbol& s) const { return *pData != *s.pData; }

	Type getType(bool skipPtrs = false) const
	{ return (!skipPtrs)?sSymbol:get().getType(skipPtrs); }
	const Symbol* clone() const { return new SharedSymbol(*this);	}
	const Symbol& get() const { assert(pData); return *pData; }
	const Symbol& get()  { assert(pData); return *pData; }
};

// comparing shared symbols
namespace Detail {
struct LtSharedSymbol
{
	bool operator()(const SharedSymbol& s1,const SharedSymbol& s2) const
	{	return &s1.get() < &s2.get();	}
};
}

template<class T>
const T& skipPtrs(const Symbol& s, const Symbol::Type type, const std::string& stype)
{
	if (s.getType(false)==type)
		return static_cast<const T&>(s);
	if (s.getType(false)==Symbol::sSymbol)
		return skipPtrs<T>(static_cast<const SharedSymbol&>(s).get(),type,stype);
		//return static_cast<const T&>(static_cast<const SharedSymbol&>(s).get());
	throw ETypeConversion(s,stype);
}

}
}


#endif /* CASPER_BINDINGS_CPP_SYMBOL_H_ */
