 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_SAFEOP
#define _H_CASPER_KERNEL_SAFEOP

#include <boost/numeric/interval/hw_rounding.hpp>
#include <boost/numeric/interval/rounded_arith.hpp>
#include <boost/numeric/interval/rounded_transc.hpp>

//#include <boost/math/special_functions/next.hpp>

#ifndef CASPER_UNSAFE_ROUNDING

//#include <gmp.h>
#include <mpfr.h>
#include <stdint.h>

namespace Casper {
namespace Util {


// the following intends to replace functionality from boost::interval
// it works but needs testing.
#if 0
// Arithmetic functions need special rounding class
template<class T>
struct mpfr_rounded_std :boost::numeric::interval_lib::rounded_arith_std<T>
{
private:
  typedef int mpfr_func_1(mpfr_t, const __mpfr_struct*, mp_rnd_t);
  typedef int mpfr_func_2(mpfr_t, const mpfr_t,const mpfr_t, mp_rnd_t);
  double invoke_mpfr(double opd, mpfr_func_1 f, mp_rnd_t r)
  {
    mpfr_t mp_opd;
    mpfr_init_set_d(mp_opd, opd, GMP_RNDN);
    f(mp_opd, mp_opd, r);
    double res = mpfr_get_d(mp_opd, r);
    mpfr_clear(mp_opd);
    return res;
  }
  double invoke_mpfr(double opd1, double opd2, mpfr_func_2 f, mp_rnd_t r)
  {
    mpfr_t mp_res,mp_opd1,mp_opd2;
    mpfr_init(mp_res);
    mpfr_init_set_d(mp_opd1, opd1, GMP_RNDN);
    mpfr_init_set_d(mp_opd2, opd2, GMP_RNDN);
    f(mp_res, mp_opd1, mp_opd2, r);
    double res = mpfr_get_d(mp_res, r);
    mpfr_clear(mp_res);
    mpfr_clear(mp_opd1);
    mpfr_clear(mp_opd2);
    return res;
  }

public:
# define GENR_FUNC_1(name) \
  double name##Lb(double x) { return invoke_mpfr(x, mpfr_##name, GMP_RNDD); } \
  double name##Ub  (double x) { return invoke_mpfr(x, mpfr_##name, GMP_RNDU); }
# define GENR_FUNC_2(name) \
  double name##Lb(double x,double y) { return invoke_mpfr(x,y, mpfr_##name, GMP_RNDD); } \
  double name##Ub  (double x,double y) { return invoke_mpfr(x,y, mpfr_##name, GMP_RNDU); }

  GENR_FUNC_2(add)
  GENR_FUNC_2(sub)
  GENR_FUNC_2(mul)
  GENR_FUNC_2(div)
 // GENR_FUNC_2(median)
  GENR_FUNC_1(sqrt)
};
#endif

// Transcendental functions need special rounding class
template<class T>
struct mpfr_rounded_transc
{
private:
  //typedef int mpfr_func(mpfr_t, const __mpfr_struct*, mp_rnd_t);
	typedef int mpfr_func(mpfr_t,const mpfr_t, mp_rnd_t);
	typedef int mpfr_func2(mpfr_t,const mpfr_t,const mpfr_t, mp_rnd_t);
  double invoke_mpfr(double x, mpfr_func f, mp_rnd_t r) {
    mpfr_t xx;
    mpfr_init_set_d(xx, x, GMP_RNDN);
    f(xx, xx, r);
    double res = mpfr_get_d(xx, r);
    mpfr_clear(xx);
    return res;
  }
  double invoke_mpfr(double x, double y, mpfr_func2 f, mp_rnd_t r) {
    mpfr_t xx;
    mpfr_t yy;
    mpfr_init_set_d(xx, x, GMP_RNDN);
    mpfr_init_set_d(yy, y, GMP_RNDN);
    f(xx, xx, yy, r);
    double res = mpfr_get_d(xx, r);
    mpfr_clear(xx);
    mpfr_clear(yy);
    return res;
  }

public:
# define GENR_FUNC(name) \
  static double name##Lb(double x) { return invoke_mpfr(x, mpfr_##name, GMP_RNDD); } \
  static double name##Ub  (double x) { return invoke_mpfr(x, mpfr_##name, GMP_RNDU); }
# define GENR_FUNC2(name) \
  static double name##Lb(double x,double y) { return invoke_mpfr(x,y, mpfr_##name, GMP_RNDD); } \
  static double name##Ub  (double x,double y) { return invoke_mpfr(x,y, mpfr_##name, GMP_RNDU); }
  GENR_FUNC(exp)
  GENR_FUNC(log)
  GENR_FUNC(sin)
  GENR_FUNC(cos)
  GENR_FUNC(tan)
  GENR_FUNC(asin)
  GENR_FUNC(acos)
  GENR_FUNC(atan)
  GENR_FUNC(sinh)
  GENR_FUNC(cosh)
  GENR_FUNC(tanh)
  GENR_FUNC(asinh)
  GENR_FUNC(acosh)
  GENR_FUNC(atanh)
  GENR_FUNC2(pow)

