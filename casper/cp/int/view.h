 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_CP_INT_VIEW
#define _H_CASPER_CP_INT_VIEW

#include <casper/cp/var.h>
#include <casper/cp/view.h>
#include <casper/cp/int/fd.h>

namespace Casper {
namespace CP {


template<class S,class E,class T,class Eval>
struct BndView<Eval,Var<Eval,FD<S,E,T> > >
{
	BndView(Store& store, const Var<Eval,FD<S,E,T> >& v) :
		store(store),d(v.domain()) {}
	Eval min() const { return d.min(); }
	Eval max() const { return d.max(); }
	bool updateMin(const Eval& val)
	{ return d.updateMin(val); }
	bool updateMax(const Eval& val)
	{ return d.updateMax(val); }
	bool updateRange(const Eval& min,const Eval& max)
	{ return d.updateRange(min,max);	}
	void range(Eval& lb, Eval& ub) const
	{ lb = d.min(); ub = d.max();	}

	void attach(INotifiable* f)
	{	attachLink = d.attachOnBounds(f);	}
	void detach()
	{	d.detachOnBounds(attachLink); }

	// temp
	typedef Var<Eval,FD<S,E,T> > Viewed;
	Var<Eval,FD<S,E,T> > getObj()  const
	{	return Var<Eval,FD<S,E,T> >(store,&d); }

	Store&			store;
	FD<S,E,T>&		d;
	typename FD<S,E,T>::AttachLink	attachLink;
};

template<class Eval,class S,class E,class T>
struct ValView<Eval,Var<Eval,FD<S,E,T> > >
{
	ValView(Store& store, const Var<Eval,FD<S,E,T> >& v) :
		store(store),d(v.domain()) {}

	Eval value() const { return d.min(); }
	bool setValue(const Eval& val)
	{	return d = val;	}

	bool ground() const { return d.singleton(); }

	void attach(INotifiable* f)
	{	attachLink = d.attachOnGround(f);	}
	void detach()
	{	d.detachOnGround(attachLink); }

	Var<Eval,FD<S,E,T> > getObj()  const
	{ return Var<Eval,FD<S,E,T> >(store,&d); }

