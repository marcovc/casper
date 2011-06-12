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
struct NonGroundIdxs;
struct DomainSize;

namespace CP {
struct Store;
};

template<class T>
Rel2<Domain,CP::Store*,T> domain(CP::Store& store,const T& t)
{	return Rel2<Domain,CP::Store*,T>(&store,t);	}

template<class T>
Rel2<DomainSize,CP::Store*,T> domainSize(CP::Store& store,const T& t)
{	return Rel2<DomainSize,CP::Store*,T>(&store,t);	}

template<class T>
Rel2<Ground,CP::Store*,T> ground(CP::Store& store,const T& t)
{	return Rel2<Ground,CP::Store*,T>(&store,t);	}

template<class T>
Rel2<NonGroundIdxs,CP::Store*,T> nonGroundIdxs(CP::Store& store,const T& t)
{	return Rel2<NonGroundIdxs,CP::Store*,T>(&store,t);	}

namespace Traits {
template<class T>
struct GetEval<Rel2<Domain,CP::Store*,T> >
{	typedef	Seq<typename GetEval<T>::Type>	Type;	};

template<class T>
struct GetElem<Rel2<Domain,CP::Store*,T> >
{	typedef	typename GetEval<T>::Type	Type;	};

template<class T>
struct GetEval<Rel2<Ground,CP::Store*,T> >
{	typedef	bool	Type;	};

template<class T>
struct GetEval<Rel2<DomainSize,CP::Store*,T> >
{
	typedef	typename Casper::CP::Traits::GetDom<T>::Type::Size	Type;
};

template<class T>
struct GetEval<Rel2<NonGroundIdxs,CP::Store*,T> >
{	typedef	Seq<int>	Type;	};

} // Traits


namespace Util {

// specialization for finite domain variables
template<class ParExpr>
struct IterationView<Rel2<Domain,CP::Store*,ParExpr> >
{
	typedef typename Traits::GetEval<ParExpr>::Type	Eval;
	typedef typename CP::Traits::GetDom<ParExpr>::Type Dom;
	typedef typename Dom::Iterator Iterator;
	typedef Rel2<Domain,CP::Store*,ParExpr> DomainRel;
	typedef IterationView<DomainRel>	Self;
	IterationView(const DomainRel& r) : r(r),
										v(const_cast<CP::Store&>(*r.p1),r.p2),
										it(v->begin()) {}
	IterationView(const DomainRel& r, Iterator it) : r(r),
													 v(const_cast<CP::Store&>(*r.p1),r.p2),
													 it(it) {}
	void		iterate()	{	assert(valid()); ++it;	}
	int	value() const 	{	assert(valid()); return *it;	}
	bool		valid() const 	{	return it != v->end();	}
	Self		next() const {	return Self(r,++Iterator(it)); }
	DomainRel r;
	CP::DomView<Eval,ParExpr,Dom>	v;
	Iterator it;
};

// specialization for finite domain variables
template<class View>
struct IterationView<Rel2<NonGroundIdxs,CP::Store*,View> >
{
	typedef typename Traits::GetEval<typename Traits::GetElem<View>::Type>::Type	Eval;
	//typedef typename CP::Traits::GetDom<View>::Type Dom;
	//typedef typename Dom::Iterator Iterator;
	typedef Rel2<NonGroundIdxs,CP::Store*,View> Rel;
	typedef IterationView<Rel>	Self;
	void sync()
	{
		for ( ; idx < v.size(); ++idx)
			if (!v[idx].ground())
				break;
	}
	IterationView(const Rel& r) : store(const_cast<CP::Store&>(*r.p1)),
								  v(store,r.p2),
								  idx(0) {sync();}
	IterationView(CP::Store& store,const View& v, int idx) :
								  store(store),v(store,v),idx(idx) { sync(); }
	void		iterate()	{	assert(valid()); ++idx; sync();	}
	uint	value() const 	{	assert(valid()); return idx;	}
	bool		valid() const 	{	return idx < v.size();	}
	Self		next() const {	return Self(store,v.getObj(),idx+1); }

	CP::Store& store;
	CP::ValArrayView<Eval,View>	v;
	uint idx;
};


}

/**
 * 	ParView over groundness testing.
 * 	\ingroup Views
 **/
template<class View>
struct ParView2<Ground,CP::Store*,View,bool> : IPar<bool>
{
	typedef typename Traits::GetEval<View>::Type	ViewEval;
	ParView2(State& state,const CP::Store* store, const View& p1) :
		store(const_cast<CP::Store&>(*store)),p1(p1) {}
	bool value() const
	{	return 	CP::ValView<ViewEval,View>(store,p1).ground();	}

//	void attach(INotifiable* f) { 	CP::ValView<ViewEval,View>(store,p1).attach(f); }
//	void detach(INotifiable* f) {	CP::ValView<ViewEval,View>(store,p1).detach(f); }

	Rel2<Ground,CP::Store*,View> getObj()  const { return ground(store,p1); }


	CP::Store& store;
	View p1;
};


/**
 * 	ParView for obtaining domain size.
 * 	\ingroup Views
 **/
template<class View,class Size>
struct ParView2<DomainSize,CP::Store*,View,Size> : IPar<Size>
{
	typedef typename Traits::GetEval<View>::Type	ViewEval;
	ParView2(State& state,const CP::Store* store, const View& p1) :
		store(const_cast<CP::Store&>(*store)),p1(p1) {}
	Size value() const
	{	return 	CP::DomView<ViewEval,View>(store,p1)->size();	}
	Rel2<DomainSize,CP::Store*,View> getObj()  const { return domainSize(store,p1); }

	CP::Store& store;
	View p1;
};


} // Casper

#endif /* PAR_H_ */
