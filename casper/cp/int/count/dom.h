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
 

#ifndef CASPER_CP_INT_COUNT_DOM_H_
#define CASPER_CP_INT_COUNT_DOM_H_

namespace Casper {
namespace CP {

template<class NbOcc,class Array>
struct DomFilterView3<CountEqual,int,NbOcc,Seq<int>,Array,int, int> : IFilter
{
	DomFilterView3(Store& store,const NbOcc& nocc, const Array& a, int val) :
		IFilter(store),store(store),nocc(store,nocc),a(store,a),
		val(val),lb(store,0),ub(store,0) {}

	// attach on ground
	struct LBDemon : INotifiable
	{
		LBDemon(DomFilterView3& rOwner, uint idx) : rOwner(rOwner),idx(idx)
		{	rOwner.a[idx]->attachOnGround(this);	}

		bool notify()
		{
			bool mustNotify = false;
			assert(rOwner.a[idx]->ground());
			if (rOwner.a[idx]->value() == rOwner.val)
			{
				rOwner.lb = rOwner.lb+1;
				mustNotify = true;
			}
			if (rOwner.lb > rOwner.nocc.max())
				return false;
			if (mustNotify or rOwner.lb == rOwner.nocc.max())
				return rOwner.pOwner->notify();
			return true;
		}

		DomFilterView3& rOwner;
		const uint idx;
	};

	// attach on domain
	struct UBDemon : INotifiable
	{
		UBDemon(DomFilterView3& rOwner, uint idx) : rOwner(rOwner),idx(idx)
		{	rOwner.a[idx]->attachOnDomain(this);	}

		bool notify()
		{
			bool mustNotify = false;
			if (*rOwner.a[idx]->delta().first() <= rOwner.val and
				*rOwner.a[idx]->delta().last() >= rOwner.val)
			{
				rOwner.a[idx]->detachOnDomain(this);
				rOwner.ub = rOwner.ub-1;
				mustNotify = true;
			}
			if (rOwner.ub < rOwner.nocc.min())
				return false;
			if (mustNotify or rOwner.ub == rOwner.nocc.min())
				return rOwner.pOwner->notify();
			return true;
		}

		DomFilterView3& rOwner;
		const uint idx;
	};

	bool execute()
	{
		if (!nocc.updateRange(lb,ub))
			return false;

		if (lb == nocc.max())
		{
			// remove val from non ground domains
			int count = ub;
			for (uint i = 0; i < a.size(); ++i)
			{
				if (!a[i]->ground())
				{
					count -= a[i]->find(val)!=a[i]->end();
					if (!a[i]->erase(val))
						return false;
				}
			}
			assert(count == lb);
		}
		else
		if (ub == nocc.min())
		{
			// instantiate non ground domains containing val to val
			int count = lb;
			for (uint i = 0; i < a.size(); ++i)
				if (!a[i]->ground())
				{
					auto it = a[i]->find(val);
					if (it != a[i]->end())
					{
						if (!(*a[i]=it))
							return false;
						++count;
					}
				}
			assert(count = ub);
		}
		return true;
	}

	Cost cost() const
	{	return IFilter::linearLo;	}

	void attach(INotifiable* n)
	{
		pOwner = n;
		for (uint i = 0; i < a.size(); ++i)
			if (a[i]->ground() and a[i]->value()==val)
			{
				lb = lb+1;
				ub = ub+1;
			}
			else
			if (a[i]->find(val)!=a[i]->end())
				ub = ub+1;

		for (uint i = 0; i < a.size(); ++i)
		{
			new (store) LBDemon(*this,i);
			new (store) UBDemon(*this,i);
		}
		nocc.attach(n);
	}

	void detach(INotifiable* n)
	{}

	Store& store;
	BndView<int,NbOcc>	nocc;
	DomArrayView<int,Array> a;
	const int val;
	Reversible<int> lb;
	Reversible<int> ub;
	INotifiable* pOwner;
};

template<class NbOcc,class Vars>
struct PostDomFilter3<CountEqual,int,NbOcc,Seq<int>,Vars,int,int>
{
	static bool post(Store& store, const NbOcc& nocc, const Vars& vars, int val)
	{	return store.post(new (store)
			DomFilterView3<CountEqual,int,NbOcc,Seq<int>,Vars,int,int>(store,nocc,vars,val));	}
};

}
}

#endif /* CASPER_CP_INT_COUNT_DOM_H_ */
