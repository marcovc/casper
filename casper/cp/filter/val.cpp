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

#include <casper/cp/filter/val.h>
#include <casper/cp/traits.h>
#include <casper/cp/int/fd.h>
#include <casper/kernel/obj/expr.h>

namespace Casper {
namespace CP {
PostValFilter postValFilter;

bool PostValFilter::operator()(Store& s,const bool& b) const
{	return b;	}

bool PostValFilter::operator()(Store& s,const Var<bool>& v) const
{	return s.post(
		new (s) BndFilterView<Rel2<Equal,Var<bool>,bool> >(s,rel<Equal>(v,true)));	}

bool PostValFilter::operator()(Store& s,const Casper::Expr<bool>& v) const
{	return v.postValFilter(s);	}

} // CP
} // Casper

