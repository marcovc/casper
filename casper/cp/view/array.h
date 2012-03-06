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

#ifndef _H_CASPER_KERNEL_VIEW_ARRAYVIEW
#define _H_CASPER_KERNEL_VIEW_ARRAYVIEW

#include <casper/cp/expr.h>
#include <casper/util/container/stdarray.h>
#include <casper/util/container/stdrange.h>
#include <casper/cp/vararray.h>
#include <casper/kernel/view/iteration.h>
#include <casper/cp/view/dom.h>
#include <casper/kernel/ref/piteration.h>
#include <casper/kernel/reversible/array.h> // tmp

namespace Casper {
namespace CP {

#if 0
struct IElemNotifier : INotifier
{
	INotifier& rElem;
	const uint idx;
	IElemNotifier(INotifier& rElem,uint idx) : rElem(rElem),idx(idx) {}
	void attach(INotifiable* pOwner)
	{	rElem.attach(pOwner); }
	void detach(INotifiable* pOwner) { 	rElem.detach(pOwner);	}
	virtual ~IElemNotifier() {}
};

template<class Obj,class Meth>
struct MethodNotifier : INotifiable
{
	Obj*	o;
	Meth 	m;
	MethodNotifier(Obj* o,Meth m) : o(o),m(m) {}
	bool notify() { return ((o)->*(m))(); }
};

template<class Obj,class Meth>
MethodNotifier<Obj,Meth>* methodNotifier(Obj* o,Meth m)
{	return new (o->solver().getHeap()) MethodNotifier<Obj,Meth>(o,m);  }
#endif

namespace Detail {

template<class>
struct ArrayViewTraits;

template<class Eval1,int dims,class Dom>
struct ArrayViewTraits<VarArray<Eval1,dims,Dom> >
{
	typedef Eval1				Eval;
	typedef Var<Eval,Dom> 	Elem;
	static uint size(const VarArray<Eval,dims,Dom>& v) { return v.size();}
};

template<class Elem>
struct ViewArray : Util::StdVector<Elem>
{
	protected:
	typedef Util::StdVector<Elem> Super;
	public:
	ViewArray(Store& store, uint size) :
		Super(store,size)
		{}

	void attach(INotifiable* f)
	{
		for (uint i = 0; i < this->size(); i++)
			Super::operator[](i).attach(f);
	}

	void detach()
	{
		for (uint i = 0; i < this->size(); i++)
			Super::operator[](i).detach();
	}

//	Store& getStore() const {	return store; }

