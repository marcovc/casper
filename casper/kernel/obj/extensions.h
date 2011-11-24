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
 

#ifndef CASPER_KERNEL_OBJ_EXTENSIONS_H_
#define CASPER_KERNEL_OBJ_EXTENSIONS_H_

#include <casper/cp/store.h>

namespace Casper {
namespace CP  {

#if defined(SWIG) | defined(SWIG_BUILD)
bool Store::post(const Casper::Expr<bool>& expr,
		  Casper::CP::Consistency consistency)
{
	switch (consistency)
	{
		case Casper::CP::Domain:
			return expr.postDomFilter(*this);
		case Casper::CP::Bounds:
			return expr.postBndFilter(*this);
		case Casper::CP::Value:
			return expr.postValFilter(*this);
		default:
			throw Casper::Exception::InvalidOperation("invalid value for consistency parameter");
	}
}
#endif

}
}

#endif /* CASPER_KERNEL_OBJ_EXTENSIONS_H_ */
