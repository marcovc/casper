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
 
#include <boost/xpressive/xpressive.hpp>
#include <libxml++/libxml++.h>
#include <bindings/cpp/cpp.h>
#include <bindings/cpp/print.h>

#include <iostream>

#include "exprparser.h"

using namespace boost::xpressive;
using namespace regex_constants;
using namespace Casperbind::cpp;

ExprParser::ExprParser()
{
	integer	= +digit;
	boolean	= as_xpr("true") | "false";
	id		= alpha >> *alnum;

	eNeg = "neg(" >> by_ref(integerExpr) >> ")";
	eAbs = "abs(" >> by_ref(integerExpr) >> ")";
	eAdd = "add(" >> by_ref(integerExpr) >> ","
									 >> by_ref(integerExpr) >> ")";
	eSub	= "sub(" >> by_ref(integerExpr) >> ","
									 >> by_ref(integerExpr) >> ")";
	eMul	= "mul(" >> by_ref(integerExpr) >> ","
									 >> by_ref(integerExpr) >> ")";
	eDiv	= "div(" >> by_ref(integerExpr) >> ","
									 >> by_ref(integerExpr) >> ")";
	eMod	= "mod(" >> by_ref(integerExpr) >> ","
									 >> by_ref(integerExpr) >> ")";
	ePow	= "pow(" >> by_ref(integerExpr) >> ","
									 >> by_ref(integerExpr) >> ")";
	eMin	= "min(" >> by_ref(integerExpr) >> ","
									 >> by_ref(integerExpr) >> ")";
	eMax	= "max(" >> by_ref(integerExpr) >> ","
									 >> by_ref(integerExpr) >> ")";
	eIf	= "if(" >>  by_ref(boolExpr) >> ","
									>> by_ref(integerExpr) >> ","
									>> by_ref(integerExpr)>> ")";

	integerExpr = integer | id | eNeg | eAbs | eAdd | eSub
			| eMul | eDiv | eMod | ePow | eMin | eMax | eIf;

	pNot= "not(" >> by_ref(boolExpr) >> ")";
	pAnd= "and(" >> by_ref(boolExpr) >> "," >> by_ref(boolExpr) >> ")";
	pOr	= "or(" >> by_ref(boolExpr) >> "," >> by_ref(boolExpr) >> ")";
	pXOr= "xor(" >> by_ref(boolExpr) >> ","	>> by_ref(boolExpr) >> ")";
	pIff= "iff(" >> by_ref(boolExpr) >> ","	>> by_ref(boolExpr) >> ")";
	pEq	= "eq(" >> integerExpr >> "," >> integerExpr >> ")";
	pNe	= "ne(" >> integerExpr >> "," >> integerExpr >> ")";
	pGe	= "ge(" >> integerExpr >> "," >> integerExpr >> ")";
	pGt	= "gt(" >> integerExpr >> "," >> integerExpr >> ")";
	pLe	= "le(" >> integerExpr >> "," >> integerExpr >> ")";
	pLt	= "lt(" >> integerExpr >> "," >> integerExpr >> ")";

	boolExpr = boolean | pNot | pAnd | pOr | pXOr | pIff | pEq
			| pNe | pGe | pGt | pLe | pLt;
}

SharedSymbol ExprParser::parse(const std::string& s,
		const std::map<std::string,casperbind::cpp::SharedSymbol>& mpars) const
{
	smatch what;
    if (!regex_match( s, what, boolExpr))
      throw xmlpp::validity_error("expected boolean expression or predicate : "+s);

    assert(what.nested_results().size()==1);

    return parseBoolExpression(what,mpars);
}

SharedSymbol ExprParser::parseBoolExpression(const smatch& what,
		const std::map<std::string,casperbind::cpp::SharedSymbol>& mpars) const
{
	assert(what.nested_results().size()==1);

	smatch::nested_results_type::const_iterator begin = what.nested_results().begin();
	smatch::nested_results_type::const_iterator end = what.nested_results().end();

	if (begin->regex_id() == boolean.regex_id())
	{
		if (begin->str() == "true")
			return int(1);
		else
		if (begin->str() == "false")
			return int(0);
		assert(0);
	}
	else
	if (begin->regex_id() == pNot.regex_id())
	{
		SymbolArray pars(1);
		pars[0] = parseBoolExpression(*begin->nested_results().begin(),mpars);
		return Predicate(Predicate::pNot,pars);
	}
	else
	if (begin->regex_id() == pAnd.regex_id() or
		begin->regex_id() == pOr.regex_id() or
		begin->regex_id() == pXOr.regex_id() or
		begin->regex_id() == pIff.regex_id())
	{
		SymbolArray pars(2);
		pars[0] = parseBoolExpression(*begin->nested_results().begin(),mpars);
		pars[1] = parseBoolExpression(*++begin->nested_results().begin(),mpars);
		if (begin->regex_id() == pAnd.regex_id())
			return Predicate(Predicate::pAnd,pars);
		else
		if (begin->regex_id() == pOr.regex_id())
			return Predicate(Predicate::pOr,pars);
		else
		if (begin->regex_id() == pXOr.regex_id())
			return Predicate(Predicate::pXOr,pars);
		else
		if (begin->regex_id() == pIff.regex_id())
			return Predicate(Predicate::pIff,pars);
	}
	else
	if (begin->regex_id() == pEq.regex_id() or
		begin->regex_id() == pNe.regex_id() or
		begin->regex_id() == pGe.regex_id() or
		begin->regex_id() == pGt.regex_id() or
		begin->regex_id() == pLe.regex_id() or
		begin->regex_id() == pLt.regex_id())
	{
		SymbolArray pars(2);
		pars[0] = parseIntExpression(*begin->nested_results().begin(),mpars);
		pars[1] = parseIntExpression(*++begin->nested_results().begin(),mpars);
		if (begin->regex_id() == pEq.regex_id())
			return Predicate(Predicate::pEqual,pars);
		else
		if (begin->regex_id() == pNe.regex_id())
			return Predicate(Predicate::pDistinct,pars);
		else
		if (begin->regex_id() == pGe.regex_id())
			return Predicate(Predicate::pGreaterEqual,pars);
		else
		if (begin->regex_id() == pGt.regex_id())
			return Predicate(Predicate::pGreater,pars);
		else
		if (begin->regex_id() == pLe.regex_id())
			return Predicate(Predicate::pLessEqual,pars);
		else
		if (begin->regex_id() == pLt.regex_id())
			return Predicate(Predicate::pLess,pars);
	}
	assert(0);
}


