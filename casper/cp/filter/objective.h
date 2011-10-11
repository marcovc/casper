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
 

#ifndef CASPER_CP_FILTER_OBJECTIVE_H_
#define CASPER_CP_FILTER_OBJECTIVE_H_

#include <casper/cp/filter/common.h>

namespace Casper {
namespace CP {


template<class T>
struct Objective
{
	struct NotifierWrapper : INotifiable
	{
		NotifierWrapper(INotifiable* p) : p(p) {}
		bool notify() { return p->notify(); }
		INotifiable* p;
	};

	struct ObjFilter : IFilter
	{
		ObjFilter(Objective& rOwner) : IFilter(rOwner.store),rOwner(rOwner) {}
		bool execute()
		{
			//rOwner.store.getEnv().getState().notifyOnNextCP(
			//		new (rOwner.store.getEnv().getState().getCPHeap()) NotifierWrapper(p));
			//rOwner.store.getEnv().getState().notifyOnNextCP(p);
			//std::cout << "execute\n";
			return rOwner.b.updateRange(rOwner.curMin,rOwner.curMax);
		}
		void attach(INotifiable* p)
		{	this->p = p; rOwner.b.attach(p); }
		void detach(INotifiable* p)
		{	rOwner.b.detach(p); }

		INotifiable* p;
		Objective& rOwner;
	};

	Objective(Store& store) :
		store(store),b(store,T())
	{
	}

	template<class T1>
	void set(const T1& t)
	{
		BndExpr<T> b(store,t);
		curMin = b.min();
		curMax = b.max();
		this->b = b;
		pFilter = new (store) ObjFilter(*this);
		store.post(pFilter);
	}

	bool updateMin(const T& newMin)
	{
		assert(newMin >= curMin);
		curMin = newMin;
		return curMin <= curMax;// and b.updateRange(curMin,curMax);
	}

	bool updateMax(const T& newMax)
	{
		assert(newMax <= curMax);
		curMax = newMax;
		return curMin <= curMax;// and b.updateRange(curMin,curMax);
	}

	bool increase(const T& delta)
	{
		assert(delta>=0);
		curMin = std::max(curMin,b.min())+delta;
		return curMin <= curMax;// and b.updateRange(curMin,curMax);
	}

	bool decrease(const T& delta)
	{
		assert(delta>=0);
		curMax = std::min(curMax,b.max())-delta;
		return curMin <= curMax;// and b.updateRange(curMin,curMax);
	}

	const T& getMin() const {	return std::max(curMin,b.min());	}
	const T& getMax() const {	return std::min(curMax,b.max());	}

	Store&		store;
	BndExpr<T>	b;
	T 			curMin;
	T 			curMax;
	ObjFilter* 	pFilter;
};

}
}

#endif /* CASPER_CP_FILTER_OBJECTIVE_H_ */
