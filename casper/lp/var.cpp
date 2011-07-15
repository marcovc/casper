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
 

#include <casper/lp/var.h>
#include <casper/lp/solver.h>

#include <casper/util/exception.h>

namespace Casper {
namespace LP {

Var::Var(Solver& solver) : solver(solver), id(solver,solver.newVar())
{}

const Var& Var::operator=(const Var& s)
{
	if (&s.solver != &solver)
		throw Exception::InvalidOperation("assigning to an LP::Var associated with a distinct LP::Solver object");
	id = s.id;
	return *this;
}

double Var::getCurValue() const
{	return solver.getValue(id);	}


void Var::getActivityRange(double& lb, double& ub) const
{	solver.getActivityRange(id,lb,ub);	}


} // LP
} // Casper

std::ostream& operator<<(std::ostream& os, const Casper::LP::Var& v)
{
	os << v.getCurValue();
	return os;
}
