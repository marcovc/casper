/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_KERNEL_REL_GOALREL_H_
#define CASPER_KERNEL_REL_GOALREL_H_

namespace Casper {

NEW_REL_2(forAll,ForAll)
NEW_FN_3(forAll,ForAll)
NEW_FN_4(forAll,ForAll)
NEW_FN_5(forAll,ForAll)
NEW_REL_1(whileDo,WhileDo)
NEW_FN_2(whileDo,WhileDo)
NEW_REL_2(tryAll,TryAll)
NEW_FN_3(tryAll,TryAll)
NEW_FN_5(tryAll,TryAll)
NEW_FN_4(tryAll,TryAll)

namespace Traits {

template<class T1,class T2,class T3>
struct GetEval<Rel3<ForAll,T1,T2,T3> >
{	typedef	bool	Type;	};

template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<ForAll,T1,T2,T3,T4> >
{	typedef	bool	Type;	};

template<class T1,class T2,class T3,class T4,class T5>
struct GetEval<Rel5<ForAll,T1,T2,T3,T4,T5> >
{	typedef	bool	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<WhileDo,T1,T2,T3> >
{	typedef	bool	Type;	};

template<class T1,class T2,class T3>
struct GetEval<Rel3<TryAll,T1,T2,T3> >
{	typedef bool	Type;	};

template<class T1,class T2,class T3,class T4>
struct GetEval<Rel4<TryAll,T1,T2,T3,T4> >
{	typedef	bool	Type;	};

template<class T1,class T2,class T3,class T4,class T5>
struct GetEval<Rel5<TryAll,T1,T2,T3,T4,T5> >
{	typedef	bool	Type;	};

} // Traits

}


#endif /* CASPER_KERNEL_REL_GOALREL_H_ */
