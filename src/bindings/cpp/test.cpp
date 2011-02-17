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

#include <bindings/cpp/cpp.h>
#include <bindings/cpp/print.h>
#include <bindings/cpp/validate.h>
#include <iostream>

using namespace casperbind::cpp;
using namespace std;

void testIntArray()
{
	const int size = 3;
	int data[size] = {1,2,3};
	IntArray a(1,&size,data);
	cout << a << endl;
//	cout << validate(a) << endl;

	const int sizes[2] = {2,3};
	int data2[6] = { 1,2,3,4,5,6 };
	IntArray a2(2,sizes,data2);
	cout << a2 << endl;
//	cout << validate(a2) << endl;
}

void testIntSet()
{
	IntSet s(IntRange(1,10));
	cout << s << endl;
//	cout << validate(s) << endl;
}

void testVariable()
{
	Variable v(Int(1));
//	cout << validate(v) << endl;
	cout << v << endl;
}
/*
void testExpression()
{
	Expression e(Expression::eAdd,Variable(IntRange(1,10)),Int(10));
//	cout << validate(e) << endl;
	cout << e << endl;
}

void testPredicate()
{
	Expression e(Expression::eAdd,Variable(IntRange(1,10)),Int(10));
	SymbolArray s(2);
	s[0] = e;
	s[1] = Int(2);
	Predicate p(Predicate::pEqual,s);
	//cout << validate(p) << endl;
	cout << p << endl;
}
*/

int main()
{
	std::cout << SharedSymbol(Int(4)) << std::endl;
	//testIntArray();
/*	testIntSet();
	testVariable();
	testExpression();
	testPredicate();*/
}

