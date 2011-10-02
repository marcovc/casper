/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

//#define DEBUG_GCC
//#define DEBUG_SCHED
//#define DEBUG_MINICASPER
//#define DEBUG_VIRTUALCALL
//#define DEBUG_RECOMP
//#define DEBUG_MDD
#define DEBUG_OTHER

#if defined(DEBUG_MINICASPER)

#include <minicasper/kernel.h>
#include <minicasper/int.h>

#include <minicasper/instance.h>

#include <casper/kernel.h>
#include <casper/int.h>

using namespace minicasper;

int main()
{
	CPSolver& s;
	IntVar x(s,0,1);

	IntBndExpr v(mul(1,x));
	std::cout << v.updateRange(0,0) << std::endl;
/*
	bool r = s.solve(Casper::label(x));

	while (r)
	{
		std::cout << x << std::endl;
		r = s.next();
	}
	std::cout << s.stats() << std::endl;
*/
	/*
	Instance i;
	i.setNbVars(2);
	i.addIntVar(0, list(Casper::range(0,63)));
	i.addIntVar(1, list(Casper::range(0,63)));
	i.addConstraint(abs(mod(i.vars()[0],8)-mod(i.vars()[1],8))==2
					);
	i.solve();*/

#if 0
	CPSolver& solver;
	IntVarArray vars(solver,4,0,3);


	IntPar val1(solver),val2(solver),val3(solver),val4(solver);

	for (int i = 0; i < 4 ; i++)
		for (int j = i+1; j < 4; j++)
		{
			solver.post(vars[i] != vars[j]);
			solver.post(vars[i] != vars[j] + i-j);
			solver.post(vars[i] != vars[j] + j-i);
		}
	Goal g(
			tryAll(val1,range(0,3), vars[0]==val1) and
			tryAll(val2,range(0,3), vars[1]==val2) /*and
			tryAll(val3,range(0,3), vars[2]==val3) and
			tryAll(val4,range(0,3), vars[3]==val4) */
		  );

	cout << solver.solve() << endl;
	cout << solver.solve(g) << endl;
	cout << vars << endl;
#endif
}

#elif defined(DEBUG_SCHED)

#include <casper/kernel.h>
#include <casper/int.h>

#include <casper/int/gcc/gac.h>
#include <iostream>
#include <string>

using namespace Casper;

struct Task
{
	Task(CPSolver& s,const char* id,int begin,int end,int length) :
			id(id),
			start(s,begin,end-length),
			length(s,length) {}
	Task(CPSolver& s,const char* id,int begin,int end) :
			id(id),
			start(s,begin,end),
			length(s,begin,end) {}

	const char* 						id;
	Var<int,FD<List,Interval,int> >	start;
	Var<int,FD<List,Interval,int> >	length;
};

const std::ostream& operator<<(std::ostream& os, const Task& task)
{
	os << task.id << " @ " << task.start << ": length=" << task.length;
	return os;
}

Filter taskEndsBefore(const Task& task, uint time)
{	return task.start+task.length <= time;	}

Filter taskEndsBeforeTaskStarts(const Task& task1, const Task& task2)
{	return task1.start+task1.length <= task2.start;	}

Filter taskEndsBeforeTaskEnds(const Task& task1, const Task& task2)
{	return task1.start+task1.length < task2.start+task2.length;	}

Filter taskStartsBefore(const Task& task, uint time)
{	return task.start < time;	}

Filter taskStartsBeforeTaskStarts(const Task& task1, const Task& task2)
{	return task1.start < task2.start;	}

Filter taskStartsAfter(const Task& task, uint time)
{	return task.start >= time;	}

Filter taskStartsAfterTaskStarts(const Task& task1, const Task& task2)
{	return task1.start >= task2.start;	}

Filter taskStartsAfterTaskEnds(const Task& task1, const Task& task2)
{	return task1.start >= task2.start+task2.length;	}

Filter taskEndsWhenTaskEnds(const Task& task1, const Task& task2)
{	return task1.start+task1.length == task2.start+task2.length;	}

Filter taskSubtasks(const Task& task, const Array<Task>& subtasks)
{
	VarArray<int,1,FD<List,Interval,int> > taskStarts(task.start.solver(),
														subtasks.size());
	VarArray<int,1,FD<List,Interval,int> > taskEnds(task.start.solver(),
														subtasks.size(),
														0,200);
	BndExpr<int> b(task.start.solver(),0);
	for (uint i = 0; i < subtasks.size(); i++)
	{
		taskStarts[i] = subtasks[i].start;
		task.start.solver().post(taskEnds[i] == subtasks[i].start + subtasks[i].length);
		b = b + subtasks[i].length;
	}
	return  task.length >= b and
			task.start == min(taskStarts) and
			task.start + task.length == max(taskEnds);
}

void postTasksNoOverlap(CPSolver& solver,Array<Task>& tasks)
{
	for (uint i = 0; i < tasks.size(); i++)
		for (uint j = i+1; j < tasks.size(); j++)
			solver.post(taskStartsAfterTaskEnds(tasks[i],tasks[j]) or
						taskEndsBeforeTaskStarts(tasks[i],tasks[j]));

	// redundant
/*	VarArray<int> rank(solver,tasks.size(),0,tasks.size()-1);
	for (uint i = 0; i < tasks.size(); i++)
		for (uint j = i+1; j < tasks.size(); j++)
		{
			solver.post( (rank[i]<rank[j]) <= taskEndsBeforeTaskStarts(tasks[i],tasks[j]) );
			solver.post( (rank[j]<rank[i]) <= taskEndsBeforeTaskStarts(tasks[j],tasks[i]) );

			solver.post( taskEndsBeforeTaskStarts(tasks[i],tasks[j]) <= (rank[i]<rank[j]));
			solver.post( taskEndsBeforeTaskStarts(tasks[j],tasks[i]) <= (rank[j]<rank[i]));

		}
	solver.post( distinct(rank), GAC );*/
}

