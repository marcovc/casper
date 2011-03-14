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

#ifndef _H_CASPER_KERNEL_SAFEOP
#define _H_CASPER_KERNEL_SAFEOP

#include <boost/numeric/interval/hw_rounding.hpp>
#include <boost/numeric/interval/rounded_arith.hpp>
#include <boost/numeric/interval/rounded_transc.hpp>

#include <boost/math/special_functions/next.hpp>

#ifndef CASPER_UNSAFE_ROUNDING

//#include <gmp.h>
#include <mpfr.h>

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
  double name##_down(double x) { return invoke_mpfr(x, mpfr_##name, GMP_RNDD); } \
  double name##_up  (double x) { return invoke_mpfr(x, mpfr_##name, GMP_RNDU); }
# define GENR_FUNC_2(name) \
  double name##_down(double x,double y) { return invoke_mpfr(x,y, mpfr_##name, GMP_RNDD); } \
  double name##_up  (double x,double y) { return invoke_mpfr(x,y, mpfr_##name, GMP_RNDU); }

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
struct mpfr_rounded_transc:
  boost::numeric::interval_lib::rounded_arith_std<T>
	//mpfr_rounded_std<T>
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
  double name##_down(double x) { return invoke_mpfr(x, mpfr_##name, GMP_RNDD); } \
  double name##_up  (double x) { return invoke_mpfr(x, mpfr_##name, GMP_RNDU); }
# define GENR_FUNC2(name) \
  double name##_down(double x,double y) { return invoke_mpfr(x,y, mpfr_##name, GMP_RNDD); } \
  double name##_up  (double x,double y) { return invoke_mpfr(x,y, mpfr_##name, GMP_RNDU); }
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
};

// Special rounding policy for handling integers, since its missing from
// boost::interval library (nothing needs rounding, except division which is
// always rounded in the non-conservative way)
template<class T, class Rounding = boost::numeric::interval_lib::rounding_control<T> >
struct rounded_arith_integer : Rounding
{
# define BOOST_DN(EXPR) this->downward();   return this->force_rounding(EXPR)
# define BOOST_NR(EXPR) this->to_nearest(); return this->force_rounding(EXPR)
# define BOOST_UP(EXPR) this->upward();     return this->force_rounding(EXPR)
  void init() { }
  template<class U> T conv_down(U const &v)
  {	 return static_cast<T>(std::ceil(v)); }
  template<class U> T conv_up  (U const &v)
  {	return static_cast<T>(std::floor(v)); }
  T conv_down(bool const &v)
  {	 return static_cast<T>(std::ceil(static_cast<double>(v))); }
  T conv_up  (bool const &v)
  {	return static_cast<T>(std::floor(static_cast<double>(v))); }
  T conv_down(int const &v)
  {	 return static_cast<T>(std::ceil(static_cast<double>(v))); }
  T conv_up  (int const &v)
  {	return static_cast<T>(std::floor(static_cast<double>(v))); }

  T add_down(const T& x, const T& y) { return x + y; }
  T sub_down(const T& x, const T& y) { return x - y; }
  T mul_down(const T& x, const T& y) { return x * y; }
  T div_down(const T& x, const T& y)
  { return conv_down(static_cast<double>(x) / y); }
  T add_up  (const T& x, const T& y) { return x + y; }
  T sub_up  (const T& x, const T& y) { return x - y; }
  T mul_up  (const T& x, const T& y) { return x * y; }
  T div_up  (const T& x, const T& y)
  { return conv_up(static_cast<double>(x) / y); }
  T median(const T& x, const T& y) { BOOST_NR((x + y) / 2); }
  T sqrt_down(const T& x)
  { BOOST_NUMERIC_INTERVAL_using_math(sqrt); BOOST_UP(sqrt(x)); }
  T sqrt_up  (const T& x)
  { BOOST_NUMERIC_INTERVAL_using_math(sqrt); BOOST_DN(sqrt(x)); }
  T int_down(const T& x) { return x; }
  T int_up  (const T& x) { return x; }
  T pow_up  (const T& x, const T& y) { return std::pow(x,y); }
  T pow_down  (const T& x, const T& y) { return std::pow(x,y); }
# undef BOOST_DN
# undef BOOST_NR
# undef BOOST_UP
};

};
};
#endif


#ifdef CASPER_UNSAFE_ROUNDING

#include <math.h>

namespace Casper {
namespace Util {


// Transcendental functions need special rounding class
template<class T>
struct no_rounding
{
private:

public:
# define GENR_FUNC(name) \
  T name##_down(T x) { return ::name(x); } \
  T name##_up  (T x) { return ::name(x); }

# define GENR_FUNC2(name,res) \
  T name##_down(T x,T y) { return res; } \
  T name##_up  (T x,T y) { return res; }

