 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_CP_INT_CUMULATIVE_H_
#define CASPER_CP_INT_CUMULATIVE_H_

#include <algorithm>
#include <vector>
#include <casper/kernel/common.h>
#include <casper/util/container/vector.h>
#include <casper/util/container/slist.h>
#include <casper/cp/int/cumulative/common.h>

/**
 *  Implements the extended edge finding algorithm from :
 *  "Mercier L., Hentenryck P., Edge finding for cumulative scheduling, ?"
 *  plus the algorithm in the cumulatives filter.
 *  \cost Runs in O(kn^2), where n is the number of tasks and k the number
 *  of different heights (capacity requirements).
 *
 *  \note This filter should work for any bndexpr type with integer capacity,
 *  although this was not tested.
 *  \note Since there are no specific task variables in casper, this filter
 *  also imposes start[i]+duration[i]=end[i] for every task i.
 *  \note This filter needs extensive testing and probably some optimization.
 *  \warning This filter needs a staged filter scheduler. It has undefined
 *  behaviour for non-staged schedulers.
 **/
namespace Casper {
namespace CP {


template<class X,class Y,class Start,class Duration,class End, class Height,class Capacity>
struct BndFilterView5<Cumulative,Seq<X>,Start,Seq<X>,Duration,
								 Seq<X>,End,Seq<Y>,Height,
								 Y,Capacity> : IFilter
{
	BndFilterView5(Store& store,const Start& start,
							 const Duration& duration,
							 const End& end,
							 const Height& height,
							 const Capacity& capacity);

	bool execute();
	bool entailed() const {	assert(0); return false; }
	Cost cost() const	{	return quadraticHi; }

	void attach(INotifiable* f)
	{	start.attach(f); duration.attach(f); end.attach(f); height.attach(f); }

	void detach(INotifiable* f)
	{	start.detach(f); duration.detach(f); end.detach(f); height.detach(f); }

	protected:
	static const X inf;

	struct Task
	{
		Task() {}
		Task(uint idx,X start,X duration,X end,Y height) :
			idx(idx),
			start(start),duration(duration),end(end),height(height) {}
		uint	idx;
		X	start;
		X	duration;
		X	end;
		Y	height;
		X	energy() const { return duration*height; }
	};

	bool efeasible(const std::vector<Task*>& xx,
				   const std::vector<Task*>& yy) const;
	void initTasks();
	bool refreshTasks();
	void sortTasks();
	uint findSc(Util::StdSList<Y>& sc,Util::StdVector<uint>& h2i);
	void calcR(const std::vector<Task*>& xx,const std::vector<Task*>& yy,
			   const Util::StdSList<Y>& sc,const Util::StdVector<uint>& h2i,
			   Util::StdArray<X,2>& R);
	bool calcEF(const std::vector<Task*>& xx,const std::vector<Task*>& yy,
			const Util::StdVector<uint>& h2i,const Util::StdArray<X,2>& R,
			bool lb);
	bool calcEEF(const std::vector<Task*>& xx,const std::vector<Task*>& yy,
				uint k,const Util::StdSList<Y>& sc,const Util::StdVector<uint>& h2i,
				const Util::StdArray<X,2>& R,bool lb);
	BndArrayView<X,Start>		start;
	BndArrayView<X,Duration>	duration;
	BndArrayView<X,End>			end;
	BndArrayView<Y,Height>		height;

	const Y						capacity;

	std::vector<Task>			tasks;
	std::vector<Task*>			sStart;	// indexes tasks sorted by start time
	std::vector<Task*>			sEnd;	// indexes tasks sorted by end time

	const uint				n;

	struct LessStartPred
	{	bool operator()(const Task* b1, const Task* b2) const
	    {	return b1->start < b2->start; }
	};
	struct LessEndPred
	{	bool operator()(const Task* b1, const Task* b2) const
	    {	return b1->end < b2->end; }
	};

	Detail::scheduling::Sweep<VarArray<int>,Start,Duration,End,Height> sweep;
};

template<class X,class Y,class Start,class Duration,class End,class Height,class Capacity>
const X BndFilterView5<Cumulative,Seq<X>,Start,Seq<X>,Duration,
			   Seq<X>,End,Seq<Y>,Height,Y,Capacity>::inf = limits<X>::max()-1;

template<class X,class Y,class Start,class Duration,class End,class Height,class Capacity>
BndFilterView5<Cumulative,Seq<X>,Start,Seq<X>,Duration,
			   Seq<X>,End,Seq<Y>,Height,Y,Capacity>::
BndFilterView5(Store& store,const Start& start,
						 const Duration& duration,
						 const End& end,
						 const Height& height,
						 const Capacity& capacity) :
	start(store,start),duration(store,duration),
	end(store,end),height(store,height),
	capacity(ValView<Y,Capacity>(store,capacity).value()),
	tasks(start.size()),
	sStart(start.size(),NULL),
	sEnd(start.size(),NULL),
	n(start.size()),
	sweep(store,VarArray<int>(store,n,0,0),start,duration,end,height)
	{	assert(start.size() == duration.size() and
			   start.size() == end.size() and
			   start.size() == height.size() and
			   start.size() >= 2);
	// tmp
		for (uint i = 0; i < n; i++)
			store.post(this->start[i].getObj()+
						this->duration[i].getObj()==
						this->end[i].getObj());
	}


template<class X,class Y,class Start,class Duration,class End,class Height,class Capacity>
void BndFilterView5<Cumulative,Seq<X>,Start,Seq<X>,Duration,
			   Seq<X>,End,Seq<Y>,Height,Y,Capacity>::initTasks()
{
	for (uint i = 0; i < n; i++)
	{
		tasks[i] = Task(i,start[i].min(),duration[i].min(),end[i].max(),
						height[i].min());
		sStart[i] = &tasks[i];
		sEnd[i] = &tasks[i];
	}
}

// returns true if something changed, false otherwise
template<class X,class Y,class Start,class Duration,class End,class Height,class Capacity>
bool BndFilterView5<Cumulative,Seq<X>,Start,Seq<X>,Duration,
			   Seq<X>,End,Seq<Y>,Height,Y,Capacity>::refreshTasks()
{
	bool modified = false;
	for (uint i = 0; i < n; i++)
	{
		X startbnd = start[i].min();
		X endbnd = end[i].max();
		X durationbnd = duration[i].min();
		Y heightbnd = height[i].min();
		if (startbnd != tasks[i].start)
		{
			tasks[i].start = startbnd;
			modified = true;
		}
		if (endbnd != tasks[i].end)
		{
			tasks[i].end = endbnd;
			modified = true;
		}
		if (durationbnd != tasks[i].duration)
		{
			tasks[i].duration = durationbnd;
			modified = true;
		}
		if (heightbnd != tasks[i].height)
		{
			tasks[i].height = heightbnd;
			modified = true;
		}
	}
	return modified;
}


template<class X,class Y,class Start,class Duration,class End,class Height,class Capacity>
void BndFilterView5<Cumulative,Seq<X>,Start,Seq<X>,Duration,
			   Seq<X>,End,Seq<Y>,Height,Y,Capacity>::sortTasks()
{
	std::sort(sStart.begin(),sStart.end(),LessStartPred());
	std::sort(sEnd.begin(),sEnd.end(),LessEndPred());
}

// O(n^2)
template<class X,class Y,class Start,class Duration,class End,class Height,class Capacity>
bool BndFilterView5<Cumulative,Seq<X>,Start,Seq<X>,Duration,
			   Seq<X>,End,Seq<Y>,Height,Y,Capacity>::efeasible(
						const std::vector<Task*>& xx,
						const std::vector<Task*>& yy) const
{
	for (uint y = 0; y < n; ++y)
	{
		X D = yy[y]->end;
		X e = 0;
		for (int x = n-1; x >= 0; --x)
			if (xx[x]->end <= D)
			{
				e += xx[x]->energy();
				if (capacity*(D-xx[x]->start) < e)
					return false;
			}
	}
	/*for (uint y = 0; y < n; ++y)
	{
		X D = yy[y]->end;
		X e = 0;
		for (int x = n-1; x >= 0; --x)
			//if (xx[x]->end <= D)
			{
				X curE = xx[x]->energy();
				if (xx[x]->end > D)
					curE = std::min(curE,(D - start[xx[x]->idx].max())*
										  xx[x]->height);
				std::cout << xx[x]->idx << " " << yy[y]->idx << " " << curE << " " << e << std::endl;
				if (curE > 0)
				{
					e += curE;
					if (capacity*(D-xx[x]->start) < e)
					{
						std::cout << D << " " << capacity << std::endl;
						for (uint i = 0; i < n ; i++)
							std::cout << start[xx[i]->idx] << " ";
						std::cout << "\nfailing\n";
						return false;
					}
				}
			}
	}*/
	return true;
}

// pre: must run after refreshTasks()
template<class X,class Y,class Start,class Duration,class End,class Height,class Capacity>
uint BndFilterView5<Cumulative,Seq<X>,Start,Seq<X>,Duration,
			   Seq<X>,End,Seq<Y>,Height,Y,Capacity>::findSc(Util::StdSList<Y>& sc,
												   Util::StdVector<uint>& h2i)
{
	Util::StdVector<bool>	c(stdHeap,capacity+1,false);
	uint k = 0;
	for (uint i = 0; i < n; i++)
	{
		assert(tasks[i].height <= capacity);	// this should never happen if it's efeasible
		if (!c[tasks[i].height])
		{
			c[tasks[i].height] = true;
			sc.pushBack(tasks[i].height);
			h2i[tasks[i].height] = k;
			k++;
		}
	}
	return k;
}

template<class X,class Y,class Start,class Duration,class End,class Height,class Capacity>
void BndFilterView5<Cumulative,Seq<X>,Start,Seq<X>,Duration,
			   Seq<X>,End,Seq<Y>,Height,Y,Capacity>::calcR(
											const std::vector<Task*>& xx,
											const std::vector<Task*>& yy,
											const Util::StdSList<Y>& sc,
											const Util::StdVector<uint>& h2i,
											Util::StdArray<X,2>& R)
{
	Util::StdVector<X> E(n);
	for (typename Util::StdSList<Y>::Iterator it = sc.begin();
			it != sc.end(); ++it)
	{
		for (uint y = 0; y < n; ++y)
		{
			E[y] = 0;
			R[h2i[*it]][y] = -inf;
		}
		for (int x = n-1; x >= 0; --x)
		{
			X xenergy = xx[x]->energy();
			for (uint y = 0; y < n; ++y)
				if (xx[x]->end <= yy[y]->end)	// this is not what the paper says
				{
					E[y] += xenergy;
					X a = R[h2i[*it]][y];
					X b = (y>0)?R[h2i[*it]][y-1]:-inf;
					X rest = E[y] - (capacity-*it)*(yy[y]->end-xx[x]->start);
					X c;
					if (rest>0)
						c = xx[x]->start+std::ceil(rest/(double)*it);
					else
						c = -inf;
					R[h2i[*it]][y] = std::max(a,std::max(b,c));
				}
		}
	}
}

template<class X,class Y,class Start,class Duration,class End,class Height,class Capacity>
bool BndFilterView5<Cumulative,Seq<X>,Start,Seq<X>,Duration,
			   Seq<X>,End,Seq<Y>,Height,Y,Capacity>::calcEF(
											const std::vector<Task*>& xx,
											const std::vector<Task*>& yy,
											const Util::StdVector<uint>& h2i,
											const Util::StdArray<X,2>& R,
											bool lb)
{
	Util::StdVector<X> E(n);
	for (uint y = 0; y < n; ++y)
	{
		X EE = 0;
		for (int x = n-1; x >= 0; --x)
		{
			if (xx[x]->end <= yy[y]->end)
				EE += xx[x]->energy();
			E[x] = EE;
		}
		X CEF = -inf;
		for (uint i = 0; i < n; ++i)
		{
			CEF = std::max(CEF,E[i]-capacity*(yy[y]->end-xx[i]->start));
			if (CEF + xx[i]->energy() > 0 and xx[i]->end > yy[y]->end)
			{
				if (lb and !start[xx[i]->idx].updateMin(R[h2i[xx[i]->height]][y]))
					return false;
				else
				if (!lb and !end[xx[i]->idx].updateMax(-R[h2i[xx[i]->height]][y]))
					return false;
			}
		}
	}
	return true;
}

// FIXME: This is not working (it prunes solutions)
// \pre calcEF has run
template<class X,class Y,class Start,class Duration,class End,class Height,class Capacity>
bool BndFilterView5<Cumulative,Seq<X>,Start,Seq<X>,Duration,
			   Seq<X>,End,Seq<Y>,Height,Y,Capacity>::calcEEF(
												const std::vector<Task*>& xx,
												const std::vector<Task*>& yy,
												uint k,
												const Util::StdSList<Y>& sc,
												const Util::StdVector<uint>& h2i,
												const Util::StdArray<X,2>& R,
												bool lb)
{
	Util::StdVector<X> E(n);
	Util::StdArray<X,2> CEEF(k,n+1);
	for (uint y = 0; y < n-1; ++y)
	{
		X EE = 0;
		for (int x = n-1; x >= 0; --x)
		{
			if (xx[x]->end <= yy[y]->end)
				EE += xx[x]->energy();
			E[x] = EE;
		}
		for (uint x = 0; x < n-1; ++x)
			if (xx[x]->start == xx[x+1]->start)
				E[x+1] = E[x];
		for (uint c = 0; c < k; c++)
			CEEF[c][n] = -inf;
		for (int x = n-1; x >= 0; --x)
		{
			for (typename Util::StdSList<Y>::Iterator it = sc.begin();
				it != sc.end(); ++it)
				CEEF[h2i[*it]][x] = std::max(CEEF[h2i[*it]][x+1],
						(capacity-*it)*xx[x]->start + E[x]-capacity*yy[y]->end);
		}
		for (uint i = 0; i < n; ++i)
			if (CEEF[h2i[xx[i]->height]][i] +
				xx[i]->height*(xx[i]->start+xx[i]->duration) > 0 and
				xx[i]->end > yy[y]->end)
			{
				if (lb and !start[xx[i]->idx].updateMin(R[h2i[xx[i]->height]][y]))
					return false;
				else
				if (!lb and !end[xx[i]->idx].updateMax(-R[h2i[xx[i]->height]][y]))
					return false;
			}
	}
	return true;
}

template<class X,class Y,class Start,class Duration,class End,class Height,class Capacity>
bool BndFilterView5<Cumulative,Seq<X>,Start,Seq<X>,Duration,
			   Seq<X>,End,Seq<Y>,Height,Y,Capacity>::execute()
{
/*	std::cout << this << " cumulative::execute()\n";
	std::cout << "starts: " << start << std::endl
			  << "durations: " << duration << std::endl
			  << "ends: " << end << std::endl
			  << "heights: " << height << std::endl
			  << "capacity: " << capacity << std::endl;
*/
	if (!sweep.sweepUB(0,capacity))
		return false;

	if (sStart[0]==NULL)
	{
		initTasks();
		// the capacity is a trivial upper bound on each individual height
		for (uint i = 0; i < n; i++)
			if (!height[i].updateMax(capacity))
				return false;
	}
	else
	if (!refreshTasks())
		return true;

	sortTasks();

	if (!efeasible(sStart,sEnd))
		return false;

	Util::StdSList<Y> sc;
	Util::StdVector<uint>	h2i(capacity+1);	// requires that heights are integers
	uint k = findSc(sc,h2i);
	Util::StdArray<X,2> R(k,n,0);

	// update lower bounds
	calcR(sStart,sEnd,sc,h2i,R);

	if (!calcEF(sStart,sEnd,h2i,R,true))
		return false;

//	if (!calcEEF(sStart,sEnd,k,sc,h2i,R,true))
//		return false;

	// update upper bounds

	std::vector<Task*>	sInvStart(sStart.rbegin(),sStart.rend());
	std::vector<Task*>	sInvEnd(sEnd.rbegin(),sEnd.rend());

	for (uint i = 0; i < n; i++)
	{
		X aux = tasks[i].start;
		tasks[i].start = -tasks[i].end;
		tasks[i].end = -aux;
	}

	if (!efeasible(sInvEnd,sInvStart))
	{
		std::cout << "here\n";
		return false;
	}

	calcR(sInvEnd,sInvStart,sc,h2i,R);

	if (!calcEF(sInvEnd,sInvStart,h2i,R,false))
		return false;

/*	if (!calcEEF(sInvEnd,sInvStart,k,sc,h2i,R,false))
		return false;
*/
	for (uint i = 0; i < n; i++)
	{
		X aux = tasks[i].start;
		tasks[i].start = -tasks[i].end;
		tasks[i].end = -aux;
	}

	return true;
}

} // CP
} // Casper


#endif /*CASPER_INT_CUMULATIVE_H_*/
