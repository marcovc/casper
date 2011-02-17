/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <casper/kernel/goal/scheduler.h>
#include <casper/kernel/goal/basic.h>

namespace casper {



// ******** TreeSearchGoalSched ***********

TreeSearchGoalSched::TreeSearchGoalSched(ICPSolver& mSolver) :
    	IGoalSched(mSolver),
    	mValid(true),
    	pCurGoal(mSolver,NULL),
    	pLeftGoal(mSolver,NULL),
    	pRightGoal(mSolver,NULL),
    	goals(mSolver),
    	fallBack(false)
{}

TreeSearchGoalSched::~TreeSearchGoalSched()
{}

bool TreeSearchGoalSched::next()
{
	assert(mValid);
	branch();
	if (pCurGoal == static_cast<IGoal*>(NULL))
	{
		if (!fallBack)
			return false;

		solver().restoreCP();
		signalCPRestore(); // new
		return solver().next();
	}
	return executeLoop();
}

bool TreeSearchGoalSched::executeLoop()
{
	for (EVER)
	{
		assert(pCurGoal);
		solver().log(this,"TreeSearchGoalSched",Logger::goalSchedIterate);
		//std::cout << solver().stats().curLevel << ": mustSucceed: " << mustSucceed() << std::endl;
		Goal g = pCurGoal->execute(*this);
		if (mValid and (/*mustSucceed() or*/ solver().valid()))
		{
			if (g.getPImpl() == NULL)
			{
				if (goals.empty() and acceptSolution())
				{
					solver().stats().signalNewSolution();
					return true;
				}
				pCurGoal = goals.top();
				goals.pop();
			}
			else
				pCurGoal = g.getPImpl();
		}
        else
		{
			//cout << "BT\n";
			branch();
            if (pCurGoal == static_cast<IGoal*>(NULL))
			{
               	//std::cout << "removing first CP\n";
               	//solver().removeCP();	// remove first CP
				return false;
            }
			mValid = true;
		}
	}
}

Double LIFOSearchPath::getRank(Int treeDepth) const
{
	Double r = 0;
	UInt d;
	if (treeDepth < 0)
		d = getDepth() - 1;	// because of extra level for bb sorted output
	else
		d = static_cast<UInt>(treeDepth);
	Int i = 0;
	for (LIFOSearchPath::Fails::Iterator it = ++fails.begin();
		it != fails.end(); ++it)
		r += (1-*it) * ::pow(2.0,static_cast<Double>(i++));
	r /= ::pow(2.0,static_cast<Double>(d))-1;
	return r;
}

// this is buggy. Among other problems, it does not agree with global stats
std::ostream& operator<<(std::ostream& os, const LIFOSearchPath& l)
{
	Double d = l.getDepth()-1;
	Double r = l.getRank();
	os << left << setw (20) << "depth" << ":" << std::setw (10) << std::right
		  						<< d << std::endl
	   << left << setw (20) << "left branches" << ":"
	   							<< std::setw (10) << std::right
		  						<< d-l.getRightDepth() << std::endl
	   << left << setw (20) << "rank" << ":" << std::setw (10) << std::right
		  						<< r << std::endl;
	return os;
}

// ********** SinglePathGoalSched *************

UInt SinglePathGoalSched::insertCP()
{
	++path.depth;
	path.fails.pushFront(0);
	solver().insertCP();
	signalCPCreate();
//	std::cout << "created CP : now at " << solver().stateID() << std::endl;
	if (discard(false))
		return branchCP();

	return 0;
}

// the stats signaling is buggy, it reports incorrect max depth
UInt SinglePathGoalSched::branchCP()
{
	solver().restoreCP();		// these remove first cp when leaving function
	solver().removeCP();        // at the next 2 returns
	signalCPRestore(); //new
	signalCPRemove();  //new
//	std::cout << "restored and removed CP " << solver().stateID() << std::endl;

	if (path.fails.empty())	// needed when there are no choice points
		return 1;

	while (*path.fails.begin() == 1)
	{
		path.fails.popFront();
		--path.rightDepth;
		--path.depth;
		if (path.fails.empty())
			return 1;
	}

	//signalCPRemove();  // these are here since the first cp is not signaled
	//signalCPRestore(); // (if we are here it's because we're not on the first cp)

//	std::cout << "now at CP " << solver().stateID() << std::endl;

	++(*path.fails.begin());
	++path.rightDepth;
	if (discard(false))
		branch();
	return 1;
}

//Void resetSearch();

// ********* MonoSearchPath **********

std::ostream& operator<<(std::ostream& os, const casper::MonoSearchPath& s)
{
	os << "depth: " << s.depth << ", rightDepth: " << s.rightDepth
	   << ", eval: " << s.eval << ", fails: " << s.fails << ", *curPosIt: ";
	if (s.curPosIt == s.fails.end())
		os << "(end)";
	else
		os << *s.curPosIt;
	return os;
}

void MonoSearchPath::insertCP()
{
	if (fails.empty())
		curPosIt = fails.pushFront(0);
	else
	if (++Fails::Iterator(curPosIt) == fails.end())
		curPosIt = fails.insertAfter(curPosIt,0);
	else
	{
		rightDepth += *curPosIt;
		++curPosIt;
	}
	++depth;
}

// \pre current position is deepest level
MonoSearchPath* MonoSearchPath::getPSibling()
{
	assert(curPosIt != fails.end() and
		   ++Fails::Iterator(curPosIt) == fails.end());
	MonoSearchPath* ret = new (fails.heap()) MonoSearchPath(*this);
	++(*ret->curPosIt);
	++ret->rightDepth;
	return ret;
}

void MonoSearchPath::goToDepth(UInt d)
{
	depth = 0;
	rightDepth = 0;
	curPosIt = fails.begin();
	if (d == 0)
		return;
	depth = 1;
	while (depth < d)
	{
		assert(curPosIt != fails.end());
		++depth;
		if (*curPosIt)
			++rightDepth;
		++curPosIt;
	}
}

UInt MonoSearchPath::getLastCommCPDepth(const MonoSearchPath& s)
{
	UInt ret = 0;
	Fails::Iterator itS = s.fails.begin();
	Fails::Iterator itM = fails.begin();
	while (itS != s.fails.end() and itM != fails.end() and *itS == *itM)
	{
		++ret;
		++itS;
		++itM;
	}
	return ret;
}

#define WORKING
#ifdef WORKING

// ********* MonoSearchPathB **********

std::ostream& operator<<(std::ostream& os, const casper::MonoSearchPathB& s)
{
	os << "depth: " << s.depth << ", rightDepth: " << s.rightDepth
	   << ", eval: " << s.eval << ", fails: " << s.fails << ", *curPosIt: ";
	if (s.curPosIt == s.fails.end())
		os << "(end)";
	else
		os << *s.curPosIt;
	return os;
}

// returns if cp is really new (true) or if we are recomputing
Void MonoSearchPathB::insertCP()
{
	if (fails.empty())
		curPosIt = fails.pushFront(0);
	else
	if (++Fails::Iterator(curPosIt) == fails.end())
		curPosIt = fails.pushBack(0);
	else
	{
		rightDepth += *curPosIt;
		++curPosIt;
	}
	++depth;
}

// *********** SinglePathGoalSchedR ***************

UInt SinglePathGoalSchedR::insertCP()
{
	path.insertCP();

	if (cps.top()==0 or path.getDepth() - cps.top() >= rd)
	{
		solver().insertCP();
		signalCPCreate();	// updates stats only
		cps.push(path.getDepth());
//		std::cout << "created CP " << solver().stateID() << " @depth=" << cps.top() << std::endl;
	}
/*	else
	// adaptive recomputation
	if (not path.atLeaf() and *path.curPosIt==0)
	{
		UInt curRD;
		UInt prevCP = cps.top();
		cps.pop();
		if (cps.top()==0)
			curRD = rd;
		else
			curRD = prevCP-cps.top();
		cps.push(prevCP);
		if (path.getDepth()-prevCP >= curRD/2)
		{
			solver().insertCP();
			signalCPCreate();	// updates stats only
			cps.push(path.getDepth());
//			std::cout << "created adaptive CP " << solver().stateID() << " @depth=" << cps.top() << std::endl;
		}
	}
*/
//	std::cout << "after insert: " << path << std::endl;
//	std::cout << "returning: " << path.getFailCount() << std::endl;
	return path.getFailCount();

}


UInt SinglePathGoalSchedR::branchCP()
{
//	std::cout << "in branchCP\n";
	solver().restoreCP();
	signalCPRestore();
//	std::cout << "pCurGoal = " << pCurGoal << " : " << solver().stateID() << std::endl;

	UInt initialCPDepth = path.getDepth();
	UInt closestCPDepth = cps.top();
	UInt distance = initialCPDepth-closestCPDepth;
//	std::cout << "initDepth : " << initialCPDepth << std::endl;
//	std::cout << "closestDepth : " << closestCPDepth << std::endl;

	// go up to first left branch before closest CP
	while (distance>0 and *path.curPosIt==1)
	{
		path.fails.erase(path.curPosIt--);
		--path.depth;
		--distance;
	}

	if (path.getDepth()==0)
		goto leave;

	if (distance>0)	// found a left branch before closest CP
	{
		++*path.curPosIt;
//		std::cout << "found a left branch before closest CP\n";
	}
	else
	if (*path.curPosIt==0) // going to the right child of closest CP
	{
//		std::cout << "going to the right child of closest CP\n";
		++*path.curPosIt;
		solver().removeCP(); // Last Alternative Optimization : remove closest CP
		signalCPRemove();    // since it will never be visited again
		cps.pop();           // (it will be recreated in next level)
		goto leave;
	}
	else  // was on the rightmost branch of closest CP
	{
//		std::cout << "was on the rightmost branch of closest CP\n";
		// the CP was already removed by the above optimization
		// so we only need to proceed up until we find a CP which has us on its left branch
		do
		{
			path.fails.erase(path.curPosIt--);
			--path.depth;
		} while (path.depth>0 and *path.curPosIt==1);
		if (path.depth==0)  // removes the first choice point
		{
			solver().removeCP();
			solver().restoreCP();
		}
		else
			++*path.curPosIt;
		assert(path.getDepth()-cps.top()<=rd);
//		std::cout << "depth=" << path.depth << std::endl;
	}

	// continue to go up until closest CP
	while (path.getDepth()>cps.top())
	{
		--path.curPosIt;
		--path.depth;
	}

	leave:

//	if (path.depth>0)
//		std::cout << "after branch: " << path << std::endl;
//	std::cout << "returning: " << (path.depth==0 or path.getFailCount()) << std::endl;
	return path.depth==0 or path.getFailCount();
}
#endif

// *********** MultiplePathGoalSched ***************

void MultiplePathGoalSched::goUp(UInt count)
{
	// going up
	for (UInt i = 0; i < count; i++)
	{
		solver().restoreCP();
		solver().removeCP();
		signalCPRemove();
	}
	solver().restoreCP();
}

void MultiplePathGoalSched::switchToBestPath()
{
	assert(!openPaths.empty());
	UInt dpsCommCP = pCurPath->getLastCommCPDepth((const MonoSearchPath&)getBestPath());
	UInt numberOfRemoves = pCurPath->getDepth()-dpsCommCP-1;
	goUp(numberOfRemoves);

	// prepare for going down (recomputing)
	pCurPath = *openPaths.begin();
	UInt trgDepth = pCurPath->getDepth();
	pCurPath->goToDepth(dpsCommCP+1);
	recomputeCount = trgDepth-dpsCommCP-1;

	openPaths.erase(openPaths.begin());
}

UInt MultiplePathGoalSched::insertCP()
{
	pCurPath->insertCP();
	solver().insertCP();
	signalCPCreate();

	if (recomputeCount-- > 0)
		return pCurPath->getFailCount();

	// get node evaluation
	Double eval = evaluate();

	// set left branch eval
	pCurPath->eval = eval;

	// get right branch and set eval
	PSearchPath aux = pCurPath;
	pCurPath = pCurPath->getPSibling();
	pCurPath->eval = evaluate();

	// put right branch into open paths
	if (!pushToOpenPaths(pCurPath))
		delete pCurPath;

	// restore curpath
	pCurPath = aux;
	if (postpone())
	{
		PSearchPath p = pCurPath;
		Bool saved = pushToOpenPaths(pCurPath);
		switchToBestPath();
		if (!saved)
			delete p;
	}
	return pCurPath->getFailCount();
}

UInt MultiplePathGoalSched::branchCP()
{
	if (openPaths.empty())
	{
		goUp(pCurPath->getDepth()+fallBack);
		return 1;
	}
	PSearchPath p = pCurPath;
	switchToBestPath();
	delete p;
	signalCPRestore();
	return pCurPath->getFailCount();
}

// returns true if the path was saved, false otherwise
// (this is to free memory after the path is discarded by switchToBestPath)
bool MultiplePathGoalSched::pushToOpenPaths(PSearchPath pPath)
{
	if (openPaths.empty() or (*openPaths.begin())->eval >= pPath->eval)
		openPaths.pushFront(pPath);
	else
	{
		OpenPaths::Iterator it = openPaths.begin();
		for (EVER)
		{
			if (it == openPaths.end())
				return false;
			if ((*it)->eval >= pPath->eval)
			{
				openPaths.insert(it,pPath);
				break;
			}
			++it;
		}
	}

	if (maxPaths>=0 and openPaths.size()>maxPaths)
	{
		PSearchPath  p = *--openPaths.end();
		delete p;
		openPaths.erase(--openPaths.end());
	}
	return true;
}

void MultiplePathGoalSched::debugOpenPaths()
{
	std::cout << "open paths: \n";
	for (OpenPaths::Iterator it = openPaths.begin();
			it != openPaths.end(); ++it)
		std::cout << **it << std::endl;
	std::cout << std::endl;
}

// ************ explore goal ***********
Goal explore(IGoalSched* gs, Goal g)
{	return new (gs->solver().heap()) Explore(gs->solver(),gs,g);	}


// ******** creators for several Goal Schedulers **********

// Depth-First Search
DFSGoalSched* dfs(ICPSolver& solver)
{	return new (solver.heap()) DFSGoalSched(solver);	}

IGoalSched* greedy(ICPSolver& solver)
{	return new (solver.heap()) GreedyGoalSched(solver);	}

// --- single iteration schedulers ---

// a single iteration of Limited Discrepancy Search
IGoalSched* lds(ICPSolver& solver,UInt maxDiscr)
{	return new (solver.heap()) LDSIteration(solver,maxDiscr);	}

// a single iteration of Depth-Bounded Discrepancy Search
IGoalSched* dds(ICPSolver& solver,UInt k)
{	return new (solver.heap()) DDSIteration(solver,k);	}

IGoalSched* limitFails(UInt n, SinglePathGoalSched* s)
{	return new (s->solver().heap()) LimitFails(n,s);	}

// --- multiple iterations of above schedulers ---

// The following works but it's ugly. forSome(n,lds(n)) is preferred.
/*
bool LDSRange::execute(Goal searchTree)
{
	if (curMaxDiscr > finalMaxDiscr)
		return false;
	return DFSGoalSched::execute(
		explore(lds(solver(),curMaxDiscr),searchTree)
							or
		explore(lds(solver(),curMaxDiscr+step,finalMaxDiscr,step),searchTree)
		);
}

// multiple iterations of Limited Discrepancy Search
IGoalSched* lds(ICPSolver& solver,UInt iMaxDiscr, UInt fMaxDiscr, UInt step)
{ return new (solver.heap()) LDSRange(solver,iMaxDiscr,fMaxDiscr,step);}
*/


// --- multiple path schedulers ---

// Depth-First Search using multiple path scheduler (for debug only)
IGoalSched* dfsMP(ICPSolver& solver)
{	return new (solver.heap()) DFSMPGoalSched(solver);	}

// Limited Discrepancy Search using multiple path scheduler
IGoalSched* ldsMP(ICPSolver& solver)
{	return new (solver.heap()) LDSMPGoalSched(solver);	}

// Depth-Bounded Discrepancy Search using multiple path scheduler
//IGoalSched* mdds(ICPSolver& solver,UInt k)
//{	return new (solver.heap()) MDDS(solver,k);	}

};

