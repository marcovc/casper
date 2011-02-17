 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <tr1/memory>

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

#define CASPER_UNTESTED(S)	\
{std::cerr << "error: Trying to use an untested feature: " \
			  << S << endl; assert(0); }

#define CASPER_UNIMPLEMENTED(S)	\
{std::cerr << "error: Trying to use an unimplemented feature: " \
			  << S << endl; assert(0); }

#define CASPER_SAFE_ROUNDING	// temporary

#define EVER ; ;

#if __GNUC__ < 3
#define force_inline inline
#else
#define force_inline inline __attribute__ ((always_inline))
#endif

/// Global namespace for the CaSPER library
namespace casper {

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

/// \name Portable datatypes
//@{
/// Void data type, currently an alias to builtin \a void type.
typedef void		Void;
/// Int data type, currently an alias to builtin \a int type.
typedef int 		Int;
/// Float data type, currently an alias to builtin \a float type.
typedef float 		Float;
/// Double data type, currently an alias to builtin \a double type.
typedef double 		Double;
/// LongDouble data type, currently an alias to builtin \a long_double type.
typedef double 		LongDouble;
/// Boolean data type, currently an alias to builtin \a bool type.
typedef bool 		Bool;
/// Char data type, currently an alias to builtin \a char type.
typedef char		Char;
/// Short data type, currently an alias to builtin \a short type.
typedef short		Short;
/// Long data type, currently an alias to builtin \a long type.
typedef long		Long;
/// LongLong data type, currently an alias to builtin \a long_long type.
typedef long long 	LongLong;
/// LongLong data type, currently an alias to builtin \a unsigned_long_long type.
typedef unsigned long long 	ULongLong;
/// UInt data type, currently an alias to builtin \a unsigned_int type.
typedef unsigned int UInt;
/// String data type, currently an alias to stl \a string type.
typedef std::string	String;
//@}

struct Bounds;
struct Value;
struct Domain;

template<class T>
struct Seq
{	typedef T Elem;	};

typedef Seq<Int>	IntSeq;
typedef Seq<Bool>	BoolSeq;

template<class T>
struct EvalTraits
{	typedef	T	Eval;	};

template<>
struct EvalTraits<UInt>
{	typedef	Int	Eval;	};

template<>
struct EvalTraits<Short>
{	typedef	Int	Eval;	};

template<>
struct EvalTraits<Long>
{	typedef	Int	Eval;	};

template<class T>
struct ElemTraits;

// msvc defines these as macros
#undef min
#undef max

// gcc defines these as c++ operators
// FIXME: must uncomment this on windows
/*
#ifndef and
	#define and &&
#endif

#ifndef or
	#define or ||
#endif
*/

/// \internal Everything inside detail namespace is not to be documented
namespace detail {

/// Pointer to implementation idiom
template<class Iface>
struct PImplIdiom
{
	typedef Iface* PImpl;
	PImplIdiom(Iface* pImpl) : pImpl(pImpl) { }
	PImplIdiom(const PImplIdiom& s) : pImpl(s.getPImpl()) { }
	Iface&				getImpl() 			{ return *pImpl;}
	const Iface&		getImpl() const 	{ return *pImpl;}
	PImpl&				getPImpl() 			{ return pImpl;}
	const PImpl&		getPImpl() const 	{ return pImpl;}

	const PImplIdiom<Iface>& operator=(const PImplIdiom<Iface>& s)
	{	pImpl = s.pImpl; return *this; 	}

	Bool operator==(const PImplIdiom<Iface>& s) const
	{	return pImpl == s.pImpl; }

	PImpl pImpl;
};

/// Pointer to implementation idiom, with automatic garbage collection
template<class Iface>
struct SPImplIdiom
{
	typedef std::tr1::shared_ptr<Iface> PImpl;
	SPImplIdiom(Iface* pImpl) : pImpl(pImpl) { }
	SPImplIdiom(PImpl pImpl) : pImpl(pImpl) { }
	SPImplIdiom(const SPImplIdiom& s) : pImpl(s.getPImpl()) { }
	Iface&				getImpl() 			{ return *pImpl;}
	const Iface&		getImpl() const 	{ return *pImpl;}
	PImpl&				getPImpl() 			{ return pImpl;}
	const PImpl&		getPImpl() const 	{ return pImpl;}

	const SPImplIdiom<Iface>& operator=(const SPImplIdiom<Iface>& s)
	{	pImpl = s.pImpl; return *this; 	}

	Bool operator==(const SPImplIdiom<Iface>& s) const
	{	return pImpl == s.pImpl; }

	PImpl pImpl;
};

template<class T>
class SwitchSignedFlag
{
public:
	class Type {};
};

template<>
class SwitchSignedFlag<unsigned int>
{
public:
	typedef int Type;
};

template<>
class SwitchSignedFlag<unsigned char>
{
public:
	typedef char Type;
};

template<>
class SwitchSignedFlag<unsigned short>
{
public:
	typedef short Type;
};

template<>
class SwitchSignedFlag<unsigned long long>
{
public:
	typedef long long Type;
};

template<>
class SwitchSignedFlag<int>
{
public:
	typedef unsigned int Type;
};

template<>
class SwitchSignedFlag<char>
{
public:
	typedef unsigned char Type;
};

template<>
class SwitchSignedFlag<short>
{
public:
	typedef unsigned short Type;
};

template<>
class SwitchSignedFlag<long long>
{
public:
	typedef unsigned long long Type;
};

// FIXME: replace by c++ builtin traits when ready
template<class T>
struct IsIntegral
{	static const bool value = false;	};

template<>
struct IsIntegral<Int>
{	static const bool value = true;	};

template<>
struct IsIntegral<Bool>
{	static const bool value = false;	};

};

/*! \mainpage CaSPER - Constraint Solving Programming Environment for Research
 *
 * See website (http://proteina.di.fct.unl.pt/casper)
*/

/** \defgroup Examples Examples
 * 	Examples of using CaSPER.
 */

/** \defgroup Domains Domains
 * 	Domains of reasoning available in CaSPER.
 */

/** \defgroup Kernel Kernel
 * 	Building blocks of the CaSPER library.
 */

/** \defgroup Containers Containers
 * 	\ingroup Kernel
 * 	Useful reversible and standard generic containers.
 */

};

#endif

