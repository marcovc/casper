/***************************************************************************
 *   Copyright (C) 2006/2007 by Marco Correia                              *
 *   mvc@di.fct.unl.pt													   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _H_CASPER_KERNEL_GOAL_SCHEDULER
#define _H_CASPER_KERNEL_GOAL_SCHEDULER

#include <vector>
#include <casper/kernel/common.h>
#include <casper/kernel/filter/common.h>
#include <casper/kernel/goal/common.h>
#include <casper/kernel/container/rsulist.h>
#include <casper/kernel/container/rlist.h>
#include <casper/kernel/container/list.h>
#include <casper/kernel/container/rstack.h>
#include <casper/kernel/container/slist.h>
#include <casper/kernel/debug.h>
#include <casper/kernel/view/arrayview.h>	// temp

namespace casper {

// goal scheduler

struct IGoal;

struct TreeSearchGoalSched : IGoalSched
{
    typedef TreeSearchGoalSched		Self;
	typedef IGoal*					PIGoal;
    typedef detail::RStack<PIGoal> 	Goals;

    Bool						mValid;
    Reversible<PIGoal>    		pCurGoal;
    Reversible<PIGoal>    		pLeftGoal;
    Reversible<PIGoal>    		pRightGoal;
    Goals                       goals;
	Bool						fallBack;

    TreeSearchGoalSched(ICPSolver& mSolver);
    ~TreeSearchGoalSched();

	bool execute(Goal g)
    {
    	solver().insertCP();	// everything is undone if the solver fails
    	pCurGoal = g.getPImpl();
		return executeLoop();
	}

	void fail()
	{	mValid = false;	}

	bool next();

	void restart()
	{
		while (pCurGoal!=NULL)
			branch();
		mValid = true;
		//solver().removeCP();
	}
	void pushAnd(Goal g)
	{	goals.push(g.getPImpl());	}

	void pushOr(Goal left,Goal right)
	{
		pLeftGoal = left.getPImpl();
		pRightGoal = right.getPImpl();
		UInt branch = insertCP();
		//Debug::log(this,"created CP at depth %d",getCurPath().getDepth());
		if (branch == 0)
		{
			goals.push(pLeftGoal);
			//Debug::log(this,"branched left");
		}
		else
		{
			goals.push(pRightGoal);
			//Debug::log(this,"branched right");
		}
	}

	void setFallBack(Bool value)
	{	fallBack = value;	}

	protected:

	// returns 0 if search should follow left child, 1 for right child
	virtual UInt insertCP() = 0;
	// returns 0 if search should follow left child, 1 for right child
	virtual UInt branchCP() = 0;
	// called when the solver is at a leaf. Returns 1 to accept it or 0 to refuse it
	virtual Bool acceptSolution() = 0;
	// returns true if node is known to succeed (e.g. when revisiting path), false otherwise
	virtual Bool mustSucceed() const { return false; }

	// called by children to signal a new CP
	void signalCPCreate()	{   solver().stats().signalCPCreate();	}
	// called by children to signal a CP restore
	void signalCPRestore()	{	solver().stats().signalCPRestore();	}
	// called by children to signal a CP removal
	void signalCPRemove()	{	solver().stats().signalCPRemove();	}

	void branch()
	{
		UInt branch = branchCP();
		//Debug::log(this,"backtracked to CP at depth %d",getCurPath().getDepth());
		if (branch == 0)
		{
			pCurGoal = pLeftGoal;
			//Debug::log(this,"branched left");
		}
		else
		{
			pCurGoal = pRightGoal;
			//Debug::log(this,"branched right");
		}
	}

	bool executeLoop();
};

struct LIFOSearchPath : ISearchPath
{
	LIFOSearchPath(Heap heap) : fails(heap),depth(0),
							rightDepth(0),eval(0) {}

	UInt getFailCount() const	{	return *fails.begin();	}
	UInt getDepth() const		{	return depth;	}
	UInt getRightDepth() const	{	return rightDepth;	}
	Double getEvaluation() const	{	return eval;	}
	Double getRank(Int treeDepth = -1) const;

	//protected:
	typedef	detail::SList<UInt>	Fails;

	Fails				fails;
	UInt				depth;
	UInt				rightDepth;
	Double				eval;
};

std::ostream& operator<<(std::ostream& os, const LIFOSearchPath& l);

/**
 * Maintains a single path from the root to the current node. This means that
 * it is able to go from a given node to its direct children or parent. It is
 * unable to jump between branches.
 */
