 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <casper/kernel/traits.h>
#include <casper/util/container/common.h>

namespace Casper {

/// \warning Make sure the view used for initializing this class is not
/// destroyed before b reaches e.
template<class View>
struct IterationView
{
	typedef typename Traits::GetElem<View>::Type	Elem;
	typedef typename View::Iterator Iterator;
	typedef IterationView<View>	Self;
	IterationView(const View& v) : b(v.begin()),e(v.end()) {}
	IterationView(const IterationView& s) : b(s.b),e(s.e) {}
	IterationView(Iterator b,Iterator e) : b(b),e(e) {}
	void		iterate()	{	assert(valid()); ++b;	}
	const Elem& 	value() const 	{	assert(valid()); return *b;	}
	bool		valid() const 	{	return b!=e;	}
	Self		next() const {	return Self(++Iterator(b),e);	}
	Iterator	b;
	Iterator	e;
};

#ifndef _MSC_VER
// iteration view over an std::initializer_list
template<class T>
struct IterationView<std::initializer_list<T> >
{
	typedef T	Elem;
	typedef typename std::initializer_list<T>::iterator Iterator;
	typedef IterationView<std::initializer_list<T> >	Self;
	IterationView(const std::initializer_list<T>& v) : b(v.begin()),e(v.end()) {}
	IterationView(const IterationView& s) : b(s.b),e(s.e) {}
	IterationView(Iterator b,Iterator e) : b(b),e(e) {}
	void		iterate()	{	assert(valid()); ++b;	}
	const Elem& 	value() const 	{	assert(valid()); return *b;	}
	bool		valid() const 	{	return b!=e;	}
	Self		next() const {	Iterator c(b); return Self(++c,e);	}
	Iterator	b;
	Iterator	e;
};
#endif

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
	void		iterate()	{	assert(valid()); ++b;	}
	const Elem& 	value() const 	{	assert(valid()); return *b;	}
	bool		valid() const 	{	return b!=e;	}
	Self		next() const {	return Self(++Iterator(b),e);	}
	Iterator	b;
	Iterator	e;
};

template<class T>
STLIterationView<T> makeSTLIt(const T& v)
{	return STLIterationView<T>(v); }

template<class View>
struct IterationView<IterationView<View> >
{
	typedef typename Traits::GetElem<View>::Type Elem;
	typedef typename IterationView<View>::Iterator Iterator;
	IterationView(const IterationView<View>& v) : v(v) {}
	void		iterate()	{	v.iterate(); }
	Elem 	value() const 	{	return v.value(); }
	bool		valid() const 	{	return v.valid();	}
	IterationView<View>	next() const
	{	return v.next(); }
	IterationView<View> v;
};


// Expr1 is iteration, Pred is a predicate function on Expr1
template<class Expr1,class Expr2>
struct PredItView
{
	typedef typename Traits::GetElem<Expr1>::Type	Elem;
	typedef PredItView<Expr1,Expr2>	Self;

	PredItView(const Expr1& v1,const Expr2& v2) :
		v1(v1),v2(v2) { sync(); }
	PredItView(const PredItView& s) : v1(s.v1),v2(s.v2) { }
	void sync()
	{
		while (v1.valid() and !v2(v1.value()))
			v1.iterate();
	}
	void		iterate()
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
	bool		valid() const 	{	return v1.valid();	}
	Expr1	v1;
	Expr2	v2;
};


template<class T1,class T2>
PredItView<T1,T2> makePredIt(const T1& t1,const T2& t2)
{	return PredItView<T1,T2>(t1,t2); }

// Expr1 and Expr2 are already iterations
template<class Expr1,class Expr2>
struct UnionItView
{
	typedef typename Traits::GetElem<Expr1>::Type	Elem;
	typedef UnionItView<Expr1,Expr2>	Self;

	UnionItView(const Expr1& v1,const Expr2& v2) :
		v1(v1),v2(v2) {}
	UnionItView(const UnionItView& s) : v1(s.v1),v2(s.v2) {}
	void		iterate()
	{
		assert(valid());
		if (!v1.valid())
			v2.iterate();
		else
		if (!v2.valid())
			v1.iterate();
		else
		if (Casper::Util::Detail::less(v1.value(),v2.value()))
			v1.iterate();
		else
		if (Casper::Util::Detail::less(v2.value(),v1.value()))
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
		if (Casper::Util::Detail::lessEqual(v1.value(),v2.value()))
			return v1.value();
		else
			return v2.value();
	}
	bool		valid() const 	{	return v1.valid() or v2.valid();	}
	Expr1	v1;
	Expr2	v2;
};

template<class T1,class T2>
UnionItView<T1,T2> makeUnionIt(const T1& t1,const T2& t2)
{	return UnionItView<T1,T2>(t1,t2); }

// Expr1 and Expr2 are already iterations
template<class Expr1,class Expr2>
struct InterItView
{
	typedef typename Traits::GetElem<Expr1>::Type	Elem;
	typedef InterItView<Expr1,Expr2>	Self;

	void sync()
	{
		if (!v1.valid() or !v2.valid())
				return;

		while (Casper::Util::Detail::distinct(v2.value(),v1.value()) )
		{
			while (Casper::Util::Detail::less(v1.value(),v2.value()))
			{
				v1.iterate();
				if (!v1.valid())
					return;
			}

			while (Casper::Util::Detail::less(v2.value(),v1.value()))
			{
				v2.iterate();
				if (!v2.valid())
					return;
			}
		}
	}
	InterItView(const Expr1& vv1,const Expr2& vv2) :
		v1(vv1),v2(vv2)
	{ sync();	}
	InterItView(const InterItView& s) : v1(s.v1),v2(s.v2) {}
	void		iterate()
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
	bool		valid() const 	{	return v1.valid() and v2.valid();	}
	Expr1	v1;
	Expr2	v2;
};

