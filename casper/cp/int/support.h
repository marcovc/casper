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
 

#ifndef CASPER_CP_INT_SUPPORT_H_
#define CASPER_CP_INT_SUPPORT_H_

#include <casper/kernel/notify/susplist.h>
#include <casper/kernel/reversible/sulist.h>
#include <map>
#include <algorithm>
#include <list>
#include <vector>

namespace Casper {
namespace CP {

namespace Detail {

template<int v,class Obj1,class Obj2>
struct GetSolvedEqRHS;

template<int v, class Obj1,class Obj2>
typename GetSolvedEqRHS<v,Obj1,Obj2>::Ret
getSolvedEqRHS(const Obj1& obj1, const Obj2& obj2)
{	return GetSolvedEqRHS<v,Obj1,Obj2>()(obj1,obj2);	}

template<class Obj1,class Obj2,class Obj3>
struct GetSolvedEqRHS<1,Rel2<Add,Obj1,Obj2>,Obj3>
{
	typedef Rel2<Sub,Obj3,Obj2> Ret;
	Ret operator()(const Rel2<Add,Obj1,Obj2>& r, const Obj3& obj3) const
	{	return rel<Sub>(obj3,r.p2);	}
};

template<class Obj1,class Obj2,class Obj3>
struct GetSolvedEqRHS<2,Rel2<Add,Obj1,Obj2>,Obj3>
{
	typedef Rel2<Sub,Obj3,Obj1> Ret;
	Ret operator()(const Rel2<Add,Obj1,Obj2>& r, const Obj3& obj3) const
	{	return rel<Sub>(obj3,r.p1);	}
};

template<class Obj1,class Obj2,class Obj3>
struct GetSolvedEqRHS<1,Rel2<Sub,Obj1,Obj2>,Obj3>
{
	typedef Rel2<Add,Obj3,Obj2> Ret;
	Ret operator()(const Rel2<Sub,Obj1,Obj2>& r, const Obj3& obj3) const
	{	return rel<Add>(obj3,r.p2);	}
};

template<class Obj1,class Obj2,class Obj3>
struct GetSolvedEqRHS<2,Rel2<Sub,Obj1,Obj2>,Obj3>
{
	typedef Rel2<Sub,Obj1,Obj3> Ret;
	Ret operator()(const Rel2<Sub,Obj1,Obj2>& r, const Obj3& obj3) const
	{	return rel<Sub>(r.p1,obj3);	}
};


template<int,class Eval,class Func>
struct SolveEqRel2;

template<class Eval>
struct SolveEqRel2<3,Eval,Add>
{
	static Eval eval(const Eval& e1, const Eval& e2)
	{	return e1+e2; }
};

template<class Eval>
struct SolveEqRel2<1,Eval,Add>
{
	static Eval evalLb(const Eval& e2, const Eval& result)
	{	return result-e2;	}
	static Eval evalUb(const Eval& e2, const Eval& result)
	{	return result-e2;	}
};

template<class Eval>
struct SolveEqRel2<2,Eval,Add>
{
	static Eval evalLb(const Eval& e1, const Eval& result)
	{	return result-e1;	}
	static Eval evalUb(const Eval& e1, const Eval& result)
	{	return result-e1;	}
};

template<class Eval>
struct SolveEqRel2<3,Eval,Sub>
{
	static Eval eval(const Eval& e1, const Eval& e2)
	{	return e1-e2; }
};

template<class Eval>
struct SolveEqRel2<1,Eval,Sub>
{
	static Eval evalLb(const Eval& e2, const Eval& result)
	{	return result+e2;	}
	static Eval evalUb(const Eval& e2, const Eval& result)
	{	return result+e2;	}
};

template<class Eval>
struct SolveEqRel2<2,Eval,Sub>
{
	static Eval evalLb(const Eval& e1, const Eval& result)
	{	return e1-result;	}
	static Eval evalUb(const Eval& e1, const Eval& result)
	{	return e1-result;	}
};

template<class Eval>
struct SolveEqRel2<3,Eval,Mul>
{
	static Eval eval(const Eval& e1, const Eval& e2)
	{	return e1*e2; }
};

template<class Eval>
struct SolveEqRel2<1,Eval,Mul>
{
	static Eval evalLb(const Eval& e2, const Eval& result)
	{
		if (e2==0)
			return limits<Eval>::negInf();
		return result/e2;
	}
	static Eval evalUb(const Eval& e2, const Eval& result)
	{
		if (e2==0)
			return limits<Eval>::posInf();
		return static_cast<Eval>(std::ceil(result/static_cast<float>(e2)));
	}
};

template<class Eval>
struct SolveEqRel2<2,Eval,Mul>
{
	static Eval evalLb(const Eval& e1, const Eval& result)
	{
		if (e1==0)
			return limits<Eval>::negInf();
		return result/e1;
	}
	static Eval evalUb(const Eval& e1, const Eval& result)
	{
		if (e1==0)
			return limits<Eval>::posInf();
		return static_cast<Eval>(std::ceil(result/static_cast<float>(e1)));
	}
};
} // Detail

template<class,class>
struct ValuesView;

template<class EvalT,class Dom>
struct ValuesView<EvalT,Var<EvalT,Dom> >
{
	typedef EvalT	Eval;
	typedef ValuesView<Eval,Var<Eval,Dom> > Container;
	typedef typename Dom::Iterator Iterator;
	typedef typename Dom::ReverseIterator ReverseIterator;
	Iterator begin() const { return d.begin();}
	Iterator end() const { return d.end();}
	ReverseIterator rbegin() const { return d.rbegin();}
	ReverseIterator rend() const { return d.rend();}
	ValuesView(Store& store, const Var<Eval,Dom>& v) : store(store),d(v.domain()) {}
	Iterator findGreater(const Eval& v) const
	{	return d.upperBound(v);	}
	ReverseIterator findLess(const Eval& v) const
	{
		assert(!d.empty());
		Iterator it(d.lowerBound(v));
		return ReverseIterator(it);
	}
	Iterator find(const Eval& v) const
	{	return d.find(v);	}