int scheduler(int schedStart, int schedEnd)
{
	CPSolver& solver;

	uint nCategories = 3;
	uint nRefactorTasks = 6;
	uint nAddTasks = 11;
	uint nFixTasks = 2;
	Array<Task> tasks(nRefactorTasks+nAddTasks+nFixTasks);
	Array<Task> refactorTasks(nRefactorTasks);
	Array<Task> addTasks(nAddTasks);
	Array<Task> fixTasks(nFixTasks);

	// category tasks have variable length
	Array<Task> categories(nCategories);
	categories[0] = Task(solver,"Refactor",schedStart,schedEnd);
	categories[1] = Task(solver,"Add",schedStart,schedEnd);
	categories[2] = Task(solver,"Fix",schedStart,schedEnd);

	refactorTasks[0] = Task(solver,"FilterView",schedStart,schedEnd,7);
	refactorTasks[1] = Task(solver,"reification",schedStart,schedEnd,4);
	refactorTasks[2] = Task(solver,"Fast attach/detach",schedStart,schedEnd,10);
	refactorTasks[3] = Task(solver,"Par strange reversible pointer",schedStart,schedEnd,2);
	refactorTasks[4] = Task(solver,"BndView and interval logical operations",schedStart,schedEnd,3);
	refactorTasks[5] = Task(solver,"Container: share code",schedStart,schedEnd,4);

	solver.post(taskSubtasks(categories[0],refactorTasks));

	addTasks[0] = Task(solver,"Memory",schedStart,schedEnd,3);
	addTasks[1] = Task(solver,"Stats",schedStart,schedEnd,2);
	addTasks[2] = Task(solver,"Debug",schedStart,schedEnd,4);
	addTasks[3] = Task(solver,"Rounded math",schedStart,schedEnd,6);
	addTasks[4] = Task(solver,"ValView -> setValue",schedStart,schedEnd,2);
	addTasks[5] = Task(solver,"IterationView used globally",schedStart,schedEnd,3);
	addTasks[6] = Task(solver,"labeling heuristics and OPL compatibility",schedStart,schedEnd,3);
	addTasks[7] = Task(solver,"OPL forAll and tryAll ordering",schedStart,schedEnd,4);
	addTasks[8] = Task(solver,"More goal schedulers",schedStart,schedEnd,4);
	addTasks[9] = Task(solver,"GAC Filter",schedStart,schedEnd,8);
	addTasks[10] = Task(solver,"AC Filter",schedStart,schedEnd,8);

	solver.post(taskSubtasks(categories[1],addTasks));

	fixTasks[0] = Task(solver,"Event",schedStart,schedEnd,1);
	fixTasks[1] = Task(solver,"Timer compatibility",schedStart,schedEnd,1);
	solver.post(taskSubtasks(categories[2],fixTasks));

	uint nTasks = 0;
	for (uint i = 0; i < nRefactorTasks; i++)
		tasks[nTasks++] = refactorTasks[i];
	for (uint i = 0; i < nAddTasks; i++)
		tasks[nTasks++] = addTasks[i];
	for (uint i = 0; i < nFixTasks; i++)
		tasks[nTasks++] = fixTasks[i];

	// milestones tasks have 0 length
	Array<Task> milestones(3);
	milestones[0] = Task(solver,"1.0-alpha",schedStart,schedEnd,0);
	milestones[1] = Task(solver,"1.0-beta",schedStart,schedEnd,0);
	milestones[2] = Task(solver,"1.0-gold",schedStart,schedEnd,0);

	// order milestones
	solver.post(taskStartsAfterTaskEnds(milestones[1],milestones[0]));
	solver.post(taskStartsAfterTaskEnds(milestones[2],milestones[1]));

	// connect milestones and categories
	solver.post(taskEndsWhenTaskEnds(milestones[0],categories[0]));
	solver.post(taskEndsWhenTaskEnds(milestones[1],categories[1]));
	solver.post(taskEndsWhenTaskEnds(milestones[2],categories[2]));

	// no overlap constraint
	//solver.post(tasksNoOverlap(tasks));
	postTasksNoOverlap(solver,tasks);

	VarArray<int,1,FD<List,Interval,int> > categoryStarts(solver,nCategories);
	for (uint i = 0; i < nCategories; i++)
		categoryStarts[i] = categories[i].start;

	VarArray<int,1,FD<List,Interval,int> > taskStarts(solver,nTasks);
	for (uint i = 0; i < nTasks; i++)
		taskStarts[i] = tasks[i].start;

	VarArray<int,1,FD<List,Interval,int> > milestoneStarts(solver,3);
	for (uint i = 0; i < 3; i++)
		milestoneStarts[i] = milestones[i].start;

	cout << solver.solve() << endl;

	Par<int> i(solver),j(solver);
	solver.setExplorer(bfMinimize(solver,milestoneStarts[1]));
	bool found = solver.solve(
				label(milestoneStarts) and
				//label(categoryStarts)
				label(taskStarts)
/*			forAll(i,range(0,15),
				forAll(j,range(0,15),
					taskStarts[i] < taskStarts[j] or
					taskStarts[i] > taskStarts[j]
			))*/
	);

	while (found)
	{
		cout << tasks << endl;
		cout << categories << endl;
		cout << milestones << endl;
		found = solver.next();
	}
	cout << solver.stats() << endl;
	cout << solver.totalTime() << endl;
	return 1;
}

int main()
{
	scheduler(0,200);
}

#elif defined(DEBUG_SCHED)

