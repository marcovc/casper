/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2009 - Marco Correia <marco.v.correia@gmail.com>           	   *
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

#ifndef CASPER_SET_CHKVIEW_H_
#define CASPER_SET_CHKVIEW_H_

#include <casper/cp/view/chk.h>
#include <casper/kernel/rel/setrel.h>

namespace Casper {
namespace CP {

template<class Elem,class Expr1,class Expr2>
struct ChkViewRel2<Member,Elem,Expr1,Set<Elem>,Expr2>
{
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
			store(store),elem(store,p1),set(store,p2) {}
	bool isTrue() const	// is it true?
	{	return elem.ground() and
			   set->findInIn(elem.value()) != set->endIn();	}
	bool canBeTrue() const 	// can it still be true?
	{	return !elem.ground() or
			   set->findInIn(elem.value()) != set->endIn() or
			   set->findInPoss(elem.value()) != set->endPoss();
	}
	bool setToTrue()
	{
		detach(pOwner);
		return store.post(member(elem.getObj(),set.getObj()));
	}
	bool setToFalse()
	{
		detach(pOwner);
		return store.post(notMember(elem.getObj(),set.getObj()));
	}

	void attach(INotifiable* f) { 	pOwner=f; elem.attach(f); set->attachOnDomain(f);}
	void detach(INotifiable* f) {	elem.detach(f); set->detachOnDomain(f);}

	Rel2<Member,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Member,Expr1,Expr2>(elem.getObj(),set.getObj());	}

	Store&		store;
	ValView<Elem,Expr1>	elem;
	DomView<Set<Elem>,Expr2>	set;
	INotifiable*	pOwner;
};



template<class Elem,class Expr1,class Expr2>
struct ChkViewRel2<NotMember,Elem,Expr1,Set<Elem>,Expr2>
{
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
			store(store),elem(store,p1),set(store,p2) {}
	bool isTrue() const	// is it true?
	{	return elem.ground() and
			   set->findInPoss(elem.value()) == set->endPoss() and
			   set->findInIn(elem.value()) == set->endIn();
	}
	bool canBeTrue() const 	// can it still be true?
	{	return !elem.ground() or
			   set->findInIn(elem.value()) == set->endIn();
	}
	bool setToTrue()
	{
		detach(pOwner);
		return store.post(notMember(elem.getObj(),set.getObj()));
	}
	bool setToFalse()
	{
		detach(pOwner);
		return store.post(member(elem.getObj(),set.getObj()));
	}

	void attach(INotifiable* f) { 	pOwner=f; elem.attach(f); set->attachOnDomain(f);}
	void detach(INotifiable* f) {	elem.detach(f); set->detachOnDomain(f);}

	Rel2<NotMember,Expr1,Expr2> getObj()  const
	{ 	return Rel2<NotMember,Expr1,Expr2>(elem.getObj(),set.getObj());	}

	Store&	store;
	ValView<Elem,Expr1>	elem;
	DomView<Set<Elem>,Expr2>	set;
	INotifiable*	pOwner;
};


// FIXME: not tested at all
template<class Elem,class Expr1,class Expr2>
struct ChkViewRel2<Contained,Set<Elem>,Expr1,Set<Elem>,Expr2>
{
	typedef typename DomView<Set<Elem>,Expr1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,Expr2>::Dom	DomY;

	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
			store(store),x(store,p1),y(store,p2) {}
	bool isTrue() const	// when lub(x) C glb(y)
	{	return !makeDiffIt(makeLUBIt(*x),makeInIt(*y)).valid();	}
	bool canBeTrue() const 	// when glb(x) C lub(y)
	{	return !makeDiffIt(makeInIt(*x),makeLUBIt(*y)).valid();	}
	bool setToTrue()
	{
		detach(pOwner);
		return postBndFilter(store,contained(x.getObj(),y.getObj()));
	}
	bool setToFalse()
	{
		detach(pOwner);
		return postBndFilter(store,cardinal(intersect(x.getObj(),y.getObj()))<cardinal(x.getObj()));
	}

	void attach(INotifiable* f)
	{ 	pOwner=f; x->attachOnDomain(f);	y->attachOnDomain(f);}
	void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f);}

	Rel2<Contained,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Contained,Expr1,Expr2>(x.getObj(),y.getObj());	}

	Store&	store;
	DomView<Set<Elem>,Expr1>	x;
	DomView<Set<Elem>,Expr2>	y;
	INotifiable*	pOwner;
};


template<class Elem,class Expr1,class Expr2>
struct ChkViewRel2<Equal,Set<Elem>,Expr1,Set<Elem>,Expr2>
{
	typedef typename DomView<Set<Elem>,Expr1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,Expr2>::Dom	DomY;

	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
			store(store),x(store,p1),y(store,p2) {}
	bool isTrue() const	// when lub(x) = lub(y)
	{
		return x->ground() and y->ground() and
			   Util::equal(x->beginIn(),x->endIn(),
							 y->beginIn(),y->endIn());
	}
	bool canBeTrue() const
	{
		return !makeDiffIt(makeInIt(*x),makeLUBIt(*y)).valid() and
			   !makeDiffIt(makeInIt(*y),makeLUBIt(*x)).valid();
	}
	bool setToTrue()
	{
		detach(pOwner);
		return postBndFilter(store,equal(x.getObj(),y.getObj()));
	}
	bool setToFalse()
	{
		detach(pOwner);
		return postBndFilter(store,distinct(x.getObj(),y.getObj()));
	}

