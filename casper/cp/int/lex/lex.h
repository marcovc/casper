 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2009 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_CP_INT_LEX
#define _H_CASPER_CP_INT_LEX

#include <casper/cp/view/array.h>
#include <casper/cp/filter/bnd.h>
#include <casper/kernel/reversible/stack.h>
#include <casper/cp/delta.h>

//#ifdef CASPER_PRECOMPILED
//#include <casper/cp/int/spexpr/explicit_postbnd.h>
//#endif

namespace Casper {
namespace CP {


/**
 *  Enforces the lexicographic constraint [\ref carlsson02].
 *
 *  \ingroup IntFilters
 */
template<class Expr1,class Expr2>
struct BndFilterView2<LessEqual,IntSeq,Expr1,IntSeq,Expr2> : IFilter
{
    BndFilterView2(Store&,const Expr1&,const Expr2&);

 	bool					execute();
	void 					attach(INotifiable* pParent);
	void 					detach(INotifiable* pParent);
	Cost 					cost() const {	return linearLo; }


  private:
 	typedef enum { fail = 0, succeed = 1, delay = 2} AutomatonState;

 //	bool					xChanged();
 //	bool					yChanged();

  	AutomatonState 				automatonProcess(uint);
  	Store&						store;
    uint						n;
	BndArrayView<int,Expr1>		x;
	BndArrayView<int,Expr2>		y;
    Reversible<uint>			q,r,s,u;
   // Stack<uint>		toRevise;
	INotifiable*				pParent;
	Reversible<bool>			first;
	Detail::IdxBoundsDeltaStack<Expr1>	dx;
	Detail::IdxBoundsDeltaStack<Expr2>	dy;
};

template<class Expr1,class Expr2>
BndFilterView2<LessEqual,IntSeq,Expr1,IntSeq,Expr2>::BndFilterView2(Store& store,
    				   const Expr1& x,
					   const Expr2& y) :
			IFilter(store),
			store(store),
    		n(x.size()),x(store,x),y(store,y),
    		q(store,0),r(store,0),
    		s(store,0),u(store,0),
    		pParent(NULL),
    		first(store,true),
    		dx(store,x),dy(store,y)
{	assert(x.size()==y.size());	}


template<class Expr1,class Expr2>
void BndFilterView2<LessEqual,IntSeq,Expr1,IntSeq,Expr2>::attach(INotifiable* pParent)
{
	//assert(this->pParent==NULL);
	typedef BndFilterView2<LessEqual,IntSeq,Expr1,IntSeq,Expr2> Self;
	this->pParent = pParent;
	dx.attach(pParent);
	dy.attach(pParent);
	//x.attach(methodNotifier(this,&Self::xChanged));
	//y.attach(methodNotifier(this,&Self::yChanged));
}

template<class Expr1,class Expr2>
void BndFilterView2<LessEqual,IntSeq,Expr1,IntSeq,Expr2>::detach(INotifiable* pParent)
{
	//this->pParent = NULL;
	//toRevise.clear(); // because some schedulers may have copies
				  	  // of this filter on queue
	dx.detach(this);
	dy.detach(this);
}
/*
template<class Expr1,class Expr2>
bool
BndFilterView2<LessEqual,IntSeq,Expr1,IntSeq,Expr2>::xChanged()
{
	toRevise.push(x.lastIdx);
	assert(pParent != NULL);
	return pParent->notify();
}

template<class Expr1,class Expr2>
bool
BndFilterView2<LessEqual,IntSeq,Expr1,IntSeq,Expr2>::yChanged()
{
	toRevise.push(y.lastIdx);
	return pParent->notify();
}*/

template<class Expr1,class Expr2>
typename BndFilterView2<LessEqual,IntSeq,Expr1,IntSeq,Expr2>::AutomatonState
BndFilterView2<LessEqual,IntSeq,Expr1,IntSeq,Expr2>::automatonProcess(uint i)
{
	if (i == q)
		goto state1;
	else
	if (i == r)
		goto state2;
	else
	if (u == 3u && (i == s || (i < s && x[i].max() != y[i].min())))
		goto state3;
	else
	if (u == 4u && (i == s || (i < s && x[i].min() != y[i].max())))
		goto state4;
	else
		return delay;

	state1:
	while (i < n && x[i].min() == y[i].max())
	{
		if (!x[i].updateMax(y[i].max()) || !y[i].updateMin(x[i].min()))
			return fail;
		q = i = i+1;
	}
	if (i == n || x[i].max() < y[i].min())
	{
		detach(pParent);
		return succeed;
	}
	if (!x[i].updateMax(y[i].max()) || !y[i].updateMin(x[i].min()))
		return fail;
	r = i = std::max(i+1,r.get());

	state2:
	while (i < n && x[i].max() == y[i].min() && 	// both instantiated
		   x[i].min() == y[i].max())
		r = i = i+1;
	if (i == n || x[i].max() < y[i].min())	// T3
	{
		detach(pParent);
		return (AutomatonState)store.post(x[q].getObj() <= y[q].getObj());
	}
	else
	if (x[i].min() > y[i].max())
	{
		detach(pParent);
		return (AutomatonState)store.post(x[q].getObj() < y[q].getObj());
	}
	else
	if (x[i].max() == y[i].min() && x[i].min() < y[i].max())
	{
		s = i = std::max(i+1,s.get());
		goto state3;
	}
	else
	if (x[i].min() == y[i].max() && x[i].max() > y[i].min())
	{
		s = i = std::max(i+1,s.get());
		goto state4;
	}
	u = 2;
	return delay;

	state3:
	while (i < n && x[i].max() == y[i].min())
		s = i = i+1;
	if (i == n || x[i].max() < y[i].min())
	{
		detach(pParent);
		return (AutomatonState)store.post(x[q].getObj() <= y[q].getObj());
	}
	u = 3;
	return delay;

	state4:
	while (i < n && x[i].min() == y[i].max())
		s = i = i+1;
	if (i < n && x[i].min() > y[i].max())
	{
		detach(pParent);
		return (AutomatonState)store.post(x[q].getObj() < y[q].getObj());
	}
	u = 4;
	return delay;
}

template<class Expr1,class Expr2>
bool BndFilterView2<LessEqual,IntSeq,Expr1,IntSeq,Expr2>::execute()
{
	if (first)
	{
		first = false;
		// test if is already inconsistent
		typename BndArrayView<int,Expr1>::Iterator it1 = x.begin();
		typename BndArrayView<int,Expr2>::Iterator it2 = y.begin();
		while (it1 != x.end())
		{
			if (it1->min() < it2->max())
				break;
			else
			if (it1->min() > it2->max())
				return false;
			++it1;
			++it2;
		}
		if (x[0].min()==x[0].max() or y[0].min()==y[0].max())
			automatonProcess(0);
	}

	AutomatonState s;
	while (!dx.empty())
	{
		s = automatonProcess(dx.top());
		if (s == fail)
			return false;
		else
		if (s == succeed)
			return true;
		dx.pop();
	}
	while (!dy.empty())
	{
		s = automatonProcess(dy.top());
		if (s == fail)
			return false;
		else
		if (s == succeed)
			return true;
		dy.pop();
	}
	return true;
}

template<class Expr1,class Expr2>
struct PostBndFilter2<LessEqual,IntSeq,Expr1,IntSeq,Expr2>
{
	static bool post(Store& store,const Expr1& x,const Expr2& y)
	{
		BndArrayView<int,Expr1>	v1(store,x);
		BndArrayView<int,Expr2>	v2(store,y);
		assert(v1.size()==v2.size());

		if (v1.size()==1)
			return postBndFilter(store,v1[0].getObj() <= v2[0].getObj());

		return store.post(new (store) BndFilterView2<LessEqual,IntSeq,Expr1,IntSeq,Expr2>(store,x,y));
	}
};


} // CP
} // Casper

#endif /*_H_CASPER_CP_INT_LEX*/
