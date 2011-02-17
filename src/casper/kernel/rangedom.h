 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_RANGEDOM
#define _H_CASPER_KERNEL_RANGEDOM

#include <casper/kernel/solver.h>
#include <casper/kernel/safeop.h>
#include <casper/kernel/io.h>
#include <casper/kernel/container/range.h>

#include <iterator>

namespace casper {

// range domain
template<class T>
struct RangeDom
{
	typedef T	Value;
	typedef T	Size;

	// Constructors
	RangeDom(ICPSolver& solver) : mSolver(solver),
							  lb(solver,limits<Value>::negInf()),
							  ub(solver,limits<Value>::posInf()),
							  groundSL(solver),
							  boundsSL(solver) {}

	RangeDom(ICPSolver& solver, const Value& v) :
							  mSolver(solver),
							  lb(solver,v),ub(solver,v),
							  groundSL(solver),
							  boundsSL(solver)  {}

  	template<class T1>
  	RangeDom(ICPSolver& solver, const T1& v) :
  							  mSolver(solver),
  							  lb(solver,detail::convLb<Value>(v)),
  							  ub(solver,detail::convUb<Value>(v)),
  							  groundSL(solver),
							  boundsSL(solver)  {}

  	RangeDom(ICPSolver& solver, const Value& l, const Value& u) :
  							  mSolver(solver),lb(solver,l),ub(solver,u),
  							  groundSL(solver),
							  boundsSL(solver)  {}

  	template<class T1, class T2>
  	RangeDom(ICPSolver& solver, const T1& l, const T2& u) :
  							  mSolver(solver),
  							  lb(solver,detail::convLb<Value>(l)),
  							  ub(solver,detail::convUb<Value>(u)),
  							  groundSL(solver),
							  boundsSL(solver)  {}

  	RangeDom(const RangeDom& r) :
  							mSolver(r.solver()),
  							lb(r.lb),ub(r.ub),
  							groundSL(solver),
							boundsSL(solver)  {}

  	template<class T1>
  	RangeDom(const RangeDom<T1>& r) :
							mSolver(r.solver()),
							lb(solver,detail::convLb<Value>(r.lb.get())),
  							ub(solver,detail::convUb<Value>(r.ub.get())),
  							groundSL(solver),
							boundsSL(solver)  {}

	// Operators

 /* 	const RangeDom& operator=(const Value& v)
  	{	}

  	template<class T1>
  	const RangeDom& operator=(const T1& v)
  	{	return ((Super&)*this) = v;	}

  	const RangeDom& operator=(const RangeDom& s)
  	{	return ((Super&)*this) = s;	}


  	template<class T1>
  	const RangeDom& operator=(const RangeDom<T1>& r)
  	{	return ((Super&)*this) = r;	}
 */
  	// Other

	ICPSolver& solver() const {	return mSolver;	}

	const Value&	min() const { return lb;	}
	const Value&	max() const { return ub;	}

	//Reversible<Value>&	min() { return lb;	}
	//Reversible<Value>&	max() { return ub;	}
	Size	size() const {	return ub.get()-lb.get(); }
	Bool 	singleton() const
	{ return lb == ub; }

	Bool 	canonical() const
	{ return detail::isCanonical(lb.get(),ub.get()); }

	Bool 	ground() const
	{ return singleton() or canonical(); }

	Bool 	empty() const { return lb > ub; }

	Bool updateMin(const Value& v);
	Bool updateMax(const Value& v);
	Bool updateRange(const Value& min,const Value& max);

	/// Registers notifiable \a f on groundness event
	Void attachOnGround(INotifiable*	f) { groundSL.attach(f); }

	/// Registers notifiable \a f on bound update event
	Void attachOnBounds(INotifiable*	f) { boundsSL.attach(f); }

	/// Unregisters notifiable \a f from groundness event
	Void detachOnGround(INotifiable*	f) { groundSL.detach(f); }

	/// Unregisters notifiable \a f from bound update event
	Void detachOnBounds(INotifiable*	f) { boundsSL.detach(f); }

	/// Access the lower bound of the last erased range (delta).
	Value	getLastMin()	const { return lastMin;	}

	/// Access the upper bound of the last erased range (delta).
	Value	getLastMax()	const { return lastMax;	}

	private:
	ICPSolver& mSolver;
	Reversible<Value>	lb;
	Reversible<Value>	ub;

	Bool triggerAfterUpdate();

	Value				lastMin;
	Value				lastMax;

