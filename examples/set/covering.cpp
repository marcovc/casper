#include <casper/set/config.h>

#include <casper/kernel.h>
#include <casper/kernel/reversible/hashmap.h>

#include <casper/set/setfd.h>
#include <casper/set.h>
#include <casper/int.h>

#include <fstream>

#include <string>
#include <list>
#include <vector>
#include <set>

#include <unistd.h>
#include <time.h>

using namespace Casper;

template <class T, class InputIterator> bool difference(SUList<T>& l, InputIterator b2, InputIterator e2)
{
	typename SUList<T>::Iterator b1 = l.begin(), e1 = l.end();
	
	while (b1 != e1)
	{
		if (b2 == e2)
			break;
		
		if (Detail::less(*b1,*b2))
		{
			b1++;
		}
		else if (!Detail::greater(*b1,*b2))
		{
			l.erase(b1);
			++b1;
			++b2;
		}		
		else
			while (b2 != e2 and Detail::greater(*b1,*b2))
				++b2;
	}
	
	return true;
}

template <class T, class InputIterator> bool my_union(SUList<T>& l, InputIterator b2, InputIterator e2)
{
	typename SUList<T>::Iterator b1 = l.begin(), e1 = l.end();
	
	for ( EVER )
	{
		if (b1 == e1)
		{
			if (b2 == e2)
				break;
			l.insert(e1,*b2++);
		}
		else if (b2 == e2)
			break;
		else if (Detail::less(*b1,*b2))
		{
			b1++;
		}		
		else if (Detail::less(*b2,*b1))
		{
			l.insert(b1,*b2++);
		}		
		else
		{
			b1++;
			b2++;
		}		
	}
	
	return true;
}

struct UpdateStructures : IFilter
{
	typedef CurSetFD<int>::Delta::Iterator DeltaIterator;
	typedef CurSetFD<int>::DeltasIterator DeltaListIterator;
	
	Var< Set<int> > cols;
	StdHashMap< int, SUList<int>* >& columnMap;
	SUList<int>& lines;
	DeltaListIterator glbDelta;
	INotifiable* f;
	
	UpdateStructures(Var< Set<int> > cols, StdHashMap< int, SUList<int>* >& columnMap, SUList<int>& lines) :
		IFilter(cols.solver()),
		cols(cols),
		columnMap(columnMap),
		lines(lines),
		glbDelta(cols.domain().glbDeltas().begin())
	{
	}
	
	bool execute()
	{
		SUList<int> remLines (cols.solver());
		
		for (glbDelta = cols.domain().glbDeltas().beginFrom(glbDelta); glbDelta != cols.domain().glbDeltas().end(); ++glbDelta)
		{
			for (DeltaIterator it = glbDelta->begin(); it != glbDelta->end(); ++it)
			{
				StdHashMap< int, SUList<int>* >::Iterator colIt = columnMap.find(*it);
				
				if (colIt != columnMap.end())
				{
					for (SUList<int>::Iterator lineIt = colIt->second->begin(); lineIt != colIt->second->end(); ++lineIt)
					{
						remLines.insert(*lineIt);
					}
					colIt->second->clear();
				}
			}
		}
		
		for (StdHashMap< int, SUList<int>* >::Iterator colIt = columnMap.begin(); colIt != columnMap.end(); ++colIt)
		{	
			if (!difference(*colIt->second,remLines.begin(),remLines.end()))
				return false;
		}
		
		if (!my_union(lines,remLines.begin(),remLines.end()))
			return false;
		
		return true;
	}
	
	Cost cost() const
	{
		return IFilter::quadraticHi;
	}
	
	bool notify()
	{
		return f->notify();
	}

	bool entailed() const
	{
		return cols.ground();
	}

	void attach(INotifiable* f)
	{
		cols.domain().attachOnGLB(this);
		this->f = f;
	}

	void detach(INotifiable* f)
	{
		cols.domain().detachOnGLB(this);
	}
};

Filter updateStructures(Var< Set<int> > cols, StdHashMap< int, SUList<int>* >& columnMap, SUList<int>& lines)
{
	return new (cols.solver().getHeap()) UpdateStructures(cols,columnMap,lines);
}

bool readInstance(string filename, uint& lines, uint& columns, std::vector< std::list<int> >& sets)
{
	ifstream instanceStream;
	
	instanceStream.open(filename.c_str());
		
	if (instanceStream.is_open())
	{
		// number of lines and number of columns
		instanceStream >> lines >> columns;
		
		// disregarding costs
		int cost;
		for (uint column = 0; column < columns; column++)
		{
			instanceStream >> cost;
		}
		
		// reading lines
		for (uint line = 0; line < lines; line++)
		{
			int numColumns;
			std::list<int> lineColumns;
			
			// number of columns that serve the line
			instanceStream >> numColumns;
			
			// columns that serve the line
			int c;
			for (int lineColumn = 0; lineColumn < numColumns; lineColumn++)
			{
				instanceStream >> c;
				
				lineColumns.push_back(c);
			}
			
			sets.push_back(lineColumns);
		}
		
		instanceStream.close();
	}
	else
	{
		cout << "Unable to open instance file \"" << filename << "\"!" << endl;
		return false;
	}
	
	return true;
}