	bool ground() const
	{
		for (uint i = 0; i < this->size(); i++)
			if (!Super::operator[](i)->ground())
				return false;
		return true;
	}
	
//	Store&					store;
};

};

/**
 *	A BndArrayView over a set of generic objects.
 *  \pre IterationView over View must be defined
 *  \pre BndView over the element type must be defined.
 *  \ingroup Views
 */
template<class Eval,class View>
struct BndArrayView :
	Detail::ViewArray<BndView<Eval,typename Casper::Traits::GetTermElem<View>::Type> >
{
	typedef typename Casper::Traits::GetTermElem<View>::Type	SElem;
	typedef BndView<Eval,SElem>	Elem;
	typedef Detail::ViewArray<Elem> Super;

	static uint getSize(const View& p1)
	{
		uint r = 0;
		for (IterationView<View>	it(p1); it.valid(); it.iterate())
			r++;
		return r;
	}
	BndArrayView(Store& store, const View& p1) :
		Super(store,getSize(p1)),store(store)
	{
		uint i = 0;
		for (IterationView<View>	it(p1); it.valid(); it.iterate())
			::new(&Super::operator[](i++)) Elem(store,it.value());
	}
	View getObj() const
	{
		View ret(store,this->size());
		for (uint i = 0; i < this->size(); ++i)
			ret[i] = Super::operator[](i).getObj();
		return ret;
	}
	Store& store;
};

/**
 *	BndArrayView over a VarArray.
 * 	\ingroup Views
 */
template<class Eval,int dims,class Dom>
struct BndArrayView<Eval,VarArray<Eval,dims,Dom> > :
	Detail::ViewArray<BndView<Eval,Var<Eval,Dom> > >
{
	typedef BndView<Eval,Var<Eval,Dom> > Elem;
	typedef Detail::ViewArray<Elem>	Super;
	typedef BndArrayView<Eval,VarArray<Eval,dims,Dom> >	Self;

	BndArrayView(Store& store, const VarArray<Eval,dims,Dom>& p1) :
		Super(store,p1.count()),b(p1)
	{
		for (uint i = 0; i < this->size(); i++)
			::new(&Super::operator[](i)) Elem(store,p1(i));
	}
	VarArray<Eval,dims,Dom> getObj() const {	return b;	}
	VarArray<Eval,dims,Dom>	b;
};

namespace Detail {

template<class View>
struct ArrayDimsTraits;

template<class T,int dims>
struct ArrayDimsTraits<Util::StdArray<T,dims> >
{
	static uint count(const Util::StdArray<T,dims>& d,uint idx)
	{	return d.count(idx);	}
	static uint nesting(const Util::StdArray<T,dims>& d)
	{	return 0;	}
};

template<class EvalT,int dims,class Dom>
struct ArrayDimsTraits<VarArray<EvalT,dims,Dom> >
{
	static uint count(const VarArray<EvalT,dims,Dom>& d,uint idx)
	{	return d.count(idx);	}
	static uint nesting(const VarArray<EvalT,dims,Dom>& d)
	{	return 0;	}
};

template<class ArrayView,class IdxView>
struct ArrayDimsTraits<Rel2<Element,ArrayView,IdxView> >
{
	static uint count(const Rel2<Element,ArrayView,IdxView>& r,uint idx)
	{	return ArrayDimsTraits<ArrayView>::count(r.p1,idx);	}
	static uint nesting(const Rel2<Element,ArrayView,IdxView>& r)
	{	return ArrayDimsTraits<ArrayView>::nesting(r.p1)+1;	}
};

/*
 *	ArrayView over an Element relation. Utility class to be derived by
 *  BndArrayView, DomArrayView and ValArrayView.
 */
template<class Rel,class IdxView, class ArrayView>
struct ArrayViewElem
{
	typedef typename ArrayView::Elem	Elem;

	ArrayViewElem(Store& store, const Rel& p1) :
		array(store,p1.p1),index(store,p1.p2),pv(p1) {}

	void attach(INotifiable* f)
	{	array[index.value()].attach(f);	}

	void detach()
	{	array[index.value()].detach();	}

	///	Returns the element at position \p i.
	Elem& operator[](uint i)
	{	return array[index.value()*size()+i];	}

	///	Returns the element at position \p i.
	const Elem& operator[](uint i) const
	{	return array[index.value()*size()+i];	}

	uint size() const
	{
		const uint nesting = ArrayDimsTraits<Rel>::nesting(pv);
		return ArrayDimsTraits<Rel>::count(pv,nesting);
	}

	Rel getObj()  const
	{ 	return pv;	}

