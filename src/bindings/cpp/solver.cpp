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
#include <casper/int.h>
#include <casper/int/cumulative/cumulative.h>
#include <casper/int/table/positive.h>

#include <casper/set/config.h> // temp
#include <casper/set.h>
#include <casper/kernel/container/hashmap.h>
#include <casper/kernel/container/range.h>

#include "cpp.h"
#include "eval.h"
#include "print.h"	// tmp

namespace casperbind {
namespace cpp {

enum Consistency { cDomain, cBounds, cValue };

template<Eval eval,Consistency cons>
struct SymbolToCasper;

struct ICPSolver&Impl
{
	casper::ICPSolver& solver;
	casper::DomVarArray<casper::Bool> boolVars;
	casper::DomVarArray<casper::Int> intVars;
	casper::DomVarArray<casper::IntSet> intSetVars;
	casper::DomVarArray<casper::BoolSet> boolSetVars;
	casper::DomVarArray<casper::Double> realVars;
	typedef casper::detail::HashMap<const Variable*,int> VarIdMap;
	VarIdMap	boolVarIds;
	VarIdMap	intVarIds;
	VarIdMap	realVarIds;
	VarIdMap	intSetVarIds;
	VarIdMap	boolSetVarIds;
	int 		nSols;

	ICPSolver&Impl(int nBoolVars, int nIntVars, int nIntSetVars, int nBoolSetVars,int nRealVars) :
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

int ICPSolver&Impl::getIntVariableValue(const Variable& v)
{
	const casper::DomVar<casper::Int>& r = intVars[intVarIds[&v]];
	assert(r.ground());
	return r.domain().value();
}

bool ICPSolver&Impl::getBoolVariableValue(const Variable& v)
{
	const casper::DomVar<casper::Bool>& r = boolVars[boolVarIds[&v]];
	assert(r.ground());
	return r.domain().value();
}

void ICPSolver&Impl::addVariable(const Variable& v)
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
					intVars[varId] = casper::DomVar<casper::Int>(solver,s);
					break;
				}
				case Symbol::sRange:
				{
					const IntRange& r = v.getDomain();
					intVars[varId] = casper::DomVar<casper::Int>(solver,r.getLower(),r.getUpper());
					break;
				}
				case Symbol::sSet:
				{
					const IntSet& s = v.getDomain();
					intVars[varId] = casper::DomVar<casper::Int>(solver,s.begin(),s.end());
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
					boolVars[varId] = casper::DomVar<casper::Bool>(solver,s);
					break;
				}
				case Symbol::sSet:
				{
					const BoolSet& s = v.getDomain();
					boolVars[varId] = casper::DomVar<casper::Bool>(solver,*s.begin(),*--s.end());
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

void ICPSolver&Impl::addVariables(const Instance& instance)
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
{	typedef casper::Add	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eSub>
{	typedef casper::Sub	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eMul>
{	typedef casper::Mul	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eDiv>
{	typedef casper::Div	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eMod>
{	typedef casper::Mod	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eSym>
{	typedef casper::Sym	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eAbs>
{	typedef casper::Abs Type;	};

template<>
struct ExprFunctorToCasper<Expression::ePow>
{	typedef casper::Pow	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eMin>
{	typedef casper::Min	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eMax>
{	typedef casper::Max	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eIfThenElse>
{	typedef casper::IfThenElse	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eElem>
{	typedef casper::Element	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eCard>
{	typedef casper::Cardinal	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eIntersect>
{	typedef casper::Mul	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eUnion>
{	typedef casper::Add	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eSymDiff>
{	typedef casper::SymDiff	Type;	};

template<>
struct ExprFunctorToCasper<Expression::eSumProduct>
{	typedef casper::SumProduct	Type;	};

// --

template<Predicate::Functor>
struct PredFunctorToCasper;

template<>
struct PredFunctorToCasper<Predicate::pEqual>
{	typedef casper::Equal	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pDistinct>
{	typedef casper::Distinct	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pLess>
{	typedef casper::Less	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pLessEqual>
{	typedef casper::LessEqual	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pGreater>
{	typedef casper::Greater	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pGreaterEqual>
{	typedef casper::GreaterEqual	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pNot>
{	typedef casper::Not	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pAnd>
{	typedef casper::And	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pOr>
{	typedef casper::Or	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pXOr>
{	typedef casper::XOr	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pIff>
{	typedef casper::Equal	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pIf>
{	typedef casper::LessEqual	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pContained>
{	typedef casper::Contained	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pMember>
{	typedef casper::Member	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pInTable>
{	typedef casper::InTable	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pNotInTable>
{	typedef casper::NotInTable	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pDisjoint>
{	typedef casper::Disjoint	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pCumulative>
{	typedef casper::Cumulative	Type;	};

template<>
struct PredFunctorToCasper<Predicate::pPartition>
{	typedef casper::Partition	Type;	};

// --

template<Consistency cons> struct SymbolToCasper<eBool,cons>
{
	typedef casper::Bool Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{	return static_cast<const Type&>(s); }
};

template<Consistency cons> struct SymbolToCasper<eInt,cons>
{
	typedef casper::Int Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{	return static_cast<const Type&>(s); }
};

template<Consistency cons> struct SymbolToCasper<eReal,cons>
{
	typedef casper::Double Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{	return static_cast<const Type&>(s); }
};

template<Consistency cons> struct SymbolToCasper<eIntSet,cons>
{
	typedef casper::Array<casper::Int> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{
		const IntSet& a = s;
		Type r(solver.solver.heap(),a.getSize());
		int c = 0;
		for (IntSet::Iterator it = a.begin(); it != a.end(); ++it)
			r[c++] = *it;
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eBoolSet,cons>
{
	typedef casper::Array<casper::Bool> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{
		const BoolSet& a = s;
		Type r(solver.solver.heap(),a.getSize());
		int c = 0;
		for (BoolSet::Iterator it = a.begin(); it != a.end(); ++it)
			r[c++] = *it;
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eBoolArray,cons>
{
	typedef casper::Array<casper::Bool> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{
		const BoolArray& a = s;
		Type r(solver.solver.heap(),a.getSize());
		for (int i = 0; i < a.getSize(); ++i)
			r[i] = a[i];
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eIntArray,cons>
{
	typedef casper::Array<casper::Int> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{
		const IntArray a = s; // tmp (copy should be by const ref)
		Type r(solver.solver.heap(),a.getSize());
		for (int i = 0; i < a.getSize(); ++i)
			r[i] = a[i];
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eRealArray,cons>
{
	typedef casper::Array<casper::Double> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{
		const RealArray& a = s;
		Type r(solver.solver.heap(),a.getSize());
		for (int i = 0; i < a.getSize(); ++i)
			r[i] = a[i];
		return r;
	}
};

template<Consistency cons> struct SymbolToCasper<eIntRange,cons>
{
	typedef casper::Range<casper::Int> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{
		const IntRange& a = s;
		return Type(a.getLower(),a.getUpper());
	}
};

template<Consistency cons> struct SymbolToCasper<eRealRange,cons>
{
	typedef casper::Range<casper::Double> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{
		const RealRange& a = s;
		return Type(a.getLower(),a.getUpper());
	}
};

template<Consistency cons> struct SymbolToCasper<eBoolVar,cons>
{
	typedef casper::DomVar<casper::Bool> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{
		const Variable& v = s;
		typename ICPSolver&Impl::VarIdMap::const_iterator it = solver.boolVarIds.find(&v);
		if (it == solver.boolVarIds.end())
			throw ENoKeyForSymbol(s);
		return solver.boolVars[it->second];
	}
};

template<Consistency cons> struct SymbolToCasper<eIntVar,cons>
{
	typedef casper::DomVar<casper::Int> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{
		const Variable& v = s;
		typename ICPSolver&Impl::VarIdMap::const_iterator it = solver.intVarIds.find(&v);
		if (it == solver.intVarIds.end())
			throw ENoKeyForSymbol(s);
		return solver.intVars[it->second];
	}
};

template<Consistency cons> struct SymbolToCasper<eRealVar,cons>
{
	typedef casper::DomVar<casper::Double> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{
		const Variable& v = s;
		return solver.realVars[solver.realVarIds[&v]];
	}
};

template<Consistency cons> struct SymbolToCasper<eIntSetVar,cons>
{
	typedef casper::DomVar<casper::IntSet> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{
		const Variable& v = s;
		return solver.intSetVars[solver.intSetVarIds[&v]];
	}
};

template<Consistency cons> struct SymbolToCasper<eBoolSetVar,cons>
{
	typedef casper::DomVar<casper::BoolSet> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
	{
		const Variable& v = s;
		return solver.boolSetVars[solver.boolSetVarIds[&v]];
	}
};

template<Consistency cons> struct SymbolToCasper<eIntVarArray,cons>
{
	typedef casper::DomVarArray<casper::Int> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
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
	typedef casper::DomVarArray<casper::Double> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
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
	typedef casper::DomVarArray<casper::Bool> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
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
	typedef casper::DomVarArray<casper::IntSet> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
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
	typedef casper::DomVarArray<casper::BoolSet> Type;
	Type operator()(ICPSolver& solver,const Symbol& s)
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
	casper::BndExpr<casper::Bool> operator()(ICPSolver& solver,const Predicate& p) const
	{
		typedef typename PredFunctorToCasper<f>::Type 	Func;
		typedef typename SymbolToCasper<e1,cons>::Type 	Op1;

		Op1 op1(SymbolToCasper<e1,cons>()(solver,p[0]));
		return casper::BndExpr<casper::Bool>(solver.solver,casper::Rel1<Func,Op1>(op1));
	}
};

template<Predicate::Functor f,Eval e1,Eval e2,Consistency cons>
struct Pred2ToCasper
{
	casper::BndExpr<casper::Bool> operator()(ICPSolver& solver,const Predicate& p) const
	{
		typedef typename PredFunctorToCasper<f>::Type 	Func;
		typedef typename SymbolToCasper<e1,cons>::Type 	Op1;
		typedef typename SymbolToCasper<e2,cons>::Type 	Op2;

		Op1 op1(SymbolToCasper<e1,cons>()(solver,p[0]));
		Op2 op2(SymbolToCasper<e2,cons>()(solver,p[1]));
		return casper::BndExpr<casper::Bool>(solver.solver,casper::Rel2<Func,Op1,Op2>(op1,op2));
	}
};

template<Predicate::Functor f,Eval e1,Eval e2,Eval e3,Consistency cons>
struct Pred3ToCasper
{
	casper::BndExpr<casper::Bool> operator()(ICPSolver& solver,const Predicate& p) const
	{
		typedef typename PredFunctorToCasper<f>::Type 	Func;
		typedef typename SymbolToCasper<e1,cons>::Type 	Op1;
		typedef typename SymbolToCasper<e2,cons>::Type 	Op2;
		typedef typename SymbolToCasper<e3,cons>::Type 	Op3;

		Op1 op1(SymbolToCasper<e1,cons>()(solver,p[0]));
		Op2 op2(SymbolToCasper<e2,cons>()(solver,p[1]));
		Op3 op3(SymbolToCasper<e3,cons>()(solver,p[2]));
		return casper::BndExpr<casper::Bool>(solver.solver,casper::Rel3<Func,Op1,Op2,Op3>(op1,op2,op3));
	}
};


template<class Ret,Expression::Operator f,Eval e1,Consistency cons>
struct Expr1ToCasper
{
	Ret operator()(ICPSolver& solver,const Expression& p) const
	{
		typedef typename ExprFunctorToCasper<f>::Type 	Func;
		typedef typename SymbolToCasper<e1,cons>::Type 	Op1;
		Op1 op1(SymbolToCasper<e1,cons>()(solver,p[0]));
		return Ret(solver.solver,casper::Rel1<Func,Op1>(op1));
	}
};

template<class Ret,Expression::Operator f,Eval e1,Eval e2,Consistency cons>
struct Expr2ToCasper
{
	Ret operator()(ICPSolver& solver,const Expression& p) const
	{
		typedef typename ExprFunctorToCasper<f>::Type 	Func;
		typedef typename SymbolToCasper<e1,cons>::Type 	Op1;
		typedef typename SymbolToCasper<e2,cons>::Type 	Op2;

		Op1 op1(SymbolToCasper<e1,cons>()(solver,p[0]));
		Op2 op2(SymbolToCasper<e2,cons>()(solver,p[1]));
		return Ret(solver.solver,casper::Rel2<Func,Op1,Op2>(op1,op2));
	}
};

template<class Ret,Expression::Operator f,Eval e1,Eval e2,Eval e3,Consistency cons>
struct Expr3ToCasper
{
	Ret operator()(ICPSolver& solver,const Expression& p) const
	{
		typedef typename ExprFunctorToCasper<f>::Type 	Func;
		typedef typename SymbolToCasper<e1,cons>::Type 	Op1;
		typedef typename SymbolToCasper<e2,cons>::Type 	Op2;
		typedef typename SymbolToCasper<e3,cons>::Type 	Op3;

		Op1 op1(SymbolToCasper<e1,cons>()(solver,p[0]));
		Op2 op2(SymbolToCasper<e2,cons>()(solver,p[1]));
		Op3 op3(SymbolToCasper<e3,cons>()(solver,p[2]));
		return Ret(solver.solver,casper::Rel3<Func,Op1,Op2,Op3>(op1,op2,op3));
	}
};

#include "casperpreds.h"

bool ICPSolver&Impl::addConstraints(const Instance& instance)
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

bool ICPSolver&Impl::solve()
{
	// FIXME
	bool r;
	if (nSols == 0)
		r = solver.solve(label(intVars));
	if (nSols>0)
		r = solver.next();
	nSols += r;
	return r;
}

void ICPSolver&Impl::printSolution()
{
	cout << intVars << endl;
}

void ICPSolver&Impl::printStats()
{
	cout << solver.stats() << endl;
	cout << solver.totalTime() << endl;
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

ICPSolver::ICPSolver&(const Instance& instance)
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
	pImpl = new ICPSolver&Impl(nBoolVars,nIntVars,nIntSetVars,nBoolSetVars,nRealVars);

	// add variables
	pImpl->addVariables(instance);

	// add constraints
	pImpl->addConstraints(instance);
}

bool ICPSolver::solve()
{
	return pImpl->solve();
}

int ICPSolver::getIntVariableValue(const Variable& v) const
{
	return pImpl->getIntVariableValue(v);
}

bool ICPSolver::getBoolVariableValue(const Variable& v) const
{
	return pImpl->getBoolVariableValue(v);
}

void ICPSolver::printSolution()
{
	pImpl->printSolution();
}

void ICPSolver::printStats()
{
	pImpl->printStats();
}

}
}
