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

#ifndef CASPER_REAL_FILTER_H_
#define CASPER_REAL_FILTER_H_

namespace casper {

///	Enforces the Greater binary relation between two double views
template<class View1,class View2>
struct BndFilterView2<Greater,Double,View1,Double,View2> :
	BndFilterView2<GreaterEqual,Double,View1,Double,View2>
{
	typedef BndFilterView2<GreaterEqual,Double,View1,Double,View2> Super;
	BndFilterView2(ICPSolver& solver, const View1& p1,const View2& p2) :
		BndFilterView2<GreaterEqual,Double,View1,Double,View2>(solver,p1,p2){}
	Bool execute()
	{
		if (this->p2.min()>=this->p1.max())
			return false;
		return Super::execute();
	}
	bool entailed() const
	{	return this->p1.min() > this->p2.max();	}
	Filter	operator!()
	{	return Bnd(this->p1.getView() <= this->p2.getView());	}
};

///	Enforces the Distinct binary relation between two double views
template<class View1,class View2>
struct BndFilterView2<Distinct,Double,View1,Double,View2> : IFilter
{
	BndFilterView2(ICPSolver& solver, const View1& p1,const View2& p2) :
		IFilter(solver),p1(solver,p1),p2(solver,p2) {}
	Bool execute()
	{
		if (p1.min()==p1.max() and p2.min()==p2.max())
			return p1.min() != p2.min();
		return true;
	}
	bool entailed() const
	{	return p1.min() > p2.max() or p2.min() > p1.max();	}
	Filter	operator!()
	{	return Bnd(p1.getView() == p2.getView());	}
	void attach(INotifiable* s)
	{	p1.attach(s); p2.attach(s); }
	void detach(INotifiable* s)
	{	p1.detach(s); p2.detach(s);	}

	BndView<Double,View1> p1;
	BndView<Double,View2> p2;
};

}

#endif /* CASPER_REAL_FILTER_H_ */
