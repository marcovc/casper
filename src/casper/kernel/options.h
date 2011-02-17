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
 

#ifndef CASPER_OPTIONS_H_
#define CASPER_OPTIONS_H_

#include <casper/kernel/common.h>
#include <boost/program_options.hpp>
#include <string>

namespace casper {


namespace po = boost::program_options;

// just a wrapper around boost::program_options
struct ProgramOptions
{
	po::options_description 			optAll;
	po::positional_options_description 	optPositional;
	po::options_description 			optVisible;
	po::options_description 			optGeneral;

	po::variables_map vm;

	ProgramOptions(const std::string& name);

	template<class T>
	ProgramOptions& addOptParam(const std::string& str, T* ptrArg, const std::string& desc)
	{
		optVisible.add(
			boost::shared_ptr<po::option_description>(new po::option_description(str.c_str(),
									po::value<T>(ptrArg)->default_value(*ptrArg),desc.c_str())));
			return *this;
	}

	template<class T>
	ProgramOptions& addOptParam(const std::string& str, const T& refArg, const std::string& desc)
	{
		optVisible.add(
			boost::shared_ptr<po::option_description>(new po::option_description(str.c_str(),
									po::value<T>()->default_value(refArg),desc.c_str())));
			return *this;
	}

	template<class T>
	ProgramOptions& addPosParam(const std::string& str, T* ptrArg, const std::string& desc)
	{
		optPositional.add(str.c_str(),1);
		optAll.add(
			boost::shared_ptr<po::option_description>(new po::option_description(str.c_str(),
									po::value<T>(ptrArg)->default_value(*ptrArg),desc.c_str())));
		return *this;
	}

	template<class T>
	ProgramOptions& addPosParam(const std::string& str, const T& refArg, const std::string& desc)
	{
		optPositional.add(str.c_str(),1);
		optAll.add(
			boost::shared_ptr<po::option_description>(new po::option_description(str.c_str(),
									po::value<T>()->default_value(refArg),desc.c_str())));
		return *this;
	}

	template<class T>
	const T& get(const std::string& str) const { return vm[str].as<T>(); }

	virtual Bool parse(int argc, char** argv);
};

struct ExampleOptions : ProgramOptions
{
	po::options_description outputOptions;
	po::options_description searchOptions;

	bool mShowStats;
	bool mShowRuntime;
	bool mShowVersion;
	UInt mNbSolutions;

	ExampleOptions(const std::string& exampleName);
	bool showStats() const;
	bool showRuntime() const;
	bool showVersion() const;
	UInt nbSolutions() const;

	Bool parse(int argc, char** argv);
};

// example specific options:
// size,heuristic,minimize,...


}

#endif /* CASPER_EXAMPLES_OPTIONS_H_ */
