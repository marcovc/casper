/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <casper/kernel/goal/explorer.h>
#include <casper/kernel/goal/basic.h>
#include <casper/util/util.h>
#include <casper/kernel/goal/terminal.h>

#include <casper/kernel/rel.h>

using namespace std;

ostream& operator<<(ostream& os, const Casper::ExplorerStats& s)
{
	os << left << setw (30) << "Number of choice points" << ":" <<  std::setw (10) << std::right
							  << s.getNbChoicePoints() << std::endl
	   << left << setw (30) << "Number of fails" << ":" <<  std::setw (10) << std::right
							  << s.getNbFails() << std::endl
	   << left << setw (30) << "Maximum depth" << ":" <<  std::setw (10) << std::right
							  << s.getMaxDepth() << std::endl
	   << left << setw (30) << "Number of solutions" << ":" <<  std::setw (10) << std::right
							  << s.getNbSolutions();
	return os;
}

namespace Casper {


// ******** Explorer ***********

IExplorer::IExplorer(State& state) :
    	state(state),
    	mValid(true),
    	pCurGoal(state,NULL),
    	pLeftGoal(state,NULL),
    	pRightGoal(state,NULL),
    	goals(state)
{}

IExplorer::~IExplorer()
{}

bool IExplorer::resume()
{
	assert(mValid);
	branch();
	if (pCurGoal == static_cast<IGoal*>(NULL))
		return false;
	return executeLoop();
}

bool IExplorer::executeLoop()
{
	for (EVER)
	{
		assert(pCurGoal);
		//solver().log(this,"Explorer",Util::Logger::goalSchedIterate);
		//std::cout << solver().stats().curLevel << ": mustSucceed: " << mustSucceed() << std::endl;
		Goal g = pCurGoal->execute(*this);
		if (mValid)
		{
			if (g.getPImpl() == NULL) // pCurGoal == succeed()
			{
				if (goals.empty())
				{
					if (acceptSolution())
					{
						stats.signalNewSolution();
						return true;
					}
					else
						goto branchLabel; //return false;
				}
				pCurGoal = goals.top();
				goals.pop();
				if (pCurGoal == static_cast<IGoal*>(NULL))
				{	assert(goals.empty());	return false; }
			}
			else	// pCurGoal != succeed() and pCurGoal != fail()
				pCurGoal = g.getPImpl();
		}
        else
		{
        	branchLabel:
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

double LIFOSearchPath::getRank(int treeDepth) const
{
	double r = 0;
	uint d;
	if (treeDepth < 0)
		d = getDepth() - 1;	// because of extra level for bb sorted output
	else
		d = static_cast<uint>(treeDepth);
	int i = 0;
	for (LIFOSearchPath::Fails::Iterator it = ++fails.begin();
		it != fails.end(); ++it)
		r += (1-*it) * ::pow(2.0,static_cast<double>(i++));
	r /= ::pow(2.0,static_cast<double>(d))-1;
	return r;
}

void LIFOSearchPath::getId(uint maxDepth, counter& lb, counter& ub) const
{
	assert(maxDepth>=depth);
	lb = ub = 0;
	counter depth = fails.size();
	ub = ::pow(2.0,static_cast<double>(maxDepth-depth-1));
	for (auto it = fails.begin(); it != fails.end(); ++it)
	{
		if (*it)
			lb += ::pow(2.0,static_cast<double>(maxDepth-depth));
		--depth;
	}
	ub += lb;
}

// ********** SinglePathExplorer *************

uint ISinglePathExplorer::insertCP()
{
	++path.depth;
	path.fails.pushFront(0);
	state.insertCP();
	stats.signalCPCreate();
//	std::cout << "created CP : now at " << solver().stateID() << std::endl;
	if (discard(false))
		return branchCP();

	return 0;
}

// the stats signaling is buggy, it reports incorrect max depth
uint ISinglePathExplorer::branchCP()
{
	state.restoreCP();		// these remove first cp when leaving function
	state.removeCP();        // at the next 2 returns
	stats.signalCPRestore(); //new
	stats.signalCPRemove();  //new

	//std::cout << path.fails.size() << std::endl;

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

//void resetSearch();

// ********* MonoSearchPath **********


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
	MonoSearchPath* ret = new (fails.getHeap()) MonoSearchPath(*this);
	++(*ret->curPosIt);
	++ret->rightDepth;
	return ret;
}

void MonoSearchPath::goToDepth(uint d)
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

uint MonoSearchPath::getLastCommCPDepth(const MonoSearchPath& s)
{
	uint ret = 0;
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

//#define WORKING
#ifdef WORKING

// ********* MonoSearchPathB **********

std::ostream& operator<<(std::ostream& os, const Casper::MonoSearchPathB& s)
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
void MonoSearchPathB::insertCP()
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

// *********** SinglePathExplorerR ***************

uint SinglePathExplorerR::insertCP()
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
		uint curRD;
		uint prevCP = cps.top();
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


uint SinglePathExplorerR::branchCP()
{
//	std::cout << "in branchCP\n";
	solver().restoreCP();
	signalCPRestore();
//	std::cout << "pCurGoal = " << pCurGoal << " : " << solver().stateID() << std::endl;

	uint initialCPDepth = path.getDepth();
	uint closestCPDepth = cps.top();
	uint distance = initialCPDepth-closestCPDepth;
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

// *********** MultiplePathExplorer ***************

void IMultiplePathExplorer::goUp(uint count)
{
	// going up
	for (uint i = 0; i < count; i++)
	{
		state.restoreCP();
		state.removeCP();
		stats.signalCPRemove();
	}
	state.restoreCP();
}

void IMultiplePathExplorer::switchToBestPath()
{
	assert(!openPaths.empty());
	uint dpsCommCP = pCurPath->getLastCommCPDepth((const MonoSearchPath&)getBestPath());
	uint numberOfRemoves = pCurPath->getDepth()-dpsCommCP-1;
	goUp(numberOfRemoves);

	// prepare for going down (recomputing)
	pCurPath = *openPaths.begin();
	uint trgDepth = pCurPath->getDepth();
	pCurPath->goToDepth(dpsCommCP+1);
	recomputeCount = trgDepth-dpsCommCP-1;

	openPaths.erase(openPaths.begin());
}

uint IMultiplePathExplorer::insertCP()
{
	pCurPath->insertCP();
	state.insertCP();
	stats.signalCPCreate();

	if (recomputeCount-- > 0)
		return pCurPath->getFailCount();

	// get node evaluation
	double eval = evaluate();

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
		bool saved = pushToOpenPaths(pCurPath);
		switchToBestPath();
		if (!saved)
			delete p;
	}
	return pCurPath->getFailCount();
}

uint IMultiplePathExplorer::branchCP()
{
	if (openPaths.empty())
	{
		goUp(pCurPath->getDepth());
		return 1;
	}
	PSearchPath p = pCurPath;
	switchToBestPath();
	delete p;
	stats.signalCPRestore();
	return pCurPath->getFailCount();
}

// returns true if the path was saved, false otherwise
// (this is to free memory after the path is discarded by switchToBestPath)
bool IMultiplePathExplorer::pushToOpenPaths(PSearchPath pPath)
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

//void IMultiplePathExplorer::debugOpenPaths()
//{
//	std::cout << "open paths: \n";
//	for (OpenPaths::Iterator it = openPaths.begin();
//			it != openPaths.end(); ++it)
//		std::cout << (**it) << std::endl;
//	std::cout << std::endl;
//}

// ************ explore goal ***********

// Goal for exploring a second goal using a specific explorer
struct Explore : IGoal
{
	Explore(State& state, IExplorer& gs, Goal g) :
		gs(gs),g(g) {}
	Goal execute(IExplorer& s)
	{
		if (gs.explore(g))
			return succeed();
		else
			return fail();
	}
	IExplorer&	gs;
	Goal 		g;
};

Goal explore(State& state,IExplorer& gs, Goal g)
{	return new (state) Explore(state,gs,g);	}


// ******** creators for several Goal Schedulers **********

// Depth-First Search
DFSExplorer* dfs(State& state)
{	return new (state) DFSExplorer(state);	}

GreedyExplorer* greedy(State& state)
{	return new (state) GreedyExplorer(state);	}

// --- single iteration schedulers ---

// a single iteration of Limited Discrepancy Search
LDSIteration* lds(State& state,uint maxDiscr)
{	return new (state) LDSIteration(state,maxDiscr);	}

// a single iteration of Depth-Bounded Discrepancy Search
DDSIteration* dds(State& state,uint k)
{	return new (state) DDSIteration(state,k);	}

IExplorer* limitFails(State& state,uint n, ISinglePathExplorer* s)
{	return new (state) LimitFails(state,n,s);	}

// --- multiple iterations of above schedulers ---

// The following works but it's ugly. forSome(n,lds(n)) is preferred.
/*
bool LDSRange::execute(Goal searchTree)
{
	if (curMaxDiscr > finalMaxDiscr)
		return false;
	return DFSExplorer::execute(
		explore(lds(solver(),curMaxDiscr),searchTree)
							or
		explore(lds(solver(),curMaxDiscr+step,finalMaxDiscr,step),searchTree)
		);
}

// multiple iterations of Limited Discrepancy Search
IExplorer* lds(CPSolver& solver,uint iMaxDiscr, uint fMaxDiscr, uint step)
{ return new (solver.getHeap()) LDSRange(solver,iMaxDiscr,fMaxDiscr,step);}
*/


// --- multiple path schedulers ---

// Depth-First Search using multiple path scheduler (for debug only)
IExplorer* dfsMP(State& state)
{	return new (state) DFSMPExplorer(state);	}

// Limited Discrepancy Search using multiple path scheduler
IExplorer* ldsMP(State& state)
{	return new (state) LDSMPExplorer(state);	}

// Depth-Bounded Discrepancy Search using multiple path scheduler
//IExplorer* mdds(CPSolver& solver,uint k)
//{	return new (solver.getHeap()) MDDS(solver,k);	}

};


// this is wromg. Among other problems, it does not agree with global stats
std::ostream& operator<<(std::ostream& os, const Casper::LIFOSearchPath& l)
{
	double d = l.getDepth()-1;
	double r = l.getRank();
	os << left << setw (20) << "depth" << ":" << std::setw (10) << std::right
		  						<< d << std::endl
	   << left << setw (20) << "left branches" << ":"
	   							<< std::setw (10) << std::right
		  						<< d-l.getRightDepth() << std::endl
	   << left << setw (20) << "rank" << ":" << std::setw (10) << std::right
		  						<< r << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, const Casper::MonoSearchPath& s)
{
	os << "depth: " << s.depth << ", rightDepth: " << s.rightDepth
	   << ", eval: " << s.eval << ", fails: " << s.fails << ", *curPosIt: ";
	if (s.curPosIt == s.fails.end())
		os << "(end)";
	else
		os << *s.curPosIt;
	return os;
}

