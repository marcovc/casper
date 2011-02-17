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

#ifndef _H_CASPER_INT_EXPRESSION
#define _H_CASPER_INT_EXPRESSION

#include <casper/kernel/variable.h>
#include <casper/kernel/view.h>
#include <casper/int/fd.h>

namespace casper {

template<class S,class E,class T,class Eval>
struct BndView<Eval,DomVar<Eval,FD<S,E,T> > >
{
	BndView(ICPSolver& solver, const DomVar<Eval,FD<S,E,T> >& v) :
		d(v.domain()) {}
	Eval min() const { return d.min(); }
	Eval max() const { return d.max(); }
	Bool updateMin(const Eval& val)
	{ return d.updateMin(val); }
	Bool updateMax(const Eval& val)
	{ return d.updateMax(val); }
	Bool updateRange(const Eval& min,const Eval& max)
	{ return d.updateRange(min,max);	}
	Void range(Eval& lb, Eval& ub) const
	{ lb = d.min(); ub = d.max();	}
	ICPSolver& solver() const	{	return d.solver();	}

	void attach(INotifiable* f) { 	d.attachOnBounds(f); }
	void detach(INotifiable* f) {	d.detachOnBounds(f); }

	// temp
	typedef DomVar<Eval,FD<S,E,T> > Viewed;
	DomVar<Eval,FD<S,E,T> > getView()  const
	{	return DomVar<Eval,FD<S,E,T> >(d.solver(),&d); }

	FD<S,E,T>&	d;
};

template<class Eval,class S,class E,class T>
struct ValView<Eval,DomVar<Eval,FD<S,E,T> > >
{
	ValView(ICPSolver& solver, const DomVar<Eval,FD<S,E,T> >& v) :
		d(v.domain()) {}

	Eval value() const { return d.min(); }
	Bool setValue(const Eval& val)
	{	return d = val;	}

	Bool ground() const { return d.singleton(); }

	void attach(INotifiable* f) { 	d.attachOnGround(f); }
	void detach(INotifiable* f) {	d.detachOnGround(f); }

	DomVar<Eval,FD<S,E,T> > getView()  const
	{ return DomVar<Eval,FD<S,E,T> >(d.solver(),&d); }
	ICPSolver& solver() const	{	return d.solver();	}

	FD<S,E,T>&	d;
};


/**
 *	BndView over integer division.
 *	\ingroup Views
 **/
template<class View1,class View2>
struct BndViewRel2<Div,View1,View2,Int> : IBndExpr<Int>
{
	BndViewRel2(ICPSolver& solver, const View1& p1, const View2& p2) :
		IBndExpr<Int>(solver),
		p1(solver,p1),p2(solver,p2) {}

	Int min() const;
	Int max() const;
	Void range(Int& min, Int& max) const;

	// these could be better ...
	Bool updateMin(const Int& m) { return updateRange(m,max());}
	Bool updateMax(const Int& m) { return updateRange(min(),m);}

	Bool updateRange(const Int& min, const Int& max);

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

	Rel2<Div,View1,View2> getView()  const
	{ 	return Rel2<Div,View1,View2>(p1.getView(),p2.getView());	}

	BndView<Int,View1> 		p1;
	BndView<Int,View2> 		p2;
};

namespace detail {

struct IDivRange;

template<class P1,class P2>
Void idivRange(const P1& p1, const P2& p2, Int& lb, Int& ub)
{	IDivRange()(p1,p2,lb,ub);	}

// Integer version
// NOTE: lb and ub are also IN parameters (will be used for pruning
// in cases the division evaluates to two intervals)
// NOTE: implementation always assume that 0 will be eventually removed from
// denominator, except in the case ?/0 in which it will raise an exception
struct IDivRange
{
	template<class P1,class P2>
	Void operator()(const P1& p1, const P2& p2, Int& lb, Int& ub)
	{
		//const Int zero = 0;

		Int a1,a2,b1,b2;

		p1.range(a1,a2);
		p2.range(b1,b2);

		if (b1==0)
			if (b2==0)
				throw EDivisionByZero();
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
				if (idivLb(a2,b1)>ub)
					ub = idivUb(a2,b2);
				if (idivUb(a2,b2)<lb)
					lb = idivLb(a2,b1);
			}
			else
			if (isPos(a1))	// [+,+] / [-,+]
			{
				if (idivUb(a1,b1)<lb)
					lb = idivLb(a1,b2);
				if (idivLb(a1,b2)>ub)
					ub = idivUb(a1,b1);
			}
		}
	}
};
}

