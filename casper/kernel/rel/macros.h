/**************************************************************************\
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

#ifndef _H_CASPER_KERNEL_REL_MACROS
#define _H_CASPER_KERNEL_REL_MACROS

#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/comparison/greater.hpp> 


#define	BOOST_PP_SEQ_ENUM_0


#define NIL_1(X)
#define NIL_2(X1,X2)
#define AT_TYPE(M)				AT_ ## M
#define WITH_TEMPLATE(M)		template< BOOST_PP_SEQ_ENUM(AD_ ## M) >
#define WITH_TEMPLATE_2(M1,M2)	template< BOOST_PP_SEQ_ENUM(AD_ ## M1 AD_ ## M2) >

#define NOT_EMPTY_SEQ(S)		BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(S(dummy)),1)
#define AT_DECL(M)				BOOST_PP_IF(NOT_EMPTY_SEQ(AD_ ## M),\
								WITH_TEMPLATE,NIL_1)(M)
#define AT_DECL_2(M1,M2)		BOOST_PP_IF(NOT_EMPTY_SEQ(AD_##M1 AD_##M2),\
								WITH_TEMPLATE_2,NIL_2)(M1,M2)


#define AT_int					int
#define AT_bool					bool
#define AT_float				float
#define AT_double				double
#define AT_T					T
#define AT_T1					T1
#define AT_T2					T2
#define AT_T3					T3
#define AT_T4					T4
#define AT_T5					T5
#define AT_Op					Op

#define AD_int					
#define AD_bool					
#define AD_float					
#define AD_double					
#define AD_T					(class T)
#define AD_T1					(class T1)
#define AD_T2					(class T2)
#define AD_T3					(class T3)
#define AD_T4					(class T4)
#define AD_T5					(class T5)
#define AD_Op					(class Op)


// --- RELATIONS (Compile time expressions) ---

#define REL_FROM_BINARY_NONBOOL(AT_LHT,AT_RHT) \
REL_FROM_COMPARISON_OPERATORS(AT_LHT,AT_RHT) \
REL_FROM_ARITH_OPERATORS(AT_LHT,AT_RHT)

#define REL_FROM_BINARY_OPERATORS(AT_LHT,AT_RHT) \
REL_FROM_COMPARISON_OPERATORS(AT_LHT,AT_RHT) \
REL_FROM_ARITH_OPERATORS(AT_LHT,AT_RHT) \
REL_FROM_LOGICAL_OPERATORS(AT_LHT,AT_RHT)

#define REL_FROM_COMPARISON_OPERATORS(AT_LHT,AT_RHT) \
\
/** Equal binary operator.  \ingroup EqualityRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Equal,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator==(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh)  \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_LHT)>::Type,\
								   typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type>::value,\
				 "operands to '==' operator must have equal evaluation types");\
	return Rel2<Equal,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh);\
} \
\
/** Greater or equal binary operator.  \ingroup InequalityRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<GreaterEqual,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator>=(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh)  \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_LHT)>::Type,\
								   typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type>::value,\
				  "operands to '>=' operator must have equal evaluation types");\
 	return Rel2<GreaterEqual,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh);\
} \
\
/** Less or equal binary operator.  \ingroup InequalityRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<LessEqual,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator<=(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_LHT)>::Type,\
								   typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type>::value,\
				  "operands to '<=' operator must have equal evaluation types");\
	return Rel2<LessEqual,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh);\
} \
\
/** Distinct binary operator.  \ingroup DisequalityRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Distinct,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator!=(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh)  \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_LHT)>::Type,\
								   typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type>::value,\
				  "operands to '!=' operator must have equal evaluation types");\
	return Rel2<Distinct,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh);\
} \
\
/** Greater than binary operator.  \ingroup InequalityRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Greater,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator>(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh)  \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_LHT)>::Type,\
								   typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type>::value,\
				  "operands to '>' operator must have equal evaluation types");\
	return Rel2<Greater,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh);\
} \
\
/** Less than binary operator.  \ingroup InequalityRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Less,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator<(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh)  \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_LHT)>::Type,\
								   typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type>::value,\
				  "operands to '<' operator must have equal evaluation types");\
 	return Rel2<Less,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh);\
}


#define REL_FROM_ARITH_OPERATORS(AT_LHT,AT_RHT) \
\
/** Addition binary operator.  \ingroup ArithRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Add,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator+(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_LHT)>::Type,\
								   typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type>::value,\
				  "operands to '+' operator must have equal evaluation types");\
 	return Rel2<Add,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh);\
} \
\
/** Subtraction binary operator.  \ingroup ArithRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Sub,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator-(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_LHT)>::Type,\
								   typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type>::value,\
				  "operands to '-' operator must have equal evaluation types");\
 	return Rel2<Sub,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh);\
} \
\
/** Multiplication binary operator.   \ingroup ArithRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) 	\
inline Rel2<Mul,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > 	\
operator*(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_LHT)>::Type,\
								   typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type>::value,\
				  "operands to '*' operator must have equal evaluation types");\
 	return Rel2<Mul,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh);\
} \
\
/** Division binary operator.   \ingroup ArithRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Div,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator/(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_LHT)>::Type,\
								   typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type>::value,\
				  "operands to '/' operator must have equal evaluation types");\
 	return Rel2<Div,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh);\
 } \

#define REL_FROM_LOGICAL_OPERATORS(AT_LHT,AT_RHT) \
\
/** Logical AND binary operator.   \ingroup LogicalRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<And,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator&&(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_LHT)>::Type,bool>::value,\
				  "first operand to '&&' (and) operator must have boolean evaluation type");\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type,bool>::value,\
				  "second operand to '&&' (and) operator must have boolean evaluation type");\
	return Rel2<And,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh);\
} \
\
/** Logical OR operator.  \ingroup LogicalRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Or,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator||(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_LHT)>::Type,bool>::value,\
				  "first operand to '||' (or) operator must have boolean evaluation type");\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type,bool>::value,\
				  "second operand to '||' (or) operator must have boolean evaluation type");\
 	return Rel2<Or,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh);\
 }

#define REL_FROM_SYM_OPERATOR(AT_RHT) \
\
/** Symmetric unary operator.  \ingroup ArithRelations */ \
AT_DECL(AT_RHT) \
inline Rel1<Sym,AT_TYPE(AT_RHT) > \
operator-(const AT_TYPE(AT_RHT)& rh) \
{ return Rel1<Sym,AT_TYPE(AT_RHT) >(rh);}