struct SinglePathGoalSched : TreeSearchGoalSched
{
    typedef SinglePathGoalSched	Self;

    SinglePathGoalSched(ICPSolver& mSolver) :
    	TreeSearchGoalSched(mSolver),
    	path(mSolver.heap()) {}	// this was sHeap, probably for a good reason...

//	protected:
	virtual Bool discard(Bool) = 0; //const = 0;

//	Void resetSearch();
	const ISearchPath& getCurPath() const
	{	return path;	}

	private:
	UInt insertCP();
	UInt branchCP();
	Bool acceptSolution()	{	return !discard(true);	}
	LIFOSearchPath				path;
};

struct DFSGoalSched : SinglePathGoalSched
{
    DFSGoalSched(ICPSolver& mSolver) :
    	SinglePathGoalSched(mSolver) {}

	Bool discard(Bool) //const
	{	return false;	}
};

DFSGoalSched* dfs(ICPSolver& solver);

// always follows the left branches - or fail
struct GreedyGoalSched : SinglePathGoalSched
{
	GreedyGoalSched(ICPSolver& solver) :
		SinglePathGoalSched(solver) {}

	bool discard(Bool) // const
	{	return getCurPath().getFailCount()==1;	}
};

IGoalSched* greedy(ICPSolver& solver);

struct DDSIteration : SinglePathGoalSched
{
	DDSIteration(ICPSolver& mSolver,UInt k) :
		SinglePathGoalSched(mSolver),k(k) {}
	Bool discard(Bool) //const
	{
		if (getCurPath().getDepth() == k and
			getCurPath().getFailCount() == 0)
			return true;
		if (getCurPath().getDepth() > k and
			getCurPath().getFailCount() == 1)
			return true;
		return false;
	}
	const UInt k;
};

IGoalSched* dds(ICPSolver& solver,UInt k);

// Incomplete Limited Discrepancy Search
struct LDSIteration : SinglePathGoalSched
{
    LDSIteration(ICPSolver& mSolver,UInt maxDiscr) :
    	SinglePathGoalSched(mSolver),
    	maxDiscr(maxDiscr) {}

	Bool discard(Bool) //const
	{	return getCurPath().getRightDepth() > maxDiscr;	}

	const UInt maxDiscr;
};

IGoalSched* lds(ICPSolver& solver,UInt maxDiscr);

template<class> struct BndFilterView;

// generic Branch and Bound search
struct BBGoalSched : SinglePathGoalSched
{
	BBGoalSched(ICPSolver& mSolver) :
		SinglePathGoalSched(mSolver),postDepth(0) {}

	template<class Rel>
	Void constrain(const Rel& r)
	{
		pFilter = new BndFilterView<Rel>(solver(),r);
		postDepth = getCurPath().getDepth();
	}

	Void constrain(IFilter* p)
	{
		pFilter = p;
		postDepth = getCurPath().getDepth();
	}

	Bool discard(Bool) //const
	{
		if (getCurPath().getDepth() < postDepth)
		{
			postDepth = getCurPath().getDepth();
			solver().post(Filter(pFilter));
		}

		return false;
	}

	IFilter*		pFilter;
	UInt			postDepth;
};

/*
struct OptimPivot : IGoal
{
	OptimPivot(ICPSolver& solver,SinglePathGoalSched& gs,Bool& at) :
		IGoal(solver),gs(gs),at(at) {}
	Goal execute()
	{
		std::cout << "at optimization pivot\n";
		at = true;
		Bool r = gs.discard();
		at = false;
		if (r)
			return fail();
		else
			return ok();
	}
	SinglePathGoalSched& gs;
	Bool& at;
};
Goal optPivot(ICPSolver& solver,SinglePathGoalSched& gs,Bool& at);
*/

// minimize using Branch and Bound search
template<class Eval,class View>
struct BBMinimizeGoalSched : SinglePathGoalSched
{
	BBMinimizeGoalSched(ICPSolver& mSolver,const View& v) :
		SinglePathGoalSched(mSolver),v(mSolver,v),
			best(limits<Eval>::max()) {}

//	bool execute(Goal g)
//	{	return SinglePathGoalSched::execute(g and optPivot(solver(),*this,atSolution));}

