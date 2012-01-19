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

#include <typeinfo>

#include <casper/cp.h>
#include <casper/kernel.h>
#include <casper/kernel/expr.h>
#include <iostream>
#include <casper/fwddecl.h>

namespace Casper {

namespace Detail {


template<class Eval>
struct IExpr
{
	virtual	Eval toLiteral() const = 0;
	virtual CP::DomExpr<Eval> toCPDomExpr(CP::Store& store) const = 0;
	virtual CP::BndExpr<Eval> toCPBndExpr(CP::Store& store) const = 0;
	virtual CP::ValExpr<Eval> toCPValExpr(CP::Store& store) const = 0;
	virtual Ref<Eval>	toRef(State& state) const = 0;
	virtual Ref<CP::Var<Eval> >	toCPVarRef(State& state) const = 0;
	virtual State* const getPState() const = 0;
	virtual std::ostream& print(std::ostream& os) const = 0;
	virtual std::string getTypeStr() const = 0;
};
 
template<>
struct IExpr<bool>
{
	virtual	bool toLiteral() const = 0;
	virtual CP::DomExpr<bool> toCPDomExpr(CP::Store& store) const = 0;
	virtual CP::BndExpr<bool> toCPBndExpr(CP::Store& store) const = 0;
	virtual CP::ValExpr<bool> toCPValExpr(CP::Store& store) const = 0;
	virtual CP::ChkExpr 	toChkExpr(CP::Store& store) const = 0;
	virtual Ref<bool>	toRef(State& state) const = 0;
	virtual Ref<CP::Var<bool> >	toCPVarRef(State& state) const = 0;
	virtual Goal	toGoal(State& state) const = 0;

	virtual bool	postDomFilter(CP::Store& store) const = 0;
	virtual bool	postBndFilter(CP::Store& store) const = 0;
	virtual bool	postValFilter(CP::Store& store) const = 0;
	virtual State* const getPState() const = 0;
	virtual std::ostream& print(std::ostream& os) const = 0;
	virtual std::string getTypeStr() const = 0;
};

template<class Eval>
struct IExpr<Seq<Eval> >
{
	virtual Util::StdArray<Eval,1> toLitStdArray() const = 0;
	virtual Util::StdArray<Eval,2> toLitStdArray2() const = 0;
	virtual Util::StdArray<Expr<Eval> > toStdArray() const = 0;
	virtual IterationExpr<Expr<Eval> > toIterationExpr() const = 0;
	virtual Expr<Eval> getElement(uint idx) const = 0;
	virtual Expr<Seq<Eval> > getSeqElement(uint idx) const = 0;
	virtual bool hasSeqElement() const = 0;
	virtual State* const getPState() const = 0;
	virtual std::ostream& print(std::ostream& os) const = 0;
	virtual std::string getTypeStr() const = 0;
};


} // Detail

template<class Eval>
struct Expr : Casper::Util::SPImplIdiom<Detail::IExpr<Eval> >
{
	typedef Casper::Util::SPImplIdiom<Detail::IExpr<Eval> > Super;

	template<class T>
	Expr(const T& t);

	Expr(Detail::IExpr<Eval>* t) : Super(t)	{}

	Expr(const Expr& expr) : Super(expr) {}

#ifndef SWIG

	Eval toLiteral() const
	{	return this->getImpl().toLiteral();	}

	CP::DomExpr<Eval> toCPDomExpr(CP::Store& store) const
	{	return this->getImpl().toCPDomExpr(store);	}
	CP::BndExpr<Eval> toCPBndExpr(CP::Store& store) const
	{	return this->getImpl().toCPBndExpr(store);	}
	CP::ValExpr<Eval> toCPValExpr(CP::Store& store) const
	{	return this->getImpl().toCPValExpr(store);	}
	Ref<Eval>	toRef(State& state) const
	{	return this->getImpl().toRef(state);	}
	Ref<CP::Var<Eval> >	toCPVarRef(State& state) const
	{	return this->getImpl().toCPVarRef(state);	}
	State* const getPState() const
	{	return this->getImpl().getPState();	}

#endif
	std::string getTypeStr() const
	{	return this->getImpl().getTypeStr();	}

	// SWIG needs this
	~Expr() {}
};

template<>
struct Expr<bool> : Casper::Util::SPImplIdiom<Detail::IExpr<bool> >
{
	typedef Casper::Util::SPImplIdiom<Detail::IExpr<bool> > Super;

	template<class T>
	Expr(const T& t);

