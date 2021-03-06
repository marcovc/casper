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
#include <casper/kernel/common.h>
#include <sstream>

namespace Casper {
 
struct Goal;

// forward declarations of classes defined in /util
namespace Util {
template<class,int> struct StdArray;
template<class> struct StdVector;
template<class> struct StdList;
template<class> struct StdSList;
template<class> struct StdRange;
}

template<class> struct IterationView;
template<class,class> struct ElementView;
template<class,class> struct PredItView;
template<class,class> struct UnionItView;
template<class,class> struct InterItView;
template<class,class> struct DiffItView;
template<class,class> struct SymDiffItView;

namespace Traits {

template<class T>
struct GetEval;

template<class T>
struct GetElem;

template<class T>
struct GetTermElem;

template<class T>
struct GetState;

template<class T>
struct GetTypeStr;

template<class T>
std::string getTypeStr()
{	return GetTypeStr<T>()(); }

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

template<>
struct GetEval<int>
{	typedef	int	Type;	};

template<>
struct GetEval<bool>
{	typedef	bool	Type;	};

template<>
struct GetEval<double>
{	typedef	double	Type;	};

template<>
struct GetEval<float>
{	typedef	float	Type;	};

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
struct GetEval<Util::StdVector<T> >
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
struct GetEval<IterationView<View> >
{	typedef Seq<typename Traits::GetElem<View>::Type>	Type;	};

template<class View,class Elem>
struct GetEval<ElementView<View,Elem> >
{	typedef typename Traits::GetEval<Elem>::Type	Type;	};

#ifdef CASPER_CPP0X
template<class Eval,class... Args>
struct GetEval<std::function<Eval(Args...)> >
{	typedef typename GetEval<Eval>::Type Type;	};

#ifndef _MSC_VER
template<class T>
struct GetEval<std::initializer_list<T> >
{	typedef Seq<typename Traits::GetEval<T>::Type> Type;	};
#endif

#endif

// GetElem

template<class T>
struct GetElem;

template<class T,int dims>
struct GetElem<Util::StdArray<T,dims> >
{	typedef typename Util::StdArray<T,dims>::Elem	Type;	};

template<class T>
struct GetElem<Util::StdVector<T> >
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
struct GetElem<IterationView<T> >
{	typedef typename Traits::GetElem<T>::Type	Type; };

template<class View,class Elem>
struct GetElem<ElementView<View,Elem> >
{	typedef Elem	Type;	};

template<class T1,class T2>
struct GetElem<PredItView<T1,T2> >
{ typedef typename Traits::GetElem<T1>::Type	Type; };

template<class T1,class T2>
struct GetElem<UnionItView<T1,T2> >
{	typedef typename Traits::GetElem<T1>::Type	Type; };

template<class T1,class T2>
struct GetElem<InterItView<T1,T2> >
{	typedef typename Traits::GetElem<T1>::Type	Type; };

template<class T1,class T2>
struct GetElem<DiffItView<T1,T2> >
{	typedef typename Traits::GetElem<T1>::Type	Type; };

template<class T1,class T2>
struct GetElem<SymDiffItView<T1,T2> >
{	typedef typename Traits::GetElem<T1>::Type	Type; };

#ifdef CASPER_CPP0X

#ifndef _MSC_VER
template<class T>
struct GetElem<std::initializer_list<T> >
{	typedef T Type;	};
#endif

#endif

// GetTermElem

template<class T>
struct GetTermElem
{	typedef T Type;	};

template<class T,int dims>
struct GetTermElem<Util::StdArray<T,dims> >
{	typedef typename GetTermElem<T>::Type	Type;	};

template<class T>
struct GetTermElem<Util::StdVector<T> >
{	typedef typename GetTermElem<T>::Type	Type;	};

template<class T>
struct GetTermElem<Util::StdList<T> >
{	typedef typename GetTermElem<T>::Type	Type;	};

template<class T>
struct GetTermElem<Util::StdSList<T> >
{	typedef typename GetTermElem<T>::Type	Type;	};

template<class T>
struct GetTermElem<Util::StdRange<T> >
{	typedef typename GetTermElem<T>::Type	Type;	};

template<class T>
struct GetTermElem<IterationView<T> >
{	typedef typename Traits::GetTermElem<T>::Type	Type; };

template<class View,class Elem>
struct GetTermElem<ElementView<View,Elem> >
{	typedef typename Traits::GetTermElem<Elem>::Type	Type;	};

template<class T1,class T2>
struct GetTermElem<PredItView<T1,T2> >
{ typedef typename Traits::GetTermElem<T1>::Type	Type; };

template<class T1,class T2>
struct GetTermElem<UnionItView<T1,T2> >
{	typedef typename Traits::GetTermElem<T1>::Type	Type; };

template<class T1,class T2>
struct GetTermElem<InterItView<T1,T2> >
{	typedef typename Traits::GetTermElem<T1>::Type	Type; };

template<class T1,class T2>
struct GetTermElem<DiffItView<T1,T2> >
{	typedef typename Traits::GetTermElem<T1>::Type	Type; };

template<class T1,class T2>
struct GetTermElem<SymDiffItView<T1,T2> >
{	typedef typename Traits::GetTermElem<T1>::Type	Type; };

#ifdef CASPER_CPP0X

#ifndef _MSC_VER
template<class T>
struct GetTermElem<std::initializer_list<T> >
{	typedef typename GetTermElem<T>::Type	Type;	};
#endif

#endif

template<>
struct GetTypeStr<int>	{
	std::string operator()()
	{ 	return	"int";	}
};

template<>
struct GetTypeStr<bool>	{
	std::string operator()()
	{ 	return	"bool";	}
};

template<>
struct GetTypeStr<double>	{
	std::string operator()()
	{ 	return	"double";	}
};

template<>
struct GetTypeStr<float>	{
	std::string operator()()
	{ 	return	"float";	}
};

template<>
struct GetTypeStr<uint>	{
	std::string operator()()
	{ 	return	"uint";	}
};

template<>
struct GetTypeStr<short>	{
	std::string operator()()
	{ 	return	"short";	}
};

template<>
struct GetTypeStr<long>	{
	std::string operator()()
	{ 	return	"long";	}
};

template<class T>
struct GetTypeStr<Seq<T> >	{
	std::string operator()()
	{ 	return	std::string("Casper::Seq<")+getTypeStr<T>()+">";	}
};

template<class T>
struct GetTypeStr<Set<T> >	{
	std::string operator()()
	{ 	return	std::string("Casper::Set<")+getTypeStr<T>()+">";	}
};

template<class T, int dims>
struct GetTypeStr<Util::StdArray<T,dims> >	{
	std::string operator()()
	{
		std::stringstream ss;
		ss << dims;
		return std::string("Casper::Util::StdArray<")+getTypeStr<T>()+","+ss.str()+">";
	}
};

template<class T>
struct GetTypeStr<Util::StdVector<T> >	{
	std::string operator()()
	{ 	return	std::string("Casper::Util::StdVector<")+getTypeStr<T>()+">";	}
};

template<class T>
struct GetTypeStr<Util::StdSList<T> >{
	std::string operator()()
	{ 	return	std::string("Casper::Util::StdSList<")+getTypeStr<T>()+">";	}
};

template<class T>
struct GetTypeStr<Util::StdList<T> >{
	std::string operator()()
	{ 	return	std::string("Casper::Util::StdList<")+getTypeStr<T>()+">";	}
};

template<class T>
struct GetTypeStr<Util::StdRange<T> >{
	std::string operator()()
	{ 	return	std::string("Casper::Util::StdRange<")+getTypeStr<T>()+">";	}
};

#ifdef CASPER_CPP0X
//template<class Eval,class... Args>
//struct GetTypeStr<std::function<Eval(Args...)> >
//{	typedef typename GetTypeStr<Eval>::Type Type;	};

#ifndef _MSC_VER
template<class T>
struct GetTypeStr<std::initializer_list<T> >{
	std::string operator()()
	{ 	return	std::string("std::initializer_list<")+getTypeStr<T>()+">";	}
};
#endif

#endif

} // Traits


template<class> struct Expr;

namespace Traits {

template<class Eval>
struct GetEval<Expr<Eval> >
{	typedef Eval	Type;	};

template<class T>
struct GetTermElem<Expr<Seq<T> > >
{	typedef Expr<T> Type;	};

template<class T>
struct GetElem<Expr<Seq<T> > >
{	typedef Expr<T> Type;	};

} // Traits

} // Casper

#ifndef SWIG
template<class Eval,class... Args>
std::ostream& operator<<(std::ostream& os, const std::function<Eval(Args...)>& f)
{	os << "std::function"; return os; }
template<class Eval>
std::ostream& operator<<(std::ostream& os, const std::initializer_list<Eval>& f)
{	os << "std::initializer_list"; return os; }
#endif

#endif /* TRAITS_H_ */
