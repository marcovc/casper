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
 

#ifndef CASPER_CP_PAR_PAR_H_
#define CASPER_CP_PAR_PAR_H_

#include <casper/kernel/par/view.h>
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
template<class ParExpr>
struct IterationView<Rel1<Domain,ParExpr> >
{
	typedef typename Traits::GetEval<ParExpr>::Type	Eval;
	typedef typename CP::Traits::GetDom<ParExpr>::Type Dom;
	typedef typename Dom::Iterator Iterator;
	typedef Rel1<Domain,ParExpr> DomainRel;
	typedef IterationView<DomainRel>	Self;
	IterationView(const DomainRel& r) : r(r),
										v(getState(r.p1),r.p1),
										it(v.value().domain().begin()) {}
	IterationView(const DomainRel& r, Iterator it) : r(r),
													 v(getState(r.p1),r.p1),
													 it(it) {}
	void		iterate()	{	assert(valid()); ++it;	}
	int			value() const 	{	assert(valid()); return *it;	}
	bool		valid() const 	{	return it != v.value().domain().end();	}
	Self		next() const {	return Self(r,++Iterator(it)); }
	DomainRel r;
	ParView<CP::Var<Eval>,ParExpr> v;
	Iterator it;
};


/**
 * 	ParView over groundness testing over a sequence type.
 * 	\ingroup Views
 **/
template<class Eval, class Obj>
struct ParView1<Ground,Seq<Eval>,Obj,bool> : IPar<bool>
{
	typedef typename Traits::GetElem<Obj>::Type	Elem;

	ParView1(State& state,const Obj& obj) :
		state(state),obj(obj) {}
	virtual ~ParView1() {}
	bool value() const
	{
		for (IterationView<Obj> it(obj); it.valid(); it.iterate())
			if (!ParView<CP::Var<Eval>,Elem>(state,it.value()).value().ground())
				return false;
		return true;
	}

	Rel1<Ground,Obj> getObj()  const { return ground(obj); }

	State& state;
	Obj obj;
};

/**
 * 	ParView over groundness testing.
 * 	\ingroup Views
 **/
template<class Eval,class Obj>
struct ParView1<Ground,Eval,Obj,bool> : IPar<bool>
{
	ParView1(State& state,const Obj& obj) :
		v(state,obj) {}
	virtual ~ParView1() {}
	bool value() const
	{	return 	v.value().ground();	}

	Rel1<Ground,Obj> getObj()  const { return ground(v.getObj()); }

	ParView<CP::Var<Eval>,Obj>	v;
};



/**
 * 	ParView for obtaining domain size.
 * 	\ingroup Views
 **/
template<class ViewEval,class View>
struct ParView1<DomainSize,ViewEval,View,int> : IPar<int>
{
	ParView1(State& state,const View& v) : v(state,v) {}
	int value() const
	{	return 	v.value().domain().size();	}
	Rel1<DomainSize,View> getObj()  const { return domainSize(v.getObj()); }

	ParView<CP::Var<ViewEval>,View>	v;
};



} // Casper

#endif /* PAR_H_ */
