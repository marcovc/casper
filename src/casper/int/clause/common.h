/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2010-2010 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_INT_CLAUSE_COMMON_H_
#define CASPER_INT_CLAUSE_COMMON_H_

namespace casper {
namespace detail {

template<class V>
struct Clause : IFilter
{
	template<class T>
	Clause(ICPSolver& solver, const T& v);

	struct Watch
	{
		Clause& owner;
		UInt idx;
		Watch(Clause& owner, UInt idx) : owner(owner),idx(idx)
		{	owner.doms[idx]->attachOnGround(this);	}
		~Watch()
		{	owner.doms[idx]->detachOnGround(this);	}
		Bool notify()
		{
			assert(owner.doms[idx]->ground());
			if (owner.doms[idx]->value()==true)
			{
				owner.detach(owner.pParent);
				return true;
			}
			owner.doms[idx]->detachOnGround(this);
			for (UInt i = 0; i < owner.doms.size(); ++i)
				if (!owner.doms[i]->ground() and
					i != owner.pWatch1->idx and i != owner.pWatch2->idx)
				{
					// not 100% sure...
					assert(owner.doms[i]->value()==false);
					idx = i;
					owner.doms[i]->attachOnGround(this);
					return true;
				}
			owner.detach(owner.pParent);
			owner.mustBeTrue = idx;
			return owner.pParent->notify();
		}
	};

	Bool 	execute();
	Void 	attach(INotifiable* f)
	{	pParent = f; }
	void 	detach(INotifiable* f)
	{
		if (pWatch1 != NULL)
			pWatch1->~Watch();
		if (pWatch2 != NULL)
			pWatch2->~Watch();
	}
	Cost 	cost() const {	return constantLo; }

	V									doms;
	Int									mustBeTrue;
	INotifiable*						pParent;
	Watch*								pWatch1;
	Watch*								pWatch2;
};

template<class V>
template<class T>
Clause<V>::Clause(ICPSolver& solver, const T& v) :
	doms(solver,v),mustBeTrue(-1),pParent(NULL),pWatch1(NULL),pWatch2(NULL)
{}

template<class V>
Bool Clause<V>::execute()
{
	if (pWatch1==NULL) // first time: set watches
	{
		UInt i = 0;
		for ( ; i < doms.size(); ++i)
			if (doms[i]->ground())
			{
				if (doms[i]->value())
					goto entailed;
			}
			else
			{
				pWatch1 = new (solver().heap()) Watch(*this,i);
				break;
			}
		for ( ; i < doms.size(); ++i)
			if (doms[i]->ground())
			{
				if (doms[i]->value())
					goto entailed;
			}
			else
			{
				pWatch2 = new (solver().heap()) Watch(*this,i);
				break;
			}
		if (pWatch1 == NULL)
			return false;
		if (pWatch2 == NULL)
		{
			detach(pParent);
			return doms[pWatch1->idx]->updateMin(true);
		}
	}
	else
	{
		assert(mustBeTrue>=0);
		detach(pParent);
		return doms[mustBeTrue]->updateMin(true);
	}

	return true;

	entailed:
	detach(pParent);
	return true;
}

}
}

#endif /* CASPER_INT_CLAUSE_COMMON_H_ */