int main()
{
	CPSolver& solver;

	//Debug::enable(solver.getPExplorer());


/*  regin example
	VarArray<int> vars(solver,7,0,4);
	VarArray<int> capacities(solver,5,0,2);
	solver.post(capacities[0]>0);
	solver.post(capacities[1]>0);
	solver.post(capacities[2]==1);
	for(int i=0;i<4;i++) solver.post(vars[i]<2);
	solver.post(vars[4]<3);
	solver.post(vars[5]>0);
	solver.post(vars[6]>1); solver.post(vars[6]!=3);
	//solver.post( gcc(vars,list(range(1,2),range(1,2),range(1,1),range(0,2),range(0,2))) );
    solver.post(gcc(vars,capacities));
    std::cout << "first propagation: " <<  solver.solve()
			  << std::endl <<  vars << std::endl;
 */

// Marco example 1
 /*   VarArray<int> vars(solver,3,0,2);
    VarArray<int> capacities(solver,3,0,2);
    solver.post( gcc(vars,capacities) );
    solver.post(capacities[0] > 1);
    solver.post(capacities[1] > 1);
 */

//   Marco example 2
/*    VarArray<int> vars(solver,3,0,2);
    VarArray<int> capacities(solver,3,0,2);
    solver.post( gcc(vars,capacities) );
    solver.post(capacities[0]+capacities[1]<capacities[2]);
  */

	//   Marco example 3 (alldiff)
	const uint n = 4;
	VarArray<int> vars(solver,n,1,n);
	VarArray<int> capacities(solver,n,1,1);
	solver.post( gcc(vars,capacities) );
	//solver.post( distinct(vars), Dom );

	for (uint i = 0; i < 2; i++)
		solver.post( vars[i] <= 2 );

    std::cout << "first propagation: " <<  solver.solve()
    			  << std::endl <<  vars << " with cap "<< capacities << std::endl;
    /*
	bool res=solver.solve(label(vars));

	while (res){
		//std::cout << vars << " with cap " << capacities << std::endl;
		res=solver.next();
	}*/
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
}
#elif defined(DEBUG_VIRTUALCALL)

#include <casper/kernel.h>
#include <casper/int.h>

#include <signal.h>

using namespace Casper;

/*
struct Explorer : IExplorer
{
    typedef Explorer		Self;
	typedef IGoal*					PIGoal;
    typedef Stack<PIGoal> 	Goals;

    bool						mValid;
    Reversible<PIGoal>    		pCurGoal;
    Reversible<PIGoal>    		pLeftGoal;
    Reversible<PIGoal>    		pRightGoal;
    Goals                       goals;
	bool						fallBack;

    Explorer(CPSolver& mSolver);
    ~Explorer();

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

	void setFallBack(bool value)
	{	fallBack = value;	}

	protected:
	Reversible<bool>	atSolution;

	// returns 0 if search should follow left child, 1 for right child
	virtual uint insertCP() = 0;
	// returns 0 if search should follow left child, 1 for right child
	virtual uint branchCP() = 0;

	// called by children to signal a new CP
	void signalCPCreate()	{	if (!atSolution) solver().stats().signalCPCreate();	}
	// called by children to signal a CP restore
	void signalCPRestore()	{	if (!atSolution) solver().stats().signalCPRestore();	}
	// called by children to signal a CP removal
	void signalCPRemove()	{	if (!atSolution) solver().stats().signalCPRemove();	}

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
};*/

CPSolver& solver;

void atExit(int)
{
	solver.totalTime().pause();
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
	std::cout << (solver.stats().countCPs / (double)solver.totalTime().getSecs()) << " CPs/sec\n";
	std::cout << (solver.stats().countSolutions / (double)solver.totalTime().getSecs()) << " nsols/sec\n";
	std::cout << "exiting..." << std::endl;
	std::cout.flush();
	exit(0);
}

#if 0
struct ForceFixPoint : IFilter
{
	ForceFixPoint(Filter f) :
		IFilter(f.solver()), f(f.getImpl()), notified(false) {}

	struct Demon : INotifiable
	{
		Demon(ForceFixPoint& rOwner) : rOwner(rOwner) {}
		bool notify()
		{
			rOwner.notified = true;
			if (!rOwner.running)
				return rOwner.pOwner->notify();
			return true;
		}
		ForceFixPoint& rOwner;
	};

	bool execute()
	{
		running = true;
	/*	if (entailed())
		{
			detach(pOwner);
			return true;
		}
	*/	do
		{
			notified = false;
			if (!f.execute())
			{
				running = false;
				return false;
			}
		} while (notified);
		running = false;
		setInQueue(IFilter::noQueue);
		return true;
	}
	bool entailed() const
	{	return f.entailed(); }
	Cost cost() const		// FIXME: how to determine ?
	{	return f.cost();	}
	Filter	operator!()
	{	return !f;	}
	void attach(INotifiable* s)
	{
		pOwner = s;
		pDemon = new (solver().getHeap()) Demon(*this);
		f.attach(pDemon);
	}
	void detach(INotifiable* s)
	{	f.detach(pDemon); }

	IFilter& 		f;
	INotifiable*	pOwner;
	bool			notified;
	Demon*			pDemon;
	bool			running;
};

Filter forceFixPoint(Filter f)
{	return new (f.solver().getHeap()) ForceFixPoint(f);	}
#endif

struct AssignMin : IGoal
{
	AssignMin(const Var<int>& var) :
		IGoal(var.solver()),var(var) {}
	Goal execute()
	{
		if (var.domain().updateMax(var.domain().min()))
			return succeed();
		return fail();
	}
	Var<int>	var;
};

Goal assignMin(const Var<int>& var)
{	return new (var.solver().getHeap()) AssignMin(var);	}

struct RemoveMin : IGoal
{
	RemoveMin(const Var<int>& var) :
		IGoal(var.solver()),var(var) {}
	Goal execute()
	{
		if (var.domain().updateMin(var.domain().min()+1))
			return succeed();
		return fail();
	}
	Var<int>	var;
};

Goal removeMin(const Var<int>& var)
{	return new (var.solver().getHeap()) RemoveMin(var);	}

template<int I,class D>
struct MyLabel : IGoal
{
	MyLabel(VarArray<int,I,D> vars) :
		IGoal(vars.solver()),
		vars(vars),idx(vars.solver(),0) {}

