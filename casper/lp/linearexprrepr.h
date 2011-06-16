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
 

#ifndef CASPER_LP_LINEAREXPRREPR_H_
#define CASPER_LP_LINEAREXPRREPR_H_

#include <casper/kernel/common.h>
#include <casper/util/container/stdlist.h>


namespace Casper {
namespace LP {

struct Var;

struct LinearExprRepr
{
	LinearExprRepr(const Var& var);
	LinearExprRepr(const LinearExprRepr& e);
	LinearExprRepr(double free);

	const LinearExprRepr& operator=(const Var& var);
	const LinearExprRepr& operator=(double free);
	const LinearExprRepr& operator=(const LinearExprRepr& expr);

	const LinearExprRepr& operator+=(const Var& var);
	const LinearExprRepr& operator+=(double free);
	const LinearExprRepr& operator+=(const LinearExprRepr& expr);

	const LinearExprRepr& operator-=(const Var& var);
	const LinearExprRepr& operator-=(double free);
	const LinearExprRepr& operator-=(const LinearExprRepr& expr);

	const LinearExprRepr& operator*=(const Var& var);
	const LinearExprRepr& operator*=(double free);
	const LinearExprRepr& operator*=(const LinearExprRepr& expr);

	const LinearExprRepr& operator/=(double free);
	const LinearExprRepr& operator/=(const LinearExprRepr& expr);

	LinearExprRepr operator+(const Var& var) const;
	LinearExprRepr operator+(double free) const;
	LinearExprRepr operator+(const LinearExprRepr& expr) const;

	LinearExprRepr operator-(const Var& var) const;
	LinearExprRepr operator-(double free) const;
	LinearExprRepr operator-(const LinearExprRepr& expr) const;

	LinearExprRepr operator*(const Var& var) const;
	LinearExprRepr operator*(double free) const;
	LinearExprRepr operator*(const LinearExprRepr& expr) const;

	LinearExprRepr operator/(double free) const;
	LinearExprRepr operator/(const LinearExprRepr& expr) const;

	Util::StdList<Var>			vars;
	Util::StdList<double>		coeffs;
	double						free;
};

LinearExprRepr operator-(const LinearExprRepr& expr);
LinearExprRepr operator/(double c, const LinearExprRepr& expr);
LinearExprRepr operator*(double c, const LinearExprRepr& expr);

}

}

std::ostream& operator<<(std::ostream& os, const Casper::LP::LinearExprRepr& v);

#endif /* CASPER_LP_LINEARREPR_H_ */
