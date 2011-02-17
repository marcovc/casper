#include <casper/set/config.h>

#include <casper/kernel.h>
#include <casper/kernel/container/hashmap.h>

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

using namespace casper;

template <class T, class InputIterator> Bool difference(detail::RSUList<T>& l, InputIterator b2, InputIterator e2)
{
	typename detail::RSUList<T>::Iterator b1 = l.begin(), e1 = l.end();
	
	while (b1 != e1)
	{
		if (b2 == e2)
			break;
		
		if (detail::less(*b1,*b2))
		{
			b1++;
		}
		else if (!detail::greater(*b1,*b2))
		{
			l.erase(b1);
			++b1;
			++b2;
		}		
		else
			while (b2 != e2 and detail::greater(*b1,*b2))
				++b2;
	}
	
	return true;
}

template <class T, class InputIterator> Bool my_union(detail::RSUList<T>& l, InputIterator b2, InputIterator e2)
{
	typename detail::RSUList<T>::Iterator b1 = l.begin(), e1 = l.end();
	
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
		else if (detail::less(*b1,*b2))
		{
			b1++;
		}		
		else if (detail::less(*b2,*b1))
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
	typedef CurSetFD<Int>::Delta::Iterator DeltaIterator;
	typedef CurSetFD<Int>::DeltasIterator DeltaListIterator;
	
	DomVar< Set<Int> > cols;
	detail::HashMap< Int, detail::RSUList<Int>* >& columnMap;
	detail::RSUList<Int>& lines;
	DeltaListIterator glbDelta;
	INotifiable* f;
	
	UpdateStructures(DomVar< Set<Int> > cols, detail::HashMap< Int, detail::RSUList<Int>* >& columnMap, detail::RSUList<Int>& lines) :
		IFilter(cols.solver()),
		cols(cols),
		columnMap(columnMap),
		lines(lines),
		glbDelta(cols.domain().glbDeltas().begin())
	{
	}
	
	Bool execute()
	{
		detail::RSUList<Int> remLines (cols.solver());
		
		for (glbDelta = cols.domain().glbDeltas().beginFrom(glbDelta); glbDelta != cols.domain().glbDeltas().end(); ++glbDelta)
		{
			for (DeltaIterator it = glbDelta->begin(); it != glbDelta->end(); ++it)
			{
				detail::HashMap< Int, detail::RSUList<Int>* >::Iterator colIt = columnMap.find(*it);
				
				if (colIt != columnMap.end())
				{
					for (detail::RSUList<Int>::Iterator lineIt = colIt->second->begin(); lineIt != colIt->second->end(); ++lineIt)
					{
						remLines.insert(*lineIt);
					}
					colIt->second->clear();
				}
			}
		}
		
		for (detail::HashMap< Int, detail::RSUList<Int>* >::Iterator colIt = columnMap.begin(); colIt != columnMap.end(); ++colIt)
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
	
	Bool notify()
	{
		return f->notify();
	}

	Bool entailed() const
	{
		return cols.ground();
	}

	Void attach(INotifiable* f)
	{
		cols.domain().attachOnGLB(this);
		this->f = f;
	}

	Void detach(INotifiable* f)
	{
		cols.domain().detachOnGLB(this);
	}
};

Filter updateStructures(DomVar< Set<Int> > cols, detail::HashMap< Int, detail::RSUList<Int>* >& columnMap, detail::RSUList<Int>& lines)
{
	return new (cols.solver().heap()) UpdateStructures(cols,columnMap,lines);
}

Bool readInstance(string filename, UInt& lines, UInt& columns, std::vector< std::list<Int> >& sets)
{
	ifstream instanceStream;
	
	instanceStream.open(filename.c_str());
		
	if (instanceStream.is_open())
	{
		// number of lines and number of columns
		instanceStream >> lines >> columns;
		
		// disregarding costs
		Int cost;
		for (UInt column = 0; column < columns; column++)
		{
			instanceStream >> cost;
		}
		
		// reading lines
		for (UInt line = 0; line < lines; line++)
		{
			Int numColumns;
			std::list<Int> lineColumns;
			
			// number of columns that serve the line
			instanceStream >> numColumns;
			
			// columns that serve the line
			Int c;
			for (Int lineColumn = 0; lineColumn < numColumns; lineColumn++)
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

Void getColumnMap(DomVar< Set<Int> > cols, const std::vector< std::list<Int> >& sets, detail::HashMap< Int, detail::RSUList<Int>* >& columnMap)
{
	for (CurSetFD<Int>::PIterator colIt = cols.domain().beginPoss(); colIt != cols.domain().endPoss(); ++colIt)
	{
		columnMap.insert(*colIt, new (cols.solver().heap()) detail::RSUList<Int>(cols.solver()));
	}
	
	for (UInt line = 0; line < sets.size(); line++)
	{
		for (std::list<Int>::const_iterator colIt = sets[line].begin(); colIt != sets[line].end(); ++colIt)
		{
			detail::HashMap< Int, detail::RSUList<Int>* >::Iterator it = columnMap.find(*colIt);
			
			if (it != columnMap.end())
				it->second->insert(line);
		}
	}
}

Int selectNextVertex(DomVar< Set<Int> > cols, const detail::HashMap< Int, detail::RSUList<Int>* >& columnMap, Bool flag)
{
	Int index = -1;
	Int value = -1;
	
	for (CurSetFD<Int>::PIterator possIt = cols.domain().beginPoss(); possIt != cols.domain().endPoss(); ++possIt)
	{
		detail::HashMap< Int, detail::RSUList<Int>* >::ConstIterator colIt = columnMap.find(*possIt);
	
		if (!flag)
		{
			if (value == -1 or ((Int)colIt->second->size()) < value)
			{
				index = colIt->first;
				value = colIt->second->size();
			}
		}
		else
		{
			if (value == -1 or ((Int)colIt->second->size()) > value)
			{
				index = colIt->first;
				value = colIt->second->size();
			}
		}
	}
	
	return index;
}

template <class Vertex> class CoveringSetLabel;
	
template <class Vertex> Goal coveringSetLabel(DomVar< Set<Vertex> > s, detail::HashMap< Int, detail::RSUList<Int>* >& columnMap, const UInt& lines, detail::RSUList<Int>& linesServed, Bool flag)
{
	return new (s.solver().heap()) CoveringSetLabel<Vertex>(s, columnMap, lines, linesServed, flag);
}

template <class Vertex> struct CoveringSetLabel : IGoal
{
	DomVar< Set<Vertex> > s;
	detail::HashMap< Int, detail::RSUList<Int>* >& columnMap;
	const UInt& lines;
	detail::RSUList<Int>& linesServed;
	Bool f;

	CoveringSetLabel(DomVar< Set<Vertex> > s, detail::HashMap< Int, detail::RSUList<Int>* >& columnMap, const UInt& lines, detail::RSUList<Int>& linesServed, Bool flag) : 
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
			return ok();
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

namespace casper {
UInt cninserts = 0;
UInt cnremoves = 0;
UInt cnclearposs = 0;

UInt cavgremovesize = 0;
UInt cavginsertsize = 0;
UInt cavgupdatesizeexceptclearposs = 0;

UInt cavgposssize = 0;
UInt cavgsearcheffort = 0;
};

Bool covering(ICPSolver&& s, const UInt& lines, const UInt& columns, const std::vector< std::list<Int> >& sets)
{
	signal(SIGALRM, catch_alarm);
	
	alarm(300);
	
	clock_t start, finish;
	
	start = clock();
	
	DomVar< Set<Int> > cols (s, new (s.heap()) CurSetFD<Int>(s,range(1,columns)));
	
	for (UInt i = 0; i < lines; i++)
	{
		DomVar< Set<Int> > inter (s, new (s.heap()) CurSetFD<Int>(s,sets[i].begin(),sets[i].end()));
		s.post(contained(inter,cols) and cardinal(inter) > 0);
	}
	
	detail::RSUList<Int> linesServed (s);
	detail::HashMap< Int, detail::RSUList<Int>* > columnMap (s.heap());
	getColumnMap(cols, sets, columnMap);
	s.post(updateStructures(cols,columnMap,linesServed));
	
	s.setGoalSched(bbMinimize(s,cardinal(cols)));
	
	Bool res = s.solve(coveringSetLabel<Int>(cols,columnMap,lines,linesServed,true));
	
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
	
	ICPSolver& s;
	
	UInt lines = 0, columns = 0;
	std::vector< std::list<Int> > sets;
	
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
