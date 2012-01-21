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
 

#ifndef CASPER_BINDINGS_PYTHON_KERNEL_EXPR_H_
#define CASPER_BINDINGS_PYTHON_KERNEL_EXPR_H_

#include <casper/kernel/spexpr/expr.h>
#include <string>

#include <bindings/python/swigtypes.h>

namespace Casper {


template<class Eval>
bool isCasperObj(PyObject* pObj)
{
	return (getPSWIGKernelTypeInfo<Eval>()!=NULL and
			getPSWIGKernelTypeInfo<Eval>()->belongs(pObj)) or
		   (getPSWIGCPTypeInfo<Eval>()!=NULL and
			getPSWIGCPTypeInfo<Eval>()->belongs(pObj));
}

template<class Eval>
Casper::Expr<Eval>* toCasperExpr(PyObject* pObj)
{
	Casper::Expr<Eval>* r = NULL;
	if (getPSWIGCPTypeInfo<Eval>() != NULL and
		(r = getPSWIGCPTypeInfo<Eval>()->toExpr(pObj)) != NULL)
		return r;
	if (getPSWIGKernelTypeInfo<Eval>() != NULL and
		(r = getPSWIGKernelTypeInfo<Eval>()->toExpr(pObj)) != NULL)
		return r;
	return r;
}



namespace Exception {
struct PyCodeException : std::runtime_error
{
	PyCodeException() : std::runtime_error("exception in python callback") {}
};
};

namespace Detail {

struct DecPyRef : ITrailAgent
{
	DecPyRef(PyObject* pFunction) :
		pFunction(pFunction)
	{}
	void restore()
	{	Py_XDECREF(pFunction);	}
	PyObject* pFunction;
};

template<class T>
struct CallableRef : IRef<T>
{
	CallableRef(State& state, PyObject* pFunction) :
		pFunction(pFunction)
	{
		Py_XINCREF(pFunction);
		state.record(new (state) DecPyRef(pFunction));
	}
	T value() const
	{
		PyObject *result = NULL;
		result = PyObject_CallObject(pFunction, NULL);
		if (result==NULL)
			throw Casper::Exception::PyCodeException();
		if (PyBool_Check(result) or PyInt_Check(result))
			return static_cast<T>(PyInt_AsLong(result));
		else
			assert(0);
	}
	PyObject* pFunction;
};


template<class Eval>
struct CallableRef<CP::Var<Eval> > : IRef<CP::Var<Eval>  >
{
	CallableRef(State& state, PyObject* pFunction) :
		state(state),pFunction(pFunction)
	{
		Py_XINCREF(pFunction);
		state.record(new (state) DecPyRef(pFunction));
	}
	CP::Var<Eval> value() const;
	State& state;
	PyObject* pFunction;
};

struct CallableGoal : IGoal
{
	CallableGoal(State& state, PyObject* pFunction) :
		state(state),pFunction(pFunction)
	{
		Py_XINCREF(pFunction);
		state.record(new (state) DecPyRef(pFunction));
	}
	Goal execute();
	State& state;
	PyObject* pFunction;
};

template<class Eval>
struct ExprFromCallable : IExpr<Eval>
{
	ExprFromCallable(PyObject* pObj)  : pFunction(pObj)
	{
		Py_XINCREF(pFunction);
	}

	~ExprFromCallable()
	{
		Py_XDECREF(pFunction);
	}

	Ref<Eval>	toRef(State& state) const
	{	return Ref<Eval>(state,static_cast<IRef<Eval>*>(new (state) CallableRef<Eval>(state,pFunction)));	}
	Ref<CP::Var<Eval> >	toCPVarRef(State& state) const
	{	return Ref<CP::Var<Eval> >(state,static_cast<IRef<CP::Var<Eval> >*>(new (state) CallableRef<CP::Var<Eval> >(state,pFunction)));	}
	std::ostream& print(std::ostream& os) const
	{	return os << "<python callable>"; }
	std::string getTypeStr() const
	{	return "<Python Callable>";	}

	Eval toLiteral() const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Eval");	}
	CP::DomExpr<Eval> toCPDomExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::DomExpr<Eval>");	}
	CP::BndExpr<Eval> toCPBndExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::BndExpr<Eval>");	}
	CP::ValExpr<Eval> toCPValExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::ValExpr<Eval>");	}
	State* const getPState() const
	{	return NULL;	}

	PyObject *pFunction;
};

// For now only functions without parameters are supported
template<>
struct ExprFromCallable<bool> : IExpr<bool>
{
	ExprFromCallable(PyObject* pObj) : pFunction(pObj)
	{
		Py_XINCREF(pFunction);
	}

	~ExprFromCallable()
	{
		Py_XDECREF(pFunction);
	}