	Bool discard(Bool atSolution) //const
	{
		if (v.min() > best)
			return true;

		if (atSolution)
		{
			if (v.max() > best)	// since v may not be ground
				return true;
			best = v.max()-1;
			return false;
		}
		return !v.updateMax(best);
	}

	BndView<Eval,View>		v;
	Eval					best;
//	Bool					atSolution;
};

template<class View>
IGoalSched* bbMinimize(ICPSolver& solver,const View& v)
{	return new (solver.heap())
	BBMinimizeGoalSched<typename EvalTraits<View>::Eval,View>(solver,v);	}

struct LimitFails : SinglePathGoalSched
{
	LimitFails(UInt n, SinglePathGoalSched* search) :
		SinglePathGoalSched(search->solver()),
		n(n+solver().stats().fails()),search(search) {}
	Bool discard(Bool atSolution)
	{
		if (solver().stats().fails() >= n)
			return true;
		else
			return search->discard(atSolution);
	}
	Void reset(UInt n)
	{	this->n = solver().stats().fails()+n; }

	Counter n;
	SinglePathGoalSched* search;
};

IGoalSched* limitFails(UInt n, SinglePathGoalSched* s);


// Monotone Search Path: Allows expansion only. Iteration is also possible.
struct MonoSearchPath : ISearchPath
{
	MonoSearchPath(Heap heap) :
		fails(heap),curPosIt(fails.begin()),
		depth(0),rightDepth(0),eval(0) {}
	MonoSearchPath(const MonoSearchPath& s) :
		fails(s.fails),curPosIt(fails.begin()),depth(0),
		rightDepth(s.rightDepth),eval(s.eval)
	{	goToDepth(s.depth);	}

	UInt getFailCount() const	{	return *curPosIt;	}
	UInt getDepth() const		{	return depth;	}
	UInt getRightDepth() const	{	return rightDepth;	}
	Double getEvaluation() const	{	return eval;	}

	void insertCP();

	// \pre current position is deepest level
	MonoSearchPath* getPSibling();

	void goToDepth(UInt d);
	UInt getLastCommCPDepth(const MonoSearchPath& s);

	typedef	detail::SList<UInt>	Fails;

	Fails				fails;
	Fails::Iterator		curPosIt;
	UInt				depth;
	UInt				rightDepth;
	Double				eval;
};

#define WORKING

#ifdef WORKING

// Monotone Search Path: Allows expansion only. Iteration is also possible.
struct MonoSearchPathB : ISearchPath
{
	MonoSearchPathB(Heap heap) :
		fails(heap),curPosIt(fails.begin()),
		depth(0),rightDepth(0),eval(0) {}


	UInt getFailCount() const	{	return *curPosIt;	}
	UInt getDepth() const		{	return depth;	}
	UInt getRightDepth() const	{	return rightDepth;	}
	Double getEvaluation() const	{	return eval;	}

	Void insertCP();
	Bool atLeaf() const { return fails.empty() or curPosIt == --fails.end(); }

	typedef	detail::List<UInt>	Fails;

	Fails				fails;
	Fails::Iterator		curPosIt;
	UInt				depth;
	UInt				rightDepth;
	Double				eval;
};

/**
 *  This implements trailing + adaptive recomputation (a la gecode). It works
 *  but it only improves runtime about 3% when rd=2, and degrades for rd>2.
 *  Adaptive recomputation helps a bit but it doesn't makes it usable.
 *  \deprecated This class will be removed.
 */
// goal sched capable of doing recomputation
struct SinglePathGoalSchedR : TreeSearchGoalSched
{
	SinglePathGoalSchedR(ICPSolver& mSolver,UInt rd=8) :
    	TreeSearchGoalSched(mSolver),
    	path(mSolver.sHeap()),
    	rd(rd)
    	{ cps.push(0);	}

	protected:
	const ISearchPath& getCurPath() const
	{	return path;	}

	private:
	UInt insertCP();
	UInt branchCP();
	Bool acceptSolution()	{	return true;	}
	Bool mustSucceed() const { return false; /*not path.atLeaf();*/ }
	MonoSearchPathB					path;
	detail::Stack<Counter>			cps;
	UInt							rd;
};
#endif