	Goal execute()
	{
		while (vars(idx).domain().ground() and idx < vars.size())
			++idx;
		if (idx == vars.size())
			return succeed();
		return (assignMin(vars(idx)) or
				removeMin(vars(idx))) and Goal(this);
	}
	VarArray<int,I,D> vars;
	Reversible<uint>	 idx;
};

template<int I,class D>
Goal myLabel(VarArray<int,I,D> vars)
{	return new (vars.solver().getHeap()) MyLabel<I,D>(vars);	}

template<int I,class D>
struct MyLabelFF : IGoal
{
	MyLabelFF(VarArray<int,I,D> vars) :
		IGoal(vars.solver()),
		vars(vars) {}

	Goal execute()
	{
		int idx = -1;
		int min = limits<int>::max();
		for (uint i = 0; i < vars.count(); ++i)
			if (!vars(i).domain().ground() and vars(i).domain().size() < min)
			{
				min = vars(i).domain().size();
				idx = i;
			}
		if (idx<0)
			return succeed();

		return (assignMin(vars(idx)) or
				removeMin(vars(idx))) and Goal(this);
		//return selectValMin(vars).select(idx) and Goal(this);
	}
	VarArray<int,I,D> vars;
};

template<int I,class D>
Goal myLabelFF(VarArray<int,I,D> vars)
{	return new (vars.solver().getHeap()) MyLabelFF<I,D>(vars);	}

int nentailed = 0;

void branching(int nvars,int nvals)
{
	VarArray<int> vars(solver,nvars,0,nvals-1);
//	SinglePathExplorerR	gs(solver,8);
//	solver.setExplorer(&gs);

	bool r = solver.solve(myLabel(vars));
	std::cout << r << std::endl;
	while (r)
	{
//		std::cout << "here : " << vars << std::endl;
		r = solver.next();
	}
	atExit(0);
}