	Ref<bool>	toRef(State& state) const
	{	return Ref<bool>(state,static_cast<IRef<bool>*>(new (state) CallableRef<bool>(state,pFunction)));	}
	Ref<CP::Var<bool> >	toCPVarRef(State& state) const
	{	return Ref<CP::Var<bool> >(state,static_cast<IRef<CP::Var<bool> >*>(new (state) CallableRef<CP::Var<bool> >(state,pFunction)));	}
	Goal	toGoal(State& state) const
	{	return new (state) CallableGoal(state,pFunction);	}
	std::ostream& print(std::ostream& os) const
	{	return os << "<python callable>"; }
	std::string getTypeStr() const
	{	return "<Python Callable>";	}

	bool toLiteral() const
	{	throw Casper::Exception::TypeCoercion("<python callable>","bool");	}
	CP::DomExpr<bool> toCPDomExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::DomExpr<bool>");	}
	CP::BndExpr<bool> toCPBndExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::BndExpr<bool>");	}
	CP::ValExpr<bool> toCPValExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::ValExpr<bool>");	}
	CP::ChkExpr toChkExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::ChkExpr<bool>");	}

	bool postDomFilter(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::Filter(Domain)");	}
	bool postBndFilter(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::Filter(Bounds)");	}
	bool postValFilter(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::Filter(Value)");	}

	State* const getPState() const
	{	return NULL; }

	PyObject *pFunction;
};


/*
 *	Creates from python literals, CP::Var, and Expr<Eval>.
 */
template<class Eval>
struct CreateFromPyObject
{
	static Casper::Expr<Eval>*	create(PyObject* pObj)
	{
		if (PyBool_Check(pObj))
			return new Casper::Expr<Eval>(static_cast<Eval>(PyInt_AsLong(pObj)));
		if (PyInt_Check(pObj))
			return  new Casper::Expr<Eval>(static_cast<Eval>(PyInt_AsLong(pObj)));
		if (PyCallable_Check(pObj))
			return  new Casper::Expr<Eval>(static_cast<Detail::IExpr<Eval>*>(new ExprFromCallable<Eval>(pObj)));
		Casper::Expr<Eval>* r;
		if ((r = toCasperExpr<Eval>(pObj)) != NULL)
			return r;
		throw Casper::Exception::TypeCoercion("<Python Object>",
											 Casper::Traits::getTypeStr<Casper::Expr<Eval> >());
	}

	static bool	check(PyObject* pObj)
	{
		return	PyBool_Check(pObj) or
				PyInt_Check(pObj) or
				PyCallable_Check(pObj) or
				isCasperObj<Eval>(pObj);
	}
};



/*
 *	Creates from python sequence types, CP::VarArray, and Expr<Eval>.
 */
template<class Eval>
struct CreateFromPyObject<Seq<Eval> >
{
	static Casper::Expr<Seq<Eval> >*	create(PyObject* pObj)
	{
		Casper::Expr<Seq<Eval> >* r;
		if ((r = toCasperExpr<Seq<Eval> >(pObj)) != NULL)
			return r;

		if (PySequence_Check(pObj))
		{
			int size = PySequence_Size(pObj);

			Casper::Util::StdList<Expr<Eval> > l;

			for (int i = 0; i < size; ++i)
			{
				Expr<Eval>* pExpr = CreateFromPyObject<Eval>::create(PySequence_GetItem(pObj, i));
				l.pushBack(Expr<Eval>(*pExpr));
				delete pExpr;
			}
			return new Casper::Expr<Seq<Eval> >(l);
		}

		throw Casper::Exception::TypeCoercion("<Python Object>",
												 Casper::Traits::getTypeStr<Casper::Expr<Seq<Eval> > >());
	}

	static bool	check(PyObject* pObj)
	{
		return 	isCasperObj<Seq<Eval> >(pObj) or
				PySequence_Check(pObj);
	}
};

Goal CallableGoal::execute()
{
	PyObject *result = NULL;
	result = PyObject_CallObject(pFunction, NULL);

	assert(CreateFromPyObject<bool>::check(result));
	Expr<bool>* pRes = CreateFromPyObject<bool>::create(result);
	Goal ret(pRes->toGoal(state));
	delete pRes;
	return ret;

/*		bool r;
	if (PyBool_Check(result) or PyInt_Check(result))		// FIXME: should cast as Goal
		r = static_cast<bool>(PyInt_AsLong(result));
	else
		assert(0);
	return Goal(state,r);
*/

}


template<class Eval>
CP::Var<Eval> CallableRef<Casper::CP::Var<Eval> >::value() const
{
	PyObject *result = NULL;
	result = PyObject_CallObject(pFunction, NULL);
	if (result==NULL)
		throw Casper::Exception::PyCodeException();
	assert(CreateFromPyObject<Eval>::check(result));
	Casper::Expr<Eval>* r = CreateFromPyObject<Eval>::create(result);
	Casper::Expr<Eval> rr = *r;
	delete r;
	return rr.toCPVarRef(state).value();
}

} // Detail


} // Casper

#endif /* CASPER_BINDINGS_PYTHON_KERNEL_EXPR_H_ */
