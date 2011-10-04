/***************************************************************************
 *   Copyright (C) 2006/2007 by Marco Correia                              *
 *   mvc@di.fct.unl.pt													   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _H_CASPER_KERNEL_GOAL_EXPLORER
#define _H_CASPER_KERNEL_GOAL_EXPLORER

#include <vector>
#include <casper/kernel/common.h>
//#include <casper/kernel/filter/common.h>
#include <casper/kernel/goal/goal.h>
//#include <casper/kernel/reversible/sulist.h>
//#include <casper/kernel/reversible/list.h>
#include <casper/util/container/stdlist.h>
#include <casper/kernel/reversible/stack.h>
#include <casper/util/container/stdslist.h>
#include <casper/util/debug.h>
//#include <casper/kernel/view/arrayview.h>	// temp

namespace Casper {

// goal scheduler

// here CP means choice point, not check point as in State
struct ExplorerStats
{
	counter countCPRestores;
	counter countCPs;
	counter maxDepth;
	counter curDepth;
	counter countSolutions;

	ExplorerStats() :
				countCPRestores(0),
				countCPs(0),
				maxDepth(0),
				curDepth(0),
				countSolutions(0){}

	void signalCPRestore()
	{ countCPRestores++; }

	void signalCPCreate()
	{
		countCPs++;
		curDepth++;
		maxDepth = std::max(maxDepth,curDepth);
	}

	void signalCPRemove()
	{ 	curDepth--; }

	void signalNewSolution()
	{ countSolutions++; }

	counter	getNbFails()	const
	{	return countCPRestores;	}

	counter getNbChoicePoints() const
	{	return countCPs;	}

	counter getMaxDepth() const
	{	return maxDepth;	}

	counter getCurDepth() const
	{	return curDepth;	}

	counter getNbSolutions() const
	{	return countSolutions;	}

};

struct IGoal;

/**
 * 	Defines an explorer. An explorer is used to explore a search tree (i.e. a Goal).
 * 	\ingroup Search
 *
 */
struct IExplorer
{
    typedef IExplorer				Self;
	typedef IGoal*					PIGoal;
    typedef Stack<PIGoal> 			Goals;

    State&						state;
    bool						mValid;
    Reversible<PIGoal>    		pCurGoal;
    Reversible<PIGoal>    		pLeftGoal;
    Reversible<PIGoal>    		pRightGoal;
    Goals                       goals;
	ExplorerStats				stats;

	IExplorer(State& state);
    ~IExplorer();

    /// Searches the Goal tree until the succeed Goal is found, in which case it returns \p true.
	bool explore(Goal g)
    {
    	state.insertCP();	// everything is undone if the solver fails
    	pCurGoal = g.getPImpl();
		return executeLoop();
	}

	/// Force the explorer to fail.
	void fail()
	{	mValid = false;	}

	/// Resumes search from the point where the last call to explore has exited.
	bool resume();

	/// Resets the search. \todo Change name to reset()
	void restart()
	{
		while (pCurGoal!=NULL)
			branch();
		mValid = true;
		//solver().removeCP();
	}

	void pushAnd(Goal g)
	{	assert(g.getPImpl()); goals.push(g.getPImpl());	}

