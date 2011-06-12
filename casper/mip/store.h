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
 

#ifndef CASPER_MIP_STORE_H_
#define CASPER_MIP_STORE_H_

#include <casper/cp/store.h>
#include <casper/cp/rangedom.h>

#include <casper/lp/solver.h>
#include <casper/lp/linearexprview.h>
#include <casper/mip/traits.h>
#include <casper/mip/view.h>

namespace Casper {
namespace MIP {

struct Store
{
	Store(CP::Store& cp, LP::Solver& lp) : cp(cp),lp(lp) {}

	template<class T>
	struct Suspend : INotifiable
	{
		Suspend(Store& store, const T& r) :
			store(store),r(r),v(store,r)
		{
			v.attach(this);
		}
		bool notify()
		{
			if (v.isLinear())
			{
				v.detach(this);
				return store.post(r);// and store.valid();
			}
			return true;
		}
		Store& store;
		T r;
		LP::LinearEqView<Store,T> v;
	};

//	template<class T1,class T2>
//	bool post(const Rel2<Equal,T1,T2>& r)
//	{	return postHelper(r);	}
//	template<class T1,class T2>
//	bool post(const Rel2<GreaterEqual,T1,T2>& r)
//	{	return postHelper(r);	}
//	template<class T1,class T2>
//	bool post(const Rel2<LessEqual,T1,T2>& r)
//	{	return postHelper(r);	}

	template<class T>
	bool post(const T& r);

	operator Env&() { return cp; }
	operator const Env&() const { return cp; }

	operator State&() { return cp; }
	operator const State&() const { return cp; }

	operator Util::IHeap&() { return cp; }
	operator const Util::IHeap&() const { return cp; }

	operator LP::Solver&() { return lp;}
	operator CP::Store&() { return cp;}

	bool valid() {	return cp.valid() and lp.valid();	}


//protected:
//	template<class Func,class T1, class T2>
//	bool postHelper(const Rel2<Func,T1,T2>& r);

//	using CP::Store::post;

	CP::Store&		cp;
	LP::Solver& 	lp;
};

//template<bool,class R>
//struct PostCPStore;
//
//template<class R>
//struct PostCPStore<true,R>
//{
//	bool operator()(Store& store, const R& r)
//	{	return static_cast<CP::Store&>(store).post(r);	}
//};
//
//template<class Func,class T1,class T2>
//struct PostCPStore<false,Rel2<Func,T1,T2> >
//{
//	bool operator()(Store& store,const Rel2<Func,T1,T2>& r)
//	{
//		LP::LinearExprView<Store,T1> v1(store,r.p1);
//		LP::LinearExprView<Store,T2> v2(store,r.p2);
//		if (v1.isLinear() and v2.isLinear())
//			return store.lp.post(r);
//		else
//		{
//			if (static_cast<CP::Store&>(store).post(r))
//				new (store) Store::Suspend<Func,T1,T2>(store,r);
//			return true;
//		}
//	}
//};
//
//template<class Func,class T1, class T2>
//bool Store::postHelper(const Rel2<Func,T1,T2>& r)
//{
//	const bool p1HasLPVar = MIP::Traits::HasLPVar<T1>::value;
//	const bool p2HasLPVar = MIP::Traits::HasLPVar<T2>::value;
//
//	return PostCPStore<!Util::IsSameType<typename Casper::Traits::GetEval<T1>::Type,double>::value
//				or	(!p1HasLPVar and !p2HasLPVar),Rel2<Func,T1,T2> >()(*this,r);
//}

template<class T>
bool Store::post(const T& r)
{
	// this doesn't work (must specialize e.g. for element relation)
//	const bool hasLPVar = MIP::Traits::HasLPVar<T>::value;
//	assert(hasLPVar);

	LP::LinearEqView<Store,T> l(*this,r);
	if (l.isTrue())
		return true;
	if (!l.canBeTrue())
		return false;
	if (l.isLinear())
		return lp.post(l.getRepr()) and lp.valid();

//	if (!cp.post(r))
//		return false;
	new (*this) Store::Suspend<T>(*this,r);
	return true;
}

} // MIP

} // Casper

#endif /* CASPER_MIP_STORE_H_ */
