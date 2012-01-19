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

#ifndef CASPER_KERNEL_OBJ_ELEMENT_H_
#define CASPER_KERNEL_OBJ_ELEMENT_H_

#include <casper/kernel/expr.h>
#include <casper/kernel/spexpr/expr.h>

namespace Casper {
namespace Detail {


 
// 1. General case. Fails since SourceElemEval must equal TargetElemEval
template<class T,class SourceElemEval,class TargetElemEval>
struct GetElementHelper
{	Expr<TargetElemEval> operator()(const T&,uint) {	throw 0; }};

// 2. Exception to 1. Succeeds.
template<class T,class ElemEval>
struct GetElementHelper<T,ElemEval,ElemEval>
{	Expr<ElemEval> operator()(const T& t,uint idx) {	return ElementView<T>(t).get(idx);}};

// 3. Expr<Seq<Eval> > may be a sequence of sequences. If it is NOT then this rule succeeds.
template<class Eval>
struct GetElementHelper<Expr<Seq<Eval> >,Eval,Eval>
{	Expr<Eval> operator()(const Expr<Seq<Eval> >& t,uint idx) {	return ElementView<Expr<Seq<Eval> >,Expr<Eval> >(t).get(idx);}};

// 4. Expr<Seq<Eval> > may be a sequence of sequences. If it is then this rule succeeds.
template<class Eval>
struct GetElementHelper<Expr<Seq<Eval> >,Seq<Eval>,Seq<Eval> >
{	Expr<Seq<Eval> > operator()(const Expr<Seq<Eval> >& t,uint idx) {	return ElementView<Expr<Seq<Eval> >,Expr<Seq<Eval> > >(t).get(idx);}};

// 5. Same as 3 but for Element.
template<class Eval>
struct GetElementHelper<Rel2<Casper::Element,Expr<Seq<Eval> >,Expr<int> >,Eval,Eval>
{
	Expr<Eval> operator()(const Rel2<Casper::Element,Expr<Seq<Eval> >,Expr<int> >& t,uint idx)
	{	return ElementView<Rel2<Casper::Element,Expr<Seq<Eval> >,Expr<int> >,Expr<Eval> >(t).get(idx);}
};

// 6. Same as 4 but for Element.
template<class Eval>
struct GetElementHelper<Rel2<Casper::Element,Expr<Seq<Eval> >,Expr<int> >,Eval,Seq<Eval> >
{
	Expr<Seq<Eval> > operator()(const Rel2<Casper::Element,Expr<Seq<Eval> >,Expr<int> >& t,uint idx)
	{	return ElementView<Rel2<Casper::Element,Expr<Seq<Eval> >,Expr<int> >,Expr<Seq<Eval> > >(t).get(idx);}
};

// 1. General case
template<class T,class TargetElemEval>
struct GetElement
{	Expr<TargetElemEval> operator()(const T& t,uint idx)
{	return GetElementHelper<T,typename Traits::GetEval<typename Traits::GetElem<T>::Type>::Type,TargetElemEval>()(t,idx); }};


// 2. Same as previous to workaround the fact that GetElem is not defined over Element<Expr<Seq<Eval> > >
template<class Eval1,class Eval2>
struct GetElement<Casper::Rel2<Casper::Element,Casper::Expr<Casper::Seq<Eval1> >,Casper::Expr<int> >,Eval2>
{
	typedef Casper::Rel2<Casper::Element,Casper::Expr<Casper::Seq<Eval1> >,Casper::Expr<int> > R;
	Expr<Eval2> operator()(const R& t,uint idx)
	{	return GetElementHelper<R,Eval1,Eval2>()(t,idx); }
};


template<class T>
struct IsSeq
{	bool operator()() const {	return false;	} };

template<class T>
struct IsSeq<Seq<T> >
{	bool operator()() const {	return true;	} };

template<class T>
struct HasSeqElement
{
	typedef typename Traits::GetElem<T>::Type Elem;
	typedef typename Traits::GetEval<Elem>::Type	ElemEval;
	bool operator()(const T&) const
	{	return IsSeq<ElemEval>()();	}
};

template<class T>
struct HasSeqElement<Expr<Seq<T> > >
{
	bool operator()(const Expr<Seq<T> >& e) const
	{	return e.hasSeqElement();	}
};

} // Detail


template<class Eval>
struct ElementView<Expr<Seq<Eval> >, typename Traits::GetTermElem<Expr<Seq<Eval> > >::Type >
{
	ElementView(const Expr<Seq<Eval> >& e) : e(e) {}
	Expr<Eval> get(uint i) const {	return e.getElement(i);	}
	Expr<Seq<Eval> > e;
};

template<class Eval>
struct ElementView<Expr<Seq<Eval> >,Expr<Seq<Eval> > >
{
	ElementView(const Expr<Seq<Eval> >& e) : e(e) {}
	Expr<Seq<Eval> > get(uint i) const {	return e.getSeqElement(i);	}
	Expr<Seq<Eval> > e;
};


template<class Eval>
struct ElementView<Rel2<Element,Expr<Seq<Eval> >,Expr<int> >,typename Traits::GetTermElem<Rel2<Element,Expr<Seq<Eval> >,Expr<int> > >::Type >
{
	ElementView(const Rel2<Element,Expr<Seq<Eval> >,Expr<int> >& e) :
		expr(e.p1.getSeqElement(e.p2.toRef(getState(e)).value())) {}

	Expr<Eval> 			get(uint i) const 	{	return expr.getElement(i);  }

	Expr<Seq<Eval> > expr;	// required to make sure expr does not get deleted while using iteration
};

template<class Eval>
struct ElementView<Rel2<Element,Expr<Seq<Eval> >,Expr<int> >,Expr<Seq<Eval> > >
{
	ElementView(const Rel2<Element,Expr<Seq<Eval> >,Expr<int> >& e) :
		expr(e.p1.getSeqElement(e.p2.toRef(getState(e)).value())) {}

	Expr<Seq<Eval> > 			get(uint i) const 	{	return expr.getSeqElement(i);  }

	Expr<Seq<Eval> > expr;	// required to make sure expr does not get deleted while using iteration
};

}

#endif /*CASPER_KERNEL_OBJ_ELEMENT_H_*/
