/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           	   *
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
 

#ifndef CASPER_KERNEL_GOAL_SELECT_H_
#define CASPER_KERNEL_GOAL_SELECT_H_

#include <casper/kernel/view/iteration.h>
#include <casper/kernel/ref/ref.h>

namespace Casper {

#ifndef SWIG

template<class Set,class Cond,class Expr>
Casper::Rel2<Casper::Assign,Casper::Ref<int>,Casper::Rel4<Casper::ArgMin,Casper::Ref<int>,Set,Cond,Expr> >
selectMin(const Casper::Ref<int>& p, const Set& s, const Cond& c, const Expr& e)
{	return assign(p,argMin(p,s,c,e));	}

template<class Set,class Expr>
Casper::Rel2<Casper::Assign,Casper::Ref<int>,Casper::Rel3<Casper::ArgMin,Casper::Ref<int>,Set,Expr> >
selectMin(const Casper::Ref<int>& p, const Set& s, const Expr& e)
{	return assign(p,argMin(p,s,e));	}

template<class Set,class Cond,class Expr>
Casper::Rel2<Casper::Assign,Casper::Ref<int>,Casper::Rel4<Casper::ArgMax,Casper::Ref<int>,Set,Cond,Expr> >
selectMax(const Casper::Ref<int>& p, const Set& s, const Cond& c, const Expr& e)
{	return assign(p,argMax(p,s,c,e));	}

template<class Set,class Expr>
Casper::Rel2<Casper::Assign,Casper::Ref<int>,Casper::Rel3<Casper::ArgMax,Casper::Ref<int>,Set,Expr> >
selectMax(const Casper::Ref<int>& p, const Set& s, const Expr& e)
{	return assign(p,argMax(p,s,e));	}

template<class Set,class Cond>
Casper::Rel2<Casper::Assign,Casper::Ref<int>,Casper::Rel4<Casper::ArgMin,Casper::Ref<int>,Set,Cond,Casper::Rel2<Casper::RandInRange,double,double> > >
selectRand(const Casper::Ref<int>& p, const Set& s, const Cond& c)
{	return assign(p,argMin(p,s,c,randInRange(0.0,1.0)));	}

template<class Set>
Casper::Rel2<Casper::Assign,Casper::Ref<int>,Casper::Rel3<Casper::ArgMin,Casper::Ref<int>,Set,Casper::Rel2<Casper::RandInRange,double,double> > >
selectRand(const Casper::Ref<int>& p, const Set& s)
{	return assign(p,argMin(p,s,randInRange(0.0,1.0)));	}

#endif

}

#endif /* CASPER_KERNEL_GOAL_SELECT_H_ */
