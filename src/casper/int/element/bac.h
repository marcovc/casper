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

#ifndef _H_CASPER_INT_ELEMENT_BND
#define _H_CASPER_INT_ELEMENT_BND

#include <casper/int/element/common.h>

#include <casper/kernel/view/arrayview.h>
#include <casper/kernel/filter/common.h>

namespace casper {

/**
 *  Filter that enforces bounds consistency
 *  on the element constraint.
 *
 *  \ingroup IntFilters
 */
template<class Eval,class ArrayView,class IdxView,class ResView>
struct BndFilterView3<Element,Seq<Eval>,ArrayView,Int,IdxView,Eval,ResView> :
	IFilter
{
    BndFilterView3( ICPSolver&,const ArrayView&,const IdxView&,const ResView&);

 	Bool	execute();
	Bool	entailed() const {	return false;	}	// FIXME
	Void 	attach(INotifiable* pParent);
	Void 	detach(INotifiable* pParent);
	Bool 	notify();
	Cost 	cost() const {	return linearLo; }
	Filter  operator!() { return Bnd(array.getView()[idx.getView()]!=res.getView()); }
	// tmp
	INotifiable* getFirst() { return pParent; }

  private:
  	//typedef typename BndArrayView<Int,ArrayView>::Elem ArrayElem;
	template<class ArrayElem>
	void 	detach(ArrayElem v);

	BndArrayView<Eval,ArrayView>	array;
	BndView<Int,IdxView>			idx;
    BndView<Eval,ResView>			res;
    Reversible<Bool>				on;
    INotifiable*					pParent;
};

template<class Eval,class ArrayView,class IdxView,class ResView>
BndFilterView3<Element,Seq<Eval>,ArrayView,Int,IdxView,Eval,ResView>::
	BndFilterView3( ICPSolver& solver, const ArrayView& array, const IdxView& idx,
				const ResView& res) :
    		IFilter(solver),array(solver,array),idx(solver,idx),
    		res(solver,res),on(solver,false)
{
#ifdef CASPER_ELEMENT_IDX_1
	this->idx.updateMin(1);
	this->idx.updateMax(this->array.size());
#else
	this->idx.updateMin(0);
	this->idx.updateMax(this->array.size()-1);
#endif
}

template<class ArrayView,class IdxView,class ResView>
Filter elementBnd(const ArrayView& array, const IdxView& idx,
				  const ResView& res)
{
	typedef typename EvalTraits<ResView>::Eval Eval;
	typedef BndFilterView3<Element,Seq<Eval>,ArrayView,Int,IdxView,Eval,ResView> T;
	ICPSolver& s(getCPSolver(array));
	return new (s.heap()) T(s,array,idx,res);
}

template<class Eval,class ArrayView,class IdxView,class ResView>
Void
BndFilterView3<Element,Seq<Eval>,ArrayView,Int,IdxView,Eval,ResView>::
attach(INotifiable* pParent)
{
	if (idx.min() == idx.max())
		return;	// equality will be posted on first execute
	this->pParent = pParent;
	for (UInt i = 0; i < array.size(); i++)
		array[i].attach(this);
	idx.attach(this);
	res.attach(this);
	on = true;
}

template<class Eval,class ArrayView,class IdxView,class ResView>
Void
BndFilterView3<Element,Seq<Eval>,ArrayView,Int,IdxView,Eval,ResView>::
detach(INotifiable*)
{
	on = false;
/*	for (UInt i = 0; i < array.size(); i++)
		array[i].domain().detachOnBounds(this);
	idx.domain().detachOnBounds(this);
	res.detach(this);*/
}


template<class Eval,class ArrayView,class IdxView,class ResView>
template<class ArrayElem>
Void
BndFilterView3<Element,Seq<Eval>,ArrayView,Int,IdxView,Eval,ResView>::
detach(ArrayElem v)
{
	//v.domain().detachOnBounds(this);
}

template<class Eval,class ArrayView,class IdxView,class ResView>
Bool BndFilterView3<Element,Seq<Eval>,ArrayView,Int,IdxView,Eval,ResView>::
notify()
{
	if (on)
		return pParent->notify();
	else
		return true;
}


template<class Eval,class ArrayView,class IdxView,class ResView>
Bool
BndFilterView3<Element,Seq<Eval>,ArrayView,Int,IdxView,Eval,ResView>::execute()
{
#ifdef CASPER_ELEMENT_IDX_1
	Int ivMin = idx.min()-1;
	Int ivMax = idx.max()-1;
#else
	Int ivMin = idx.min();
	Int ivMax = idx.max();
#endif

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
		detach(array[ivMin++]);
	while (ivMax >= ivMin &&
		   (array[ivMax].max() < res.min() ||
			array[ivMax].min() > res.max()))
		detach(array[ivMax--]);

	// if no idx-domain pairs are possible then fail
	if (ivMin > ivMax)
		return false;

#ifdef CASPER_ELEMENT_IDX_1
	// if all possible domains have invalid idx then fail
	if (!idx.updateMin(ivMin+1) || !idx.updateMax(ivMax+1))
		return false;
#else
	// if all possible domains have invalid idx then fail
	if (!idx.updateMin(ivMin) || !idx.updateMax(ivMax))
		return false;
#endif

	// if there is only one possible domain then detach and post equality
	if (ivMin == ivMax)	// post equality
	{
		detach(pParent);
		return this->solver().post(array[ivMin].getView() == res.getView());
  	}

	// update res bounds from possible domains
	Eval min = array[ivMin].min();
	Eval max = array[ivMin].max();
	while (++ivMin <= ivMax)
	{
		min = std::min(array[ivMin].min(),min);
		max = std::max(array[ivMin].max(),max);
	}
	return res.updateMax(max) && res.updateMin(min);
}

// element as expression:


// optimization: when posting v[idx]==eval no auxilliary variable is needed
template<class Eval,class V1,class V2,class V3>
struct BndFilterFact2<Equal,Eval,Rel2<Element,V1,V2>,Eval,V3>
{
    static Filter create( ICPSolver& s,const Rel2<Element,V1,V2>& p1,const V3& p2)
    {
    	return BndFilterFact3<Element,Seq<Eval>,V1,Int,V2,Eval,V3>::
    				create(s,p1.p1,p1.p2,p2);
    }

};

// optimization: when posting eval==v[idx] no auxilliary variable is needed
template<class Eval,class V1,class V2,class V3>
struct BndFilterFact2<Equal,Eval,V1,Eval,Rel2<Element,V2,V3> >
{
    static Filter create( ICPSolver& s,const V1& p1, const Rel2<Element,V2,V3>& p2)
    {
    	return BndFilterFact3<Element,Seq<Eval>,V2,Int,V3,Eval,V1>::
    				create(s,p2.p1,p2.p2,p1);
    }
};

// optimization: when posting v1[idx1]==v2[idx2] only one
// auxilliary variable is needed
template<class Eval,class V1,class V2,class V3,class V4>
struct BndFilterFact2<Equal,Eval,Rel2<Element,V1,V2>,Eval,Rel2<Element,V3,V4> >
{
    static Filter create( ICPSolver& s,const Rel2<Element,V1,V2>& p1,
    						 const Rel2<Element,V3,V4>& p2)
    {
    	return BndFilterFact3<Element,Seq<Eval>,V1,Int,V2,Eval,Rel2<Element,V3,V4> >::
    				create(s,p1.p1,p1.p2,p2);
    }
};

#if 0
// element as expression: introduces one auxilliary variable
template<class View1,class View2,class Eval>
struct BndViewRel2<Element,View1,View2,Eval> :
	BndView<Eval,DomVar<Eval> >
{
	typedef BndView<Eval,DomVar<Eval> >	Super;
	using Super::attach;
	using Super::detach;

	BndViewRel2(ICPSolver& solver,const View1& p1, const View2& p2) :
		Super(solver,
			DomVar<Eval>(solver,
				detail::findMin(BndArrayView<Eval,View1>(solver,p1)),
				detail::findMax(BndArrayView<Eval,View1>(solver,p1)))),
		f(elementBnd(p1,p2,Super::v))	{}

	void attach()
	{
	 	Super::attach();
		this->solver().post(f);
	}

	protected:
	Filter			f;
};
#endif

/*
// element as expression: same as above for the case View1 is a DomVarArray.
// tries to save the auxilliary variable creation when index is instantiated
// at attach, which is always the case when using a MutVar as index.
template<class View1,class View2,class Eval>
struct BndViewRel2<Element,DomVarArray<Eval,1,View1>,View2,Eval> :
	BndView<Eval,DomVar<Eval> >
{
	typedef BndView<Eval,DomVar<Eval> >	Super;
	using Super::attach;
	using Super::detach;

	BndViewRel2(ICPSolver& solver,const DomVarArray<Eval,1,View1>& p1,
							  const View2& p2) :
		Super(solver,
			DomVar<Eval>(solver,
				detail::findMin(BndArrayView<Eval,DomVarArray<Eval,1,View1> >(solver,p1)),
				detail::findMax(BndArrayView<Eval,DomVarArray<Eval,1,View1> >(solver,p1)))),
		array(p1),
		index(solver,p2)
		{}

	void attach()
	{
		if (index.min()==index.max())
			Super::v = array[index.min()];
		else
			this->solver().post(elementBnd(array,index,Super::v));
		Super::attach();
	}

	protected:
	typedef typename EvalTraits<View2>::Eval	Eval2;

	DomVarArray<Eval,1,View1>	array;
	BndView<Eval2,View2>		index;
};

// element as expression: same as above for the case View1 is a DomVarArray<2>.
// tries to save the auxilliary variable creation when index is instantiated
// at attach, which is always the case when using a MutVar as index.
template<class Idx1View,class Idx2View,class Dom,class Eval>
struct BndViewRel2<Element,Rel2<Element,DomVarArray<Eval,2,Dom>,Idx1View>,Idx2View,Eval> :
	BndView<Eval,DomVar<Eval> >
{
	typedef BndView<Eval,DomVar<Eval> >	Super;
	using Super::attach;
	using Super::detach;
	typedef Rel2<Element,DomVarArray<Eval,2,Dom>,Idx1View>	Rel;

	BndViewRel2(ICPSolver& solver,const Rel& p1,
							  const Idx2View& p2) :
		Super(solver,
			DomVar<Eval>(solver,
				detail::findMin(BndArrayView<Eval,DomVarArray<Eval,2,Dom> >(solver,p1.p1)),
				detail::findMax(BndArrayView<Eval,DomVarArray<Eval,2,Dom> >(solver,p1.p1)))),
		array(p1.p1),
		index1(solver,p1.p2),
		index2(solver,p2)
		{}

	void attach()
	{
		if (index1.min()==index1.max())
			if (index2.min()==index2.max())
				Super::v = array[index1.min()][index2.min()];
			else
				this->solver().post(elementBnd(array[index1.min()],index2,Super::v));
		else
			this->solver().post(elementBnd(array,index1*array.size(1)+index2,Super::v));
		Super::attach();
	}

	protected:
	//typedef typename EvalTraits<View2>::Eval	Eval2;

	DomVarArray<Eval,2,Dom>		array;
	BndView<Int,Idx1View>		index1;
	BndView<Int,Idx2View>		index2;
};
*/

};

#endif /*_H_CASPER_INT_ELEMENT_Bnd*/
