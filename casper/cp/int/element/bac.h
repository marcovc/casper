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

#ifndef _H_CASPER_CP_INT_ELEMENT_BND
#define _H_CASPER_CP_INT_ELEMENT_BND

#include <casper/cp/int/element/common.h>

#include <casper/cp/view/array.h>
#include <casper/cp/filter/common.h>

//#ifdef CASPER_PRECOMPILED
//#include <casper/cp/int/spexpr/explicit_postbnd.h>
//#endif

namespace Casper {
namespace CP {


/**
 *  Enforces the subscript constraint (element).
 *
 *  \ingroup IntFilters
 */
template<class Eval,class ArrayView,class IdxView,class ResView>
struct BndFilterView3<ElementEqual,Seq<Eval>,ArrayView,int,IdxView,Eval,ResView> :
	IFilter
{
    BndFilterView3(Store&,const ArrayView&,const IdxView&,const ResView&);

 	bool	execute();

	void 	attach(INotifiable* pParent);
	void 	detach();
	bool 	notify();
	Cost 	cost() const {	return linearLo; }
//	Filter  operator!() { return Bnd(array.getObj()[idx.getObj()]!=res.getObj()); }

  private:

	Store&							store;
	BndArrayView<Eval,ArrayView>	array;
	BndView<int,IdxView>			idx;
    BndView<Eval,ResView>			res;
    Reversible<bool>				on;
    INotifiable*					pParent;
};

template<class Eval,class ArrayView,class IdxView,class ResView>
BndFilterView3<ElementEqual,Seq<Eval>,ArrayView,int,IdxView,Eval,ResView>::
	BndFilterView3(Store& store, const ArrayView& array, const IdxView& idx,
				const ResView& res) :
    		IFilter(store),store(store),array(store,array),idx(store,idx),
    		res(store,res),on(store,false)
{
}

template<class Eval,class ArrayView,class IdxView,class ResView>
void
BndFilterView3<ElementEqual,Seq<Eval>,ArrayView,int,IdxView,Eval,ResView>::
attach(INotifiable* pParent)
{
	if (idx.min() == idx.max())
		return;	// equality will be posted on first execute
	this->pParent = pParent;
	for (uint i = 0; i < array.size(); i++)
		array[i].attach(this);
	idx.attach(this);
	res.attach(this);
	on = true;
}

template<class Eval,class ArrayView,class IdxView,class ResView>
void
BndFilterView3<ElementEqual,Seq<Eval>,ArrayView,int,IdxView,Eval,ResView>::
detach()
{
	on = false;
/*	for (uint i = 0; i < array.size(); i++)
		array[i].domain().detachOnBounds(this);
	idx.domain().detachOnBounds(this);
	res.detach(this);*/
}

template<class Eval,class ArrayView,class IdxView,class ResView>
bool BndFilterView3<ElementEqual,Seq<Eval>,ArrayView,int,IdxView,Eval,ResView>::
notify()
{
	if (on)
		return pParent->notify();
	else
		return true;
}


template<class Eval,class ArrayView,class IdxView,class ResView>
bool
BndFilterView3<ElementEqual,Seq<Eval>,ArrayView,int,IdxView,Eval,ResView>::execute()
{
	#ifdef CASPER_LOG
	store.getEnv().log(this, "BndFilterView3<ElementEqual,Seq<Eval>,ArrayView,int,IdxView,Eval,ResView>", Util::Logger::filterExecuteBegin);
	#endif

	if (!idx.updateRange(CASPER_CP_ELEMENT_ARRAY_BASE,
						 array.size()+CASPER_CP_ELEMENT_ARRAY_BASE-1))
		return false;

	int ivMin = idx.min()-CASPER_CP_ELEMENT_ARRAY_BASE;
	int ivMax = idx.max()-CASPER_CP_ELEMENT_ARRAY_BASE;

	// detach from unnecessary vars in array
/*	while (idx.domain().min() > ivMin)
		detach(array[ivMin++]);
	while (idx.domain().max() < ivMax)
		detach(array[ivMax--]);
*/
	// check for incompatible vars in array
	while (ivMin <= ivMax &&
		   (array[ivMin].max() < res.min() ||
			array[ivMin].min() > res.max()))
		array[ivMin++].detach();
	while (ivMax >= ivMin &&
		   (array[ivMax].max() < res.min() ||
			array[ivMax].min() > res.max()))
		array[ivMax--].detach();

	// if no idx-domain pairs are possible then fail
	if (ivMin > ivMax)
		return false;

	// if all possible domains have invalid idx then fail
	if (!idx.updateRange(ivMin+CASPER_CP_ELEMENT_ARRAY_BASE,
						 ivMax+CASPER_CP_ELEMENT_ARRAY_BASE))
		return false;

	// if there is only one possible domain then detach and post equality
	if (ivMin == ivMax)	// post equality
	{
		detach();
		return store.post(rel<Equal>(array[ivMin].getObj(),res.getObj()));
  	}

	// update res bounds from possible domains
	Eval min = array[ivMin].min();
	Eval max = array[ivMin].max();
	while (++ivMin <= ivMax)
	{
		min = std::min(array[ivMin].min(),min);
		max = std::max(array[ivMin].max(),max);
	}
	return res.updateRange(min,max);
}

// element as expression:


// optimization: when posting v[idx]==eval no auxilliary variable is needed
template<class Eval,class V1,class V2,class V3>
struct PostBndFilter2<Equal,Eval,Rel2<Element,V1,V2>,Eval,V3>
{
    static bool post(Store& s,const Rel2<Element,V1,V2>& p1,const V3& p2)
    {
    	return PostBndFilter3<ElementEqual,Seq<Eval>,V1,int,V2,Eval,V3>::
    				post(s,p1.p1,p1.p2,p2);
    }

};

// optimization: when posting eval==v[idx] no auxilliary variable is needed
template<class Eval,class V1,class V2,class V3>
struct PostBndFilter2<Equal,Eval,V1,Eval,Rel2<Element,V2,V3> >
{
    static bool post(Store& s,const V1& p1, const Rel2<Element,V2,V3>& p2)
    {
    	return PostBndFilter3<ElementEqual,Seq<Eval>,V2,int,V3,Eval,V1>::
    				post(s,p2.p1,p2.p2,p1);
    }
};

// optimization: when posting v1[idx1]==v2[idx2] only one
// auxilliary variable is needed
template<class Eval,class V1,class V2,class V3,class V4>
struct PostBndFilter2<Equal,Eval,Rel2<Element,V1,V2>,Eval,Rel2<Element,V3,V4> >
{
    static bool post(Store& s,const Rel2<Element,V1,V2>& p1,
    						  const Rel2<Element,V3,V4>& p2)
    {
    	return PostBndFilter3<ElementEqual,Seq<Eval>,V1,int,V2,Eval,Rel2<Element,V3,V4> >::
    				post(s,p1.p1,p1.p2,p2);
    }
};


} // CP
} // Casper

#endif /*_H_CASPER_CP_INT_ELEMENT_Bnd*/