	typedef Util::StdList<INotifiable*>	List;
	typedef std::map<Eval,List> NotifyMap;
	typedef Util::StdPair<typename NotifyMap::iterator,typename List::Iterator> AttachRef;

	struct OnErase : INotifiable
	{
		OnErase(ValuesView& rOwner) :
			rOwner(rOwner) {}
		bool notify()
		{
			auto itdb = rOwner.d.delta().first();
			auto itde = rOwner.d.delta().last();
//			std::cout << "variable "<< &rOwner.d <<  " is now " << rOwner.d << " - lost values [" << *itdb << "," << *itde << "]\n";

			// TODO: investigate why usual itdb<itde does not work below when itde is end()
			for ( ; itdb != rOwner.d.end() and *itdb <= *itde; ++itdb )
			{
//				std::cout << "searching slot " << *itdb << std::endl;
				auto it = rOwner.notifyMap.find(*itdb);	// FIXME: should increment instead?
				if (it != rOwner.notifyMap.end())
					for (auto lit = it->second.begin(); lit != it->second.end(); )
						if (not (*(lit++))->notify())
						{
							std::cout << "notify is failing\n";
							return false;
						}
			}
			return true;
		}
		ValuesView& rOwner;
	};

	AttachRef attach(const Iterator& vit, INotifiable* f)
	{
		if (notifyMap.empty())
			d.attachOnDomain(new (store) OnErase(*this));
		auto mit = notifyMap.find(*vit);
		if (mit == notifyMap.end())
		{
			List list;
			auto nmit = notifyMap.insert(typename NotifyMap::value_type(*vit,list)).first;
			auto nlit = nmit->second.pushFront(f);
//			std::cout << &d << ": returning new attach ref in new tree node : " << nmit->first << "," << *nlit << std::endl;
			return Casper::pair(nmit,nlit);
		}
		else
		{
			auto lit = mit->second.pushFront(f);
//			std::cout << &d << ": returning new attach ref in existing tree node : " << mit->first << "," << *lit << std::endl;
			return Casper::pair(mit,lit);
		}
	}

	AttachRef attach(ReverseIterator vit, INotifiable* f)
	{	return attach((++vit).base(),f);	}

