/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2009 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_BINDINGS_XCSP_EXPRPARSER_H_
#define CASPER_BINDINGS_XCSP_EXPRPARSER_H_

struct ExprParser
{
   boost::xpressive::sregex integer,boolean,id,integerExpr,boolExpr,
	    eNeg,eAbs,eAdd,eSub,eMul,eDiv,eMod,ePow,eMin,eMax,eIf,
	    pNot,pAnd,pOr,pXOr,pIff,pEq,pNe,pGe,pGt,pLe,pLt;

  ExprParser();
  //casperbind::cpp::Expression
  casperbind::cpp::SharedSymbol parse(const std::string& s,
		  const std::map<std::string,casperbind::cpp::SharedSymbol>& mpars) const;
  casperbind::cpp::SharedSymbol parseIntExpression(const boost::xpressive::smatch&,
		  const std::map<std::string,casperbind::cpp::SharedSymbol>& mpars) const;
  casperbind::cpp::SharedSymbol parseBoolExpression(const boost::xpressive::smatch&,
		  const std::map<std::string,casperbind::cpp::SharedSymbol>& mpars) const;
};


#endif /* CASPER_BINDINGS_XCSP_EXPRPARSER_H_ */
