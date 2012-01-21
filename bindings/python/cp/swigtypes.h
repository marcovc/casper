
#ifndef CASPER_BINDINGS_PYTHON_CP_SWIGTYPES_H
#define CASPER_BINDINGS_PYTHON_CP_SWIGTYPES_H

#include <bindings/python/swigtypes.h>

namespace Casper {

template<class Eval>
struct SWIGCPTypeInfo : ISWIGTypeInfo<Eval>
{
	swig_type_info * Var;

	SWIGCPTypeInfo()
	{
		std::string eval = Traits::getTypeStr<Eval>();
		Var =  SWIG_TypeQuery((std::string("Casper::CP::Var<")+eval+",Casper::CP::Traits::GetDefaultDom<"+eval+">::Type>*").c_str());
	}

	bool belongs(PyObject* pObj, int i = 0) const
	{
		void * argp = NULL;
		return SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, Var, i));
	}

	Casper::Expr<Eval>* toExpr(PyObject* pObj, int i = 0) const
	{
		void* argp = NULL;
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, Var, i)))
			return new Casper::Expr<Eval>(*static_cast<Casper::CP::Var<Eval,typename Casper::CP::Traits::GetDefaultDom<Eval>::Type>*>(argp));
		return NULL;
	}
};

template<class Eval>
struct SWIGCPTypeInfo<Seq<Eval> > : ISWIGTypeInfo<Seq<Eval> >
{
	swig_type_info * VarArray1;
	swig_type_info * VarArray2;
	swig_type_info * VarArray3;

	SWIGCPTypeInfo()
	{
		std::string eval = Traits::getTypeStr<Eval>();

		VarArray1 = SWIG_TypeQuery(
								(std::string("Casper::CP::VarArray<")+eval+
								std::string(",1,Casper::CP::Traits::GetDefaultDom<")+
								eval+
								std::string(">::Type>*")).c_str());
		VarArray2 = SWIG_TypeQuery(
								(std::string("Casper::CP::VarArray<")+
								eval+
								std::string(",2,Casper::CP::Traits::GetDefaultDom<")+
								eval+
								std::string(">::Type>*")).c_str());
		VarArray3 = SWIG_TypeQuery(
								(std::string("Casper::CP::VarArray<")+
								eval+
								std::string(",3,Casper::CP::Traits::GetDefaultDom<")+
								eval+
								std::string(">::Type>*")).c_str());
	}

	bool belongs(PyObject* pObj, int i = 0) const
	{
		void * argp = NULL;
		return SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, VarArray1, i)) or
				SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, VarArray2, i)) or
				SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, VarArray3, i));
	}

	Casper::Expr<Seq<Eval> >* toExpr(PyObject* pObj, int i = 0) const
	{
		void* argp = NULL;
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, VarArray1, i)))
			return new Casper::Expr<Seq<Eval> >(*static_cast<Casper::CP::VarArray<Eval,1,typename Casper::CP::Traits::GetDefaultDom<Eval>::Type>*>(argp));
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, VarArray2, i)))
			return new Casper::Expr<Seq<Eval> >(*static_cast<Casper::CP::VarArray<Eval,2,typename Casper::CP::Traits::GetDefaultDom<Eval>::Type>*>(argp));
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, VarArray3, i)))
			return new Casper::Expr<Seq<Eval> >(*static_cast<Casper::CP::VarArray<Eval,3,typename Casper::CP::Traits::GetDefaultDom<Eval>::Type>*>(argp));
		return NULL;
	}
};




}

#endif
