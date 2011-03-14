 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

/**
 *	\file
 *
 *  This file was adapted from an implementation of [\ref ortiz03] available in
 *  http://ai.uwaterloo.ca/~vanbeek/software/.
 */

/*
	ORIGINAL HEADER:

 	File: alldiff.hpp

 	Implementation of Mehlhorn and Sven Thiel's algorithm for Bound-Consistency
 	of the all-diff constraint.

 	By: Claude-Guy Quimper
*/

#ifndef _H_CASPER_INT_DISTINCT_BND
#define _H_CASPER_INT_DISTINCT_BND

#include <casper/kernel/filter/common.h>
#include <casper/kernel/container/rstack.h>
#include <casper/kernel/view/arrayview.h>
#include <casper/int/distinct/common.h>
#include <casper/int/distinct/val.h>
#include <casper/int/variable.h>

namespace casper {


/**
 *  Filter that enforces bounds consistency
 *  on the distinct constraint over a sequence of
 *  integer variables [\ref ortiz03].
 *
 *  \ingroup IntFilters
 *  \todo Fix attach, as soon as DomView is ready.
 *  \todo Use solver's own heap.
 */
template<class View>
struct BndFilterView1<Distinct,IntSeq,View> : IFilter
{
  public:
	BndFilterView1( ICPSolver&, const View& v);
    ~BndFilterView1() {}
	Bool	execute();
	Bool	entailed() const {	return false;	}	// FIXME
	Void 	attach(INotifiable* pParent);
	Void 	detach(INotifiable* pParent);
	Cost 	cost() const {	return linearHi; }

  private:
  	typedef BndFilterView1<Distinct,IntSeq,View>	Self;
  	enum { INCONSISTENT, CHANGES, NO_CHANGES };

	struct Interval {
  		Int min, max;			// start, end of Interval
  		Int minrank, maxrank; 	// rank of min & max in bounds[] of an adcsp
	};

	struct VarDemon : INotifiable
	{
		BndFilterView1& rOwner;
		const UInt varIdx;
		VarDemon(BndFilterView1* pOwner, UInt varIdx) :
			rOwner(*pOwner),varIdx(varIdx)
		{ rOwner.vars[varIdx].attach(this); }
		bool notify()
		{
			//if (rOwner.solver().stats().fails() > rOwner.fails)
			if (rOwner.btSentinel.hasBacktracked())
				return rOwner.pParent->notify();
			rOwner.btSentinel.update();

			 // incremental
			Int l,u;
			Bool run = false;
			rOwner.vars[varIdx].range(l,u);
			if (l > rOwner.iv[varIdx].min)
			{
				rOwner.iv[varIdx].min = l;
				run = true;
			}
			if (u < rOwner.iv[varIdx].max)
			{
				rOwner.iv[varIdx].max = u;
				run = true;
			}
			return !run or rOwner.pParent->notify();
		}
		// tmp
		INotifiable* getFirst() { return rOwner.pParent; }

	};

	Void sortmin( Interval *v[], Int n );
	Void sortmax( Interval *v[], Int n );
	Void pathset(Int *t, Int start, Int end, Int to);
	Int pathmin(Int *t, Int i);
	Int pathmax(Int *t, Int i);

