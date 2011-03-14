 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_INT_GOAL
#define _H_CASPER_INT_GOAL

#include <casper/cp/goal/labeling.h>

namespace Casper {
namespace CP {


/**
 * \defgroup IntGoals Integer search
 * Goals specific to integer domain variables.
 * \ingroup Integer
 * \ingroup Search
 */
/*@{*/

namespace Detail {

template<int I,class D>
struct SelectValsMin<VarArray<int,I,D> > : IValSelector
{
	SelectValsMin(Store& store,VarArray<int,I,D> vars) : store(store),vars(vars) {}

	Goal select(uint idx)
	{
		if (vars(idx).domain().ground())
			return succeed();
		return Goal(store,
				post(store,vars(idx)==vars(idx).domain().min()) or
				(post(store,vars(idx)>vars(idx).domain().min()) and callValSelector(store,this,idx))
				);
	}
	Store&	store;
	VarArray<int,I,D> vars;
};

template<int I,class D>
struct SelectValsMax<VarArray<int,I,D> > : IValSelector
{
	SelectValsMax(Store& store,VarArray<int,I,D> vars) : store(store),vars(vars) {}

	Goal select(uint idx)
	{
		if (vars(idx).domain().ground())
			return succeed();
		return Goal(store,
				post(store,vars(idx)==vars(idx).domain().max()) or
				(post(store,vars(idx)<vars(idx).domain().max()) and callValSelector(store,this,idx))
				);
	}
	Store&	store;
	VarArray<int,I,D> vars;
};

template<int I,class D>
struct SelectValsRand<VarArray<int,I,D> > : IValSelector
{
	SelectValsRand(Store& store,const VarArray<int,I,D>& vars) :
		store(store),vars(vars) {}
	Goal select(uint idx)
	{
		if (vars(idx).domain().ground())
			return succeed();
		uint s = vars(idx).domain().size();
		uint r = static_cast<uint>(rand()/(double)RAND_MAX * s);
		typename D::Iterator it = vars(idx).domain().begin();
		while (r-- > 0)
			++it;
		return Goal(store,
				post(store,vars(idx) == *it) or
				(post(store,vars(idx) != *it and callValSelector(store,this,idx)))
				);
	}
	Store&	store;
	VarArray<int,I,D> vars;
};

template<int I,class D>
struct SelectValMin<VarArray<int,I,D> > : IValSelector
{
	SelectValMin(Store& store,VarArray<int,I,D> vars) : store(store),vars(vars) {}

	Goal select(uint idx)
	{
		if (vars(idx).domain().ground())
			return succeed();
		return Goal(store,
				post(store,vars(idx)==vars(idx).domain().min()) or
				post(store,vars(idx)>vars(idx).domain().min())
			);
	}
	Store& store;
	VarArray<int,I,D> vars;
};

template<int I,class D>
struct SelectValMax<VarArray<int,I,D> > : IValSelector
{
	SelectValMax(Store& store,VarArray<int,I,D> vars) : store(store),vars(vars) {}

	Goal select(uint idx)
	{
		if (vars(idx).domain().ground())
			return succeed();
		return Goal(store,
				post(store,vars(idx)==vars(idx).domain().min()) or
				post(store,vars(idx)<vars(idx).domain().min())
				);
	}
	Store& store;
	VarArray<int,I,D> vars;
};

}

/*@}*/

} // CP
} // Casper

#endif /*_H_CASPER_INT_GOAL*/
