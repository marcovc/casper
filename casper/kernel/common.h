 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_COMMON
#define _H_CASPER_KERNEL_COMMON

#ifdef _MSC_VER
#include <iso646.h>
#endif

#include <iostream>
#include <limits>
#include <string>

//#include <tr1/memory>


/**
	\file kernel/common.h
    \brief Global definitions for the CaSPER library
*/

#ifdef C_DEBUG

#define C_DEBUG_COUT(X) std::cout << X << std::endl;
#define C_LOG_MEM(X) std::cout << X << std::endl;

#else

#define C_DEBUG_COUT(X)
#define C_LOG_MEM(X)

#endif

/// Defines is Casper treats arrays as 0 or 1 indexed based arrays
#define CASPER_ELEMENT_ARRAY_BASE	0

#define CASPER_UNTESTED(S)	\
{std::cerr << "error: Trying to use an untested feature: " \
			  << S << std::endl; assert(0); }

#define CASPER_UNIMPLEMENTED(S)	\
{std::cerr << "error: Trying to use an unimplemented feature: " \
			  << S << std::endl; assert(0); }

#define CASPER_SAFE_ROUNDING	// temporary

#if __GNUC__ < 3
#define force_inline inline
#else
#define force_inline inline __attribute__ ((always_inline))
#endif

#if defined(__APPLE__) | defined(_MSC_VER) | defined(SWIG)
typedef unsigned int uint;
#endif

/// Global namespace for the CaSPER library
namespace Casper {

/// \name Datatypes aliases
//@{
typedef long long 	longlong;
/// LongLong data type, currently an alias to builtin \a unsigned_long_long type.
typedef unsigned long long 	ulonglong;
/// uint data type, currently an alias to builtin \a unsigned_int type.
//typedef unsigned int uint;
/// String data type, currently an alias to stl \a string type.
typedef std::string	string;
/// Integer data type with largest available size.
typedef ulonglong counter;
//@}

/**
	Standard defined for a generic datatype, based on the std::numeric_limits class.
	This should work for common pod datatypes (int,short,unsigned char, etc...) but
	must be specialized for user-defined data types.
*/
template<class T>
struct limits
{
	static const T max()	{	return std::numeric_limits<T>::max();	}
	static const T min()	{	return std::numeric_limits<T>::min();	}
	static const T inf()	{	return std::numeric_limits<T>::infinity();	}
	static const T posInf()	{	return limits<T>::inf();	}
	static const T negInf()	{	return -limits<T>::inf();	}
};

template<>
struct limits<int>
{
	static const int max()	{	return std::numeric_limits<int>::max();	}
	static const int min()	{	return std::numeric_limits<int>::min();	}
	static const int inf()	{	return max();	}
	static const int posInf()	{	return max();	}
	static const int negInf()	{	return min();	}
};

template<>
struct limits<uint>
{
	static const uint max()	{	return std::numeric_limits<uint>::max();	}
	static const uint min()	{	return std::numeric_limits<uint>::min();	}
	static const uint inf()	{	return max();	}
	static const uint posInf()	{	return max();	}
	static const uint negInf()	{	return min();	}
};

template<>
struct limits<longlong>
{
	static const longlong max()	{	return std::numeric_limits<longlong>::max();	}
	static const longlong min()	{	return std::numeric_limits<longlong>::min();	}
	static const longlong inf()	{	return max();	}
	static const longlong posInf()	{	return max();	}
	static const longlong negInf()	{	return min();	}
};

template<>
struct limits<ulonglong>
{
	static const ulonglong max()	{	return std::numeric_limits<ulonglong>::max();	}
	static const ulonglong min()	{	return std::numeric_limits<ulonglong>::min();	}
	static const ulonglong inf()	{	return max();	}
	static const ulonglong posInf()	{	return max();	}
	static const ulonglong negInf()	{	return min();	}
};


template<class T>
struct Set
{	typedef T	Elem;	};

///	Set of integers. \ingroup Kernel
typedef Set<int>	IntSet;

///	Set of booleans. \ingroup Kernel
typedef Set<bool>	BoolSet;


template<class T>
struct Seq
{	typedef T Type;	};

///	Sequence of integers. \ingroup Kernel
typedef Seq<int>	IntSeq;

///	Sequence of booleans. \ingroup Kernel
typedef Seq<bool>	BoolSeq;

///	Sequence of sets of integers. \ingroup Kernel
typedef Seq<IntSet>	IntSetSeq;

///	Sequence of sets of booleans. \ingroup Kernel
typedef Seq<BoolSet>	BoolSetSeq;

///	Sequence of integers. \ingroup Kernel
typedef Seq<float>	FloatSeq;

///	Sequence of booleans. \ingroup Kernel
typedef Seq<double>	DoubleSeq;

//msvc defines these as macros
#undef min
#undef max


/// \internal Everything inside detail namespace is not to be documented
namespace Detail {



};

/*! \mainpage CaSPER - Constraint Solving Programming Environment for Research
 *
 * See website (http://proteina.di.fct.unl.pt/casper)
*/

/** \defgroup CP CP
 * 	Constraint Programming module in CaSPER.
 */

/** \defgroup Kernel Kernel
 * 	Building blocks of the CaSPER library.
 */

/** \defgroup Containers Containers
 * 	\ingroup Kernel
 * 	Useful reversible and standard generic containers.
 */

/** \defgroup Examples Examples
 * 	Examples of using CaSPER.
 */

};

#endif

