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
 

#include <casper/kernel/state/env.h>


ostream& operator<<(ostream& os, const Casper::EnvStats& s)
{
	os  << left << setw (30) << "Number of Boolean domains" << ":" <<  std::setw (10) << std::right
							  << s.getNbBoolDomains() << std::endl
	   << left << setw (30) << "Number of integer domains" << ":" <<  std::setw (10) << std::right
							  << s.getNbIntDomains() << std::endl
	   << left << setw (30) << "Number of set domains" << ":" <<  std::setw (10) << std::right
							  << s.getNbSetDomains() << std::endl
	   << left << setw (30) << "Number of range domains" << ":" <<  std::setw (10) << std::right
							  << s.getNbRangeDomains();
	return os;
}


