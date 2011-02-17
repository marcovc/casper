/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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
	ICPSolver& s;
	IntDomVar x(s,0,1);

	IntBndExpr v(mul(1,x));
	std::cout << v.updateRange(0,0) << std::endl;
/*
	Bool r = s.solve(casper::label(x));

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
	i.addIntVar(0, list(casper::range(0,63)));
	i.addIntVar(1, list(casper::range(0,63)));
	i.addConstraint(abs(mod(i.vars()[0],8)-mod(i.vars()[1],8))==2
					);
	i.solve();*/

#if 0
	ICPSolver& solver;
	IntDomVarArray vars(solver,4,0,3);


	IntMutVar val1(solver),val2(solver),val3(solver),val4(solver);

	for (Int i = 0; i < 4 ; i++)
		for (Int j = i+1; j < 4; j++)
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

using namespace casper;

struct Task
{
	Task(ICPSolver& s,const char* id,Int begin,Int end,Int length) :
			id(id),
			start(s,begin,end-length),
			length(s,length) {}
	Task(ICPSolver& s,const char* id,Int begin,Int end) :
			id(id),
			start(s,begin,end),
			length(s,begin,end) {}

	const char* 						id;
	DomVar<Int,FD<List,Interval,Int> >	start;
	DomVar<Int,FD<List,Interval,Int> >	length;
};

const std::ostream& operator<<(std::ostream& os, const Task& task)
{
	os << task.id << " @ " << task.start << ": length=" << task.length;
	return os;
}

Filter taskEndsBefore(const Task& task, UInt time)
{	return task.start+task.length <= time;	}

Filter taskEndsBeforeTaskStarts(const Task& task1, const Task& task2)
{	return task1.start+task1.length <= task2.start;	}

Filter taskEndsBeforeTaskEnds(const Task& task1, const Task& task2)
{	return task1.start+task1.length < task2.start+task2.length;	}

Filter taskStartsBefore(const Task& task, UInt time)
{	return task.start < time;	}

Filter taskStartsBeforeTaskStarts(const Task& task1, const Task& task2)
{	return task1.start < task2.start;	}

Filter taskStartsAfter(const Task& task, UInt time)
{	return task.start >= time;	}

Filter taskStartsAfterTaskStarts(const Task& task1, const Task& task2)
{	return task1.start >= task2.start;	}

Filter taskStartsAfterTaskEnds(const Task& task1, const Task& task2)
{	return task1.start >= task2.start+task2.length;	}

Filter taskEndsWhenTaskEnds(const Task& task1, const Task& task2)
{	return task1.start+task1.length == task2.start+task2.length;	}

Filter taskSubtasks(const Task& task, const Array<Task>& subtasks)
{
	DomVarArray<Int,1,FD<List,Interval,Int> > taskStarts(task.start.solver(),
														subtasks.size());
	DomVarArray<Int,1,FD<List,Interval,Int> > taskEnds(task.start.solver(),
														subtasks.size(),
														0,200);
	BndExpr<Int> b(task.start.solver(),0);
	for (UInt i = 0; i < subtasks.size(); i++)
	{
		taskStarts[i] = subtasks[i].start;
		task.start.solver().post(taskEnds[i] == subtasks[i].start + subtasks[i].length);
		b = b + subtasks[i].length;
	}
	return  task.length >= b and
			task.start == min(taskStarts) and
			task.start + task.length == max(taskEnds);
}

