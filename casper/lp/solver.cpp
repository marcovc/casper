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

namespace Casper {
namespace LP {

Solver::Solver(Env& env) : env(env),driver(env)
{}

uint Solver::addVar(const Idx& idx,double lb, double ub)
{
	auto it = idxColMap.find(idx);
	if (it == idxColMap.end())
	{
		if (lb==ub)
			idxColMap[idx] = driver.addVariable(Driver::Fix,lb,ub);
		else
		if (lb>limits<double>::negInf() and ub<limits<double>::posInf())
			idxColMap[idx] = driver.addVariable(Driver::DB,lb,ub);
		else
		if (lb>limits<double>::negInf() )
			idxColMap[idx] = driver.addVariable(Driver::LB,lb,ub);
		else
		if (ub<limits<double>::posInf())
			idxColMap[idx] = driver.addVariable(Driver::UB,lb,ub);
		else
			idxColMap[idx] = driver.addVariable(Driver::Free,lb,ub);
		return idxColMap[idx];
	}
	else
		return it->second;
}

uint Solver::addVar(const Idx& idx)
{
	auto it = idxColMap.find(idx);
	if (it == idxColMap.end())
	{
		idxColMap[idx] = driver.addVariable(Driver::Free,0,0);
		return idxColMap[idx];
	}
	else
		return it->second;
}

Util::StdVector<uint> Solver::addVars(const Idxs& idxs)
{
	Util::StdVector<uint> ridxs(idxs.size());
	for (uint i = 0; i < idxs.size(); ++i)
		ridxs[i] = addVar(idxs[i]);
	return ridxs;
}

Util::StdVector<uint> Solver::addVars(const Idxs& idxs,double lb,double ub)
{
	Util::StdVector<uint> ridxs(idxs.size());
	for (uint i = 0; i < idxs.size(); ++i)
		ridxs[i] = addVar(idxs[i],lb,ub);
	return ridxs;
}

void Solver::setObjective(ObjectiveType type,
						  const Idxs& idxs,
	  		  	  	  	  const Coeffs& coeffs,
	  		  	  	  	  double indterm)
{
	driver.setObjectiveCoeff(0,indterm);
	driver.setObjectiveDir(type == Maximize);
	Util::StdVector<uint> ridxs = addVars(idxs);
	for (uint i = 0; i < ridxs.size(); ++i)
		driver.setObjectiveCoeff(ridxs[i],coeffs[i]);
}

void Solver::postEqual(const Idxs& idxs,
		  			 const Coeffs& coeffs,
		  			 double rhs)
{
	driver.addConstraint(addVars(idxs),coeffs,Driver::Fix,rhs,rhs);
}

void Solver::postLessEqual(const Idxs& idxs,
		  				const Coeffs& coeffs,
		  				double ub)
{
	driver.addConstraint(addVars(idxs),coeffs,Driver::UB,ub,ub);
}

void Solver::postGreaterEqual(const Idxs& idxs,
		  		   const Coeffs& coeffs,
		  		   double lb)
{
	driver.addConstraint(addVars(idxs),coeffs,Driver::LB,lb,lb);
}

void Solver::updateVar(const Idx& idx, double lb,double ub)
{
	driver.updateVariableBounds(addVar(idx),Driver::DB,lb,ub);
}

void Solver::updateVarUb(const Idx& idx, double ub)
{
	driver.updateVariableBounds(addVar(idx),Driver::UB,ub,ub);
}

void Solver::updateVarLb(const Idx& idx, double lb)
{
	driver.updateVariableBounds(addVar(idx),Driver::LB,lb,lb);
}

void Solver::updateVarFix(const Idx& idx, double val)
{
	driver.updateVariableBounds(addVar(idx),Driver::Fix,val,val);
}

bool Solver::solve()
{
	// solve problem
	int status = driver.solve();
	return true;
}

double Solver::getValue(uint idx) const
{
	auto it = idxColMap.find(idx);
	assert(it != idxColMap.end());
	return driver.getValue(it->second);
}

double Solver::getObjValue() const
{
	return driver.getObjValue();
}

struct GetValue : IParExpr<double>
{
	typedef IParExpr<double> Super;
	GetValue(const Solver& solver,uint idx) :
		Super(solver.getEnv()),solver(solver),idx(idx) {}
	double value() const
	{	return solver.getValue(idx);	}

	// FIXME: should ParExpr support attach/detach ?
	void attach(INotifiable*) {}
	void detach(INotifiable*) {}

	const Solver&	solver;
	const uint idx;
};

DoubleParExpr Solver::getValueParExpr(uint idx) const
{	return new (env) GetValue(*this,idx);	}

}
}

