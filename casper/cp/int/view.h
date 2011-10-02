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

	void attach(INotifiable* f) { 	d.attachOnBounds(f); }
	void detach(INotifiable* f) {	d.detachOnBounds(f); }

	// temp
	typedef Var<Eval,FD<S,E,T> > Viewed;
	Var<Eval,FD<S,E,T> > getObj()  const
	{	return Var<Eval,FD<S,E,T> >(store,&d); }

	Store&	store;
	FD<S,E,T>&	d;
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

	void attach(INotifiable* f) { 	d.attachOnGround(f); }
	void detach(INotifiable* f) {	d.detachOnGround(f); }

	Var<Eval,FD<S,E,T> > getObj()  const
	{ return Var<Eval,FD<S,E,T> >(store,&d); }

	Store&	store;
	FD<S,E,T>&	d;
};


/**
 *	BndView over integer division.
 *	\ingroup Views
 **/
template<class Expr1,class Expr2>
struct BndViewRel2<Div,Expr1,Expr2,int>
{
	BndViewRel2(Store& store, const Expr1& p1, const Expr2& p2) :
		p1(store,p1),p2(store,p2) {}

	int min() const;
	int max() const;
	void range(int& min, int& max) const;

	// these could be better ...
	bool updateMin(const int& m) { return updateRange(m,max());}
	bool updateMax(const int& m) { return updateRange(min(),m);}

	bool updateRange(const int& min, const int& max);

	void attach(INotifiable* f) { 	p1.attach(f); p2.attach(f);	}
	void detach(INotifiable* f) {	p1.detach(f); p2.detach(f);	}

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

// Integer version
// NOTE: lb and ub are also IN parameters (will be used for pruning
// in cases the division evaluates to two intervals)
// NOTE: implementation always assume that 0 will be eventually removed from
// denominator, except in the case ?/0 in which it will raise an exception
struct IDivRange
{
	template<class P1,class P2>
	void operator()(const P1& p1, const P2& p2, int& lb, int& ub)
	{
		using Util::isPos;
		using Util::isNeg;
		using Util::idivLb;
		using Util::idivUb;

		//const int zero = 0;

		int a1,a2,b1,b2;

		p1.range(a1,a2);
		p2.range(b1,b2);

		if (b1==0)
			if (b2==0)
				throw Exception::DivisionByZero();
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

template<class Expr1,class Expr2>
int BndViewRel2<Div,Expr1,Expr2,int>::min() const
{
	using Util::idivUb;
	using Util::idivLb;
	using Util::isNeg;
	using Util::isPos;

	//const int zero = static_cast<int>(0);

	int a1,a2,b1,b2;

	p1.range(a1,a2);
	p2.range(b1,b2);

	if (b1==0)
		if (b2==0)
			throw Exception::DivisionByZero();
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
	return limits<int>::min();
}

template<class Expr1,class Expr2>
int BndViewRel2<Div,Expr1,Expr2,int>::max() const
{
	using Util::idivUb;
	using Util::idivLb;
	using Util::isNeg;
	using Util::isPos;

	//const int zero = static_cast<int>(0);

	int a1,a2,b1,b2;

	p1.range(a1,a2);
	p2.range(b1,b2);

	if (b1==0)
		if (b2==0)
			throw Exception::DivisionByZero();
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
	return limits<int>::max();
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
	Detail::mulRange(p2,r,l1,u1);

	int restUb = std::min(
					std::max(std::abs(p1.min()),std::abs(p1.max())),
					std::max(std::abs(p2.min()),std::abs(p2.max()))-1);

	if (!p1.updateRange(l1,u1+restUb))
		return false;

	int l2,u2;
	p2.range(l2,u2);

	// try to remove 0 from p2 if possible
	if (l2 == 0 and !p2.updateMin(l2+1))
		return false;
	if (u2 == 0 and !p2.updateMax(l2-1))
		return false;

	if (r.min()*r.max()>0)
		Detail::idivRange(Util::StdRange<int>(p1.min()-restUb,p1.max()),r,l2,u2);

	if (!p2.updateRange(l2,u2))
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
	void detach(INotifiable* f)	{	p1.detach(f); p2.detach(f);	}
	Rel2<Div,Expr1,Expr2>	getObj() const
	{	return Rel2<Div,Expr1,Expr2>(p1.getObj(),p2.getObj());}

	ValView<View1Eval,Expr1>	p1;
	ValView<View2Eval,Expr2>	p2;
};

} // CP
} // Casper

#endif // _H_CASPER_CP_INT_VIEW