Void postTasksNoOverlap(ICPSolver& solver,Array<Task>& tasks)
{
	for (UInt i = 0; i < tasks.size(); i++)
		for (UInt j = i+1; j < tasks.size(); j++)
			solver.post(taskStartsAfterTaskEnds(tasks[i],tasks[j]) or
						taskEndsBeforeTaskStarts(tasks[i],tasks[j]));

	// redundant
/*	DomVarArray<Int> rank(solver,tasks.size(),0,tasks.size()-1);
	for (UInt i = 0; i < tasks.size(); i++)
		for (UInt j = i+1; j < tasks.size(); j++)
		{
			solver.post( (rank[i]<rank[j]) <= taskEndsBeforeTaskStarts(tasks[i],tasks[j]) );
			solver.post( (rank[j]<rank[i]) <= taskEndsBeforeTaskStarts(tasks[j],tasks[i]) );

			solver.post( taskEndsBeforeTaskStarts(tasks[i],tasks[j]) <= (rank[i]<rank[j]));
			solver.post( taskEndsBeforeTaskStarts(tasks[j],tasks[i]) <= (rank[j]<rank[i]));

		}
	solver.post( distinct(rank), GAC );*/
}

int scheduler(Int schedStart, Int schedEnd)
{
	ICPSolver& solver;

	UInt nCategories = 3;
	UInt nRefactorTasks = 6;
	UInt nAddTasks = 11;
	UInt nFixTasks = 2;
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
	refactorTasks[3] = Task(solver,"MutVar strange reversible pointer",schedStart,schedEnd,2);
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

	UInt nTasks = 0;
	for (UInt i = 0; i < nRefactorTasks; i++)
		tasks[nTasks++] = refactorTasks[i];
	for (UInt i = 0; i < nAddTasks; i++)
		tasks[nTasks++] = addTasks[i];
	for (UInt i = 0; i < nFixTasks; i++)
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

	DomVarArray<Int,1,FD<List,Interval,Int> > categoryStarts(solver,nCategories);
	for (UInt i = 0; i < nCategories; i++)
		categoryStarts[i] = categories[i].start;

	DomVarArray<Int,1,FD<List,Interval,Int> > taskStarts(solver,nTasks);
	for (UInt i = 0; i < nTasks; i++)
		taskStarts[i] = tasks[i].start;

	DomVarArray<Int,1,FD<List,Interval,Int> > milestoneStarts(solver,3);
	for (UInt i = 0; i < 3; i++)
		milestoneStarts[i] = milestones[i].start;

	cout << solver.solve() << endl;

	MutVar<Int> i(solver),j(solver);
	solver.setGoalSched(bfMinimize(solver,milestoneStarts[1]));
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
	ICPSolver& solver;

	//Debug::enable(solver.getPGoalSched());


/*  regin example
	DomVarArray<Int> vars(solver,7,0,4);
	DomVarArray<Int> capacities(solver,5,0,2);
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
 /*   DomVarArray<Int> vars(solver,3,0,2);
    DomVarArray<Int> capacities(solver,3,0,2);
    solver.post( gcc(vars,capacities) );
    solver.post(capacities[0] > 1);
    solver.post(capacities[1] > 1);
 */

//   Marco example 2
/*    DomVarArray<Int> vars(solver,3,0,2);
    DomVarArray<Int> capacities(solver,3,0,2);
    solver.post( gcc(vars,capacities) );
    solver.post(capacities[0]+capacities[1]<capacities[2]);
  */

	//   Marco example 3 (alldiff)
	const UInt n = 4;
	DomVarArray<Int> vars(solver,n,1,n);
	DomVarArray<Int> capacities(solver,n,1,1);
	solver.post( gcc(vars,capacities) );
	//solver.post( distinct(vars), Dom );

	for (UInt i = 0; i < 2; i++)
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

using namespace casper;

/*
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
	Reversible<Bool>	atSolution;

	// returns 0 if search should follow left child, 1 for right child
	virtual UInt insertCP() = 0;
	// returns 0 if search should follow left child, 1 for right child
	virtual UInt branchCP() = 0;

	// called by children to signal a new CP
	void signalCPCreate()	{	if (!atSolution) solver().stats().signalCPCreate();	}
	// called by children to signal a CP restore
	void signalCPRestore()	{	if (!atSolution) solver().stats().signalCPRestore();	}
	// called by children to signal a CP removal
	void signalCPRemove()	{	if (!atSolution) solver().stats().signalCPRemove();	}

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
};*/

ICPSolver& solver;

void atExit(int)
{
	solver.totalTime().pause();
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
	std::cout << (solver.stats().countCPs / (Double)solver.totalTime().getSecs()) << " CPs/sec\n";
	std::cout << (solver.stats().countSolutions / (Double)solver.totalTime().getSecs()) << " nsols/sec\n";
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
		Bool notify()
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
		pDemon = new (solver().heap()) Demon(*this);
		f.attach(pDemon);
	}
	void detach(INotifiable* s)
	{	f.detach(pDemon); }

	IFilter& 		f;
	INotifiable*	pOwner;
	Bool			notified;
	Demon*			pDemon;
	Bool			running;
};

