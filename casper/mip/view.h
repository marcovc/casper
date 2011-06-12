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
 

#ifndef CASPER_MIP_VIEW_H_
#define CASPER_MIP_VIEW_H_

#include <casper/cp/view.h>

namespace Casper {

namespace MIP {
struct Store;
}

namespace CP {

// FIXME: fill in isTrue, canBeTrue, attach
//template<>
//struct ChkView<LP::LinearEq>
//{
//	ChkView(CP::Store& store, const LP::LinearEq& leq) :
//			leq(leq),lsolver(leq.getSolver()) {}
//	bool isTrue() const	// is it true?
//	{	return false;	}
//	bool canBeTrue() const 	// can it still be true?
//	{	return true;	}
//	bool setToTrue()
//	{	return lsolver.post(leq);	}
//	bool setToFalse()
//	{	return lsolver.post(!leq);	}
//
//	void attach(INotifiable* f) { 	}
//	void detach(INotifiable* f) {	}
//
//	const LP::LinearEq& getObj()  const
//	{ 	return leq;	}
//
//
//	LP::LinearEq leq;
//	LP::Solver&	lsolver;
//};

//template<class T>
//struct BndView<double,MIP::Var<T> > : BndView<double,CP::Var<T> >
//{
//	BndView(CP::Store& store, const MIP::Var<T>& v) :
//		BndView<double,CP::Var<T> >(store,v),v(v) {}
//	MIP::Var<T> getObj() const { return v;}
//	MIP::Var<T> v;
//};

template<>
struct BndView<double,LP::Var>
{
	BndView(CP::Store& store, const LP::Var& v) : solver(v.getSolver()),v(v) {}
	double min() const {	return limits<double>::negInf();	}
	double max() const {	return limits<double>::posInf();	}
	void range(double& mi, double &ma) const
	{
		mi = limits<double>::negInf();
		ma = limits<double>::posInf();
	}
	bool updateMin(double nmin) {	return nmin == limits<double>::negInf() or solver.post( v >= nmin);	}
	bool updateMax(double nmax) {	return nmax == limits<double>::posInf() or solver.post( v <= nmax);	}
	bool updateRange(double nmin, double nmax)
	{	return updateMin(nmin) and updateMax(nmax);	}
	void attach(INotifiable*) {}
	void detach(INotifiable*) {}
	LP::Var getObj() const { return v;}
	LP::Solver&	solver;
	LP::Var v;
};

template<>
struct ValView<double,LP::Var>
{
	ValView(CP::Store& store, const LP::Var& v) : solver(v.getSolver()),v(v) {}
	double value() const {	assert(0); return 0; }
	bool ground() const {	return false; }
	bool setValue(double val) {	return solver.post( v == val);	}
	void attach(INotifiable*) {}
	void detach(INotifiable*) {}
	LP::Var getObj() const { return v;}
	LP::Solver&	solver;
	LP::Var v;
};
} // CP

namespace LP {
//
//template<class Store,class T>
//struct LinearExprView<Store,MIP::Var<T> >
//{
//	LinearExprView(Store& store, const MIP::Var<T>& v) : v(v) {}
//	bool isLinear() const {	return true; }
//	bool isConstant() const {	return v.ground(); }
//	LinearExprRepr getRepr() const
//	{
//		assert(isLinear());
//		if (isConstant())
//			return v.domain().value();
//		else
//			return v.lpvar;
//	}
//	void attach(INotifiable* n) {	v.domain().attachOnGround(n);	}
//	void detach(INotifiable* n) {	v.domain().detachOnGround(n);	}
//	MIP::Var<T> v;
//};

} // LP

} // Casper

#endif /* CASPER_MIP_VIEW_H_ */
