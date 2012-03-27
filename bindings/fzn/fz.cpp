/*
 *  Adapted for Casper from skeleton code by Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *  	Guido Tack, 2007
 *  	Marco Correia, 2012
 */


#include <iostream>
#include <fstream>
#include <cstring>
#include "flatzinc.hh"
#include "options.h"

using namespace std;

int main(int argc, char** argv)
{
	FlatZinc::Options opt;

	if (!opt.parse(argc,argv))
		return 1;

	FlatZinc::Printer p;
	FlatZinc::FlatZincModel* fg = NULL;

	if (!strcmp(opt.filename().c_str(), "-"))
		fg = FlatZinc::parse(cin, p);
	else
		fg = FlatZinc::parse(opt.filename().c_str(), p);
  

	if (fg)
	{
		fg->createBranchers(fg->solveAnnotations(), false, std::cerr);
		fg->run(std::cout, p, opt);
	}
	else
		return 1;

	delete fg;
  
	return 0;
}
