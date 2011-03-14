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

/**
 *	\file
 * 	Mutable filters.
 */

#ifndef _H_CASPER_KERNEL_FILTER_MUTFILTER
#define _H_CASPER_KERNEL_FILTER_MUTFILTER

#include <casper/kernel/filter/common.h>
#include <casper/kernel/filter/bacfilter.h>
#include <casper/kernel/relation.h>

namespace Casper {

struct IMutFilter
{
	virtual Filter instantiate(CPSolver&) const = 0;
	virtual ~IMutFilter() {}
};

template<class View>
struct MutFilterView : IMutFilter
{
	MutFilterView(const View& v) : pv(v) {}
	Filter instantiate(CPSolver& s) const
	{	return Bnd(pv); /*new (s.getHeap()) BndFilterView<View>(s,pv.getCRef())*/	}
	View	pv;
};

/**
 *	The handler class for MutFilters. See IMutFilter for a complete description
 *  of all members.
 *  \sa IMutFilter
 */
struct MutFilter : Util::PImplIdiom<IMutFilter>
{
	typedef MutFilter						Self;
	typedef Util::PImplIdiom<IMutFilter> 	Super;

	/// Creates a new MutFilter pointing to an existing MutFilter \p f.
	MutFilter(IMutFilter*	f) : Super(f) {}

	/// Creates a new MutFilter enforcing an unary relation \p r.
	template<class View>
	MutFilter(const View& v) :
		Super(new (getState(v).getHeap())
			MutFilterView<View>(v)) {}

	Filter instantiate(CPSolver& solver) const
	{	return Super::pImpl->instantiate(solver); }
};

};

#endif /*_H_CASPER_KERNEL_FILTER_BASIC */