	void detach(const AttachRef& r)	// O(1)
	{
//		std::cout << "variable " << &d <<  " detaching from value " << r.first->first << std::endl;
		r.first->second.erase(r.second);
	}

	void attach(INotifiable* f)
	{	d.attachOnDomain(f);	}

	Store& store;
	Dom& d;
	NotifyMap notifyMap;
};

namespace Detail {

// Requires both ValsView1 and ValsView2 to be strictly increasing
template<class Eval,class Func,class ValsView1,class ValsView2>
struct Rel2Iterator
{
	typedef Rel2Iterator<Eval,Func,ValsView1,ValsView2>	Self;
	Rel2Iterator(const ValsView1& v1,const ValsView2& v2) :
		pV1(&v1),pV2(&v2),
		it1(v1.begin()),
		it2(v2.begin()) {}
	Rel2Iterator(const ValsView1& v1,const ValsView2& v2,bool) :
		pV1(&v1),pV2(&v2),
		it1(v1.end()),
		it2(v2.end()) {}
	Rel2Iterator(const Self& s) :
		pV1(s.pV1),pV2(s.pV2),
		it1(s.it1),
		it2(s.it2) {}
	bool operator==(const Self& s) const
	{
		return it1 == s.it1 and (it1==pV1->end() or it2 == s.it2);
	}
	bool operator!=(const Self& s) const
	{
		return it1 != s.it1 or (it1!=pV1->end() and it2 != s.it2);
	}
	Eval operator*() const
	{
		assert(it1!=pV1->end() and it2!=pV2->end());
		return func3.eval(*it1,*it2);
	}
	Self& operator++()
	{
		assert(it1 != pV1->end());
		assert(it2 != pV2->end());
		Eval curValue = **this;

		// search on the direction 1
		typename ValsView1::Iterator newIt1(it1);
		typename ValsView2::Iterator newIt2(pV2->end());
		++newIt1;

		if (newIt1 != pV1->end())
		{
			newIt2 = pV2->findGreater(func2.evalLb(*newIt1,curValue));
			assert(newIt2 != pV2->end());
			for (typename ValsView1::Iterator aux1(newIt1); ++aux1 != pV1->end(); )
			{
				// TODO: improve: use previous newIt2, e.g. pV2->findGreaterInRange(...,pV2->begin(),newIt2);
				typename ValsView2::Iterator aux2(pV2->findGreater(func2.evalLb(*aux1,curValue)));
				assert(aux2 != pV2->end());
				if (func3.eval(*aux1,*aux2) < func3.eval(*newIt1,*newIt2))
				{
					newIt1 = aux1;
					newIt2 = aux2;
				}
				if (aux2 == pV2->begin())
					break;
			}
		}

		// search on the direction 2
		++it2;

		if (it2 == pV2->end())
		{
			it1 = newIt1;
			it2 = newIt2;
			return *this;
		}

		it1 = pV1->findGreater(func1.evalLb(*it2,curValue));
		assert(it1 != pV1->end());
		for (typename ValsView2::Iterator aux2(it2); ++aux2 != pV2->end(); )
		{
			typename ValsView1::Iterator aux1(pV1->findGreater(func1.evalLb(*aux2,curValue)));
			assert(aux1 != pV1->end());
			if (func3.eval(*aux1,*aux2) < func3.eval(*it1,*it2))
			{
				it1 = aux1;
				it2 = aux2;
			}
			if (aux1 == pV1->begin())
				break;
		}

		assert(it1!=pV1->end() and it2!=pV2->end());

		if (newIt1 != pV1->end() and func3.eval(*newIt1,*newIt2) < func3.eval(*it1,*it2))
		{
			it1 = newIt1;
			it2 = newIt2;
		}

		return *this;
	}