struct MultiplePathGoalSched : TreeSearchGoalSched
{
    MultiplePathGoalSched(ICPSolver& mSolver, Int maxPaths = -1) :
    	TreeSearchGoalSched(mSolver),
    	pCurPath(new MonoSearchPath(stdHeap)),
    	openPaths(stdHeap),
    	recomputeCount(0),
    	maxPaths(maxPaths)
    	{}

	protected:
	virtual Double evaluate() const = 0;
	virtual Bool postpone() const = 0;

	const ISearchPath& getCurPath() const
	{	assert(pCurPath); return *pCurPath;	}
	const ISearchPath& getBestPath() const
	{	assert(!openPaths.empty()); return **openPaths.begin();	}

	private:
    typedef MultiplePathGoalSched		Self;
	typedef MonoSearchPath*				PSearchPath;
	typedef detail::List<PSearchPath>	OpenPaths;

	bool pushToOpenPaths(PSearchPath pPath);
	void debugOpenPaths();

	void goUp(UInt count);
	void switchToBestPath();
	UInt insertCP();
	UInt branchCP();
	Bool acceptSolution() {	return !postpone();	}

	PSearchPath						pCurPath;
	OpenPaths						openPaths;
	Int								recomputeCount;
	const Int						maxPaths;
};

// Depth First Search using MultiplePathGoalSched
struct DFSMPGoalSched : MultiplePathGoalSched
{
    DFSMPGoalSched(ICPSolver& mSolver) :
    	MultiplePathGoalSched(mSolver) {}

	Double evaluate() const
	{	return -Double(getCurPath().getDepth());	}
	Bool postpone() const
	{	return false;	}
};

IGoalSched* dfsMP(ICPSolver& solver);

// Limited Discrepancy Search using MultiplePathGoalSched
struct LDSMPGoalSched : MultiplePathGoalSched
{
    LDSMPGoalSched(ICPSolver& mSolver) :
    	MultiplePathGoalSched(mSolver) {}

	Double evaluate() const
	{	return getCurPath().getRightDepth();	}
	Bool postpone() const
	{	return evaluate() > getBestPath().getEvaluation();	}
};

IGoalSched* ldsMP(ICPSolver& solver);


// Goal for changing current goal scheduler
struct Explore : IGoal
{
	Explore(ICPSolver& solver, IGoalSched* gs, Goal g) :
		IGoal(solver),gs(gs),g(g),first(true) {}
	Goal execute(IGoalSched& s)
	{
		solver().setGoalSched(gs);
		solver().goalSched().setFallBack(true);
		bool found = solver().solve(g);
		if (found)
			return ok();
		return fail();
	}
	IGoalSched*	gs;
	Goal 		g;
	Bool		first;
};

Goal explore(IGoalSched* gs, Goal g);


// Best First search using MultiplePathGoalSched
template<class Eval,class View>
struct BFGoalSched : MultiplePathGoalSched
{
	BFGoalSched(ICPSolver& mSolver,const View& v, Int maxPaths = -1) :
    	MultiplePathGoalSched(mSolver,maxPaths),v(mSolver,v) {}

	Double evaluate() const
	{	return v.min();	}
	Bool postpone() const
	{	return evaluate() > getBestPath().getEvaluation();	}

	BndView<Eval,View>	v;
};

template<class View>
IGoalSched* bfMinimize(ICPSolver& solver,const View& v, Int maxPaths = -1)
{	return new (solver.heap())
	BFGoalSched<typename EvalTraits<View>::Eval,View>(solver,v,maxPaths);	}


/*
IGoalSched* lds(ICPSolver& solver,UInt iMaxDiscr, UInt fMaxDiscr, UInt step=1);

// Range of Limited Discrepancy Search (Goal)
struct LDSRange : DFSGoalSched
{
    LDSRange(ICPSolver& mSolver,UInt initMaxDiscr,
    		 UInt finalMaxDiscr,UInt step) :
    	DFSGoalSched(mSolver),
    	curMaxDiscr(initMaxDiscr),
    	finalMaxDiscr(finalMaxDiscr),
    	step(step) {}

	virtual bool execute(Goal searchTree);

	const UInt curMaxDiscr;
	const UInt finalMaxDiscr;
	const UInt step;
};*/

std::ostream& operator<<(std::ostream& os, const casper::MonoSearchPath& s);

};


#endif
