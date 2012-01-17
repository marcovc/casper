 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_KERNEL_VIEW_ELEMENT_H_
#define CASPER_KERNEL_VIEW_ELEMENT_H_

#include <casper/kernel/traits.h>
#include <casper/util/container/common.h>
#include <casper/kernel/view/iteration.h>

namespace Casper {

// make sure Obj is not destroyed before calling get()
template<class Obj,class Elem = typename Traits::GetElem<Obj>::Type>
struct ElementView
{
	ElementView(const Obj& v) : v(v) {}
	Elem 	get(uint idx) const 	{	return v[idx];	}
	const Obj& v;
};

// defaults to use IterationView, meaning get() is O(n)
// make sure Obj is not destroyed before calling get()
template<class Obj>
struct ElementView<Obj,typename Traits::GetTermElem<Obj>::Type>
{
	typedef typename Traits::GetTermElem<Obj>::Type Elem;
	ElementView(const Obj& v) : v(v) {}
	Elem 	get(uint idx) const
	{
		IterationView<Obj> it(v);
		for ( ; it.valid() and idx > 0; it.iterate())
			--idx;
		if (!it.valid())
			throw Exception::IndexOutOfBounds();
		return it.value();
	}
	const Obj& v;
};

template<class SeqT,class Index,class Elem>
struct ElementView<Rel2<Element,SeqT,Index>,Elem>
{
	typedef Rel2<Element,SeqT,Index> RelT;
	ElementView(const RelT& v) : v(v.p1),i(getState(v),v.p2) {}
	Elem get(uint idx) const
	{
		ElementView<typename Traits::GetElem<SeqT>::Type> r(v.get(i.value()));
		return r.get(idx);
	}
	ElementView<SeqT> v;
	Ref<int>	i;
};


} // Casper

#endif /*CASPER_KERNEL_VIEW_ELEMENT_H_*/