	Store&	store;
	FD<S,E,T>&	d;
	typename FD<S,E,T>::AttachLink	attachLink;
};


/**
 *	BndView over integer division.
 *	\todo Split in 6 views p|n|z / p|n|z and store a (reversible) pointer to the current case
 *	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct BndViewRel2<Div,Expr1,Expr2,int>
{
	BndViewRel2(Store& store, const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2)
	{
		// FIXME: if this view is used before the filter below is run then
		// it may fail in one of the asserts(p2!=0) below. One possible fix
		// is to attach a demon to p2 to fail when p2=0.
		store.post(p2!=0);
	}

	int min() const;
	int max() const;
	void range(int& min, int& max) const;

	// these could be better ...
	bool updateMin(const int& m) { return updateRange(m,max());}
	bool updateMax(const int& m) { return updateRange(min(),m);}

	bool updateRange(const int& min, const int& max);

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach() {	p1.detach(); p2.detach();	}

	Rel2<Div,Expr1,Expr2> getObj()  const
	{ 	return rel<Div>(p1.getObj(),p2.getObj());	}

	BndView<int,Expr1> 		p1;
	BndView<int,Expr2> 		p2;
};

namespace Detail {

struct IDivRange;

template<class P1,class P2>
void idivRange(const P1& p1, const P2& p2, int& lb, int& ub)
{	IDivRange()(p1,p2,lb,ub);	}

/**
 * Integer division: lb..ub <= p1/p2
 * \note lb and ub are also IN parameters (will be used for pruning
 * in cases the division evaluates to two intervals)
 * \note implementation always assume that 0 will be eventually removed from
 * denominator, except in the case ?/0 in which it will raise an exception
 * \note rounding is always towards 0 as in standard C++
 */
struct IDivRange
{
	template<class P1,class P2>
	void operator()(const P1& p1, const P2& p2, int& lb, int& ub)
	{
		using Util::idivUb;
		using Util::idivLb;
		using Util::isNeg;
		using Util::isPos;

		int a1,a2,b1,b2;

		p1.range(a1,a2);
		p2.range(b1,b2);

		if (b1==0)
			if (b2==0)
			{
				lb = limits<int>::posInf();
				ub = limits<int>::negInf();
				return;
			}
			else
				++b1;
		else
		if (b2==0)
			--b2;

		if (isPos(b1))	// ? / [+,+]
		{
			if (!isNeg(a1))	// [0+,+] / [+,+]
			{
				lb = idivLb(a1,b2);
				ub = idivUb(a2,b1);
			}
			else
			if (!isPos(a2))	// [-,-0] / [+,+]
			{
				lb = idivLb(a1,b1);
				ub = idivUb(a2,b2);
			}
			else
			{				// [-,+] / [+,+]
				lb = idivLb(a1,b1);
				ub = idivUb(a2,b1);
			}
		}
		else
		if (isNeg(b2))	// ? / [-,-]
		{
			if (!isNeg(a1))	// [0+,+] / [-,-]
			{
				lb = idivLb(a2,b2);
				ub = idivUb(a1,b1);
			}
			else
			if (!isPos(a2))	// [-,-0] / [-,-]
			{
				lb = idivLb(a2,b1);
				ub = idivUb(a1,b2);
			}
			else
			{				// [-,+] / [-,-]
				lb = idivLb(a2,b2);
				ub = idivUb(a1,b2);
			}
		}
		else	// ? / [-,+]
		{
			if (isNeg(a2))	// [-,-] / [-,+]
			{
				if (a2/b1>ub)
					ub = idivUb(a2,b2);
				else
					ub = -a1;
				if (a2/b2<lb)
					lb = idivLb(a2,b1);
				else
					lb = a1;
			}
			else
			if (isPos(a1))	// [+,+] / [-,+]
			{
				if (a1/b1<lb)
					lb = idivLb(a1,b2);
				else
					lb = -a2;
				if (a1/b2>ub)
					ub = idivUb(a1,b1);
				else
					ub = a2;
			}
			else	// [-,+] / [-,+]
			{
				lb = std::min(a1,-a2);
				ub = std::max(a2,-a1);
			}
		}
	}
};
}

template<class Expr1,class Expr2>
int BndViewRel2<Div,Expr1,Expr2,int>::min() const
{
	using Util::idivUb;
	using Util::idivLb;
	using Util::isNeg;
	using Util::isPos;

	int a1,a2,b1,b2;

	p1.range(a1,a2);
	p2.range(b1,b2);

	if (b1==0)
		if (b2==0)
			limits<int>::posInf();	// force fail
		else
			++b1;
	else
	if (b2==0)
		--b2;

	if (isPos(b1))	// ? / [+,+]
	{
		if (isPos(a1))	// [+,+] / [+,+]
			return idivLb(a1,b2);
		else			// [-,+] / [+,+]
			return idivLb(a1,b1);
	}
	else
	if (isNeg(b2))	// ? / [-,-]
	{
		if (isNeg(a2))	// [-,-] / [-,-]
			return idivLb(a2,b1);
		else			// [-,+] / [-,-]
			return idivLb(a2,b2);
	}
	else	// ? / [-,+]
	{
		if (!isNeg(a1)) // [0+,+] / [-,+]
			return -a2;
		else
		if (!isPos(a2)) // [-,-0] / [-,+]
			return a1;
		else
			return std::min(a1,-a2);
	}
	assert(0);
}

template<class Expr1,class Expr2>
int BndViewRel2<Div,Expr1,Expr2,int>::max() const
{
	using Util::idivUb;
	using Util::idivLb;
	using Util::isNeg;
	using Util::isPos;

	int a1,a2,b1,b2;

	p1.range(a1,a2);
	p2.range(b1,b2);

	if (b1==0)
		if (b2==0)
			limits<int>::negInf();	// force fail
		else
			++b1;
	else
	if (b2==0)
		--b2;

	if (isPos(b1))	// ? / [+,+]
	{
		if (isNeg(a2))	// [-,-] / [+,+]
			return idivUb(a2,b2);
		else			// [0+,+] / [+,+] or [-,+] / [+,+]
			return idivUb(a2,b1);
	}
	else
	if (isNeg(b2))	// ? / [-,-]
	{
		if (isPos(a1))	// [+,+] / [-,-]
			return idivUb(a1,b1);
		else			// [-,-0] / [-,-] or [-,+] / [-,-]
			return idivUb(a1,b2);
	}
	else	// ? / [-,+]
	{
		if (!isNeg(a1)) // [0+,+] / [-,+]
			return a2;
		else
		if (!isPos(a2)) // [-,-0] / [-,+]
			return -a1;
		else
			return std::max(a2,-a1);
	}
	assert(0);
};

template<class Expr1,class Expr2>
void BndViewRel2<Div,Expr1,Expr2,int>::range(int& lb, int& ub) const
{
	lb = limits<int>::negInf();
	ub = limits<int>::posInf();
	Detail::idivRange(p1,p2,lb,ub);
}

template<class Expr1,class Expr2>
bool BndViewRel2<Div,Expr1,Expr2,int>::updateRange(const int& lb,
												   const int& ub)
{
	Util::StdRange<int> r(lb,ub);
	if (lb>max() or ub<min())
		return false;

	int l1,u1;
	p1.range(l1,u1);

	int l2,u2;
	p2.range(l2,u2);

	int nl1 = l1;
	int nu1 = u1;
	Detail::mulRange(p2,r,nl1,nu1);

	int nl2 = l2;
	int nu2 = u2;

//	std::cout << p1 << "/" << p2 << "=" << r << std::endl;
//	std::cout << "n1=" << nl1 << ".." << nu1 << std::endl;

	// find upper bound on the remainder of the division
	int rest = std::min(std::max(std::abs(l1),std::abs(u1)),
						std::max(std::abs(l2),std::abs(u2))-1);

//	std::cout << "rest=" << rest << std::endl;

	// if the remainder is non negative
	if (l1>=0 or (lb>0 and l2>0) or (ub<0 and u2<0))
	{
		if (!p1.updateRange(nl1,std::max(nu1,nu1+rest)))
			return false;
		if (lb>0 or ub<0)
		{
			Detail::idivRange(Util::StdRange<int>(std::min(nl1-rest,nl1),nu1),r,nl2,nu2);
			if (!p2.updateRange(nl2,nu2))
				return false;
		}
	}
	else
	// if the remainder is non positive
	if (u1<=0 or (lb>0 and u2<0) or (ub<0 and l2>0))
	{
		if (!p1.updateRange(std::min(nl1,nl1-rest),nu1))
			return false;
		if (lb>0 or ub<0)
		{
			Detail::idivRange(Util::StdRange<int>(nl1,std::max(nu1,nu1+rest)),r,nl2,nu2);
			if (!p2.updateRange(nl2,nu2))
				return false;
		}
	}
	else
	// if the remainder may be positive or negative
	{
		nl1 = std::min(nl1,nl1-rest); // to prevent wrap around -inf
		nu1 = std::max(nu1,nu1+rest); // to prevent wrap around +inf
		if (!p1.updateRange(nl1,nu1))
			return false;
		if (lb>0 or ub<0)
		{
			Detail::idivRange(Util::StdRange<int>(nl1,nu1),r,nl2,nu2);
			if (!p2.updateRange(nl2,nu2))
				return false;
		}
	}

	// try to remove 0 from p2 if possible
	if (l2 == 0 and !p2.updateMin(l2+1))
		return false;
	if (u2 == 0 and !p2.updateMax(u2-1))
		return false;

	return true;
}


/**
 * 	ValView over integer division.
 * 	\ingroup Views
 **/
template<class Expr1,class Expr2,class Eval>
struct ValViewRel2<Div,Expr1,Expr2,Eval>
{
	typedef typename Casper::Traits::GetEval<Expr1>::Type	View1Eval;
	typedef typename Casper::Traits::GetEval<Expr2>::Type	View2Eval;

