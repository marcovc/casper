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
 

#ifndef CASPER_LP_SOLVER_H_
#define CASPER_LP_SOLVER_H_

#include <casper/kernel/state/env.h>
#include <casper/kernel/par/parexpr.h>
#include <casper/lp/driver.h>

#include <casper/util/container/stdhashmap.h>

namespace Casper {
namespace LP {

typedef uint 	 				Idx;
typedef Util::StdVector<Idx> 	Idxs;
typedef Util::StdVector<double> Coeffs;

// \note Does not support adding new variables during search, which
// implies that the set of variables covered by the constraints and objective
// function defined before the first call to solve must remain constant
struct Solver
{
	Solver(Env& env);

	typedef enum { Maximize, Minimize } ObjectiveType;
	void setObjective(ObjectiveType type,
					  const Idxs& idxs, const Coeffs& coeffs,double indterm);

	void postEqual(const Idxs& idxs,const Coeffs& coeffs,double rhs);
	void postLessEqual(const Idxs& idxs,const Coeffs& coeffs,double ub);
	void postGreaterEqual(const Idxs& idxs,const Coeffs& coeffs,double lb);

	bool solve();

	uint addVar(const Idx& idx,double lb, double ub);
	uint addVar(const Idx& idx);

	Util::StdVector<uint> addVars(const Idxs& idxs);
	Util::StdVector<uint> addVars(const Idxs& idxs,double lb,double ub);

	double getValue(uint idx) const;
	double getObjValue() const;

	DoubleParExpr getValueParExpr(uint idx) const;

	Env&	getEnv() const {	return env;	}

	protected:

	void updateVar(const Idx& v, double lb,double ub);
	void updateVarUb(const Idx& v, double ub);
	void updateVarLb(const Idx& v, double lb);
	void updateVarFix(const Idx& v, double val);

	Env&						env;
	Driver 						driver;
	Util::StdHashMap<Idx,int>	idxColMap;
};

} // LP
} // Casper

#endif /* CASPER_LP_SOLVER_H_ */
