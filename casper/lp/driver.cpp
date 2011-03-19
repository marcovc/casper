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
 
#include <casper/lp/driver.h>
#include <casper/kernel/state/state.h>
#include <casper/kernel/state/trail.h>

#include <glpk.h>

namespace Casper {
namespace LP {

// translates StdVector to glp format (1-based arrays!)
template<class T>
T* newGLPVector(const Util::StdVector<T>& vals)
{
	T* glpVals = new T[vals.size()+1];
	for (uint i = 0; i < vals.size(); ++i)
		glpVals[i+1] = vals[i];
	return glpVals;
}

// translates StdVector to glp format (1-based arrays!)
// specialization for uint->int conversion
int* newGLPVector(const Util::StdVector<uint>& vals)
{
	int* glpVals = new int[vals.size()+1];
	for (uint i = 0; i < vals.size(); ++i)
		glpVals[i+1] = static_cast<int>(vals[i]);
	return glpVals;
}

Driver::Driver(State& state) :
		state(state)
{
	glpProb = glp_create_prob();
}

Driver::~Driver()
{
	glp_delete_prob(static_cast<glp_prob*>(glpProb));
}

// ---- Objective ----

struct UndoUpdateObjectiveCoef : ITrailAgent
{
	UndoUpdateObjectiveCoef(Driver& driver,uint idx, double oldCoef) :
		driver(driver),idx(idx),oldCoef(oldCoef) {}
	void restore()
	{
		glp_set_obj_coef(static_cast<glp_prob*>(driver.glpProb),idx,oldCoef);
	}
	Driver& driver;
	const uint idx;
	const double oldCoef;
};

struct UndoSetObjectiveDir : ITrailAgent
{
	UndoSetObjectiveDir(Driver& driver,int oldDir) : driver(driver),oldDir(oldDir) {}
	void restore()
	{
		glp_set_obj_dir(static_cast<glp_prob*>(driver.glpProb),oldDir);
	}
	Driver& driver;
	const int oldDir;
};

// 0 is independent term
// WARNING: this is not reversible (see updateObjectiveCoef)
void Driver::setObjectiveCoeff(uint idx, double coef)
{
	glp_set_obj_coef(static_cast<glp_prob*>(glpProb),idx,coef);
}

// 0 is independent term
// \note This is reversible
void Driver::updateObjectiveCoeff(uint idx, double coef)
{
	// store undo information
	const double oldCoef = glp_get_obj_coef(static_cast<glp_prob*>(glpProb),idx);
	if (coef == oldCoef)
		return;
	state.trail.record(new (state) UndoUpdateObjectiveCoef(*this,idx,oldCoef));

	// change current value
	setObjectiveCoeff(idx,coef);
}

// WARNING: this is not reversible (see updateObjectiveDir)
void Driver::setObjectiveDir(bool upDir)
{
	if (upDir)
		glp_set_obj_dir(static_cast<glp_prob*>(glpProb),GLP_MAX);
	else
		glp_set_obj_dir(static_cast<glp_prob*>(glpProb),GLP_MIN);
}

// \note This is reversible
void Driver::updateObjectiveDir(bool upDir)
{
	const double oldDir = glp_get_obj_dir(static_cast<glp_prob*>(glpProb));

	if (upDir)
	{
		if (oldDir == GLP_MAX)
			return;
		// store undo information
		state.trail.record(new (state) UndoSetObjectiveDir(*this,GLP_MIN));
	}
	else
	{
		if (oldDir == GLP_MIN)
			return;
		// store undo information
		state.trail.record(new (state) UndoSetObjectiveDir(*this,GLP_MAX));
	}

	// perform the update
	setObjectiveDir(upDir);
}

// ---- Constraints ----

// delete allocated row on backtrack
struct UndoAllocateRow : ITrailAgent
{
	UndoAllocateRow(Driver& driver,uint rowId) : driver(driver),rowId(rowId) {}
	void restore()
	{
		const int rows[2] = { 0, rowId  };
		glp_del_rows(static_cast<glp_prob*>(driver.glpProb),1,rows);
	}
	Driver& driver;
	const int rowId;
};

struct UndoUpdateConstraintBounds : ITrailAgent
{
	UndoUpdateConstraintBounds(Driver& driver,uint rowId, int oldType, double oldLb, double oldUb) :
		driver(driver),rowId(rowId),oldType(oldType),oldLb(oldLb),oldUb(oldUb) {}
	void restore()
	{
		glp_set_row_bnds(static_cast<glp_prob*>(driver.glpProb),rowId,oldType,oldLb,oldUb);
	}
	Driver& driver;
	const uint rowId;
	const int oldType;
	const double oldLb;
	const double oldUb;
};


uint Driver::addConstraint(const Util::StdVector<uint>& idxs,
						   const Util::StdVector<double>& vals,
						   ConstraintType type, double lb, double ub)
{
	assert(idxs.size()==vals.size());

	// allocate one row
	uint rowId = glp_add_rows(static_cast<glp_prob*>(glpProb),1);

	// store undo information
	state.trail.record(new (state) UndoAllocateRow(*this,rowId));

	setConstraintCoeffs(rowId,idxs,vals);
	setConstraintBounds(rowId,type,lb,ub);

	return rowId;
}

// WARNING: this is not reversible
void Driver::setConstraintCoeffs(uint rowId,
								 const Util::StdVector<uint>& idxs,
						   	   	 const Util::StdVector<double>& vals)
{
	assert(idxs.size()==vals.size());

	int* glpIdxs = newGLPVector(idxs);
	double* glpVals = newGLPVector(vals);

	// set coefficient matrix values for this row
	glp_set_mat_row(static_cast<glp_prob*>(glpProb),rowId,idxs.size(),glpIdxs,glpVals);

	// free allocated arrays
	delete glpIdxs;
	delete glpVals;
}

// WARNING: this is not reversible (see updateConstraintBounds)
void Driver::setConstraintBounds(uint rowId,ConstraintType type, double lb, double ub)
{
	int glpType;
	switch (type)
	{
		case ConstraintType::LB:
			glpType = GLP_LO;
			break;
		case ConstraintType::UB:
			glpType = GLP_UP;
			break;
		case ConstraintType::DB:
			glpType = GLP_DB;
			break;
		case ConstraintType::Fix:
			glpType = GLP_FX;
			break;
		case ConstraintType::Free:
			glpType = GLP_FR;
			break;
	}
	glp_set_row_bnds(static_cast<glp_prob*>(glpProb),rowId,glpType,lb,ub);
}

// \note this is reversible
void Driver::updateConstraintBounds(uint rowId,ConstraintType type, double lb, double ub)
{
	// store undo information
	int oldType = glp_get_row_type(static_cast<glp_prob*>(glpProb),rowId);

	double oldLb = glp_get_row_lb(static_cast<glp_prob*>(glpProb),rowId);
	double oldUb = glp_get_row_ub(static_cast<glp_prob*>(glpProb),rowId);

	state.trail.record(new (state) UndoUpdateConstraintBounds(*this,rowId,oldType,oldLb,oldUb));

	// perform the update
	setConstraintBounds(rowId,type,lb,ub);
}

// ---- Variables ----

// delete allocated col on backtrack
struct UndoAllocateCol : ITrailAgent
{
	UndoAllocateCol(Driver& driver,int colId) : driver(driver),colId(colId) {}
	void restore()
	{
		const int cols[2] = { 0, colId };
		glp_del_cols(static_cast<glp_prob*>(driver.glpProb),1,cols);
	}
	Driver& driver;
	const int colId;
};

struct UndoUpdateVariableBounds : ITrailAgent
{
	UndoUpdateVariableBounds(Driver& driver,uint colId, int oldType, double oldLb, double oldUb) :
		driver(driver),colId(colId),oldType(oldType),oldLb(oldLb),oldUb(oldUb) {}
	void restore()
	{
		glp_set_col_bnds(static_cast<glp_prob*>(driver.glpProb),colId,oldType,oldLb,oldUb);
	}
	Driver& driver;
	const uint colId;
	const int oldType;
	const double oldLb;
	const double oldUb;
};

uint Driver::addVariable(ConstraintType type, double lb, double ub)
{
	// allocate one row
	int colId = glp_add_cols(static_cast<glp_prob*>(glpProb),1);

	// store undo information
	state.trail.record(new (state) UndoAllocateCol(*this,colId));

	setVariableBounds(colId,type,lb,ub);

	return colId;
}

uint Driver::addVariable(const Util::StdVector<uint>& idxs,
						   const Util::StdVector<double>& vals,
						   ConstraintType type, double lb, double ub)
{
	assert(idxs.size()==vals.size());

	// allocate one row
	int colId = glp_add_cols(static_cast<glp_prob*>(glpProb),1);

	// store undo information
	state.trail.record(new (state) UndoAllocateCol(*this,colId));

	setVariableCoeffs(colId,idxs,vals);
	setVariableBounds(colId,type,lb,ub);

	return colId;
}

// WARNING: this is not reversible
void Driver::setVariableCoeffs(uint colId,
								 const Util::StdVector<uint>& idxs,
						   	   	 const Util::StdVector<double>& vals)
{
	assert(idxs.size()==vals.size());

	int* glpIdxs = newGLPVector(idxs);
	double* glpVals = newGLPVector(vals);

	// set coefficient matrix values for this row
	glp_set_mat_col(static_cast<glp_prob*>(glpProb),colId,idxs.size(),glpIdxs,glpVals);

	// free allocated arrays
	delete glpIdxs;
	delete glpVals;
}

// WARNING: this is not reversible (see updateVariableBounds)
void Driver::setVariableBounds(uint colId,ConstraintType type, double lb, double ub)
{
	int glpType;
	switch (type)
	{
		case ConstraintType::LB:
			glpType = GLP_LO;
			break;
		case ConstraintType::UB:
			glpType = GLP_UP;
			break;
		case ConstraintType::DB:
			glpType = GLP_DB;
			break;
		case ConstraintType::Fix:
			glpType = GLP_FX;
			break;
		case ConstraintType::Free:
			glpType = GLP_FR;
			break;
	}
	glp_set_col_bnds(static_cast<glp_prob*>(glpProb),colId,glpType,lb,ub);
}

// \note this is reversible
void Driver::updateVariableBounds(uint colId,ConstraintType type, double lb, double ub)
{
	// store undo information
	int oldType = glp_get_col_type(static_cast<glp_prob*>(glpProb),colId);

	double oldLb = glp_get_col_lb(static_cast<glp_prob*>(glpProb),colId);
	double oldUb = glp_get_col_ub(static_cast<glp_prob*>(glpProb),colId);

	state.trail.record(new (state) UndoUpdateVariableBounds(*this,colId,oldType,oldLb,oldUb));

	// perform the update
	setVariableBounds(colId,type,lb,ub);
}

// ---- Solve ----

// reconstructs simplex basis
struct UndoSolve : ITrailAgent
{
	UndoSolve(Driver& driver) : driver(driver) {}
	void restore()
	{
		glp_std_basis(static_cast<glp_prob*>(driver.glpProb));
		//glp_adv_basis(static_cast<glp_prob*>(driver.glpProb),0);
	}
	Driver& driver;
};

bool Driver::solve()
{
	state.trail.record(new (state) UndoSolve(*this));
	int status = glp_simplex(static_cast<glp_prob*>(glpProb),NULL);
	if (status!=0)
		std::cerr << "glp_simplex error: " << status << std::endl;
	assert(status == 0);
	return status == 0;
}

// ---- Getters ----

double Driver::getValue(uint colId) const
{
	return glp_get_col_prim(static_cast<glp_prob*>(glpProb),colId);
}

double Driver::getObjValue() const
{
	return glp_get_obj_val(static_cast<glp_prob*>(glpProb));
}

} // LP
} // Casper
