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

#ifndef _H_CASPER_CP_RANGEDOM
#define _H_CASPER_CP_RANGEDOM

#include <casper/cp/store.h>
#include <casper/util/safeop.h>
#include <casper/util/io.h>
#include <casper/util/container/stdrange.h>

#include <iterator>

namespace Casper {

namespace LP {
struct Var;
};

namespace CP {

// range domain
template<class T>
struct RangeDom
{
	typedef T	Value;
	typedef T	Size;

	// Constructors
	RangeDom(Env& env) :  	env(env),
							  lb(env,limits<Value>::negInf()),
							  ub(env,limits<Value>::posInf()),
							  groundSL(env),
							  boundsSL(env) {}

	RangeDom(Env& env, const Value& v) :
							  env(env),
							  lb(env,v),ub(env,v),
							  groundSL(env),
							  boundsSL(env)  {}

  	template<class T1>
  	RangeDom(Env& env, const T1& v) :
  							  env(env),
  							  lb(env,Casper::Util::convLb<Value>(v)),
  							  ub(env,Casper::Util::convUb<Value>(v)),
  							  groundSL(env),
							  boundsSL(env)  {}

  	RangeDom(Env& env, const Value& l, const Value& u) :
  							  env(env),lb(env,l),ub(env,u),
  							  groundSL(env),
							  boundsSL(env)  {}

  	template<class T1, class T2>
  	RangeDom(Env& env, const T1& l, const T2& u) :
  							  env(env),
  							  lb(env,Casper::Util::convLb<Value>(l)),
  							  ub(env,Casper::Util::convUb<Value>(u)),
  							  groundSL(env),
							  boundsSL(env)  {}

  	RangeDom(const RangeDom& r) :
  							env(r.getEnv()),
  							lb(r.lb),ub(r.ub),
  							groundSL(env),
							boundsSL(env)  {}

  	template<class T1>
  	RangeDom(const RangeDom<T1>& r) :
  							env(r.getEnv()),
							lb(env,Casper::Util::convLb<Value>(r.lb.get())),
  							ub(env,Casper::Util::convUb<Value>(r.ub.get())),
  							groundSL(env),
							boundsSL(env)  {}

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

	Env& getEnv() const {	return env;	}

	const Value&	min() const { return lb;	}
	const Value&	max() const { return ub;	}

	const Value&	value() const {	assert(singleton());	return lb;	}

	//Reversible<Value>&	min() { return lb;	}
	//Reversible<Value>&	max() { return ub;	}
	Size	size() const {	return ub.get()-lb.get(); }
	bool 	singleton() const
	{ return lb == ub; }

	bool 	canonical() const
	{ return Casper::Util::isCanonical(lb.get(),ub.get()); }

	bool 	ground() const
	{ return singleton() or canonical(); }

	bool 	empty() const { return lb > ub; }

	bool lowerBounded() const {	return lb > limits<T>::negInf();	}
	bool upperBounded() const {	return ub < limits<T>::posInf();	}
	bool bounded() const {	return lowerBounded() and upperBounded();	}

	bool updateMin(const Value& v);
	bool updateMax(const Value& v);
	bool updateRange(const Value& min,const Value& max);

	/// Registers notifiable \a f on groundness event
	void attachOnGround(INotifiable*	f) { groundSL.attach(f); }

	/// Registers notifiable \a f on bound update event
	void attachOnBounds(INotifiable*	f) { boundsSL.attach(f); }

	/// Unregisters notifiable \a f from groundness event
	void detachOnGround(INotifiable*	f) { groundSL.detach(f); }

	/// Unregisters notifiable \a f from bound update event
	void detachOnBounds(INotifiable*	f) { boundsSL.detach(f); }

	/// Access the lower bound of the last erased range (delta).
	Value	getLastMin()	const { return lastMin;	}

	/// Access the upper bound of the last erased range (delta).
	Value	getLastMax()	const { return lastMax;	}

	private:
	Env& 				env;
	Reversible<Value>	lb;
	Reversible<Value>	ub;

	bool triggerAfterUpdate();

	Value				lastMin;
	Value				lastMax;

	EventSuspList			groundSL;
	EventSuspList			boundsSL;

//	friend std::ostream& operator<< <>(std::ostream& os, const Interval& f);
};

template<class T>
bool RangeDom<T>::updateMin(const T& v)
{
	assert(!isnan(v));
	if (v <= min())	return true;
	lastMin = min();
	lb = v;
	if (empty()) return false;
	return triggerAfterUpdate();
}

template<class T>
bool RangeDom<T>::updateMax(const T& v)
{
	assert(!isnan(v));
	if (v >= max())	return true;
	lastMax = max();
	ub = v;
	if (empty()) return false;
	return triggerAfterUpdate();
}

template<class T>
bool RangeDom<T>::updateRange(const Value& l,const Value& u)
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
bool RangeDom<T>::triggerAfterUpdate()
{
	assert(!empty());

	//solver().signalDomainUpdate();
	if (singleton())
		if (!groundSL.notifyAll() || empty())
			return false;
	assert(lastMin < lb || lastMax > ub);
	const double modRatio = 1.0E-3;
	if ((lb-lastMin)/(ub-lb)<modRatio and	// TODO: test
		(lastMax-ub)/(ub-lb)<modRatio)
		return true;
	return (boundsSL.notifyAll() && !empty());
}

// printing RangeDom
template<class T>
std::ostream& operator<<(std::ostream& os,
						 const Casper::CP::RangeDom<T>& i)
{	return Casper::Util::RangePrinter<T,Casper::CP::RangeDom<T> >()(os,i);	}

namespace Traits {
template<class T>
struct GetDefaultDom;

template<>
struct GetDefaultDom<double>
{	typedef RangeDom<double>	Type;	};

template<>
struct GetDefaultDom<float>
{	typedef RangeDom<float>	Type;	};
}

namespace Detail {

template<class> struct VarDomCreator;

// support for variables of RangeDom domain
template<class T>
struct VarDomCreator<RangeDom<T> >
{
	RangeDom<T>* operator()(Store& store)
	{
		RangeDom<T>* ret = new (store) RangeDom<T>(store);
		store.getStats().signalNewRangeDomain();
		return ret;
	}