	ArrayView						array;
	IdxView							index;
	Rel								pv;
};
};


/**
 *	BndArrayView over an Element relation. Index must be ground on attachment and
 *  element access.
 * 	\ingroup Views
 */
template<class Eval,class ArrayView,class IdxView>
struct BndArrayView<Eval,Rel2<Element,ArrayView,IdxView> > :
	Detail::ArrayViewElem<Rel2<Element,ArrayView,IdxView>,
						  ValView<int,IdxView>,
						  BndArrayView<Eval,ArrayView> >
{
	typedef Rel2<Element,ArrayView,IdxView>	Rel;
	typedef Detail::ArrayViewElem<Rel,
	  							  ValView<int,IdxView>,
	  							  BndArrayView<Eval,ArrayView> >	Super;

	typedef typename Super::Elem	Elem;

	BndArrayView(Store& store, const Rel& p1) : Super(store,p1) {}
};

/**
 *	A ValArrayView over a set of generic objects.
 *  \pre IterationView over View must be defined
 *  \pre ValView over the element type must be defined.
 *  \ingroup Views
 */
template<class Eval,class View>
struct ValArrayView :
	Detail::ViewArray<ValView<Eval,typename Casper::Traits::GetTermElem<View>::Type> >
{
	typedef typename Casper::Traits::GetTermElem<View>::Type	SElem;
	typedef ValView<Eval,SElem>	Elem;
	typedef Detail::ViewArray<Elem> Super;

	static uint getSize(const View& p1)
	{
		uint r = 0;
		for (IterationView<View>	it(p1); it.valid(); it.iterate())
			r++;
		return r;
	}
	ValArrayView(Store& store, const View& p1) :
		Super(store,getSize(p1))
	{
		uint i = 0;
		for (IterationView<View>	it(p1); it.valid(); it.iterate())
			::new(&Super::operator[](i++)) Elem(store,it.value());
	}
};

/**
 *	A DomArrayView over a set of generic objects.
 *  \pre IterationView over View must be defined
 *  \pre DomView over the element type must be defined.
 *  \ingroup Views
 */

template<class Eval,class View>
struct DomArrayView :
	Detail::ViewArray<DomView<Eval,typename Casper::Traits::GetTermElem<View>::Type> >
{
	typedef typename Casper::Traits::GetTermElem<View>::Type	SElem;
	typedef DomView<Eval,SElem>	Elem;
	typedef Detail::ViewArray<Elem> Super;

	static uint getSize(const View& p1)
	{
		uint r = 0;
		for (IterationView<View>	it(p1); it.valid(); it.iterate())
			r++;
		return r;
	}
	DomArrayView(Store& store, const View& p1) :
		Super(store,getSize(p1)),v(p1)
	{
		uint i = 0;
		for (IterationView<View>	it(p1); it.valid(); it.iterate())
			::new(&Super::operator[](i++)) Elem(store,it.value());
	}
	View getObj() const
	{
		return v;
	}
	View	v;
};



/**
 *	DomArrayView over an Element relation. Index must be ground on attachment and
 *  element access.
 * 	\ingroup Views
 */
template<class Eval,class ArrayView,class IdxView>
struct DomArrayView<Eval,Rel2<Element,ArrayView,IdxView> > :
	Detail::ArrayViewElem<Rel2<Element,ArrayView,IdxView>,
						  	ValView<int,IdxView>,
						  	DomArrayView<Eval,ArrayView> >
{
	typedef Rel2<Element,ArrayView,IdxView>	Rel;
	typedef Detail::ArrayViewElem<Rel,
	  							  ValView<int,IdxView>,
	  							  DomArrayView<Eval,ArrayView> >	Super;

	typedef typename Super::Elem	Elem;

	DomArrayView(Store& store, const Rel& p1) : Super(store,p1) {}
};

/**
 *	BndArrayView over a DomArrayView.
 * 	\ingroup Views
 */
/*
template<class Eval,class View>
struct BndArrayView<Eval,DomArrayView<Eval,View> > :
	Detail::ViewArray<BndView<Eval,typename DomArrayView<Eval,View>::Elem> >
{
	typedef DomView<Eval,typename DomArrayView<Eval,View>::Elem> DomElem;
	typedef BndView<Eval,DomElem> 	Elem;
	typedef Detail::ViewArray<Elem>	Super;

	BndArrayView(CPSolver& solver, const DomArrayView<Eval,View>& p1) :
		Super(solver,p1.size())
	{
		for (uint i = 0; i < this->size(); i++)
			::new(&Super::operator[](i)) Elem(solver,DomElem(solver,&(*p1[i])));
	}
};*/


/**
 *	Identity: BndArrayView over a BndArrayView
 *  \ingroup Views
 */
template<class Eval,class View>
struct BndArrayView<Eval,BndArrayView<Eval,View> > :
	BndArrayView<Eval,View>
{
	typedef BndArrayView<Eval,View>	Super;
	typedef typename Super::Elem 	Elem;

	BndArrayView(Store& store, const BndArrayView<Eval,View>& p1) :
		Super(p1)
	{}
};


/**
 *	DomArrayView over a VarArray.
 *  \ingroup Views
 */
template<class Eval,int dims,class DomT>
struct DomArrayView<Eval,VarArray<Eval,dims,DomT> > :
	Detail::ViewArray<DomView<Eval,Var<Eval,DomT>,DomT> >
{
	typedef DomT	Dom;
	typedef DomView<Eval,Var<Eval,Dom>,Dom> Elem;
	typedef Detail::ViewArray<Elem>	Super;
	typedef DomArrayView<Eval,VarArray<Eval,dims,Dom> >	Self;
	//typedef typename Dom::Iterator	DomIterator;

	DomArrayView(Store& store, const VarArray<Eval,dims,Dom>& p1) :
		Super(store,p1.count())
	{
		for (uint i = 0; i < this->size(); i++)
			::new(&Super::operator[](i)) Elem(store,p1(i));
	}
};

namespace Detail {

// **** Find min and max of iterable sequences ****

template<class View>
struct FindMin
{
	typedef	typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<View>::Type>::Type	Eval;
	Eval	operator()(Store& s,const View& v)
	{
		BndArrayView<Eval,View>	a(s,v);
		assert(a.size() >= 1);
		Eval ret = a[0].min();
		for (uint i = 1; i < a.size(); i++)
			if (a[i].min() < ret)
				ret = a[i].min();
		return ret;
	}
};

template<class View>
struct FindMax
{
	typedef	typename Casper::Traits::GetEval<typename Casper::Traits::GetTermElem<View>::Type>::Type	Eval;
	Eval	operator()(Store& s,const View& v)
	{
		BndArrayView<Eval,View>	a(s,v);
		assert(a.size() >= 1);
		Eval ret = a[0].max();
		for (uint i = 1; i < a.size(); i++)
			if (a[i].max() > ret)
				ret = a[i].max();
		return ret;
	}
};

template<class View>
typename FindMin<View>::Eval findMin(Store& s,const View& view)
{	return FindMin<View>()(s,view);	}

template<class View>
typename FindMax<View>::Eval findMax(Store& s,const View& view)
{	return FindMax<View>()(s,view);	}

/*
template<class EvalT,class View>
struct FindMin<BndArrayView<EvalT,View> >
{
	typedef	EvalT	Eval;
	Eval	operator()(const BndArrayView<Eval,View>& a)
	{
		assert(a.size() >= 1);
		Eval ret = a[0].min();
		for (uint i = 1; i < a.size(); i++)
			if (a[i].min() < ret)
				ret = a[i].min();
		return ret;
	}
};

template<class EvalT,class View>
struct FindMax<BndArrayView<EvalT,View> >
{
	typedef	EvalT	Eval;
	Eval	operator()(const BndArrayView<Eval,View>& a)
	{
		assert(a.size() >= 1);
		Eval ret = a[0].max();
		for (uint i = 1; i < a.size(); i++)
			if (a[i].max() > ret)
				ret = a[i].max();
		return ret;
	}
};
*/

};

/**
 *	BndView over minimum of an array.
 *	\ingroup Views
 **/
template<class Expr1,class Eval>
struct BndViewRel1<Min,Expr1,Eval>
{
	BndViewRel1(Store& store, const Expr1& p1) :
		a(store,p1) {}
	Eval min() const
	{
		Eval r = a[0].min();
		for (uint i = 1; i < a.size(); i++)
			if (a[i].min() < r)
				r = a[i].min();
		return r;
	}
	Eval max() const
	{
		Eval r = a[0].max();
		for (uint i = 1; i < a.size(); i++)
			if (a[i].max() < r)
				r = a[i].max();
		return r;
	}
	bool updateMin(const Eval& v)
	{
		for (uint i = 0; i < a.size(); i++)
			if (!a[i].updateMin(v))
				return false;
		return true;
	}
	// only does anything if there is only one element with min <= v
	bool updateMax(const Eval& v)
	{
		bool found = false;
		uint mi = a.size();
		for (uint i = 0; i < a.size(); i++)
			if (a[i].min() <= v)
			{
				if (found)
					return true;
				found = true;
				mi = i;
			}
		assert(mi < a.size());
		return a[mi].updateMax(v);
	}
	void attach(INotifiable* f) { 	a.attach(f); }
	void detach() {	a.detach(); }
	Rel1<Min,Expr1>	getObj() const {	return rel<Min>(a.getObj());	}

