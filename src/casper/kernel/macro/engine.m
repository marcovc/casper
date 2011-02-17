/**************************************************************************\
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

#ifndef _M_CASPER_KERNEL_MACRO_ENGINE
#define _M_CASPER_KERNEL_MACRO_ENGINE

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


#define AT_Int					Int
#define AT_Bool					Bool
#define AT_Float				Float
#define AT_Double				Double
#define AT_T					T
#define AT_T1					T1
#define AT_T2					T2
#define AT_T3					T3
#define AT_T4					T4
#define AT_T5					T5
#define AT_Op					Op
#define AT_DomVar(T,M)			DomVar< AT_ ## T,AT_ ## M >
#define AT_DomVarArray(T,D,M)	DomVarArray< AT_ ## T,D,AT_ ## M >
#define AT_DomVarArrayD(T,D,M)	DomVarArray< AT_ ## T,D,AT_ ## M >
#define AT_Array(T,D)			Array< AT_ ## T,D>
#define AT_ArrayD(T,D)			Array< AT_ ## T,D>
#define AT_MutVar(T)			MutVar< AT_ ## T >
#define AT_Rel0(M)				Rel0< AT_ ## M >
#define AT_Rel1(M1,M2)			Rel1< AT_ ## M1,AT_ ## M2 >
#define AT_Rel2(M1,M2,M3)		Rel2< AT_ ## M1,AT_ ## M2,AT_ ## M3 >
#define AT_Rel3(M1,M2,M3,M4)	Rel3< AT_ ## M1,AT_ ## M2,AT_ ## M3,AT_ ## M4 >
#define AT_Rel4(M1,M2,M3,M4,M5)	Rel4< AT_ ## M1,AT_ ## M2,AT_ ## M3,AT_ ## M4, \
									 AT_ ## M5 >
#define AT_ValExpr(M1)			ValExpr< AT_ ## M1 >
#define AT_BndExpr(M1)			BndExpr< AT_ ## M1 >
#define AT_BndView(M1,M2)		BndView< AT_ ## M1,AT_ ## M2>
#define AT_MutExpr(M1)			MutExpr< AT_ ## M1 >
#define AT_DomExpr(M1,M2)		DomExpr< AT_ ## M1,AT_ ## M2>
#define AT_Filter				Filter
#define AT_Goal					Goal

#define AD_Int					
#define AD_Bool					
#define AD_Float					
#define AD_Double					
#define AD_T					(class T)
#define AD_T1					(class T1)
#define AD_T2					(class T2)
#define AD_T3					(class T3)
#define AD_T4					(class T4)
#define AD_T5					(class T5)
#define AD_Op					(class Op)
#define AD_DomVar(T,M)			AD_ ## T AD_ ## M
#define AD_DomVarArray(T,D,M)	AD_ ## T (Int D) AD_ ## M
#define AD_DomVarArrayD(T,D,M)	AD_ ## T AD_ ## M
#define AD_Array(T,D)			AD_ ## T (Int D)
#define AD_ArrayD(T,D)			AD_ ## T
#define AD_MutVar(T)			AD_ ## T
#define AD_Rel0(M)				AD_ ## M
#define AD_Rel1(M1,M2)			AD_ ## M1 AD_ ## M2
#define AD_Rel2(M1,M2,M3)		AD_ ## M1 AD_ ## M2 AD_ ## M3
#define AD_Rel3(M1,M2,M3,M4)	AD_ ## M1 AD_ ## M2 AD_ ## M3 AD_ ## M4
#define AD_Rel4(M1,M2,M3,M4,M5)	AD_ ## M1 AD_ ## M2 AD_ ## M3 AD_ ## M4 AD_ ## M5
#define AD_ValExpr(M1)			AD_ ## M1
#define AD_BndExpr(M1)			AD_ ## M1
#define AD_BndView(M1,M2)		AD_ ## M1 AD_ ## M2
#define AD_MutExpr(M1)			AD_ ## M1
#define AD_DomExpr(M1,M2)		AD_ ## M1 AD_ ## M2
#define AD_Filter				
#define AD_Goal					

// --- RELATIONS (Compile time expressions) ---

#define REL_FROM_BINARY_OPERATORS(AT_LHT,AT_RHT) \
REL_FROM_REL_OPERATORS(AT_LHT,AT_RHT) \
REL_FROM_ARITH_OPERATORS(AT_LHT,AT_RHT) \
REL_FROM_LOGICAL_OPERATORS(AT_LHT,AT_RHT)

#define REL_FROM_REL_OPERATORS(AT_LHT,AT_RHT) \
\
/** Equal binary operator.  \ingroup EqualityRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Equal,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator==(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh)  \
{ return Rel2<Equal,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh); } \
\
/** Greater or equal binary operator.  \ingroup InequalityRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<GreaterEqual,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator>=(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh)  \
{ return Rel2<GreaterEqual,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh); } \
\
/** Less or equal binary operator.  \ingroup InequalityRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<LessEqual,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator<=(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{ return Rel2<LessEqual,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh); } \
\
/** Distinct binary operator.  \ingroup DisequalityRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Distinct,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator!=(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh)  \
{ return Rel2<Distinct,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh); } \
\
/** Greater than binary operator.  \ingroup InequalityRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Greater,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator>(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh)  \
{ return Rel2<Greater,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh); } \
\
/** Less than binary operator.  \ingroup InequalityRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Less,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator<(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh)  \
{ return Rel2<Less,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh); } \


#define REL_FROM_ARITH_OPERATORS(AT_LHT,AT_RHT) \
\
/** Addition binary operator.  \ingroup ArithRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Add,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator+(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{ return Rel2<Add,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh); } \
\
/** Subtraction binary operator.  \ingroup ArithRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Sub,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator-(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{ return Rel2<Sub,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh); } \
\
/** Multiplication binary operator.   \ingroup ArithRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) 	\
inline Rel2<Mul,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > 	\
operator*(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{ return Rel2<Mul,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh); } \
\
/** Division binary operator.   \ingroup ArithRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Div,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator/(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{ return Rel2<Div,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh); } \

#define REL_FROM_LOGICAL_OPERATORS(AT_LHT,AT_RHT) \
\
/** Logical AND binary operator.   \ingroup LogicalRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<And,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator&&(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{ return Rel2<And,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh); } \
\
/** Logical OR operator.  \ingroup LogicalRelations */ \
AT_DECL_2(AT_LHT,AT_RHT) \
inline Rel2<Or,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) > \
operator||(const AT_TYPE(AT_LHT)& lh, const AT_TYPE(AT_RHT)& rh) \
{ return Rel2<Or,AT_TYPE(AT_LHT),AT_TYPE(AT_RHT) >(lh,rh); }

#define REL_FROM_UNARY_OPERATORS(AT_RHT) \
\
/** Symmetric unary operator.  \ingroup ArithRelations */ \
AT_DECL(AT_RHT) \
inline Rel1<Sym,AT_TYPE(AT_RHT) > \
operator-(const AT_TYPE(AT_RHT)& rh) \
{ return Rel1<Sym,AT_TYPE(AT_RHT) >(rh);} \
\
/** Negation unary operator.  \ingroup LogicalRelations */ \
AT_DECL(AT_RHT) \
inline Rel1<Not,AT_TYPE(AT_RHT) > \
operator!(const AT_TYPE(AT_RHT)& rh) \
{ return Rel1<Not,AT_TYPE(AT_RHT) >(rh);} \
\
/** Absolute value unary operator.  \ingroup ArithRelations */ \
AT_DECL(AT_RHT) \
inline Rel1<Abs,AT_TYPE(AT_RHT) > \
abs(const AT_TYPE(AT_RHT)& rh) \
{ return Rel1<Abs,AT_TYPE(AT_RHT) >(rh);} \
\
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


#endif // _M_CASPER_KERNEL_MACRO_ENGINE