	Expr(const Expr& expr) : Super(expr) {}

	Expr(Detail::IExpr<bool>* t) : Super(t)	{}

#ifndef SWIG

	bool toLiteral() const
	{	return this->getImpl().toLiteral();	}

	CP::DomExpr<bool> toCPDomExpr(CP::Store& store) const
	{	return this->getImpl().toCPDomExpr(store);	}
	CP::BndExpr<bool> toCPBndExpr(CP::Store& store) const
	{	return this->getImpl().toCPBndExpr(store);	}
	CP::ValExpr<bool> toCPValExpr(CP::Store& store) const
	{	return this->getImpl().toCPValExpr(store);	}
	CP::ChkExpr toChkExpr(CP::Store& store) const
	{	return this->getImpl().toChkExpr(store);	}
	Ref<bool>	toRef(State& state) const
	{	return this->getImpl().toRef(state);	}
	Ref<CP::Var<bool> >	toCPVarRef(State& state) const
	{	return this->getImpl().toCPVarRef(state);	}
	Goal	toGoal(State& state) const
	{	return this->getImpl().toGoal(state);	}

	bool postDomFilter(CP::Store& store) const
	{	return this->getImpl().postDomFilter(store);	}
	bool postBndFilter(CP::Store& store) const
	{	return this->getImpl().postBndFilter(store);	}
	bool postValFilter(CP::Store& store) const
	{	return this->getImpl().postValFilter(store);	}
	State* const getPState() const
	{	return this->getImpl().getPState();	}

#endif

	std::string getTypeStr() const
	{	return this->getImpl().getTypeStr();	}

	// SWIG needs this
	~Expr() {}
};

template<class Eval>
struct Expr<Casper::Seq<Eval> > : Casper::Util::SPImplIdiom<Detail::IExpr<Casper::Seq<Eval> > >
{
	typedef Casper::Util::SPImplIdiom<Detail::IExpr<Casper::Seq<Eval> > > Super;

	template<class T>
	Expr(const T& t);

	Expr(const Expr& expr) : Super(expr) {}

#ifndef SWIG

	Util::StdArray<Eval,1> toLitStdArray() const
	{	return this->getImpl().toLitStdArray();	}

	Util::StdArray<Eval,2> toLitStdArray2() const
	{	return this->getImpl().toLitStdArray2();	}

	Util::StdArray<Expr<Eval> > toStdArray() const
	{	return this->getImpl().toStdArray();	}

	// \note Make sure *this is alive while using the returned IterationExpr
	IterationExpr<Expr<Eval> > toIterationExpr() const
	{	return this->getImpl().toIterationExpr();	}

	// \note Make sure *this is alive while using the returned ElementExpr
	Expr<Eval> getElement(uint i) const
	{	return this->getImpl().getElement(i);	}

	Expr<Seq<Eval> > getSeqElement(uint i) const
	{	return this->getImpl().getSeqElement(i);	}

//	operator const Util::StdArray<Eval,1>&() const
//	{	return toStdArray(); }
//	operator const Util::StdArray<Eval,2>&() const
//	{	return toStdArray2(); }
//
	State* const getPState() const
	{	return this->getImpl().getPState();	}

#endif

	bool hasSeqElement() const
	{	return this->getImpl().hasSeqElement();	}

	std::string getTypeStr() const
	{	return this->getImpl().getTypeStr();	}

	// SWIG needs this
	~Expr() {}
};

namespace Detail {
template<class Eval>
uint getIterationExprSize(const Expr<Seq<Eval> >& expr)
{
	uint c=0;
	for (auto e(expr.toIterationExpr()) ; e.valid(); e.iterate())
		++c;
	return c;
}
} // Detail

template<class T,int dims>
RefArray<T,dims>::RefArray(State& state,const Expr<Seq<T> >& expr) :
	Super(state,Detail::getIterationExprSize(expr))
{
	uint i = 0;
	for (auto e(expr.toIterationExpr()); e.valid(); e.iterate())
		::new (&Super::operator[](i++)) Ref<T>(e.value().toRef(state));

}

template<class Eval>
struct GetPState<Expr<Eval> >
{	State* operator()(const Expr<Eval>& e) { return e.getPState();	}};

} // Casper

template<class T>
std::ostream& operator<<(std::ostream& os,const Casper::Expr<T>& expr)
{	return expr.getImpl().print(os); }


#include <casper/kernel/spexpr/expr_wrapper.h>


#endif /* CASPER_KERNEL_OBJ_EXPR_H_ */

