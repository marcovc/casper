 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_CP_INT_VAR
#define _H_CASPER_CP_INT_VAR

#include <casper/cp/int/common.h>
#include <casper/kernel/common.h>
#include <casper/cp/var.h>
#include <casper/cp/int/fd.h>
#include <casper/cp/view/array.h>

#include <iostream>

namespace Casper {
namespace CP {

namespace Detail {

template<FD_TP_SPEC(1)>
struct VarDomCreator<FD<FD_TP_LIST(1)> >
{
	FD<FD_TP_LIST(1)>* operator()(Store& store);
	FD<FD_TP_LIST(1)>* operator()(Store& store,
								  const T1& min, const T1& max);
	FD<FD_TP_LIST(1)>* operator()(Store& store, const T1& i);
	template<class InputIterator>
	FD<FD_TP_LIST(1)>* operator()(Store& store,
								  InputIterator min,
								  InputIterator max);

	FD<FD_TP_LIST(1)>* operator()(Store& store,const Util::StdList<StdRange<T1> >& l);

	// returned domain is the union of all domains in the array
	template<class View>
	FD<FD_TP_LIST(1)>* unionOf(Store& store, const View& v);
};

template<FD_TP_SPEC(1)>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::operator()(Store& store)
{
	FD<FD_TP_LIST(1)>* ret = new (store) FD<FD_TP_LIST(1)>(store,
						limits<typename FD<FD_TP_LIST(1)>::Value>::min(),
						limits<typename FD<FD_TP_LIST(1)>::Value>::max());
	store.getStats().signalNewIntDomain();
	return ret;
}

template<FD_TP_SPEC(1)>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::operator()(Store& store,
							 const T1& min, const T1& max)
{
	assert(min <= max);
	FD<FD_TP_LIST(1)>* ret =
		new (store) FD<FD_TP_LIST(1)>(store,min,max);
	store.getStats().signalNewIntDomain();
	return ret;
}

template<FD_TP_SPEC(1)>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::operator()(Store& store, const T1& i)
{
	FD<FD_TP_LIST(1)>* ret = new (store) FD<FD_TP_LIST(1)>(store,i,i);
	store.getStats().signalNewIntDomain();
	return ret;
}

template<FD_TP_SPEC(1)>
template<class InputIterator>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::operator()(Store& store,
							  InputIterator min,
							  InputIterator max)
{
	//assert(min <= max);
	FD<FD_TP_LIST(1)>* ret =
		new (store) FD<FD_TP_LIST(1)>(store,min,max);
	store.getStats().signalNewIntDomain();
	return ret;
}

template<FD_TP_SPEC(1)>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::operator()
							(Store& store,const Util::StdList<StdRange<T1> >& l)
{
	//assert(min <= max);
	FD<FD_TP_LIST(1)>* ret =
		new (store) FD<FD_TP_LIST(1)>(store,l);
	store.getStats().signalNewIntDomain();
	return ret;
}


// returned domain is the union of all domains in the array
template<FD_TP_SPEC(1)>
template<class View>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::unionOf(Store& store, const View& v)
{
	DomArrayView<T1,View> doms(store,v);
	set<int> u;
	for (uint i = 0; i < doms.size(); i++)
		u.insert(doms[i]->begin(),doms[i]->end());
	return operator()(store,u.begin(),u.end());
}


// specialization for bd's
template<>
struct VarDomCreator<BD>
{
	BD* operator()(Store& store)
	{
		BD* ret = new (store) BD(store,false,true);
		store.getStats().signalNewBoolDomain();
		return ret;
	}

	BD* operator()(Store& store, bool i)
	{
		BD* ret = new (store) BD(store,i,i);
		store.getStats().signalNewBoolDomain();
		return ret;
	}

	BD* operator()(Store& store, bool l,bool h)
	{
		BD* ret = new (store) BD(store,l,h);
		store.getStats().signalNewBoolDomain();
		return ret;
	}

	// returned domain is the union of all domains in the array
	template<class View>
	BD* unionOf(Store& store, const View& v);
};

// returned domain is the union of all domains in the array
//template<>
template<class View>
BD* VarDomCreator<BD>::unionOf(Store& store, const View& v)
{
	BndArrayView<bool,View> doms(store,v);
	SUList<bool> u(store);
	bool min = true;
	bool max = false;
	for (uint i = 0; i < doms.size(); i++)
	{
		if (min > doms[i].min())
			min = doms[i].min();
		if (max < doms[i].max())
			max = doms[i].max();
		if (min == max)
			return operator()(store,min);
	}
	return operator()(store);
}

};


template<FD_TP_SPEC(1)>
ostream& operator<<(std::ostream& os,
					const Casper::CP::Var<T1,Casper::CP::FD<FD_TP_LIST(1)> >& v)
{
	os << v.domain()
	#if FD_SHOW_CARD
	   << ",#" << (long long)v.domain().size()
	#endif
	   ;
	return os;
}

typedef Casper::CP::Var<int> IntVar;
typedef Casper::CP::Var<bool> BoolVar;
typedef Casper::CP::VarArray<int> IntVarArray;
typedef Casper::CP::VarArray<bool> BoolVarArray;
typedef Casper::CP::VarArray<int,2> IntVarArray2;
typedef Casper::CP::VarArray<bool,2> BoolVarArray2;
typedef Casper::CP::VarArray<int,3> IntVarArray3;
typedef Casper::CP::VarArray<bool,3> BoolVarArray3;
typedef Casper::CP::VarArray<int,4> IntVarArray4;
typedef Casper::CP::VarArray<bool,4> BoolVarArray4;

}
}

#endif // _H_CASPER_CP_INT_VAR

