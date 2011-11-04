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
T* newGLPVector(const Util::StdArray<T>& vals)
{
	T* glpVals = new T[vals.size()+1];
	for (uint i = 0; i < vals.size(); ++i)
		glpVals[i+1] = vals[i];
	return glpVals;
}

// translates StdVector to glp format (1-based arrays!)
// specialization for uint->int conversion
int* newGLPVector(const Util::StdArray<uint>& vals)
{
	int* glpVals = new int[vals.size()+1];
	for (uint i = 0; i < vals.size(); ++i)
		glpVals[i+1] = static_cast<int>(vals[i]);
	return glpVals;
}

Driver::Driver(State& state) :
		state(state),first(state,true)
{
	glpProb = glp_create_prob();
	glpSMCP = new glp_smcp();
	//glp_get_bfcp(static_cast<glp_prob*>(glpProb),static_cast<glp_smcp*>(glpSMCP));

	glp_init_smcp(static_cast<glp_smcp*>(glpSMCP));
	static_cast<glp_smcp*>(glpSMCP)->msg_lev = GLP_MSG_ERR;
	//static_cast<glp_smcp*>(glpSMCP)->presolve = GLP_ON; //GLP_MSG_ERR;
}

Driver::~Driver()
{
	glp_delete_prob(static_cast<glp_prob*>(glpProb));
	delete static_cast<glp_smcp*>(glpSMCP);
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
	state.record(new (state) UndoUpdateObjectiveCoef(*this,idx,oldCoef));

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
		state.record(new (state) UndoSetObjectiveDir(*this,GLP_MIN));
	}
	else
	{
		if (oldDir == GLP_MIN)
			return;
		// store undo information
		state.record(new (state) UndoSetObjectiveDir(*this,GLP_MAX));
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


uint Driver::addConstraint(const Idxs& idxs,
						   const Coeffs& vals,
						   ConstraintType type, double lb, double ub)
{
	assert(idxs.size()==vals.size());

	// allocate one row
	uint rowId = glp_add_rows(static_cast<glp_prob*>(glpProb),1);

	// store undo information
	state.record(new (state) UndoAllocateRow(*this,rowId));

	setConstraintCoeffs(rowId,idxs,vals);
	setConstraintBounds(rowId,type,lb,ub);

	return rowId;
}

// WARNING: this is not reversible
void Driver::setConstraintCoeffs(uint rowId,
								 const Idxs& idxs,
						   	   	 const Coeffs& vals)
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

	state.record(new (state) UndoUpdateConstraintBounds(*this,rowId,oldType,oldLb,oldUb));

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
	state.record(new (state) UndoAllocateCol(*this,colId));

	setVariableBounds(colId,type,lb,ub);

	return colId;
}

uint Driver::addVariable(const Idxs& idxs,
						   const Coeffs& vals,
						   ConstraintType type, double lb, double ub)
{
	assert(idxs.size()==vals.size());

	// allocate one row
	int colId = glp_add_cols(static_cast<glp_prob*>(glpProb),1);

	// store undo information
	state.record(new (state) UndoAllocateCol(*this,colId));

	setVariableCoeffs(colId,idxs,vals);
	setVariableBounds(colId,type,lb,ub);

	return colId;
}

// WARNING: this is not reversible
void Driver::setVariableCoeffs(uint colId,
								 const Idxs& idxs,
						   	   	 const Coeffs& vals)
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
bool Driver::updateVariableBounds(uint colId,ConstraintType type, double lb, double ub)
{
	// store undo information
	int oldType = glp_get_col_type(static_cast<glp_prob*>(glpProb),colId);

	double oldLb = glp_get_col_lb(static_cast<glp_prob*>(glpProb),colId);
	double oldUb = glp_get_col_ub(static_cast<glp_prob*>(glpProb),colId);

	// if the update does not provide any bounds then there is nothing to do
	if (type == ConstraintType::Free)
		return true;

	// compute updated bounds
	if (type != ConstraintType::UB)
		lb = std::max(oldLb,lb);
	if (type != ConstraintType::LB)
		ub = std::min(oldUb,ub);

	// if new domain is empty then return false
	if (lb>ub)
		return false;

	// set new type

	// if we are here and had a ground variable it means that nothing changed
	if (oldType == GLP_FX)
		return true;

	// if previous type was Free, new type is 'type'

	// if previous type is double bounded, new type may be double bounded or fixed
	if (oldType == GLP_DB)
	{
		if (type == ConstraintType::Fix or lb==ub)
			type = ConstraintType::Fix;
		else
			type = ConstraintType::DB;
	}

	// if previous type is lower bounded, new type may be:
	if (oldType == GLP_LO)
	{
		if (type == ConstraintType::Fix or lb==ub)
			type = ConstraintType::Fix;	// fixed
		else
		if (type == ConstraintType::UB or type == ConstraintType::DB)
			type = ConstraintType::DB;	// double bounded
		else
			type = ConstraintType::LB;	// remain lower bounded
	}

	// if previous type is upper bounded, new type may be:
	if (oldType == GLP_UP)
	{
		if (type == ConstraintType::Fix or lb==ub)
			type = ConstraintType::Fix;	// fixed
		else
		if (type == ConstraintType::LB or type == ConstraintType::DB)
			type = ConstraintType::DB;	// double bounded
		else
			type = ConstraintType::UB;	// remain upper bounded
	}

	// record undo action
	state.record(new (state) UndoUpdateVariableBounds(*this,colId,oldType,oldLb,oldUb));

	// perform the update
	setVariableBounds(colId,type,lb,ub);

	return true;
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
	state.record(new (state) UndoSolve(*this));
	if (first)
	{
		first = false;
		static_cast<glp_smcp*>(glpSMCP)->presolve = GLP_ON;
	}
	else
		static_cast<glp_smcp*>(glpSMCP)->presolve = GLP_OFF;

	int status = glp_simplex(static_cast<glp_prob*>(glpProb),static_cast<glp_smcp*>(glpSMCP));
	if (status!=0)
	{
		std::cerr << "ERROR: glp_simplex return status is " << status << std::endl;
		return false;
	}
	status = glp_get_status(static_cast<glp_prob*>(glpProb));
	return status != GLP_INFEAS and status != GLP_NOFEAS and status != GLP_UNDEF;
}

void Driver::printModel(const char* fname)
{
	glp_write_mps(static_cast<glp_prob*>(glpProb),GLP_MPS_FILE,NULL,fname);
}

// ---- Getters ----

double Driver::getValue(uint colId) const
{
	return glp_get_col_prim(static_cast<glp_prob*>(glpProb),colId);
}

void Driver::getActivityRange(uint colId,double& lb, double& ub) const
{
	glp_prob* const p = static_cast<glp_prob*>(glpProb);

	if (!glp_bf_exists(p))
		glp_factorize(p);

	const int k = glp_get_num_rows(p) + colId;

	glp_print_ranges(static_cast<glp_prob*>(glpProb),0,NULL,0,"sens.txt");
	if (glp_get_col_stat(p,colId) != GLP_BS)
		glp_analyze_bound(p, k, &ub, NULL, &lb, NULL);
	else
		glp_analyze_coef(p, k, NULL, NULL, &ub, NULL, NULL, &lb);
}

double Driver::getObjValue() const
{
	return glp_get_obj_val(static_cast<glp_prob*>(glpProb));
}

uint Driver::getNbRows() const
{
	return glp_get_num_rows(static_cast<glp_prob*>(glpProb));
}

uint Driver::getNbCols() const
{
	return glp_get_num_cols(static_cast<glp_prob*>(glpProb));
}

} // LP
} // Casper
