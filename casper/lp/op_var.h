/************************************************************************** *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
*
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
/************************************************************************** *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *
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
// --- RELATIONS (Compile time expressions) ---
namespace Casper {
/** Addition binary operator.  \ingroup ArithRelations */ template< class T > inline Rel2<Add,LP::Var,T > operator+(const LP::Var& lh, const T& rh) { static_assert(std::is_same<typename Traits::GetEval<LP::Var>::Type, typename Traits::GetEval<T>::Type>::value, "operands to '+' operator must have equal evaluation types"); return Rel2<Add,LP::Var,T >(lh,rh);} /** Subtraction binary operator.  \ingroup ArithRelations */ template< class T > inline Rel2<Sub,LP::Var,T > operator-(const LP::Var& lh, const T& rh) { static_assert(std::is_same<typename Traits::GetEval<LP::Var>::Type, typename Traits::GetEval<T>::Type>::value, "operands to '-' operator must have equal evaluation types"); return Rel2<Sub,LP::Var,T >(lh,rh);} /** Multiplication binary operator.   \ingroup ArithRelations */ template< class T > inline Rel2<Mul,LP::Var,T > operator*(const LP::Var& lh, const T& rh) { static_assert(std::is_same<typename Traits::GetEval<LP::Var>::Type, typename Traits::GetEval<T>::Type>::value, "operands to '*' operator must have equal evaluation types"); return Rel2<Mul,LP::Var,T >(lh,rh);} /** Division binary operator.   \ingroup ArithRelations */ template< class T > inline Rel2<Div,LP::Var,T > operator/(const LP::Var& lh, const T& rh) { static_assert(std::is_same<typename Traits::GetEval<LP::Var>::Type, typename Traits::GetEval<T>::Type>::value, "operands to '/' operator must have equal evaluation types"); return Rel2<Div,LP::Var,T >(lh,rh); }
/** Addition binary operator.  \ingroup ArithRelations */ inline Rel2<Add,double,LP::Var > operator+(const double& lh, const LP::Var& rh) { static_assert(std::is_same<typename Traits::GetEval<double>::Type, typename Traits::GetEval<LP::Var>::Type>::value, "operands to '+' operator must have equal evaluation types"); return Rel2<Add,double,LP::Var >(lh,rh);} /** Subtraction binary operator.  \ingroup ArithRelations */ inline Rel2<Sub,double,LP::Var > operator-(const double& lh, const LP::Var& rh) { static_assert(std::is_same<typename Traits::GetEval<double>::Type, typename Traits::GetEval<LP::Var>::Type>::value, "operands to '-' operator must have equal evaluation types"); return Rel2<Sub,double,LP::Var >(lh,rh);} /** Multiplication binary operator.   \ingroup ArithRelations */ inline Rel2<Mul,double,LP::Var > operator*(const double& lh, const LP::Var& rh) { static_assert(std::is_same<typename Traits::GetEval<double>::Type, typename Traits::GetEval<LP::Var>::Type>::value, "operands to '*' operator must have equal evaluation types"); return Rel2<Mul,double,LP::Var >(lh,rh);} /** Division binary operator.   \ingroup ArithRelations */ inline Rel2<Div,double,LP::Var > operator/(const double& lh, const LP::Var& rh) { static_assert(std::is_same<typename Traits::GetEval<double>::Type, typename Traits::GetEval<LP::Var>::Type>::value, "operands to '/' operator must have equal evaluation types"); return Rel2<Div,double,LP::Var >(lh,rh); }
/** Equal binary operator.  \ingroup EqualityRelations */ template< class T > inline Rel2<Equal,LP::Var,T > operator==(const LP::Var& lh, const T& rh) { static_assert(std::is_same<typename Traits::GetEval<LP::Var>::Type, typename Traits::GetEval<T>::Type>::value, "operands to '==' operator must have equal evaluation types"); return Rel2<Equal,LP::Var,T >(lh,rh);} /** Greater or equal binary operator.  \ingroup InequalityRelations */ template< class T > inline Rel2<GreaterEqual,LP::Var,T > operator>=(const LP::Var& lh, const T& rh) { static_assert(std::is_same<typename Traits::GetEval<LP::Var>::Type, typename Traits::GetEval<T>::Type>::value, "operands to '>=' operator must have equal evaluation types"); return Rel2<GreaterEqual,LP::Var,T >(lh,rh);} /** Less or equal binary operator.  \ingroup InequalityRelations */ template< class T > inline Rel2<LessEqual,LP::Var,T > operator<=(const LP::Var& lh, const T& rh) { static_assert(std::is_same<typename Traits::GetEval<LP::Var>::Type, typename Traits::GetEval<T>::Type>::value, "operands to '<=' operator must have equal evaluation types"); return Rel2<LessEqual,LP::Var,T >(lh,rh);} /** Distinct binary operator.  \ingroup DisequalityRelations */ template< class T > inline Rel2<Distinct,LP::Var,T > operator!=(const LP::Var& lh, const T& rh) { static_assert(std::is_same<typename Traits::GetEval<LP::Var>::Type, typename Traits::GetEval<T>::Type>::value, "operands to '!=' operator must have equal evaluation types"); return Rel2<Distinct,LP::Var,T >(lh,rh);} /** Greater than binary operator.  \ingroup InequalityRelations */ template< class T > inline Rel2<Greater,LP::Var,T > operator>(const LP::Var& lh, const T& rh) { static_assert(std::is_same<typename Traits::GetEval<LP::Var>::Type, typename Traits::GetEval<T>::Type>::value, "operands to '>' operator must have equal evaluation types"); return Rel2<Greater,LP::Var,T >(lh,rh);} /** Less than binary operator.  \ingroup InequalityRelations */ template< class T > inline Rel2<Less,LP::Var,T > operator<(const LP::Var& lh, const T& rh) { static_assert(std::is_same<typename Traits::GetEval<LP::Var>::Type, typename Traits::GetEval<T>::Type>::value, "operands to '<' operator must have equal evaluation types"); return Rel2<Less,LP::Var,T >(lh,rh);}
/** Equal binary operator.  \ingroup EqualityRelations */ inline Rel2<Equal,double,LP::Var > operator==(const double& lh, const LP::Var& rh) { static_assert(std::is_same<typename Traits::GetEval<double>::Type, typename Traits::GetEval<LP::Var>::Type>::value, "operands to '==' operator must have equal evaluation types"); return Rel2<Equal,double,LP::Var >(lh,rh);} /** Greater or equal binary operator.  \ingroup InequalityRelations */ inline Rel2<GreaterEqual,double,LP::Var > operator>=(const double& lh, const LP::Var& rh) { static_assert(std::is_same<typename Traits::GetEval<double>::Type, typename Traits::GetEval<LP::Var>::Type>::value, "operands to '>=' operator must have equal evaluation types"); return Rel2<GreaterEqual,double,LP::Var >(lh,rh);} /** Less or equal binary operator.  \ingroup InequalityRelations */ inline Rel2<LessEqual,double,LP::Var > operator<=(const double& lh, const LP::Var& rh) { static_assert(std::is_same<typename Traits::GetEval<double>::Type, typename Traits::GetEval<LP::Var>::Type>::value, "operands to '<=' operator must have equal evaluation types"); return Rel2<LessEqual,double,LP::Var >(lh,rh);} /** Distinct binary operator.  \ingroup DisequalityRelations */ inline Rel2<Distinct,double,LP::Var > operator!=(const double& lh, const LP::Var& rh) { static_assert(std::is_same<typename Traits::GetEval<double>::Type, typename Traits::GetEval<LP::Var>::Type>::value, "operands to '!=' operator must have equal evaluation types"); return Rel2<Distinct,double,LP::Var >(lh,rh);} /** Greater than binary operator.  \ingroup InequalityRelations */ inline Rel2<Greater,double,LP::Var > operator>(const double& lh, const LP::Var& rh) { static_assert(std::is_same<typename Traits::GetEval<double>::Type, typename Traits::GetEval<LP::Var>::Type>::value, "operands to '>' operator must have equal evaluation types"); return Rel2<Greater,double,LP::Var >(lh,rh);} /** Less than binary operator.  \ingroup InequalityRelations */ inline Rel2<Less,double,LP::Var > operator<(const double& lh, const LP::Var& rh) { static_assert(std::is_same<typename Traits::GetEval<double>::Type, typename Traits::GetEval<LP::Var>::Type>::value, "operands to '<' operator must have equal evaluation types"); return Rel2<Less,double,LP::Var >(lh,rh);}
/** Symmetric unary operator.  \ingroup ArithRelations */ inline Rel1<Sym,LP::Var > operator-(const LP::Var& rh) { return Rel1<Sym,LP::Var >(rh);}
}
