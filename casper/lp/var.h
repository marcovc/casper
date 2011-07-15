/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_LP_VAR_H_
#define CASPER_LP_VAR_H_

#include <iostream>
#include <casper/kernel/reversible/reversible.h>
#include <casper/kernel/traits.h>
#include <casper/kernel/rel/rel.h>

namespace Casper {
namespace LP {

struct Solver;

struct Var
{
	Var(Solver& solver);

	const Var& operator=(const Var& s);

	Solver& getSolver() const {	return solver; }

	double getCurValue() const;
	uint getId() const { return id;	}

	void getActivityRange(double& lb, double& ub) const;

	Solver& solver;
	Reversible<uint> id;
};

} // LP

namespace Traits {

template<>
struct GetEval<LP::Var>
{	typedef double Type;	};

} // Traits

} // Casper

std::ostream& operator<<(std::ostream& os, const Casper::LP::Var& v);

#endif /* CASPER_LP_VAR_H_ */

#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_LP_OP_VAR_H
#define CASPER_LP_OP_VAR_H
#include <casper/lp/op_var.h>
#endif
#endif
