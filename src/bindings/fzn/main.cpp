/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2010-2010 - Marco Correia <marco.v.correia@gmail.com>           *
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
 
#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include <cpp.h>

#include "driver.h"
#include <print.h>

namespace po = boost::program_options;
using namespace std;

po::variables_map processOptions(int argc, char **argv)
{
	po::options_description general("General options");
	general.add_options()
	    ("help", "this help message")
	    ("version", "print detailed build information and exit")
	    ("random-seed",po::value<int>()->default_value(0), "initialize with random seed")
	    ("max-memory",po::value<int>()->default_value(900E3), "maximum memory allowed (Kb)")
	    ("solver-stats", "print solver statistics")
//	    ("buffer-stdout",po::value<int>()->default_value(0), "buffer output")
	    ;

	po::options_description modeling("Modeling options");
	modeling.add_options()
	    ("break-symmetries",po::value<int>()->default_value(1), "try to find and break (variable) symmetries")
	    ("preds-extension",po::value<int>()->default_value(1), "convert predicates with few solutions to tables")
	    ;

	po::options_description search("Search options");
	search.add_options()
	    ("preprocess",po::value<int>()->default_value(1), "enforce sac in initial propagation")
	    ("varh-domwdeg",po::value<int>()->default_value(1), "use the domwdeg variable heuristic")
	    ("varh-impacts",po::value<int>()->default_value(1), "use the impacts variable heuristic")
	    ("varh-la",po::value<int>()->default_value(0), "use the lookahead variable heuristic")
	    ("all",po::value<bool>()->default_value(false), "search for all solutions")
	    ;

	po::options_description hidden("Hidden options");
	hidden.add_options()
	    ("input-file",po::value<string>(), "name of CSPXML file with problem to solve")
	    ;

	po::options_description cmdline;
	cmdline.add(general).add(modeling).add(search).add(hidden);

	po::options_description visible("Allowed options");
	visible.add(general).add(modeling).add(search);

	po::positional_options_description p;
	p.add("input-file",1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(cmdline).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help") or (!vm.count("input-file") and !vm.count("version")))
	{
		cout << "\nusage: " << argv[0] << " [options] input-file\n\n";
	    cout << visible << "\n";
	    exit(0);
	}

//	if (vm["buffer-stdout"].as<int>())
//		COUT.buffered = true;

	return vm;
}

void printVariable(const ICPSolver&& solver, const Symbol& s)
{
	if (s.getType(true) == Symbol::sVariable)
	{
		const Variable& v0 = s;
		if (v0.getVariableType() == Container::sInt)
			std::cout << solver.getIntVariableValue(v0);
		else
		if (v0.getVariableType() == Container::sBool)
			std::cout << solver.getBoolVariableValue(v0);
		else
			assert(0);
	}
	else
	if (s.getType(true) == Symbol::sInt or
		s.getType(true) == Symbol::sDouble or
		s.getType(true) == Symbol::sBool)
	{
		std::cout << s;
	}
	else
		assert(0);
}

void printArrayOfIntRanges(const Symbol& s)
{
	const SymbolArray& a = s;
	std::cout << static_cast<const IntRange&>(a[0]).getLower() << ".."
			  << static_cast<const IntRange&>(a[0]).getUpper();
	for (int i = 1; i < a.getSize(); ++i)
		std::cout << ","
				  << static_cast<const IntRange&>(a[i]).getLower() << ".."
				  << static_cast<const IntRange&>(a[i]).getUpper();
}

void printSolution(const casperbind::cpp::ICPSolver&& solver,const Instance& instance)
{
	for (SymbolSet::Iterator it = instance.getVariables().begin();
			it != instance.getVariables().end(); ++it)
		if (instance.getAnnotations().hasAnnotation(*it,"output_var") or
			instance.getAnnotations().hasAnnotation(*it,"output_array"))
			for (std::list<std::string>::const_iterator sit =
					instance.getIndex().getKeys(*it).begin();
					sit != instance.getIndex().getKeys(*it).end(); ++sit)
			{
				std::cout << *sit << " = ";
				if (it->getType(true) == Symbol::sVariable and
					instance.getAnnotations().hasAnnotation(*it,"output_var"))
					printVariable(solver,*it);
				else
				if (it->getType(true) == Symbol::sArray and
					instance.getAnnotations().hasAnnotation(*it,"output_array"))
				{
					const Annotation& ann = instance.getAnnotations().
											getAnnotation(*it,"output_array");
					const SymbolArray& s = *it;
					std::cout << "array" << static_cast<const SymbolArray&>(ann[0]).getSize()
							<< "d(";
					printArrayOfIntRanges(ann[0]);
					std::cout << ",[";
					printVariable(solver,s[0]);
					for (int i = 1; i < s.getSize(); ++i)
					{
						std::cout << ",";
						printVariable(solver,s[i]);
					}
					std::cout << "])";
				}
				std::cout << ";" << std::endl;
			}
}


int main(int argc, char **argv)
{
	po::variables_map vm = processOptions(argc,argv);

	cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
	cout << "%% This is fzn-casper, the flatzinc frontend for CaSPER.                          %%\n";
	cout << "%% Using libcasper-" << casperbind::cpp::getCaSPERVersion()
		 << " (rev" << casperbind::cpp::getCaSPERRevision() << ")                                                 %%\n";
	cout << "%% CaSPER - Constraint Solving Platform for Engineering and Research              %%\n";
	cout << "%% 2003-2010 Copyright (C) The CaSPER Team (http://proteina.di.fct.unl.pt/casper) %%\n";
	cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";

	if (vm.count("version"))
		return 0;

	FlatzincDriver d(0, 1000, 0,1000);
	d.parse(vm["input-file"].as<string>().c_str());
	SymStream s(std::cout, d.index);

	//std::cout << "Variables: " << std::endl;
	//s << d.variables << std::endl << std::endl << std::endl;

	//std::cout << "Constraints: " << std::endl;
	//s << d.constraints << std::endl;

	Instance instance = *d.newInstance();
	int solCount = 0;
	try
	{
		casperbind::cpp::ICPSolver& solver(instance);
		while (solver.solve())
		{
			++solCount;
			printSolution(solver,instance);
			std::cout << "----------\n";
			if (!vm["all"].as<bool>())
				break;
		}
		if (solCount == 0)
			std::cout << "=====UNSATISFIABLE=====\n";
		else
		if (vm["all"].as<bool>())
			std::cout << "==========\n";
		if (vm.count("solver-stats"))
			solver.printStats();
	}
	catch (const casperbind::cpp::EImplement& e)
	{
		std::cerr << "error (EImplemented): CaSPER does not implement the following symbol: " << std::endl;
		std::cerr << e.getSymbol() << std::endl;
	}
	catch (const casperbind::cpp::EStructure& e)
	{
		std::cerr << "error (EStructure): Invalid structure for ";
		std::cerr << e.getExpected() << ":\n";
		std::cerr << e.getSymbol() << std::endl;
	}
	catch (const casperbind::cpp::ETypeConversion& e)
	{
		std::cerr << "error (ETypeConversion): Cannot convert ";
		std::cerr << e.s << " to ";
		std::cerr << e.d << std::endl;
	}

}