	void attach(INotifiable* f)
	{ 	pOwner=f; x->attachOnDomain(f);	y->attachOnDomain(f);}
	void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f);}

	Rel2<Equal,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Equal,Expr1,Expr2>(x.getObj(),y.getObj());	}

	Store&	store;
	DomView<Set<Elem>,Expr1>	x;
	DomView<Set<Elem>,Expr2>	y;
	INotifiable*	pOwner;
};


template<class Elem,class Expr1,class Expr2>
struct ChkViewRel2<Disjoint,Set<Elem>,Expr1,Set<Elem>,Expr2>
{
	ChkViewRel2(Store& store, const Expr1& p1,const Expr2& p2) :
			store(store),x(store,p1),y(store,p2) {}
	bool isTrue() const	// is it true?
	{
		return !makeInterIt(makeLUBIt(*x),makeLUBIt(*y)).valid();
	}
	bool canBeTrue() const 	// can it still be true?
	{
		return !Util::intersect(x->beginIn(),x->endIn(),
								  y->beginIn(),y->endIn());
	}
	bool setToTrue()
	{
		detach(pOwner);
		return store.post(disjoint(x.getObj(),y.getObj()));
	}
	bool setToFalse()		// FIXME: using views instead of auxvar
	{
		detach(pOwner);
		Util::StdVector<Elem> lub1(x->inSize()+x->possSize());
		Detail::setLUB(*x,lub1);

		Util::StdVector<Elem> lub2(y->inSize()+y->possSize());
		Detail::setLUB(*y,lub2);

		Util::QuickList<Elem> lub3(lub1.size()+lub2.size());
		Util::setUnion(lub1.begin(),lub1.end(),
						 lub2.begin(),lub2.end(),
						 Util::ListInserter<Elem>(lub3));
		Var<Set<Elem> > aux(store,lub3);
		return store.post(intersectEqual(x.getObj(),y.getObj(),aux) and
							 cardinal(aux)>0);
	}

	void attach(INotifiable* f) { 	pOwner=f; x->attachOnDomain(f); y->attachOnDomain(f);}
	void detach(INotifiable* f) {	x->detachOnDomain(f); y->detachOnDomain(f);}

	Rel2<Disjoint,Expr1,Expr2> getObj()  const
	{ 	return Rel2<Disjoint,Expr1,Expr2>(x.getObj(),y.getObj());	}

	Store&	store;
	DomView<Set<Elem>,Expr1>	x;
	DomView<Set<Elem>,Expr2>	y;
	INotifiable*	pOwner;
};


// FIXME: not tested at all
template<class Elem,class Expr1,class Expr2,class Expr3>
struct ChkViewRel3<IntersectEqual,Set<Elem>,Expr1,Set<Elem>,Expr2,
					Set<Elem>,Expr3>
{
	typedef typename DomView<Set<Elem>,Expr1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,Expr2>::Dom	DomY;
	typedef typename DomView<Set<Elem>,Expr3>::Dom	DomZ;

	ChkViewRel3(Store& store, const Expr1& p1,const Expr2& p2,
					const Expr3& p3) :
			store(store),x(store,p1),y(store,p2),z(store,p3) {}
	bool isTrue() const	// is it true?
	{
		if (!z->ground() or
			z->minCard()>std::min(x->maxCard(),y->maxCard()))
			return false;

		return  !makeSymDiffIt(makeInterIt(makeInIt(*x),makeInIt(*y)),makeInIt(*z)).valid() and
				!makeDiffIt(makePossIt(*x),makeInIt(*z)).valid() and
				!makeDiffIt(makePossIt(*y),makeInIt(*z)).valid();
	}
	bool canBeTrue() const 	// can it still be true?
	{
		// if in(z) C intersection(lub(x),lub(y)) and
		//  intersection(in(x),in(y)) C lub(z)

		return z->minCard()<=std::min(x->maxCard(),y->maxCard()) and
				!makeDiffIt(makeInIt(*z),makeInterIt(makeLUBIt(*x),makeLUBIt(*y))).valid() and
				!makeDiffIt(makeInterIt(makeInIt(*x),makeInIt(*y)),makeLUBIt(*z)).valid();
	}
	bool setToTrue()
	{
		detach(pOwner);
		return store.post(intersectEqual(x.getObj(),y.getObj(),z.getObj()));
	}
	bool setToFalse()		// FIXME: using views instead of auxvar
	{	throw Exception::UndefinedFilter("not intersect(set,set,set)");	}

	void attach(INotifiable* f)
	{ 	pOwner=f; x->attachOnDomain(f); y->attachOnDomain(f); z->attachOnDomain(f);}
	void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f); z->detachOnDomain(f);	}

	Rel3<IntersectEqual,Expr1,Expr2,Expr3> getObj()  const
	{ 	return Rel3<IntersectEqual,Expr1,Expr2,Expr3>(x.getObj(),y.getObj(),z.getObj());	}

	Store& store;
	DomView<Set<Elem>,Expr1>	x;
	DomView<Set<Elem>,Expr2>	y;
	DomView<Set<Elem>,Expr3>	z;
	INotifiable*	pOwner;
};