	const ValsView1* pV1;
	const ValsView2* pV2;
	typename ValsView1::Iterator it1;
	typename ValsView2::Iterator it2;
	static Detail::SolveEqRel2<1,Eval,Func>	func1;
	static Detail::SolveEqRel2<2,Eval,Func>	func2;
	static Detail::SolveEqRel2<3,Eval,Func>	func3;
};

// Requires both ValsView1 and ValsView2 to be strictly increasing
template<class Eval,class Func,class ValsView1,class ValsView2>
struct Rel2ReverseIterator
{
	typedef Rel2ReverseIterator<Eval,Func,ValsView1,ValsView2>	Self;
	Rel2ReverseIterator(const ValsView1& v1,const ValsView2& v2) :
		pV1(&v1),pV2(&v2),
		it1(v1.rbegin()),
		it2(v2.rbegin()) {}
	Rel2ReverseIterator(const ValsView1& v1,const ValsView2& v2,bool) :
		pV1(&v1),pV2(&v2),
		it1(v1.rend()),
		it2(v2.rend()) {}
	Rel2ReverseIterator(const Self& s) :
		pV1(s.pV1),pV2(s.pV2),
		it1(s.it1),
		it2(s.it2) {}
	bool operator==(const Self& s) const
	{
		return it1 == s.it1 and (it1==pV1->rend() or it2 == s.it2);
	}
	bool operator!=(const Self& s) const
	{
		return it1 != s.it1 or (it1!=pV1->rend() and it2 != s.it2);
	}
	Eval operator*() const
	{
		assert(it1!=pV1->rend() and it2!=pV2->rend());
		return func3.eval(*it1,*it2);
	}
	Self& operator++()
	{
		assert(it1 != pV1->rend());
		assert(it2 != pV2->rend());
		Eval curValue = **this;

		// search on the direction 1
		typename ValsView1::ReverseIterator newIt1(it1);
		typename ValsView2::ReverseIterator newIt2(pV2->rend());
		++newIt1;

		if (newIt1 != pV1->rend())
		{
			newIt2 = pV2->findLess(func2.evalUb(*newIt1,curValue));
			assert(newIt2 != pV2->rend());
			for (typename ValsView1::ReverseIterator aux1(newIt1); ++aux1 != pV1->rend(); )
			{
				typename ValsView2::ReverseIterator aux2(pV2->findLess(func2.evalUb(*aux1,curValue)));
				assert(aux2 != pV2->rend());
				if (func3.eval(*aux1,*aux2) > func3.eval(*newIt1,*newIt2))
				{
					newIt1 = aux1;
					newIt2 = aux2;
				}
				if (aux2 == pV2->rbegin())
					break;
			}
		}

		// search on the direction 2
		++it2;

		if (it2 == pV2->rend())
		{
			it1 = newIt1;
			it2 = newIt2;
			return *this;
		}

		it1 = pV1->findLess(func1.evalUb(*it2,curValue));
		assert(it1 != pV1->rend());
		for (typename ValsView2::ReverseIterator aux2(it2); ++aux2 != pV2->rend(); )
		{
			typename ValsView1::ReverseIterator aux1(pV1->findLess(func1.evalUb(*aux2,curValue)));
			assert(aux1 != pV1->rend());
			if (func3.eval(*aux1,*aux2) > func3.eval(*it1,*it2))
			{
				it1 = aux1;
				it2 = aux2;
			}
			if (aux1 == pV1->rbegin())
				break;
		}

		assert(it1!=pV1->rend() and it2!=pV2->rend());

		if (newIt1 != pV1->rend() and func3.eval(*newIt1,*newIt2) > func3.eval(*it1,*it2))
		{
			it1 = newIt1;
			it2 = newIt2;
		}

		return *this;
	}


	const ValsView1* pV1;
	const ValsView2* pV2;
	typename ValsView1::ReverseIterator it1;
	typename ValsView2::ReverseIterator it2;
	static Detail::SolveEqRel2<1,Eval,Func>	func1;
	static Detail::SolveEqRel2<2,Eval,Func>	func2;
	static Detail::SolveEqRel2<3,Eval,Func>	func3;
};

template<class Func, class ValsView1, class ValsView2, class EvalT>
struct Rel2ValuesView
{
	typedef EvalT	Eval;
	typedef Rel2Iterator<Eval,Func,ValsView1,ValsView2> Iterator;
	typedef Rel2ReverseIterator<Eval,Func,ValsView1,ValsView2> ReverseIterator;


