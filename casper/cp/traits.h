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
 

#ifndef CASPER_CP_TRAITS_H_
#define CASPER_CP_TRAITS_H_

#include <casper/kernel/traits.h>

namespace Casper {
namespace CP {
namespace Traits {

template<class View>
struct GetDefaultDom;

template<class View>
struct GetDom
{	typedef typename GetDefaultDom<typename Casper::Traits::GetEval<View>::Type>::Type	 	Type;};

// FIXME: this is required to work with different domain types
#if 0

struct Traits::GetDom<Rel2<Add,T1,T2> >::Typepedef typename Traits::GetDom<T1>::Type	Dom;	};

template<class T1,class T2>
struct Traits::GetDom<Rel2<Mul,T1,T2> >::Typepedef typename Traits::GetDom<T1>::Type	Dom;	};

template<class T1,class T2>
struct Traits::GetDom<Rel2<Pow,T1,T2> >::Typepedef typename Traits::GetDom<T1>::Type	Dom;	};

template<class T1,class T2>
struct Traits::GetDom<Rel2<Div,T1,T2> >::Typepedef typename Traits::GetDom<T1>::Type	Dom;	};

template<class T1,class T2>
struct Traits::GetDom<Rel2<Min,T1,T2> >::Typepedef typename Traits::GetDom<T1>::Type	Dom;	};

template<class T1,class T2>
struct Traits::GetDom<Rel2<Max,T1,T2> >::Typepedef typename Traits::GetDom<T1>::Type	Dom;	};

template<class T1>
struct Traits::GetDom<Rel1<Sym,T1> >::Typepedef typename Traits::GetDom<T1>::Type	Dom;	};

template<class T1>
struct Traits::GetDom<Rel1<Abs,T1> >::Typepedef typename Traits::GetDom<T1>::Type	Dom;	};

template<class T1>
struct Traits::GetDom<Rel1<Exp,T1> >::Typepedef typename Traits::GetDom<T1>::Type	Dom;	};

template<class T1>
struct Traits::GetDom<Rel1<Log,T1> >::Typepedef typename Traits::GetDom<T1>::Type	Dom;	};

template<class ArrayView,class IdxView>
struct Traits::GetDom<Rel2<Element,ArrayView,IdxView> >::Typepedef typename Traits::GetDom<typename Casper::Traits::GetElem<ArrayView>::Type>::Type	Dom;	};


template<class T1,class T2,class T3>
struct Traits::GetDom<Rel3<InRange,T1,T2,T3> >::Typepedef	typename Traits::GetDom<T1>::Type Dom;	};

template<class T1,class T2,class T3>
struct Traits::GetDom<Rel3<Cache,T1,T2,T3> >::Typepedef	typename Traits::GetDom<T1>::Type Dom;	};

template<class T1,class T2,class T3,class T4>
struct Traits::GetDom<Rel4<All,T1,T2,T3,T4> >::Typepedef	typename Traits::GetDom<T4>::Type Dom;	};
#endif

} // Traits
} // CP
} // Casper

#endif /* CASPER_CP_TRAITS_H_ */