	BndArrayView<Eval,Expr1>	a;
};

/**
 *	BndView over maximum of an array.
 *	\ingroup Views
 **/
template<class Expr1,class Eval>
struct BndViewRel1<Max,Expr1,Eval>
{
	BndViewRel1(Store& store, const Expr1& p1) :
		a(store,p1) {}
	Eval min() const
	{
		Eval r = a[0].min();
		for (uint i = 1; i < a.size(); i++)
			if (a[i].min() > r)
				r = a[i].min();
		return r;
	}
	Eval max() const
	{
		Eval r = a[0].max();
		for (uint i = 1; i < a.size(); i++)
			if (a[i].max() > r)
				r = a[i].max();
		return r;
	}
	bool updateMax(const Eval& v)
	{
		for (uint i = 0; i < a.size(); i++)
			if (!a[i].updateMax(v))
				return false;
		return true;
	}
	// only does anything if there is only one element with max >= v
	bool updateMin(const Eval& v)
	{
		bool found = false;
		uint mi = a.size();
		for (uint i = 0; i < a.size(); i++)
			if (a[i].max() >= v)
			{
				if (found)
					return true;
				found = true;
				mi = i;
			}
		assert(mi < a.size());
		return a[mi].updateMin(v);
	}
	void attach(INotifiable* f) { 	a.attach(f); }
	void detach() {	a.detach(); }
	Rel1<Max,Expr1> getObj() const {	return rel<Max>(a.getObj()); }