  template<class U> T conv_down(U const &v)
  { /*assert(v != std::numeric_limits<U>::infinity() &&
  		   -v != std::numeric_limits<U>::infinity());*/
  		   return static_cast<T>(::ceil(static_cast<double>(v))); }
  template<class U> T conv_up  (U const &v)
  { /*assert(v != std::numeric_limits<U>::infinity() &&
  		   -v != std::numeric_limits<U>::infinity());*/
  		   return static_cast<T>(::floor(static_cast<double>(v))); }

  GENR_FUNC2(add,x+y);
  GENR_FUNC2(sub,x-y);
  GENR_FUNC2(mul,x*y);
  GENR_FUNC2(div,x/y);
  GENR_FUNC(sqrt);

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
  GENR_FUNC(pow)

  T median(const T& x, const T& y) { return (x + y) / 2; }

  #undef GENR_FUNC
  #undef GENR_FUNC2
};


template<class T>
//struct Round : boost::numeric::interval_lib::rounded_math<T>
struct Round : Util::no_rounding<T>
{};
};
};

#else

namespace Casper {
namespace Util {

template<class T>
//struct Round : boost::numeric::interval_lib::rounded_math<T>
struct Round :
	boost::numeric::interval_lib::save_state<
		mpfr_rounded_transc<T> >
{};


template<>
struct Round<int> :
	boost::numeric::interval_lib::save_state_nothing<rounded_arith_integer<int> >
{};

};
};

#endif



namespace Casper {
namespace Util {


template<class T>
Round<T> rounder()
{	return	Round<T>();	}

// conversion

template<class To,class From>
struct Convert
{
	static To down(const From& from)
	{	return rounder<To>().conv_down(from); }
	static To up(const From& from)
	{	return rounder<To>().conv_up(from); }
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
	{	return from > 0 ; }
	static bool up(const int& from)
	{	return from > 0; }
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

template<class T>
T succ(const T& v)
{	return boost::math::float_next(v); 	}

template<class T>
T pred(const T& v)
{	return boost::math::float_prior(v); }

template<class T>
bool isCanonical(const T& min, const T& max)
{	return succ(min)==max;	}

// arithmetic

template<class Eval>
Eval addUb(const Eval& t1, const Eval& t2)
{
	return rounder<Eval>().add_up(t1,t2);
}

template<class Eval>
Eval addLb(const Eval& t1, const Eval& t2)
{
	return rounder<Eval>().add_down(t1,t2);
}

template<class Eval>
Eval subUb(const Eval& t1, const Eval& t2)
{
	return rounder<Eval>().sub_up(t1,t2);
}

template<class Eval>
Eval subLb(const Eval& t1, const Eval& t2)
{
	return rounder<Eval>().sub_down(t1,t2);
}

template<class Eval>
Eval mulUb(const Eval& t1, const Eval& t2)
{
	return rounder<Eval>().mul_up(t1,t2);
}

template<class Eval>
Eval mulLb(const Eval& t1, const Eval& t2)
{
	return rounder<Eval>().mul_down(t1,t2);
}

template<class Eval>
Eval divUb(const Eval& t1, const Eval& t2)
{
	return rounder<Eval>().div_up(t1,t2);
}

template<class Eval>
Eval divLb(const Eval& t1, const Eval& t2)
{
	return rounder<Eval>().div_down(t1,t2);
}

inline int idivUb(const int& t1, const int& t2)
{
	return t1/t2;
}

inline int idivLb(const int& t1, const int& t2)
{
	return t1/t2;
}

template<class Eval>
Eval powUb(const Eval& t1, const Eval& t2)
{
	return rounder<Eval>().pow_up(t1,t2);
}

template<class Eval>
Eval powLb(const Eval& t1, const Eval& t2)
{
	return rounder<Eval>().pow_down(t1,t2);
}

template<class Eval>
Eval expUb(const Eval& t1)
{
	return rounder<Eval>().exp_up(t1);
}

template<class Eval>
Eval expLb(const Eval& t1)
{
	return rounder<Eval>().exp_down(t1);
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
	return rounder<Eval>().sqrt_up(t1);
}

template<class Eval>
Eval sqrtLb(const Eval& t1)
{
	return rounder<Eval>().sqrt_down(t1);
}

template<class Eval>
Eval logUb(const Eval& t1)
{
	return rounder<Eval>().log_up(t1);
}

template<class Eval>
Eval logLb(const Eval& t1)
{
	return rounder<Eval>().log_down(t1);
}

template<class Eval>
Eval median(const Eval& t1,const Eval& t2)
{	return rounder<Eval>().median(t1,t2);	}

};


};

#endif /*_H_CASPER_KERNEL_SAFEOP */
