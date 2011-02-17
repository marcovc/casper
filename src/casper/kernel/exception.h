 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_KERNEL_EXCEPTION_H_
#define CASPER_KERNEL_EXCEPTION_H_

#include <exception>
#include <stdexcept>
#include <string>

namespace casper {

struct EIndexError : std::runtime_error
{
	EIndexError() : std::runtime_error("index error") {}
};

struct ETimeOut : std::runtime_error
{
	ETimeOut() : std::runtime_error("process timed out") {}
};

typedef std::bad_alloc	EMemOut;

struct EConversion : std::runtime_error
{
	EConversion(std::string e) : std::runtime_error(
			std::string("conversion error : ")+e)
	{}
};

struct EDivisionByZero : std::runtime_error
{
	EDivisionByZero() : std::runtime_error("division by zero")
	{}
};

struct ENoFilter: std::runtime_error
{
	ENoFilter(std::string c) : std::runtime_error(
			std::string("no filter for constraint: ")+c)
	{}
};

struct EEmptySetVar : std::runtime_error
{
	EEmptySetVar(std::string c) : std::runtime_error(
			std::string("constraint ")+c+
			std::string(" over an empty set variable"))
	{}
};

struct ENonReifiable : std::runtime_error
{
	ENonReifiable(std::string c) : std::runtime_error(
			std::string("constraint ")+c+
			std::string(" is non reifiable"))
	{}
};

struct ENullSolver : std::runtime_error
{
	ENullSolver() : std::runtime_error("attempt to use a null solver object") {}
};

}

#endif /*CASPER_KERNEL_EXCEPTION_H_*/