SharedSymbol ExprParser::parseIntExpression(const smatch& what,
		const std::map<std::string,casperbind::cpp::SharedSymbol>& mpars) const
{
	assert(what.nested_results().size()==1);

	smatch::nested_results_type::const_iterator begin = what.nested_results().begin();
	smatch::nested_results_type::const_iterator end = what.nested_results().end();

	assert(what.nested_results().size()==1);

	if (begin->regex_id() == integer.regex_id())
		return int(atoi(begin->str().c_str()));
	else
	if (begin->regex_id() == id.regex_id())
	{
		std::map<std::string,casperbind::cpp::SharedSymbol>::const_iterator
				it = mpars.find(begin->str());
		if (it==mpars.end())
			throw xmlpp::validity_error("undeclared variable appearing in expression : "+begin->str());
		return it->second;
	}
	else
	if (begin->regex_id() == eNeg.regex_id() or
		begin->regex_id() == eAbs.regex_id())
	{
		SymbolArray pars(1);
		pars[0] = parseIntExpression(*begin->nested_results().begin(),mpars);
		if (begin->regex_id() == eNeg.regex_id())
			return Expression(Expression::eSym,pars);
		else
		if (begin->regex_id() == eAbs.regex_id())
			return Expression(Expression::eAbs,pars);
	}
	else
	if (begin->regex_id() == eAdd.regex_id() or
		begin->regex_id() == eSub.regex_id() or
		begin->regex_id() == eMul.regex_id() or
		begin->regex_id() == eDiv.regex_id() or
		begin->regex_id() == eMod.regex_id() or
		begin->regex_id() == ePow.regex_id() or
		begin->regex_id() == eMin.regex_id() or
		begin->regex_id() == eMax.regex_id())
	{
		SymbolArray pars(2);
		pars[0] = parseIntExpression(*begin->nested_results().begin(),mpars);
		pars[1] = parseIntExpression(*++begin->nested_results().begin(),mpars);
		if (begin->regex_id() == eAdd.regex_id())
			return Expression(Expression::eAdd,pars);
		else
		if (begin->regex_id() == eSub.regex_id())
			return Expression(Expression::eSub,pars);
		else
		if (begin->regex_id() == eMul.regex_id())
			return Expression(Expression::eMul,pars);
		else
		if (begin->regex_id() == eDiv.regex_id())
			return Expression(Expression::eDiv,pars);
		else
		if (begin->regex_id() == eMod.regex_id())
			return Expression(Expression::eMod,pars);
		else
		if (begin->regex_id() == ePow.regex_id())
			return Expression(Expression::ePow,pars);
		else
		if (begin->regex_id() == eMin.regex_id())
			return Expression(Expression::eMin,pars);
		else
		if (begin->regex_id() == eMax.regex_id())
			return Expression(Expression::eMax,pars);
	}
	else
	if (begin->regex_id() == eIf.regex_id())
	{
		SymbolArray pars(3);
		pars[0] = parseBoolExpression(*begin->nested_results().begin(),mpars);
		pars[1] = parseIntExpression(*++begin->nested_results().begin(),mpars);
		pars[2] = parseIntExpression(*++(++begin->nested_results().begin()),mpars);
		return Expression(Expression::eIfThenElse,pars);
	}

	assert(0);
}

#if 0
int main(int argc, char** argv)
{
	std::map<std::string,casperbind::cpp::SharedSymbol> mpars;
	mpars["V0"] = casperbind::cpp::Variable(casperbind::cpp::IntDomain(casperbind::cpp::IntRange(1,10)));
	mpars["V1"] = casperbind::cpp::Variable(casperbind::cpp::IntDomain(casperbind::cpp::IntRange(2,4)));

	std::string s(argv[1]);
	std::cout << "input string is [" << s << "]" << std::endl;
	ExprParser e;
	SharedSymbol o = e.parse(s,mpars);
	std::cout << o << std::endl;
}
#endif
