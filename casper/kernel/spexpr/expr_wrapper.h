/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2012-2012 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_KERNEL_SPEXPR_EXPR_WRAPPER_H_
#define CASPER_KERNEL_SPEXPR_EXPR_WRAPPER_H_

#include <casper/kernel/spexpr/expr.h>

namespace Casper {
namespace Detail {

template<class Source,class Target>
struct Create;
//{
//	Target operator()();
//	//{	throw Casper::Exception::TypeCoercion(typeid(Source).name(),typeid(Target).name());	}
//	template<class T1>
//	Target operator()(T1);
//	//{	throw Casper::Exception::TypeCoercion(typeid(Source).name(),typeid(Target).name());	}
//	template<class T1,class T2>
//	Target operator()(T1,T2);
//	//{	throw Casper::Exception::TypeCoercion(typeid(Source).name(),typeid(Target).name());	}
//
//};

template<class Source,class Target>
struct CreateLiteral
{
	Target operator()(const Source& t1)
	{	throw Casper::Exception::TypeCoercion(typeid(Source).name(),typeid(Target).name());	}
};

} // Detail
} // Casper


#include <casper/kernel/spexpr/ref.h>
#include <casper/kernel/spexpr/goal.h>


namespace Casper {
namespace Detail {

template<class Eval,class T>
struct ExprWrapper : IExpr<Eval>
{
	ExprWrapper(const T& t) : t(t) {}
	Eval toLiteral() const
	{	return CreateLiteral<T,Eval>()(t);	}
	CP::DomExpr<Eval> toCPDomExpr(CP::Store& store) const;
	//{	return Create<T,CP::DomExpr<Eval> >()(store,t);	}
	CP::BndExpr<Eval> toCPBndExpr(CP::Store& store) const;
	//{	return Create<T,CP::BndExpr<Eval> >()(store,t);	}
	CP::ValExpr<Eval> toCPValExpr(CP::Store& store) const;
	//{	return Create<T,CP::ValExpr<Eval> >()(store,t);	}
	Ref<Eval>	toRef(State& state) const
	{	return Create<T,Ref<Eval> >()(state,t);	}
	Ref<CP::Var<Eval> >	toCPVarRef(State& state) const;
	//{	return Create<T,Ref<CP::Var<Eval> > >()(state,t);	}
	State* const getPState() const
	{	return GetPState<T>()(t);	}

	std::ostream& print(std::ostream& os) const
	{	return os << t; }
	T t;
};

template<class T>
struct ExprWrapper<bool,T> : IExpr<bool>
{
	ExprWrapper(const T& t) : t(t) {}
	bool toLiteral() const
	{	return CreateLiteral<T,bool>()(t);	}
	CP::DomExpr<bool> toCPDomExpr(CP::Store& store) const;
	//{	return Create<T,CP::DomExpr<bool> >()(store,t);	}
	CP::BndExpr<bool> toCPBndExpr(CP::Store& store) const;
	//{	return Create<T,CP::BndExpr<bool> >()(store,t);	}
	CP::ValExpr<bool> toCPValExpr(CP::Store& store) const;
	//{	return Create<T,CP::ValExpr<bool> >()(store,t);	}
	CP::ChkExpr toChkExpr(CP::Store& store) const;
	//{	return Create<T,CP::ChkExpr>()(store,t);	}
	Ref<bool>	toRef(State& state) const
	{	return Create<T,Ref<bool> >()(state,t);	}
	Ref<CP::Var<bool> >	toCPVarRef(State& state) const;
	//{	return Create<T,Ref<CP::Var<bool> > >()(state,t);	}
	Goal	toGoal(State& state) const
	{	return Create<T,Goal>()(state,t);	}

	bool	postDomFilter(CP::Store& store) const;
	//{	return PostFilter<T>::dom(store,t);	}
	bool	postBndFilter(CP::Store& store) const;
	//{	return PostFilter<T>::bnd(store,t);	}
	bool	postValFilter(CP::Store& store) const;
	//{	return PostFilter<T>::val(store,t);	}

	State* const getPState() const
	{	return GetPState<T>()(t);	}
	std::ostream& print(std::ostream& os) const
	{	return os << t; }
	T t;
};

template<class Eval,class T>
struct ExprWrapper<Seq<Eval>,T> : IExpr<Seq<Eval> >
{
	ExprWrapper(const T& t) : t(t) {}
	Util::StdArray<Eval,1> toLitStdArray() const
	{	return Create<T,Util::StdArray<Eval,1> >()(t);	}
	Util::StdArray<Eval,2> toLitStdArray2() const
	{	return Create<T,Util::StdArray<Eval,2> >()(t);	}
	Util::StdArray<Expr<Eval> > toStdArray() const
	{	return Create<T,Util::StdArray<Expr<Eval> > >()(t);	}
	IterationExpr<Expr<Eval> > toIterationExpr() const
	{	return Create<T,IterationExpr<Expr<Eval> > >()(t);	}
	State* const getPState() const
	{	return GetPState<T>()(t);	}
	std::ostream& print(std::ostream& os) const
	{	return ::operator<<(os,t); }
	T t;
};

} // Detail

template<class Eval>
template<class T>
Expr<Eval>::Expr(const T& t) : Super(new Detail::ExprWrapper<Eval,T>(t)) {}

//template<>
template<class T>
Expr<bool>::Expr(const T& t) : Super(new Detail::ExprWrapper<bool,T>(t)) {}

template<class Eval>
template<class T>
Expr<Casper::Seq<Eval> >::Expr(const T& t) : Super(new Detail::ExprWrapper<Casper::Seq<Eval>,T>(t)) {}


} // Casper


