/*
 * variable.h
 *
 *  Created on: Oct 29, 2011
 *      Author: marco
 */

#ifndef CASPER_KERNEL_OBJ_VARIABLE_H_
#define CASPER_KERNEL_OBJ_VARIABLE_H_

#include <casper/cp/int/var.h>

namespace Casper {


namespace CP {
namespace Detail {


template<FD_TP_SPEC(1)>
FD<FD_TP_LIST(1)>* VarDomCreator<FD<FD_TP_LIST(1)> >::operator()(Store& store,const Expr<T1>& expr)
{	return VarDomCreator<FD<FD_TP_LIST(1)> >::operator()(store,expr.toLiteral());	}

inline BD* VarDomCreator<BD>::operator()(Store& store,const Expr<bool>& expr)
{	return VarDomCreator<BD>::operator()(store,expr.toLiteral());	}

} // Detail
} // CP
} // Casper

#endif /* CASPER_KERNEL_OBJ_VARIABLE_H_ */
