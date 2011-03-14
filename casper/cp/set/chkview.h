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

namespace Casper {
namespace CP {

template<class Elem,class View1,class View2>
struct ChkViewRel2<Member,Elem,View1,Set<Elem>,View2>
{
	ChkViewRel2(Store& store, const View1& p1,const View2& p2) :
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

	Rel2<Member,View1,View2> getObj()  const
	{ 	return Rel2<Member,View1,View2>(elem.getObj(),set.getObj());	}

	Store&		store;
	ValView<Elem,View1>	elem;
	DomView<Set<Elem>,View2>	set;
	INotifiable*	pOwner;
};



template<class Elem,class View1,class View2>
struct ChkViewRel2<NotMember,Elem,View1,Set<Elem>,View2>
{
	ChkViewRel2(Store& store, const View1& p1,const View2& p2) :
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

	Rel2<NotMember,View1,View2> getObj()  const
	{ 	return Rel2<NotMember,View1,View2>(elem.getObj(),set.getObj());	}

	Store&	store;
	ValView<Elem,View1>	elem;
	DomView<Set<Elem>,View2>	set;
	INotifiable*	pOwner;
};


// FIXME: not tested at all
template<class Elem,class View1,class View2>
struct ChkViewRel2<Contained,Set<Elem>,View1,Set<Elem>,View2>
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;

	ChkViewRel2(Store& store, const View1& p1,const View2& p2) :
			store(store),x(store,p1),y(store,p2) {}
	bool isTrue() const	// when lub(x) C glb(y)
	{
		return x->cardMin() <= y->cardMax() and
			   Casper::Util::contained(x->beginIn(),x->endIn(),
								 y->beginIn(),y->endIn()) and
			   Casper::Util::contained(x->beginPoss(),x->endPoss(),
								 y->beginIn(),y->endIn());
	}
	bool canBeTrue() const 	// when glb(x) C lub(y)
	{
		return x->cardMin() <= y->cardMax() and
				!makeDiffIt(makeInIt(*x),makeLUBIt(*y)).valid();
	}
	bool setToTrue()
	{
		detach(pOwner);
		return postBndFilter(store,contained(x.getObj(),y.getObj()));
	}
	bool setToFalse()
	{
		detach(pOwner);
		throw Exception::NoFilter("NotContained(set,set)");
	}

	void attach(INotifiable* f)
	{ 	pOwner=f; x->attachOnDomain(f);	y->attachOnDomain(f);}
	void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f);}

	Rel2<Contained,View1,View2> getObj()  const
	{ 	return Rel2<Contained,View1,View2>(x.getObj(),y.getObj());	}

	Store&	store;
	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	INotifiable*	pOwner;
};


// FIXME: not tested at all
template<class Elem,class View1,class View2>
struct ChkViewRel2<Equal,Set<Elem>,View1,Set<Elem>,View2>
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;

	ChkViewRel2(Store& store, const View1& p1,const View2& p2) :
			store(store),x(store,p1),y(store,p2),xCy(store,p1,p2),yCx(store,p2,p1) {}
	bool isTrue() const	// when lub(x) = lub(y)
	{
		return x->ground() and y->ground() and
			   Util::equal(x->beginIn(),x->endIn(),
							 y->beginIn(),y->endIn());
	}
	bool canBeTrue() const 	// when glb(x) C lub(y) and glb(y) C lub(x)
	{	return xCy.canBeTrue() and yCx.canBeTrue();	}
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

	Rel2<Equal,View1,View2> getObj()  const
	{ 	return Rel2<Equal,View1,View2>(x.getObj(),y.getObj());	}

	Store&	store;
	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	INotifiable*	pOwner;
	ChkViewRel2<Contained,Set<Elem>,View1,Set<Elem>,View2>	xCy;
	ChkViewRel2<Contained,Set<Elem>,View2,Set<Elem>,View1>	yCx;
};


template<class Elem,class View1,class View2>
struct ChkViewRel2<Disjoint,Set<Elem>,View1,Set<Elem>,View2>
{
	ChkViewRel2(Store& store, const View1& p1,const View2& p2) :
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
		Var<IntSet> aux(store(),lub3);
		return store.post(intersect(x.getObj(),y.getObj(),aux) and
							 cardinal(aux)>0);
	}

	void attach(INotifiable* f) { 	pOwner=f; x->attachOnDomain(f); y->attachOnDomain(f);}
	void detach(INotifiable* f) {	x->detachOnDomain(f); y->detachOnDomain(f);}

	Rel2<Disjoint,View1,View2> getObj()  const
	{ 	return Rel2<Disjoint,View1,View2>(x.getObj(),y.getObj());	}

	Store&	store;
	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	INotifiable*	pOwner;
};


// FIXME: not tested at all
template<class Elem,class View1,class View2,class View3>
struct ChkViewRel3<Intersect,Set<Elem>,View1,Set<Elem>,View2,
					Set<Elem>,View3>
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;
	typedef typename DomView<Set<Elem>,View3>::Dom	DomZ;

	ChkViewRel3(Store& store, const View1& p1,const View2& p2,
					const View3& p3) :
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
		return store.post(intersect(x.getObj(),y.getObj(),z.getObj()));
	}
	bool setToFalse()		// FIXME: using views instead of auxvar
	{	throw Exception::NoFilter("NotIntersect(set,set,set)");	}

	void attach(INotifiable* f)
	{ 	pOwner=f; x->attachOnDomain(f); y->attachOnDomain(f); z->attachOnDomain(f);}
	void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f); z->detachOnDomain(f);	}

	Rel3<Intersect,View1,View2,View3> getObj()  const
	{ 	return Rel3<Intersect,View1,View2,View3>(x.getObj(),y.getObj(),z.getObj());	}

	Store& store;
	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	DomView<Set<Elem>,View3>	z;
	INotifiable*	pOwner;
};


// FIXME: not tested at all
template<class Elem,class View1,class View2,class View3>
struct ChkViewRel3<Union,Set<Elem>,View1,Set<Elem>,View2,Set<Elem>,View3>
{
	typedef typename DomView<Set<Elem>,View1>::Dom	DomX;
	typedef typename DomView<Set<Elem>,View2>::Dom	DomY;
	typedef typename DomView<Set<Elem>,View3>::Dom	DomZ;

	ChkViewRel3(Store& store, const View1& p1,const View2& p2,
					const View3& p3) :
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
		return store.post(union_(x.getObj(),y.getObj(),z.getObj()));
	}
	bool setToFalse()
	{	throw Exception::NoFilter("NotUnion(set,set,set)");	}

	void attach(INotifiable* f)
	{ 	pOwner=f; x->attachOnDomain(f); y->attachOnDomain(f); z->attachOnDomain(f);}
	void detach(INotifiable* f)
	{	x->detachOnDomain(f); y->detachOnDomain(f); z->detachOnDomain(f);	}

	Rel3<Union,View1,View2,View3> getObj()  const
	{ 	return Rel3<Union,View1,View2,View3>(x.getObj(),y.getObj(),z.getObj());	}

	Store& 	store;
	DomView<Set<Elem>,View1>	x;
	DomView<Set<Elem>,View2>	y;
	DomView<Set<Elem>,View3>	z;
	INotifiable*	pOwner;
};


} // CP
} // Casper

#endif /* CASPER_SET_CHKVIEW_H_ */
