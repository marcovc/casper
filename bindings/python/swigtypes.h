/*
 * swigtypes.h
 *
 *  Created on: Jan 21, 2012
 *      Author: marco
 */

#ifndef CASPER_BINDINGS_PYTHON_SWIGTYPES_H_
#define CASPER_BINDINGS_PYTHON_SWIGTYPES_H_

#include <casper/kernel/spexpr/expr.h>

namespace Casper
{

template<class Eval>
struct ISWIGTypeInfo
{
	virtual bool belongs(PyObject* pObj, int i = 0) const = 0;
	virtual Casper::Expr<Eval>* toExpr(PyObject* pObj, int i = 0) const = 0;
};

extern ISWIGTypeInfo<int>* 			pIntSWIGKernelTypeInfo;
extern ISWIGTypeInfo<bool>* 		pBoolSWIGKernelTypeInfo;
extern ISWIGTypeInfo<Seq<int> >* 	pIntSeqSWIGKernelTypeInfo;
extern ISWIGTypeInfo<Seq<bool> >* 	pBoolSeqSWIGKernelTypeInfo;

extern ISWIGTypeInfo<int>* 			pIntSWIGCPTypeInfo;
extern ISWIGTypeInfo<bool>* 		pBoolSWIGCPTypeInfo;
extern ISWIGTypeInfo<Seq<int> >* 	pIntSeqSWIGCPTypeInfo;
extern ISWIGTypeInfo<Seq<bool> >* 	pBoolSeqSWIGCPTypeInfo;

template<class Eval>
ISWIGTypeInfo<Eval>* getPSWIGKernelTypeInfo();

template<>
ISWIGTypeInfo<int>* getPSWIGKernelTypeInfo()
{	return pIntSWIGKernelTypeInfo; }

template<>
ISWIGTypeInfo<bool>* getPSWIGKernelTypeInfo()
{	return pBoolSWIGKernelTypeInfo; }

template<>
ISWIGTypeInfo<Seq<int> >* getPSWIGKernelTypeInfo()
{	return pIntSeqSWIGKernelTypeInfo; }

template<>
ISWIGTypeInfo<Seq<bool> >* getPSWIGKernelTypeInfo()
{	return pBoolSeqSWIGKernelTypeInfo; }

template<class Eval>
ISWIGTypeInfo<Eval>* getPSWIGCPTypeInfo();

template<>
ISWIGTypeInfo<int>* getPSWIGCPTypeInfo()
{	return pIntSWIGCPTypeInfo; }

template<>
ISWIGTypeInfo<bool>* getPSWIGCPTypeInfo()
{	return pBoolSWIGCPTypeInfo; }

template<>
ISWIGTypeInfo<Seq<int> >* getPSWIGCPTypeInfo()
{	return pIntSeqSWIGCPTypeInfo; }

template<>
ISWIGTypeInfo<Seq<bool> >* getPSWIGCPTypeInfo()
{	return pBoolSeqSWIGCPTypeInfo; }



template<class Eval>
ISWIGTypeInfo<Eval>& getSWIGKernelTypeInfo()
{
	ISWIGTypeInfo<Eval>* pSWIGTypeInfo = getPSWIGKernelTypeInfo<Eval>();
	assert(pSWIGTypeInfo!=NULL);
	return *pSWIGTypeInfo;
}

template<class Eval>
ISWIGTypeInfo<Eval>& getSWIGCPTypeInfo()
{
	ISWIGTypeInfo<Eval>* pSWIGTypeInfo = getPSWIGCPTypeInfo<Eval>();
	assert(pSWIGTypeInfo!=NULL);
	return *pSWIGTypeInfo;
}

} // Casper


#endif /* CASPER_BINDINGS_PYTHON_SWIGTYPES_H_ */