template<class T1,class T2>
InterItView<T1,T2> makeInterIt(const T1& t1,const T2& t2)
{	return InterItView<T1,T2>(t1,t2); }

// Expr1 and Expr2 are already iterations
template<class Expr1,class Expr2>
struct DiffItView
{
	typedef typename Traits::GetElem<Expr1>::Type	Elem;
	typedef DiffItView<Expr1,Expr2>	Self;

	// find first element of v1 not in v2
	void sync()
	{
		while ( v1.valid() and v2.valid())
		{
			while (Casper::Util::Detail::less(v2.value(),v1.value() ))
			{
				v2.iterate();
				if (!v2.valid())
					return;
			}

			if (Casper::Util::Detail::less(v1.value(),v2.value()))
				return;

			v1.iterate();
			v2.iterate();
		}
	}

	DiffItView(const Expr1& v1,const Expr2& v2) :
		v1(v1),v2(v2) {	sync(); }
	DiffItView(const DiffItView& s) : v1(s.v1),v2(s.v2) {}
	void		iterate()
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
	bool		valid() const 	{	return v1.valid();	}
	Expr1	v1;
	Expr2	v2;
};

template<class T1,class T2>
DiffItView<T1,T2> makeDiffIt(const T1& t1,const T2& t2)
{	return DiffItView<T1,T2>(t1,t2); }

// Symmetric difference
// Expr1 and Expr2 are already iterations
template<class Expr1,class Expr2>
struct SymDiffItView
{
	typedef typename Traits::GetElem<Expr1>::Type	Elem;
	typedef SymDiffItView<Expr1,Expr2>	Self;

	// find first different element
	void sync()
	{
		while ( v1.valid() and v2.valid() and
				Casper::Util::Detail::equal(v1.value(),v2.value()))
		{
			v1.iterate();
			v2.iterate();
		}
	}

	SymDiffItView(const Expr1& v1,const Expr2& v2) :
		v1(v1),v2(v2) {	sync(); }
	SymDiffItView(const SymDiffItView& s) : v1(s.v1),v2(s.v2) {}
	void		iterate()
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
	bool		valid() const 	{	return v1.valid() or v2.valid();	}
	Expr1	v1;
	Expr2	v2;
};


template<class T1,class T2>
SymDiffItView<T1,T2> makeSymDiffIt(const T1& t1,const T2& t2)
{	return SymDiffItView<T1,T2>(t1,t2); }


#ifdef CASPER_UNUSED
// Provides a sorted view over an unsorted iteration
// Expr1 is an iteration, Less is comparison operator
// cost = O(n^2)
template<class View,class Less>
struct SortedItView
{
	typedef typename Traits::GetElem<View>::Type Elem;
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

	void		iterate() // O(n)
	{
		assert(valid());
		Elem minElem;
		bool first = true;
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

	bool		valid() const 	{	return pCurElem != NULL; }

	View 	origView;
	Elem* 	pCurElem;
	Less	less;
};

template<class View,class Less>
struct Traits::GetElem<SortedItView<View,Less> >::Typeedef typename Traits::GetElem<View>::Type	Elem; };

template<class View,class Less>
SortedItView<View,Less> makeSortedIt(const View& v)
{	return v;	}

template<class Elem,class Iterator>
struct IteratorPairItView
{
	IteratorPairItView(const IteratorPairItView& s) : b(s.b),e(s.e) {}
	IteratorPairItView(Iterator b,Iterator e) : b(b),e(e) {}
	void		iterate()	{	assert(valid()); ++b;	}
	const Elem& 	value() const 	{	assert(valid()); return *b;	}
	bool		valid() const 	{	return b!=e;	}
	Iterator	b;
	Iterator	e;
};

template<class Elem,class Iterator>
IteratorPairItView<Elem,Iterator> makeIteratorPairIt(Iterator b, Iterator e)
{	return IteratorPairItView<Elem,Iterator>(b,e); }

template<class ElemT,class Iterator>
struct Traits::GetElem<IteratorPairItView<ElemT,Iterator> >::Typeedef ElemT Elem; };

// iteration of iterations
template<class Iteration>
struct IterationItView
{
	typedef typename Traits::GetElem<Iteration>::Type ItElem;	// ItElem = Delta
	typedef typename Traits::GetElem<ItElem>::Type	Elem;
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

	void iterate()
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

	bool valid() const
	{	assert(pCurIt==NULL or pCurIt->valid()); return pCurIt != NULL;	}

	Iteration globalIt;
	ItElemIt* pCurIt;
};

template<class Iteration>
struct Traits::GetElem<IterationItView<Iteration> >::Typeedef typename Traits::GetElem<typename ElemTraits<Iteration>::Elem>::Type Elem; };

template<class Iteration>
IterationItView<Iteration> makeIterationIt(const Iteration& it)
{	return it; }
#endif

namespace Detail {

template<class IView>
uint distance(IView iview)
{
	uint ret = 0;
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

} // Detail

} // Casper

#endif /*CASPER_KERNEL_VIEW_ITERATION_H_*/
