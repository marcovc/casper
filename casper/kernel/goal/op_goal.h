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
/** Logical AND binary operator.   \ingroup LogicalRelations */ template< class T > inline Rel2<And,Goal,T > operator&&(const Goal& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Goal>::Type,bool>::value, "first operand to '&&' (and) operator must have boolean evaluation type"); static_assert(Util::IsSameType<typename Traits::GetEval<T>::Type,bool>::value, "second operand to '&&' (and) operator must have boolean evaluation type"); return Rel2<And,Goal,T >(lh,rh);} /** Logical OR operator.  \ingroup LogicalRelations */ template< class T > inline Rel2<Or,Goal,T > operator||(const Goal& lh, const T& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Goal>::Type,bool>::value, "first operand to '||' (or) operator must have boolean evaluation type"); static_assert(Util::IsSameType<typename Traits::GetEval<T>::Type,bool>::value, "second operand to '||' (or) operator must have boolean evaluation type"); return Rel2<Or,Goal,T >(lh,rh); }
/** Logical AND binary operator.   \ingroup LogicalRelations */ inline Rel2<And,bool,Goal > operator&&(const bool& lh, const Goal& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type,bool>::value, "first operand to '&&' (and) operator must have boolean evaluation type"); static_assert(Util::IsSameType<typename Traits::GetEval<Goal>::Type,bool>::value, "second operand to '&&' (and) operator must have boolean evaluation type"); return Rel2<And,bool,Goal >(lh,rh);} /** Logical OR operator.  \ingroup LogicalRelations */ inline Rel2<Or,bool,Goal > operator||(const bool& lh, const Goal& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<bool>::Type,bool>::value, "first operand to '||' (or) operator must have boolean evaluation type"); static_assert(Util::IsSameType<typename Traits::GetEval<Goal>::Type,bool>::value, "second operand to '||' (or) operator must have boolean evaluation type"); return Rel2<Or,bool,Goal >(lh,rh); }
/** Logical AND binary operator.   \ingroup LogicalRelations */ inline Rel2<And,Goal,std::function<Goal()> > operator&&(const Goal& lh, const std::function<Goal()>& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Goal>::Type,bool>::value, "first operand to '&&' (and) operator must have boolean evaluation type"); static_assert(Util::IsSameType<typename Traits::GetEval<std::function<Goal()> >::Type,bool>::value, "second operand to '&&' (and) operator must have boolean evaluation type"); return Rel2<And,Goal,std::function<Goal()> >(lh,rh);} /** Logical OR operator.  \ingroup LogicalRelations */ inline Rel2<Or,Goal,std::function<Goal()> > operator||(const Goal& lh, const std::function<Goal()>& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<Goal>::Type,bool>::value, "first operand to '||' (or) operator must have boolean evaluation type"); static_assert(Util::IsSameType<typename Traits::GetEval<std::function<Goal()> >::Type,bool>::value, "second operand to '||' (or) operator must have boolean evaluation type"); return Rel2<Or,Goal,std::function<Goal()> >(lh,rh); }
/** Logical AND binary operator.   \ingroup LogicalRelations */ inline Rel2<And,std::function<Goal()>,Goal > operator&&(const std::function<Goal()>& lh, const Goal& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<std::function<Goal()> >::Type,bool>::value, "first operand to '&&' (and) operator must have boolean evaluation type"); static_assert(Util::IsSameType<typename Traits::GetEval<Goal>::Type,bool>::value, "second operand to '&&' (and) operator must have boolean evaluation type"); return Rel2<And,std::function<Goal()>,Goal >(lh,rh);} /** Logical OR operator.  \ingroup LogicalRelations */ inline Rel2<Or,std::function<Goal()>,Goal > operator||(const std::function<Goal()>& lh, const Goal& rh) { static_assert(Util::IsSameType<typename Traits::GetEval<std::function<Goal()> >::Type,bool>::value, "first operand to '||' (or) operator must have boolean evaluation type"); static_assert(Util::IsSameType<typename Traits::GetEval<Goal>::Type,bool>::value, "second operand to '||' (or) operator must have boolean evaluation type"); return Rel2<Or,std::function<Goal()>,Goal >(lh,rh); }
}
