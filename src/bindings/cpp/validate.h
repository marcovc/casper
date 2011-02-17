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


#ifndef CASPER_BINDINGS_CPP_VALIDATE_H_
#define CASPER_BINDINGS_CPP_VALIDATE_H_

#include "cpp.h"
#include <iostream>

namespace casperbind {
namespace cpp {

/**
 * 	Checks whether the symbol \a s is well formed (syntactically).
 * 	\return A pointer to an invalid subexpression of s if one is found,
 * 	NULL otherwise.
 *  \ingroup CasperBindingsCPPProcessors
 */
const Symbol* validate(const Symbol& s);

const Symbol* validateIntEval(const Symbol& s);
const Symbol* validateSetEval(const Symbol& s);
const Symbol* validateBoolEval(const Symbol& s);

}
}


#endif /* CASPER_BINDINGS_CPP_VALIDATE_H_ */
