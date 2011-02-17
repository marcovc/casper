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

#include <casper/kernel/goal/labeling.h>

namespace casper {

/**
 * \defgroup IntGoals Integer search
 * Goals specific to integer domain variables.
 * \ingroup Integer
 * \ingroup Search
 */
/*@{*/

namespace detail {

template<int I,class D>
struct SelectValsMin<DomVarArray<Int,I,D> > : IValSelector
{
	SelectValsMin(DomVarArray<Int,I,D> vars) : vars(vars) {}

	Goal select(UInt idx)
	{
		if (vars(idx).domain().ground())
			return ok();
		return vars(idx)==vars(idx).domain().min() or
		(vars(idx)>vars(idx).domain().min() and callValSelector(vars.solver(),this,idx));
	}
	DomVarArray<Int,I,D> vars;
};

template<int I,class D>
struct SelectValsMax<DomVarArray<Int,I,D> > : IValSelector
{
	SelectValsMax(DomVarArray<Int,I,D> vars) : vars(vars) {}

	Goal select(UInt idx)
	{
		if (vars(idx).domain().ground())
			return ok();
		return vars(idx)==vars(idx).domain().max() or
		(vars(idx)<vars(idx).domain().max() and callValSelector(vars.solver(),this,idx));
	}
	DomVarArray<Int,I,D> vars;
};

template<int I,class D>
struct SelectValsRand<DomVarArray<Int,I,D> > : IValSelector
{
	SelectValsRand(const DomVarArray<Int,I,D>& vars) :
		vars(vars) {}
	Goal select(UInt idx)
	{
		if (vars(idx).domain().ground())
			return ok();
		UInt s = vars(idx).domain().size();
		UInt r = static_cast<UInt>(rand()/(Double)RAND_MAX * s);
		typename D::Iterator it = vars(idx).domain().begin();
		while (r-- > 0)
			++it;
		return vars(idx) == *it or
				(vars(idx) != *it and callValSelector(vars.solver(),this,idx));
	}
	DomVarArray<Int,I,D> vars;
};

template<int I,class D>
struct SelectValMin<DomVarArray<Int,I,D> > : IValSelector
{
	SelectValMin(DomVarArray<Int,I,D> vars) : vars(vars) {}

	Goal select(UInt idx)
	{
		if (vars(idx).domain().ground())
			return ok();
		return vars(idx)==vars(idx).domain().min() or
		       vars(idx)>vars(idx).domain().min();
	}
	DomVarArray<Int,I,D> vars;
};

template<int I,class D>
struct SelectValMax<DomVarArray<Int,I,D> > : IValSelector
{
	SelectValMax(DomVarArray<Int,I,D> vars) : vars(vars) {}

	Goal select(UInt idx)
	{
		if (vars(idx).domain().ground())
			return ok();
		return vars(idx)==vars(idx).domain().min() or
		       vars(idx)<vars(idx).domain().min();
	}
	DomVarArray<Int,I,D> vars;
};

}

/*@}*/

};

#endif /*_H_CASPER_INT_GOAL*/