	void pushOr(Goal left,Goal right)
	{
		pLeftGoal = left.getPImpl();
		pRightGoal = right.getPImpl();
		uint branch = insertCP();
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

	/// Returns search statistics.
	ExplorerStats& getStats() {	return stats; }
	/// Returns search statistics.
	const ExplorerStats& getStats() const {	return stats; }

	operator State&() {	return state; }
	operator const State&() const {	return state; }

	protected:

	// returns 0 if search should follow left child, 1 for right child
	virtual uint insertCP() = 0;
	// returns 0 if search should follow left child, 1 for right child
	virtual uint branchCP() = 0;
	// called when the solver is at a leaf. Returns 1 to accept it or 0 to refuse it
	virtual bool acceptSolution() = 0;
	// returns true if node is known to succeed (e.g. when revisiting path), false otherwise
	virtual bool mustSucceed() const { return false; }

	void branch()
	{
		uint branch = branchCP();
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
	LIFOSearchPath(Util::IHeap& heap) : fails(heap),depth(0),
							rightDepth(0),eval(0) {}

	uint getFailCount() const	{	return *fails.begin();	}
	uint getDepth() const		{	return depth;	}
	uint getRightDepth() const	{	return rightDepth;	}
	double getEvaluation() const	{	return eval;	}
	double getRank(int treeDepth = -1) const;

	// for debugging
	void getId(uint maxDepth, counter& lb, counter& ub) const;

	//protected:
	typedef	Util::StdSList<uint>	Fails;

	Fails				fails;
	uint				depth;
	uint				rightDepth;
	double				eval;
};

std::ostream& operator<<(std::ostream& os, const LIFOSearchPath& l);

/**
 * Maintains a single path from the root to the current node. This means that
 * it is able to go from a given node to its direct children or parent. It is
 * unable to jump between branches.
 */
struct ISinglePathExplorer : IExplorer
{
    typedef ISinglePathExplorer	Self;

    ISinglePathExplorer(State& state) :
    	IExplorer(state),
    	path(state) {}	// this was sHeap, probably for a good reason...

//	protected:
	virtual bool discard(bool) = 0; //const = 0;

//	void resetSearch();
	const ISearchPath& getCurPath() const
	{	return path;	}

	private:
	uint insertCP();
	uint branchCP();
	bool acceptSolution()	{	return !discard(true);	}
	LIFOSearchPath				path;
};

/**
 * 	Depth-First search. Explores the search tree in a depth-first, left to right fashion.
 * 	Search requires polynomial memory, but exponential time, in the worst case.
 * 	\ingroup Search
 *
 */
struct DFSExplorer : ISinglePathExplorer
{
    DFSExplorer(State& state) :
    	ISinglePathExplorer(state) {}

	bool discard(bool) //const
	{	return false;	}
};

/**
 * 	Returns a new DFSExplorer.
 *  \ingroup Search
 */
DFSExplorer* dfs(State& state);

/**
 * 	Greedy search. Always assume leftmost goal on all goal disjunctions,
 * 	and never backtrack. Search space and time are polynomial, but may fail
 * 	to find an existing solution.
 * 	\ingroup Search
 */
struct GreedyExplorer : ISinglePathExplorer
{
	GreedyExplorer(State& state) :
		ISinglePathExplorer(state) {}

	bool discard(bool) // const
	{	return getCurPath().getFailCount()==1;	}
};

/**
 * 	Returns a new GreedyExplorer.
 * 	\ingroup Search
 */
GreedyExplorer* greedy(State& state);

/**
 * 	Depth bounded discrepancy search. Only
 * 	branches above a given depth \a k.
 * 	\ingroup Search
 */
struct DDSIteration : ISinglePathExplorer
{
	DDSIteration(State& state,uint k) :
		ISinglePathExplorer(state),k(k) {}
	bool discard(bool) //const
	{
		if (getCurPath().getDepth() > k and
			getCurPath().getFailCount() == 1)
			return true;
		return false;
	}
	const uint k;
};

/**
 * 	Returns a new DDSIteration.
 * 	\ingroup Search
 */
DDSIteration* dds(State& state,uint k);

/**
 * 	 Limited Discrepancy Search. Only branches right
 * 	 at most \a maxDiscr times on any search path.
 * 	 \ingroup Search
 */

struct LDSIteration : ISinglePathExplorer
{
    LDSIteration(State& state,uint maxDiscr) :
    	ISinglePathExplorer(state),
    	maxDiscr(maxDiscr) {}

	bool discard(bool) //const
	{	return getCurPath().getRightDepth() > maxDiscr;	}

	const uint maxDiscr;
};

/**
 * 	Returns a new LDSIteration.
 * 	\ingroup Search
 */
LDSIteration* lds(State& state,uint maxDiscr);

// FIXME: put this BB explorer in CP
namespace CP {
struct IFilter;
}

/**
 * 	Branch and Bound optimization.
 * 	\ingroup Search
 */
template<class Store>
struct BBExplorer : ISinglePathExplorer
{
	BBExplorer(State& state,Store& store) :
		ISinglePathExplorer(state),store(store),postDepth(0) {}

	void constrain(CP::IFilter* p)
	{
		pFilter = p;
		postDepth = getCurPath().getDepth();
	}

	bool discard(bool) //const
	{
		if (getCurPath().getDepth() < postDepth)
		{
			postDepth = getCurPath().getDepth();
			store.post(pFilter);
		}

		return false;
	}

	Store&			store;
	CP::IFilter*		pFilter;
	uint			postDepth;
};

/*
struct OptimPivot : IGoal
{
	OptimPivot(CPSolver& solver,SinglePathExplorer& gs,bool& at) :
		IGoal(solver),gs(gs),at(at) {}
	Goal execute()
	{
		std::cout << "at optimization pivot\n";
		at = true;
		bool r = gs.discard();
		at = false;
		if (r)
			return fail();
		else
			return succeed();
	}
	SinglePathExplorer& gs;
	bool& at;
};
Goal optPivot(CPSolver& solver,SinglePathExplorer& gs,bool& at);
*/

// TODO: test this
struct LimitFails : ISinglePathExplorer
{
	LimitFails(State& state,uint n, ISinglePathExplorer* search) :
		ISinglePathExplorer(state),
		n(n+getStats().getNbFails()),search(search) {}
	bool discard(bool atSolution)
	{
		if (getStats().getNbFails() >= n)
			return true;
		else
			return search->discard(atSolution);
	}
	void reset(uint n)
	{	this->n = getStats().getNbFails()+n; }

	counter n;
	ISinglePathExplorer* search;
};

IExplorer* limitFails(State& state,uint n, ISinglePathExplorer* s);


// Monotone Search Path: allows expansion and Iteration only.
struct MonoSearchPath : ISearchPath
{
	MonoSearchPath(Util::IHeap& heap) :
		fails(heap),curPosIt(fails.begin()),
		depth(0),rightDepth(0),eval(0) {}
	MonoSearchPath(const MonoSearchPath& s) :
		fails(s.fails),curPosIt(fails.begin()),depth(0),
		rightDepth(s.rightDepth),eval(s.eval)
	{	goToDepth(s.depth);	}

	uint getFailCount() const	{	return *curPosIt;	}
	uint getDepth() const		{	return depth;	}
	uint getRightDepth() const	{	return rightDepth;	}
	double getEvaluation() const	{	return eval;	}

	void insertCP();

	// \pre current position is deepest level
	MonoSearchPath* getPSibling();

	void goToDepth(uint d);
	uint getLastCommCPDepth(const MonoSearchPath& s);

	typedef	Util::StdSList<uint>	Fails;

	Fails				fails;
	Fails::Iterator		curPosIt;
	uint				depth;
	uint				rightDepth;
	double				eval;
};

//#define WORKING

#ifdef WORKING

// Monotone Search Path: Allows expansion only. Iteration is also possible.
struct MonoSearchPathB : ISearchPath
{
	MonoSearchPathB(IHeap& heap) :
		fails(heap),curPosIt(fails.begin()),
		depth(0),rightDepth(0),eval(0) {}


	uint getFailCount() const	{	return *curPosIt;	}
	uint getDepth() const		{	return depth;	}
	uint getRightDepth() const	{	return rightDepth;	}
	double getEvaluation() const	{	return eval;	}

	void insertCP();
	bool atLeaf() const { return fails.empty() or curPosIt == --fails.end(); }

	typedef	Util::StdList<uint>	Fails;

	Fails				fails;
	Fails::Iterator		curPosIt;
	uint				depth;
	uint				rightDepth;
	double				eval;
};

/**
 *  This implements trailing + adaptive recomputation (a la gecode). It works
 *  but it only improves runtime about 3% when rd=2, and degrades for rd>2.
 *  Adaptive recomputation helps a bit but it doesn't makes it usable.
 *  \deprecated This class will be removed.
 */
// goal sched capable of doing recomputation
struct SinglePathExplorerR : IExplorer
{
	SinglePathExplorerR(State& state,uint rd=8) :
    	IExplorer(state),
    	path(state.sHeap()),
    	rd(rd)
    	{ cps.push(0);	}

	protected:
	const ISearchPath& getCurPath() const
	{	return path;	}

	private:
	uint insertCP();
	uint branchCP();
	bool acceptSolution()	{	return true;	}
	bool mustSucceed() const { return false; /*not path.atLeaf();*/ }
	MonoSearchPathB					path;
	Util::StdStack<Counter>			cps;
	uint							rd;
};
#endif

struct IMultiplePathExplorer : IExplorer
{
    IMultiplePathExplorer(State& state, int maxPaths = -1) :
    	IExplorer(state),
    	pCurPath(new MonoSearchPath(Util::stdHeap)),
    	openPaths(Util::stdHeap),
    	recomputeCount(0),
    	maxPaths(maxPaths)
    	{}

	protected:
	virtual double evaluate() const = 0;
	virtual bool postpone() const = 0;

	const ISearchPath& getCurPath() const
	{	assert(pCurPath); return *pCurPath;	}
	const ISearchPath& getBestPath() const
	{	assert(!openPaths.empty()); return **openPaths.begin();	}

	private:
    typedef IMultiplePathExplorer		Self;
	typedef MonoSearchPath*				PSearchPath;
	typedef Util::StdList<PSearchPath>	OpenPaths;

	bool pushToOpenPaths(PSearchPath pPath);
//	void debugOpenPaths();

	void goUp(uint count);
	void switchToBestPath();
	uint insertCP();
	uint branchCP();
	bool acceptSolution() {	return !postpone();	}

	PSearchPath						pCurPath;
	OpenPaths						openPaths;
	int								recomputeCount;
	const int						maxPaths;
};

// Depth First Search using MultiplePathExplorer
struct DFSMPExplorer : IMultiplePathExplorer
{
    DFSMPExplorer(State& state) :
    	IMultiplePathExplorer(state) {}

	double evaluate() const
	{	return -double(getCurPath().getDepth());	}
	bool postpone() const
	{	return false;	}
};

IExplorer* dfsMP(State& state);

// Limited Discrepancy Search using MultiplePathExplorer
struct LDSMPExplorer : IMultiplePathExplorer
{
    LDSMPExplorer(State& state) :
    	IMultiplePathExplorer(state) {}

	double evaluate() const
	{	return getCurPath().getRightDepth();	}
	bool postpone() const
	{	return evaluate() > getBestPath().getEvaluation();	}
};

IExplorer* ldsMP(State& state);




/*
IExplorer* lds(CPSolver& solver,uint iMaxDiscr, uint fMaxDiscr, uint step=1);

// StdRange of Limited Discrepancy Search (Goal)
struct LDSRange : DFSExplorer
{
    LDSRange(CPSolver& mSolver,uint initMaxDiscr,
    		 uint finalMaxDiscr,uint step) :
    	DFSExplorer(mSolver),
    	curMaxDiscr(initMaxDiscr),
    	finalMaxDiscr(finalMaxDiscr),
    	step(step) {}

	virtual bool execute(Goal searchTree);

	const uint curMaxDiscr;
	const uint finalMaxDiscr;
	const uint step;
};*/


Goal explore(State& state,IExplorer& gs, Goal g);



};

std::ostream& operator<<(std::ostream& os, const Casper::MonoSearchPath& s);
std::ostream& operator<<(std::ostream& os, const Casper::ExplorerStats& s);

#endif
