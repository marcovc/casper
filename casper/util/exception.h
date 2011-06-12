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

#ifndef CASPER_UTIL_EXCEPTION_H_
#define CASPER_UTIL_EXCEPTION_H_

#include <exception>
#include <stdexcept>
#include <string>

namespace Casper {
namespace Exception {


struct IndexError : std::runtime_error
{
	IndexError() : std::runtime_error("index error") {}
};

struct IndexOutOfBounds : std::runtime_error
{
	IndexOutOfBounds() : std::runtime_error("array index out of bounds") {}
};

struct TimeOut : std::runtime_error
{
	TimeOut() : std::runtime_error("process timed out") {}
};

typedef std::bad_alloc	EMemOut;

struct Conversion : std::runtime_error
{
	Conversion(std::string e) : std::runtime_error(
			std::string("conversion error : ")+e)
	{}
};

struct DivisionByZero : std::runtime_error
{
	DivisionByZero() : std::runtime_error("division by zero")
	{}
};

struct NoFilter: std::runtime_error
{
	NoFilter(std::string c) : std::runtime_error(
			std::string("no CP::Filter object found for constraint: ")+c)
	{}
};

struct EmptySetVar : std::runtime_error
{
	EmptySetVar(std::string c) : std::runtime_error(
			std::string("constraint ")+c+
			std::string(" over an empty set variable"))
	{}
};

struct NonReifiable : std::runtime_error
{
	NonReifiable(std::string c) : std::runtime_error(
			std::string("constraint ")+c+
			std::string(" is non reifiable"))
	{}
};

struct InvalidOperation : std::runtime_error
{
	InvalidOperation(std::string c) : std::runtime_error(
			std::string("invalid operation: ")+c)
	{}
};

struct NotGround : std::runtime_error
{
	NotGround() : std::runtime_error("expecting a ground expression")
	{}
};

struct Extract : std::runtime_error
{
	Extract(std::string obj) : std::runtime_error("could not extract "+obj+" from object") {}
};

struct NonLinear : std::runtime_error
{
	NonLinear() : std::runtime_error("unexpected non-linear expression") {}
};

} // Exception
} // Casper

#endif /*CASPER_KERNEL_EXCEPTION_H_*/
