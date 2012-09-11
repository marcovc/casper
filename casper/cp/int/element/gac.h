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

#ifndef _H_CASPER_CP_INT_ELEMENT_GAC
#define _H_CASPER_CP_INT_ELEMENT_GAC

#include <casper/cp/int/element/common.h>
#include <casper/util/util.h>

//#ifdef CASPER_PRECOMPILED
//#include <casper/cp/int/spexpr/explicit_postdom.h>
//#endif


//#include <casper/kernel/util.h>	// FIXME
#include <casper/cp/int/element/bac.h>

namespace Casper {
namespace CP {

/**
 *  Enforces the subscript constraint (element).
 *
 *  \ingroup IntFilters
 *  \todo improve: 1. using deltas 2. using supports (GAC6)
 */
template<class ArrayView,class IdxView,class EvalView>
struct DomFilterView3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView> : IFilter
{
	DomFilterView3(Store&, const ArrayView&, const IdxView&, const EvalView&);

 	bool	execute();

	void 	attach(INotifiable* f);
	void 	detach();
	Cost 	cost() const {	return quadraticHi; }
	Filter  operator!() { return Dom(array.getObj()[idx.getObj()]!=eval.getObj()); }

	private:

	DomArrayView<int,ArrayView>		array;
	DomView<int,IdxView>			idx;
    DomView<int,EvalView>			eval;
    INotifiable*	pOwner;
	#ifdef CASPER_LOG
    Store& store;
	#endif
};

/*
template<class ArrayView,class IdxView,class EvalView>
Filter elementDom( const ArrayView& vars, const IdxView& idx,
				   const EvalView& res)
{	return Dom(rel<Element>(vars,idx,res));	}
*/

template<class ArrayView,class IdxView,class EvalView>
DomFilterView3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView>::
DomFilterView3(Store& store, const ArrayView& array,
    			const IdxView& idx,const EvalView& eval) :
    IFilter(store),array(store,array),idx(store,idx),eval(store,eval)
	#ifdef CASPER_LOG
	,store(store)
	#endif
	{}


template<class ArrayView,class IdxView,class EvalView>
void DomFilterView3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView>::
attach(INotifiable* f)
{
	pOwner = f;
	for (uint i = 0; i < array.size(); i++)
		array[i]->attachOnDomain(f);
	idx->attachOnDomain(f);
	eval->attachOnDomain(f);
}

template<class ArrayView,class IdxView,class EvalView>
void DomFilterView3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView>::
detach()
{
	for (uint i = 0; i < array.size(); i++)
		array[i]->detachOnDomain(pOwner);
	idx->detachOnDomain(pOwner);
	eval->detachOnDomain(pOwner);
}


template<class ArrayView,class IdxView,class EvalView>
bool DomFilterView3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView>::
execute()
{
	#ifdef CASPER_LOG_OLD
	store.getEnv().log(this, "DomFilterView3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView>", Util::Logger::filterExecuteBegin);
	#endif

	typedef typename Traits::GetDom<IdxView>::Type	IdxDom;

	if (idx->ground())		// FIXME: should post a gac == and detach
		return eval->intersect(array[idx->value()-CASPER_CP_ELEMENT_ARRAY_BASE]->begin(),
							   array[idx->value()-CASPER_CP_ELEMENT_ARRAY_BASE]->end()) and
			   array[idx->value()-CASPER_CP_ELEMENT_ARRAY_BASE]->intersect(eval->begin(),
			   							   	  	  	  	  	  	  	    eval->end());

	for (typename IdxDom::Iterator idxIt = idx->begin(); idxIt != idx->end(); )
		if (!Util::intersect(array[*idxIt-CASPER_CP_ELEMENT_ARRAY_BASE]->begin(),
							 array[*idxIt-CASPER_CP_ELEMENT_ARRAY_BASE]->end(),
							 eval->begin(),eval->end()))
		{
			if (!idx->erase(idxIt++))
				return false;
		}
		else
			++idxIt;
	return true;
}

template<class ArrayView,class IdxView,class EvalView>
struct PostDomFilter3<ElementEqual,IntSeq,ArrayView,int,IdxView,int,EvalView>
{
	static bool post(Store& s,const ArrayView& a,const IdxView& idx,
							const EvalView& eval)
	{
    	typedef Casper::Detail::DeRefAndSimplify<ArrayView> SArrayView;
    	typedef Casper::Detail::DeRefAndSimplify<IdxView> SIdxView;
    	typedef Casper::Detail::DeRefAndSimplify<EvalView> SEvalView;
    	const SArrayView sa = Casper::Detail::DeRefAndSimplify<ArrayView>()(a);
    	const SIdxView sidx = Casper::Detail::DeRefAndSimplify<IdxView> ()(idx);
    	const SEvalView seval = Casper::Detail::DeRefAndSimplify<EvalView> ()(eval);

		return postBndFilter(s,elementEqual(sa,sidx,seval)) and
			   s.post(new (s) DomFilterView3<ElementEqual,IntSeq,ArrayView,
										int,IdxView,int,EvalView>(s,sa,sidx,seval));
	}
};

} // CP
} // Casper



#endif /*_H_CASPER_INT_ELEMENT_GAC*/
