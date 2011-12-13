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
 

#ifndef CASPER_LP_GOAL_H_
#define CASPER_LP_GOAL_H_

#include <casper/kernel/goal/goal.h>
#include <casper/lp/solver.h>

namespace Casper {
namespace LP {

namespace Detail {

struct Solve : IGoal
{
	Solve(Solver& solver) : solver(solver) {}
	Goal execute()
	{
		if (solver.solve())
			return succeed();
		else
			return fail();
	}
	Solver& solver;
};

template<class Idx,class RHS>
struct PostLessEqual2 : IGoal
{
	typedef typename Traits::GetEval<Idx>::Type IdxEval;
	typedef typename Traits::GetEval<RHS>::Type RHSEval;
	PostLessEqual2(Solver& solver, const Idx& idx, const RHS& rhs) :
		solver(solver),idx(idx),rhs(rhs) {}
	Goal execute()
	{
//		std::cout << solver.getEnv().getState().getStats().curLevel << " : posting "
//				<< RefView<IdxEval,Idx>(solver,idx).value() << " <= "
//				<< RefView<RHSEval,RHS>(solver,rhs).value() << std::endl;
		return solver.postLessEqual(RefView<IdxEval,Idx>(solver,idx).value(),
							 	 	RefView<RHSEval,RHS>(solver,rhs).value());
	}
	Solver& solver;
	Idx idx;
	RHS rhs;
};

template<class Idx,class RHS>
struct PostGreaterEqual2 : IGoal
{
	typedef typename Traits::GetEval<Idx>::Type IdxEval;
	typedef typename Traits::GetEval<RHS>::Type RHSEval;
	PostGreaterEqual2(Solver& solver, const Idx& idx, const RHS& rhs) :
		solver(solver),idx(idx),rhs(rhs) {}
	Goal execute()
	{
//		std::cout << solver.getEnv().getState().getStats().curLevel << " : posting "
//				<< RefView<IdxEval,Idx>(solver,idx).value() << " >= "
//				<< RefView<RHSEval,RHS>(solver,rhs).value() << std::endl;
		return solver.postGreaterEqual(RefView<IdxEval,Idx>(solver,idx).value(),
							 	 	   RefView<RHSEval,RHS>(solver,rhs).value());
	}
	Solver& solver;
	Idx idx;
	RHS rhs;
};

template<class Idx,class RHS>
struct PostEqual2 : IGoal
{
	typedef typename Traits::GetEval<Idx>::Type IdxEval;
	typedef typename Traits::GetEval<RHS>::Type RHSEval;
	PostEqual2(Solver& solver, const Idx& idx, const RHS& rhs) :
		solver(solver),idx(idx),rhs(rhs) {}
	Goal execute()
	{
//		std::cout << solver.getEnv().getState().getStats().curLevel << " : posting "
//				<< RefView<IdxEval,Idx>(solver,idx).value() << " == "
//				<< RefView<RHSEval,RHS>(solver,rhs).value() << std::endl;
		return solver.postEqual(RefView<IdxEval,Idx>(solver,idx).value(),
							    RefView<RHSEval,RHS>(solver,rhs).value());
	}
	Solver& solver;
	Idx idx;
	RHS rhs;
};

template<class Idxs,class Coeffs,class RHS>
struct PostLessEqual3 : IGoal
{
	PostLessEqual3(Solver& solver, const Idxs& idxs, const Coeffs& coeffs, const RHS& rhs) :
		solver(solver),idxs(idxs),coeffs(coeffs),rhs(rhs) { assert(this->idxs.size()==this->coeffs.size()); }
	Goal execute()
	{
//		std::cout << solver.getEnv().getState().getStats().curLevel << " : posting "
//				<< RefView<IdxEval,Idx>(solver,idx).value() << " <= "
//				<< RefView<RHSEval,RHS>(solver,rhs).value() << std::endl;
		solver.postLessEqual(idxs,coeffs,rhs);
		return solver.solve();
	}
	Solver& solver;
	Solver::Idxs idxs;
	Solver::Coeffs coeffs;
	RHS rhs;
};

template<class Idxs,class Coeffs,class RHS>
struct PostGreaterEqual3 : IGoal
{
	PostGreaterEqual3(Solver& solver, const Idxs& idxs, const Coeffs& coeffs, const RHS& rhs) :
		solver(solver),idxs(idxs),coeffs(coeffs),rhs(rhs) { assert(this->idxs.size()==this->coeffs.size()); }
	Goal execute()
	{
//		std::cout << solver.getEnv().getState().getStats().curLevel << " : posting "
//				<< RefView<IdxEval,Idx>(solver,idx).value() << " <= "
//				<< RefView<RHSEval,RHS>(solver,rhs).value() << std::endl;
		solver.postGreaterEqual(idxs,coeffs,rhs);
		return solver.solve();
	}
	Solver& solver;
	Solver::Idxs idxs;
	Solver::Coeffs coeffs;
	RHS rhs;
};

template<class Idxs,class Coeffs,class RHS>
struct PostEqual3 : IGoal
{
	PostEqual3(Solver& solver, const Idxs& idxs, const Coeffs& coeffs, const RHS& rhs) :
		solver(solver),idxs(idxs),coeffs(coeffs),rhs(rhs) { assert(this->idxs.size()==this->coeffs.size());}
	Goal execute()
	{
//		std::cout << solver.getEnv().getState().getStats().curLevel << " : posting "
//				<< RefView<IdxEval,Idx>(solver,idx).value() << " <= "
//				<< RefView<RHSEval,RHS>(solver,rhs).value() << std::endl;
		solver.postEqual(idxs,coeffs,rhs);
		return solver.solve();
	}
	Solver& solver;
	Solver::Idxs idxs;
	Solver::Coeffs coeffs;
	RHS rhs;
};


}

Goal solve(Solver& solver)
{	return new (solver.getEnv()) Detail::Solve(solver);	}

template<class Idx,class RHS>
Goal postLessEqual(Solver& solver,const Idx& idx, const RHS& rhs)
{	return new (solver.getEnv()) Detail::PostLessEqual2<Idx,RHS>(solver,idx,rhs);	}

template<class Idx,class RHS>
Goal postGreaterEqual(Solver& solver,const Idx& idx, const RHS& rhs)
{	return new (solver.getEnv()) Detail::PostGreaterEqual2<Idx,RHS>(solver,idx,rhs);	}

template<class Idx,class RHS>
Goal postEqual(Solver& solver,const Idx& idx, const RHS& rhs)
{	return new (solver.getEnv()) Detail::PostEqual2<Idx,RHS>(solver,idx,rhs);	}

template<class Idx,class Coeff,class RHS>
Goal postLessEqual(Solver& solver,const Idx& idx, const Coeff& coeff, const RHS& rhs)
{	return new (solver.getEnv()) Detail::PostLessEqual3<Idx,Coeff,RHS>(solver,idx,coeff,rhs);	}

template<class Idx,class Coeff,class RHS>
Goal postGreaterEqual(Solver& solver,const Idx& idx, const Coeff& coeff, const RHS& rhs)
{	return new (solver.getEnv()) Detail::PostGreaterEqual3<Idx,Coeff,RHS>(solver,idx,coeff,rhs);	}

template<class Idx,class Coeff,class RHS>
Goal postEqual(Solver& solver,const Idx& idx, const Coeff& coeff, const RHS& rhs)
{	return new (solver.getEnv()) Detail::PostEqual3<Idx,Coeff,RHS>(solver,idx,coeff,rhs);	}


}
}

#endif /* CASPER_LP_GOAL_H_ */
