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
 

#ifndef CASPER_MIP_STORE_H_
#define CASPER_MIP_STORE_H_

#include <casper/cp/store.h>
#include <casper/cp/rangedom.h>

#include <casper/lp/solver.h>

#include <casper/lp/lineareqvalview.h>

#include <casper/mip/traits.h>
#include <casper/mip/view.h>

namespace Casper {
namespace MIP {

struct Store
{
	Store(CP::Store& cp, LP::Solver& lp) : cp(cp),lp(lp) {}

	template<class T>
	bool post(const T& r);

	operator Env&() { return cp; }
	operator const Env&() const { return cp; }

	operator State&() { return cp; }
	operator const State&() const { return cp; }

	operator Util::IHeap&() { return cp; }
	operator const Util::IHeap&() const { return cp; }

	operator LP::Solver&() { return lp;}
	operator CP::Store&() { return cp;}

	bool valid() {	return cp.valid() and lp.valid();	}


//protected:

	CP::Store&		cp;
	LP::Solver& 	lp;
};


template<class T>
bool Store::post(const T& r)
{
	return LP::PostValActor<Store,T>()(*this,r);
}

} // MIP

} // Casper

#endif /* CASPER_MIP_STORE_H_ */
