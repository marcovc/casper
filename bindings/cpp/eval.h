/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2010-2010 - Marco Correia <marco.v.correia@gmail.com>           *
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
 
#ifndef CASPERBIND_CPP_EVAL_H_
#define CASPERBIND_CPP_EVAL_H_

#include "symbol.h"

namespace Casperbind {
namespace cpp {

enum Eval { eBool,eInt, eReal,
			eIntSet,eBoolSet,
			eIntRange,eRealRange,eIntSetRange,eBoolSetRange,
		    eIntArray, eRealArray,eBoolArray,
		    eBoolVar, eIntVar, eRealVar,
		    eIntSetVar,eBoolSetVar,
		    eIntVarArray, eRealVarArray,eBoolVarArray,
		    eIntSetVarArray,eBoolSetVarArray,
		    eBoolExpr,eIntExpr,eRealExpr,
		    eIntSetExpr,eBoolSetExpr
			};


Eval getEval(const Symbol& s);

#define CASPERBIND_CPP_EVAL_SEQ (eBool)(eInt)(eReal)\
								(eIntSet)(eBoolSet)\
								(eIntRange)(eRealRange)\
								(eIntArray)(eRealArray)(eBoolArray)\
								(eBoolVar)(eIntVar)(eRealVar)\
								(eIntSetVar)(eBoolSetVar)\
								(eIntVarArray)(eRealVarArray)(eBoolVarArray)\
								(eIntSetVarArray)(eBoolSetVarArray)\
							    (eBoolExpr)(eIntExpr)(eRealExpr)\
							    (eIntSetExpr)(eBoolSetExpr)



}
}

#endif /* CASPERBIND_CPP_EVAL_H_ */
