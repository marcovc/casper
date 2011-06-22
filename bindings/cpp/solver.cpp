/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2010-2010 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#include "solver.h"

#include <sstream>

#include <casper/kernel.h>
#include <casper/cp/int.h>
#include <casper/cp/solver.h>
#include <casper/cp/int/cumulative/cumulative.h>
#include <casper/cp/int/table/positive.h>

#include <casper/cp/set/config.h> // temp
#include <casper/cp/set.h>
#include <casper/util/container/stdhashmap.h>
#include <casper/util/container/stdrange.h>

#include "cpp.h"
#include "eval.h"
#include "print.h"	// tmp

namespace Casperbind {
namespace cpp {

enum Consistency { cDomain, cBounds, cValue };

template<Eval eval,Consistency cons>
struct SymbolToCasper;

struct CPSolverImpl
{
	Casper::CP::Solver 			solver;
	Casper::CP::BoolVarArray 	boolVars;
	Casper::CP::IntVarArray 	intVars;
	Casper::CP::IntSetVarArray 	intSetVars;
	Casper::CP::VarArray<Casper::BoolSet>	boolSetVars;
	Casper::CP::VarArray<double> realVars;
	typedef Casper::Util::StdHashMap<const Variable*,int> VarIdMap;
	VarIdMap	boolVarIds;
	VarIdMap	intVarIds;
	VarIdMap	realVarIds;
	VarIdMap	intSetVarIds;
	VarIdMap	boolSetVarIds;
	int 		nSols;

	CPSolverImpl(int nBoolVars, int nIntVars, int nIntSetVars, int nBoolSetVars,int nRealVars) :
			boolVars(solver,nBoolVars),
			intVars(solver,nIntVars),
			intSetVars(solver,nIntSetVars),
			boolSetVars(solver,nBoolSetVars),
			realVars(solver,nRealVars),
			nSols(0)
	{}

