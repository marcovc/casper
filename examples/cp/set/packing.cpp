/***************************************************************************
 *   Copyright (C) 2008 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include <casper/set/config.h>

#include <casper/kernel.h>
#include <casper/int.h>
#include <casper/set.h>

#include <fstream>
#include <sstream>

using namespace std;
using namespace Casper;

/**
 *  set packing (optimization) : find the largest number of pairwise
 *  disjoint subsets.
 *
 * instance file format:
 * http://www.nlsde.buaa.edu.cn/~kexu/benchmarks/set-benchmarks.htm
 */

struct LessSecond
{
	bool operator()(const std::pair<int,int>& s1,const std::pair<int,int>& s2) const
	{	return s1.second < s2.second;	}
};

void packing(const char* instance)
{
	typedef List<int> 	Set;
	typedef List<Set>	Sets;
	uint nsets = 0;
	Sets sets;

	// read instance
	ifstream f(instance);
	while (!f.eof())
	{
		int a;
		Set s;
		string line;
		getline(f,line);
		f >> ws;
		stringstream os;
		os << line;
		char ch;
		// read line type
		os >> ch >> ws;
		if (ch=='c' or ch=='p')
			continue;
		while (!os.eof())
		{
			os >> a >> ws;
			s.pushBack(a);
		}
		sets.pushBack(s);
		++nsets;
	}
	f.close();

	// create conflict sets
	typedef std::pair<int,int> P;
	std::vector<P> cards(nsets);
	for (uint i = 0; i < nsets; i++)
	{
		cards[i].first = i;
		cards[i].second = 0;
	}

	Sets conflictsets;
	uint nconflicts = 0;
	Sets::Iterator it1 = sets.begin();
	for (uint i1 = 0 ; i1 < nsets; ++i1,++it1)
	{
		Sets::Iterator it2 = it1;
		++it2;
		Set conflictset;
		for (uint i2 = i1+1; i2 < nsets; ++i2,++it2)
			if (Detail::intersect(it1->begin(),it1->end(),it2->begin(),it2->end()))
			{
				conflictset.pushBack(i2);
				cards[i2].second++;
			}

		if (!conflictset.empty())
		{
			conflictset.pushFront(i1);
			conflictsets.pushBack(conflictset);
			nconflicts++;
			cards[i1].second++;
		}
	}

	cout << "conflict sets : " << conflictsets << endl;

	// solve problem

	CPSolver& solver;
	Var<IntSet> packing(solver,range(0,nsets-1));
	VarArray<IntSet>	conflicts(solver,nconflicts);
	uint i = 0;
	for (Sets::Iterator it = conflictsets.begin();
			it != conflictsets.end(); ++it, ++i)
	{
		conflicts[i] = Var<IntSet>(solver,*it,list<int>());
		solver.post(cardinal(intersect(conflicts[i],packing)) <= 1);
	}
	std::cout << "nsets: " << nsets << std::endl;
	std::cout << "nconflicts: " << nconflicts << std::endl;
	std::sort(cards.begin(),cards.end(),LessSecond());
	List<int> ordering;
	for (uint j = 0; j < nsets; j++)
		ordering.pushBack(cards[j].first);

	solver.setExplorer(bbMinimize(solver,-cardinal(packing)));
	Ref<int> val(solver);
	bool found = solver.solve( forAll(val,ordering,
									member(val,packing) or
									notMember(val,packing)));

	while (found)
	{
		cout << "cost " << packing.domain().card() << " : " << packing << endl;
		found = solver.next();
	}
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		cout << "usage: " << argv[0] << " instance-file\n";
		return 1;
	}
	packing(argv[1]);
}
