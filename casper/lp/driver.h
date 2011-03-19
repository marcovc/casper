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

#include <casper/util/container/stdvector.h>
#include <casper/kernel/state/state.h>

namespace Casper {
namespace LP {

struct Driver
{
	typedef enum { LB, UB, DB, Fix, Free} ConstraintType;

	Driver(State& state);
	~Driver();

	// 0 is independent term
	// WARNING: this is not reversible (see updateObjectiveCoef)
	void setObjectiveCoeff(uint idx, double coef);

	// 0 is independent term
	// \note This is reversible
	void updateObjectiveCoeff(uint idx, double coef);

	// WARNING: this is not reversible (see updateObjectiveDir)
	void setObjectiveDir(bool upDir);

	// \note This is reversible
	void updateObjectiveDir(bool upDir);

	// \note This is reversible
	uint addConstraint(const Util::StdVector<uint>& idxs,
						const Util::StdVector<double>& vals,
						ConstraintType type, double lb, double ub);

	// WARNING: this is not reversible
	void setConstraintCoeffs(uint rowId,
								const Util::StdVector<uint>& idxs,
							   	const Util::StdVector<double>& vals);

	// WARNING: this is not reversible (see updateConstraintBounds)
	void setConstraintBounds(uint rowId,ConstraintType type, double lb, double ub);

	// \note this is reversible
	void updateConstraintBounds(uint rowId,ConstraintType type, double lb, double ub);

	// \note This is reversible
	uint addVariable(ConstraintType type, double lb, double ub);

	// \note This is reversible
	uint addVariable(const Util::StdVector<uint>& idxs,
					const Util::StdVector<double>& vals,
					ConstraintType type, double lb, double ub);

	// WARNING: this is not reversible
	void setVariableCoeffs(uint colId,
							const Util::StdVector<uint>& idxs,
							const Util::StdVector<double>& vals);

	// WARNING: this is not reversible (see updateVariableBounds)
	void setVariableBounds(uint colId,ConstraintType type, double lb, double ub);

	// \note this is reversible
	void updateVariableBounds(uint colId,ConstraintType type, double lb, double ub);

	bool solve();

	double getValue(uint colId) const;
	double getObjValue() const;

	State&				state;
	void*				glpProb;
};

} // LP
} // Casper

#endif /* CASPER_LP_DRIVER_H_ */