  boost::numeric::interval_lib::rounded_arith_std<T>	rounder;
};

// Utility class for avoiding numeric overflows below
template<class>
struct GetHigherRank;

template<>
struct GetHigherRank<std::int8_t>
{	typedef std::int16_t Type;	};

template<>
struct GetHigherRank<std::int16_t>
{	typedef std::int32_t Type;	};

template<>
struct GetHigherRank<std::int32_t>
{	typedef std::int64_t Type;	};

//template<>
//struct GetHigherRank<std::int64_t>
//{	typedef std::int128_t Type;	};

// Special rounding policy for handling integers, since its missing from
// boost::interval library (nothing needs rounding, except division which is
// always rounded in the non-conservative way)
template<class T, class Rounder = boost::numeric::interval_lib::rounding_control<T> >
struct rounded_arith_integer : Rounder
{
# define BOOST_DN(EXPR) rounder.downward();   return rounder.force_rounding(EXPR)
# define BOOST_NR(EXPR) rounder.to_nearest(); return rounder.force_rounding(EXPR)
# define BOOSTUb(EXPR) rounder.upward();     return rounder.force_rounding(EXPR)

  template<class U>
  static T convLb(U const &v)
  {	 return static_cast<T>(std::ceil(v)); }
  template<class U>
  static T convUb  (U const &v)
  {	return static_cast<T>(std::floor(v)); }
  static T convLb(bool const &v)
  {	 return static_cast<T>(std::ceil(static_cast<double>(v))); }
  static T convUb  (bool const &v)
  {	return static_cast<T>(std::floor(static_cast<double>(v))); }
  static T convLb(int const &v)
  {	 return static_cast<T>(std::ceil(static_cast<double>(v))); }
  static T convUb  (int const &v)
  {	return static_cast<T>(std::floor(static_cast<double>(v))); }

