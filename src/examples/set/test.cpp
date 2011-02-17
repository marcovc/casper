#include <casper/set/config.h>

#include <casper/kernel.h>
#include <casper/graph/graphfd.h>
#include <casper/graph/graphfilter.h>
#include <casper/set/setfd.h>
#include <casper/set.h>
#include <casper/int.h>

#include <casper/kernel/timer.h>

using namespace casper;

Timer timer = Timer("My test timer",false);

Void testFindGLB(DomVar< IntSet > set, Int nElems, Int nTests)
{
	set.solver().post(cardinal(set) ==  nElems);
	set.solver().solve(setLabel(set,true));
	
	timer.resume();
	
	for (Int i = 1; i <= nTests; i++)
	{
		Int elem = (Int)((double)nElems * (rand() / (double)RAND_MAX));
		
		set.domain().findInIn(elem);
	}
	
	timer.pause();
}

Void testFindPoss(DomVar< IntSet > set, Int nElems, Int nTests)
{
	timer.resume();
	
	for (Int i = 1; i <= nTests; i++)
	{
		Int elem = (Int)((double)nElems * (rand() / (double)RAND_MAX));
		
		set.domain().findInPoss(elem);
	}
	
	timer.pause();
}

Void testFindLUB(DomVar< IntSet > set, Int nElems, Int nTests)
{
	for (Int i = 1; i <= nTests / 2; i++)
	{	
		Int elem = (Int)((double) set.domain().possSize() * (rand() / (double)RAND_MAX)); 

		set.domain().insert(elem);
	}
				
	timer.resume();
	
	for (Int i = 1; i <= nTests; i++)
	{
		Int elem = (Int)((double)nElems * (rand() / (double)RAND_MAX));
		
		if (set.domain().findInIn(elem) == set.domain().endIn())
			set.domain().findInPoss(elem);
	}
	
	timer.pause();
}

Void testErases(DomVar< IntSet > set, Int nElems, Int nTests)
{
	timer.resume();
	
	for (Int i = 1; i <= min(nTests,nElems); i++)
	{	
		Int elem = (Int)((double) nElems * (rand() / (double)RAND_MAX)); 

		set.domain().erase(elem);
	}
	
	timer.pause();
}

Void testInserts(DomVar< IntSet > set, Int nElems, Int nTests)
{
	timer.resume();
	
	for (Int i = 1; i <= min(nTests,nElems); i++)
	{	
		Int elem = (Int)((double) nElems * (rand() / (double)RAND_MAX)); 

		set.domain().insert(elem);
	}
	
	timer.pause();
}

Void testEraseRanges(DomVar< IntSet > set, Int nElems, Int nTests)
{
	for (Int i = 1; i < (Int)sqrt(nTests); i++)
	{
		Int elem = (Int)((double) nElems * (rand() / (double)RAND_MAX)); 

		set.domain().insert(elem);
	}
	
	std::set<Int> toErase;
	for (Int i = 1; i <= nElems; i++)
	{
		toErase.insert(i);
	}
	
	for (Int i = 1; i < (Int)sqrt(nTests); i++)
	{
		/*
		Int startPos = (Int)((double) set.domain().possSize() * (rand() / (double)RAND_MAX));
		Int finishPos = min((Int)((double) (set.domain().possSize() - startPos) * (rand() / (double)RAND_MAX)),10);
		
		CurSetFD<Int>::PIterator b = set.domain().beginPoss();
		
		while (startPos-- > 0)
			b++;
		
		CurSetFD<Int>::PIterator start = b;
		
		while (finishPos-- > 0)
			b++;
		
		CurSetFD<Int>::PIterator finish = b;
		*/
		
		Int start = max(1,(Int)((double) nElems * (rand() / (double)RAND_MAX)));
		Int finish = min(start + min((Int)((double) nElems * (rand() / (double)RAND_MAX)),(Int)(nElems / sqrt(nTests))),nElems);
		
		std::set<Int>::iterator b = toErase.find(start), e = toErase.find(finish);
		
		timer.resume();
		detail::setEraseRange(set.domain(), b, e);
		timer.pause();
	}
}

Void testInsertRanges(DomVar< IntSet > set, Int nElems, Int nTests)
{
	for (Int i = 1; i < (Int)sqrt(nTests); i++)
	{
		Int elem = (Int)((double) nElems * (rand() / (double)RAND_MAX)); 

		set.domain().erase(elem);
	}

	std::set<Int> toErase;
	for (Int i = 1; i <= nElems; i++)
	{
		toErase.insert(i);
	}

	for (Int i = 1; i < (Int)sqrt(nTests); i++)
	{
		/*
			Int startPos = (Int)((double) set.domain().possSize() * (rand() / (double)RAND_MAX));
			Int finishPos = min((Int)((double) (set.domain().possSize() - startPos) * (rand() / (double)RAND_MAX)),10);

			CurSetFD<Int>::PIterator b = set.domain().beginPoss();

			while (startPos-- > 0)
				b++;

			CurSetFD<Int>::PIterator start = b;

			while (finishPos-- > 0)
				b++;

			CurSetFD<Int>::PIterator finish = b;
		 */

		Int start = max(1,(Int)((double) nElems * (rand() / (double)RAND_MAX)));
		Int finish = min(start + min((Int)((double) nElems * (rand() / (double)RAND_MAX)),(Int)(nElems / sqrt(nTests))),nElems);

		std::set<Int>::iterator b = toErase.find(start), e = toErase.find(finish);

		timer.resume();
		detail::setInsertRange(set.domain(), b, e);
		timer.pause();
	}
}

Void test(Int nElems, Int testType, Int nTests)
{
	ICPSolver& s;
	
	DomVar< Set<Int> > set (s, new (s.heap()) CurSetFD<Int>(s,range(1,nElems)));
	
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
