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

#include <casper/kernel/obj/expr.h>
#include <string>

namespace Casper {

namespace Exception {
struct PyCodeException : std::runtime_error
{
	PyCodeException() : std::runtime_error("exception in python callback") {}
};
};

namespace Detail {

template<class>
struct EvalStr;

template<>
struct EvalStr<int>
{	std::string operator()() { return "int"; }};

template<>
struct EvalStr<bool>
{	std::string operator()() { return "bool"; }};

template<class T>
struct EvalStr<Seq<T> >
{	std::string operator()() { return "Casper::Seq<" + EvalStr<T>()() + ">"; }};

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
struct CallablePar : IPar<T>
{
	CallablePar(State& state, PyObject* pFunction) :
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
struct CallablePar<CP::Var<Eval> > : IPar<CP::Var<Eval>  >
{
	CallablePar(State& state, PyObject* pFunction) :
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

	Par<Eval>	toPar(State& state) const
	{	return Par<Eval>(state,static_cast<IPar<Eval>*>(new (state) CallablePar<Eval>(state,pFunction)));	}
	Par<CP::Var<Eval> >	toCPVarPar(State& state) const
	{	return Par<CP::Var<Eval> >(state,static_cast<IPar<CP::Var<Eval> >*>(new (state) CallablePar<CP::Var<Eval> >(state,pFunction)));	}
	std::ostream& print(std::ostream& os) const
	{	return os << "<python callable>"; }

	Eval toLiteral() const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Eval");	}
	CP::DomExpr<Eval> toDomExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::DomExpr<Eval>");	}
	CP::BndExpr<Eval> toBndExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::BndExpr<Eval>");	}
	CP::ValExpr<Eval> toValExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::ValExpr<Eval>");	}

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

	Par<bool>	toPar(State& state) const
	{	return Par<bool>(state,static_cast<IPar<bool>*>(new (state) CallablePar<bool>(state,pFunction)));	}
	Par<CP::Var<bool> >	toCPVarPar(State& state) const
	{	return Par<CP::Var<bool> >(state,static_cast<IPar<CP::Var<bool> >*>(new (state) CallablePar<CP::Var<bool> >(state,pFunction)));	}
	Goal	toGoal(State& state) const
	{	return new (state) CallableGoal(state,pFunction);	}
	std::ostream& print(std::ostream& os) const
	{	return os << "<python callable>"; }

	bool toLiteral() const
	{	throw Casper::Exception::TypeCoercion("<python callable>","bool");	}
	CP::DomExpr<bool> toDomExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::DomExpr<bool>");	}
	CP::BndExpr<bool> toBndExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::BndExpr<bool>");	}
	CP::ValExpr<bool> toValExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::ValExpr<bool>");	}
	CP::ChkExpr toChkExpr(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::CP::ChkExpr<bool>");	}

	bool postDomFilter(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::Filter(Domain)");	}
	bool postBndFilter(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::Filter(Bounds)");	}
	bool postValFilter(CP::Store& store) const
	{	throw Casper::Exception::TypeCoercion("<python callable>","Casper::Filter(Value)");	}

	PyObject *pFunction;
};


/*
 *	Creates from python literals, CP::Var, and Expr<Eval>.
 */
template<class Eval>
struct CreateFromPyObject
{
	static swig_type_info * isCPVar;
	static swig_type_info * isPar;
	static swig_type_info * isExpr;
	static swig_type_info * isGoal;

	CreateFromPyObject()
	{
		isCPVar =  SWIG_TypeQuery((std::string("Casper::CP::Var<")+
								   Detail::EvalStr<Eval>()()+
								   std::string(">*")).c_str());
		isPar =	SWIG_TypeQuery((std::string("Casper::Par<")+
								 Detail::EvalStr<Eval>()()+
								 std::string(">*")).c_str());
		isExpr = SWIG_TypeQuery((std::string("Casper::Expr<")+
								Detail::EvalStr<Eval>()()+
								std::string(">*")).c_str());
		isExpr = SWIG_TypeQuery("Casper::Goal*");

	}
	static Casper::Expr<Eval>*	create(PyObject* pObj)
	{
		void * argp = NULL;

		if (PyBool_Check(pObj))
			return new Casper::Expr<Eval>(static_cast<Eval>(PyInt_AsLong(pObj)));
		else
		if (PyInt_Check(pObj))
			return new Casper::Expr<Eval>(static_cast<Eval>(PyInt_AsLong(pObj)));
		else
		if (PyCallable_Check(pObj))
			return new Casper::Expr<Eval>(static_cast<Detail::IExpr<Eval>*>(new ExprFromCallable<Eval>(pObj)));
		else
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isCPVar, 0)))
			return new Casper::Expr<Eval>(*static_cast<Casper::CP::Var<Eval,typename Casper::CP::Traits::GetDefaultDom<Eval>::Type>*>(argp));
		else
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isPar, 0)))
			return new Casper::Expr<Eval>(*static_cast<Casper::Par<Eval>*>(argp));
		else
		if (Detail::EvalStr<Eval>()()=="bool" and
			SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isGoal, 0)))
			return new Casper::Expr<Eval>(*static_cast<Casper::Goal*>(argp));
		else
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isExpr, 0)))
			return new Casper::Expr<Eval>(*static_cast<Casper::Expr<Eval>*>(argp));
		else
		{
			std::cout << "unkown conversion in typemap\n";
			assert(0);
		}
	}

	static bool	check(PyObject* pObj)
	{
		void * argp = NULL;

		return 	SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isCPVar, 0)) or
				SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isPar, 0)) or
				SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isExpr, 0)) or
				(SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isGoal, 0))) or
				PyCallable_Check(pObj) or
				PyBool_Check(pObj) or
				PyInt_Check(pObj);
	}
};

