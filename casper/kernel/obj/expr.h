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
 

#ifndef CASPER_KERNEL_OBJ_EXPR_H_
#define CASPER_KERNEL_OBJ_EXPR_H_

#include <casper/cp.h>

namespace Casper {

namespace Detail {

template<class Eval>
struct IExpr
{
	virtual CP::DomExpr<Eval> toDomExpr(CP::Store& store) const = 0;
	virtual CP::BndExpr<Eval> toBndExpr(CP::Store& store) const = 0;
	virtual CP::ValExpr<Eval> toValExpr(CP::Store& store) const = 0;
	virtual std::ostream& print(std::ostream& os) const = 0;
};

template<>
struct IExpr<bool>
{
	virtual CP::DomExpr<bool> toDomExpr(CP::Store& store) const = 0;
	virtual CP::BndExpr<bool> toBndExpr(CP::Store& store) const = 0;
	virtual CP::ValExpr<bool> toValExpr(CP::Store& store) const = 0;
	virtual CP::ChkExpr 	toChkExpr(CP::Store& store) const = 0;
	virtual bool	postDomFilter(CP::Store& store) const = 0;
	virtual bool	postBndFilter(CP::Store& store) const = 0;
	virtual bool	postValFilter(CP::Store& store) const = 0;
	virtual std::ostream& print(std::ostream& os) const = 0;
};

template<class Eval>
struct IExpr<Seq<Eval> >
{
	virtual const Util::StdArray<Eval,1>& toStdArray() const = 0;
	virtual const Util::StdArray<Eval,2>& toStdArray2() const = 0;
	virtual std::ostream& print(std::ostream& os) const = 0;
};

template<class Source,class Target> struct Create;

template<class Eval,class T>
struct ExprWrapper : IExpr<Eval>
{
	ExprWrapper(const T& t) : t(t) {}
	CP::DomExpr<Eval> toDomExpr(CP::Store& store) const
	{	return Create<T,CP::DomExpr<Eval> >()(store,t);	}
	CP::BndExpr<Eval> toBndExpr(CP::Store& store) const
	{	return Create<T,CP::BndExpr<Eval> >()(store,t);	}
	CP::ValExpr<Eval> toValExpr(CP::Store& store) const
	{	return Create<T,CP::ValExpr<Eval> >()(store,t);	}
	std::ostream& print(std::ostream& os) const
	{	::operator<<(os,t); return os; }
	T t;
};

template<class T>
struct ExprWrapper<bool,T> : IExpr<bool>
{
	ExprWrapper(const T& t) : t(t) {}
	CP::DomExpr<bool> toDomExpr(CP::Store& store) const
	{	return Create<T,CP::DomExpr<bool> >()(store,t);	}
	CP::BndExpr<bool> toBndExpr(CP::Store& store) const
	{	return Create<T,CP::BndExpr<bool> >()(store,t);	}
	CP::ValExpr<bool> toValExpr(CP::Store& store) const
	{	return Create<T,CP::ValExpr<bool> >()(store,t);	}
	CP::ChkExpr toChkExpr(CP::Store& store) const
	{	return Create<T,CP::ChkExpr>()(store,t);	}
	bool	postDomFilter(CP::Store& store) const
	{	return store.post(t,Casper::CP::postDomFilter);	}
	bool	postBndFilter(CP::Store& store) const
	{	return store.post(t,Casper::CP::postBndFilter);	}
	bool	postValFilter(CP::Store& store) const
	{	return store.post(t,Casper::CP::postValFilter);	}
	std::ostream& print(std::ostream& os) const
	{	return ::operator<<(os,t); }
	T t;
};

template<class Eval,class T>
struct ExprWrapper<Seq<Eval>,T> : IExpr<Seq<Eval> >
{
	ExprWrapper(const T& t) : t(t) {}
	const Util::StdArray<Eval,1>& toStdArray() const
	{	return Create<T,Util::StdArray<Eval,1> >()(t);	}
	const Util::StdArray<Eval,2>& toStdArray2() const
	{	return Create<T,Util::StdArray<Eval,2> >()(t);	}
	std::ostream& print(std::ostream& os) const
	{	return ::operator<<(os,t); }
	T t;
};

} // Detail

template<class Eval>
struct Expr : Casper::Util::SPImplIdiom<Detail::IExpr<Eval> >
{
	typedef Casper::Util::SPImplIdiom<Detail::IExpr<Eval> > Super;