  static T addLb(const T& x, const T& y)
  {
#ifdef CASPER_CHECK_OVERFLOW
	  typedef typename GetHigherRank<T>::Type HT;
	  HT r = static_cast<HT>(x) + static_cast<HT>(y);
	  assert(r <= static_cast<HT>(limits<T>::posInf()));
	  if (r < static_cast<HT>(limits<T>::negInf()))
		  return limits<T>::negInf();
	  return r;
#else
	  return x+y;
#endif
  }
  static T subLb(const T& x, const T& y)
  {
#ifdef CASPER_CHECK_OVERFLOW
	  typedef typename GetHigherRank<T>::Type HT;
	  HT r = static_cast<HT>(x) - static_cast<HT>(y);
	  assert(r <= static_cast<HT>(limits<T>::posInf()));
	  if (r < static_cast<HT>(limits<T>::negInf()))
		  return limits<T>::negInf();
	  return r;
#else
	  return x-y;
#endif
  }
  static T mulLb(const T& x, const T& y)
  {
#ifdef CASPER_CHECK_OVERFLOW
	  typedef typename GetHigherRank<T>::Type HT;
	  HT r = static_cast<HT>(x) * static_cast<HT>(y);
	  assert(r <= static_cast<HT>(limits<T>::posInf()));
	  if (r < static_cast<HT>(limits<T>::negInf()))
		  return limits<T>::negInf();
	  return r;
#else
	  return x*y;
#endif
  }
  static T divLb(const T& x, const T& y)
  {	return convLb(static_cast<double>(x) / y);	}
  static T addUb  (const T& x, const T& y)
  {
#ifdef CASPER_CHECK_OVERFLOW
	  typedef typename GetHigherRank<T>::Type HT;
	  HT r = static_cast<HT>(x) + static_cast<HT>(y);
	  assert(r >= static_cast<HT>(limits<T>::negInf()));
	  if (r > static_cast<HT>(limits<T>::posInf()))
		  return limits<T>::posInf();
	  return r;
#else
	  return x+y;
#endif
  }
  static T subUb  (const T& x, const T& y)
  {
#ifdef CASPER_CHECK_OVERFLOW
	  typedef typename GetHigherRank<T>::Type HT;
	  HT r = static_cast<HT>(x) - static_cast<HT>(y);
	  assert(r >= static_cast<HT>(limits<T>::negInf()));
	  if (r > static_cast<HT>(limits<T>::posInf()))
		  return limits<T>::posInf();
	  return r;
#else
	  return x-y;
#endif
  }
  static T mulUb  (const T& x, const T& y)
  {
#ifdef CASPER_CHECK_OVERFLOW
	  typedef typename GetHigherRank<T>::Type HT;
	  HT r = static_cast<HT>(x) * static_cast<HT>(y);
	  assert(r >= static_cast<HT>(limits<T>::negInf()));
	  if (r > static_cast<HT>(limits<T>::posInf()))
		  return limits<T>::posInf();
	  return r;
#else
	  return x*y;
#endif
  }
  static T divUb  (const T& x, const T& y)
  {	return convUb(static_cast<double>(x) / y);	}

  /// division when the sign of x and y is known in advance
  /// (assumes integer division rounds toward zero)
  static T divUbPP(const T& x, const T& y)
  {	return x/y;	}
  static T divLbPP(const T& x, const T& y)
  {	return (x+y-1)/y;	}
  static T divUbPN(const T& x, const T& y)
  {	return (x+y+1)/y;	}
  static T divLbPN(const T& x, const T& y)
  {	return x/y; }
  static T divUbNP(const T& x, const T& y)
  {	return (x-y+1)/y;	}
  static T divLbNP(const T& x, const T& y)
  {	return x/y; }
  static T divUbNN(const T& x, const T& y)
  {	return x/y; }
  static T divLbNN(const T& x, const T& y)
  {	return (x+y+1)/y;	}

  static T median(const T& x, const T& y) { BOOST_NR((x + y) / 2); }
  static T sqrtLb(const T& x)
  { BOOST_NUMERIC_INTERVAL_using_math(sqrt); BOOSTUb(sqrt(x)); }
  static T sqrtUb  (const T& x)
  { BOOST_NUMERIC_INTERVAL_using_math(sqrt); BOOST_DN(sqrt(x)); }
  static T intLb(const T& x) { return x; }
  static T intUb  (const T& x) { return x; }
  static T powUb  (const T& x, const T& y) { return std::pow(x,y); }
  static T powLb  (const T& x, const T& y) { return std::pow(x,y); }
  static T expUb  (const T& x) { return std::exp(x); }
  static T expLb  (const T& x) { return std::exp(x); }
  static T logUb  (const T& x) { return std::log(x); }
  static T logLb  (const T& x) { return std::log(x); }

  static Rounder rounder;

# undef BOOST_DN
# undef BOOST_NR
# undef BOOSTUb
};

};
};
#endif


namespace Casper {
namespace Util {

template<class T>
struct Round : boost::numeric::interval_lib::save_state<mpfr_rounded_transc<T> >
{};


template<>
struct Round<int> : rounded_arith_integer<int>
{};

template<>
struct Round<bool> : rounded_arith_integer<bool>
{};

};
};




