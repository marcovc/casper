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

#ifndef _H_CASPER_INT_LEX
#define _H_CASPER_INT_LEX

#include <casper/kernel/view/arrayview.h>
#include <casper/kernel/filter/bacfilter.h>
#include <casper/kernel/container/rstack.h>
#include <casper/kernel/delta.h>

namespace casper {

/**
 *  Filter for the lexicographic constraint [\ref carlsson02].
 *
 *  \ingroup IntFilters
 */
template<class View1,class View2>
struct BndFilterView2<LessEqual,IntSeq,View1,IntSeq,View2> : public IFilter
{
    BndFilterView2( ICPSolver&,const View1&,const View2&);

 	Bool					execute();
	Bool					entailed() const {	return false;	}	// FIXME
	Void 					attach(INotifiable* pParent);
	void 					detach(INotifiable* pParent);
	Cost 					cost() const {	return linearLo; }


  private:
 	typedef enum { fail = 0, succeed = 1, delay = 2} AutomatonState;

 //	Bool					xChanged();
 //	Bool					yChanged();

  	AutomatonState 				automatonProcess(UInt);
    UInt						n;
	BndArrayView<Int,View1>		x;
	BndArrayView<Int,View2>		y;
    Reversible<UInt>			q,r,s,u;
   // detail::RStack<UInt>		toRevise;
	INotifiable*				pParent;
	Reversible<Bool>			first;
	detail::IdxBoundsDeltaStack<View1>	dx;
	detail::IdxBoundsDeltaStack<View2>	dy;
};

/*
template<class View1,class View2>
Filter lex( const View1& x, const View2& y)
{
	ICPSolver& s(getCPSolver(x,y));
	return new (s.heap())
		BndFilterView2<LessEqual,Int,View1,Int,View2>(s,x,y);
}*/


template<class View1,class View2>
BndFilterView2<LessEqual,IntSeq,View1,IntSeq,View2>::BndFilterView2( ICPSolver& solver,
    				   const View1& x,
					   const View2& y) :
    		IFilter(solver),n(x.size()),x(solver,x),y(solver,y),
    		q(solver,0),r(solver,0),s(solver,0),u(solver,0),
    		pParent(NULL),
    		first(solver,true),
    		dx(x),dy(y)
{	assert(x.size()==y.size());	}


template<class View1,class View2>
Void BndFilterView2<LessEqual,IntSeq,View1,IntSeq,View2>::attach(INotifiable* pParent)
{
	//assert(this->pParent==NULL);
	typedef BndFilterView2<LessEqual,IntSeq,View1,IntSeq,View2> Self;
	this->pParent = pParent;
	dx.attach(pParent);
	dy.attach(pParent);
	//x.attach(methodNotifier(this,&Self::xChanged));
	//y.attach(methodNotifier(this,&Self::yChanged));
}

template<class View1,class View2>
Void BndFilterView2<LessEqual,IntSeq,View1,IntSeq,View2>::detach(INotifiable* pParent)
{
	//this->pParent = NULL;
	//toRevise.clear(); // because some schedulers may have copies
				  	  // of this filter on queue
	dx.detach(this);
	dy.detach(this);
}
/*
template<class View1,class View2>
Bool
BndFilterView2<LessEqual,IntSeq,View1,IntSeq,View2>::xChanged()
{
	toRevise.push(x.lastIdx);
	assert(pParent != NULL);
	return pParent->notify();
}

template<class View1,class View2>
Bool
BndFilterView2<LessEqual,IntSeq,View1,IntSeq,View2>::yChanged()
{
	toRevise.push(y.lastIdx);
	return pParent->notify();
}*/

template<class View1,class View2>
typename BndFilterView2<LessEqual,IntSeq,View1,IntSeq,View2>::AutomatonState
BndFilterView2<LessEqual,IntSeq,View1,IntSeq,View2>::automatonProcess(UInt i)
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
		return (AutomatonState)this->solver().post(x[q].getView() <= y[q].getView());
	}
	else
	if (x[i].min() > y[i].max())
	{
		detach(pParent);
		return (AutomatonState)this->solver().post(x[q].getView() < y[q].getView());
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
		return (AutomatonState)this->solver().post(x[q].getView() <= y[q].getView());
	}
	u = 3;
	return delay;

	state4:
	while (i < n && x[i].min() == y[i].max())
		s = i = i+1;
	if (i < n && x[i].min() > y[i].max())
	{
		detach(pParent);
		return (AutomatonState)this->solver().post(x[q].getView() < y[q].getView());
	}
	u = 4;
	return delay;
}

template<class View1,class View2>
Bool BndFilterView2<LessEqual,IntSeq,View1,IntSeq,View2>::execute()
{
	if (first)
	{
		first = false;
		// test if is already inconsistent
		typename BndArrayView<Int,View1>::Iterator it1 = x.begin();
		typename BndArrayView<Int,View2>::Iterator it2 = y.begin();
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

template<class View1,class View2>
struct BndFilterFact2<LessEqual,IntSeq,View1,IntSeq,View2>
{
	static Filter create(ICPSolver& solver,const View1& x,const View2& y)
	{
		BndArrayView<Int,View1>	v1(solver,x);
		BndArrayView<Int,View2>	v2(solver,y);
		assert(v1.size()==v2.size());

		if (v1.size()==1)
			return Bnd(v1[0].getView() <= v2[0].getView());

		return new (solver.heap())
			BndFilterView2<LessEqual,IntSeq,View1,IntSeq,View2>(solver,x,y);
	}
};


};

#endif /*_H_CASPER_INT_LEX*/