// FIXME: not tested at all
template<class Elem,class Expr1,class Expr2,class Expr3>
struct ChkViewRel3<UnionEqual,Set<Elem>,Expr1,Set<Elem>,Expr2,Set<Elem>,Expr3>
{
	typedef typename DomView<Set<Elem>,Expr1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,Expr2>::Dom	DomY;
	typedef typename DomView<Set<Elem>,Expr3>::Dom	DomZ;

	ChkViewRel3(Store& store, const Expr1& p1,const Expr2& p2,
					const Expr3& p3) :
			store(store),x(store,p1),y(store,p2),z(store,p3) {}

	bool isTrue() const	// is it true?
	{
		if (!z->ground() or z->card().value()>x->card().max()+y->card().max())
			return false;

		return !makeSymDiffIt(makeUnionIt(makeInIt(*x),makeInIt(*y)),makeInIt(*z)).valid() and
			   !makeDiffIt(makePossIt(*x),makeInIt(*z)).valid() and
			   !makeDiffIt(makePossIt(*y),makeInIt(*z)).valid();
	}

	bool canBeTrue() const 	// can it still be true?
	{
		// if (in(x) U in(y)) C lub(z)  and
		//    in(z) C (lub(x) U lub(y))

		return z->card().min() <= x->card().max()+y->card().max() and
				!makeDiffIt(makeUnionIt(makeInIt(*x),makeInIt(*y)),makeLUBIt(*z)).valid() and
				!makeDiffIt(makeInIt(*z),makeUnionIt(makeLUBIt(*x),makeLUBIt(*y))).valid();
	}
	bool setToTrue()
	{
		detach(pOwner);
		return store.post(unionEqual(x.getObj(),y.getObj(),z.getObj()));
	}
	bool setToFalse()
	{	throw Exception::UndefinedFilter("not unionEqual(set,set,set)");	}

	void attach(INotifiable* f)
	{ 	pOwner=f; x->attachOnDomain(f); y->attachOnDomain(f); z->attachOnDomain(f);}
	void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f); z->detachOnDomain(f);	}

	Rel3<UnionEqual,Expr1,Expr2,Expr3> getObj()  const
	{ 	return Rel3<UnionEqual,Expr1,Expr2,Expr3>(x.getObj(),y.getObj(),z.getObj());	}

	Store& 	store;
	DomView<Set<Elem>,Expr1>	x;
	DomView<Set<Elem>,Expr2>	y;
	DomView<Set<Elem>,Expr3>	z;
	INotifiable*	pOwner;
};

// WARNING: not tested
// FIXME: this should be incremental (i.e. use deltas)
template<class Elem,class Expr1>
struct ChkViewRel1<Partition,Seq<Set<Elem> >,Expr1>
{
	ChkViewRel1(Store& store, const Expr1& p1) :
			store(store),x(store,p1) {}
	bool isTrue() const // is it true if no 'lub' set intersect with another 'lub' set
	{
		if (!canBeTrue())
			return false;
		for (uint i = 0; i < x.size(); ++i)
			for (uint j = i+1; j < x.size(); ++j)
				if (Util::intersect(x[i]->beginPoss(),x[i]->endPoss(),
								    x[j]->beginPoss(),x[j]->endPoss()))
					return false;
		for (uint i = 0; i < x.size(); ++i)
			for (uint j = 0; j < x.size(); ++j)
				if (i!=j)
					if (Util::intersect(x[i]->beginIn(),x[i]->endIn(),
										x[j]->beginPoss(),x[j]->endPoss()))
						return false;
		return true;
	}

	bool canBeTrue() const 	// it can be true if no two 'glb' sets intersect
	{
		for (uint i = 0; i < x.size(); ++i)
			for (uint j = i+1; j < x.size(); ++j)
				if (Util::intersect(x[i]->beginIn(),x[i]->endIn(),
								    x[j]->beginIn(),x[j]->endIn()))
					return false;
		return true;
	}
	bool setToTrue()
	{
		detach(pOwner);
		return store.post(partition(x.getObj()));
	}
	bool setToFalse()
	{
		detach(pOwner);
		throw Exception::UndefinedFilter("not Partition(sets)");
	}

	void attach(INotifiable* f) { 	pOwner=f; x.attach(f); }
	void detach(INotifiable* f) {	x.detach(f); }

	Rel1<Disjoint,Expr1> getObj()  const
	{ 	return Rel1<Disjoint,Expr1>(x.getObj());	}

	Store&	store;
	DomArrayView<Set<Elem>,Expr1> x;
	INotifiable*	pOwner;
};

} // CP
} // Casper

#endif /* CASPER_SET_CHKVIEW_H_ */