template<class View1,class View2>
Int BndViewRel2<Div,View1,View2,Int>::min() const
{
	using detail::idivUb;
	using detail::idivLb;
	using detail::isNeg;
	using detail::isPos;

	//const Int zero = static_cast<Int>(0);

	Int a1,a2,b1,b2;

	p1.range(a1,a2);
	p2.range(b1,b2);

	if (b1==0)
		if (b2==0)
			throw EDivisionByZero();
		else
			++b1;
	else
	if (b2==0)
		--b2;

	if (isPos(b1))	// ? / [+,+]
	{
		if (!isNeg(a1))	// [0+,+] / [+,+]
			return idivLb(a1,b2);
		else
		if (!isPos(a2))	// [-,-0] / [+,+]
			return idivLb(a1,b1);
		else
			return idivLb(a1,b1);	// [-,+] / [+,+]
	}
	else
	if (isNeg(b2))	// ? / [-,-]
	{
		if (!isNeg(a1))	// [0+,+] / [-,-]
			return idivLb(a2,b2);
		else
		if (!isPos(a2))	// [-,-0] / [-,-]
			return idivLb(a2,b1);
		else
			return idivLb(a2,b2);	// [-,+] / [-,-]
	}
	return limits<Int>::min();
}

template<class View1,class View2>
Int BndViewRel2<Div,View1,View2,Int>::max() const
{
	using detail::idivUb;
	using detail::idivLb;
	using detail::isNeg;
	using detail::isPos;

	//const Int zero = static_cast<Int>(0);

	Int a1,a2,b1,b2;

	p1.range(a1,a2);
	p2.range(b1,b2);

	if (b1==0)
		if (b2==0)
			throw EDivisionByZero();
		else
			++b1;
	else
	if (b2==0)
		--b2;

	if (isPos(b1))	// ? / [+,+]
	{
		if (!isNeg(a1))	// [0+,+] / [+,+]
			return idivUb(a2,b1);
		else
		if (!isPos(a2))	// [-,-0] / [+,+]
			return idivUb(a2,b2);
		else
			return idivUb(a2,b1);	// [-,+] / [+,+]
	}
	else
	if (isNeg(b2))	// ? / [-,-]
	{
		if (!isNeg(a1))	// [0+,+] / [-,-]
			return idivUb(a1,b1);
		else
		if (!isPos(a2))	// [-,-0] / [-,-]
			return idivUb(a1,b2);
		else
			return idivUb(a1,b2);	// [-,+] / [-,-]
	}
	return limits<Int>::max();
};

template<class View1,class View2>
Void BndViewRel2<Div,View1,View2,Int>::range(Int& lb, Int& ub) const
{
	lb = limits<Int>::negInf();
	ub = limits<Int>::posInf();
	detail::idivRange(p1,p2,lb,ub);
}

template<class View1,class View2>
Bool BndViewRel2<Div,View1,View2,Int>::updateRange(const Int& lb,
												const Int& ub)
{
	Range<Int> r(lb,ub);
	if (lb>max() or ub<min())
		return false;

	Int l1,u1;
	detail::mulRange(p2,r,l1,u1);

	Int restUb = std::min(
					std::max(std::abs(p1.min()),std::abs(p1.max())),
					std::max(std::abs(p2.min()),std::abs(p2.max()))-1);

	if (!p1.updateRange(l1,u1+restUb))
		return false;

	Int l2,u2;
	p2.range(l2,u2);

	// try to remove 0 from p2 if possible
	if (l2 == 0 and !p2.updateMin(l2+1))
		return false;
	if (u2 == 0 and !p2.updateMax(l2-1))
		return false;

	if (r.min()*r.max()>0)
		detail::idivRange(Range<Int>(p1.min()-restUb,p1.max()),r,l2,u2);

	if (!p2.updateRange(l2,u2))
		return false;

	return true;
}

};

#endif

