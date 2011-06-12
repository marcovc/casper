 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_CP_SOLVER
#define _H_CASPER_CP_SOLVER

#include <casper/kernel/state/env.h>
#include <casper/kernel/goal/explorer.h>
#include <casper/cp/store.h>
// not needed by this header (just for user convenience)
#include <casper/cp/goal/bb.h>
#include <casper/cp/view/par.h>
#include <casper/cp/filter/objective.h>

namespace Casper {
namespace CP {


/*
cp::Var<int> v();

CP::IntVar v;
CP::IntSetVar v;

Gecode::Store v;
Model::IntVar v;

LS::IntVar v;
LP::IntVar v;

SAT::Var v;
SAT::MiniSat::Store store;
store.post(v);

Int v2(env);	// not just a typedef for a reversible...

Goal g =	forAll(v2,range(2,4))
			(
				tryAll(v3,range(3,4))
				( post(v[v2] == v3) )
			);
*/


struct Solver;

struct SolverStats
{
	SolverStats(Solver& solver) : solver(solver) {}
	Solver& solver;
};

struct Solver : Env,Store
{
	Solver();
	~Solver() {}

	// these are necessary to solve ambiguity since both Env and Store auto convert to IHeap
	operator Util::IHeap&() { return static_cast<Env&>(*this); }
	operator const Util::IHeap&() const { return static_cast<const Env&>(*this); }

	operator State&() { return static_cast<Env&>(*this); }
	operator const State&() const { return static_cast<const Env&>(*this); }

	void setExplorer(IExplorer* e) {	pExplorer = e;	}
	IExplorer* getExplorer() {	return pExplorer;	}

	bool solve(Goal g) {	return valid() and pExplorer->explore(g);	}
	template<class T>
	bool solve(const T& t) {	return valid() and pExplorer->explore(Goal(*this,t));	}
	bool next()	{	return	pExplorer->resume();	}

	const SolverStats& getStats() const
	{	return stats;	}
	SolverStats& getStats()
	{	return stats;	}

	const Util::CPUTimer& getCPUTimer() const
	{	return static_cast<const Env&>(*this).getStats().getCPUTimer(); }

	Reversible<IExplorer*>			pExplorer;
	SolverStats						stats;
};



} // CP
} // Casper

std::ostream& operator<<(std::ostream& os, const Casper::CP::SolverStats& s);

#endif

