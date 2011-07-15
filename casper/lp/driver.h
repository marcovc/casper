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
 

#ifndef CASPER_LP_DRIVER_H_
#define CASPER_LP_DRIVER_H_

#include <casper/util/container/stdarray.h>
#include <casper/kernel/state/state.h>
#include <casper/kernel/reversible/reversible.h>

namespace Casper {
namespace LP {

struct Driver
{
	typedef uint Idx;
	typedef double Coeff;
	typedef Util::StdArray<uint,1> 	Idxs;
	typedef Util::StdArray<Coeff> 	Coeffs;

	typedef enum { LB, UB, DB, Fix, Free} ConstraintType;

	Driver(State& state);
	~Driver();

	// 0 is independent term
	// WARNING: this is not reversible (see updateObjectiveCoef)
	void setObjectiveCoeff(Idx idx, Coeff coef);

	// 0 is independent term
	// \note This is reversible
	void updateObjectiveCoeff(Idx idx, Coeff coef);

	// WARNING: this is not reversible (see updateObjectiveDir)
	void setObjectiveDir(bool upDir);

	// \note This is reversible
	void updateObjectiveDir(bool upDir);

	// \note This is reversible
	uint addConstraint(const Idxs& idxs,
						const Coeffs& coeffs,
						ConstraintType type, double lb, double ub);

	// WARNING: this is not reversible
	void setConstraintCoeffs(uint rowId,
								const Idxs& idxs,
							   	const Coeffs& coeffs);

	// WARNING: this is not reversible (see updateConstraintBounds)
	void setConstraintBounds(uint rowId,ConstraintType type, double lb, double ub);

	// This is effective only if the new bounds are tighter than the current bounds
	// \note this is reversible
	void updateConstraintBounds(uint rowId,ConstraintType type, double lb, double ub);

	// \note This is reversible
	uint addVariable(ConstraintType type, double lb, double ub);

	// \note This is reversible
	uint addVariable(const Idxs& idxs,
					const Coeffs& coeffs,
					ConstraintType type, double lb, double ub);

	// WARNING: this is not reversible
	void setVariableCoeffs(uint colId,
							const Idxs& idxs,
							const Coeffs& coeffs);

	// WARNING: this is not reversible (see updateVariableBounds)
	void setVariableBounds(Idx colId,ConstraintType type, double lb, double ub);

	// The provided bounds are always intersected with the current bounds to
	// guarantee that the update is monotonic (see setVariableBounds if this is
	// not desired)
	// \return False if the update generates an empty range for the variable, true otherwise
	// \note this is reversible
	bool updateVariableBounds(Idx colId,ConstraintType type, double lb, double ub);

	bool solve();

	double getValue(uint colId) const;
	void getActivityRange(uint colId, double& min, double& max) const;

	double getObjValue() const;

	void printModel(const char* fname);

	uint getNbRows() const;
	uint getNbCols() const;

	State&				state;
	void*				glpProb;
	void*				glpSMCP;

	Reversible<bool>	first;
};

} // LP
} // Casper

#endif /* CASPER_LP_DRIVER_H_ */
