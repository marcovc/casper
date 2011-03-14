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
 

#ifndef CASPERBIND_CPP_SOLVER_H_
#define CASPERBIND_CPP_SOLVER_H_

#include <casper/version.h>
#include "instance.h"

namespace Casperbind {
namespace cpp {

struct CPSolver&Impl;

struct CPSolver&
{
	CPSolver&Impl* pImpl;

	CPSolver&(const Instance& instance);

	bool solve();
	bool next();
	void printSolution();
	void printStats();
	int getIntVariableValue(const Variable& v) const;
	bool getBoolVariableValue(const Variable& v) const;
};

inline unsigned int getCaSPERVersionMajor()
{	return Casper::getVersionMajor(); }

inline unsigned int getCaSPERVersionMinor()
{	return Casper::getVersionMinor(); }

inline unsigned int getCaSPERVersionRelease()
{	return Casper::getVersionRelease(); }

inline unsigned int getCaSPERRevision()
{	return Casper::getRevision(); }

inline std::string getCaSPERVersion()
{	return Casper::getVersion(); }

inline std::string getCaSPERBuildCompiler()
{	return Casper::getBuildCompiler(); }

inline std::string getCaSPERBuildStdlib()
{	return Casper::getBuildStdlib(); }

inline std::string getCaSPERBuildFlags()
{	return Casper::getBuildFlags(); }

inline std::string getCaSPERBuildDate()
{	return Casper::getBuildDate(); }




}
}

#endif /* CASPERBIND_CPP_SOLVER_H_ */
