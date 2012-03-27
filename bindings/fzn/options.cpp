/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2012-2012 - Marco Correia <marco.v.correia@gmail.com>           *
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
 
#include <bindings/fzn/options.h>
#include "flatzinc.hh"

namespace FlatZinc {


Options::Options() :
		Casper::Util::ProgramOptions("Casper-FlatZinc Options"),
		outputOptions("Output Options"),
		searchOptions("Search Options"),
		mShowStats(true),
		mShowRuntime(true),
		mShowVersion(false),
		mNbSolutions(1),
		mAllSolutions(false),
		fznfilename("-")
{
	outputOptions.add_options()
	    ("show-stats,s", po::value<bool>(&mShowStats)->zero_tokens(), "show search statistics")
	    ("show-runtime,t",po::value<bool>(&mShowRuntime)->zero_tokens(), "show solving time")
	    ("show-version,v",po::value<bool>(&mShowVersion)->zero_tokens(), "show version and copyright information")
	    ;

	searchOptions.add_options()
	    ("nb-solutions,n",po::value<uint>(&mNbSolutions)->default_value(mNbSolutions), "number of solutions to search for (0=all)")
	    ("all-solutions,a",po::value<bool>(&mAllSolutions)->zero_tokens(), "search for all solutions")
	    ;

	optVisible.add(outputOptions).add(searchOptions);
	addPosParam("fznfile",&fznfilename,"path to flatzinc file");

}

bool Options::parse(int argc, char** argv)
{
	if (!ProgramOptions::parse(argc,argv))
		return false;

	if (mShowVersion)
	{
		std::cout << "This is the FlatZinc interface for CaSPER (supports FlatZinc-" << CASPER_FLATZINC_VERSION << ")" << std::endl;
		std::cout << "CaSPER - Constraint Solving Platform for Engineering and Research" << std::endl;
		std::cout << "Version " << Casper::getVersionDetailed() << std::endl;
		std::cout << "2003-2012 Copyright (C) Marco Correia (http://proteina.di.fct.unl.pt/casper)" << std::endl;
		return true;
	}
	return true;
}

bool Options::showStats() const
{	return mShowStats; }

bool Options::showRuntime() const
{	return mShowRuntime; }

bool Options::showVersion() const
{	return mShowVersion; }

uint Options::nbSolutions() const
{	return mNbSolutions; }

bool Options::allSolutions() const
{	return mAllSolutions; }

std::string Options::filename() const
{	return fznfilename;	}

}