	struct Watch : INotifiable
	{
		Watch(Rel2ValuesView& owner, Iterator it, INotifiable* pNot) :
			owner(owner),
			it(it),val(*it),
			pNot(pNot),
			attachRef1(owner.v1.attach(it.it1,this)),
			attachRef2(owner.v2.attach(it.it2,this))
		{	}
		bool notify()
		{
//			std::cout << this << ": lost (" << *it.it1 << "," << *it.it2  << ") support for "
//				<< val << ". searching for new support..." << std::endl;
			Iterator newIt = owner.find(val); // TODO: use old iter as starting point

			if (newIt!=owner.end())
			{
//				std::cout << "found (" << *newIt.it1 << "," << *newIt.it2 << "), reattaching.\n";
				owner.v1.detach(attachRef1);
				owner.v2.detach(attachRef2);
				it = newIt;
				attachRef1 = owner.v1.attach(it.it1,this);
				attachRef2 = owner.v2.attach(it.it2,this);
				return true;
			}
//			std::cout << "not found, notifying.\n";
			return pNot->notify();
		}
		void detach()
		{
//			std::cout << "detaching expression\n";
			owner.v1.detach(attachRef1);
			owner.v2.detach(attachRef2);
		}
		Rel2ValuesView& owner;
		Iterator it;
		const Eval	val;
		INotifiable* pNot;
		typename ValsView1::AttachRef attachRef1;
		typename ValsView2::AttachRef attachRef2;
	};

	typedef Watch*	AttachRef;

	template<class Obj1,class Obj2>
	Rel2ValuesView(Store& store,const Obj1& p1, const Obj2& p2) :
		store(store),v1(store,p1),v2(store,p2) {}

	Iterator begin() const {	return Iterator(v1,v2); }
	Iterator end() const {	return Iterator(v1,v2,true); }
	ReverseIterator rbegin() const {	return ReverseIterator(v1,v2); }
	ReverseIterator rend() const {	return ReverseIterator(v1,v2,true); }

	Iterator find(const Eval& val) const
	{
		auto it = begin();
		if (val < *it or val > *rbegin())
			return end();
		for ( ; it != end(); ++it)
		{
			auto itval = *it;
			if (itval == val)
				return it;
			if (val < itval)
				return end();
		}
		return end();
	}

	Iterator findGreater(const Eval& val) const
	{
		auto it = begin();
		for ( ; it != end(); ++it)
			if (val < *it)
				return it;
		return end();
	}

	ReverseIterator findLess(const Eval& val) const
	{
		auto it = rbegin();
		for ( ; it != rend(); ++it)
			if (val > *it)
				return it;
		return rend();
	}

	AttachRef attach(const Iterator& it, INotifiable* f)
	{
		return new Watch(*this,it,f);
	}

	AttachRef attach(const ReverseIterator& it, INotifiable* f)
	{
		return new Watch(*this,it,f);
	}

	void attach(INotifiable* f)
	{
		v1.attach(f);
		v2.attach(f);
	}

	void detach(const AttachRef& attachRef)
	{
		attachRef->detach();
		delete attachRef;
	}

	Store&	store;
	ValsView1 v1;
	ValsView2 v2;

};

template<class ValsView>
struct ValuesViewInverter
{
	typedef typename ValsView::Eval	Eval;
	typedef typename ValsView::ReverseIterator Iterator;
	typedef typename ValsView::Iterator ReverseIterator;
	typedef typename ValsView::AttachRef AttachRef;

	template<class Obj>
	ValuesViewInverter(Store& store, const Obj& obj) : v(store,obj) {}
	Iterator begin() const {	return v.rbegin(); }
	Iterator end() const {	return v.rend(); }
	ReverseIterator rbegin() const {	return v.begin(); }
	ReverseIterator rend() const {	return v.end(); }
	AttachRef attach(const Iterator& it, INotifiable* f) {	return v.attach(it,f);	}
	AttachRef attach(const ReverseIterator& it, INotifiable* f) {	return v.attach(it,f);	}
	void attach(INotifiable* f) { v.attach(f);	}
	void detach(const AttachRef& attachRef) {	v.detach(attachRef);	}
	Iterator findGreater(const Eval& val) const
	{	return v.findLess(val);	}
	ReverseIterator findLess(const Eval& val) const
	{	return v.findGreater(val);	}
	ValsView v;
};

template<class Func, class ValsView1, class ValsView2, class EvalT>
struct CachedRODomView
{
	typedef EvalT	Eval;
	typedef SUList<EvalT>	Container;
	typedef typename Container::Iterator Iterator;
	typedef typename Container::ReverseIterator ReverseIterator;
	typedef typename ValsView1::Iterator Iterator1;
	typedef typename ValsView2::Iterator Iterator2;

