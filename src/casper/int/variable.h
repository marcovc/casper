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

#ifndef _H_CASPER_INT_VARIABLE
#define _H_CASPER_INT_VARIABLE

#include <casper/int/common.h>
#include <casper/kernel/common.h>
#include <casper/kernel/variable.h>
#include <casper/int/fd.h>
#include <casper/kernel/view/arrayview.h>

#include <iostream>

namespace casper {

namespace detail {

template<FD_TP_SPEC(1)>
struct VarDomCreator<FD<FD_TP_LIST(1)> >
{
	FD<FD_TP_LIST(1)>* operator()(ICPSolver& solver);
	FD<FD_TP_LIST(1)>* operator()(ICPSolver& solver,
								  const T1& min, const T1& max);
	FD<FD_TP_LIST(1)>* operator()(ICPSolver& solver, const T1& i);
	template<class InputIterator>
	FD<FD_TP_LIST(1)>* operator()(ICPSolver& solver,
								  InputIterator min,
								  InputIterator max);

	FD<FD_TP_LIST(1)>* operator()(ICPSolver& solver,
								  const detail::List<Range<T1> >& l);

	// returned domain is the union of all domains in the array
	template<class View>
	FD<FD_TP_LIST(1)>* unionOf(ICPSolver& solver, const View& v);

	// FIXME: temp: only for minicasper
	FD<FD_TP_LIST(1)>* operator()(ICPSolver& solver,
								  const BndExpr<T1>& l)
	{
		FD<FD_TP_LIST(1)>* ret = new (solver.heap()) FD<FD_TP_LIST(1)>(solver,l.min(),l.max());
		solver.signalNewDomain(ret);
		return ret;
	}
};

template<FD_TP_SPEC(1)>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::operator()(ICPSolver& solver)
{
	FD<FD_TP_LIST(1)>* ret = new (solver.heap()) FD<FD_TP_LIST(1)>(solver,
						limits<typename FD<FD_TP_LIST(1)>::Value>::min(),
						limits<typename FD<FD_TP_LIST(1)>::Value>::max());
	solver.signalNewDomain(ret);
	return ret;
}

template<FD_TP_SPEC(1)>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::operator()(ICPSolver& solver,
							 const T1& min, const T1& max)
{
	assert(min <= max);
	FD<FD_TP_LIST(1)>* ret =
		new (solver.heap()) FD<FD_TP_LIST(1)>(solver,min,max);
	solver.signalNewDomain(ret);
	return ret;
}

template<FD_TP_SPEC(1)>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::operator()(ICPSolver& solver, const T1& i)
{
	FD<FD_TP_LIST(1)>* ret = new (solver.heap()) FD<FD_TP_LIST(1)>(solver,i,i);
	solver.signalNewDomain(ret);
	return ret;
}

template<FD_TP_SPEC(1)>
template<class InputIterator>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::operator()(ICPSolver& solver,
							  InputIterator min,
							  InputIterator max)
{
	//assert(min <= max);
	FD<FD_TP_LIST(1)>* ret =
		new (solver.heap()) FD<FD_TP_LIST(1)>(solver,min,max);
	solver.signalNewDomain(ret);
	return ret;
}

template<FD_TP_SPEC(1)>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::operator()
							(ICPSolver& solver,
							  const detail::List<Range<T1> >& l)
{
	//assert(min <= max);
	FD<FD_TP_LIST(1)>* ret =
		new (solver.heap()) FD<FD_TP_LIST(1)>(solver,l);
	solver.signalNewDomain(ret);
	return ret;
}


// returned domain is the union of all domains in the array
template<FD_TP_SPEC(1)>
template<class View>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::unionOf(ICPSolver& solver, const View& v)
{
	DomArrayView<T1,View> doms(solver,v);
	set<Int> u;
	for (UInt i = 0; i < doms.size(); i++)
		u.insert(doms[i]->begin(),doms[i]->end());
	return operator()(solver,u.begin(),u.end());
}


// specialization for bd's
template<>
struct VarDomCreator<BD>
{
	BD* operator()(ICPSolver& solver)
	{
		BD* ret = new (solver.heap()) BD(solver,false,true);
		solver.signalNewDomain(ret);
		return ret;
	}

	BD* operator()(ICPSolver& solver, bool i)
	{
		BD* ret = new (solver.heap()) BD(solver,i,i);
		solver.signalNewDomain(ret);
		return ret;
	}

	BD* operator()(ICPSolver& solver, bool l,bool h)
	{
		BD* ret = new (solver.heap()) BD(solver,l,h);
		solver.signalNewDomain(ret);
		return ret;
	}

	// returned domain is the union of all domains in the array
	template<class View>
	BD* unionOf(ICPSolver& solver, const View& v);

	// FIXME: temp: only for minicasper
	BD* operator()(ICPSolver& solver, const BndExpr<Bool>& l)
	{
		BD* ret = new (solver.heap()) BD(solver,l.min(),l.max());
		solver.signalNewDomain(ret);
		return ret;
	}
};

// returned domain is the union of all domains in the array
//template<>
template<class View>
BD* VarDomCreator<BD>::unionOf(ICPSolver& solver, const View& v)
{
	BndArrayView<Bool,View> doms(solver,v);
	detail::RSUList<Bool> u(solver);
	Bool min = true;
	Bool max = false;
	for (UInt i = 0; i < doms.size(); i++)
	{
		if (min > doms[i].min())
			min = doms[i].min();
		if (max < doms[i].max())
			max = doms[i].max();
		if (min == max)
			return operator()(solver,min);
	}
	return operator()(solver);
}

};


template<FD_TP_SPEC(1)>
ostream& operator<<(std::ostream& os,
					const casper::DomVar<T1,casper::FD<FD_TP_LIST(1)> >& v)
{
	os << v.domain()
	#if FD_SHOW_CARD
	   << ",#" << (long long)v.domain().size()
	#endif
	   ;
	return os;
}


}


#endif

