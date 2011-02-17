 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_KERNEL_VIEW_ITERATION_H_
#define CASPER_KERNEL_VIEW_ITERATION_H_

namespace casper {

/// \warning Make sure the view used for initializing this class is not
/// destroyed before b reaches e.
template<class View>
struct IterationView
{
	typedef typename ElemTraits<View>::Elem	Elem;
	typedef typename View::Iterator Iterator;
	typedef IterationView<View>	Self;
	IterationView(const View& v) : b(v.begin()),e(v.end()) {}
	IterationView(const IterationView& s) : b(s.b),e(s.e) {}
	IterationView(Iterator b,Iterator e) : b(b),e(e) {}
	Void		iterate()	{	assert(valid()); ++b;	}
	const Elem& 	value() const 	{	assert(valid()); return *b;	}
	Bool		valid() const 	{	return b!=e;	}
	Self		next() const {	return Self(++Iterator(b),e);	}
	Iterator	b;
	Iterator	e;
};

template<class T>
IterationView<T> makeIt(const T& v)
{	return IterationView<T>(v); }

/// \warning Make sure the view used for initializing this class is not
/// destroyed before b reaches e.
template<class View>
struct STLIterationView
{
	typedef typename View::value_type Elem;
	typedef typename View::iterator Iterator;
	typedef STLIterationView<View>	Self;
	STLIterationView(const View& v) : b(v.begin()),e(v.end()) {}
	STLIterationView(const STLIterationView& s) : b(s.b),e(s.e) {}
	STLIterationView(Iterator b,Iterator e) : b(b),e(e) {}
	Void		iterate()	{	assert(valid()); ++b;	}
	const Elem& 	value() const 	{	assert(valid()); return *b;	}
	Bool		valid() const 	{	return b!=e;	}
	Self		next() const {	return Self(++Iterator(b),e);	}
	Iterator	b;
	Iterator	e;
};

template<class T>
STLIterationView<T> makeSTLIt(const T& v)
{	return STLIterationView<T>(v); }

template<class T>
struct ElemTraits<IterationView<T> >
{	typedef typename ElemTraits<T>::Elem	Elem; };

template<class View>
struct IterationView<IterationView<View> >
{
	typedef typename ElemTraits<View>::Elem	Elem;
	typedef typename View::Iterator Iterator;
	IterationView(const IterationView<View>& v) : b(v.b),e(v.e) {}
	IterationView(Iterator b,Iterator e) : b(b),e(e) {}
	Void		iterate()	{	assert(valid()); ++b;	}
	const Elem& 	value() const 	{	assert(valid()); return *b;	}
	Bool		valid() const 	{	return b!=e;	}
	IterationView<View>	next() const
	{	return IterationView<View>(++Iterator(b),e);	}
	Iterator	b;
	Iterator	e;
};

template<class View>
struct EvalTraits<IterationView<View> >
{	typedef Seq<typename ElemTraits<View>::Elem>	Eval;	};

template<class ArrayT,class IndexT>
struct IterationView<Rel2<Element,ArrayT,IndexT> > :
	IterationView<typename ArrayT::Elem>
{
	typedef IterationView<typename ArrayT::Elem> Super;
	typedef Rel2<Element,ArrayT,IndexT> RelT;
	IterationView(const RelT& v) : Super(v.p1[v.p2.value()]) {}
};

// View1 is iteration, Pred is a predicate function on View1
template<class View1,class View2>
struct PredItView
{
	typedef typename ElemTraits<View1>::Elem	Elem;
	typedef PredItView<View1,View2>	Self;