	void addVariable(const Variable& variable);
	void addVariables(const Instance& instance);
	bool addConstraints(const Instance& instance);
	bool solve();
	void printSolution();
	void printStats();
	int getIntVariableValue(const Variable& v);
	bool getBoolVariableValue(const Variable& v);
};

int CPSolverImpl::getIntVariableValue(const Variable& v)
{
	const Casper::CP::IntVar& r = intVars[intVarIds[&v]];
	assert(r.ground());
	return r.domain().value();
}

bool CPSolverImpl::getBoolVariableValue(const Variable& v)
{
	const Casper::CP::BoolVar& r = boolVars[boolVarIds[&v]];
	assert(r.ground());
	return r.domain().value();
}

void CPSolverImpl::addVariable(const Variable& v)
{
	switch (v.getVariableType())
	{
		case Container::sInt:
		{
			const int varId = intVarIds.size();
			intVarIds[&v] = varId;
			switch (v.getDomain().getType(true))
			{
				case Symbol::sInt:
				{
					const int& s = v.getDomain();
					intVars[varId] = Casper::CP::IntVar(solver,s);
					break;
				}
				case Symbol::sRange:
				{
					const IntRange& r = v.getDomain();
					intVars[varId] = Casper::CP::IntVar(solver,r.getLower(),r.getUpper());
					break;
				}
				case Symbol::sSet:
				{
					const IntSet& s = v.getDomain();
					intVars[varId] = Casper::CP::IntVar(solver,s.begin(),s.end());
					break;
				}
				default:
					throw EStructure("Variable",v);
			}
			break;
		}
		case Container::sBool:
		{
			const int varId = boolVarIds.size();
			boolVarIds[&v] = varId;
			switch (v.getDomain().getType(true))
			{
				case Symbol::sBool:
				{
					const bool& s = v.getDomain();
					boolVars[varId] = Casper::CP::BoolVar(solver,s);
					break;
				}
				case Symbol::sSet:
				{
					const BoolSet& s = v.getDomain();
					boolVars[varId] = Casper::CP::BoolVar(solver,*s.begin(),*--s.end());
					break;
				}
				default:
					throw EStructure("Variable",v);
			}
			break;
		}
		default:
			throw EStructure("Variable",v);
	}
}

void CPSolverImpl::addVariables(const Instance& instance)
{
	const SymbolSet& vars = instance.getVariables();
	int nBoolVars,nIntVars,nSetVars;
	nBoolVars = nIntVars = nSetVars = 0;
	for (SymbolSet::Iterator it = vars.begin(); it != vars.end(); ++it)
		if (it->getType(true) == Container::sVariable)
			addVariable(static_cast<const Variable&>(*it));
		else
		{
			const SymbolArray& s = *it;
			for (int i = 0; i < s.getSize(); ++i)
				if (s[i].getType(true) == Container::sVariable and
						not instance.getIndex().hasKeys(s[i]))
					addVariable(static_cast<const Variable&>(s[i]));
		}
}

// --

template<Expression::Operator>
struct ExprFunctorToCasper;

template<>
struct ExprFunctorToCasper<Expression::eAdd>
{	typedef Casper::Add	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eSub>
{	typedef Casper::Sub	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eMul>
{	typedef Casper::Mul	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eDiv>
{	typedef Casper::Div	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eMod>
{	typedef Casper::Mod	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eSym>
{	typedef Casper::Sym	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eAbs>
{	typedef Casper::Abs Type;	};

template<>
struct ExprFunctorToCasper<Expression::ePow>
{	typedef Casper::Pow	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eMin>
{	typedef Casper::Min	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eMax>
{	typedef Casper::Max	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eIfThenElse>
{	typedef Casper::IfThenElse	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eElem>
{	typedef Casper::Element	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eCard>
{	typedef Casper::Cardinal	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eIntersect>
{	typedef Casper::Mul	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eUnion>
{	typedef Casper::Add	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eSymDiff>
{	typedef Casper::SymDiff	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eSumProduct>
{	typedef Casper::SumProduct	Type;	};

// --

template<Predicate::Functor>
struct PredFunctorToCasper;

template<>
struct PredFunctorToCasper<Predicate::pEqual>
{	typedef Casper::Equal	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pDistinct>
{	typedef Casper::Distinct	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pLess>
{	typedef Casper::Less	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pLessEqual>
{	typedef Casper::LessEqual	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pGreater>
{	typedef Casper::Greater	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pGreaterEqual>
{	typedef Casper::GreaterEqual	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pNot>
{	typedef Casper::Not	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pAnd>
{	typedef Casper::And	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pOr>
{	typedef Casper::Or	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pXOr>
{	typedef Casper::XOr	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pIff>
{	typedef Casper::Equal	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pIf>
{	typedef Casper::LessEqual	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pContained>
{	typedef Casper::Contained	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pMember>
{	typedef Casper::Member	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pInTable>
{	typedef Casper::InTable	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pNotInTable>
{	typedef Casper::NotInTable	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pDisjoint>
{	typedef Casper::Disjoint	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pCumulative>
{	typedef Casper::Cumulative	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pPartition>
{	typedef Casper::Partition	Type;	};

// --

template<Consistency cons> struct SymbolToCasper<eBool,cons>
{
	typedef bool Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{	return static_cast<const Type&>(s); }
};

template<Consistency cons> struct SymbolToCasper<eInt,cons>
{
	typedef int Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{	return static_cast<const Type&>(s); }
};

template<Consistency cons> struct SymbolToCasper<eReal,cons>
{
	typedef double Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{	return static_cast<const Type&>(s); }
};

template<Consistency cons> struct SymbolToCasper<eIntSet,cons>
{
	typedef Casper::Util::StdArray<int> Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const IntSet& a = s;
		Type r(solver.solver.getHeap(),a.getSize());
		int c = 0;
		for (IntSet::Iterator it = a.begin(); it != a.end(); ++it)
			r[c++] = *it;
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eBoolSet,cons>
{
	typedef Casper::Util::StdArray<bool> Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const BoolSet& a = s;
		Type r(solver.solver.getHeap(),a.getSize());
		int c = 0;
		for (BoolSet::Iterator it = a.begin(); it != a.end(); ++it)
			r[c++] = *it;
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eBoolArray,cons>
{
	typedef Casper::Util::StdArray<bool> Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const BoolArray& a = s;
		Type r(solver.solver.getHeap(),a.getSize());
		for (int i = 0; i < a.getSize(); ++i)
			r[i] = a[i];
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eIntArray,cons>
{
	typedef Casper::Util::StdArray<int> Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const IntArray a = s; // tmp (copy should be by const ref)
		Type r(solver.solver.getHeap(),a.getSize());
		for (int i = 0; i < a.getSize(); ++i)
			r[i] = a[i];
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eRealArray,cons>
{
	typedef Casper::Util::StdArray<double> Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const RealArray& a = s;
		Type r(solver.solver.getHeap(),a.getSize());
		for (int i = 0; i < a.getSize(); ++i)
			r[i] = a[i];
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eIntRange,cons>
{
	typedef Casper::Util::StdRange<int> Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const IntRange& a = s;
		return Type(a.getLower(),a.getUpper());
	}
};

template<Consistency cons> struct SymbolToCasper<eRealRange,cons>
{
	typedef Casper::Util::StdRange<double> Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const RealRange& a = s;
		return Type(a.getLower(),a.getUpper());
	}
};

template<Consistency cons> struct SymbolToCasper<eBoolVar,cons>
{
	typedef Casper::CP::BoolVar Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const Variable& v = s;
		auto it = solver.boolVarIds.find(&v);
		if (it == solver.boolVarIds.end())
			throw ENoKeyForSymbol(s);
		return solver.boolVars[it->second];
	}
};

template<Consistency cons> struct SymbolToCasper<eIntVar,cons>
{
	typedef Casper::CP::IntVar Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const Variable& v = s;
		auto it = solver.intVarIds.find(&v);
		if (it == solver.intVarIds.end())
			throw ENoKeyForSymbol(s);
		return solver.intVars[it->second];
	}
};

template<Consistency cons> struct SymbolToCasper<eRealVar,cons>
{
	typedef Casper::CP::Var<double> Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const Variable& v = s;
		return solver.realVars[solver.realVarIds[&v]];
	}
};

template<Consistency cons> struct SymbolToCasper<eIntSetVar,cons>
{
	typedef Casper::CP::IntSetVar Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const Variable& v = s;
		return solver.intSetVars[solver.intSetVarIds[&v]];
	}
};

template<Consistency cons> struct SymbolToCasper<eBoolSetVar,cons>
{
	typedef Casper::CP::Var<Casper::BoolSet> Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const Variable& v = s;
		return solver.boolSetVars[solver.boolSetVarIds[&v]];
	}
};

template<Consistency cons> struct SymbolToCasper<eIntVarArray,cons>
{
	typedef Casper::CP::IntVarArray Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const SymbolArray& a = s;
		Type r(solver.solver,a.getSize());
		for (int i = 0; i < a.getSize(); ++i)
			r[i] = SymbolToCasper<eIntVar,cons>()(solver,a[i]);
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eRealVarArray,cons>
{
	typedef Casper::CP::VarArray<double> Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const SymbolArray& a = s;
		Type r(solver.solver,a.getSize());
		for (int i = 0; i < a.getSize(); ++i)
			r[i] = SymbolToCasper<eRealVar,cons>()(solver,a[i]);
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eBoolVarArray,cons>
{
	typedef Casper::CP::BoolVarArray Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const SymbolArray& a = s;
		Type r(solver.solver,a.getSize());
		for (int i = 0; i < a.getSize(); ++i)
			r[i] = SymbolToCasper<eBoolVar,cons>()(solver,a[i]);
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eIntSetVarArray,cons>
{
	typedef Casper::CP::IntSetVarArray Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const SymbolArray& a = s;
		Type r(solver.solver,a.getSize());
		for (int i = 0; i < a.getSize(); ++i)
			r[i] = SymbolToCasper<eIntSetVar,cons>()(solver,a[i]);
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eBoolSetVarArray,cons>
{
	typedef Casper::CP::VarArray<Casper::BoolSet> Type;
	Type operator()(CPSolverImpl& solver,const Symbol& s)
	{
		const SymbolArray& a = s;
		Type r(solver.solver,a.getSize());
		for (int i = 0; i < a.getSize(); ++i)
			r[i] = SymbolToCasper<eBoolSetVar,cons>()(solver,a[i]);
		return r;
	}
};

template<Predicate::Functor f,Eval e1,Consistency cons>
struct Pred1ToCasper
{
	Casper::CP::BndExpr<bool> operator()(CPSolverImpl& solver,const Predicate& p) const
	{
		typedef typename PredFunctorToCasper<f>::Type 	Func;
		typedef typename SymbolToCasper<e1,cons>::Type 	Op1;

		Op1 op1(SymbolToCasper<e1,cons>()(solver,p[0]));
		return Casper::CP::BndExpr<bool>(solver.solver,Casper::Rel1<Func,Op1>(op1));
	}
};

template<Predicate::Functor f,Eval e1,Eval e2,Consistency cons>
struct Pred2ToCasper
{
	Casper::CP::BndExpr<bool> operator()(CPSolverImpl& solver,const Predicate& p) const
	{
		typedef typename PredFunctorToCasper<f>::Type 	Func;
		typedef typename SymbolToCasper<e1,cons>::Type 	Op1;
		typedef typename SymbolToCasper<e2,cons>::Type 	Op2;

		Op1 op1(SymbolToCasper<e1,cons>()(solver,p[0]));
		Op2 op2(SymbolToCasper<e2,cons>()(solver,p[1]));
		return Casper::CP::BndExpr<bool>(solver.solver,Casper::Rel2<Func,Op1,Op2>(op1,op2));
	}
};


template<Predicate::Functor f,Eval e1,Eval e2,Eval e3,Consistency cons>
struct Pred3ToCasper
{
	Casper::CP::BndExpr<bool> operator()(CPSolverImpl& solver,const Predicate& p) const
	{
		typedef typename PredFunctorToCasper<f>::Type 	Func;
		typedef typename SymbolToCasper<e1,cons>::Type 	Op1;
		typedef typename SymbolToCasper<e2,cons>::Type 	Op2;
		typedef typename SymbolToCasper<e3,cons>::Type 	Op3;

		Op1 op1(SymbolToCasper<e1,cons>()(solver,p[0]));
		Op2 op2(SymbolToCasper<e2,cons>()(solver,p[1]));
		Op3 op3(SymbolToCasper<e3,cons>()(solver,p[2]));
		return Casper::CP::BndExpr<bool>(solver.solver,Casper::Rel3<Func,Op1,Op2,Op3>(op1,op2,op3));
	}
};


template<class Ret,Expression::Operator f,Eval e1,Consistency cons>
struct Expr1ToCasper
{
	Ret operator()(CPSolverImpl& solver,const Expression& p) const
	{
		typedef typename ExprFunctorToCasper<f>::Type 	Func;
		typedef typename SymbolToCasper<e1,cons>::Type 	Op1;
		Op1 op1(SymbolToCasper<e1,cons>()(solver,p[0]));
		return Ret(solver.solver,Casper::Rel1<Func,Op1>(op1));
	}
};

template<class Ret,Expression::Operator f,Eval e1,Eval e2,Consistency cons>
struct Expr2ToCasper
{
	Ret operator()(CPSolverImpl& solver,const Expression& p) const
	{
		typedef typename ExprFunctorToCasper<f>::Type 	Func;
		typedef typename SymbolToCasper<e1,cons>::Type 	Op1;
		typedef typename SymbolToCasper<e2,cons>::Type 	Op2;

		Op1 op1(SymbolToCasper<e1,cons>()(solver,p[0]));
		Op2 op2(SymbolToCasper<e2,cons>()(solver,p[1]));
		return Ret(solver.solver,Casper::Rel2<Func,Op1,Op2>(op1,op2));
	}
};

template<class Ret,Expression::Operator f,Eval e1,Eval e2,Eval e3,Consistency cons>
struct Expr3ToCasper
{
	Ret operator()(CPSolverImpl& solver,const Expression& p) const
	{
		typedef typename ExprFunctorToCasper<f>::Type 	Func;
		typedef typename SymbolToCasper<e1,cons>::Type 	Op1;
		typedef typename SymbolToCasper<e2,cons>::Type 	Op2;
		typedef typename SymbolToCasper<e3,cons>::Type 	Op3;

		Op1 op1(SymbolToCasper<e1,cons>()(solver,p[0]));
		Op2 op2(SymbolToCasper<e2,cons>()(solver,p[1]));
		Op3 op3(SymbolToCasper<e3,cons>()(solver,p[2]));
		return Ret(solver.solver,Casper::Rel3<Func,Op1,Op2,Op3>(op1,op2,op3));
	}
};

#include "casperpreds.h"

bool CPSolverImpl::addConstraints(const Instance& instance)
{
	for (SymbolSet::Iterator it = instance.getConstraints().begin();
			it != instance.getConstraints().end(); ++it)
	{
		const Predicate& p = *it;
		assert(getEval(p)==eBoolExpr);
		if (!solver.post(SymbolToCasper<eBoolExpr,cBounds>()(*this,p)))
			return false;
	}
	return true;
}

bool CPSolverImpl::solve()
{
	// FIXME
	bool r;
	if (nSols == 0)
		r = solver.solve(label(solver,intVars));
	if (nSols>0)
		r = solver.next();
	nSols += r;
	return r;
}

void CPSolverImpl::printSolution()
{
	std::cout << intVars << std::endl;
}

void CPSolverImpl::printStats()
{
	std::cout << solver.getStats() << std::endl;
	std::cout << solver.getCPUTimer() << std::endl;
}


void countVars(const Variable& v,int& nBoolVars, int& nIntVars, int& nRealVars)
{
	switch (v.getVariableType())
	{
		case Container::sBool:
			++nBoolVars;
			break;
		case Container::sInt:
			++nIntVars;
			break;
		case Container::sDouble:
			++nRealVars;
			break;
		default:
			throw EStructure("Variable",v);
	}
}

CPSolver::CPSolver(const Instance& instance)
{
	// count int vars and set vars
	int nBoolVars,nIntVars,nIntSetVars,nBoolSetVars,nRealVars;
	nBoolVars = nIntVars = nIntSetVars = nBoolSetVars = nRealVars = 0;
	for (SymbolSet::Iterator it = instance.getVariables().begin();
			it != instance.getVariables().end(); ++it)
		if (it->getType(true) == Symbol::sArray)
		{
			const SymbolArray& s = *it;
			for (int i = 0; i < s.getSize(); ++i)
				if (s[i].getType(true) == Container::sVariable and
					not instance.getIndex().hasKeys(s[i]))
					countVars(static_cast<const Variable&>(s[i]),nBoolVars,nIntVars,nRealVars);
		}
		else
		countVars(static_cast<const Variable&>(*it),nBoolVars,nIntVars,nRealVars);

	// create solver instance
	pImpl = new CPSolverImpl(nBoolVars,nIntVars,nIntSetVars,nBoolSetVars,nRealVars);

	// add variables
	pImpl->addVariables(instance);

	// add constraints
	pImpl->addConstraints(instance);
}

bool CPSolver::solve()
{
	return pImpl->solve();
}

int CPSolver::getIntVariableValue(const Variable& v) const
{
	return pImpl->getIntVariableValue(v);
}

bool CPSolver::getBoolVariableValue(const Variable& v) const
{
	return pImpl->getBoolVariableValue(v);
}

void CPSolver::printSolution()
{
	pImpl->printSolution();
}

void CPSolver::printStats()
{
	pImpl->printStats();
}

}
}
