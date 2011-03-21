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
 

#ifndef CASPER_CP_PAR_PAR_H_
#define CASPER_CP_PAR_PAR_H_

#include <casper/kernel/par/view.h>
#include <casper/cp/view/val.h>

namespace Casper {


/**
 * 	ConstParView over groundness testing.
 * 	\ingroup Views
 **/
template<class View>
struct ConstParView1<Ground,View,bool> : IConstPar<bool>
{
	typedef typename Traits::GetEval<View>::Type	ViewEval;
	ConstParView1(Env& env, const View& p1) :
		IConstPar<bool>(env),p1(env,p1) {}
	bool value() const
	{	return p1.ground();	}
	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f)	{	p1.detach(f); }
	Rel1<Ground,View> getObj()  const { return ground(p1.getObj()); }
	CP::ValView<ViewEval,View>	p1;
};

} // Casper

#endif /* PAR_H_ */
