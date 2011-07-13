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
 
#include <casper/lp/solver.h>
#include <casper/lp/var.h>

namespace Casper {
namespace LP {



Solver::Solver(Env& env) : env(env),driver(env),
		lastInsertedCol(env,0),
		bValid(env,true),
	    bPending(env,false),
	    stats(driver)
{}

uint Solver::newVar()
{
	int newColIdx = driver.addVariable(Driver::Free,0,0);
	lastInsertedCol = newColIdx;
	return newColIdx;
}

bool Solver::post(const LinearEqRepr& l)
{
	switch (l.getOperator())
	{
		case LinearEqRepr::Equal:
			return postEqualZero(l.getExpr());
		case LinearEqRepr::LessEqual:
			return postLessEqualZero(l.getExpr());
	}
	return true;
}

bool Solver::postEqualZero(const LinearExprRepr& l)
{
	if (!bValid)
		return false;
	if (l.vars.size()==0)
		return l.free == 0;
	++stats.nbPosts;
	if (l.vars.size()==1 and *l.coeffs.begin()==1)
	{
		++stats.nbColUpdates;
		bValid = postEqual(l.vars.begin()->id, -l.free);
		return bValid;
	}
	Idxs 	idxs(l.vars.size());
	uint c = 0;
	for (auto it = l.vars.begin(); it != l.vars.end(); ++it)
		idxs[c++] = it->id;
	Coeffs 	coeffs(l.vars.size());
	c = 0;
	for (auto it = l.coeffs.begin(); it != l.coeffs.end(); ++it)
		coeffs[c++] = *it;
	postEqual(idxs,coeffs,-l.free);
	bPending = true;
	return true;
}


bool Solver::postLessEqualZero(const LinearExprRepr& l)
{
	if (!bValid)
		return false;
	if (l.vars.size()==0)
		return l.free <= 0;
	++stats.nbPosts;
	if (l.vars.size()==1 and *l.coeffs.begin()==1)
	{
		++stats.nbColUpdates;
		bValid = postLessEqual(l.vars.begin()->id, -l.free);
		return bValid;
	}
	Idxs 	idxs(l.vars.size());
	uint c = 0;
	for (auto it = l.vars.begin(); it != l.vars.end(); ++it)
		idxs[c++] = it->id;
	Coeffs 	coeffs(l.vars.size());
	c = 0;
	for (auto it = l.coeffs.begin(); it != l.coeffs.end(); ++it)
		coeffs[c++] = *it;
	postLessEqual(idxs,coeffs,-l.free);
	bPending = true;
	return true;
}

bool Solver::postEqual(const Idxs& idxs,double rhs)
{
	for (uint i = 0; i < idxs.size(); ++i)
		if (!postEqual(idxs[i],rhs))
			return false;
	return true;
}

bool Solver::postLessEqual(const Idxs& idxs,double ub)
{
	for (uint i = 0; i < idxs.size(); ++i)
		if (!postLessEqual(idxs[i],ub))
			return false;
	return true;
}

bool Solver::postGreaterEqual(const Idxs& idxs,double lb)
{
	for (uint i = 0; i < idxs.size(); ++i)
		if (!postGreaterEqual(idxs[i],lb))
			return false;
	return true;
}

bool Solver::postIn(const Idxs& idxs,double lb, double ub)
{
	for (uint i = 0; i < idxs.size(); ++i)
		if (!postIn(idxs[i],lb,ub))
			return false;
	return true;
}

//#define VAR_UPDATES_AS_CONSTRAINTS

bool Solver::postEqual(const Idx& idx,double rhs)
{
#ifdef VAR_UPDATES_AS_CONSTRAINTS
	postEqual({idx},{1},rhs);
	return true;
#else
	return driver.updateVariableBounds(idx,Driver::Fix, rhs,rhs);
#endif
}

bool Solver::postLessEqual(const Idx& idx,double ub)
{
#ifdef VAR_UPDATES_AS_CONSTRAINTS
	postLessEqual({idx},{1},ub);
	return true;
#else
	return driver.updateVariableBounds(idx,Driver::UB, ub,ub);
#endif
}

bool Solver::postGreaterEqual(const Idx& idx,double lb)
{
#ifdef VAR_UPDATES_AS_CONSTRAINTS
	postGreaterEqual({idx},{1},lb);
	return true;
#else
	return driver.updateVariableBounds(idx,Driver::LB, lb,lb);
#endif
}

bool Solver::postIn(const Idx& idx,double lb, double ub)
{
#ifdef VAR_UPDATES_AS_CONSTRAINTS
	postIn({idx},{1},lb,ub);
	return true;
#else
	return driver.updateVariableBounds(idx,Driver::DB, lb,ub);
#endif
}

void Solver::setObjective(ObjectiveType type,
						  const Idxs& idxs,
	  		  	  	  	  const Coeffs& coeffs,
	  		  	  	  	  double indterm)
{
	driver.setObjectiveCoeff(0,indterm);
	driver.setObjectiveDir(type == Maximize);
	for (uint i = 0; i < idxs.size(); ++i)
		driver.setObjectiveCoeff(idxs[i],coeffs[i]);
}

void Solver::postEqual(const Idxs& idxs,
		  			 const Coeffs& coeffs,
		  			 double rhs)
{
	driver.addConstraint(idxs,coeffs,Driver::Fix,rhs,rhs);
}

void Solver::postLessEqual(const Idxs& idxs,
		  				const Coeffs& coeffs,
		  				double ub)
{
	driver.addConstraint(idxs,coeffs,Driver::UB,ub,ub);
}

void Solver::postGreaterEqual(const Idxs& idxs,
		  		   const Coeffs& coeffs,
		  		   double lb)
{
	driver.addConstraint(idxs,coeffs,Driver::LB,lb,lb);
}

void Solver::postIn(const Idxs& idxs,
		  		   const Coeffs& coeffs,
		  		   double lb,double ub)
{
	driver.addConstraint(idxs,coeffs,Driver::DB,lb,ub);
}

void Solver::updateVarDb(const Idx& idx, double lb,double ub)
{
	driver.updateVariableBounds(idx,Driver::DB,lb,ub);
}

void Solver::updateVarUb(const Idx& idx, double ub)
{
	driver.updateVariableBounds(idx,Driver::UB,ub,ub);
}

void Solver::updateVarLb(const Idx& idx, double lb)
{
	driver.updateVariableBounds(idx,Driver::LB,lb,lb);
}

void Solver::updateVarFix(const Idx& idx, double val)
{
	driver.updateVariableBounds(idx,Driver::Fix,val,val);
}

bool Solver::valid()
{
	if (!bPending)
		return bValid;

	// solve problem
	++stats.nbSolveCalls;
	bValid = driver.solve();
	if (!bValid)
		++stats.nbFails;
	bPending = false;
	return bValid;
}

double Solver::getValue(uint idx) const
{
	return driver.getValue(idx);
}

void Solver::getActivityRange(uint idx, double& lb, double& ub) const
{
	driver.getActivityRange(idx,lb,ub);
}

double Solver::getObjValue() const
{
	return driver.getObjValue();
}

//struct GetValue : IPar<double>
//{
//	typedef IPar<double> Super;
//	GetValue(const Solver& solver,uint idx) :
//		solver(solver),idx(idx) {}
//	double value() const
//	{	return solver.getValue(idx);	}
//
//	const Solver&	solver;
//	const uint idx;
//};
//
//struct GetObjValue : IPar<double>
//{
//	typedef IPar<double> Super;
//	GetObjValue(const Solver& solver) :
//		solver(solver) {}
//	double value() const
//	{	return solver.getObjValue();	}
//
//	const Solver&	solver;
//};
//
//DoublePar Solver::getValuePar(uint idx) const
//{	return DoublePar(env,static_cast<const IPar<double>*>(new (env) GetValue(*this,idx)));	}
//
//DoubleParArray Solver::getValuePars() const
//{
//	const uint s = idxColMap.size();
//	DoubleParArray ret(env,s);
//	for (uint i = 0; i < s; ++i)
//		ret[i] = DoublePar(env,static_cast<const IPar<double>*>(new (env) GetValue(*this,i)));
//	return ret;
//}
//
//DoublePar Solver::getObjValuePar() const
//{	return DoublePar(env,static_cast<const IPar<double>*>(new (env) GetObjValue(*this)));	}



}
}

std::ostream& operator<<(std::ostream& os, const Casper::LP::SolverStats& s)
{
	using namespace std;
	os  << left << setw (30) << "Number of rows" << ":" <<  std::setw (10) << std::right
							  << s.getNbRows() << std::endl
	   << left << setw (30) << "Number of columns" << ":" <<  std::setw (10) << std::right
							  << s.getNbCols() << std::endl
	   << left << setw (30) << "Number of solve calls" << ":" <<  std::setw (10) << std::right
							  << s.getNbSolveCalls() << std::endl
	   << left << setw (30) << "Number of fails" << ":" <<  std::setw (10) << std::right
							  << s.getNbFails() << std::endl
	   << left << setw (30) << "Number of column updates" << ":" <<  std::setw (10) << std::right
							  << s.getNbColUpdates() << std::endl
	   << left << setw (30) << "Number of posts" << ":" <<  std::setw (10) << std::right
							  << s.getNbPosts();
	return os;

}

