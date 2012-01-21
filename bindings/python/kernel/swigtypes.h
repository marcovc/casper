
#ifndef CASPER_BINDINGS_PYTHON_KERNEL_SWIGTYPES_H
#define CASPER_BINDINGS_PYTHON_KERNEL_SWIGTYPES_H

#include <bindings/python/swigtypes.h>


namespace Casper {

template<class Eval>
struct SWIGKernelTypeInfo : ISWIGTypeInfo<Eval>
{
	swig_type_info * Ref;
	swig_type_info * Expr;

	SWIGKernelTypeInfo()
	{
		std::string eval = Traits::getTypeStr<Eval>();
		Ref =	SWIG_TypeQuery((std::string("Casper::Ref<")+eval+std::string(">*")).c_str());
		Expr = SWIG_TypeQuery((std::string("Casper::Expr<")+eval+std::string(">*")).c_str());
	}

	bool belongs(PyObject* pObj, int i = 0) const
	{
		void * argp = NULL;
		return SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, Ref, i)) or
			   SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, Expr, i));
	}

	Casper::Expr<Eval>* toExpr(PyObject* pObj, int i = 0) const
	{
		void * argp = NULL;
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, Ref, i)))
			return new Casper::Expr<Eval>(*static_cast<Casper::Ref<Eval>*>(argp));
		else
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, Expr, i)))
			return new Casper::Expr<Eval>(*static_cast<Casper::Expr<Eval>*>(argp));
		return NULL;
	}
};

template<>
struct SWIGKernelTypeInfo<bool> : ISWIGTypeInfo<bool>
{
	swig_type_info * Ref;
	swig_type_info * Goal;
	swig_type_info * Expr;

	SWIGKernelTypeInfo()
	{
		Ref =	SWIG_TypeQuery((std::string("Casper::Ref<bool>*")).c_str());
		Expr = SWIG_TypeQuery((std::string("Casper::Expr<bool>*")).c_str());
		Goal = SWIG_TypeQuery("Casper::Goal*");
	}

	bool belongs(PyObject* pObj, int i = 0) const
	{
		void * argp = NULL;
		return SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, Ref, i)) or
			   SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, Expr, i)) or
			   SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, Goal, i));
	}

	Casper::Expr<bool>* toExpr(PyObject* pObj, int i = 0) const
	{
		void * argp = NULL;
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, Ref, i)))
			return new Casper::Expr<bool>(*static_cast<Casper::Ref<bool>*>(argp));
		else
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, Goal, i)))
			return new Casper::Expr<bool>(*static_cast<Casper::Goal*>(argp));
		else
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, Expr, i)))
			return new Casper::Expr<bool>(*static_cast<Casper::Expr<bool>*>(argp));
		return NULL;
	}
};

template<class Eval>
struct SWIGKernelTypeInfo<Seq<Eval> > : ISWIGTypeInfo<Seq<Eval> >
{
	swig_type_info * SeqExpr;

	SWIGKernelTypeInfo()
	{
		std::string eval = Traits::getTypeStr<Eval>();
		SeqExpr =	SWIG_TypeQuery((std::string("Casper::Expr<Casper::Seq<")+eval+std::string("> >*")).c_str());
	}

	bool belongs(PyObject* pObj, int i = 0) const
	{
		void * argp = NULL;
		return SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, SeqExpr, i));
	}

	Casper::Expr<Seq<Eval> >* toExpr(PyObject* pObj, int i = 0) const
	{
		void * argp = NULL;
		if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, SeqExpr, i)))
			return new Casper::Expr<Seq<Eval> >(*static_cast<Casper::Expr<Seq<Eval> >*>(argp));
		return NULL;
	}
};

ISWIGTypeInfo<int>* 		pIntSWIGKernelTypeInfo = NULL;
ISWIGTypeInfo<bool>* 		pBoolSWIGKernelTypeInfo = NULL;
ISWIGTypeInfo<Seq<int> >* 	pIntSeqSWIGKernelTypeInfo = NULL;
ISWIGTypeInfo<Seq<bool> >* 	pBoolSeqSWIGKernelTypeInfo = NULL;

ISWIGTypeInfo<int>* 		pIntSWIGCPTypeInfo = NULL;
ISWIGTypeInfo<bool>* 		pBoolSWIGCPTypeInfo = NULL;
ISWIGTypeInfo<Seq<int> >* 	pIntSeqSWIGCPTypeInfo = NULL;
ISWIGTypeInfo<Seq<bool> >* 	pBoolSeqSWIGCPTypeInfo = NULL;

}

#endif
