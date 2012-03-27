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
 

#ifndef CASPER_BINDINGS_FZN_OPTIONS_H_
#define CASPER_BINDINGS_FZN_OPTIONS_H_

#include <casper/util/options.h>
#include <casper/version.h>

namespace FlatZinc {

namespace po = boost::program_options;

struct Options : Casper::Util::ProgramOptions
{
	po::options_description outputOptions;
	po::options_description searchOptions;

	bool mShowStats;
	bool mShowRuntime;
	bool mShowVersion;
	uint mNbSolutions;
	bool mAllSolutions;
	std::string fznfilename;

	Options();
	bool showStats() const;
	bool showRuntime() const;
	bool showVersion() const;
	uint nbSolutions() const;
	bool allSolutions() const;
	std::string filename() const;

	bool parse(int argc, char** argv);
};


}

#endif /* CASPER_BINDINGS_FZN_OPTIONS_H_ */