	ValViewRel2(Store& store, const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}
	Eval value() const { return p1.value() / p2.value(); }
	bool ground() const { return p1.ground() and p2.ground(); }
	bool setValue(const Eval& val)
	{
		if (p1.ground())
			return p2.setValue(p1.value() / val);
		if (p2.ground())
			return p1.setValue(val * p2.value());
		return true;
	}
	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach()	{	p1.detach(); p2.detach();	}
	Rel2<Div,Expr1,Expr2>	getObj() const
	{	return Rel2<Div,Expr1,Expr2>(p1.getObj(),p2.getObj());}

	ValView<View1Eval,Expr1>	p1;
	ValView<View2Eval,Expr2>	p2;
};

/**
 * 	BndView over a int->bool cast expression.
 * 	\note We follow the same semantics of the C/C++ programming language:
 * 	From bool to int, false is always 0, true is always 1.	<== implemented below
 * 	From int to bool, 0 is always false, anything other than 0 is true. (see ChkView<Cast<bool> >)
 * 	\ingroup BndViews
 */
template<class View>
struct BndViewCast<bool,int,View>
{
	BndViewCast(Store& store, const View& v) :
		v(store,v) {}

	int min() const
	{	return (int) v.min();	}
	int max() const
	{	return (int) v.max();	}
	bool updateMin(const int& val)
	{	return val<1 or (val==1 and v.updateMin(true));	}
	bool updateMax(const int& val)
	{	return val>0 or (val==0 and v.updateMax(false));	}
	void range(int& v1,int& v2) const
	{ v1 = min(); v2=max(); }
	bool updateRange(const int& v1, const int& v2)
	{	return updateMin(v1) and updateMax(v2);	}

