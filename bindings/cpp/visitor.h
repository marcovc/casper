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
 

#ifndef CASPER_BINDINGS_CPP_VISITOR_H_
#define CASPER_BINDINGS_CPP_VISITOR_H_

#include "cpp.h"

// general interface

struct IVisitor
{	virtual void operator()(const Casperbind::cpp::Symbol& s) = 0; };

void visitTopDown(const Casperbind::cpp::Symbol& s, IVisitor& v);

// useful visits
void collectType(const Casperbind::cpp::Symbol& s,
				 Casperbind::cpp::Symbol::Type type,
				 std::set<const Casperbind::cpp::Symbol*>& collection);


#endif /* CASPER_BINDINGS_CPP_VISITOR_H_ */