	PredItView(const View1& v1,const View2& v2) :
		v1(v1),v2(v2) { sync(); }
	PredItView(const PredItView& s) : v1(s.v1),v2(s.v2) { }
	Void sync()
	{
		while (v1.valid() and !v2(v1.value()))
			v1.iterate();
	}
	Void		iterate()
	{
		assert(valid());
		v1.iterate();
		sync();
	}
	const Elem& 	value() const
	{
		assert(valid());
		return v1.value();
	}
	Bool		valid() const 	{	return v1.valid();	}
	View1	v1;
	View2	v2;
};


template<class T1,class T2>
PredItView<T1,T2> makePredIt(const T1& t1,const T2& t2)
{	return PredItView<T1,T2>(t1,t2); }

template<class T1,class T2>
struct ElemTraits<PredItView<T1,T2> >
{	typedef typename ElemTraits<T1>::Elem	Elem; };

// View1 and View2 are already iterations
template<class View1,class View2>
struct UnionItView
{
	typedef typename ElemTraits<View1>::Elem	Elem;
	typedef UnionItView<View1,View2>	Self;

	UnionItView(const View1& v1,const View2& v2) :
		v1(v1),v2(v2) {}
	UnionItView(const UnionItView& s) : v1(s.v1),v2(s.v2) {}
	Void		iterate()
	{
		assert(valid());
		if (!v1.valid())
			v2.iterate();
		else
		if (!v2.valid())
			v1.iterate();
		else
		if (detail::less(v1.value(),v2.value()))
			v1.iterate();
		else
		if (detail::less(v2.value(),v1.value()))
			v2.iterate();
		else
		{
			v1.iterate();
			v2.iterate();
		}
	}
	const Elem& 	value() const
	{
		assert(valid());
		if (!v1.valid())
			return v2.value();
		else
		if (!v2.valid())
			return v1.value();
		else
		if (detail::lessEqual(v1.value(),v2.value()))
			return v1.value();
		else
			return v2.value();
	}
	Bool		valid() const 	{	return v1.valid() or v2.valid();	}
	View1	v1;
	View2	v2;
};

template<class T1,class T2>
UnionItView<T1,T2> makeUnionIt(const T1& t1,const T2& t2)
{	return UnionItView<T1,T2>(t1,t2); }

template<class T1,class T2>
struct ElemTraits<UnionItView<T1,T2> >
{	typedef typename ElemTraits<T1>::Elem	Elem; };

// View1 and View2 are already iterations
template<class View1,class View2>
struct InterItView
{
	typedef typename ElemTraits<View1>::Elem	Elem;
	typedef InterItView<View1,View2>	Self;

	Void sync()
	{
		if (!v1.valid() or !v2.valid())
				return;

		while (detail::distinct(v2.value(),v1.value()) )
		{
			while (detail::less(v1.value(),v2.value()))
			{
				v1.iterate();
				if (!v1.valid())
					return;
			}

			while (detail::less(v2.value(),v1.value()))
			{
				v2.iterate();
				if (!v2.valid())
					return;
			}
		}
	}
	InterItView(const View1& vv1,const View2& vv2) :
		v1(vv1),v2(vv2)
	{ sync();	}
	InterItView(const InterItView& s) : v1(s.v1),v2(s.v2) {}
	Void		iterate()
	{
		assert(valid());

		v1.iterate();
		v2.iterate();

		sync();
	}
	const Elem& 	value() const
	{
		assert(valid());
		return v1.value();
	}
	Bool		valid() const 	{	return v1.valid() and v2.valid();	}
	View1	v1;
	View2	v2;
};

template<class T1,class T2>
InterItView<T1,T2> makeInterIt(const T1& t1,const T2& t2)
{	return InterItView<T1,T2>(t1,t2); }

template<class T1,class T2>
struct ElemTraits<InterItView<T1,T2> >
{	typedef typename ElemTraits<T1>::Elem	Elem; };

// View1 and View2 are already iterations
template<class View1,class View2>
struct DiffItView
{
	typedef typename ElemTraits<View1>::Elem	Elem;
	typedef DiffItView<View1,View2>	Self;