	EventSuspList			groundSL;
	EventSuspList			boundsSL;

//	friend std::ostream& operator<< <>(std::ostream& os, const Interval& f);
};

template<class T>
Bool RangeDom<T>::updateMin(const T& v)
{
	assert(!isnan(v));
	if (v <= min())	return true;
	lastMin = min();
	lb = v;
	if (empty()) return false;
	return triggerAfterUpdate();
}

template<class T>
Bool RangeDom<T>::updateMax(const T& v)
{
	assert(!isnan(v));
	if (v >= max())	return true;
	lastMax = max();
	ub = v;
	if (empty()) return false;
	return triggerAfterUpdate();
}

template<class T>
Bool RangeDom<T>::updateRange(const Value& l,const Value& u)
{
	assert(!isnan(l) and !isnan(u));
	assert(u >= l);
	if (l <= min())
		return updateMax(u);
	if (u >= max())
		return updateMin(l);
	lastMin = min();
	lastMax = max();
	lb = l;
	ub = u;
	return triggerAfterUpdate();
}

template<class T>
Bool RangeDom<T>::triggerAfterUpdate()
{
	assert(!empty());

	//solver().signalDomainUpdate();
	if (singleton())
		if (!groundSL.notifyAll() || empty())
			return false;
	assert(lastMin < lb || lastMax > ub);
	const Double modRatio = 1.0E-3;
	if ((lb-lastMin)/(ub-lb)<modRatio and	// TODO: test
		(lastMax-ub)/(ub-lb)<modRatio)
		return true;
	return (boundsSL.notifyAll() && !empty());
}

// printing RangeDom
template<class T>
std::ostream& operator<<(std::ostream& os,
						 const casper::RangeDom<T>& i)
{	return casper::detail::RangePrinter<T,casper::RangeDom<T> >()(os,i);	}

template<class T>
struct DefaultDomType;

template<>
struct DefaultDomType<Double>
{	typedef RangeDom<Double>	Type;	};

namespace detail {

template<class> struct VarDomCreator;

// support for variables of RangeDom domain
template<class T>
struct VarDomCreator<RangeDom<T> >
{
	RangeDom<T>* operator()(ICPSolver& solver)
	{
		RangeDom<T>* ret = new (solver.heap()) RangeDom<T>(solver);
		solver.signalNewDomain(ret);
		return ret;
	}

	RangeDom<T>* operator()(ICPSolver& solver,
							const T& min, const T& max)
	{
		assert(min <= max);
		RangeDom<T>* ret = new (solver.heap()) RangeDom<T>(solver,min,max);
		solver.signalNewDomain(ret);
		return ret;
	}

	RangeDom<T>* operator()(ICPSolver& solver, const T& i)
	{
		RangeDom<T>* ret = new (solver.heap()) RangeDom<T>(solver,i);
		solver.signalNewDomain(ret);
		return ret;
	}

	template<class T1,class T2>
	RangeDom<T>* operator()(ICPSolver& solver,
							const T1& min, const T2& max)
	{
		assert(min <= max);
		RangeDom<T>* ret = new (solver.heap()) RangeDom<T>(solver,min,max);
		solver.signalNewDomain(ret);
		return ret;
	}

};
};

template<class Eval1,class Eval2> struct BndBiew;

// bndView from a (constant) range
template<class Eval1,class Eval2>
struct BndView<Eval1,Range<Eval2> >
{
	BndView(ICPSolver& solver, const Range<Eval2>& v) :
		v(v) {}
	Eval1 min() const { return detail::convLb<Eval1>(v.min()); }
	Eval1 max() const { return detail::convUb<Eval1>(v.max()); }
	Void range(Eval1& lb,Eval1& ub) const
	{
		lb = detail::convLb<Eval1>(v.min());
		ub = detail::convUb<Eval1>(v.max());
	}

	Bool updateMin(const Eval1& val)
	{ return val <= min();	}
	Bool updateMax(const Eval1& val)
	{ return val >= max();	}
	Bool updateRange(const Eval2& lb,const Eval2& ub)
	{ return ub >= max() and lb <= min();	}
	void attach(INotifiable* f) { 	}
	void detach(INotifiable* f) {	}
	Range<Eval2> getView() const	{ return v;	}

	Range<Eval2> v;
};

template<class Eval1,class Eval2>
struct BndView<Eval1,DomVar<Eval2,RangeDom<Eval2> > >
{
	BndView(ICPSolver& solver, const DomVar<Eval2,RangeDom<Eval2> >& v) :
		v(v) {}
	Eval1 min() const { return detail::convLb<Eval1>(v.domain().min()); }
	Eval1 max() const { return detail::convUb<Eval1>(v.domain().max()); }
	Void range(Eval2& lb,Eval2& ub) const
	{	lb = min(); ub = max(); }
	Bool updateMin(const Eval1& val)
	{ return v.domain().updateMin(detail::convLb<Eval2>(val)); }
	Bool updateMax(const Eval1& val)
	{ return v.domain().updateMax(detail::convUb<Eval2>(val)); }
	Bool updateRange(const Eval2& lb,const Eval2& ub)
	{ return v.domain().updateRange(lb,ub);	}
	ICPSolver& solver() const
	{	return v.solver();	}
	void attach(INotifiable* f) { 	v.domain().attachOnBounds(f); }
	void detach(INotifiable* f) {	v.domain().detachOnBounds(f); }
	DomVar<Eval2,RangeDom<Eval2> >  getView() const	{ return v;	}

	DomVar<Eval2,RangeDom<Eval2> >	v;
};

};

#endif /*_H_CASPER_REAL_INTERVAL */
