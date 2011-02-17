/**************************************************************************\
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

#include <casper/kernel/filter/bacfilter.h>

namespace casper {

BndFilterFact Bnd;

BndD1FilterFact BndD1;

Filter::Filter(BndExpr<Bool> b) :
		Super(BndFilterFact()(b))
{}

Filter BndFilterFact::operator()(BndExpr<Bool> v) const
{
	return new (v.solver().heap()) BndFilterView<BndExpr<Bool> >(v.solver(),v);
}

Filter BndFilterFact::operator()(Filter f) const
{	return f;	}

Filter	BndFilterView<BndExpr<Bool> >::operator!()
{	return !BndExpr<Bool>(&rExpr);	}


};
