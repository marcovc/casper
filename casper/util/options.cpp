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
 
#include <casper/util/options.h>
#include <casper/version.h>


using namespace std;

namespace Casper {
namespace Util {


ProgramOptions::ProgramOptions(const std::string& name) : optVisible(name),optGeneral("General Options")
{
	optGeneral.add(
		boost::shared_ptr<po::option_description>(new po::option_description("help,h",
								po::value<bool>()->zero_tokens(),"display help message")));
	optVisible.add(optGeneral);
}

bool ProgramOptions::parse(int argc, char** argv)
{
	optAll.add(optVisible);
	bool parsedOk = true;
	try {
		po::store(po::command_line_parser(argc, argv).options(optAll).positional(optPositional).run(),vm);
		po::notify(vm);
	} catch (po::error& err)
	{
		cout << "error parsing command line:\n" << err.what() << endl << endl;
		parsedOk= false;
	}

	if (!parsedOk or vm.count("help"))
	{
		cout << "== Help ==" << endl << endl;
		cout << "usage: " << endl;
		cout << "\t" << argv[0];
		for (uint i = 0; i < optPositional.max_total_count(); ++i)
			cout << " " << optPositional.name_for_position(i);
		cout << " [options]";
		cout << endl << endl;
		cout << "where," << endl;
		for (uint i = 0; i < optPositional.max_total_count(); ++i)
		{
			const po::option_description& opt = optAll.find(optPositional.name_for_position(i),false);
			boost::any defaultValue;
			if (opt.semantic()->apply_default(defaultValue))
				cout << "\t" << opt.long_name() << opt.format_parameter().substr(3) << "\t\t" << opt.description() << endl;
			else
				cout << "\t" << opt.long_name() << "\t\t" << opt.description() << endl;
		}
		cout << endl;
		cout << "and options is a combination of,\n\n";
	    cout << optVisible << endl;
	    return false;
	}

	return true;
}

ExampleOptions::ExampleOptions(const std::string& example_name) :
		ProgramOptions(example_name+" Options"),
		outputOptions("Output Options"),
		searchOptions("Search Options"),
		mShowStats(true),
		mShowRuntime(true),
		mShowVersion(false),
		mNbSolutions(1)
{
	outputOptions.add_options()
	    ("show-stats,a", po::value<bool>(&mShowStats)->implicit_value(true), "show search statistics")
	    ("show-runtime,t",po::value<bool>(&mShowRuntime)->implicit_value(true), "show solving time")
	    ("show-version,v",po::value<bool>(&mShowVersion)->implicit_value(true), "show casper version and copyright")
	    ;

	searchOptions.add_options()
	    ("solutions,s",po::value<uint>(&mNbSolutions)->default_value(mNbSolutions), "number of solutions (0=all)")
	    ;

	optVisible.add(outputOptions).add(searchOptions);
}

bool ExampleOptions::parse(int argc, char** argv)
{
	if (!ProgramOptions::parse(argc,argv))
		return false;

	if (mShowVersion)
	{
		cout << "CaSPER - Constraint Solving Platform for Engineering and Research" << endl;
		cout << "2003-2011 Copyright (C) The CaSPER team (http://proteina.di.fct.unl.pt/casper)" << endl;
		cout << "using version " << getVersionDetailed() << endl;
		return true;
	}
	return true;
}

bool ExampleOptions::showStats() const
{	return mShowStats; }

bool ExampleOptions::showRuntime() const
{	return mShowRuntime; }

bool ExampleOptions::showVersion() const
{	return mShowVersion; }

uint ExampleOptions::nbSolutions() const
{	return mNbSolutions; }

} // Util
} // Casper

