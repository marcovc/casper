/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2010-2010 - Marco Correia <marco.v.correia@gmail.com>           *
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
 
#ifndef CASPER_KERNEL_PITERATION_H_
#define CASPER_KERNEL_PITERATION_H_

#include <casper/util/iteration.h>
#include <casper/util/container/stdpair.h>
#include <casper/kernel/par/par.h>

namespace Casper {
namespace Detail {

template<class VarT,class SetT,class CondT>
struct PIteration;

template<class T,class SetT,class CondT>
struct PIteration<Par<T>,SetT,CondT>
{
	PIteration(const Par<T>& v, const SetT& s,const CondT& c) :
		v(v),it(s),cond(v.getState(),c)
	{
		sync();
		if (!cond.value())
			iterate();
	}
	bool	valid()	{	return it.valid() and cond.value();	}
	void	iterate()
	{
		do
		{
			it.iterate();
			sync();
		} while (it.valid() and !cond.value());
	}
	void sync()
	{
		if (it.valid())
			v = it.value();
	}

	Par<T> v;
	Util::IterationView<SetT>	it;
	ParView<bool,CondT>	cond;
};

template<class T,class SetT,class CondT>
struct PIteration<Util::StdPair<Par<T>,Par<T> >,SetT,CondT>
{
	PIteration(const Util::StdPair<Par<T>,Par<T> >& p, const SetT& s,const CondT& c) :
		v1(p.first),v2(p.second),it1(s),s(s),it2(new Util::IterationView<SetT>(s)),
		cond(p.first.getState(),c)
	{
		sync();
		if (!cond.value())
			iterate();
	}
	bool	valid()
	{
		return 	it1.valid() and it2->valid() and cond.value();
	}
	void	iterate()
	{
		do
		{
			it2->iterate();
			sync();
			if (!it2->valid())
			{
				it1.iterate();
				delete it2;
				it2 = new Util::IterationView<SetT>(s);
				v2 = it2->value();
				sync();
			}
		} while (it1.valid() and !cond.value());
	}
	void sync()
	{
		if (it1.valid())
			v1 = it1.value();
		if (it2->valid())
			v2 = it2->value();
	}

	Par<T> v1;
	Par<T> v2;
	Util::IterationView<SetT>	it1;
	SetT					s;
	Util::IterationView<SetT>*	it2;
	ParView<bool,CondT>	cond;
};

}
}

#endif /* CASPER_KERNEL_PITERATION_H_ */