	void attach(INotifiable* n) {	v.attach(n);	}
	void detach() {	v.detach();	}
	Rel1<Cast<int>,View> getObj()  const
	{ return Rel1<Cast<int>,View>(v.getObj()); }

	BndView<bool,View>	v;
};

/**
 * 	ChkView over the LinearGreater constraint.
 * 	\pre Requires p1 (the coefficients) to be instantiated.
 * 	\ingroup ChkViews
 */
template<class Eval,class Expr1,class Expr2,class Expr3>
struct ChkViewRel3<LinearGreater,Seq<Eval>,Expr1,Seq<Eval>,Expr2,Eval,Expr3> :
	ChkViewRel3<LinearGreaterEqual,Seq<Eval>,Expr1,Seq<Eval>,Expr2,Eval,Rel2<Add,Expr3,int> >
{
	typedef ChkViewRel3<LinearGreaterEqual,Seq<Eval>,Expr1,
						Seq<Eval>,Expr2,Eval,Rel2<Add,Expr3,int> > Super;
	ChkViewRel3(Store& store, const Expr1& p1, const Expr2& p2, const Expr3& p3) :
		Super(store,p1,p2,rel<Add>(p3,1)) {}
	Rel3<LinearGreater,Expr1,Expr2,Expr3> getObj()  const
	{ 	return rel<LinearGreater>(Super::getObj().p1,
								  Super::getObj().p2,
								  Super::getObj().p3.p1);	}
};

/**
 * 	ChkView over the LinearGreater constraint.
 * 	\pre Requires p1 (the coefficients) to be instantiated.
 * 	\ingroup ChkViews
 */
template<class Eval,class Expr1,class Expr2,class Expr3>
struct ChkViewRel3<LinearLess,Seq<Eval>,Expr1,Seq<Eval>,Expr2,Eval,Expr3> :
	ChkViewRel3<LinearLessEqual,Seq<Eval>,Expr1,Seq<Eval>,Expr2,Eval,Rel2<Sub,Expr3,int> >
{
	typedef ChkViewRel3<LinearLessEqual,Seq<Eval>,Expr1,
						Seq<Eval>,Expr2,Eval,Rel2<Sub,Expr3,int> > Super;
	ChkViewRel3(Store& store, const Expr1& p1, const Expr2& p2, const Expr3& p3) :
		Super(store,p1,p2,rel<Sub>(p3,1)) {}
	Rel3<LinearLess,Expr1,Expr2,Expr3> getObj()  const
	{ 	return rel<LinearLess>(Super::getObj().p1,
							   Super::getObj().p2,
							   Super::getObj().p3.p1);	}
};


/**
 * 	Enforces the seq membership constraint.
 */
template<class Expr1,class Elem,class Expr2>
struct ChkViewRel2<Member,Elem,Expr1,Seq<Elem>,Expr2>
{
	ChkViewRel2(Store& s,const Expr1& elem, const Expr2& set) :
		store(s),elem(s,elem),seq(s,seq) {}

	bool isTrue() const	// is it true?
	{	return !makeDiffIt(makeIt(*elem),makeIt(seq)).valid();	}
	bool canBeTrue() const 	// can it still be true?
	{	return makeInterIt(makeIt(*elem),makeIt(seq)).valid();	}
	bool setToTrue()
	{
		detach();
		return store.post(getObj());
	}
	bool setToFalse()
	{	throw Exception::UndefinedFilter("not member(elem,seq)");	}
	//Store& store() const {	return mSolver;	}

	void attach(INotifiable* f) { elem->attach(f);	}
	void detach() {	elem->detach(); }

	bool getObj()  const
	{ 	return member(elem.getObj(),seq);	}

	Store& store;
	DomView<Elem,Expr1>	elem;
	Expr2	seq;
};

} // CP
} // Casper

#endif // _H_CASPER_CP_INT_VIEW

