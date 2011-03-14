/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <casper/util/timer.h>

namespace Casper {
namespace Util {

void WallTimer::print(std::ostream& os) const
{
	WallTimer t(*this);
	t.pause();
	os << std::left << std::setw (30) << t.name << ":" << std::setw(10) << std::right
	   << std::setprecision (3) << t.secs;
}

void CPUTimer::print(std::ostream& os) const
{
	CPUTimer t(*this);
	t.pause();
	os << std::left << std::setw (30) << t.name << ":" << std::setw(10) << std::right
	   << std::setprecision (3) << t.secs;
}

} // Util
} // Casper


std::ostream& operator<<(std::ostream& os, const Casper::Util::ITimer& t)
{	t.print(os); return os;	}