Filter forceFixPoint(Filter f)
{	return new (f.solver().heap()) ForceFixPoint(f);	}
#endif

struct AssignMin : IGoal
{
	AssignMin(const DomVar<Int>& var) :
		IGoal(var.solver()),var(var) {}
	Goal execute()
	{
		if (var.domain().updateMax(var.domain().min()))
			return ok();
		return fail();
	}
	DomVar<Int>	var;
};

Goal assignMin(const DomVar<Int>& var)
{	return new (var.solver().heap()) AssignMin(var);	}

struct RemoveMin : IGoal
{
	RemoveMin(const DomVar<Int>& var) :
		IGoal(var.solver()),var(var) {}
	Goal execute()
	{
		if (var.domain().updateMin(var.domain().min()+1))
			return ok();
		return fail();
	}
	DomVar<Int>	var;
};

Goal removeMin(const DomVar<Int>& var)
{	return new (var.solver().heap()) RemoveMin(var);	}

template<int I,class D>
struct MyLabel : IGoal
{
	MyLabel(DomVarArray<Int,I,D> vars) :
		IGoal(vars.solver()),
		vars(vars),idx(vars.solver(),0) {}

	Goal execute()
	{
		while (vars(idx).domain().ground() and idx < vars.size())
			++idx;
		if (idx == vars.size())
			return ok();
		return (assignMin(vars(idx)) or
				removeMin(vars(idx))) and Goal(this);
	}
	DomVarArray<Int,I,D> vars;
	Reversible<UInt>	 idx;
};

template<int I,class D>
Goal myLabel(DomVarArray<Int,I,D> vars)
{	return new (vars.solver().heap()) MyLabel<I,D>(vars);	}

template<int I,class D>
struct MyLabelFF : IGoal
{
	MyLabelFF(DomVarArray<Int,I,D> vars) :
		IGoal(vars.solver()),
		vars(vars) {}

	Goal execute()
	{
		Int idx = -1;
		Int min = limits<Int>::max();
		for (UInt i = 0; i < vars.count(); ++i)
			if (!vars(i).domain().ground() and vars(i).domain().size() < min)
			{
				min = vars(i).domain().size();
				idx = i;
			}
		if (idx<0)
			return ok();

		return (assignMin(vars(idx)) or
				removeMin(vars(idx))) and Goal(this);
		//return selectValMin(vars).select(idx) and Goal(this);
	}
	DomVarArray<Int,I,D> vars;
};

template<int I,class D>
Goal myLabelFF(DomVarArray<Int,I,D> vars)
{	return new (vars.solver().heap()) MyLabelFF<I,D>(vars);	}

int nentailed = 0;