namespace Casper {
namespace Util {


// conversion

template<class To,class From>
struct Convert
{
	static To down(const From& from)
	{	return Round<To>::convLb(from); }
	static To up(const From& from)
	{	return Round<To>::convUb(from); }
};

template<class To>
struct Convert<To,To>
{
	static To down(const To& from)
	{	return from; }
	static To up(const To& from)
	{	return from; }
};

template<>
struct Convert<bool,int>
{
	static bool down(const int& from)
	{	return from != 0 ; }
	static bool up(const int& from)
	{	return from != 0; }
};

template<class Eval, class T1>
Eval convLb(const T1& t1)
{	return Convert<Eval,T1>::down(t1);	}

template<class Eval, class T1>
Eval convUb(const T1& t1)
{	return Convert<Eval,T1>::up(t1);	}

template<class From,class To> struct ConvertIfInteger	{ typedef From Type; };
template<class To> struct ConvertIfInteger<int,To>	{ typedef To Type; };
template<class To> struct ConvertIfInteger<uint,To>	{ typedef To Type; };
template<class To> struct ConvertIfInteger<bool,To>	{ typedef To Type; };

// test

template<class T>
bool isNeg(const T& v)
{	return v < static_cast<T>(0);	}

template<class T>
bool isPos(const T& v)
{	return v > static_cast<T>(0);	}

// canonical

namespace Detail {
template<class T>
struct Succ
{	
	T operator()(const T& v)
	{	return std::nextafter(v, std::numeric_limits<T>::max());	}
};
template<>
struct Succ<int> 
{	
	int operator()(const int& t) 
	{	return t+1;	}
};

template<class T>
struct Pred
{	
	T operator()(const T& v)
	{	return std::nextafter(v, -std::numeric_limits<T>::max());	}
};
template<>
struct Pred<int>
{	
	int operator()(const int& t) 
	{	return t-1;	}
};
} // Detail

template<class T>
T succ(const T& v)
{
	return Detail::Succ<T>()(v);
	//return boost::math::float_next(v);
}

template<class T>
T pred(const T& v)
{
	return Detail::Pred<T>()(v);
//	return boost::math::float_prior(v);
}

template<class T>
bool isCanonical(const T& min, const T& max)
{	return succ(min)==max;	}

// arithmetic

template<class Eval>
Eval addUb(const Eval& t1, const Eval& t2)
{
	return Round<Eval>::addUb(t1,t2);
}

template<class Eval>
Eval addLb(const Eval& t1, const Eval& t2)
{
	return Round<Eval>::addLb(t1,t2);
}

template<class Eval>
Eval subUb(const Eval& t1, const Eval& t2)
{
	return Round<Eval>::subUb(t1,t2);
}

template<class Eval>
Eval subLb(const Eval& t1, const Eval& t2)
{
	return Round<Eval>::subLb(t1,t2);
}

template<class Eval>
Eval mulUb(const Eval& t1, const Eval& t2)
{
	return Round<Eval>::mulUb(t1,t2);
}

template<class Eval>
Eval mulLb(const Eval& t1, const Eval& t2)
{
	return Round<Eval>::mulLb(t1,t2);
}

template<class Eval>
Eval divUb(const Eval& t1, const Eval& t2)
{
	return Round<Eval>::divUb(t1,t2);
}

template<class Eval>
Eval divLb(const Eval& t1, const Eval& t2)
{
	return Round<Eval>::divLb(t1,t2);
}

/*
 *  The following 8 routines divide t1 by t2 when
 *  the sign of t1 and t2 is known in advance.
 *  This speeds up computation for integer values.
 */

template<class Eval>
inline Eval divUbPP(const Eval& t1, const Eval& t2)
{	return Round<Eval>::divUbPP(t1,t2);	}

template<class Eval>
inline Eval divLbPP(const Eval& t1, const Eval& t2)
{	return Round<Eval>::divLbPP(t1,t2);	}

template<class Eval>
inline Eval divUbPN(const Eval& t1, const Eval& t2)
{	return Round<Eval>::divUbPN(t1,t2); }

template<class Eval>
inline Eval divLbPN(const Eval& t1, const Eval& t2)
{	return Round<Eval>::divLbPN(t1,t2); }

template<class Eval>
inline Eval divUbNP(const Eval& t1, const Eval& t2)
{	return Round<Eval>::divUbNP(t1,t2); }

template<class Eval>
inline Eval divLbNP(const Eval& t1, const Eval& t2)
{	return Round<Eval>::divLbNP(t1,t2); }

template<class Eval>
inline Eval divUbNN(const Eval& t1, const Eval& t2)
{	return Round<Eval>::divUbNN(t1,t2); }

template<class Eval>
inline Eval divLbNN(const Eval& t1, const Eval& t2)
{	return Round<Eval>::divLbNN(t1,t2); }

/*
template<class Eval>
inline Eval divUbPN(const Eval& t1, const Eval& t2)
{	return -divLbPP(t1,-t2); }

template<class Eval>
inline Eval divUbNP(const Eval& t1, const Eval& t2)
{	return -divLbPP(-t1,t2); }

template<class Eval>
inline Eval divUbNN(const Eval& t1, const Eval& t2)
{	return divUbPP(-t1,-t2); }

template<class Eval>
inline Eval divLbPN(const Eval& t1, const Eval& t2)
{	return -divUbPP(t1,-t2); }

template<class Eval>
inline Eval divLbNP(const Eval& t1, const Eval& t2)
{	return -divUbPP(-t1,t2); }

template<class Eval>
inline Eval divLbNN(const Eval& t1, const Eval& t2)
{	return divLbPP(-t1,-t2); }
*/

// Integer division

inline int idivUb(const int& t1, const int& t2)
{
#ifdef CASPER_CHECK_OVERFLOW
	typedef typename GetHigherRank<int>::Type HT;
	HT r = static_cast<HT>(t1) / static_cast<HT>(t2);
	assert(r <= static_cast<HT>(limits<int>::posInf()));
	if (r < static_cast<HT>(limits<int>::negInf()))
	  return limits<int>::negInf();
	return r;
#else
	return t1/t2;
#endif
}

inline int idivLb(const int& t1, const int& t2)
{
#ifdef CASPER_CHECK_OVERFLOW
	typedef typename GetHigherRank<int>::Type HT;
	HT r = static_cast<HT>(t1) / static_cast<HT>(t2);
	assert(r >= static_cast<HT>(limits<int>::negInf()));
	if (r > static_cast<HT>(limits<int>::posInf()))
	  return limits<int>::posInf();
	return r;
#else
	return t1/t2;
#endif
}

template<class Eval>
Eval powUb(const Eval& t1, const Eval& t2)
{
	return Round<Eval>::powUb(t1,t2);
}

template<class Eval>
Eval powLb(const Eval& t1, const Eval& t2)
{
	return Round<Eval>::powLb(t1,t2);
}

template<class Eval>
Eval expUb(const Eval& t1)
{
	return Round<Eval>::expUb(t1);
}

template<class Eval>
Eval expLb(const Eval& t1)
{
	return Round<Eval>::expLb(t1);
}

template<class Eval>
Eval sqrUb(const Eval& t1)
{
	return mulUb(t1,t1);
}

template<class Eval>
Eval sqrLb(const Eval& t1)
{
	return mulLb(t1,t1);
}

template<class Eval>
Eval sqrtUb(const Eval& t1)
{
	return Round<Eval>::sqrtUb(t1);
}

template<class Eval>
Eval sqrtLb(const Eval& t1)
{
	return Round<Eval>::sqrtLb(t1);
}

template<class Eval>
Eval logUb(const Eval& t1)
{
	return Round<Eval>::logUb(t1);
}

template<class Eval>
Eval logLb(const Eval& t1)
{
	return Round<Eval>::logLb(t1);
}

template<class Eval>
Eval median(const Eval& t1,const Eval& t2)
{	return Round<Eval>::median(t1,t2);	}

};


};

#endif /*_H_CASPER_KERNEL_SAFEOP */
