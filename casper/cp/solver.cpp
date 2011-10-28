/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <casper/cp/solver.h>

using namespace std;

namespace Casper {
namespace CP {

const StateStats& SolverStats::getStateStats() const
{	return static_cast<const Casper::State&>(solver).getStats();	}

const StoreStats& SolverStats::getStoreStats() const
{	return static_cast<const Casper::CP::Store&>(solver).getStats();	}

const ExplorerStats& SolverStats::getExplorerStats() const
{	return solver.getExplorer()->getStats();	}

Solver::Solver() :
		Env(),
		Store(static_cast<Env&>(*this)),
		pExplorer(*this,new (*this) DFSExplorer(*this)),
		stats(*this)
{}

} // CP
} // Casper

ostream& operator<<(ostream& os, const Casper::CP::SolverStats& s)
{
	os << s.getStateStats() << std::endl;
	os << s.getStoreStats() << std::endl;
	os << s.getExplorerStats();
	return os;
}