    BndArrayView<Int,View> vars;
	BacktrackSentinel 	btSentinel;
    Int n;
    Int *t;		// tree links
    Int *d;		// diffs between critical capacities
    Int *h;		// hall Interval links
    Interval *iv;
    Interval **minsorted;
    Interval **maxsorted;
    Int *bounds;  // bounds[1..nb] hold set of min & max in the niv intervals
                  // while bounds[0] and bounds[nb+1] allow sentinels
    Int nb;
    Void sortit();
    Int filterlower();
    Int filterupper();
    INotifiable* pParent;
};

template<class View>
BndFilterView1<Distinct,IntSeq,View>::BndFilterView1(ICPSolver& s,const View& v)
  							: IFilter(s), vars(s,v), btSentinel(s)/*fails(0)*/, pParent(NULL)
{
	Int i;

	n = vars.size();

	iv        = new (s.heap()) Interval[n];
	minsorted = new (s.heap()) Interval*[n];
	maxsorted = new (s.heap()) Interval*[n];
	bounds    = new (s.heap()) Int[2*n+2];

	for( i = 0; i < n; i++ )
    	minsorted[i] = maxsorted[i] = &iv[i];


	t = new (s.heap()) Int[2*n+2];
	d = new (s.heap()) Int[2*n+2];
	h = new (s.heap()) Int[2*n+2];

	//s.post(distinct(v),Val);
}

template<class View>
Void BndFilterView1<Distinct,IntSeq,View>::attach(INotifiable* pParent)
{
//	assert(this->pParent==NULL);
	this->pParent= pParent;
	for (UInt i = 0; i < vars.size(); i++)
		new (vars.solver().heap()) VarDemon(this,i);

	// by doing this it may achieve more than bounds consistency
	//vars.solver().post(distinct(vars),Val);

	// initialize
    for (UInt i = 0; i < vars.size(); i++)
    {
    	iv[i].min = vars[i].min();
      	iv[i].max = vars[i].max();
    }
}

template<class View>
Void BndFilterView1<Distinct,IntSeq,View>::detach(INotifiable* pParent)
{
	//this->pParent= NULL;
//	TODO: kill demons
//	for (UInt i = 0; i < vars.size(); i++)
//		new (vars.solver().heap()) VarDemon(this,i);
}

template<class View>
Void BndFilterView1<Distinct,IntSeq,View>::sortmax( Interval *v[], Int n )
{
	Int i, current;
	Bool sorted;
	Interval *t;

	current = 0;
	sorted = false;
	while( !sorted )
	{
		sorted = true;
		for( i = n-1; i > current; i-- )
		{
      		if( v[i-1]->max > v[i]->max )
      		{
        		t = v[i];
        		v[i] = v[i-1];
        		v[i-1] = t;
        		sorted = false;
      		}
    	}
    	current++;
  	}
}

template<class View>
Void BndFilterView1<Distinct,IntSeq,View>::sortmin( Interval *v[], Int n )
{
	Int i, current;
	Bool sorted;
	typename Self::Interval *t;

	current = n-1;
	sorted = false;
	while( !sorted )
	{
		sorted = true;
    	for( i = 0; i < current; i++ )
    	{
      		if( v[i]->min > v[i+1]->min )
      		{
        		t = v[i];
        		v[i] = v[i+1];
        		v[i+1] = t;
        		sorted = false;
      		}
    	}
    	current--;
  	}
}

template<class View>
Void BndFilterView1<Distinct,IntSeq,View>::sortit()
{
	Int i,j,nb,min,max,last;

	sortmin(minsorted, n);
	sortmax(maxsorted, n);

	min = minsorted[0]->min;
	max = maxsorted[0]->max + 1;
	bounds[0] = last = min-2;

	for (i=j=nb=0;;)	 // merge minsorted[] and maxsorted[] into bounds[]
    	if (i<n && min<=max)
    	{	// make sure minsorted exhausted first
      		if (min != last)
        		bounds[++nb] = last = min;
      		minsorted[i]->minrank = nb;
      		if (++i < n)
        		min = minsorted[i]->min;
    	}
    	else
    	{
      		if (max != last)
         		bounds[++nb] = last = max;
      		maxsorted[j]->maxrank = nb;
      		if (++j == n)
      			break;
      		max = maxsorted[j]->max + 1;
    	}
	Self::nb = nb;
	bounds[nb+1] = bounds[nb] + 2;
}

template<class View>
Void BndFilterView1<Distinct,IntSeq,View>::pathset(Int *t, Int start, Int end, Int to)
{
	Int k, l;
  	for (l=start; (k=l) != end; t[k]=to)
    	l = t[k];
}

template<class View>
Int BndFilterView1<Distinct,IntSeq,View>::pathmin(Int *t, Int i)
{
	for (; t[i] < i; i=t[i]) {
    	;
  	}
  	return i;
}

template<class View>
Int BndFilterView1<Distinct,IntSeq,View>::pathmax(Int *t, Int i)
{
	for (; t[i] > i; i=t[i]) {
    	;
  	}
  	return i;
}


template<class View>
Int BndFilterView1<Distinct,IntSeq,View>::filterlower()
{
	Int i,j,w,x,y,z;
  	Int changes = 0;

	for (i=1; i<=nb+1; i++)
    	d[i] = bounds[i] - bounds[t[i]=h[i]=i-1];
  	for (i=0; i<n; i++)	// visit intervals in increasing max order
  	{
    	x = maxsorted[i]->minrank; y = maxsorted[i]->maxrank;
    	j = t[z = pathmax(t, x+1)];
    	if (--d[z] == 0)
      		t[z = pathmax(t, t[z]=z+1)] = j;
    	pathset(t, x+1, z, z); // path compression
    	if (d[z] < bounds[z]-bounds[y])
    		return INCONSISTENT; // no solution
    	if (h[x] > x)
    	{
      		maxsorted[i]->min = bounds[w = pathmax(h, h[x])];
      		pathset(h, x, w, w); // path compression
      		changes = 1;
    	}
    	if (d[z] == bounds[z]-bounds[y])
    	{
      		pathset(h, h[y], j-1, y); // mark hall Interval
      		h[y] = j-1; //("hall Interval [%d,%d)\n",bounds[j],bounds[y]);
    	}
  	}
  	if( changes )
    	return CHANGES;
  	else
    	return NO_CHANGES;
}


template<class View>
Int BndFilterView1<Distinct,IntSeq,View>::filterupper()
{
	Int i,j,w,x,y,z;
  	Int changes = 0;

  	for (i=0; i<=nb; i++)
    	d[i] = bounds[t[i]=h[i]=i+1] - bounds[i];
  	for (i=n; --i>=0; )	// visit intervals in decreasing min order
  	{
    	x = minsorted[i]->maxrank; y = minsorted[i]->minrank;
    	j = t[z = pathmin(t, x-1)];
    	if (--d[z] == 0)
      		t[z = pathmin(t, t[z]=z-1)] = j;
    	pathset(t, x-1, z, z);
    	if (d[z] < bounds[y]-bounds[z])
    		return INCONSISTENT; // no solution
    	if (h[x] < x)
    	{
      		minsorted[i]->max = bounds[w = pathmin(h, h[x])] - 1;
      		pathset(h, x, w, w);
      		changes = 1;
    	}
    	if (d[z] == bounds[y]-bounds[z])
    	{
      		pathset(h, h[y], j+1, y);
      		h[y] = j+1;
    	}
  	}
  	if( changes )
    	return CHANGES;
  	else
    	return NO_CHANGES;
}

template<class View>
Bool	BndFilterView1<Distinct,IntSeq,View>::execute()
{
/*	std::cout << vars.solver().stats().curLevel << " : execute\n";
	for (UInt i = 0; i < vars.size(); i++)
		std::cout << "[" << vars[i].min() << ".." << vars[i].max() << "] ";
	std::cout << std::endl;
*/
	Int  status_lower, status_upper;

//	if( vars.solver().fails() > fails )
	if( btSentinel.hasBacktracked())
  	{
    	// solver has backtracked: reset
    	for (UInt i = 0; i < vars.size(); i++)
    	{
    		//vars[i].range(iv[i].min,iv[i].max);
    		iv[i].min = vars[i].min();
      		iv[i].max = vars[i].max();
    	}
  	}
  	//fails = vars.solver().fails();
	btSentinel.update();

	status_lower = status_upper = CHANGES;
  	sortit();

	status_lower = filterlower();
  	if( status_lower == INCONSISTENT )
  		return false;

  	status_upper = filterupper();
  	if( status_upper == INCONSISTENT )
    	return false;
/*
  	if( status_lower == CHANGES || status_upper == CHANGES )
  		for (UInt i = 0; i < vars.size(); i++)
  			if (!vars[i].updateRange(iv[i].min, iv[i].max))
  				return false;
  				*/

  	if( status_lower == CHANGES)
  	{
  		if (status_upper == CHANGES )
  		{
  			for (UInt i = 0; i < vars.size(); i++)
  				if (!vars[i].updateRange(iv[i].min, iv[i].max))
  					return false;
  		}
  		else
  		{
			for (UInt i = 0; i < vars.size(); i++)
				if (!vars[i].updateMin(iv[i].min))
					return false;
  		}
  	}
  	else
  	if (status_upper == CHANGES )
  	{
		for (UInt i = 0; i < vars.size(); i++)
			if (!vars[i].updateMax(iv[i].max))
				return false;
  	}
  	return true;
}

};

#endif
