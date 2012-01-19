/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_CP_REF_REF_H_
#define CASPER_CP_REF_REF_H_

#include <casper/kernel/ref/view.h>
#include <casper/kernel/view/element.h>
#include <casper/cp/view/val.h>

namespace Casper {

struct Domain;
struct Ground;
struct DomainSize;

namespace CP {
struct Store;
};

NEW_REL_1(domain,Domain)

NEW_REL_1(domainSize,DomainSize)

//template<class T,int dims,class Dom>
//Rel1<Ground,CP::VarArray<T,dims,Dom> > ground(const CP::VarArray<T,dims,Dom>& vars)
//{	return Rel1<Ground,CP::VarArray<T,dims,Dom> >(vars);	}

NEW_REL_1(ground,Ground)

namespace Traits {

template<class T>
struct GetEval<Rel1<Domain,T> >
{	typedef	Seq<typename GetEval<T>::Type>	Type;	};

template<class T>
struct GetElem<Rel1<Domain,T> >
{	typedef	typename GetEval<T>::Type	Type;	};

template<class T>
struct GetEval<Rel1<Ground,T> >
{	typedef	bool	Type;	};


template<class T>
struct GetEval<Rel1<DomainSize,T> >
{
	//typedef	typename Casper::CP::Traits::GetDom<T>::Type::Size	Type;
	typedef	int	Type;
};

} // Traits



// specialization for finite domain variables
template<class RefExpr>
struct IterationView<Rel1<Domain,RefExpr> >
{
	typedef typename Traits::GetEval<RefExpr>::Type	Eval;
	typedef typename CP::Traits::GetDom<RefExpr>::Type Dom;
	typedef typename Dom::Iterator Iterator;
	typedef Rel1<Domain,RefExpr> DomainRel;
	typedef IterationView<DomainRel>	Self;
	IterationView(const DomainRel& r) :var(RefView<CP::Var<Eval>,RefExpr>(getState(r.p1),r.p1).value()),
									   it(var.domain().begin()) {}
//	IterationView(const DomainRel& r, Iterator it) : r(r),
//													 v(getState(r.p1),r.p1),
//													 it(it) {}
	IterationView(const IterationView& s) : var(s.var),
											it(s.it) {}
	void		iterate()	{	assert(valid()); ++it;	}
	Eval		value() const 	{	assert(valid()); return *it;	}
	bool		valid() const 	{	return it != var.domain().end();	}
	//RefView<CP::Var<Eval>,RefExpr> v;
	CP::Var<Eval> var;
	Iterator it;
};

template<class RefExpr,class Elem>
struct ElementView<Rel1<Domain,RefExpr>,Elem> : NoElementView<Rel1<Domain,RefExpr>,Elem >
{
	ElementView(const Rel1<Domain,RefExpr>& r) :
		NoElementView<Rel1<Domain,RefExpr>,Elem>(r) {}
};

/**
 * 	RefView over groundness testing over a sequence type.
 * 	\ingroup Views
 **/
template<class Eval, class Obj>
struct RefView1<Ground,Seq<Eval>,Obj,bool> : IRef<bool>
{
	typedef typename Traits::GetElem<Obj>::Type	Elem;

	RefView1(State& state,const Obj& obj) :
		state(state),obj(obj) {}
	virtual ~RefView1() {}
	bool value() const
	{
		for (IterationView<Obj> it(obj); it.valid(); it.iterate())
			if (!RefView<CP::Var<Eval>,Elem>(state,it.value()).value().ground())
				return false;
		return true;
	}

	Rel1<Ground,Obj> getObj()  const { return ground(obj); }

	State& state;
	Obj obj;
};

/**
 * 	RefView over groundness testing.
 * 	\ingroup Views
 **/
template<class Eval,class Obj>
struct RefView1<Ground,Eval,Obj,bool> : IRef<bool>
{
	RefView1(State& state,const Obj& obj) :
		v(state,obj) {}
	virtual ~RefView1() {}
	bool value() const
	{	return 	v.value().ground();	}

	Rel1<Ground,Obj> getObj()  const { return ground(v.getObj()); }

	RefView<CP::Var<Eval>,Obj>	v;
};



/**
 * 	RefView for obtaining domain size.
 * 	\ingroup Views
 **/
template<class ViewEval,class View>
struct RefView1<DomainSize,ViewEval,View,int> : IRef<int>
{
	RefView1(State& state,const View& v) : v(state,v) {}
	int value() const
	{	return 	v.value().domain().size();	}
	Rel1<DomainSize,View> getObj()  const { return domainSize(v.getObj()); }

	RefView<CP::Var<ViewEval>,View>	v;
};



} // Casper

#endif /* PAR_H_ */