void branching(int nvars,int nvals)
{
	DomVarArray<Int> vars(solver,nvars,0,nvals-1);
//	SinglePathGoalSchedR	gs(solver,8);
//	solver.setGoalSched(&gs);

	Bool r = solver.solve(myLabel(vars));
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
	DomVarArray<Int> vars(solver,8,0,10);

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
	Bool r = solver.solve(myLabel(vars));
	std::cout << r << std::endl;
	while (r)
	{
//		std::cout << "here : " << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

void linearLinear()
{
	DomVarArray<Int> vars(solver,8,0,10);

	detail::List<BndExpr<Int> > l;
	for (UInt i = 0; i < vars.count()/2; i++)
		l.pushBack(BndExpr<Int>(solver,vars[i]));
	for (UInt i = vars.count()/2; i < vars.count(); i++)
		l.pushBack(BndExpr<Int>(solver,-vars[i]));
	solver.post( linear(l,0) );

	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
//		std::cout << "here : " << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

void linearAuxVars()
{
	const UInt nvars = 8;
	const UInt maxVal = 10;
	DomVarArray<Int> vars(solver,nvars,0,maxVal);

	DomVarArray<Int> aux(solver,vars.count()-1,
						static_cast<Int>(-maxVal*nvars),
						static_cast<Int>(maxVal*nvars));

	solver.post(vars[0]+vars[1]==aux[0]);
	for (UInt i = 1; i < vars.count()/2-1; i++)
		solver.post(aux[i-1]+vars[i+1]==aux[i]);
	for (UInt i = vars.count()/2-1; i < vars.count()-1; i++)
		solver.post(aux[i-1]-vars[i+1]==aux[i]);
	solver.post(aux[vars.count()-2]==0);

	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
//		std::cout << "here : " << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

#if 0
void linearSystemViewsArity4(UInt nvars,UInt nvals,UInt nsums)
{
	const UInt arity = 4;	// don't change this

	DomVarArray<Int> vars(solver,nvars,0,nvals-1);

	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*arity*nvals);
		std::cout << "val: " << val << std::endl;
		solver.post( vars[perm[0]]+vars[perm[1]]+vars[perm[2]]+vars[perm[3]]==val);
	}
	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		//std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

#if 0
void linearSystemViewsArity8(UInt nvars,UInt nvals,UInt nsums)
{
	const UInt arity = 8;	// don't change this

	DomVarArray<Int> vars(solver,nvars,0,nvals-1);

	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*arity*nvals);
		std::cout << "val: " << val << std::endl;
		solver.post( vars[perm[0]]+vars[perm[1]]+vars[perm[2]]+vars[perm[3]]+
					 vars[perm[4]]+vars[perm[5]]+vars[perm[6]]+vars[perm[7]]==val);
	}
	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while(r)
	{
		std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif


#if 0
void linearSystemViewsArity20(UInt nvars,UInt nvals,UInt nsums)
{
	const UInt arity = 20;	// don't change this

	DomVarArray<Int> vars(solver,nvars,0,nvals-1);

	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*arity*nvals);
		std::cout << "val: " << val << std::endl;
		solver.post( vars[perm[0]]+vars[perm[1]]+vars[perm[2]]+vars[perm[3]]+
				     vars[perm[4]]+vars[perm[5]]+vars[perm[6]]+vars[perm[7]]+
				     vars[perm[8]]+vars[perm[9]]+vars[perm[10]]+vars[perm[11]]+
				     vars[perm[12]]+vars[perm[13]]+vars[perm[14]]+vars[perm[15]]+
				     vars[perm[16]]+vars[perm[17]]+vars[perm[18]]+vars[perm[19]]
				     ==val);
	}
	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

#if 1
void linearSystemViewsArity40(UInt nvars,UInt nvals,UInt nsums)
{
	const UInt arity = 40;	// don't change this

	DomVarArray<Int> vars(solver,nvars,0,nvals-1);

	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*arity*nvals);
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
	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

Double log2(Double x)
{	return std::log(x)/std::log(2.0);	}

Double gauss(Double mean,Double dev,Double x)
{
	return std::exp(-std::pow(x-mean,2.0)/(2.0*dev))/
			std::sqrt(dev*2*3.141592);
}

Double dev(Double mean,Double n)
{
	Double d=0;
	for (UInt i = 0; i <= n; i++)
		d += std::pow(i-mean,2.0);
	return d/(n+1);
}

// k=constrainedness
// n=nvars
// c=constraints
//

Double getVal(Double k, Double n, Double D, Double c,Double m, Double d)
{
	Double v = (c/2-rand()/(Double)RAND_MAX*c)+pow(-1.0,rand()/(Double)RAND_MAX>=0.5) * std::sqrt(((n*log2(D)*k*log(2.0))/c - log(std::sqrt(d*2*3.141592)))*2*d)+m;
	return v;
}

void linearSystemLinear(UInt nvars,UInt nvals,UInt nsums,UInt arity)
{
	DomVarArray<Int> vars(solver,nvars,0,nvals-1);
	const Double K = 0.7;
	Double sumlog = 0;
	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		DomVarArray<Int> lin(solver,arity);
		for (UInt j = 0; j < arity; j++)
			lin[j] = vars[perm[j]];
		Double mean = (nvals-1)/2.0;
	//	Int cval = getVal(K,nvars,nvals,nsums,arity*mean,arity*dev(mean,nvals-1));
		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*arity*nvals);
//		while (rand()/(Double)RAND_MAX > gauss(arity*mean,arity*dev(mean,nvals-1),val))
//			val = static_cast<Int>(rand()/(Double)RAND_MAX*arity*nvals);
		sumlog += log2(gauss(arity*mean,arity*dev(mean,nvals-1),val));
		std::cout << "val: " << val << " "
				<< gauss(arity*mean,arity*dev(mean,nvals-1),val)
				<<	std::endl;
		solver.post( sum(lin,val));
	}
	std::cout << "K = " << (-sumlog/((Double)nvars*log2(nvals)))
			<< std::endl;

	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		//std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}




DomVar<Int> getSumDec(const DomVarArray<Int>& vars )
{
	Int mm = 0;
	for (UInt n = 0; n < vars.count(); n++)
		mm += vars[n].domain().max();

	DomVarArray<Int> aux(vars.solver(),vars.count()-1,-mm,mm);
	vars.solver().post(vars[0]+vars[1]==aux[0]);
	for (UInt i = 1; i < vars.count()-1; i++)
		vars.solver().post(aux[i-1]+vars[i+1]==aux[i]);
	return aux[vars.count()-2];
}

DomVar<Int> getProdDec(const DomVarArray<Int>& vars )
{
	Int mmax = 1;
	Int mmin = 1;
	for (UInt n = 0; n < vars.count(); n++)
	{
		mmax *= vars[n].domain().max();
		mmin *= vars[n].domain().min();
	}
	DomVarArray<Int> aux(vars.solver(),vars.count()-1,mmin,mmax);
	vars.solver().post(vars[0]*vars[1]==aux[0]);
	for (UInt i = 1; i < vars.count()-1; i++)
		vars.solver().post(aux[i-1]*vars[i+1]==aux[i]);
	return aux[vars.count()-2];
}

void linearSystemAuxVars(UInt nvars,UInt nvals,UInt nsums,UInt arity)
{
	DomVarArray<Int> vars(solver,nvars,0,nvals-1);

	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		DomVarArray<Int> lin(solver,arity);
		for (UInt j = 0; j < arity; j++)
			lin[j] = vars[perm[j]];
		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*arity*nvals);
		std::cout << "val: " << val << std::endl;
		solver.post( getSumDec(lin)==val);
	}
	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

