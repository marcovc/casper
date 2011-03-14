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

#ifndef CASPER_BINDINGS_CPP_INSTANCE_H_
#define CASPER_BINDINGS_CPP_INSTANCE_H_

#include "index.h"
#include "container.h"
#include "annotations.h"

namespace Casperbind {
namespace cpp {

/**
 * 	Represents a CSP instance.
 * \ingroup CasperBindingsCPP
 */
struct Instance : Symbol
{
	Index index;
	Annotations annotations;
	const SymbolSet variables;
	const SymbolSet constraints;

	Instance(const SymbolSet& variables,
			 const SymbolSet& constraints) :
				 variables(variables),constraints(constraints) {}
	Instance(const Index& index,
			 const SymbolSet& variables,
			 const SymbolSet& constraints) :
				 index(index),variables(variables),constraints(constraints) {}
	Instance(const Index& index,
			 const Annotations& annotations,
			 const SymbolSet& variables,
			 const SymbolSet& constraints) :
				 index(index),annotations(annotations),variables(variables),constraints(constraints) {}
	~Instance() {}
	Type getType(bool) const {	return sInstance; }
	const SymbolSet& getVariables() const { return variables;}
	const SymbolSet& getConstraints() const { return constraints;}
	const Symbol* clone() const { return new Instance(variables,constraints); }
	const Index& getIndex() const { return index; }
	Index& getIndex() { return index; }
	const Annotations& getAnnotations() const { return annotations; }
	Annotations& getAnnotations() { return annotations; }
};


}
}

#endif /* CASPER_BINDINGS_CPP_INSTANCE_H_ */
