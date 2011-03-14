/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2009 - Marco Correia <marco.v.correia@gmail.com>           *
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

#ifndef CASPER_REAL_VIEW_H_
#define CASPER_REAL_VIEW_H_

#include <casper/kernel/rangedom.h>
#include <casper/kernel/view/common.h>

namespace Casper {

template<class T>
struct IDomExpr<RangeDom<T> > : INotifier
{
	typedef RangeDom<T> Dom;
	IDomExpr(CPSolver& solver) : mSolver(solver) {}
	virtual ~IDomExpr()	{}

	///	Returns a pointer to the domain of the expression.
	virtual Dom*		operator->()		=	0;
	///	Returns a pointer to the domain of the expression.
	virtual Dom*		operator->() const	=	0;
	///	Returns a reference to the domain of the expression.
	virtual Dom&		operator*()			=	0;
	///	Returns a reference to the domain of the expression.
	virtual Dom&		operator*() const	=	0;

	// temporary -> the ArrayView mess depends on this.
	void	attach(INotifiable*	n)	{	(*this)->attachOnBounds(n);	}
	void	detach(INotifiable*	n)	{	(*this)->detachOnBounds(n);	}

	bool notify() { assert(0); return true; }
	/// Return CPSolver& object associated with the expression.
	CPSolver&		solver() const {	return mSolver; }

	CPSolver&				mSolver;
};
}

#endif /* CASPER_REAL_VIEW_H_ */