	BndArrayView<Eval,Expr1>	a;
};

template<class Eval,class VarT,class SetT,class CondT,class ExprT>
struct BndArrayView<Eval,Rel4<All,VarT,SetT,CondT,ExprT> > :
	Detail::ViewArray<BndView<Eval,ExprT> >
{
	typedef Rel4<All,VarT,SetT,CondT,ExprT> RelT;

	typedef BndView<Eval,ExprT>	Elem;
	typedef Detail::ViewArray<Elem> Super;

	static uint getSize(const RelT& p)
	{
		uint r = 0;
		for (Casper::Detail::PIteration<VarT,SetT,CondT> it(p.p1,p.p2,p.p3); it.valid(); it.iterate())
			r++;
		return r;
	}

	BndArrayView(Store& store, const RelT& p) :
		Super(store,getSize(p))
	{
		uint i = 0;
		for (Casper::Detail::PIteration<VarT,SetT,CondT> it(p.p1,p.p2,p.p3); it.valid(); it.iterate())
			::new(&Super::operator[](i++)) Elem(store,p.p4);
	}

};

template<class Eval,class VarT,class SetT,class CondT,class ExprT>
struct DomArrayView<Eval,Rel4<All,VarT,SetT,CondT,ExprT> > :
	Detail::ViewArray<DomView<Eval,ExprT> >
{
	typedef Rel4<All,VarT,SetT,CondT,ExprT> RelT;

	typedef DomView<Eval,ExprT>	Elem;
	typedef Detail::ViewArray<Elem> Super;

	static uint getSize(const RelT& p)
	{
		uint r = 0;
		for (Casper::Detail::PIteration<VarT,SetT,CondT> it(p.p1,p.p2,p.p3); it.valid(); it.iterate())
			r++;
		return r;
	}

	DomArrayView(Store& store, const RelT& p) :
		Super(store,getSize(p))
	{
		uint i = 0;
		for (Casper::Detail::PIteration<VarT,SetT,CondT> it(p.p1,p.p2,p.p3); it.valid(); it.iterate())
			::new(&Super::operator[](i++)) Elem(store,p.p4);
	}

};

#define BNDVIEW_SUM_INCREMENTAL
#ifndef BNDVIEW_SUM_INCREMENTAL
/**
 *	BndView over a Sum of values.
 *	\ingroup Views
 **/
template<class Expr1,class Eval>
struct BndViewRel1<Sum,Expr1,Eval>
{
	BndViewRel1(CPSolver& solver, const Expr1& p1) : v(solver,p1) {}
	Eval min() const
	{
		Eval r = 0;
		for (uint i = 0; i < v.size(); ++i)
			r += v[i].min();
		return r;
	}
	Eval max() const
	{
		Eval r = 0;
		for (uint i = 0; i < v.size(); ++i)
			r += v[i].max();
		return r;
	}
	bool updateMin(const Eval& mi)
	{
		Eval m = max();
		for (uint i = 0; i < v.size(); ++i)
			if (!v[i].updateMin(mi-m+v[i].max()))
				return false;
		return true;
	}
	bool updateMax(const Eval& ma)
	{
		Eval m = min();
		for (uint i = 0; i < v.size(); ++i)
			if (!v[i].updateMax(ma-m+v[i].min()))
				return false;
		return true;
	}
	void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	bool updateRange(const Eval& l, const Eval& u)
	{	return updateMin(l) and updateMax(u);	}