BndExpr<Int> getSumDecBndExpr(const DomVarArray<Int>& vars )
{
	BndExpr<Int> r(vars.solver(),vars[0]);
	for (UInt n = 1; n < vars.count(); n++)
		r = r+vars[n];
	return r;
}

BndExpr<Int> getProdDecBndExpr(const DomVarArray<Int>& vars )
{
	BndExpr<Int> r(vars.solver(),vars[0]);
	for (UInt n = 1; n < vars.count(); n++)
		r = r*vars[n];
	return r;
}

void linearSystemBndExpr(UInt nvars,UInt nvals,UInt nsums,UInt arity)
{
	DomVarArray<Int> vars(solver,nvars,0,nvals-1);

	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		DomVarArray<Int> lin(solver,arity);
		for (UInt j = 0; j < arity; j++)
			lin[j] = vars[perm[j]];
		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*arity*nvals);
		std::cout << "val: " << val << std::endl;
		solver.post( getSumDecBndExpr(lin)==val);
	}
	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}


void nonlinearSystemLinear(UInt nvars,UInt nvals,UInt nsums,UInt aritySum,UInt arityProd)
{
	if (aritySum * arityProd > nvars)
	{
		std::cout << "error: arity must be <= nvars\n";
		return;
	}
	DomVarArray<Int> vars(solver,nvars,1,nvals);
	//SinglePathGoalSchedR gs(solver,8);
	//solver.setGoalSched(&gs);
	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		DomVarArray<Int> lin(solver,aritySum);
		for (UInt j = 0; j < aritySum; j++)
		{
			DomVarArray<Int> term(solver,arityProd);
			for (UInt k = 0; k < arityProd; k++)
				term[k] = vars[perm[j*arityProd+k]];
			lin[j] = getProdDec(term);
		}
		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*aritySum*nvals*arityProd/*pow(nvals,(double)arityProd)*/);
		std::cout << "val: " << val << std::endl;
		std::cout << "vars["<<perm[0]<< "]*vars["<<perm[1]<< "]*vars["<<perm[2]<< "]+"
				  << "vars["<<perm[3]<< "]*vars["<<perm[4]<< "]*vars["<<perm[5]<< "]+"
				  << "vars["<<perm[6]<< "]*vars["<<perm[7]<< "]*vars["<<perm[8]<< "]+"
				  << "vars["<<perm[9]<< "]*vars["<<perm[10]<< "]*vars["<<perm[11]<< "]=="
				  << val << std::endl;

		solver.post( sum(lin)==val);
	}
	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	if (r)
	{
		std::cout << vars << std::endl;
		//r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

void nonlinearSystemBndExpr(UInt nvars,UInt nvals,UInt nsums,UInt aritySum,UInt arityProd)
{
	if (aritySum * arityProd > nvars)
	{
		std::cout << "error: arity must be <= nvars\n";
		return;
	}
	DomVarArray<Int> vars(solver,nvars,1,nvals);
	//SinglePathGoalSchedR gs(solver,8);
	//solver.setGoalSched(&gs);
	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		BndExpr<Int> lin(solver,0);
		//DomVarArray<Int> lin(solver,aritySum);
		for (UInt j = 0; j < aritySum; j++)
		{
			DomVarArray<Int> term(solver,arityProd);
			for (UInt k = 0; k < arityProd; k++)
				term[k] = vars[perm[j*arityProd+k]];
			lin = lin + getProdDecBndExpr(term);
		}
		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*aritySum*nvals*arityProd/*pow(nvals,(double)arityProd)*/);
		for (UInt j = 0; j < aritySum*arityProd; j++)
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
	Bool r = solver.solve(myLabelFF(vars));
	//Bool r = solver.solve(label(vars,selectVarLex(vars)));
	std::cout << r << std::endl;
	if (r)
	{
		std::cout << vars << std::endl;
		//r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

void nonlinearSystemAuxVars(UInt nvars,UInt nvals,UInt nsums,UInt aritySum,UInt arityProd)
{
	if (aritySum * arityProd > nvars)
	{
		std::cout << "error: arity must be <= nvars\n";
		return;
	}
	DomVarArray<Int> vars(solver,nvars,1,nvals);
	//SinglePathGoalSchedR gs(solver,8);
	//solver.setGoalSched(&gs);
	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		DomVarArray<Int> lin(solver,aritySum);
		for (UInt j = 0; j < aritySum; j++)
		{
			DomVarArray<Int> term(solver,arityProd);
			for (UInt k = 0; k < arityProd; k++)
				term[k] = vars[perm[j*arityProd+k]];
			lin[j] = getProdDec(term);
		}
		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*aritySum*nvals*arityProd/*pow(nvals,(double)arityProd)*/);
		for (UInt j = 0; j < aritySum*arityProd; j++)
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
	Bool r = solver.solve(myLabelFF(vars));
	//Bool r = solver.solve(label(vars,selectVarLex(vars)));//myLabelFF(vars));
	std::cout << r << std::endl;
	if (r)
	{
		std::cout << vars << std::endl;
		//r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

#if 0
void nonlinearSystemViewsArity4x2(UInt nvars,UInt nvals,UInt nsums)
{
	const UInt arity = 8;
	DomVarArray<Int> vars(solver,nvars,1,nvals);

	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*arity*nvals);
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
		Array<Int> mperm(solver.heap(),perm.size());
		std::copy(perm.begin(),perm.end(),mperm.begin());
		MutVar<Int> mi(solver);
		solver.post(sum(all(mi,range(0,3),true,vars[mperm[mi*2]]*vars[mperm[mi*2+1]]))==val);
#endif
	}
	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	if (r)
	{
		std::cout << vars << std::endl;
		//r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

#if 0
void nonlinearSystemViewsArity4x3(UInt nvars,UInt nvals,UInt nsums)
{
	const UInt arity = 12;
	DomVarArray<Int> vars(solver,nvars,1,nvals);

	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());
		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*arity*nvals);
		for (UInt j = 0; j < arity; j++)
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
		Array<Int> mperm(solver.heap(),perm.size());
		std::copy(perm.begin(),perm.end(),mperm.begin());
		MutVar<Int> mi(solver);
		solver.post(sum(all(mi,range(0,3),true,vars[mperm[mi*3]]*vars[mperm[mi*3+1]]*vars[mperm[mi*3+2]]))==val);
#endif

	}
	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		//std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

#if 0
void nonlinearSystemViewsArity4x4(UInt nvars,UInt nvals,UInt nsums)
{
	const UInt arity = 16;
	DomVarArray<Int> vars(solver,nvars,1,nvals);

	for (UInt i = 0; i < nsums; i++)
	{
		vector<int> perm(nvars);
		for (UInt j = 0; j < nvars; j++)
			perm[j] = j;
		random_shuffle(perm.begin(),perm.end());

		Int val = static_cast<Int>(rand()/(Double)RAND_MAX*arity*nvals);
		for (UInt j = 0; j < arity; j++)
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
		Array<Int> mperm(solver.heap(),perm.size());
		std::copy(perm.begin(),perm.end(),mperm.begin());
		MutVar<Int> mi(solver);
		solver.post(sum(all(mi,range(0,3),true,vars[mperm[mi*4]]*vars[mperm[mi*4+1]]*vars[mperm[mi*4+2]]*vars[mperm[mi*4+3]]))==val);
#endif

	}
	Bool r = solver.solve(myLabelFF(vars));
	std::cout << r << std::endl;
	while (r)
	{
		//std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}
#endif

#if 0
void inlinedNonLinear()
{
	DomVarArray<Int> vars(solver,7,-5,5);

	solver.post( (vars[0]*vars[1]*vars[2]*vars[3])-
				 (vars[4]*vars[5]*vars[6]) == 0 );

	Bool r = solver.solve(myLabel(vars));
	std::cout << r << std::endl;
	while (r)
	{
		//std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;
}

void virtualcallVersion()
{
	DomVarArray<Int> vars(solver,8,0,10);
	BndExpr<Int> e1(solver,vars[0]+vars[1]);
	BndExpr<Int> e2(solver,e1+vars[2]);
	BndExpr<Int> e3(solver,e2+vars[3]);
	BndExpr<Int> e4(solver,e3-vars[4]);
	BndExpr<Int> e5(solver,e4-vars[5]);
	BndExpr<Int> e6(solver,e5-vars[6]);
	BndExpr<Int> e7(solver,e6-vars[7]);
	solver.post( e7 == 0 );
	Bool r = solver.solve(label(vars));
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

	DomVarArray<Int> vars(solver,7,0,10);
	for (UInt i = 0; i < 20; i++)
		solver.post( coefs[i][1]*vars[0]+coefs[i][2]*vars[1]+
					 coefs[i][3]*vars[2]+coefs[i][4]*vars[3]+
					 coefs[i][5]*vars[4]+coefs[i][6]*vars[5]+
					 coefs[i][7]*vars[6] == coefs[i][0]);
	Bool r = solver.solve(myLabel(vars));
	std::cout << r << std::endl;
	while (r)
	{
		std::cout << vars << std::endl;
		r = solver.next();
	}
	std::cout << nentailed << std::endl;
	std::cout << solver.stats() << std::endl;
	std::cout << solver.totalTime() << std::endl;
	std::cout << (solver.heap().maxAllocated()/1024) << " KiB " <<  std::endl;

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

using namespace casper;
using namespace std;

casper::Counter saves = 0;
casper::Counter doubleSaves = 0;
casper::Counter avgDistance = 0;

int main()
{
	ICPSolver& solver;
	UInt nvars = 4;
	DomVarArray<Int> vars(solver,nvars,0,1);
	SinglePathGoalSchedR	gs(solver,4);
	solver.setGoalSched(&gs);
//	Goal searchTree = ok();
//	for (UInt i = 0; i < nvars; ++i)
//		searchTree = searchTree and (vars[i]==0 or vars[i]!=0);
//	solver.post(vars[0]+vars[1] >= vars[2]+vars[3]);
	Goal searchTree =
			(vars[0]==0 or vars[0]==1) and
			(vars[1]==0 or vars[1]==1) and
			(vars[2]==0 or vars[2]==1) and
			(vars[3]==0 or vars[3]==1);
	Bool found = solver.solve(searchTree);
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

using namespace casper;

void testGBC()
{
	ICPSolver& solver;
	DomVar<Int> a(solver,-1,1);
	DomVar<Int> b(solver,-1,1);
	DomVar<Int> c(solver,0,2);
	SharedDomVar<Int> sb(b);
	solver.post( sb * sb == a);
	//solver.post( a == 2);
	cout << "SAT : " << solver.solve() << endl;
	cout << a << " " << b << " " << c << endl;
}

void testMDD()
{
	detail::Trie<Int> t(3);
	t.insert(list(1,3,4));
	t.insert(list(2,3,4));
	t.insert(list(1,3,5));
	t.insert(list(5,3,5));
	t.insert(list(1,4,4));
	t.insert(list(5,3,4));
	std::cout << t << std::endl;

	detail::MDD<Int> mdd(3);
	mdd.insert(t);
	std::cout << mdd << std::endl;
}

void testGACMDD()
{
	const UInt ntuples = 6;
	const UInt nvars = 3;

	int tuples[ntuples][nvars] =
		{ {1,3,4},
		  {2,3,4},
		  {1,3,5},
		  {5,3,5},
		  {1,4,4},
		  {5,3,4} };

	Array<Int,2> table(ntuples,nvars);
	for (UInt i = 0; i < ntuples; ++i)
		for (UInt j = 0; j < nvars; ++j)
			table[i][j] = tuples[i][j];

	ICPSolver& 			solver;
	DomVarArray<Int> vars(solver,nvars,1,10);
	solver.post(gacMDD(vars,table));
	std::cout << solver.solve() << std::endl;
	std::cout << vars << std::endl;
	Bool r = solver.solve(label(vars));
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

using namespace casper;

int main()
{
	ICPSolver& solver;
	DomVarArray<Int> v(solver,3,1,3);

	solver.post(v[0]+v[1]==v[2]);
	solver.post(v[0]!=v[1]);

	//LAInfo<DomVarArray<Int> > laInfo(v);
    //Bool f = solver.solve(label(v,rsc(v,laInfo),selectVarLAMinDomSize(v,laInfo)));

    Bool f = solver.solve(labelWithImpacts(v));

    cout << f << endl;
	cout << v << endl;
    cout << solver.stats() << endl;
}

#endif