	struct OperandIterators
	{
		OperandIterators(const Iterator1& it1, const Iterator2& it2) :
			it1(it1),it2(it2) {}
		Iterator1 it1;
		Iterator2 it2;
	};

	struct Watch : INotifiable
	{
		Watch(CachedRODomView& owner, Iterator it, INotifiable* pNot) :
			owner(owner),
			it(it),
			opit(owner.findOperandSupport(*it)),
			pNot(pNot)
		{
			assert(opit.it1 != owner.v1.end() and opit.it2 != owner.v2.end());
			attachRef1 = owner.v1.attach(opit.it1,this);
			attachRef2 = owner.v2.attach(opit.it2,this);
		}
		bool notify()
		{
			std::cout << this << ": lost support for "
				<< *it << ". searching for new support..." << std::endl;
			OperandIterators newIt = owner.findOperandSupport(*it); // TODO: use old iter as starting point

			if (newIt.it1!=owner.v1.end())
			{
				assert(newIt.it2!=owner.v2.end()); // by definition
				std::cout << "found (" << *newIt.it1 << "," << *newIt.it2 << "), reattaching.\n";
				owner.v1.detach(attachRef1);
				owner.v2.detach(attachRef2);
				attachRef1 = owner.v1.attach(newIt.it1,this);
				attachRef2 = owner.v2.attach(newIt.it2,this);
				return true;
			}
			std::cout << "not found, notifying.\n";
			owner.container.erase(it);
			std::cout << "after erase " << owner.container << std::endl;
			return !owner.container.empty() and pNot->notify();
		}
		void detach()
		{
//			std::cout << "detaching expression\n";
			owner.v1.detach(attachRef1);
			owner.v2.detach(attachRef2);
		}
		CachedRODomView& owner;
		Iterator it;
		OperandIterators opit;
		INotifiable* pNot;
		typename ValsView1::AttachRef attachRef1;
		typename ValsView2::AttachRef attachRef2;
	};

	typedef Watch*	AttachRef;

	template<class Obj1,class Obj2>
	CachedRODomView(Store& store,const Obj1& p1, const Obj2& p2) :
		store(store),v1(store,p1),v2(store,p2),container(store) {}

	Iterator begin() const	{	return container.begin();	}
	Iterator end() const	{	return container.end();	}
	ReverseIterator rbegin() const	{	return container.rbegin();	}
	ReverseIterator rend() const	{	return container.rend();}

	Iterator find(const Eval& val) const	{	return container.find(val);}

	Iterator findGreater(const Eval& val) const	{	return container.upperBound(val);	}

	ReverseIterator findLess(const Eval& val) const
	{
		auto it = rbegin();
		for ( ; it != rend(); ++it)
			if (val > *it)
				return it;
		return rend();
	}

	AttachRef attach(const Iterator& it, INotifiable* f)
	{
		return new Watch(*this,it,f);
	}

	AttachRef attach(const ReverseIterator& it, INotifiable* f)
	{
		return new Watch(*this,it,f);
	}

	void attach(INotifiable* f)
	{
		v1.attach(f);
		v2.attach(f);
	}

	void detach(const AttachRef& attachRef)
	{
		attachRef->detach();
		delete attachRef;
	}

