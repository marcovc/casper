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
#include <casper/kernel/par/par.h>
#include <casper/lp/driver.h>
//#include <casper/lp/lineareqrepr.h>
#include <casper/lp/lineareqvalview.h>


#include <casper/util/container/stdhashmap.h>

namespace Casper {
namespace LP {

struct Solver;

struct SolverStats
{
	SolverStats(const Driver& driver) :
		driver(driver),nbSolveCalls(0),nbFails(0),nbColUpdates(0),nbPosts(0) {}

	counter getNbRows() const { return driver.getNbRows(); }
	counter getNbCols() const { return driver.getNbCols();	}
	counter getNbSolveCalls() const { return nbSolveCalls; }
	counter getNbFails() const { return nbFails; }
	counter getNbColUpdates() const { return nbColUpdates; }
	counter getNbPosts() const { return nbPosts; }

	const Driver& driver;
	counter nbSolveCalls;
	counter nbFails;
	counter nbColUpdates;
	counter nbPosts;
};

struct Var;
struct LinearEq;

struct Solver
{
	typedef Driver::Idx		Idx;
	typedef Driver::Coeff	Coeff;
	typedef Driver::Idxs	Idxs;
	typedef Driver::Coeffs	Coeffs;

	Solver(Env& env);

	typedef enum { Maximize, Minimize } ObjectiveType;
	void setObjective(ObjectiveType type,
					  const Idxs& idxs, const Coeffs& coeffs,double indterm);


	bool solve() {	return valid();	}
	bool valid();

	double getObjValue() const;

//	DoublePar 		getValuePar(uint idx) const;
//	DoubleParArray 	getValuePars() const;
//	DoublePar 		getObjValuePar() const;

	double getValue(uint idx) const;
	void getActivityRange(uint idx, double& lb, double& ub) const;

	Env&	getEnv() const {	return env;	}

	operator Env&() const {	return env;	}
	operator Util::IHeap&() const {	return env;	}
	operator State&() const {	return env;	}

	void printModel(const char* fname) { driver.printModel(fname); }

	uint newVar();

	template<class T>
	bool post(const T& r);

	bool post(const LinearEqRepr& r);

//	template<class T1,class T2>
//	bool post(const Rel2<Equal,T1,T2>& r);
//	template<class T1,class T2>
//	bool post(const Rel2<GreaterEqual,T1,T2>& r);
//	template<class T1,class T2>
//	bool post(const Rel2<LessEqual,T1,T2>& r);

	const SolverStats& getStats() const { return stats; }

	protected:

	bool postEqualZero(const LinearExprRepr& r);
	bool postLessEqualZero(const LinearExprRepr& r);

	// Linear constraints (variables*coefficients==rhs)
	void postEqual(const Idxs& idxs,const Coeffs& coeffs,double rhs);
	void postLessEqual(const Idxs& idxs,const Coeffs& coeffs,double ub);
	void postGreaterEqual(const Idxs& idxs,const Coeffs& coeffs,double lb);
	void postIn(const Idxs& idxs,const Coeffs& coeffs,double lb,double ub);

	// Updating single variable (variable==[lb,ub])
	bool postEqual(const Idx& idx,double rhs);
	bool postLessEqual(const Idx& idx,double ub);
	bool postGreaterEqual(const Idx& idx,double lb);
	bool postIn(const Idx& idx,double lb, double ub);

	// Updating multiple variables (variables==[lb,ub])
	bool postEqual(const Idxs& idxs,double rhs);
	bool postLessEqual(const Idxs& idxs,double ub);
	bool postGreaterEqual(const Idxs& idxs,double lb);
	bool postIn(const Idxs& idxs,double lb, double ub);

	void updateVarDb(const Idx& v, double lb,double ub);
	void updateVarUb(const Idx& v, double ub);
	void updateVarLb(const Idx& v, double lb);
	void updateVarFix(const Idx& v, double val);

	Env&						env;
	Driver 						driver;
	Reversible<int>				lastInsertedCol;
	Reversible<bool> 			bValid;
	Reversible<bool> 			bPending;
	SolverStats					stats;
};


template<class T>
bool Solver::post(const T& r)
{
	LinearEqValView<Solver,T> v(*this,r);
	if (!v.isLinear())
		throw Exception::NonLinear();
	return post(v.getRepr());
}

} // LP
} // Casper

std::ostream& operator<<(std::ostream& os, const Casper::LP::SolverStats&);

#endif /* CASPER_LP_SOLVER_H_ */