template<class T> swig_type_info * CreateFromPyObject<T>::isCPVar = NULL;
template<class T> swig_type_info * CreateFromPyObject<T>::isPar = NULL;
template<class T> swig_type_info * CreateFromPyObject<T>::isExpr = NULL;
template<class T> swig_type_info * CreateFromPyObject<T>::isGoal = NULL;

/*
 *	Creates from python sequence types, CP::VarArray, and Expr<Eval>.
 */
template<class Eval>
struct CreateFromPyObject<Seq<Eval> >
{
	static swig_type_info * isCPVarArray1;
	static swig_type_info * isCPVarArray2;
	static swig_type_info * isSeqExpr;

	CreateFromPyObject()
	{
		isCPVarArray1 =
				SWIG_TypeQuery(
						(std::string("Casper::CP::VarArray<")+Detail::EvalStr<Eval>()()+
						std::string(",1,Casper::CP::Traits::GetDefaultDom<")+
						Detail::EvalStr<Eval>()()+
						std::string(">::Type>*")).c_str());
		isCPVarArray2 =
				SWIG_TypeQuery(
						(std::string("Casper::CP::VarArray<")+
						Detail::EvalStr<Eval>()()+
						std::string(",2,Casper::CP::Traits::GetDefaultDom<")+
						Detail::EvalStr<Eval>()()+
						std::string(">::Type>*")).c_str());
		isSeqExpr =
				SWIG_TypeQuery(
						(std::string("Casper::Expr<")+
						Detail::EvalStr<Eval>()()+
						std::string(">*")).c_str());

	}
	static Casper::Expr<Seq<Eval> >*	create(PyObject* pObj)
	{
		void * argp = NULL;

		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isCPVarArray1, 0)))
			return new Casper::Expr<Seq<Eval> >(*static_cast<Casper::CP::VarArray<Eval,1>*>(argp));
		else
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isCPVarArray2, 0)))
			return new Casper::Expr<Seq<Eval> >(*static_cast<Casper::CP::VarArray<Eval,2>*>(argp));
		else
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isSeqExpr, 0)))
			return new Casper::Expr<Seq<Eval> >(*static_cast<Casper::Expr<Seq<Eval> >*>(argp));
		else
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
		else
		{
			std::cout << "unkown conversion in typemap" << std::endl;
			assert(0);
		}
	}

	static bool	check(PyObject* pObj)
	{

		void * argp = NULL;

		return 	SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isCPVarArray1, 0)) or
				SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isCPVarArray2, 0)) or
				SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, isSeqExpr, 0)) or
				PySequence_Check(pObj);
	}
};

template<class T> swig_type_info * CreateFromPyObject<Seq<T> >::isCPVarArray1 = NULL;
template<class T> swig_type_info * CreateFromPyObject<Seq<T> >::isCPVarArray2 = NULL;
template<class T> swig_type_info * CreateFromPyObject<Seq<T> >::isSeqExpr = NULL;


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
CP::Var<Eval> CallablePar<Casper::CP::Var<Eval> >::value() const
{
	PyObject *result = NULL;
	result = PyObject_CallObject(pFunction, NULL);
	if (result==NULL)
		throw Casper::Exception::PyCodeException();
	assert(CreateFromPyObject<Eval>::check(result));
	Casper::Expr<Eval>* r = CreateFromPyObject<Eval>::create(result);
	Casper::Expr<Eval> rr = *r;
	delete r;
	return rr.toCPVarPar(state).value();
}

} // Detail


} // Casper

#endif /* CASPER_BINDINGS_PYTHON_KERNEL_EXPR_H_ */
