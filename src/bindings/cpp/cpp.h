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

#ifndef CASPER_BINDINGS_CPP_CPP_H_
#define CASPER_BINDINGS_CPP_CPP_H_

/**
 *	\defgroup CasperBindings CaSPER bindings.
 *	Interfaces CaSPER with other programming and
 *	constraint modeling languages.
 */

/**
 *	\defgroup CasperBindingsCPP CaSPER bindings for C++.
 *	A set of c++ classes for representing CSP's.
 *	\ingroup CasperBindings
 */

/**
 *	\defgroup CasperBindingsCPPProcessors Processors for c++ bindings.
 *	Functions for processing data described using cpp::Symbol api.
 *	\ingroup CasperBindingsCPP
 */

/**
 *	\namespace casperbind
 *  \ingroup CasperBindings
 */

/**
 *	\namespace casperbind::cpp
 *  \ingroup CasperBindingsCPP
 */

/**
 *  Include this file to include all headers from bindings/cpp
 *
 */

#include "symbol.h"
#include "container.h"
#include "variable.h"
#include "expression.h"
#include "predicate.h"
#include "index.h"
#include "instance.h"
#include "solver.h"

#endif /* CASPER_BINDINGS_CPP_CPP_H_ */
