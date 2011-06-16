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
 

#ifndef CASPER_MIP_VAR_H_
#define CASPER_MIP_VAR_H_

#include <casper/cp/var.h>
#include <casper/cp/rangedom.h>
#include <casper/lp/var.h>

namespace Casper {
namespace MIP {

struct Store;

template<class T>
struct Var : CP::Var<T>
{
	typedef T	Eval;

	struct CP2LPDemon : INotifiable
	{
		CP2LPDemon(const Var& v) : v(v) {}
		bool notify()
		{
			LP::Solver& lp = v.store;
			return lp.post(v.lpvar >= v.domain().min()) and
				   lp.post(v.lpvar <= v.domain().max());
		}
		Var v;
	};

	Var(MIP::Store& s) : CP::Var<T>(s),store(s),lpvar(s)
	{
		this->domain().attachOnBounds(new (store) CP2LPDemon(*this));
	}
	MIP::Store& store;
	LP::Var	lpvar;
};

} // MIP

namespace Traits {

template<class T>
struct GetEval<MIP::Var<T> >
{	typedef T	Type;	};

} // Traits

} // Casper

template<class T>
std::ostream& operator<<(std::ostream& os, const Casper::MIP::Var<T>& v)
{
	os << static_cast<const Casper::CP::Var<T>&>(v) << "(" << v.lpvar << ")";
	return os;
}

#endif /* CASPER_MIP_VAR_H_ */


#ifndef CASPER_NO_OPERATORS
#ifndef CASPER_MIP_OP_VAR_H
#define CASPER_MIP_OP_VAR_H
#include <casper/mip/op_var.h>
#endif
#endif