	void attach(INotifiable* f) { 	v.attach(f); }
	void detach(INotifiable* f) {	v.detach(f); }

	Rel1<Sum,Expr1> getObj()  const
	{ 	return Rel1<Sum,Expr1>(v.getObj());	}

	BndArrayView<Eval,Expr1>	v;
};
#else
/**
 *	BndView over a Sum of values.
 *	\ingroup Views
 **/
template<class Expr1,class Eval>
struct BndViewRel1<Sum,Expr1,Eval>
{
	BndViewRel1(Store& store, const Expr1& p1) :
		v(store,p1),cachedMin(store),cachedMax(store),
		cachedViewMin(store,v.size(),0),
		cachedViewMax(store,v.size(),0) { init(); }

	void init()
	{
		Eval mi = 0;
		Eval ma = 0;
		for (uint i = 0; i < v.size(); ++i)
		{
			cachedViewMin[i] = v[i].min();
			cachedViewMax[i] = v[i].max();
			mi += cachedViewMin[i];
			ma += cachedViewMax[i];
		}
		cachedMin = mi;
		cachedMax = ma;
	}

	struct TermDemon : INotifiable
	{
		TermDemon(BndViewRel1* pOwner, uint idx) : rOwner(*pOwner),idx(idx) {}
		bool notify()
		{
			bool ch = false;
			Eval mi,ma;
			rOwner.v[idx].range(mi,ma);
			if (mi > rOwner.cachedViewMin[idx])
			{
				rOwner.cachedMin = rOwner.cachedMin - rOwner.cachedViewMin[idx] + mi;
				rOwner.cachedViewMin[idx] = mi;
				ch = true;
			}
			if (ma < rOwner.cachedViewMax[idx])
			{
				rOwner.cachedMax = rOwner.cachedMax - rOwner.cachedViewMax[idx] + ma;
				rOwner.cachedViewMax[idx] = ma;
				ch = true;
			}
			return !ch or rOwner.pOwner->notify();
		}
		BndViewRel1& rOwner;
		const uint idx;
	};
	Eval min() const
	{	return cachedMin;	}
	Eval max() const
	{	return cachedMax;	}
	bool updateMin(const Eval& mi)
	{
		const Eval d = mi-cachedMax;
		for (uint i = 0; i < v.size(); ++i)
			if (!v[i].updateMin(d+cachedViewMax[i]))
				return false;
		return true;
	}
	bool updateMax(const Eval& ma)
	{
		const Eval d = ma-cachedMin;
		for (uint i = 0; i < v.size(); ++i)
			if (!v[i].updateMax(d+cachedViewMin[i]))
				return false;
		return true;
	}
	void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	bool updateRange(const Eval& l, const Eval& u)
	{
		const Eval dl = l-cachedMax;
		const Eval du = u-cachedMin;
		for (uint i = 0; i < v.size(); ++i)
			if (!v[i].updateRange(dl+cachedViewMax[i],du+cachedViewMin[i]))
				return false;
		return true;
	}