	// find first element of v1 not in v2
	Void sync()
	{
		while ( v1.valid() and v2.valid())
		{
			while (detail::less(v2.value(),v1.value() ))
			{
				v2.iterate();
				if (!v2.valid())
					return;
			}

			if (detail::less(v1.value(),v2.value()))
				return;

			v1.iterate();
			v2.iterate();
		}
	}

	DiffItView(const View1& v1,const View2& v2) :
		v1(v1),v2(v2) {	sync(); }
	DiffItView(const DiffItView& s) : v1(s.v1),v2(s.v2) {}
	Void		iterate()
	{
		assert(valid());
		v1.iterate();
		sync();
	}
	const Elem& 	value() const
	{
		assert(valid());
		return v1.value();
	}
	Bool		valid() const 	{	return v1.valid();	}
	View1	v1;
	View2	v2;
};

template<class T1,class T2>
DiffItView<T1,T2> makeDiffIt(const T1& t1,const T2& t2)
{	return DiffItView<T1,T2>(t1,t2); }

template<class T1,class T2>
struct ElemTraits<DiffItView<T1,T2> >
{	typedef typename ElemTraits<T1>::Elem	Elem; };

// Symmetric difference
// View1 and View2 are already iterations
template<class View1,class View2>
struct SymDiffItView
{
	typedef typename ElemTraits<View1>::Elem	Elem;
	typedef SymDiffItView<View1,View2>	Self;

	// find first different element
	Void sync()
	{
		while ( v1.valid() and v2.valid() and
				detail::equal(v1.value(),v2.value()))
		{
			v1.iterate();
			v2.iterate();
		}
	}

	SymDiffItView(const View1& v1,const View2& v2) :
		v1(v1),v2(v2) {	sync(); }
	SymDiffItView(const SymDiffItView& s) : v1(s.v1),v2(s.v2) {}
	Void		iterate()
	{
		assert(valid());
		if (!v2.valid())
			v1.iterate();
		else
		if (!v1.valid())
			v2.iterate();
		else
		if (v1.value() < v2.value())
			v1.iterate();
		else
			v2.iterate();
		sync();
	}
	const Elem& 	value() const
	{
		assert(valid());
		if (!v2.valid())
			return v1.value();
		else
		if (!v1.valid())
			return v2.value();
		else
		if (v1.value() < v2.value())
			return v1.value();
		else
			return v2.value();
	}
	Bool		valid() const 	{	return v1.valid() or v2.valid();	}
	View1	v1;
	View2	v2;
};


template<class T1,class T2>
SymDiffItView<T1,T2> makeSymDiffIt(const T1& t1,const T2& t2)
{	return SymDiffItView<T1,T2>(t1,t2); }

template<class T1,class T2>
struct ElemTraits<SymDiffItView<T1,T2> >
{	typedef typename ElemTraits<T1>::Elem	Elem; };

#ifdef CASPER_UNUSED
// Provides a sorted view over an unsorted iteration
// View1 is an iteration, Less is comparison operator
// cost = O(n^2)
template<class View,class Less>
struct SortedItView
{
	typedef typename ElemTraits<View>::Elem Elem;
	SortedItView(const View& v) : origView(v),pCurElem(NULL) // O(n)
	{
		View view(origView);
		if (!view.valid())
			return;
		pCurElem = new Elem(view.value());
		view.iterate();
		for ( ; view.valid(); view.iterate())
			if (less(view.value(),*pCurElem))
				*pCurElem = view.value();
	}

	SortedItView(const SortedItView& s) : origView(s.origView),
			pCurElem(s.valid()?new Elem(*s.pCurElem):NULL)
	{}

	~SortedItView()
	{
		if (pCurElem!=NULL)
			delete pCurElem;
	}

	const Elem& 	value() const
	{	assert(valid()); return *pCurElem; }

