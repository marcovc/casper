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

#ifndef _H_CASPER_KERNEL_VIEW_ARRAYVIEW
#define _H_CASPER_KERNEL_VIEW_ARRAYVIEW

#include <casper/kernel/expression.h>
#include <casper/kernel/container/array.h>
#include <casper/kernel/domvararray.h>
#include <casper/kernel/view/iteration.h>
#include <casper/kernel/view/domview.h>
#include <casper/kernel/view/piteration.h>
#include <casper/kernel/container/rarray.h> // tmp

namespace casper {

struct IElemNotifier : INotifier
{
	INotifier& rElem;
	const UInt idx;
	IElemNotifier(INotifier& rElem,UInt idx) : rElem(rElem),idx(idx) {}
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
	Bool notify() { return ((o)->*(m))(); }
};

template<class Obj,class Meth>
MethodNotifier<Obj,Meth>* methodNotifier(Obj* o,Meth m)
{	return new (o->solver().heap()) MethodNotifier<Obj,Meth>(o,m);  }

namespace detail {

template<class>
struct ArrayViewTraits;

template<class Eval1,int dims,class Dom>
struct ArrayViewTraits<DomVarArray<Eval1,dims,Dom> >
{
	typedef Eval1				Eval;
	typedef DomVar<Eval,Dom> 	Elem;
	static UInt size(const DomVarArray<Eval,dims,Dom>& v) { return v.size();}
};

template<class Elem>
struct ViewArray : Vector<Elem>
{
	protected:
	typedef Vector<Elem> Super;
	public:
	ViewArray(ICPSolver& solver, UInt size) :
		Super(solver.heap(),size),mSolver(solver)
		{}

	void attach(INotifiable* f)
	{
		for (UInt i = 0; i < this->size(); i++)
			Super::operator[](i).attach(f);
	}

	void detach(INotifiable* f)
	{
		for (UInt i = 0; i < this->size(); i++)
			Super::operator[](i).detach(f);
	}

	ICPSolver& solver() const {	return mSolver; }

	Bool ground() const
	{
		for (UInt i = 0; i < this->size(); i++)
			if (!Super::operator[](i)->ground())
				return false;
		return true;
	}
	
	ICPSolver&					mSolver;
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
	detail::ViewArray<BndView<Eval,typename ElemTraits<View>::Elem> >
{
	typedef typename ElemTraits<View>::Elem	SElem;
	typedef BndView<Eval,SElem>	Elem;
	typedef detail::ViewArray<Elem> Super;

	static UInt getSize(const View& p1)
	{
		UInt r = 0;
		for (IterationView<View>	it(p1); it.valid(); it.iterate())
			r++;
		return r;
	}
	BndArrayView(ICPSolver& solver, const View& p1) :
		Super(solver,getSize(p1))
	{
		UInt i = 0;
		for (IterationView<View>	it(p1); it.valid(); it.iterate())
			::new(&Super::operator[](i++)) Elem(solver,it.value());
	}
};

/**
 *	BndArrayView over a DomVarArray.
 * 	\ingroup Views
 */
template<class Eval,int dims,class Dom>
struct BndArrayView<Eval,DomVarArray<Eval,dims,Dom> > :
	detail::ViewArray<BndView<Eval,DomVar<Eval,Dom> > >
{
	typedef BndView<Eval,DomVar<Eval,Dom> > Elem;
	typedef detail::ViewArray<Elem>	Super;
	typedef BndArrayView<Eval,DomVarArray<Eval,dims,Dom> >	Self;

	BndArrayView(ICPSolver& solver, const DomVarArray<Eval,dims,Dom>& p1) :
		Super(solver,p1.count()),b(p1)
	{
		for (UInt i = 0; i < this->size(); i++)
			::new(&Super::operator[](i)) Elem(solver,p1(i));
	}
	DomVarArray<Eval,dims,Dom> getView() const {	return b;	}
	DomVarArray<Eval,dims,Dom>	b;
};

namespace detail {

template<class View>
struct ArrayDimsTraits;

template<class T,int dims>
struct ArrayDimsTraits<Array<T,dims> >
{
	static UInt count(const Array<T,dims>& d,UInt idx)
	{	return d.count(idx);	}
	static UInt nesting(const Array<T,dims>& d)
	{	return 0;	}
};

template<class EvalT,int dims,class Dom>
struct ArrayDimsTraits<DomVarArray<EvalT,dims,Dom> >
{
	static UInt count(const DomVarArray<EvalT,dims,Dom>& d,UInt idx)
	{	return d.count(idx);	}
	static UInt nesting(const DomVarArray<EvalT,dims,Dom>& d)
	{	return 0;	}
};

template<class ArrayView,class IdxView>
struct ArrayDimsTraits<Rel2<Element,ArrayView,IdxView> >
{
	static UInt count(const Rel2<Element,ArrayView,IdxView>& r,UInt idx)
	{	return ArrayDimsTraits<ArrayView>::count(r.p1,idx);	}
	static UInt nesting(const Rel2<Element,ArrayView,IdxView>& r)
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