	template<class T>
	Expr(const T& t) : Super(new Detail::ExprWrapper<Eval,T>(t)) {}

	Expr(const Expr& expr) : Super(expr) {}

	CP::DomExpr<Eval> toDomExpr(CP::Store& store) const
	{	return this->getImpl().toDomExpr(store);	}
	CP::BndExpr<Eval> toBndExpr(CP::Store& store) const
	{	return this->getImpl().toBndExpr(store);	}
	CP::ValExpr<Eval> toValExpr(CP::Store& store) const
	{	return this->getImpl().toValExpr(store);	}

	// SWIG needs this
	~Expr() {}
};

template<>
struct Expr<bool> : Casper::Util::SPImplIdiom<Detail::IExpr<bool> >
{
	typedef Casper::Util::SPImplIdiom<Detail::IExpr<bool> > Super;

	template<class T>
	Expr(const T& t) : Super(new Detail::ExprWrapper<bool,T>(t)) {}

	Expr(const Expr& expr) : Super(expr) {}

	CP::DomExpr<bool> toDomExpr(CP::Store& store) const
	{	return this->getImpl().toDomExpr(store);	}
	CP::BndExpr<bool> toBndExpr(CP::Store& store) const
	{	return this->getImpl().toBndExpr(store);	}
	CP::ValExpr<bool> toValExpr(CP::Store& store) const
	{	return this->getImpl().toValExpr(store);	}
	CP::ChkExpr toChkExpr(CP::Store& store) const
	{	return this->getImpl().toChkExpr(store);	}

	bool postDomFilter(CP::Store& store) const
	{	return this->getImpl().postDomFilter(store);	}
	bool postBndFilter(CP::Store& store) const
	{	return this->getImpl().postBndFilter(store);	}
	bool postValFilter(CP::Store& store) const
	{	return this->getImpl().postValFilter(store);	}

	// SWIG needs this
	~Expr() {}
};

template<class Eval>
struct Expr<Seq<Eval> > : Casper::Util::SPImplIdiom<Detail::IExpr<Seq<Eval> > >
{
	typedef Casper::Util::SPImplIdiom<Detail::IExpr<Seq<Eval> > > Super;

	template<class T>
	Expr(const T& t) : Super(new Detail::ExprWrapper<Seq<Eval>,T>(t)) {}

	Expr(const Expr& expr) : Super(expr) {}

	const Util::StdArray<Eval,1>& toStdArray() const
	{	return this->getImpl().toStdArray();	}
	const Util::StdArray<Eval,2>& toStdArray2() const
	{	return this->getImpl().toStdArray2();	}

	operator const Util::StdArray<Eval,1>&() const
	{	return toStdArray(); }
	operator const Util::StdArray<Eval,2>&() const
	{	return toStdArray2(); }

	// SWIG needs this
	~Expr() {}
};

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

//namespace CP {
//namespace Traits {
//struct GetDom<Casper>
//} // Traits
//} // CP

}

template<class T>
std::ostream& operator<<(std::ostream& os,const Casper::Expr<T>& expr)
{	return expr.getImpl().print(os); }


#endif /* CASPER_KERNEL_OBJ_EXPR_H_ */

#include <casper/kernel/obj/domexpr.h>
#include <casper/kernel/obj/bndexpr.h>
#include <casper/kernel/obj/valexpr.h>
#include <casper/kernel/obj/chkexpr.h>
#include <casper/kernel/obj/array.h>
