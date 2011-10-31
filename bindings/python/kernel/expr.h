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
namespace Detail {

namespace Traits {
template<class Eval> struct GetEvalTypeStr;

template<>
struct GetEvalTypeStr<bool>
{	std::string operator()() const { return "bool";	}	};

template<>
struct GetEvalTypeStr<int>
{	std::string operator()() const { return "int";	}	};

template<class T>
struct GetEvalTypeStr<Set<T> >
{	std::string operator()() const { return std::string("Casper::Set<")+GetEvalTypeStr<T>()()+std::string(">");	}	};

template<class T>
struct GetEvalTypeStr<Seq<T> >
{	std::string operator()() const { return std::string("Casper::Seq<")+GetEvalTypeStr<T>()()+std::string(">");	}	};

} // Traits

/*
 *	Creates from python literals, CP::Var, and Expr<Eval>.
 */
template<class Eval>
struct CreateFromPyObject
{
	static Casper::Expr<Eval>*	create(PyObject* pObj)
	{
		std::string evalStr = Traits::GetEvalTypeStr<Eval>()();

		const char* swigExprStr = (std::string("Casper::Expr<")+evalStr+std::string(">*")).c_str();
		const char* swigVarStr = (std::string("Casper::CP::Var<")+evalStr+
									std::string(",Casper::CP::Traits::GetDefaultDom<")+evalStr+
												std::string(">::Type>*")).c_str();

		static swig_type_info * swig_Expr_ptr = SWIG_TypeQuery(swigExprStr);
		static swig_type_info * swig_CPVar_ptr = SWIG_TypeQuery(swigVarStr);

		void * argp = NULL;

		if (PyBool_Check(pObj))
			return new Casper::Expr<Eval>(static_cast<Eval>(PyInt_AsLong(pObj)));
		else
		if (PyInt_Check(pObj))
			return new Casper::Expr<Eval>(static_cast<Eval>(PyInt_AsLong(pObj)));
		else
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_CPVar_ptr, 0)))
			return new Casper::Expr<Eval>(*static_cast<Casper::CP::Var<Eval,typename Casper::CP::Traits::GetDefaultDom<Eval>::Type>*>(argp));
		else
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_Expr_ptr, 0)))
			return new Casper::Expr<Eval>(*static_cast<Casper::Expr<Eval>*>(argp));
		else
		{
			std::cout << "unkown conversion in typemap\n";
			assert(0);
		}
	}

	static bool	check(PyObject* pObj)
	{
		std::string evalStr = Traits::GetEvalTypeStr<Eval>()();
		const char* swigExprStr = (std::string("Casper::Expr<")+evalStr+std::string(">*")).c_str();
		const char* swigVarStr = (std::string("Casper::CP::Var<")+evalStr+
									std::string(",Casper::CP::Traits::GetDefaultDom<")+evalStr+
												std::string(">::Type>*")).c_str();

		static swig_type_info * swig_Expr_ptr = SWIG_TypeQuery(swigExprStr);
		static swig_type_info * swig_CPVar_ptr = SWIG_TypeQuery(swigVarStr);

		void * argp = NULL;

		return 	SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_Expr_ptr, 0)) or
				SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_CPVar_ptr, 0)) or
				PyBool_Check(pObj) or
				PyInt_Check(pObj);
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
		std::string evalStr = Traits::GetEvalTypeStr<Eval>()();
		std::string seqEvalStr = std::string("Seq<")+evalStr+std::string(">");

		const char* swigExprStr = (std::string("Casper::Expr<")+seqEvalStr+std::string(">*")).c_str();
		const char* swigVarArray1Str = (std::string("Casper::CP::VarArray<")+evalStr+
									std::string(",1,Casper::CP::Traits::GetDefaultDom<")+evalStr+
									std::string(">::Type>*")).c_str();
		const char* swigVarArray2Str = (std::string("Casper::CP::VarArray<")+evalStr+
									std::string(",2,Casper::CP::Traits::GetDefaultDom<")+evalStr+
									std::string(">::Type>*")).c_str();

		static swig_type_info * swig_Expr_ptr = SWIG_TypeQuery(swigExprStr);
		static swig_type_info * swig_CPVarArray1_ptr = SWIG_TypeQuery(swigVarArray1Str);
		static swig_type_info * swig_CPVarArray2_ptr = SWIG_TypeQuery(swigVarArray2Str);

		void * argp = NULL;

		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_CPVarArray1_ptr, 0)))
			return new Casper::Expr<Seq<Eval> >(*static_cast<Casper::CP::VarArray<Eval,1>*>(argp));
		else
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_CPVarArray2_ptr, 0)))
			return new Casper::Expr<Seq<Eval> >(*static_cast<Casper::CP::VarArray<Eval,2>*>(argp));
		else
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_Expr_ptr, 0)))
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
		std::string evalStr = Traits::GetEvalTypeStr<Eval>()();
		std::string seqEvalStr = std::string("Seq<")+evalStr+std::string(">");

		const char* swigExprStr = (std::string("Casper::Expr<")+seqEvalStr+std::string(">*")).c_str();
		const char* swigVarArray1Str = (std::string("Casper::CP::VarArray<")+evalStr+
									std::string(",1,Casper::CP::Traits::GetDefaultDom<")+evalStr+
									std::string(">::Type>*")).c_str();
		const char* swigVarArray2Str = (std::string("Casper::CP::VarArray<")+evalStr+
									std::string(",2,Casper::CP::Traits::GetDefaultDom<")+evalStr+
									std::string(">::Type>*")).c_str();

		static swig_type_info * swig_Expr_ptr = SWIG_TypeQuery(swigExprStr);
		static swig_type_info * swig_CPVarArray1_ptr = SWIG_TypeQuery(swigVarArray1Str);
		static swig_type_info * swig_CPVarArray2_ptr = SWIG_TypeQuery(swigVarArray2Str);

		void * argp = NULL;

		return 	SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_Expr_ptr, 0)) or
				SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_CPVarArray1_ptr, 0)) or
				SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_CPVarArray2_ptr, 0)) or
				PySequence_Check(pObj);
	}
};

} // Detail


} // Casper

#endif /* CASPER_BINDINGS_PYTHON_KERNEL_EXPR_H_ */
