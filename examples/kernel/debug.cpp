/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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


#include <casper/kernel.h>
#include <casper/cp/int.h>
#include <casper/cp.h>

#include <casper/kernel/spexpr/expr.h>

#include <iostream>

namespace Casper {
namespace CP {

// element among Expr<Seq<Eval> >
template<class Eval>
struct BndViewRel2<Element,Expr<Seq<Eval> >,Expr<int>,Eval> :
		BndView<Eval,Expr<Eval> >
{
	CASPER_ASSERT_BNDVIEW_EVAL(Eval)

	typedef BndView<Eval,Expr<Eval> >	Super;
	typedef Var<Eval,typename Traits::GetDefaultDom<Eval>::Type> Elem;
	static Super getSuper(Store& s,const Expr<Seq<Eval> >& p1,const Expr<int>& p2)
	{
		if (ValView<int,Expr<int> >(s,p2).ground())
		{
			int idx = ValView<int,Expr<int> >(s,p2).value();

			ElementView<Expr<Seq<Eval> > > elem(p1);
			return Super(s,elem.get(idx));
		}

		Elem v(s,Detail::VarDomCreator<typename Elem::Dom>().unionOf(s,p1));
		s.post(elementEqual(p1,p2,v));
		return Super(s,v);
	}

	Rel2<Element,Expr<Seq<Eval> >,Expr<int> > getObj() const
	{	return Rel2<Element,Expr<Seq<Eval> >,Expr<int> >(v1,v2); }

	BndViewRel2(Store& store,const Expr<Seq<Eval> >& p1, const Expr<int>& p2) :
		Super(getSuper(store,p1,p2)),
		v1(p1),v2(p2)
	{}

	Expr<Seq<Eval> > v1;
	Expr<int> v2;
};

} // CP

} // Casper

using namespace Casper;
using namespace Casper::CP;
using namespace std;

template<class T>
BndArrayView<int,T> newBndArrayView(Solver& solver, const T& t)
{	return BndArrayView<int,T>(solver,t); }

void test1()
{
	Solver solver;

	IntVarArray y(solver,2,0,3);
	IntVarArray2 x(solver,3,2,1,3);

	Expr<int> i(0);
	Expr<int> j(1);
	//IntVar i(solver,0,1);

	Expr<Seq<int> > ex(x);

	cout << ex.toElementSeqExpr().get(1) << endl;

//	Expr<Seq<int> > r1(rel<Element>(ex,i));
//	Expr<int> r(rel<Element>(r1,j));

//	BndExpr<int> b(solver,r);
//	cout << b << endl;

//	Expr<bool> f(rel<Equal>(r,1));
//	bool found  = solver.post(f);
//	found = found and solver.solve(label(solver,x));
//	while (found)
//	{
//		cout << i << " " << x << endl;
//		found = solver.next();
//	}

}


int main()
{
	test1();
}

