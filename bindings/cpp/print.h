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

#ifndef CASPER_BINDINGS_CPP_PRINT_H_
#define CASPER_BINDINGS_CPP_PRINT_H_

#include "cpp.h"
#include <iostream>

struct SymStream
{
	std::ostream& os;
	const Casperbind::cpp::Index index;
	bool bShowVariableContents;
	bool bShowSetContents;
	bool bShowArrayContents;
	bool bShowRangeContents;
	bool bShowPredicateContents;
	bool bShowAnnotationContents;
	bool bShowAddress;
	bool bShowDelimiters;
	bool bShowIdentifier;

	SymStream(std::ostream& s) : os(s) { showAll(true); }
	SymStream(std::ostream& s,const Casperbind::cpp::Index& index) :
		os(s),index(index) { showAll(true); }

	void showAll(bool val)
	{
		bShowVariableContents = bShowSetContents =
				bShowArrayContents = bShowRangeContents =
						bShowPredicateContents = bShowAnnotationContents =
								bShowAddress = bShowDelimiters =
										bShowIdentifier = val;
	}
	SymStream& operator<<(int t)
	{	os << t; return *this; }
	SymStream& operator<<(bool t)
	{	os << t; return *this; }
	SymStream& operator<<(double t)
	{	os << t; return *this; }
	SymStream& operator<<(std::string t)
	{	os << t; return *this; }
	template<class T>
	SymStream& operator<<(const T* t)
	{	os << t; return *this; }

	// this is the type of std::cout
	typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
	// this is the function signature of std::endl
	typedef CoutType& (*StandardEndLine)(CoutType&);
	// define an operator<< to take in std::endl
	SymStream& operator<<(StandardEndLine manip)
	{	manip(os);  return *this; }

	const bool& showVariableContents() const { return bShowVariableContents; }
	const bool& showSetContents() const { return bShowSetContents; }
	const bool& showArrayContents() const { return bShowArrayContents; }
	const bool& showRangeContents() const { return bShowRangeContents; }
	const bool& showPredicateContents() const { return bShowPredicateContents; }
	const bool& showAnnotationContents() const { return bShowAnnotationContents; }
	const bool& showAddress() const { return bShowAddress; }
	const bool& showDelimiters() const { return bShowDelimiters; }
	const bool& showIdentifier() const { return bShowIdentifier; }

	bool& showVariableContents() { return bShowVariableContents; }
	bool& showSetContents() { return bShowSetContents; }
	bool& showArrayContents() { return bShowArrayContents; }
	bool& showRangeContents() { return bShowRangeContents; }
	bool& showPredicateContents() { return bShowPredicateContents; }
	bool& showAnnotationContents() { return bShowAnnotationContents; }
	bool& showAddress() { return bShowAddress; }
	bool& showDelimiters() { return bShowDelimiters; }
	bool& showIdentifier() { return bShowIdentifier; }
};

/**
 * 	Streams a text representation of \a s to \a os.
 *  \ingroup CasperBindingsCPPProcessors
 */
SymStream& operator<<(SymStream& os, const Casperbind::cpp::Symbol& s);


inline std::ostream& operator<<(std::ostream& os, const Casperbind::cpp::Symbol& s)
{
	SymStream ss(os);
	ss << s;
	return os;
}

#endif /* CASPER_BINDINGS_CPP_PRINT_H_ */
