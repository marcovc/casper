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
 

#ifndef CASPER_KERNEL_TRAITS_H_
#define CASPER_KERNEL_TRAITS_H_

#include <functional>

namespace Casper {

struct Goal;

// forward declarations of classes defined in /util
namespace Util {
template<class,int> struct StdArray;
template<class> struct StdList;
template<class> struct StdSList;
template<class> struct StdRange;
template<class> struct IterationView;
template<class,class> struct PredItView;
template<class,class> struct UnionItView;
template<class,class> struct InterItView;
template<class,class> struct DiffItView;
template<class,class> struct SymDiffItView;
}

namespace Traits {

template<class T>
struct GetEval;

template<class T>
struct GetElem;

// IsIntegral

// FIXME: replace by c++ builtin traits when ready
template<class T>
struct IsIntegral
{	static const bool value = false;	};

template<>
struct IsIntegral<int>
{	static const bool value = true;	};

template<>
struct IsIntegral<bool>
{	static const bool value = false;	};

// GetEval

template<class T>
struct GetEval
{	typedef	T	Type;	};

template<>
struct GetEval<uint>
{	typedef	int	Type;	};

template<>
struct GetEval<short>
{	typedef	int	Type;	};

template<>
struct GetEval<long>
{	typedef	int	Type;	};

template<class T, int dims>
struct GetEval<Util::StdArray<T,dims> >
{	typedef Seq<typename Traits::GetEval<T>::Type>	Type;	};

template<class T>
struct GetEval<Util::StdSList<T> >
{	typedef Seq<typename Traits::GetEval<T>::Type>	Type;	};

template<class T>
struct GetEval<Util::StdList<T> >
{	typedef Seq<typename Traits::GetEval<T>::Type>	Type;	};

template<class T>
struct GetEval<Util::StdRange<T> >
{ 	typedef Seq<typename Traits::GetEval<T>::Type>	Type;	};

template<class View>
struct GetEval<Util::IterationView<View> >
{	typedef Seq<typename Traits::GetElem<View>::Type>	Type;	};

template<>
struct GetEval<std::function<Goal()> >
{	typedef bool Type;	};

// GetElem

template<class T>
struct GetElem;

template<class T,int dims>
struct GetElem<Util::StdArray<T,dims> >
{	typedef T	Type;	};

template<class T>
struct GetElem<Util::StdList<T> >
{	typedef T	Type;	};

template<class T>
struct GetElem<Util::StdSList<T> >
{	typedef T	Type;	};

template<class T>
struct GetElem<Util::StdRange<T> >
{	typedef T	Type;	};

template<class T>
struct GetElem<Util::IterationView<T> >
{	typedef typename Traits::GetElem<T>::Type	Type; };

template<class T1,class T2>
struct GetElem<Util::PredItView<T1,T2> >
{ typedef typename Traits::GetElem<T1>::Type	Type; };

template<class T1,class T2>
struct GetElem<Util::UnionItView<T1,T2> >
{	typedef typename Traits::GetElem<T1>::Type	Type; };

template<class T1,class T2>
struct GetElem<Util::InterItView<T1,T2> >
{	typedef typename Traits::GetElem<T1>::Type	Type; };

template<class T1,class T2>
struct GetElem<Util::DiffItView<T1,T2> >
{	typedef typename Traits::GetElem<T1>::Type	Type; };

template<class T1,class T2>
struct GetElem<Util::SymDiffItView<T1,T2> >
{	typedef typename Traits::GetElem<T1>::Type	Type; };

}
}

#endif /* TRAITS_H_ */