#if 0
void linearViews()
{
	VarArray<int> vars(solver,8,0,10);

	solver.post( forceFixPoint(vars[0]+vars[1]+vars[2]+vars[3]-
				 vars[4]-vars[5]-vars[6]-vars[7]==0) );
/*
	solver.post( forceFixPoint((((vars[0]+vars[1]+vars[2])+(vars[3]+
					           vars[4]+vars[5]))+((vars[6]+vars[7]+
					           vars[8])+(vars[9]+vars[10]+vars[11])))+(((-
					           vars[12]-vars[13]-vars[14])+(-vars[15]-
					           vars[16]-vars[17]))+((-vars[18]-vars[19]-
					           vars[20])+(-vars[21]-vars[22]-vars[23])))
					           ==0) );
*/
	solver.post(distinct(vars),Dom);
	bool r = solver.solve(myLabel(vars));
	std::cout << r << std::endl;
	while (r)
	{
//		std::cout << "here : " << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

void linearLinear()
{
	VarArray<int> vars(solver,8,0,10);

	List<BndExpr<int> > l;
	for (uint i = 0; i < vars.count()/2; i++)
		l.pushBack(BndExpr<int>(solver,vars[i]));
	for (uint i = vars.count()/2; i < vars.count(); i++)
		l.pushBack(BndExpr<int>(solver,-vars[i]));
	solver.post( linear(l,0) );

	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
//		std::cout << "here : " << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

void linearAuxVars()
{
	const uint nvars = 8;
	const uint maxVal = 10;
	VarArray<int> vars(solver,nvars,0,maxVal);

	VarArray<int> aux(solver,vars.count()-1,
						static_cast<int>(-maxVal*nvars),
						static_cast<int>(maxVal*nvars));

	solver.post(vars[0]+vars[1]==aux[0]);
	for (uint i = 1; i < vars.count()/2-1; i++)
		solver.post(aux[i-1]+vars[i+1]==aux[i]);
	for (uint i = vars.count()/2-1; i < vars.count()-1; i++)
		solver.post(aux[i-1]-vars[i+1]==aux[i]);
	solver.post(aux[vars.count()-2]==0);

	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
//		std::cout << "here : " << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

#if 0
void linearSystemViewsArity4(uint nvars,uint nvals,uint nsums)
{
	const uint arity = 4;	// don't change this

	VarArray<int> vars(solver,nvars,0,nvals-1);

	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		int val = static_cast<int>(rand()/(double)RAND_MAX*arity*nvals);
		std::cout << "val: " << val << std::endl;
		solver.post( vars[perm[0]]+vars[perm[1]]+vars[perm[2]]+vars[perm[3]]==val);
	}
	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		//std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

#if 0
void linearSystemViewsArity8(uint nvars,uint nvals,uint nsums)
{
	const uint arity = 8;	// don't change this

	VarArray<int> vars(solver,nvars,0,nvals-1);

	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		int val = static_cast<int>(rand()/(double)RAND_MAX*arity*nvals);
		std::cout << "val: " << val << std::endl;
		solver.post( vars[perm[0]]+vars[perm[1]]+vars[perm[2]]+vars[perm[3]]+
					 vars[perm[4]]+vars[perm[5]]+vars[perm[6]]+vars[perm[7]]==val);
	}
	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while(r)
	{
		std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif


#if 0
void linearSystemViewsArity20(uint nvars,uint nvals,uint nsums)
{
	const uint arity = 20;	// don't change this

	VarArray<int> vars(solver,nvars,0,nvals-1);

	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		int val = static_cast<int>(rand()/(double)RAND_MAX*arity*nvals);
		std::cout << "val: " << val << std::endl;
		solver.post( vars[perm[0]]+vars[perm[1]]+vars[perm[2]]+vars[perm[3]]+
				     vars[perm[4]]+vars[perm[5]]+vars[perm[6]]+vars[perm[7]]+
				     vars[perm[8]]+vars[perm[9]]+vars[perm[10]]+vars[perm[11]]+
				     vars[perm[12]]+vars[perm[13]]+vars[perm[14]]+vars[perm[15]]+
				     vars[perm[16]]+vars[perm[17]]+vars[perm[18]]+vars[perm[19]]
				     ==val);
	}
	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

#if 1
void linearSystemViewsArity40(uint nvars,uint nvals,uint nsums)
{
	const uint arity = 40;	// don't change this

	VarArray<int> vars(solver,nvars,0,nvals-1);

	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		int val = static_cast<int>(rand()/(double)RAND_MAX*arity*nvals);
		std::cout << "val: " << val << std::endl;
		solver.post(
					(vars[perm[0]]+vars[perm[1]]+vars[perm[2]]+vars[perm[3]]+
				     vars[perm[4]]+vars[perm[5]]+vars[perm[6]]+vars[perm[7]]+
				     vars[perm[8]]+vars[perm[9]]+vars[perm[10]]+vars[perm[11]]+
				     vars[perm[12]]+vars[perm[13]]+vars[perm[14]]+vars[perm[15]]+
				     vars[perm[16]]+vars[perm[17]]+vars[perm[18]]+vars[perm[19]])+
				     (vars[perm[20]]+vars[perm[21]]+vars[perm[22]]+vars[perm[23]]+
					 vars[perm[24]]+vars[perm[25]]+vars[perm[26]]+vars[perm[27]]+
					 vars[perm[28]]+vars[perm[29]]+vars[perm[30]]+vars[perm[31]]+
					 vars[perm[32]]+vars[perm[33]]+vars[perm[34]]+vars[perm[35]]+
					 vars[perm[36]]+vars[perm[37]]+vars[perm[38]]+vars[perm[39]])
				     ==val);
		std::cout << "here\n";
	}
	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

double log2(double x)
{	return std::log(x)/std::log(2.0);	}

double gauss(double mean,double dev,double x)
{
	return std::exp(-std::pow(x-mean,2.0)/(2.0*dev))/
			std::sqrt(dev*2*3.141592);
}

double dev(double mean,double n)
{
	double d=0;
	for (uint i = 0; i <= n; i++)
		d += std::pow(i-mean,2.0);
	return d/(n+1);
}

// k=constrainedness
// n=nvars
// c=constraints
//

double getVal(double k, double n, double D, double c,double m, double d)
{
	double v = (c/2-rand()/(double)RAND_MAX*c)+pow(-1.0,rand()/(double)RAND_MAX>=0.5) * std::sqrt(((n*log2(D)*k*log(2.0))/c - log(std::sqrt(d*2*3.141592)))*2*d)+m;
	return v;
}

void linearSystemLinear(uint nvars,uint nvals,uint nsums,uint arity)
{
	VarArray<int> vars(solver,nvars,0,nvals-1);
	const double K = 0.7;
	double sumlog = 0;
	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		VarArray<int> lin(solver,arity);
		for (uint j = 0; j < arity; j++)
			lin[j] = vars[perm[j]];
		double mean = (nvals-1)/2.0;
	//	int cval = getVal(K,nvars,nvals,nsums,arity*mean,arity*dev(mean,nvals-1));
		int val = static_cast<int>(rand()/(double)RAND_MAX*arity*nvals);
//		while (rand()/(double)RAND_MAX > gauss(arity*mean,arity*dev(mean,nvals-1),val))
//			val = static_cast<int>(rand()/(double)RAND_MAX*arity*nvals);
		sumlog += log2(gauss(arity*mean,arity*dev(mean,nvals-1),val));
		std::cout << "val: " << val << " "
				<< gauss(arity*mean,arity*dev(mean,nvals-1),val)
				<<	std::endl;
		solver.post( sum(lin,val));
	}
	std::cout << "K = " << (-sumlog/((double)nvars*log2(nvals)))
			<< std::endl;

	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		//std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}




Var<int> getSumDec(const VarArray<int>& vars )
{
	int mm = 0;
	for (uint n = 0; n < vars.count(); n++)
		mm += vars[n].domain().max();

	VarArray<int> aux(vars.solver(),vars.count()-1,-mm,mm);
	vars.solver().post(vars[0]+vars[1]==aux[0]);
	for (uint i = 1; i < vars.count()-1; i++)
		vars.solver().post(aux[i-1]+vars[i+1]==aux[i]);
	return aux[vars.count()-2];
}

Var<int> getProdDec(const VarArray<int>& vars )
{
	int mmax = 1;
	int mmin = 1;
	for (uint n = 0; n < vars.count(); n++)
	{
		mmax *= vars[n].domain().max();
		mmin *= vars[n].domain().min();
	}
	VarArray<int> aux(vars.solver(),vars.count()-1,mmin,mmax);
	vars.solver().post(vars[0]*vars[1]==aux[0]);
	for (uint i = 1; i < vars.count()-1; i++)
		vars.solver().post(aux[i-1]*vars[i+1]==aux[i]);
	return aux[vars.count()-2];
}

void linearSystemAuxVars(uint nvars,uint nvals,uint nsums,uint arity)
{
	VarArray<int> vars(solver,nvars,0,nvals-1);

	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		VarArray<int> lin(solver,arity);
		for (uint j = 0; j < arity; j++)
			lin[j] = vars[perm[j]];
		int val = static_cast<int>(rand()/(double)RAND_MAX*arity*nvals);
		std::cout << "val: " << val << std::endl;
		solver.post( getSumDec(lin)==val);
	}
	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

BndExpr<int> getSumDecBndExpr(const VarArray<int>& vars )
{
	BndExpr<int> r(vars.solver(),vars[0]);
	for (uint n = 1; n < vars.count(); n++)
		r = r+vars[n];
	return r;
}

BndExpr<int> getProdDecBndExpr(const VarArray<int>& vars )
{
	BndExpr<int> r(vars.solver(),vars[0]);
	for (uint n = 1; n < vars.count(); n++)
		r = r*vars[n];
	return r;
}

void linearSystemBndExpr(uint nvars,uint nvals,uint nsums,uint arity)
{
	VarArray<int> vars(solver,nvars,0,nvals-1);

	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		VarArray<int> lin(solver,arity);
		for (uint j = 0; j < arity; j++)
			lin[j] = vars[perm[j]];
		int val = static_cast<int>(rand()/(double)RAND_MAX*arity*nvals);
		std::cout << "val: " << val << std::endl;
		solver.post( getSumDecBndExpr(lin)==val);
	}
	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}


void nonlinearSystemLinear(uint nvars,uint nvals,uint nsums,uint aritySum,uint arityProd)
{
	if (aritySum * arityProd > nvars)
	{
		std::cout << "error: arity must be <= nvars\n";
		return;
	}
	VarArray<int> vars(solver,nvars,1,nvals);
	//SinglePathExplorerR gs(solver,8);
	//solver.setExplorer(&gs);
	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		VarArray<int> lin(solver,aritySum);
		for (uint j = 0; j < aritySum; j++)
		{
			VarArray<int> term(solver,arityProd);
			for (uint k = 0; k < arityProd; k++)
				term[k] = vars[perm[j*arityProd+k]];
			lin[j] = getProdDec(term);
		}
		int val = static_cast<int>(rand()/(double)RAND_MAX*aritySum*nvals*arityProd/*pow(nvals,(double)arityProd)*/);
		std::cout << "val: " << val << std::endl;
		std::cout << "vars["<<perm[0]<< "]*vars["<<perm[1]<< "]*vars["<<perm[2]<< "]+"
				  << "vars["<<perm[3]<< "]*vars["<<perm[4]<< "]*vars["<<perm[5]<< "]+"
				  << "vars["<<perm[6]<< "]*vars["<<perm[7]<< "]*vars["<<perm[8]<< "]+"
				  << "vars["<<perm[9]<< "]*vars["<<perm[10]<< "]*vars["<<perm[11]<< "]=="
				  << val << std::endl;

		solver.post( sum(lin)==val);
	}
	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	if (r)
	{
		std::cout << vars << std::endl;
		//r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

void nonlinearSystemBndExpr(uint nvars,uint nvals,uint nsums,uint aritySum,uint arityProd)
{
	if (aritySum * arityProd > nvars)
	{
		std::cout << "error: arity must be <= nvars\n";
		return;
	}
	VarArray<int> vars(solver,nvars,1,nvals);
	//SinglePathExplorerR gs(solver,8);
	//solver.setExplorer(&gs);
	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		BndExpr<int> lin(solver,0);
		//VarArray<int> lin(solver,aritySum);
		for (uint j = 0; j < aritySum; j++)
		{
			VarArray<int> term(solver,arityProd);
			for (uint k = 0; k < arityProd; k++)
				term[k] = vars[perm[j*arityProd+k]];
			lin = lin + getProdDecBndExpr(term);
		}
		int val = static_cast<int>(rand()/(double)RAND_MAX*aritySum*nvals*arityProd/*pow(nvals,(double)arityProd)*/);
		for (uint j = 0; j < aritySum*arityProd; j++)
		{
			if (j % aritySum == 0 and j>0)
				std::cout << "+";
			else
			if (j>0)
				std::cout << "*";
			std::cout << "vars["<<perm[j]<< "]";
		}
		std::cout << "==" << val << std::endl;

		solver.post( lin==val);
	}
	bool r = solver.solve(myLabelFF(vars));
	//bool r = solver.solve(label(vars,selectVarLex(vars)));
	std::cout << r << std::endl;
	if (r)
	{
		std::cout << vars << std::endl;
		//r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

void nonlinearSystemAuxVars(uint nvars,uint nvals,uint nsums,uint aritySum,uint arityProd)
{
	if (aritySum * arityProd > nvars)
	{
		std::cout << "error: arity must be <= nvars\n";
		return;
	}
	VarArray<int> vars(solver,nvars,1,nvals);
	//SinglePathExplorerR gs(solver,8);
	//solver.setExplorer(&gs);
	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		VarArray<int> lin(solver,aritySum);
		for (uint j = 0; j < aritySum; j++)
		{
			VarArray<int> term(solver,arityProd);
			for (uint k = 0; k < arityProd; k++)
				term[k] = vars[perm[j*arityProd+k]];
			lin[j] = getProdDec(term);
		}
		int val = static_cast<int>(rand()/(double)RAND_MAX*aritySum*nvals*arityProd/*pow(nvals,(double)arityProd)*/);
		for (uint j = 0; j < aritySum*arityProd; j++)
		{
			if (j % aritySum == 0 and j>0)
				std::cout << "+";
			else
			if (j>0)
				std::cout << "*";
			std::cout << "vars["<<perm[j]<< "]";
		}
		std::cout << "==" << val << std::endl;

		solver.post( getSumDec(lin)==val);
	}
	bool r = solver.solve(myLabelFF(vars));
	//bool r = solver.solve(label(vars,selectVarLex(vars)));//myLabelFF(vars));
	std::cout << r << std::endl;
	if (r)
	{
		std::cout << vars << std::endl;
		//r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

#if 0
void nonlinearSystemViewsArity4x2(uint nvars,uint nvals,uint nsums)
{
	const uint arity = 8;
	VarArray<int> vars(solver,nvars,1,nvals);

	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		int val = static_cast<int>(rand()/(double)RAND_MAX*arity*nvals);
		std::cout << "vars["<<perm[0]<< "]*vars["<<perm[1]<< "]+"
				  << "vars["<<perm[2]<< "]*vars["<<perm[3]<< "]+"
				  << "vars["<<perm[4]<< "]*vars["<<perm[5]<< "]+"
				  << "vars["<<perm[6]<< "]*vars["<<perm[7]<< "]=="
				  << val << std::endl;
#if 1
		solver.post(vars[perm[0]]*vars[perm[1]]+
					vars[perm[2]]*vars[perm[3]])+
					vars[perm[4]]*vars[perm[5]])+
					vars[perm[6]]*vars[perm[7]] == val
					);
#else
		Array<int> mperm(solver.getHeap(),perm.size());
		std::copy(perm.begin(),perm.end(),mperm.begin());
		Par<int> mi(solver);
		solver.post(sum(all(mi,range(0,3),true,vars[mperm[mi*2]]*vars[mperm[mi*2+1]]))==val);
#endif
	}
	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	if (r)
	{
		std::cout << vars << std::endl;
		//r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

#if 0
void nonlinearSystemViewsArity4x3(uint nvars,uint nvals,uint nsums)
{
	const uint arity = 12;
	VarArray<int> vars(solver,nvars,1,nvals);

	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());
		int val = static_cast<int>(rand()/(double)RAND_MAX*arity*nvals);
		for (uint j = 0; j < arity; j++)
		{
			if (j % 4 == 0 and j>0)
				std::cout << "+";
			else
			if (j>0)
				std::cout << "*";
			std::cout << "vars["<<perm[j]<< "]";
		}
		std::cout << "==" << val << std::endl;

#if 1
		solver.post(vars[perm[0]]*vars[perm[1]]*vars[perm[2]]+
					vars[perm[3]]*vars[perm[4]]*vars[perm[5]]+
					vars[perm[6]]*vars[perm[7]]*vars[perm[8]]+
					vars[perm[9]]*vars[perm[10]]*vars[perm[11]] == val
					);
#else
		Array<int> mperm(solver.getHeap(),perm.size());
		std::copy(perm.begin(),perm.end(),mperm.begin());
		Par<int> mi(solver);
		solver.post(sum(all(mi,range(0,3),true,vars[mperm[mi*3]]*vars[mperm[mi*3+1]]*vars[mperm[mi*3+2]]))==val);
#endif

	}
	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		//std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

#if 0
void nonlinearSystemViewsArity4x4(uint nvars,uint nvals,uint nsums)
{
	const uint arity = 16;
	VarArray<int> vars(solver,nvars,1,nvals);

	for (uint i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (uint j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		int val = static_cast<int>(rand()/(double)RAND_MAX*arity*nvals);
		for (uint j = 0; j < arity; j++)
		{
			if (j % 4 == 0 and j>0)
				std::cout << "+";
			else
			if (j>0)
				std::cout << "*";
			std::cout << "vars["<<perm[j]<< "]";
		}
		std::cout << "==" << val << std::endl;
#if 1
		solver.post(vars[perm[0]]*vars[perm[1]]*vars[perm[2]]*vars[perm[3]]+
					vars[perm[4]]*vars[perm[5]]*vars[perm[6]]*vars[perm[7]]+
						vars[perm[8]]*vars[perm[9]]*vars[perm[10]]*vars[perm[11]]+
								vars[perm[12]]*vars[perm[13]]*vars[perm[14]]*vars[perm[15]] == val
					);
#else
		Array<int> mperm(solver.getHeap(),perm.size());
		std::copy(perm.begin(),perm.end(),mperm.begin());
		Par<int> mi(solver);
		solver.post(sum(all(mi,range(0,3),true,vars[mperm[mi*4]]*vars[mperm[mi*4+1]]*vars[mperm[mi*4+2]]*vars[mperm[mi*4+3]]))==val);
#endif

	}
	bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		//std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

#if 0
void inlinedNonLinear()
{
	VarArray<int> vars(solver,7,-5,5);

	solver.post( (vars[0]*vars[1]*vars[2]*vars[3])-
				 (vars[4]*vars[5]*vars[6]) == 0 );

	bool r = solver.solve(myLabel(vars));
	std::cout << r << std::endl;
	while (r)
	{
		//std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

void virtualcallVersion()
{
	VarArray<int> vars(solver,8,0,10);
	BndExpr<int> e1(solver,vars[0]+vars[1]);
	BndExpr<int> e2(solver,e1+vars[2]);
	BndExpr<int> e3(solver,e2+vars[3]);
	BndExpr<int> e4(solver,e3-vars[4]);
	BndExpr<int> e5(solver,e4-vars[5]);
	BndExpr<int> e6(solver,e5-vars[6]);
	BndExpr<int> e7(solver,e6-vars[7]);
	solver.post( e7 == 0 );
	bool r = solver.solve(label(vars));
	std::cout << r << std::endl;
	while (r)
	{
	//	std::cout << "here : " << vars << std::endl;
		r = solver.next();
	}
	std::cout << r << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
}

void eqs()
{
	int coefs[20][8] = {
	      {876370, -16105, 62397, -6704, 43340, 95100, -68610, 58301},
	      {533909, 51637, 67761, 95951, 3834, -96722, 59190, 15280},
	      {915683, 1671, -34121, 10763, 80609, 42532, 93520, -33488},
	      {129768, 71202, -11119, 73017, -38875, -14413, -29234, 72370},
	      {752447, 8874, -58412, 73947, 17147, 62335, 16005, 8632},
	      {90614, 85268, 54180, -18810, -48219, 6013, 78169, -79785},
	      {1198280, -45086, 51830, -4578, 96120, 21231, 97919, 65651},
	      {18465, -64919, 80460, 90840, -59624, -75542, 25145, -47935},
	      {1503588, -43277, 43525, 92298, 58630, 92590, -9372, -60227},
	      {1244857, -16835, 47385, 97715, -12640, 69028, 76212, -81102},
	      {1410723, -60301, 31227, 93951, 73889, 81526, -72702, 68026},
	      {25334, 94016, -82071, 35961, 66597, -30705, -44404, -38304},
	      {277271, -67456, 84750, -51553, 21239, 81675, -99395, -4254},
	      {249912, -85698, 29958, 57308, 48789, -78219, 4657, 34539},
	      {373854, 85176, -95332, -1268, 57898, 15883, 50547, 83287},
	      {740061, -10343, 87758, -11782, 19346, 70072, -36991, 44529},
	      {146074, 49149, 52871, -7132, 56728, -33576, -49530, -62089},
	      {251591, -60113, 29475, 34421, -76870, 62646, 29278, -15212},
	      {22167, 87059, -29101, -5513, -21219, 22128, 7276, 57308},
	      {821228, -76706, 98205, 23445, 67921, 24111, -48614, -41906}
	    };

	VarArray<int> vars(solver,7,0,10);
	for (uint i = 0; i < 20; i++)
		solver.post( coefs[i][1]*vars[0]+coefs[i][2]*vars[1]+
					 coefs[i][3]*vars[2]+coefs[i][4]*vars[3]+
					 coefs[i][5]*vars[4]+coefs[i][6]*vars[5]+
					 coefs[i][7]*vars[6] == coefs[i][0]);
	bool r = solver.solve(myLabel(vars));
	std::cout << r << std::endl;
	while (r)
	{
		std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.getHeap().maxAllocated()/1024) << " KiB " <<  std::endl;

}
#endif


int main(int argc,char** argv)
{
	signal(SIGINT, atExit);
	//branching(atoi(argv[1]),atoi(argv[2]));
	srand(atoi(argv[1]));

	//linearSystemLinear(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5])); // nvars,nvals,sums,arity
	//linearSystemAuxVars(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5])); // nvars,nvals,sums,arity
	//linearSystemBndExpr(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5])); // nvars,nvals,sums,arity
	//linearSystemViewsArity4(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
	//linearSystemViewsArity8(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
	linearSystemViewsArity20(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
	//linearSystemViewsArity40(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
	//linearAuxVars();

	//nonlinearSystemLinear(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),atoi(argv[6])); // nvar,nvals,sums,aritySum,arityProd
	//nonlinearSystemBndExpr(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),atoi(argv[6])); // nvar,nvals,sums,aritySum,arityProd
	//nonlinearSystemAuxVars(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),atoi(argv[6])); // nvar,nvals,sums,aritySum,arityProd
	//nonlinearSystemViewsArity4x2(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
	//nonlinearSystemViewsArity4x3(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
	//nonlinearSystemViewsArity4x4(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));

	//debug2();
	//virtualcallVersion();
	//eqs();
	//magicSquareViews();
	//magicSquareLinear(atoi(argv[2]));
	//magicSquareAuxVars(7);

	//inlinedNonLinear();
}

// -------------------------------
#elif defined(DEBUG_RECOMP)

#include <casper/kernel.h>
#include <casper/int.h>

#include <iostream>

using namespace Casper;
using namespace std;

Casper::Counter saves = 0;
Casper::Counter doubleSaves = 0;
Casper::Counter avgDistance = 0;

int main()
{
	CPSolver& solver;
	uint nvars = 4;
	VarArray<int> vars(solver,nvars,0,1);
	SinglePathExplorerR	gs(solver,4);
	solver.setExplorer(&gs);
//	Goal searchTree = succeed();
//	for (uint i = 0; i < nvars; ++i)
//		searchTree = searchTree and (vars[i]==0 or vars[i]!=0);
//	solver.post(vars[0]+vars[1] >= vars[2]+vars[3]);
	Goal searchTree =
			(vars[0]==0 or vars[0]==1) and
			(vars[1]==0 or vars[1]==1) and
			(vars[2]==0 or vars[2]==1) and
			(vars[3]==0 or vars[3]==1);
	bool found = solver.solve(searchTree);
	cout << found << endl;
	while (found)
	{
		cout << vars << endl;
		found = solver.next();
	}
	cout << solver.stats() << endl;
	cout << solver.totalTime() << endl;
}


#elif defined(DEBUG_MDD)

#include <casper/kernel.h>
#include <casper/int.h>

#include <casper/int/table/mdd.h>

#include <iostream>

using namespace Casper;

void testGBC()
{
	CPSolver& solver;
	Var<int> a(solver,-1,1);
	Var<int> b(solver,-1,1);
	Var<int> c(solver,0,2);
	SharedVar<int> sb(b);
	solver.post( sb * sb == a);
	//solver.post( a == 2);
	cout << "SAT : " << solver.solve() << endl;
	cout << a << " " << b << " " << c << endl;
}

void testMDD()
{
	StdTrie<int> t(3);
	t.insert(list(1,3,4));
	t.insert(list(2,3,4));
	t.insert(list(1,3,5));
	t.insert(list(5,3,5));
	t.insert(list(1,4,4));
	t.insert(list(5,3,4));
	std::cout << t << std::endl;

	Detail::MDD<int> mdd(3);
	mdd.insert(t);
	std::cout << mdd << std::endl;
}

void testGACMDD()
{
	const uint ntuples = 6;
	const uint nvars = 3;

	int tuples[ntuples][nvars] =
		{ {1,3,4},
		  {2,3,4},
		  {1,3,5},
		  {5,3,5},
		  {1,4,4},
		  {5,3,4} };

	Array<int,2> table(ntuples,nvars);
	for (uint i = 0; i < ntuples; ++i)
		for (uint j = 0; j < nvars; ++j)
			table[i][j] = tuples[i][j];

	CPSolver& 			solver;
	VarArray<int> vars(solver,nvars,1,10);
	solver.post(gacMDD(vars,table));
	std::cout << solver.solve() << std::endl;
	std::cout << vars << std::endl;
	bool r = solver.solve(label(vars));
	while (r)
	{
		std::cout << vars << std::endl;
		r = solver.next();
	}
}

int main()
{
	//testMDD();
	testGACMDD();
}
#elif defined(DEBUG_OTHER)

#include <casper/kernel.h>
#include <casper/int.h>

#include <casper/int/sc.h>
#include <casper/int/la.h>
#include <casper/int/impacts.h>

#include <iostream>

using namespace Casper;

int main()
{
	CPSolver& solver;
	VarArray<int> v(solver,3,1,3);

	solver.post(v[0]+v[1]==v[2]);
	solver.post(v[0]!=v[1]);

	//LAInfo<VarArray<int> > laInfo(v);
    //bool f = solver.solve(label(v,rsc(v,laInfo),selectVarLAMinDomSize(v,laInfo)));

    bool f = solver.solve(labelWithImpacts(v));

    cout << f << endl;
	cout << v << endl;
    cout << solver.stats() << endl;
}

#endif

