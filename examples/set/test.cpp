#include <casper/set/config.h>

#include <casper/kernel.h>
#include <casper/graph/graphfd.h>
#include <casper/graph/graphfilter.h>
#include <casper/set/setfd.h>
#include <casper/set.h>
#include <casper/int.h>

#include <casper/kernel/timer.h>

using namespace Casper;

Timer timer = Timer("My test timer",false);

void testFindGLB(Var< IntSet > set, int nElems, int nTests)
{
	set.solver().post(cardinal(set) ==  nElems);
	set.solver().solve(setLabel(set,true));
	
	timer.resume();
	
	for (int i = 1; i <= nTests; i++)
	{
		int elem = (int)((double)nElems * (rand() / (double)RAND_MAX));
		
		set.domain().findInIn(elem);
	}
	
	timer.pause();
}

void testFindPoss(Var< IntSet > set, int nElems, int nTests)
{
	timer.resume();
	
	for (int i = 1; i <= nTests; i++)
	{
		int elem = (int)((double)nElems * (rand() / (double)RAND_MAX));
		
		set.domain().findInPoss(elem);
	}
	
	timer.pause();
}

void testFindLUB(Var< IntSet > set, int nElems, int nTests)
{
	for (int i = 1; i <= nTests / 2; i++)
	{	
		int elem = (int)((double) set.domain().possSize() * (rand() / (double)RAND_MAX)); 

		set.domain().insert(elem);
	}
				
	timer.resume();
	
	for (int i = 1; i <= nTests; i++)
	{
		int elem = (int)((double)nElems * (rand() / (double)RAND_MAX));
		
		if (set.domain().findInIn(elem) == set.domain().endIn())
			set.domain().findInPoss(elem);
	}
	
	timer.pause();
}

void testErases(Var< IntSet > set, int nElems, int nTests)
{
	timer.resume();
	
	for (int i = 1; i <= min(nTests,nElems); i++)
	{	
		int elem = (int)((double) nElems * (rand() / (double)RAND_MAX)); 

		set.domain().erase(elem);
	}
	
	timer.pause();
}

void testInserts(Var< IntSet > set, int nElems, int nTests)
{
	timer.resume();
	
	for (int i = 1; i <= min(nTests,nElems); i++)
	{	
		int elem = (int)((double) nElems * (rand() / (double)RAND_MAX)); 

		set.domain().insert(elem);
	}
	
	timer.pause();
}

void testEraseRanges(Var< IntSet > set, int nElems, int nTests)
{
	for (int i = 1; i < (int)sqrt(nTests); i++)
	{
		int elem = (int)((double) nElems * (rand() / (double)RAND_MAX)); 

		set.domain().insert(elem);
	}
	
	std::set<int> toErase;
	for (int i = 1; i <= nElems; i++)
	{
		toErase.insert(i);
	}
	
	for (int i = 1; i < (int)sqrt(nTests); i++)
	{
		/*
		int startPos = (int)((double) set.domain().possSize() * (rand() / (double)RAND_MAX));
		int finishPos = min((int)((double) (set.domain().possSize() - startPos) * (rand() / (double)RAND_MAX)),10);
		
		CurSetFD<int>::PIterator b = set.domain().beginPoss();
		
		while (startPos-- > 0)
			b++;
		
		CurSetFD<int>::PIterator start = b;
		
		while (finishPos-- > 0)
			b++;
		
		CurSetFD<int>::PIterator finish = b;
		*/
		
		int start = max(1,(int)((double) nElems * (rand() / (double)RAND_MAX)));
		int finish = min(start + min((int)((double) nElems * (rand() / (double)RAND_MAX)),(int)(nElems / sqrt(nTests))),nElems);
		
		std::set<int>::iterator b = toErase.find(start), e = toErase.find(finish);
		
		timer.resume();
		Detail::setEraseRange(set.domain(), b, e);
		timer.pause();
	}
}

void testInsertRanges(Var< IntSet > set, int nElems, int nTests)
{
	for (int i = 1; i < (int)sqrt(nTests); i++)
	{
		int elem = (int)((double) nElems * (rand() / (double)RAND_MAX)); 

		set.domain().erase(elem);
	}

	std::set<int> toErase;
	for (int i = 1; i <= nElems; i++)
	{
		toErase.insert(i);
	}

	for (int i = 1; i < (int)sqrt(nTests); i++)
	{
		/*
			int startPos = (int)((double) set.domain().possSize() * (rand() / (double)RAND_MAX));
			int finishPos = min((int)((double) (set.domain().possSize() - startPos) * (rand() / (double)RAND_MAX)),10);

			CurSetFD<int>::PIterator b = set.domain().beginPoss();

			while (startPos-- > 0)
				b++;

			CurSetFD<int>::PIterator start = b;

			while (finishPos-- > 0)
				b++;

			CurSetFD<int>::PIterator finish = b;
		 */

		int start = max(1,(int)((double) nElems * (rand() / (double)RAND_MAX)));
		int finish = min(start + min((int)((double) nElems * (rand() / (double)RAND_MAX)),(int)(nElems / sqrt(nTests))),nElems);

		std::set<int>::iterator b = toErase.find(start), e = toErase.find(finish);

		timer.resume();
		Detail::setInsertRange(set.domain(), b, e);
		timer.pause();
	}
}

void test(int nElems, int testType, int nTests)
{
	CPSolver& s;
	
	Var< Set<int> > set (s, new (s.getHeap()) CurSetFD<int>(s,range(1,nElems)));
	
	switch (testType)
	{
		case 1:
			testFindGLB(set,nElems,nTests);
			break;
		case 2:
			testFindPoss(set,nElems,nTests);
			break;
		case 3:
			testFindLUB(set,nElems,nTests);
			break;
		case 4:
			testErases(set,nElems,nTests);
			break;
		case 5:
			testInserts(set,nElems,nTests);
			break;
		case 6:
			testEraseRanges(set,nElems,nTests);
			break;
		case 7:
			testInsertRanges(set,nElems,nTests);
			break;
		default:
			exit(-2);
	}
	
	std::cout << "Operation performed in: " << timer.getSecs() << " second(s)!" << std::endl;
	
	s.destroy();
}

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cout << "Usage: test nElems testType nTests" << std::endl;
		exit(-1);
	}
	
	srandom(123456789);
	
	test(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]));
	
	return 0;
}
