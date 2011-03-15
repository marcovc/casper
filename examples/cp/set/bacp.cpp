/***************************************************************************
 *   Copyright (C) 2007 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <casper/set/config.h>

#include <casper/kernel.h>
#include <casper/set.h>
#include <casper/int.h>
#include <iostream>

#include <casper/set/weight.h>

int curFilter;
int sumDelta[4] = {0};
int countDelta[4] = {0};
int sumInsert[4] = {0};
int sumErase[4] = {0};
int countInsert[4] = {0};
int countErase[4] = {0};

using namespace Casper;
using namespace std;

enum courses {
	dew100,fis100,hcw310,iwg101,mat190,mat192, dew101,
	fis101, iwi131, mat191 , mat193 , fis102 , hxwxx1,
	iei134 , iei141 ,  mat194,
	dewxx0, hcw311 ,iei132 ,iei133, iei142, iei162,
	iwn170, mat195,  hxwxx2,  iei231, iei241, iei271, iei281, iwn261,
	hfw120,  iei233, iei238, iei261, iei272,  iei273, iei161,  iei232,
	iei262, iei274,  iwi365,   iwn270 , hrw130 , iei218 , iei219 ,iei248 };

template<int nCourses, int nPrereq>
struct Problem
{
	const int nPeriods;
	const int minLoadPeriod;
	const int maxLoadPeriod;
	const int minCoursesPeriod;
	const int maxCoursesPeriod;
	const int credits[nCourses];
	const int prereq[nPrereq][2];
};

Problem<46,38> p1 = { 8, 10, 24, 2, 10,
					{1,3,1,2,4,4,1,5,3,4,4,5,1,3,3,4,1,1,
					 3,3,3,3,3,3,1,4,4,3,3,3,2,4,3,3,3,3,
					 3,3,3,3,3,3,2,3,3,3},
					{ {dew101,dew100},{fis101,fis100},{fis101,mat192},
					  {mat191,mat190},{mat193,mat190},{mat193,mat192},{fis102,fis101},
					  {fis102,mat193},{iei134,iwi131},{iei141,iwi131},{mat194,mat191},
					  {mat194,mat193},{dewxx0,dew101},{hcw311,hcw310},{iei132,iei134},
					  {iei133,iei134},{iei142,iei141},{mat195,mat194},{iei231,iei134},
					  {iei241,iei142},{iei271,iei162},{iei281,mat195},{iei233,iei231},
					  {iei238,iei231},{iei261,iwn261},{iei272,iei271},{iei273,iei271},
					  {iei273,iei271},{iei161,iwn261},{iei161,iwn261},{iei232,iei273},
					  {iei232,iei273},{iei262,iwn261},{iei274,iei273},{iei274,iei273},
					  {iei219,iei232},{iei248,iei233},{iei248,iei233} } };

// Balanced Academic Curriculum
template<int nCourses, int nPrereq>
void bacp(const Problem<nCourses,nPrereq>& i)
{
	CPSolver& solver;
	std::vector<int> credits(&i.credits[0],&i.credits[nCourses]);
	VarArray<IntSet> courses(solver,i.nPeriods,range(0,nCourses-1));

	// min/max load per period
	VarArray<int> load(solver,i.nPeriods,i.minLoadPeriod,i.maxLoadPeriod);
	for (int p = 0; p < i.nPeriods; ++p)
		solver.post( sumEqual(courses[p],credits,load[p]) );

	// min/max courses per period
	VarArray<int> ncourses(solver,i.nPeriods,i.minCoursesPeriod,i.maxCoursesPeriod);
	for (int p = 0; p < i.nPeriods; ++p)
		solver.post( cardinal(courses[p])==ncourses[p] );

	// channel to period array
	VarArray<int> period(solver,nCourses,0,i.nPeriods);
	for (int p = 0; p < i.nPeriods; ++p)
		for (uint c = 0; c < nCourses; ++c)
			solver.post( member(c,courses[p]) == (period[c]==p) );

	// prereqs
	for (int r = 0; r < nPrereq; ++r)
		solver.post(period[i.prereq[r][0]] > period[i.prereq[r][1]]);

	bool ret = solver.solve() and solver.solve( label( period, selectVarFFRR(period) ));

	cout << ret << endl << courses << endl << solver.stats() << endl
		 << solver.totalTime() << endl;
}


int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " problem\n";
		return 1;
	}
	int p = atoi(argv[1]);
	switch (p)
	{
		case 1:
			bacp(p1);
			break;
/*		case 2:
			bacp<2>();
			break;
		case 3:
			bacp<3>();
			break;
*/		default:
			std::cout << "problem must be 1,2 or 3\n";
	}
	for (int f = 0; f < 4; ++f)
	{
		cout << "filter " << f << ":\n";
		cout << "avgDeltas=" << ((float)sumDelta[f])/countDelta[f] << endl;
		cout << "avgInsert=" << ((float)sumInsert[f])/countInsert[f] << endl;
		cout << "avgErase=" << ((float)sumErase[f])/countErase[f] << endl;

		cout << "totalDeltas=" << countDelta[f] << endl;
		cout << "totalInsert=" << countInsert[f] << endl;
		cout << "totalErase=" << countErase[f] << endl;
	}
}