#define REL_FROM_NOT_OPERATOR(AT_RHT) \
\
/** Negation unary operator.  \ingroup LogicalRelations */ \
AT_DECL(AT_RHT) \
inline Rel1<Not,AT_TYPE(AT_RHT) > \
operator!(const AT_TYPE(AT_RHT)& rh) \
{\
	static_assert(std::is_same<typename Traits::GetEval<AT_TYPE(AT_RHT)>::Type,bool>::value,\
				  "operand to '!' (not) operator must have boolean evaluation type");\
 	return Rel1<Not,AT_TYPE(AT_RHT) >(rh);\
}

#define REL_FROM_ABS_OPERATOR(AT_RHT) \
\
/** Absolute value unary operator.  \ingroup ArithRelations */ \
AT_DECL(AT_RHT) \
inline Rel1<Abs,AT_TYPE(AT_RHT) > \
abs(const AT_TYPE(AT_RHT)& rh) \
{ return Rel1<Abs,AT_TYPE(AT_RHT) >(rh);} 
 
#define REL_FROM_UNARY_SCALAR_OPERATORS(AT_RHT) \
		REL_FROM_SYM_OPERATOR(AT_RHT)\
		REL_FROM_NOT_OPERATOR(AT_RHT)\
		REL_FROM_ABS_OPERATOR(AT_RHT)

#define REL_FROM_UNARY_SET_OPERATORS(AT_RHT) \
/** Min value (from a sequence) unary operator.  \ingroup ArithRelations */ \
AT_DECL(AT_RHT) \
inline Rel1<Min,AT_TYPE(AT_RHT) > \
min(const AT_TYPE(AT_RHT)& rh) \
{ return Rel1<Min,AT_TYPE(AT_RHT) >(rh);} \
\
/** Max value (from a sequence) unary operator. \ingroup ArithRelations */ \
AT_DECL(AT_RHT) \
inline Rel1<Max,AT_TYPE(AT_RHT) > \
max(const AT_TYPE(AT_RHT)& rh) \
{ return Rel1<Max,AT_TYPE(AT_RHT) >(rh);}

#define REL_FROM_UNARY_OPERATORS(T) REL_FROM_UNARY_SCALAR_OPERATORS(T) REL_FROM_UNARY_SET_OPERATORS(T)

#endif // _H_CASPER_KERNEL_REL_MACROS