void getColumnMap(Var< Set<int> > cols, const std::vector< std::list<int> >& sets, StdHashMap< int, SUList<int>* >& columnMap)
{
	for (CurSetFD<int>::PIterator colIt = cols.domain().beginPoss(); colIt != cols.domain().endPoss(); ++colIt)
	{
		columnMap.insert(*colIt, new (cols.solver().getHeap()) SUList<int>(cols.solver()));
	}
	
	for (uint line = 0; line < sets.size(); line++)
	{
		for (std::list<int>::const_iterator colIt = sets[line].begin(); colIt != sets[line].end(); ++colIt)
		{
			StdHashMap< int, SUList<int>* >::Iterator it = columnMap.find(*colIt);
			
			if (it != columnMap.end())
				it->second->insert(line);
		}
	}
}

int selectNextVertex(Var< Set<int> > cols, const StdHashMap< int, SUList<int>* >& columnMap, bool flag)
{
	int index = -1;
	int value = -1;
	
	for (CurSetFD<int>::PIterator possIt = cols.domain().beginPoss(); possIt != cols.domain().endPoss(); ++possIt)
	{
		StdHashMap< int, SUList<int>* >::ConstIterator colIt = columnMap.find(*possIt);
	
		if (!flag)
		{
			if (value == -1 or ((int)colIt->second->size()) < value)
			{
				index = colIt->first;
				value = colIt->second->size();
			}
		}
		else
		{
			if (value == -1 or ((int)colIt->second->size()) > value)
			{
				index = colIt->first;
				value = colIt->second->size();
			}
		}
	}
	
	return index;
}

template <class Vertex> class CoveringSetLabel;
	
template <class Vertex> Goal coveringSetLabel(Var< Set<Vertex> > s, StdHashMap< int, SUList<int>* >& columnMap, const uint& lines, SUList<int>& linesServed, bool flag)
{
	return new (s.solver().getHeap()) CoveringSetLabel<Vertex>(s, columnMap, lines, linesServed, flag);
}

template <class Vertex> struct CoveringSetLabel : IGoal
{
	Var< Set<Vertex> > s;
	StdHashMap< int, SUList<int>* >& columnMap;
	const uint& lines;
	SUList<int>& linesServed;
	bool f;

	CoveringSetLabel(Var< Set<Vertex> > s, StdHashMap< int, SUList<int>* >& columnMap, const uint& lines, SUList<int>& linesServed, bool flag) : 
		IGoal(s.solver()),
		s(s),
		columnMap(columnMap),
		lines(lines),
		linesServed(linesServed),
		f(flag)
	{
	}

	Goal execute() 
	{ 		
		if (s.ground())
		{
			return succeed();
		}		
		else if (lines == linesServed.size())
		{
			return s.domain().clearPoss();
		}
		else
		{
			Vertex elem = selectNextVertex(s, columnMap, f);

			if (f)
				return (member(elem,s) or notMember(elem,s)) and coveringSetLabel(s,columnMap,lines,linesServed,f);
			else
				return (notMember(elem,s) or member(elem,s)) and coveringSetLabel(s,columnMap,lines,linesServed,f);
		}
	}
};

void catch_alarm(int sig_num)
{
    printf("Operation timed out. Exiting...\n\n");
    exit(0);
}

namespace Casper {
uint cninserts = 0;
uint cnremoves = 0;
uint cnclearposs = 0;

uint cavgremovesize = 0;
uint cavginsertsize = 0;
uint cavgupdatesizeexceptclearposs = 0;

uint cavgposssize = 0;
uint cavgsearcheffort = 0;
};

bool covering(CPSolver&& s, const uint& lines, const uint& columns, const std::vector< std::list<int> >& sets)
{
	signal(SIGALRM, catch_alarm);
	
	alarm(300);
	
	clock_t start, finish;
	
	start = clock();
	
	Var< Set<int> > cols (s, new (s.getHeap()) CurSetFD<int>(s,range(1,columns)));
	
	for (uint i = 0; i < lines; i++)
	{
		Var< Set<int> > inter (s, new (s.getHeap()) CurSetFD<int>(s,sets[i].begin(),sets[i].end()));
		s.post(contained(inter,cols) and cardinal(inter) > 0);
	}
	
	SUList<int> linesServed (s);
	StdHashMap< int, SUList<int>* > columnMap (s.getHeap());
	getColumnMap(cols, sets, columnMap);
	s.post(updateStructures(cols,columnMap,linesServed));
	
	s.setExplorer(bbMinimize(s,cardinal(cols)));
	
	bool res = s.solve(coveringSetLabel<int>(cols,columnMap,lines,linesServed,true));
	
	if (!res)
	{
		alarm(0);
		return false;
	}

	while (res)
	{
		finish = clock();
		
		std::cout << "Cols: " << cols.domain().card() << " : " << cols.domain() << " in " << (double) (finish - start) / CLOCKS_PER_SEC << " second(s)" << std::endl;
		res = s.next();
	}
	
	finish = clock();
	
	std::cout << "Solution found in " << (double) (finish - start) / CLOCKS_PER_SEC << " second(s)" << std::endl;
	
	alarm(0);
	
	return true;
}

int main(int argc, char **argv)
{
	if (argc < 2) 
	{		
		std::cout << "Usage: covering instance_filename" << std::endl;
		exit(-1);
	}
	
	string filename = argv[1];
	
	CPSolver& s;
	
	uint lines = 0, columns = 0;
	std::vector< std::list<int> > sets;
	
	if(!readInstance(filename, lines, columns, sets))
	{
		std::cout << "Failed to read instance file \"" << filename << "\"!" << std::endl;
		s.destroy();
		exit(-2);
	}
	
	if (!covering(s,lines,columns,sets))
	{
		std::cout << "Failed to solve instance!" << std::endl;
		s.destroy();
		exit(-3);
	}
	
	s.destroy();
	
	return 0;
}