	bool initialize()
	{
		std::list<int> l;
		for (auto it1 = v1.begin(); it1 != v1.end(); ++it1)
			for (auto it2 = v2.begin(); it2 != v2.end(); ++it2)
				l.push_back(func3.eval(*it1,*it2));
		if (l.empty())
			return false;
		std::vector<int> v(l.begin(),l.end());
		std::sort(v.begin(),v.end());
		Eval c = *v.begin();
		container.insert(container.end(),c);
		for (auto it = ++v.begin(); it != v.end(); ++it)
			if (*it>c)
			{
				c = *it;
				container.insert(container.end(),c);
			}
		return !container.empty();
	}

protected:
	OperandIterators findOperandSupport(const Eval& val)
	{
		for (auto it1 = v1.begin(); it1 != v1.end(); ++it1)
			for (auto it2 = v2.begin(); it2 != v2.end(); ++it2)
				if (func3.eval(*it1,*it2)==val)
					return OperandIterators(it1,it2);
		return OperandIterators(v1.end(),v2.end());
	}
	Store&	store;
	ValsView1 v1;
	ValsView2 v2;
	Container	container;
	mutable bool	first;
	static Detail::SolveEqRel2<3,Eval,Func>	func3;

};

} // Detail

#if 0
/**
 * 	ValuesView for A+B
 */
template<class Obj1, class Obj2,class Eval>
struct ValuesView<Eval,Rel2<Add,Obj1,Obj2> > :
	Detail::Rel2ValuesView<Add,ValuesView<Eval,Obj1>,ValuesView<Eval,Obj2>,Eval>
{
	typedef Detail::Rel2ValuesView<Add,ValuesView<Eval,Obj1>,ValuesView<Eval,Obj2>,Eval> Super;
	ValuesView(Store& store,const Rel2<Add,Obj1,Obj2>& r) :
		Super(store,r.p1,r.p2) {}
};

/**
 * 	ValuesView for A-B
 */
template<class Obj1, class Obj2,class Eval>
struct ValuesView<Eval,Rel2<Sub,Obj1,Obj2> > :
	Detail::Rel2ValuesView<Sub,ValuesView<Eval,Obj1>,
						Detail::ValuesViewInverter<ValuesView<Eval,Obj2> >,Eval>
{
	typedef Detail::Rel2ValuesView<Sub,ValuesView<Eval,Obj1>,
						Detail::ValuesViewInverter<ValuesView<Eval,Obj2> >,Eval> Super;
	ValuesView(Store& store,const Rel2<Sub,Obj1,Obj2>& r) :
		Super(store,r.p1,r.p2) {}
};

/**
 * 	ValuesView for A*B
 * 	\note Works for A>0, B>0 only.
 */
template<class Obj1, class Obj2,class Eval>
struct ValuesView<Eval,Rel2<Mul,Obj1,Obj2> > :
	Detail::Rel2ValuesView<Mul,ValuesView<Eval,Obj1>,ValuesView<Eval,Obj2>,Eval>
{
	typedef Detail::Rel2ValuesView<Mul,ValuesView<Eval,Obj1>,ValuesView<Eval,Obj2>,Eval> Super;
	ValuesView(Store& store,const Rel2<Mul,Obj1,Obj2>& r) :
		Super(store,r.p1,r.p2) {}
};
#else
/**
 * 	ValuesView for A+B
 */
template<class Obj1, class Obj2,class Eval>
struct ValuesView<Eval,Rel2<Add,Obj1,Obj2> > :
	Detail::CachedRODomView<Add,ValuesView<Eval,Obj1>,ValuesView<Eval,Obj2>,Eval>
{
	typedef Detail::CachedRODomView<Add,ValuesView<Eval,Obj1>,ValuesView<Eval,Obj2>,Eval> Super;
	ValuesView(Store& store,const Rel2<Add,Obj1,Obj2>& r) :
		Super(store,r.p1,r.p2) {}
};

/**
 * 	ValuesView for A-B
 */
template<class Obj1, class Obj2,class Eval>
struct ValuesView<Eval,Rel2<Sub,Obj1,Obj2> > :
	Detail::CachedRODomView<Sub,ValuesView<Eval,Obj1>,
						Detail::ValuesViewInverter<ValuesView<Eval,Obj2> >,Eval>
{
	typedef Detail::CachedRODomView<Sub,ValuesView<Eval,Obj1>,
						Detail::ValuesViewInverter<ValuesView<Eval,Obj2> >,Eval> Super;
	ValuesView(Store& store,const Rel2<Sub,Obj1,Obj2>& r) :
		Super(store,r.p1,r.p2) {}
};

/**
 * 	ValuesView for A*B
 * 	\note Works for A>0, B>0 only.
 */
template<class Obj1, class Obj2,class Eval>
struct ValuesView<Eval,Rel2<Mul,Obj1,Obj2> > :
	Detail::CachedRODomView<Mul,ValuesView<Eval,Obj1>,ValuesView<Eval,Obj2>,Eval>
{
	typedef Detail::CachedRODomView<Mul,ValuesView<Eval,Obj1>,ValuesView<Eval,Obj2>,Eval> Super;
	ValuesView(Store& store,const Rel2<Mul,Obj1,Obj2>& r) :
		Super(store,r.p1,r.p2) {}
};
#endif

struct AC3;
struct AC6;

template<class Alg,class Obj1,class Obj2>
struct Project;

template<class Alg,class Obj1,class Obj2>
bool postProject(Store& store, const Obj1& obj1, const Obj2& obj2)
{	return Project<Alg,Obj1,Obj2>::post(store,obj1,obj2);	}

template<class Alg,class Func,class Obj1,class Obj2,class Obj3>
struct Project<Alg,Rel2<Func,Obj1,Obj2>,Obj3>
{
	static bool post(Store& store,const Rel2<Func,Obj1,Obj2>& r, const Obj3& obj3)
	{
		return postProject<Alg>(store,r.p1,Detail::getSolvedEqRHS<1>(r,obj3)) and
			   postProject<Alg>(store,r.p2,Detail::getSolvedEqRHS<2>(r,obj3));
	}
};

template<class Eval,class Dom,class Obj2>
struct Project<AC3,Var<Eval,Dom>,Obj2> : IFilter
{
	Project(Store& store,const Var<Eval,Dom>& v, const Obj2& obj2) :
		IFilter(store),d(v.domain()),v2(store,obj2),first(true) {}
	bool execute()
	{
		std::cout << "AC3. Projecting values {";
		for (auto it = v2.begin(); it != v2.end(); ++it)
			std::cout << *it << " ";
		std::cout << "}" << std::endl;
		if (first)
		{
			std::cout << "AC3:init\n";
			first = false;
			if (!v2.initialize())
				return false;
		}
		return d.intersectFwd(v2.begin(), v2.end());
	}
	void attach(INotifiable* f)
	{	v2.attach(f);	}
	void detach(INotifiable* f)
	{	/* TODO */ }
	static bool post(Store& store,const Var<Eval,Dom>& v, const Obj2& obj2)
	{	return store.post(new (store) Project(store,v,obj2));	}
	Dom&	d;
	ValuesView<Eval,Obj2> v2;
	bool	first;
};

template<class Eval,class Dom,class Obj2>
struct Project<AC6,Var<Eval,Dom>,Obj2> : IFilter
{
	Project(Store& store,const Var<Eval,Dom>& v, const Obj2& obj2) :
		IFilter(store),d(v.domain()),v2(store,obj2),first(true) {}
	bool execute()
	{
		if (first)
		{
			first = false;
			if (!v2.initialize())
				return false;
		}
		return d.intersectFwd(v2.begin(), v2.end());
	}
	void attach(INotifiable* f)
	{
		if (first)
		{
			first = false;
			assert(v2.initialize());
		}
		auto dit = d.begin();
		auto vit = v2.begin();	// TODO: could use v2.findGreater
		for ( ; vit != v2.end() and dit != d.end(); )
			if (*vit < *dit)
				++vit;
			else
			if (*vit > *dit)
				++dit;
			else
			{
				assert(*vit==*dit);
				v2.attach(vit,f);
				++vit;
				++dit;
			}
	}
	void detach(INotifiable* f)
	{	/* TODO */ }
	static bool post(Store& store,const Var<Eval,Dom>& v, const Obj2& obj2)
	{	return store.post(new (store) Project(store,v,obj2));	}
	Dom&	d;
	ValuesView<Eval,Obj2> v2;
	bool	first;
};

template<class Eval,class Obj1,class Obj2>
struct PostDomFilter2<Equal,Eval,Obj1,Eval,Obj2>
{
	static bool post(Store& store,const Obj1& obj1, const Obj2& obj2)
	{
		return postProject<AC6>(store,obj1,obj2) and
			   postProject<AC6>(store,obj2,obj1);
	}
};

}
}


#endif /* CASPER_CP_INT_SUPPORT_H_ */