	Void		iterate() // O(n)
	{
		assert(valid());
		Elem minElem;
		Bool first = true;
		for (View view(origView); view.valid(); view.iterate())
			if (less(*pCurElem,view.value()) and
				(first or less(view.value(),minElem)))
			{
				first = false;
				minElem = view.value();
			}
		if (first)
		{
			delete pCurElem;
			pCurElem = NULL;
		}
		else
			*pCurElem = minElem;
	}

	Bool		valid() const 	{	return pCurElem != NULL; }

	View 	origView;
	Elem* 	pCurElem;
	Less	less;
};

template<class View,class Less>
struct ElemTraits<SortedItView<View,Less> >
{	typedef typename ElemTraits<View>::Elem	Elem; };

template<class View,class Less>
SortedItView<View,Less> makeSortedIt(const View& v)
{	return v;	}

template<class Elem,class Iterator>
struct IteratorPairItView
{
	IteratorPairItView(const IteratorPairItView& s) : b(s.b),e(s.e) {}
	IteratorPairItView(Iterator b,Iterator e) : b(b),e(e) {}
	Void		iterate()	{	assert(valid()); ++b;	}
	const Elem& 	value() const 	{	assert(valid()); return *b;	}
	Bool		valid() const 	{	return b!=e;	}
	Iterator	b;
	Iterator	e;
};

template<class Elem,class Iterator>
IteratorPairItView<Elem,Iterator> makeIteratorPairIt(Iterator b, Iterator e)
{	return IteratorPairItView<Elem,Iterator>(b,e); }

template<class ElemT,class Iterator>
struct ElemTraits<IteratorPairItView<ElemT,Iterator> >
{	typedef ElemT Elem; };

// iteration of iterations
template<class Iteration>
struct IterationItView
{
	typedef typename ElemTraits<Iteration>::Elem ItElem;	// ItElem = Delta
	typedef typename ElemTraits<ItElem>::Elem	Elem;
	typedef IterationView<ItElem> ItElemIt;

	IterationItView(Iteration it) :
		globalIt(it),pCurIt(NULL)
	{
		if (globalIt.valid())
			pCurIt = new ItElemIt(globalIt.value());
	}

	IterationItView(const IterationItView& s) :
		globalIt(s.globalIt)
	{
		if (s.pCurIt == NULL)
			pCurIt = NULL;
		else
			pCurIt = new ItElemIt(globalIt.value());
	}

	~IterationItView()
	{
		if (pCurIt!=NULL)
			delete pCurIt;
	}

	const Elem& value() const
	{	assert(valid()); return pCurIt->value(); }

	Void iterate()
	{
		assert(valid());
		pCurIt->iterate();
		if (!pCurIt->valid())
		{
			globalIt.iterate();
			if (globalIt.valid())
				*pCurIt = ItElemIt(globalIt.value());
			else
			{
				delete pCurIt;
				pCurIt = NULL;
			}
		}
	}

	Bool valid() const
	{	assert(pCurIt==NULL or pCurIt->valid()); return pCurIt != NULL;	}

	Iteration globalIt;
	ItElemIt* pCurIt;
};

template<class Iteration>
struct ElemTraits<IterationItView<Iteration> >
{	typedef typename ElemTraits<typename ElemTraits<Iteration>::Elem>::Elem Elem; };

template<class Iteration>
IterationItView<Iteration> makeIterationIt(const Iteration& it)
{	return it; }
#endif

namespace detail {

template<class IView>
UInt distance(IView iview)
{
	UInt ret = 0;
	while (iview.valid())
	{
		++ret;
		iview.iterate();
	}
	return ret;
}

template<class IView>
void printIteration(IView iview)
{
	while (iview.valid())
	{
		std::cout << iview.value() << " ";
		iview.iterate();
	}
	std::cout << std::endl;
}

template<class InputIterator>
void print(InputIterator b,InputIterator e)
{
	for ( ; b!=e; ++b)
		std::cout << *b << " ";
}

}

}

#endif /*CASPER_KERNEL_VIEW_ITERATION_H_*/
