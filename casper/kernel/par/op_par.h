/************************************************************************** *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
*
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
/************************************************************************** *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
*
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
// --- RELATIONS (Compile time expressions) ---
namespace Casper {
/** Symmetric unary operator.  \ingroup ArithRelations */ template< class T1 > inline Rel1<Sym,Par< T1 > > operator-(const Par< T1 >& rh) { return Rel1<Sym,Par< T1 > >(rh);} /** Negation unary operator.  \ingroup LogicalRelations */ template< class T1 > inline Rel1<Not,Par< T1 > > operator!(const Par< T1 >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type,bool>::value, "operand to '!' (not) operator must have boolean evaluation type"); return Rel1<Not,Par< T1 > >(rh);} /** Absolute value unary operator.  \ingroup ArithRelations */ template< class T1 > inline Rel1<Abs,Par< T1 > > abs(const Par< T1 >& rh) { return Rel1<Abs,Par< T1 > >(rh);} /** Min value (from a sequence) unary operator.  \ingroup ArithRelations */ template< class T1 > inline Rel1<Min,Par< T1 > > min(const Par< T1 >& rh) { return Rel1<Min,Par< T1 > >(rh);} /** Max value (from a sequence) unary operator. \ingroup ArithRelations */ template< class T1 > inline Rel1<Max,Par< T1 > > max(const Par< T1 >& rh) { return Rel1<Max,Par< T1 > >(rh);}
/** Equal binary operator.  \ingroup EqualityRelations */ template< class T1, class T > inline Rel2<Equal,Par< T1 >,T > operator==(const Par< T1 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '==' operator must have equal evaluation types"); return Rel2<Equal,Par< T1 >,T >(lh,rh);} /** Greater or equal binary operator.  \ingroup InequalityRelations */ template< class T1, class T > inline Rel2<GreaterEqual,Par< T1 >,T > operator>=(const Par< T1 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '>=' operator must have equal evaluation types"); return Rel2<GreaterEqual,Par< T1 >,T >(lh,rh);} /** Less or equal binary operator.  \ingroup InequalityRelations */ template< class T1, class T > inline Rel2<LessEqual,Par< T1 >,T > operator<=(const Par< T1 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '<=' operator must have equal evaluation types"); return Rel2<LessEqual,Par< T1 >,T >(lh,rh);} /** Distinct binary operator.  \ingroup DisequalityRelations */ template< class T1, class T > inline Rel2<Distinct,Par< T1 >,T > operator!=(const Par< T1 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '!=' operator must have equal evaluation types"); return Rel2<Distinct,Par< T1 >,T >(lh,rh);} /** Greater than binary operator.  \ingroup InequalityRelations */ template< class T1, class T > inline Rel2<Greater,Par< T1 >,T > operator>(const Par< T1 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '>' operator must have equal evaluation types"); return Rel2<Greater,Par< T1 >,T >(lh,rh);} /** Less than binary operator.  \ingroup InequalityRelations */ template< class T1, class T > inline Rel2<Less,Par< T1 >,T > operator<(const Par< T1 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '<' operator must have equal evaluation types"); return Rel2<Less,Par< T1 >,T >(lh,rh);} /** Addition binary operator.  \ingroup ArithRelations */ template< class T1, class T > inline Rel2<Add,Par< T1 >,T > operator+(const Par< T1 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '+' operator must have equal evaluation types"); return Rel2<Add,Par< T1 >,T >(lh,rh);} /** Subtraction binary operator.  \ingroup ArithRelations */ template< class T1, class T > inline Rel2<Sub,Par< T1 >,T > operator-(const Par< T1 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '-' operator must have equal evaluation types"); return Rel2<Sub,Par< T1 >,T >(lh,rh);} /** Multiplication binary operator.   \ingroup ArithRelations */ template< class T1, class T > inline Rel2<Mul,Par< T1 >,T > operator*(const Par< T1 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '*' operator must have equal evaluation types"); return Rel2<Mul,Par< T1 >,T >(lh,rh);} /** Division binary operator.   \ingroup ArithRelations */ template< class T1, class T > inline Rel2<Div,Par< T1 >,T > operator/(const Par< T1 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '/' operator must have equal evaluation types"); return Rel2<Div,Par< T1 >,T >(lh,rh); } /** Logical AND binary operator.   \ingroup LogicalRelations */ template< class T1, class T > inline Rel2<And,Par< T1 >,T > operator&&(const Par< T1 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type,bool>::value, "first operand to '&&' (and) operator must have boolean evaluation type"); static_assert(Util::IsSameType<typename Traits::GetEval<T>::Type,bool>::value, "second operand to '&&' (and) operator must have boolean evaluation type"); return Rel2<And,Par< T1 >,T >(lh,rh);} /** Logical OR operator.  \ingroup LogicalRelations */ template< class T1, class T > inline Rel2<Or,Par< T1 >,T > operator||(const Par< T1 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Par< T1 > >::Type,bool>::value, "first operand to '||' (or) operator must have boolean evaluation type"); static_assert(Util::IsSameType<typename Traits::GetEval<T>::Type,bool>::value, "second operand to '||' (or) operator must have boolean evaluation type"); return Rel2<Or,Par< T1 >,T >(lh,rh); }
/** Addition binary operator.  \ingroup ArithRelations */ inline Rel2<Add,int,Par< int > > operator+(const int& lh, const Par< int >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<int>::Type, typename Traits::GetEval<Par< int > >::Type>::value, "operands to '+' operator must have equal evaluation types"); return Rel2<Add,int,Par< int > >(lh,rh);} /** Subtraction binary operator.  \ingroup ArithRelations */ inline Rel2<Sub,int,Par< int > > operator-(const int& lh, const Par< int >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<int>::Type, typename Traits::GetEval<Par< int > >::Type>::value, "operands to '-' operator must have equal evaluation types"); return Rel2<Sub,int,Par< int > >(lh,rh);} /** Multiplication binary operator.   \ingroup ArithRelations */ inline Rel2<Mul,int,Par< int > > operator*(const int& lh, const Par< int >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<int>::Type, typename Traits::GetEval<Par< int > >::Type>::value, "operands to '*' operator must have equal evaluation types"); return Rel2<Mul,int,Par< int > >(lh,rh);} /** Division binary operator.   \ingroup ArithRelations */ inline Rel2<Div,int,Par< int > > operator/(const int& lh, const Par< int >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<int>::Type, typename Traits::GetEval<Par< int > >::Type>::value, "operands to '/' operator must have equal evaluation types"); return Rel2<Div,int,Par< int > >(lh,rh); }
/** Equal binary operator.  \ingroup EqualityRelations */ inline Rel2<Equal,int,Par< int > > operator==(const int& lh, const Par< int >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<int>::Type, typename Traits::GetEval<Par< int > >::Type>::value, "operands to '==' operator must have equal evaluation types"); return Rel2<Equal,int,Par< int > >(lh,rh);} /** Greater or equal binary operator.  \ingroup InequalityRelations */ inline Rel2<GreaterEqual,int,Par< int > > operator>=(const int& lh, const Par< int >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<int>::Type, typename Traits::GetEval<Par< int > >::Type>::value, "operands to '>=' operator must have equal evaluation types"); return Rel2<GreaterEqual,int,Par< int > >(lh,rh);} /** Less or equal binary operator.  \ingroup InequalityRelations */ inline Rel2<LessEqual,int,Par< int > > operator<=(const int& lh, const Par< int >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<int>::Type, typename Traits::GetEval<Par< int > >::Type>::value, "operands to '<=' operator must have equal evaluation types"); return Rel2<LessEqual,int,Par< int > >(lh,rh);} /** Distinct binary operator.  \ingroup DisequalityRelations */ inline Rel2<Distinct,int,Par< int > > operator!=(const int& lh, const Par< int >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<int>::Type, typename Traits::GetEval<Par< int > >::Type>::value, "operands to '!=' operator must have equal evaluation types"); return Rel2<Distinct,int,Par< int > >(lh,rh);} /** Greater than binary operator.  \ingroup InequalityRelations */ inline Rel2<Greater,int,Par< int > > operator>(const int& lh, const Par< int >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<int>::Type, typename Traits::GetEval<Par< int > >::Type>::value, "operands to '>' operator must have equal evaluation types"); return Rel2<Greater,int,Par< int > >(lh,rh);} /** Less than binary operator.  \ingroup InequalityRelations */ inline Rel2<Less,int,Par< int > > operator<(const int& lh, const Par< int >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<int>::Type, typename Traits::GetEval<Par< int > >::Type>::value, "operands to '<' operator must have equal evaluation types"); return Rel2<Less,int,Par< int > >(lh,rh);}
/** Addition binary operator.  \ingroup ArithRelations */ inline Rel2<Add,double,Par< double > > operator+(const double& lh, const Par< double >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<double>::Type, typename Traits::GetEval<Par< double > >::Type>::value, "operands to '+' operator must have equal evaluation types"); return Rel2<Add,double,Par< double > >(lh,rh);} /** Subtraction binary operator.  \ingroup ArithRelations */ inline Rel2<Sub,double,Par< double > > operator-(const double& lh, const Par< double >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<double>::Type, typename Traits::GetEval<Par< double > >::Type>::value, "operands to '-' operator must have equal evaluation types"); return Rel2<Sub,double,Par< double > >(lh,rh);} /** Multiplication binary operator.   \ingroup ArithRelations */ inline Rel2<Mul,double,Par< double > > operator*(const double& lh, const Par< double >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<double>::Type, typename Traits::GetEval<Par< double > >::Type>::value, "operands to '*' operator must have equal evaluation types"); return Rel2<Mul,double,Par< double > >(lh,rh);} /** Division binary operator.   \ingroup ArithRelations */ inline Rel2<Div,double,Par< double > > operator/(const double& lh, const Par< double >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<double>::Type, typename Traits::GetEval<Par< double > >::Type>::value, "operands to '/' operator must have equal evaluation types"); return Rel2<Div,double,Par< double > >(lh,rh); }
/** Equal binary operator.  \ingroup EqualityRelations */ inline Rel2<Equal,double,Par< double > > operator==(const double& lh, const Par< double >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<double>::Type, typename Traits::GetEval<Par< double > >::Type>::value, "operands to '==' operator must have equal evaluation types"); return Rel2<Equal,double,Par< double > >(lh,rh);} /** Greater or equal binary operator.  \ingroup InequalityRelations */ inline Rel2<GreaterEqual,double,Par< double > > operator>=(const double& lh, const Par< double >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<double>::Type, typename Traits::GetEval<Par< double > >::Type>::value, "operands to '>=' operator must have equal evaluation types"); return Rel2<GreaterEqual,double,Par< double > >(lh,rh);} /** Less or equal binary operator.  \ingroup InequalityRelations */ inline Rel2<LessEqual,double,Par< double > > operator<=(const double& lh, const Par< double >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<double>::Type, typename Traits::GetEval<Par< double > >::Type>::value, "operands to '<=' operator must have equal evaluation types"); return Rel2<LessEqual,double,Par< double > >(lh,rh);} /** Distinct binary operator.  \ingroup DisequalityRelations */ inline Rel2<Distinct,double,Par< double > > operator!=(const double& lh, const Par< double >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<double>::Type, typename Traits::GetEval<Par< double > >::Type>::value, "operands to '!=' operator must have equal evaluation types"); return Rel2<Distinct,double,Par< double > >(lh,rh);} /** Greater than binary operator.  \ingroup InequalityRelations */ inline Rel2<Greater,double,Par< double > > operator>(const double& lh, const Par< double >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<double>::Type, typename Traits::GetEval<Par< double > >::Type>::value, "operands to '>' operator must have equal evaluation types"); return Rel2<Greater,double,Par< double > >(lh,rh);} /** Less than binary operator.  \ingroup InequalityRelations */ inline Rel2<Less,double,Par< double > > operator<(const double& lh, const Par< double >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<double>::Type, typename Traits::GetEval<Par< double > >::Type>::value, "operands to '<' operator must have equal evaluation types"); return Rel2<Less,double,Par< double > >(lh,rh);}
/** Addition binary operator.  \ingroup ArithRelations */ inline Rel2<Add,float,Par< float > > operator+(const float& lh, const Par< float >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<float>::Type, typename Traits::GetEval<Par< float > >::Type>::value, "operands to '+' operator must have equal evaluation types"); return Rel2<Add,float,Par< float > >(lh,rh);} /** Subtraction binary operator.  \ingroup ArithRelations */ inline Rel2<Sub,float,Par< float > > operator-(const float& lh, const Par< float >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<float>::Type, typename Traits::GetEval<Par< float > >::Type>::value, "operands to '-' operator must have equal evaluation types"); return Rel2<Sub,float,Par< float > >(lh,rh);} /** Multiplication binary operator.   \ingroup ArithRelations */ inline Rel2<Mul,float,Par< float > > operator*(const float& lh, const Par< float >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<float>::Type, typename Traits::GetEval<Par< float > >::Type>::value, "operands to '*' operator must have equal evaluation types"); return Rel2<Mul,float,Par< float > >(lh,rh);} /** Division binary operator.   \ingroup ArithRelations */ inline Rel2<Div,float,Par< float > > operator/(const float& lh, const Par< float >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<float>::Type, typename Traits::GetEval<Par< float > >::Type>::value, "operands to '/' operator must have equal evaluation types"); return Rel2<Div,float,Par< float > >(lh,rh); }
/** Equal binary operator.  \ingroup EqualityRelations */ inline Rel2<Equal,float,Par< float > > operator==(const float& lh, const Par< float >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<float>::Type, typename Traits::GetEval<Par< float > >::Type>::value, "operands to '==' operator must have equal evaluation types"); return Rel2<Equal,float,Par< float > >(lh,rh);} /** Greater or equal binary operator.  \ingroup InequalityRelations */ inline Rel2<GreaterEqual,float,Par< float > > operator>=(const float& lh, const Par< float >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<float>::Type, typename Traits::GetEval<Par< float > >::Type>::value, "operands to '>=' operator must have equal evaluation types"); return Rel2<GreaterEqual,float,Par< float > >(lh,rh);} /** Less or equal binary operator.  \ingroup InequalityRelations */ inline Rel2<LessEqual,float,Par< float > > operator<=(const float& lh, const Par< float >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<float>::Type, typename Traits::GetEval<Par< float > >::Type>::value, "operands to '<=' operator must have equal evaluation types"); return Rel2<LessEqual,float,Par< float > >(lh,rh);} /** Distinct binary operator.  \ingroup DisequalityRelations */ inline Rel2<Distinct,float,Par< float > > operator!=(const float& lh, const Par< float >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<float>::Type, typename Traits::GetEval<Par< float > >::Type>::value, "operands to '!=' operator must have equal evaluation types"); return Rel2<Distinct,float,Par< float > >(lh,rh);} /** Greater than binary operator.  \ingroup InequalityRelations */ inline Rel2<Greater,float,Par< float > > operator>(const float& lh, const Par< float >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<float>::Type, typename Traits::GetEval<Par< float > >::Type>::value, "operands to '>' operator must have equal evaluation types"); return Rel2<Greater,float,Par< float > >(lh,rh);} /** Less than binary operator.  \ingroup InequalityRelations */ inline Rel2<Less,float,Par< float > > operator<(const float& lh, const Par< float >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<float>::Type, typename Traits::GetEval<Par< float > >::Type>::value, "operands to '<' operator must have equal evaluation types"); return Rel2<Less,float,Par< float > >(lh,rh);}
/** Equal binary operator.  \ingroup EqualityRelations */ inline Rel2<Equal,bool,Par< bool > > operator==(const bool& lh, const Par< bool >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type, typename Traits::GetEval<Par< bool > >::Type>::value, "operands to '==' operator must have equal evaluation types"); return Rel2<Equal,bool,Par< bool > >(lh,rh);} /** Greater or equal binary operator.  \ingroup InequalityRelations */ inline Rel2<GreaterEqual,bool,Par< bool > > operator>=(const bool& lh, const Par< bool >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type, typename Traits::GetEval<Par< bool > >::Type>::value, "operands to '>=' operator must have equal evaluation types"); return Rel2<GreaterEqual,bool,Par< bool > >(lh,rh);} /** Less or equal binary operator.  \ingroup InequalityRelations */ inline Rel2<LessEqual,bool,Par< bool > > operator<=(const bool& lh, const Par< bool >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type, typename Traits::GetEval<Par< bool > >::Type>::value, "operands to '<=' operator must have equal evaluation types"); return Rel2<LessEqual,bool,Par< bool > >(lh,rh);} /** Distinct binary operator.  \ingroup DisequalityRelations */ inline Rel2<Distinct,bool,Par< bool > > operator!=(const bool& lh, const Par< bool >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type, typename Traits::GetEval<Par< bool > >::Type>::value, "operands to '!=' operator must have equal evaluation types"); return Rel2<Distinct,bool,Par< bool > >(lh,rh);} /** Greater than binary operator.  \ingroup InequalityRelations */ inline Rel2<Greater,bool,Par< bool > > operator>(const bool& lh, const Par< bool >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type, typename Traits::GetEval<Par< bool > >::Type>::value, "operands to '>' operator must have equal evaluation types"); return Rel2<Greater,bool,Par< bool > >(lh,rh);} /** Less than binary operator.  \ingroup InequalityRelations */ inline Rel2<Less,bool,Par< bool > > operator<(const bool& lh, const Par< bool >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type, typename Traits::GetEval<Par< bool > >::Type>::value, "operands to '<' operator must have equal evaluation types"); return Rel2<Less,bool,Par< bool > >(lh,rh);} /** Addition binary operator.  \ingroup ArithRelations */ inline Rel2<Add,bool,Par< bool > > operator+(const bool& lh, const Par< bool >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type, typename Traits::GetEval<Par< bool > >::Type>::value, "operands to '+' operator must have equal evaluation types"); return Rel2<Add,bool,Par< bool > >(lh,rh);} /** Subtraction binary operator.  \ingroup ArithRelations */ inline Rel2<Sub,bool,Par< bool > > operator-(const bool& lh, const Par< bool >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type, typename Traits::GetEval<Par< bool > >::Type>::value, "operands to '-' operator must have equal evaluation types"); return Rel2<Sub,bool,Par< bool > >(lh,rh);} /** Multiplication binary operator.   \ingroup ArithRelations */ inline Rel2<Mul,bool,Par< bool > > operator*(const bool& lh, const Par< bool >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type, typename Traits::GetEval<Par< bool > >::Type>::value, "operands to '*' operator must have equal evaluation types"); return Rel2<Mul,bool,Par< bool > >(lh,rh);} /** Division binary operator.   \ingroup ArithRelations */ inline Rel2<Div,bool,Par< bool > > operator/(const bool& lh, const Par< bool >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type, typename Traits::GetEval<Par< bool > >::Type>::value, "operands to '/' operator must have equal evaluation types"); return Rel2<Div,bool,Par< bool > >(lh,rh); } /** Logical AND binary operator.   \ingroup LogicalRelations */ inline Rel2<And,bool,Par< bool > > operator&&(const bool& lh, const Par< bool >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type,bool>::value, "first operand to '&&' (and) operator must have boolean evaluation type"); static_assert(Util::IsSameType<typename Traits::GetEval<Par< bool > >::Type,bool>::value, "second operand to '&&' (and) operator must have boolean evaluation type"); return Rel2<And,bool,Par< bool > >(lh,rh);} /** Logical OR operator.  \ingroup LogicalRelations */ inline Rel2<Or,bool,Par< bool > > operator||(const bool& lh, const Par< bool >& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type,bool>::value, "first operand to '||' (or) operator must have boolean evaluation type"); static_assert(Util::IsSameType<typename Traits::GetEval<Par< bool > >::Type,bool>::value, "second operand to '||' (or) operator must have boolean evaluation type"); return Rel2<Or,bool,Par< bool > >(lh,rh); }
} // Casper