 /*************************************************************************\
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

#ifndef _H_CASPER_INT_GOAL
#define _H_CASPER_INT_GOAL

#include <casper/cp/goal/labeling.h>

namespace Casper {
namespace CP {

namespace Detail {

#define LABEL_OPTIM

#ifdef LABEL_OPTIM
template<class Dom,class Eval>
struct PostVarEqualCons : IGoal
{
	PostVarEqualCons(Store& s, Dom& d, const Eval& c) : s(s),d(d),c(c) {}
	Goal execute()
	{	return Goal((d = c) and s.valid());	}
	Store& s;
	Dom& d;
	Eval c;
};

template<class Dom,class Eval>
Goal postVarEqualCons(Store& s, Dom& d, const Eval& c)
{	return new (s) PostVarEqualCons<Dom,Eval>(s,d,c);	}

template<class Dom,class Eval>
struct PostVarGreaterCons : IGoal
{
	PostVarGreaterCons(Store& s, Dom& d, const Eval& c) : s(s),d(d),c(c) {}
	Goal execute()
	{	return Goal(d.updateMin(Util::succ(c)) and s.valid());	}
	Store& s;
	Dom& d;
	Eval c;
};

template<class Dom,class Eval>
Goal postVarGreaterCons(Store& s, Dom& d, const Eval& c)
{	return new (s) PostVarGreaterCons<Dom,Eval>(s,d,c);	}
#endif


template<class Eval,class View>
struct SelectValsMin<Seq<Eval>,View> : IValSelector
{
//	typedef typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<View>::Type>::Type Eval;
	SelectValsMin(Store& store,const View& vars) : store(store),doms(store,vars) {}

	Goal select(uint idx)
	{
		if (doms[idx]->ground())
			return succeed();
		return Goal(store,
				post(store,rel<Equal>(doms[idx].getObj(),doms[idx]->min())) or
				(post(store,rel<Greater>(doms[idx].getObj(),doms[idx]->min())) and callValSelector(store,this,idx))
				);
	}
	Store&	store;
	DomArrayView<Eval,View> doms;
};

template<class Eval,class View>
struct SelectValsMax<Seq<Eval>,View> : IValSelector
{
//	typedef typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<View>::Type>::Type Eval;
	SelectValsMax(Store& store,const View& vars) : store(store),doms(store,vars) {}

	Goal select(uint idx)
	{
		if (doms[idx]->ground())
			return succeed();
		return Goal(store,
				post(store,rel<Equal>(doms[idx].getObj(),doms[idx]->max())) or
				(post(store,rel<Less>(doms[idx].getObj(),doms[idx]->max())) and
						callValSelector(store,this,idx))
				);
	}
	Store&	store;
	DomArrayView<Eval,View> doms;
};

template<class Eval,class View>
struct SelectValsRand<Seq<Eval>,View> : IValSelector
{
	//typedef typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<View>::Type>::Type Eval;
	typedef typename CP::Traits::GetDom<typename Casper::Traits::GetTermElem<View>::Type>::Type D;
	SelectValsRand(Store& store,const View& vars) :
		store(store),doms(store,vars) {}
	Goal select(uint idx)
	{
		if (doms[idx]->ground())
			return succeed();
		uint s = doms[idx]->size();
		uint r = static_cast<uint>(rand()/(double)RAND_MAX * s);
		typename D::Iterator it = doms[idx]->begin();
		while (r-- > 0)
			++it;
		return Goal(store,
				post(store,rel<Equal>(doms[idx].getObj(),*it)) or
				(post(store,rel<Distinct>(doms[idx].getObj(),*it)) and callValSelector(store,this,idx))
				);
	}
	Store&	store;
	DomArrayView<Eval,View> doms;
};

template<class Eval,class View>
struct SelectValMin<Seq<Eval>,View> : IValSelector
{
	//typedef typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<View>::Type>::Type Eval;
	SelectValMin(Store& store,const View& vars) : store(store),doms(store,vars) {}

	Goal select(uint idx)
	{
		if (doms[idx]->ground())
			return succeed();

#ifdef LABEL_OPTIM
		return Goal(store,
				postVarEqualCons(store,*doms[idx],doms[idx]->min()) or
				postVarGreaterCons(store,*doms[idx],doms[idx]->min())
			);
#else
		return Goal(store,
				post(store,rel<Equal>(doms[idx].getObj(),doms[idx]->min())) or
				post(store,rel<Greater>(doms[idx].getObj(),doms[idx]->min()))
			);
#endif
	}
	Store& store;
	DomArrayView<Eval,View> doms;
};

template<class Eval,class View>
struct SelectValMax<Seq<Eval>,View> : IValSelector
{
	//typedef typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<View>::Type>::Type Eval;
	SelectValMax(Store& store,const View& vars) : store(store),doms(store,vars) {}

	Goal select(uint idx)
	{
		if (doms[idx]->ground())
			return succeed();
		return Goal(store,
				post(store,rel<Equal>(doms[idx].getObj(),doms[idx]->max())) or
				post(store,rel<Less>(doms[idx].getObj(),doms[idx]->max()))
			);
	}
	Store& store;
	DomArrayView<Eval,View> doms;
};

}


} // CP
} // Casper

#endif /*_H_CASPER_INT_GOAL*/