	RangeDom<T>* operator()(Store& store,
							const T& min, const T& max)
	{
		assert(min <= max);
		RangeDom<T>* ret = new (store) RangeDom<T>(store,min,max);
		store.getStats().signalNewRangeDomain();
		return ret;
	}

	RangeDom<T>* operator()(Store& store, const T& i)
	{
		RangeDom<T>* ret = new (store) RangeDom<T>(store,i);
		store.getStats().signalNewRangeDomain();
		return ret;
	}

	template<class T1,class T2>
	RangeDom<T>* operator()(Store& store,const T1& min, const T2& max)
	{
		assert(min <= max);
		RangeDom<T>* ret = new (store) RangeDom<T>(store,min,max);
		store.getStats().signalNewRangeDomain();
		return ret;
	}
	// returned domain is the union of all domains in the array
	template<class View>
	RangeDom<T>*  unionOf(Store& store, const View& v);

	// this should be elsewhere...
	RangeDom<T>* operator()(Store& store, const LP::Var& v)
	{
		RangeDom<T>* ret = new (store) RangeDom<T>(store,limits<T>::negInf(),limits<T>::posInf());
		store.getStats().signalNewRangeDomain();
		return ret;
	}

};

// returned domain is the union of all domains in the array
template<class T>
template<class View>
RangeDom<T>* VarDomCreator<RangeDom<T> >::unionOf(Store& store, const View& v)
{
	DomArrayView<T,View> doms(store,v);
	T mi = doms[0]->min();
	T ma = doms[0]->max();
	for (uint i = 1; i < doms.size(); i++)
	{
		mi = std::min(mi,doms[i]->min());
		ma = std::max(ma,doms[i]->max());
	}
	return operator()(store,mi,ma);
}

} // Detail

template<class Eval1,class Eval2> struct BndView;

// bndView from a (constant) range
template<class Eval1,class Eval2>
struct BndView<Eval1,Util::StdRange<Eval2> >
{
	BndView(Store& store, const Util::StdRange<Eval2>& v) :
		v(v) {}
	Eval1 min() const { return Util::convLb<Eval1>(v.min()); }
	Eval1 max() const { return Util::convUb<Eval1>(v.max()); }
	void range(Eval1& lb,Eval1& ub) const
	{
		lb = Util::convLb<Eval1>(v.min());
		ub = Util::convUb<Eval1>(v.max());
	}

	bool updateMin(const Eval1& val)
	{ return val <= min();	}
	bool updateMax(const Eval1& val)
	{ return val >= max();	}
	bool updateRange(const Eval2& lb,const Eval2& ub)
	{ return ub >= max() and lb <= min();	}
	void attach(INotifiable* f) { 	}
	void detach(INotifiable* f) {	}
	Util::StdRange<Eval2> getObj() const	{ return v;	}

	Util::StdRange<Eval2> v;
};

template<class Eval1,class Eval2>
struct BndView<Eval1,Var<Eval2,RangeDom<Eval2> > >
{
	BndView(Store& store, const Var<Eval2,RangeDom<Eval2> >& v) :
		v(v) {}
	Eval1 min() const { return Util::convLb<Eval1>(v.domain().min()); }
	Eval1 max() const { return Util::convUb<Eval1>(v.domain().max()); }
	void range(Eval2& lb,Eval2& ub) const
	{	lb = min(); ub = max(); }
	bool updateMin(const Eval1& val)
	{ return v.domain().updateMin(Casper::Util::convLb<Eval2>(val)); }
	bool updateMax(const Eval1& val)
	{ return v.domain().updateMax(Casper::Util::convUb<Eval2>(val)); }
	bool updateRange(const Eval2& lb,const Eval2& ub)
	{ return v.domain().updateRange(lb,ub);	}

	void attach(INotifiable* f) { 	v.domain().attachOnBounds(f); }
	void detach(INotifiable* f) {	v.domain().detachOnBounds(f); }
	Var<Eval2,RangeDom<Eval2> >  getObj() const	{ return v;	}

	Var<Eval2,RangeDom<Eval2> >	v;
};

} // CP
} // Casper

#endif /*_H_CASPER_REAL_INTERVAL */