	ArrayViewElem(ICPSolver& solver, const Rel& p1) :
		array(solver,p1.p1),index(solver,p1.p2),pv(p1) {}

	void attach(INotifiable* f)
	{	array[index.value()].attach(f);	}

	void detach(INotifiable* f)
	{	array[index.value()].detach(f);	}

	///	Returns the element at position \p i.
	Elem& operator[](UInt i)
	{	return array[index.value()*size()+i];	}

	///	Returns the element at position \p i.
	const Elem& operator[](UInt i) const
	{	return array[index.value()*size()+i];	}

	ICPSolver& solver() const {	return array.solver(); }

	UInt size() const
	{
		const UInt nesting = ArrayDimsTraits<Rel>::nesting(pv);
		return ArrayDimsTraits<Rel>::count(pv,nesting);
	}

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
	detail::ArrayViewElem<Rel2<Element,ArrayView,IdxView>,
						  ValView<Int,IdxView>,
						  BndArrayView<Eval,ArrayView> >
{
	typedef Rel2<Element,ArrayView,IdxView>	Rel;
	typedef detail::ArrayViewElem<Rel,
	  							  ValView<Int,IdxView>,
	  							  BndArrayView<Eval,ArrayView> >	Super;

	typedef typename Super::Elem	Elem;

	BndArrayView(ICPSolver& solver, const Rel& p1) : Super(solver,p1) {}
};

/**
 *	A ValArrayView over a set of generic objects.
 *  \pre IterationView over View must be defined
 *  \pre ValView over the element type must be defined.
 *  \ingroup Views
 */
template<class Eval,class View>
struct ValArrayView :
	detail::ViewArray<ValView<Eval,typename ElemTraits<View>::Elem> >
{
	typedef typename ElemTraits<View>::Elem	SElem;
	typedef ValView<Eval,SElem>	Elem;
	typedef detail::ViewArray<Elem> Super;

	static UInt getSize(const View& p1)
	{
		UInt r = 0;
		for (IterationView<View>	it(p1); it.valid(); it.iterate())
			r++;
		return r;
	}
	ValArrayView(ICPSolver& solver, const View& p1) :
		Super(solver,getSize(p1))
	{
		UInt i = 0;
		for (IterationView<View>	it(p1); it.valid(); it.iterate())
			::new(&Super::operator[](i++)) Elem(solver,it.value());
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
	detail::ViewArray<DomView<Eval,typename ElemTraits<View>::Elem> >
{
	typedef typename ElemTraits<View>::Elem	SElem;
	typedef DomView<Eval,SElem>	Elem;
	typedef detail::ViewArray<Elem> Super;

	static UInt getSize(const View& p1)
	{
		UInt r = 0;
		for (IterationView<View>	it(p1); it.valid(); it.iterate())
			r++;
		return r;
	}
	DomArrayView(ICPSolver& solver, const View& p1) :
		Super(solver,getSize(p1))
	{
		UInt i = 0;
		for (IterationView<View>	it(p1); it.valid(); it.iterate())
			::new(&Super::operator[](i++)) Elem(solver,it.value());
	}
};



/**
 *	DomArrayView over an Element relation. Index must be ground on attachment and
 *  element access.
 * 	\ingroup Views
 */
template<class Eval,class ArrayView,class IdxView>
struct DomArrayView<Eval,Rel2<Element,ArrayView,IdxView> > :
	detail::ArrayViewElem<Rel2<Element,ArrayView,IdxView>,
						  	ValView<Int,IdxView>,
						  	DomArrayView<Eval,ArrayView> >
{
	typedef Rel2<Element,ArrayView,IdxView>	Rel;
	typedef detail::ArrayViewElem<Rel,
	  							  ValView<Int,IdxView>,
	  							  DomArrayView<Eval,ArrayView> >	Super;

	typedef typename Super::Elem	Elem;

	DomArrayView(ICPSolver& solver, const Rel& p1) : Super(solver,p1) {}
};

/**
 *	BndArrayView over a DomArrayView.
 * 	\ingroup Views
 */
/*
template<class Eval,class View>
struct BndArrayView<Eval,DomArrayView<Eval,View> > :
	detail::ViewArray<BndView<Eval,typename DomArrayView<Eval,View>::Elem> >
{
	typedef DomView<Eval,typename DomArrayView<Eval,View>::Elem> DomElem;
	typedef BndView<Eval,DomElem> 	Elem;
	typedef detail::ViewArray<Elem>	Super;

	BndArrayView(ICPSolver& solver, const DomArrayView<Eval,View>& p1) :
		Super(solver,p1.size())
	{
		for (UInt i = 0; i < this->size(); i++)
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

	BndArrayView(ICPSolver& solver, const BndArrayView<Eval,View>& p1) :
		Super(p1)
	{}
};


/**
 *	DomArrayView over a DomVarArray.
 *  \ingroup Views
 */
template<class Eval,int dims,class DomT>
struct DomArrayView<Eval,DomVarArray<Eval,dims,DomT> > :
	detail::ViewArray<DomView<Eval,DomVar<Eval,DomT>,DomT> >
{
	typedef DomT	Dom;
	typedef DomView<Eval,DomVar<Eval,Dom>,Dom> Elem;
	typedef detail::ViewArray<Elem>	Super;
	typedef DomArrayView<Eval,DomVarArray<Eval,dims,Dom> >	Self;
	//typedef typename Dom::Iterator	DomIterator;

	DomArrayView(ICPSolver& solver, const DomVarArray<Eval,dims,Dom>& p1) :
		Super(solver,p1.count())
	{
		for (UInt i = 0; i < this->size(); i++)
			::new(&Super::operator[](i)) Elem(solver,p1(i));
	}
};

/**
 *	Identity: DomArrayView over a DomArrayView.
 *  \ingroup Views
 *  \FIXME this is not working. Either fix it or disallow the use of views over
 *  views (which would probably make things simpler and easier to debug).
 */
template<class Eval,class View>
struct DomArrayView<Eval,DomArrayView<Eval,View> >; /*:
	detail::ViewArray<typename DomArrayView<Eval,View>::Elem>
{
	typedef typename DomTraits<View>::Dom	Dom;
	typedef typename DomArrayView<Eval,View>::Elem	Elem;
	typedef detail::ViewArray<Elem>	Super;

	DomArrayView(ICPSolver& solver, const DomArrayView<Eval,View>& p1) :
		Super(solver,p1.count())
	{
		for (UInt i = 0; i < this->size(); i++)
			::new(&Super::operator[](i)) Elem(solver,p1(i).getView());
	}
};*/


// domview over an element expression
// TODO: this is general domain element (requires ground index). Must specialize
// to integer domains for nonground index
/*
template<class Eval,class ArrayView,class IdxView,class DomT>
struct DomView<Eval,Rel2<Element,ArrayView,IdxView>,DomT> : IDomExpr<DomT>
{
	typedef	DomT			Dom;
	typedef IDomExpr<Dom>	Super;

	DomView(ICPSolver& solver, const Rel2<Element,ArrayView,IdxView>& r) :
		Super(solver),array(solver,r.p1),index(solver,r.p2) {}

	Dom*	operator->()
	{ 	return &(*array[index.value()]);	}
	Dom*	operator->() const
	{ 	return const_cast<Dom*>(&(*array[index.value()]));	}
	Dom&	operator*()
	{ 	return (*array[index.value()]);	}
	Dom&	operator*() const
	{ 	return const_cast<Dom&>((*array[index.value()]));	}

	DomArrayView<Eval,ArrayView>	array;
	ValView<Int,IdxView> 			index;
};*/


/*
template<class Eval,class View>
std::ostream& operator<<(std::ostream& os, const BndView<Eval,View>& v)
{
	if (v.min() == v.max())
		os << v.min();
	else
		os << "[" << v.min() << ".." << v.max() << "]";
	return os;
}*/


/*
template<class Eval,class View>
struct DomTraits<BndArrayView<Eval,View> >
{	typedef typename DomTraits<View>::Dom	Dom;	};

template<class Eval,class View>
struct DomTraits<DomArrayView<Eval,View> >
{	typedef typename DomTraits<View>::Dom	Dom;	};

template<class Eval,int dims,class DomT>
struct DomTraits<DomVarArray<Eval,dims,DomT> >
{	typedef DomT	Dom;	};
*/

template<class EvalT,class View>
struct EvalTraits<ValArrayView<EvalT,View> >
{	typedef Seq<EvalT>	Eval;	};

template<class EvalT,class View>
struct EvalTraits<BndArrayView<EvalT,View> >
{	typedef Seq<EvalT>	Eval;	};

template<class EvalT,class View>
struct EvalTraits<DomArrayView<EvalT,View> >
{	typedef Seq<EvalT>	Eval;	};

template<class Eval,class View>
struct ElemTraits<ValArrayView<Eval,View> >
{	typedef ValView<Eval,typename ElemTraits<View>::Elem>	Elem;	};

template<class Eval,class View>
struct ElemTraits<BndArrayView<Eval,View> >
{	typedef BndView<Eval,typename ElemTraits<View>::Elem>	Elem;	};

template<class Eval,class View>
struct ElemTraits<DomArrayView<Eval,View> >
{	//typedef typename ElemTraits<View>::Elem	Elem;
	typedef typename ElemTraits<View>::Elem	A;
	typedef DomView<Eval,A,typename DomTraits<A>::Dom>	Elem;	};

template<class Eval,class View>
struct GetCPSolver<DomArrayView<Eval,View> >
{	ICPSolver& operator()(const DomArrayView<Eval,View>& v) { return v.solver(); }	};

template<class Eval,class View>
struct GetCPSolver<BndArrayView<Eval,View> >
{	ICPSolver& operator()(const BndArrayView<Eval,View>& v) { return v.solver(); }	};

namespace detail {

// **** Find min and max of iterable sequences ****

template<class View>
struct FindMin
{
	typedef	typename EvalTraits<View>::Eval::Elem	Eval;
	Eval	operator()(const View& v)
	{
		BndArrayView<Eval,View>	a(getCPSolver(v),v);
		assert(a.size() >= 1);
		Eval ret = a[0].min();
		for (UInt i = 1; i < a.size(); i++)
			if (a[i].min() < ret)
				ret = a[i].min();
		return ret;
	}
};

template<class View>
struct FindMax
{
	typedef	typename EvalTraits<View>::Eval::Elem	Eval;
	Eval	operator()(const View& v)
	{
		BndArrayView<Eval,View>	a(getCPSolver(v),v);
		assert(a.size() >= 1);
		Eval ret = a[0].max();
		for (UInt i = 1; i < a.size(); i++)
			if (a[i].max() > ret)
				ret = a[i].max();
		return ret;
	}
};

template<class View>
typename FindMin<View>::Eval findMin(const View& view)
{	return FindMin<View>()(view);	}

template<class View>
typename FindMax<View>::Eval findMax(const View& view)
{	return FindMax<View>()(view);	}

/*
template<class EvalT,class View>
struct FindMin<BndArrayView<EvalT,View> >
{
	typedef	EvalT	Eval;
	Eval	operator()(const BndArrayView<Eval,View>& a)
	{
		assert(a.size() >= 1);
		Eval ret = a[0].min();
		for (UInt i = 1; i < a.size(); i++)
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
		for (UInt i = 1; i < a.size(); i++)
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
template<class View1,class Eval>
struct BndViewRel1<Min,View1,Eval>
{
	BndViewRel1(ICPSolver& solver, const View1& p1) :
		a(solver,p1) {}
	Eval min() const
	{
		Eval r = a[0].min();
		for (UInt i = 1; i < a.size(); i++)
			if (a[i].min() < r)
				r = a[i].min();
		return r;
	}
	Eval max() const
	{
		Eval r = a[0].max();
		for (UInt i = 1; i < a.size(); i++)
			if (a[i].max() < r)
				r = a[i].max();
		return r;
	}
	Bool updateMin(const Eval& v)
	{
		for (UInt i = 0; i < a.size(); i++)
			if (!a[i].updateMin(v))
				return false;
		return true;
	}
	// only does anything if there is only one element with min <= v
	Bool updateMax(const Eval& v)
	{
		Bool found = false;
		UInt mi = a.size();
		for (UInt i = 0; i < a.size(); i++)
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
	void detach(INotifiable* f) {	a.detach(f); }
	Rel1<Min,View1>	getView() const {	return rel<Min>(a.getView());	}

	BndArrayView<Eval,View1>	a;
};

/**
 *	BndView over maximum of an array.
 *	\ingroup Views
 **/
template<class View1,class Eval>
struct BndViewRel1<Max,View1,Eval>
{
	BndViewRel1(ICPSolver& solver, const View1& p1) :
		a(solver,p1) {}
	Eval min() const
	{
		Eval r = a[0].min();
		for (UInt i = 1; i < a.size(); i++)
			if (a[i].min() > r)
				r = a[i].min();
		return r;
	}
	Eval max() const
	{
		Eval r = a[0].max();
		for (UInt i = 1; i < a.size(); i++)
			if (a[i].max() > r)
				r = a[i].max();
		return r;
	}
	Bool updateMax(const Eval& v)
	{
		for (UInt i = 0; i < a.size(); i++)
			if (!a[i].updateMax(v))
				return false;
		return true;
	}
	// only does anything if there is only one element with max >= v
	Bool updateMin(const Eval& v)
	{
		Bool found = false;
		UInt mi = a.size();
		for (UInt i = 0; i < a.size(); i++)
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
	void detach(INotifiable* f) {	a.detach(f); }
	Rel1<Max,View1> getView() const {	return rel<Max>(a.getView()); }

	BndArrayView<Eval,View1>	a;
};

template<class Eval,class VarT,class SetT,class CondT,class ExprT>
struct BndArrayView<Eval,Rel4<All,VarT,SetT,CondT,ExprT> > :
	detail::ViewArray<BndView<Eval,ExprT> >
{
	typedef Rel4<All,VarT,SetT,CondT,ExprT> RelT;

	typedef BndView<Eval,ExprT>	Elem;
	typedef detail::ViewArray<Elem> Super;

	static UInt getSize(const RelT& p)
	{
		UInt r = 0;
		for (detail::PIteration<VarT,SetT,CondT> it(p.p1,p.p2,p.p3); it.valid(); it.iterate())
			r++;
		return r;
	}

	BndArrayView(ICPSolver& solver, const RelT& p) :
		Super(solver,getSize(p))
	{
		UInt i = 0;
		for (detail::PIteration<VarT,SetT,CondT> it(p.p1,p.p2,p.p3); it.valid(); it.iterate())
			::new(&Super::operator[](i++)) Elem(solver,p.p4);
	}

};

template<class Eval,class VarT,class SetT,class CondT,class ExprT>
struct DomArrayView<Eval,Rel4<All,VarT,SetT,CondT,ExprT> > :
	detail::ViewArray<DomView<Eval,ExprT> >
{
	typedef Rel4<All,VarT,SetT,CondT,ExprT> RelT;

	typedef DomView<Eval,ExprT>	Elem;
	typedef detail::ViewArray<Elem> Super;

	static UInt getSize(const RelT& p)
	{
		UInt r = 0;
		for (detail::PIteration<VarT,SetT,CondT> it(p.p1,p.p2,p.p3); it.valid(); it.iterate())
			r++;
		return r;
	}

	DomArrayView(ICPSolver& solver, const RelT& p) :
		Super(solver,getSize(p))
	{
		UInt i = 0;
		for (detail::PIteration<VarT,SetT,CondT> it(p.p1,p.p2,p.p3); it.valid(); it.iterate())
			::new(&Super::operator[](i++)) Elem(solver,p.p4);
	}

};

#define BNDVIEW_SUM_INCREMENTAL
#ifndef BNDVIEW_SUM_INCREMENTAL
/**
 *	BndView over a Sum of values.
 *	\ingroup Views
 **/
template<class View1,class Eval>
struct BndViewRel1<Sum,View1,Eval>
{
	BndViewRel1(ICPSolver& solver, const View1& p1) : v(solver,p1) {}
	Eval min() const
	{
		Eval r = 0;
		for (UInt i = 0; i < v.size(); ++i)
			r += v[i].min();
		return r;
	}
	Eval max() const
	{
		Eval r = 0;
		for (UInt i = 0; i < v.size(); ++i)
			r += v[i].max();
		return r;
	}
	Bool updateMin(const Eval& mi)
	{
		Eval m = max();
		for (UInt i = 0; i < v.size(); ++i)
			if (!v[i].updateMin(mi-m+v[i].max()))
				return false;
		return true;
	}
	Bool updateMax(const Eval& ma)
	{
		Eval m = min();
		for (UInt i = 0; i < v.size(); ++i)
			if (!v[i].updateMax(ma-m+v[i].min()))
				return false;
		return true;
	}
	Void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	Bool updateRange(const Eval& l, const Eval& u)
	{	return updateMin(l) and updateMax(u);	}

	void attach(INotifiable* f) { 	v.attach(f); }
	void detach(INotifiable* f) {	v.detach(f); }

	Rel1<Sum,View1> getView()  const
	{ 	return Rel1<Sum,View1>(v.getView());	}

	BndArrayView<Eval,View1>	v;
};
#else
/**
 *	BndView over a Sum of values.
 *	\ingroup Views
 **/
template<class View1,class Eval>
struct BndViewRel1<Sum,View1,Eval>
{
	BndViewRel1(ICPSolver& solver, const View1& p1) :
		v(solver,p1),cachedMin(solver),cachedMax(solver),
		cachedViewMin(solver,v.size(),0),
		cachedViewMax(solver,v.size(),0) { init(); }

	void init()
	{
		Eval mi = 0;
		Eval ma = 0;
		for (UInt i = 0; i < v.size(); ++i)
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
		TermDemon(BndViewRel1* pOwner, UInt idx) : rOwner(*pOwner),idx(idx) {}
		bool notify()
		{
			Bool ch = false;
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
		// tmp
		INotifiable* getFirst() { return rOwner.pOwner; }
		BndViewRel1& rOwner;
		const UInt idx;
	};
	Eval min() const
	{	return cachedMin;	}
	Eval max() const
	{	return cachedMax;	}
	Bool updateMin(const Eval& mi)
	{
		const Eval d = mi-cachedMax;
		for (UInt i = 0; i < v.size(); ++i)
			if (!v[i].updateMin(d+cachedViewMax[i]))
				return false;
		return true;
	}
	Bool updateMax(const Eval& ma)
	{
		const Eval d = ma-cachedMin;
		for (UInt i = 0; i < v.size(); ++i)
			if (!v[i].updateMax(d+cachedViewMin[i]))
				return false;
		return true;
	}
	Void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	Bool updateRange(const Eval& l, const Eval& u)
	{
		const Eval dl = l-cachedMax;
		const Eval du = u-cachedMin;
		for (UInt i = 0; i < v.size(); ++i)
			if (!v[i].updateRange(dl+cachedViewMax[i],du+cachedViewMin[i]))
				return false;
		return true;
	}

	void attach(INotifiable* f)
	{
		pOwner = f;
		for (UInt i = 0; i < v.size(); ++i)
			v[i].attach(new TermDemon(this,i));
		init();
	}
	void detach(INotifiable* f) {	}

	Rel1<Sum,View1> getView()  const
	{ 	return Rel1<Sum,View1>(v.getView());	}


	BndArrayView<Eval,View1>	v;
	Reversible<Eval>			cachedMin;
	Reversible<Eval>			cachedMax;
	detail::RevVector<Eval>		cachedViewMin;
	detail::RevVector<Eval>		cachedViewMax;
	INotifiable*				pOwner;
};
#endif

template<class> struct Range;

template<class View1,class View2,class Eval>
struct BndViewRel2<SumProduct,View1,View2,Eval> :
	BndViewRel1<Sum,Rel4<All,MutVar<Int>,Range<Int>,Bool,
		Rel2<Mul,Rel2<Element,View1,MutVar<Int> >,
				 Rel2<Element,View2,MutVar<Int> > > >,Eval>
{
	typedef Rel4<All,MutVar<Int>,Range<Int>,Bool,
				Rel2<Mul,Rel2<Element,View1,MutVar<Int> >,
						 Rel2<Element,View2,MutVar<Int> > > > Agg;
	Agg agg(ICPSolver& solver,const View1& v1, const View2& v2)
	{
		BndArrayView<Eval,View1> b1(solver,v1);
		MutVar<Int> i(solver);
		return all(i,range(0,b1.size()-1),true,v1[i]*v2[i]);
	}
	typedef BndViewRel1<Sum,Rel4<All,MutVar<Int>,Range<Int>,Bool,
			Rel2<Mul,Rel2<Element,View1,MutVar<Int> >,
					 Rel2<Element,View2,MutVar<Int> > > >,Eval> Super;
	BndViewRel2(ICPSolver& solver,const View1& v1, const View2& v2) :
		Super(solver,agg(solver,v1,v2))
		{}

};

};

#endif // _H_CASPER_KERNEL_VIEW_ARRAYVIEW
