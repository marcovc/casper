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
/** Min value (from a sequence) unary operator.  \ingroup ArithRelations */ template< class T1, int dims, class T2 > inline Rel1<Min,CP::VarArray< T1,dims,T2 > > min(const CP::VarArray< T1,dims,T2 >& rh) { return Rel1<Min,CP::VarArray< T1,dims,T2 > >(rh);} /** Max value (from a sequence) unary operator. \ingroup ArithRelations */ template< class T1, int dims, class T2 > inline Rel1<Max,CP::VarArray< T1,dims,T2 > > max(const CP::VarArray< T1,dims,T2 >& rh) { return Rel1<Max,CP::VarArray< T1,dims,T2 > >(rh);}
/** Equal binary operator.  \ingroup EqualityRelations */ template< class T1, int dims, class T2, class T > inline Rel2<Equal,CP::VarArray< T1,dims,T2 >,T > operator==(const CP::VarArray< T1,dims,T2 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<CP::VarArray< T1,dims,T2 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '==' operator must have equal evaluation types"); return Rel2<Equal,CP::VarArray< T1,dims,T2 >,T >(lh,rh);} /** Greater or equal binary operator.  \ingroup InequalityRelations */ template< class T1, int dims, class T2, class T > inline Rel2<GreaterEqual,CP::VarArray< T1,dims,T2 >,T > operator>=(const CP::VarArray< T1,dims,T2 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<CP::VarArray< T1,dims,T2 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '>=' operator must have equal evaluation types"); return Rel2<GreaterEqual,CP::VarArray< T1,dims,T2 >,T >(lh,rh);} /** Less or equal binary operator.  \ingroup InequalityRelations */ template< class T1, int dims, class T2, class T > inline Rel2<LessEqual,CP::VarArray< T1,dims,T2 >,T > operator<=(const CP::VarArray< T1,dims,T2 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<CP::VarArray< T1,dims,T2 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '<=' operator must have equal evaluation types"); return Rel2<LessEqual,CP::VarArray< T1,dims,T2 >,T >(lh,rh);} /** Distinct binary operator.  \ingroup DisequalityRelations */ template< class T1, int dims, class T2, class T > inline Rel2<Distinct,CP::VarArray< T1,dims,T2 >,T > operator!=(const CP::VarArray< T1,dims,T2 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<CP::VarArray< T1,dims,T2 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '!=' operator must have equal evaluation types"); return Rel2<Distinct,CP::VarArray< T1,dims,T2 >,T >(lh,rh);} /** Greater than binary operator.  \ingroup InequalityRelations */ template< class T1, int dims, class T2, class T > inline Rel2<Greater,CP::VarArray< T1,dims,T2 >,T > operator>(const CP::VarArray< T1,dims,T2 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<CP::VarArray< T1,dims,T2 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '>' operator must have equal evaluation types"); return Rel2<Greater,CP::VarArray< T1,dims,T2 >,T >(lh,rh);} /** Less than binary operator.  \ingroup InequalityRelations */ template< class T1, int dims, class T2, class T > inline Rel2<Less,CP::VarArray< T1,dims,T2 >,T > operator<(const CP::VarArray< T1,dims,T2 >& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<CP::VarArray< T1,dims,T2 > >::Type, typename Traits::GetEval<T>::Type>::value, "operands to '<' operator must have equal evaluation types"); return Rel2<Less,CP::VarArray< T1,dims,T2 >,T >(lh,rh);}
}