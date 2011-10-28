/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <casper/cp/filter/bnd.h>
#include <casper/kernel/obj/expr.h>

namespace Casper {
namespace CP {


PostBndFilter postBndFilter;

PostD1BndFilter postD1BndFilter;

bool PostBndFilter::operator()(Store& s,BndExpr<bool> v) const
{
	return new (s) BndFilterView<BndExpr<bool> >(s,v);
}

bool PostBndFilter::operator()(Store& store, const Casper::Expr<bool>& expr) const
{	return expr.postBndFilter(store);	}

/*
Filter::Filter(CPSolver& s,BndExpr<bool> b) :
		Super(BndFilterFact()(s,b))
{}

*/

/*
Filter BndFilterFact::operator()(Filter f) const
{	return f;	}

Filter	BndFilterView<BndExpr<bool> >::operator!()
{	return Filter(solver(),!BndExpr<bool>(&rExpr));	}
*/

} // CP
} // Casper