	void attach(INotifiable* f)
	{
		pOwner = f;
		for (uint i = 0; i < v.size(); ++i)
			v[i].attach(new TermDemon(this,i));
		init();
	}
	void detach() {	}

	Rel1<Sum,Expr1> getObj()  const
	{ 	return Rel1<Sum,Expr1>(v.getObj());	}


	BndArrayView<Eval,Expr1>	v;
	Reversible<Eval>			cachedMin;
	Reversible<Eval>			cachedMax;
	Vector<Eval>				cachedViewMin;
	Vector<Eval>				cachedViewMax;
	INotifiable*				pOwner;
};
#endif


template<class Expr1,class Expr2,class Eval>
struct BndViewRel2<SumProduct,Expr1,Expr2,Eval> :
	BndViewRel1<Sum,Rel4<All,Ref<int>,Casper::Util::StdRange<int>,bool,
		Rel2<Mul,Rel2<Element,Expr1,Ref<int> >,
				 Rel2<Element,Expr2,Ref<int> > > >,Eval>
{
	typedef Rel4<All,Ref<int>,Casper::Util::StdRange<int>,bool,
				Rel2<Mul,Rel2<Element,Expr1,Ref<int> >,
						 Rel2<Element,Expr2,Ref<int> > > > Agg;
	typedef BndViewRel1<Sum,Agg,Eval> Super;
	Agg agg(Store& store,const Expr1& v1, const Expr2& v2)
	{
		BndArrayView<Eval,Expr1> b1(store,v1);
		Ref<int> i(store);
		return all(i,range(0,b1.size()-1),true,element(v1,i)*element(v2,i));
	}

	BndViewRel2(Store& store,const Expr1& v1, const Expr2& v2) :
		Super(store,agg(store,v1,v2))
		{}

};


// dom view over element expression
template<class Expr1,class Eval>
struct DomView<Eval,Rel2<Element,Expr1,Ref<int> > >
{
	typedef typename Casper::Traits::GetElem<Expr1>::Type	Elem;
	typedef typename Traits::GetDom<Elem>::Type	Dom;
	DomView(Store& store,const Rel2<Element,Expr1,Ref<int> >& p) :
		store(store),v(store,p.p1),i(p.p2.value()) {}
	Dom*	operator->()			{	return &*v[i];	}
	Dom*	operator->() const	{	return const_cast<Dom*>(&*v[i]);	}
	Dom&	operator*()				{	return *v[i];	}
	Dom&	operator*() const	{	return const_cast<Dom&>(*v[i]); }
	Rel2<Element,Expr1,Ref<int> > getObj() const
	{	return Rel2<Element,Expr1,Ref<int> >(v.getObj(),Ref<int>(store,i));}

	Store& store;
	DomArrayView<Eval,Expr1>	v;
	const int	i;
//	Dom&	d;
};

} // CP


namespace Traits {
template<class EvalT,class View>
struct GetEval<CP::ValArrayView<EvalT,View> >
{	typedef Seq<EvalT>	Type;	};

template<class EvalT,class View>
struct GetEval<CP::BndArrayView<EvalT,View> >
{	typedef Seq<EvalT>	Type;	};

template<class EvalT,class View>
struct GetEval<CP::DomArrayView<EvalT,View> >
{	typedef Seq<EvalT>	Type;	};

template<class Eval,class View>
struct GetElem<CP::ValArrayView<Eval,View> >
{	typedef CP::ValView<Eval,typename Casper::Traits::GetElem<View>::Type>	Type;	};

template<class Eval,class View>
struct GetElem<CP::BndArrayView<Eval,View> >
{	typedef CP::BndView<Eval,typename Casper::Traits::GetElem<View>::Type>	Type;	};

template<class Eval,class View>
struct GetElem<CP::DomArrayView<Eval,View> >
{
	typedef typename Casper::Traits::GetElem<View>::Type	Elem;
	typedef typename Casper::Traits::GetElem<View>::Type	A;
	typedef CP::DomView<Eval,A,typename CP::Traits::GetDom<A>::Type>	Type;
};
} // Traits

} // Casper

#endif // _H_CASPER_KERNEL_VIEW_ARRAYVIEW