#ifdef CASPER_PRECOMPILED
#include <casper/kernel/spexpr/explicit.h>
//#include <casper/cp/spexpr/wrapper.h>
#include <casper/cp/int/spexpr/wrapper.h>
#include <casper/cp/set/spexpr/wrapper.h>

//#include <casper/cp/spexpr/expr.h>
//#include <casper/cp/int/spexpr/explicit.h>
//#include <casper/cp/set/spexpr/explicit.h>
#endif

#include <casper/kernel/spexpr/array.h>
#include <casper/kernel/spexpr/list.h>
#include <casper/kernel/spexpr/iteration.h>
#include <casper/kernel/spexpr/literal.h>
#include <casper/kernel/spexpr/variable.h>
#include <casper/cp/spexpr/expr.h>

namespace Casper {
namespace Detail {

#ifndef SWIG
template<class Obj>
struct PostFilter
{
	static bool dom(CP::Store& store, const Obj& obj);
	static bool bnd(CP::Store& store, const Obj& obj);
	static bool val(CP::Store& store, const Obj& obj);
};

template<class Obj>
bool PostFilter<Obj>::dom(CP::Store& store, const Obj& obj)
{	return Casper::CP::postDomFilter(store,obj);	}

template<class Obj>
bool PostFilter<Obj>::bnd(CP::Store& store, const Obj& obj)
{	return Casper::CP::postBndFilter(store,obj);	}

template<class Obj>
bool PostFilter<Obj>::val(CP::Store& store, const Obj& obj)
{	return Casper::CP::postValFilter(store,obj);	}
#endif

template<class Eval,class T>
CP::DomExpr<Eval> ExprWrapper<Eval,T>::toCPDomExpr(CP::Store& store) const
{	return Create<T,CP::DomExpr<Eval> >()(store,t);	}

template<class Eval,class T>
CP::BndExpr<Eval> ExprWrapper<Eval,T>::toCPBndExpr(CP::Store& store) const
{	return Create<T,CP::BndExpr<Eval> >()(store,t);	}

template<class Eval,class T>
CP::ValExpr<Eval> ExprWrapper<Eval,T>::toCPValExpr(CP::Store& store) const
{	return Create<T,CP::ValExpr<Eval> >()(store,t);	}

template<class Eval,class T>
Ref<CP::Var<Eval> >	ExprWrapper<Eval,T>::toCPVarRef(State& state) const
{	return Create<T,Ref<CP::Var<Eval> > >()(state,t);	}

template<class T>
CP::DomExpr<bool> ExprWrapper<bool,T>::toCPDomExpr(CP::Store& store) const
{	return Create<T,CP::DomExpr<bool> >()(store,t);	}

template<class T>
CP::BndExpr<bool> ExprWrapper<bool,T>::toCPBndExpr(CP::Store& store) const
{	return Create<T,CP::BndExpr<bool> >()(store,t);	}

template<class T>
CP::ValExpr<bool> ExprWrapper<bool,T>::toCPValExpr(CP::Store& store) const
{	return Create<T,CP::ValExpr<bool> >()(store,t);	}

template<class T>
CP::ChkExpr ExprWrapper<bool,T>::toChkExpr(CP::Store& store) const
{	return Create<T,CP::ChkExpr>()(store,t);	}

template<class T>
Ref<CP::Var<bool> >	ExprWrapper<bool,T>::toCPVarRef(State& state) const
{	return Create<T,Ref<CP::Var<bool> > >()(state,t);	}

template<class T>
bool	ExprWrapper<bool,T>::postDomFilter(CP::Store& store) const
{	return PostFilter<T>::dom(store,t);	}

template<class T>
bool	ExprWrapper<bool,T>::postBndFilter(CP::Store& store) const
{	return PostFilter<T>::bnd(store,t);	}

template<class T>
bool	ExprWrapper<bool,T>::postValFilter(CP::Store& store) const
{	return PostFilter<T>::val(store,t);	}

}
}




#endif /* CASPER_KERNEL_SPEXPR_EXPR_WRAPPER_H_ */
