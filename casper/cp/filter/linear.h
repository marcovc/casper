/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2012-2012 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_CP_FILTER_LINEAR_H_
#define CASPER_CP_FILTER_LINEAR_H_

namespace Casper {
namespace CP {

/**
 *	Enforces the constraint \f$\sum_{i=1}^{n}c_{i}x_{i}=b\f$.
 *	\tparam Expr1 The sequence of coefficients \f$c_1\ldots c_n\f$
 *	\tparam Expr2 The sequence of variables \f$x_1\ldots x_n\f$
 *	\tparam Expr3 \f$b\f$.
 *
 *	Propagator from
 *  "Yuanlin,Yap, Arc-consistency on n-ary monotonic and linear constraints"
 *	\note Requires Expr1 to be ground.
 *  \todo: improve using index deltas
 *  \todo: make sure it also works for reals
 *  \todo: optimize (see LinearLessEqual)
 *  \ingroup Filtering
 */
template<class Eval,class Expr1,class Expr2,class Expr3>
struct BndFilterView3<LinearEqual,Seq<Eval>,Expr1,Seq<Eval>,Expr2,Eval,Expr3> : IFilter
{
	BndFilterView3(Store& store,const Expr1& v1, const Expr2& v2,
					const Expr3& v3) :
						IFilter(store),c(store,v1),x(store,v2),v(store,v3),
					f(store,rel<Mul>(c[0].value(),x[0].getObj()))
					#ifdef CASPER_LOG
					,store(store)
					#endif

	{
		for (uint i = 1; i < c.size(); i++)
			f = f + rel<Mul>(c[i].value(),x[i].getObj());
	}
	bool execute()
	{
		#ifdef CASPER_LOG
		store.getEnv().log(this, "BndFilterView3<LinearEqual,Seq<Eval>,Expr1,Seq<Eval>,Expr2,Eval,Expr3>", Util::Logger::filterExecuteBegin);
		#endif

		Eval fmin = f.min();
		Eval fmax = f.max();
		if (!v.updateRange(fmin,fmax))
			return false;
		fmin = fmin-v.max();
		fmax = fmax-v.min();

		for (uint i = 0; i < c.size(); i++)
		{
			Eval lb = limits<Eval>::negInf();
			Eval ub = limits<Eval>::posInf();
			Detail::mulRange(range(c[i].value(),c[i].value()),
							 range(x[i].min(),x[i].max()),lb,ub);
			Eval lb1 = x[i].min();
			Eval ub1 = x[i].max();
			Detail::divRange(range(ub-fmax,lb-fmin),
							 range(c[i].value(),c[i].value()),lb1,ub1);
			Eval oldxmin = x[i].min();
			Eval oldxmax = x[i].max();
			if (oldxmin < lb1 or oldxmax > ub1)
			{
				if (!x[i].updateRange(lb1,ub1))
					return false;
				fmin = f.min()-v.max();
				fmax = f.max()-v.min();
			}
		}
		return true;
	}
	Cost cost() const {	return linearLo; }
	void attach(INotifiable* s)
	{	x.attach(s); v.attach(s);	}
	void detach()
	{	x.detach(); v.detach();	}

	ValArrayView<Eval,Expr1> c;
	BndArrayView<Eval,Expr2> x;
	BndView<Eval,Expr3>		 v;
	BndExpr<Eval>			 f;
	#ifdef CASPER_LOG
	Store& store;
	#endif
};

/**
 *	Enforces the constraint \f$\sum_{i=1}^{n}c_{i}x_{i}>=b\f$.
 *	\tparam Expr1 The sequence of coefficients \f$c_1\ldots c_n\f$
 *	\tparam Expr2 The sequence of variables \f$x_1\ldots x_n\f$
 *	\tparam Expr3 \f$b\f$.
 *
 *	Propagator from
 *  "Yuanlin,Yap, Arc-consistency on n-ary monotonic and linear constraints"
 *	\note Requires Expr1 to be ground.
 *  \todo: improve using index deltas
 *  \todo: make sure it also works for reals
 *  \ingroup Filtering
 */
template<class Eval,class Expr1,class Expr2,class Expr3>
struct BndFilterView3<LinearGreaterEqual,Seq<Eval>,Expr1,Seq<Eval>,Expr2,Eval,Expr3> : IFilter
{
	BndFilterView3(Store& store,const Expr1& v1, const Expr2& v2,
					const Expr3& v3) :
						IFilter(store),c(store,v1),x(store,v2),v(store,v3),
					f(store,rel<Mul>(c[0].value(),x[0].getObj()))
					#ifdef CASPER_LOG
					,store(store)
					#endif

	{
		// count positive and negative coefficients
		npc=0,nnc=0;
		for (uint i = 0; i < c.size(); ++i)
			if (c[i].value()>0)
				++npc;
			else
			{
				assert(c[i].value()!=0);
				++nnc;
			}
		// store positive and negative coefficients
		pc = new (store) int[npc];
		nc = new (store) int[nnc];
		px = new (store) int[npc];
		nx = new (store) int[nnc];
		npc = nnc = 0;
		for (uint i = 0; i < c.size(); ++i)
			if (c[i].value()>0)
			{
				pc[npc] = c[i].value();
				px[npc] = i;
				++npc;
			}
			else
			{
				nc[nnc] = c[i].value();
				nx[nnc] = i;
				++nnc;
			}

		for (uint i = 1; i < c.size(); i++)
			f = f + rel<Mul>(c[i].value(),x[i].getObj());
	}
	bool execute()
	{
		#ifdef CASPER_LOG
		store.getEnv().log(this, "BndFilterView3<LinearGreaterEqual,Seq<Eval>,Expr1,Seq<Eval>,Expr2,Eval,Expr3>", Util::Logger::filterExecuteBegin);
		#endif

		Eval fmax = f.max();
		if (!v.updateMax(fmax))
			return false;
		fmax = fmax-v.min();

		for (uint i = 0; i < npc; ++i)
			if (!x[px[i]].updateMin(Util::divLb(pc[i]*x[px[i]].min()-fmax,pc[i])))
				return false;

		for (uint i = 0; i < nnc; i++)
			if (!x[nx[i]].updateMax(Util::divUb(nc[i]*x[nx[i]].max()-fmax,nc[i])))
				return false;

		return true;
	}
	Cost cost() const {	return linearLo; }
	void attach(INotifiable* s)
	{	x.attach(s); v.attach(s);	}
	void detach()
	{	x.detach(); v.detach();	}

	ValArrayView<Eval,Expr1> c;
	BndArrayView<Eval,Expr2> x;
	BndView<Eval,Expr3>		 v;
	BndExpr<Eval>			 f;

	#ifdef CASPER_LOG
	Store& store;
	#endif
	int* pc;
	int* nc;
	int* px;
	int* nx;
	uint npc;
	uint nnc;
};

template<class Eval,class Expr1,class Expr2,class Expr3>
struct PostBndFilter3<LinearGreaterEqual,Seq<Eval>,Expr1,Seq<Eval>,Expr2,Eval,Expr3>
{
    static bool post(Store& s,const Expr1& p1,const Expr2& p2,const Expr3& p3)
    {
    	ValArrayView<Eval,Expr1> c(s,p1);
    	ValArrayView<Eval,Expr2> x(s,p2);

    	assert(c.size()==x.size());	// FIXME: throw exception instead

    	ChkViewRel3<LinearGreaterEqual,Seq<Eval>,Expr1,
    						Seq<Eval>,Expr2,Eval,Expr3> chk(s,p1,p2,p3);

    	if (chk.isTrue())
    		return true;
    	if (!chk.canBeTrue())
    		return false;

    	// TODO: check for zero coefficients, all unity coefficients (i.e. sum)
    	// and rewriting when number of terms is small

    	return s.post(new (s) BndFilterView3<LinearGreaterEqual,Seq<Eval>,
    							Expr1,Seq<Eval>,Expr2,Eval,Expr3>(s,p1,p2,p3));
    }
};

template<class Eval,class Expr1,class Expr2,class Expr3>
struct PostBndFilter3<LinearLessEqual,Seq<Eval>,Expr1,Seq<Eval>,Expr2,Eval,Expr3>
{
    static bool post(Store& s,const Expr1& p1,const Expr2& p2,const Expr3& p3)
    {
    	ValArrayView<Eval,Expr1> c(s,p1);
    	Util::StdArray<Eval> nc(s,c.size());
    	for (uint i = 0; i < c.size(); ++i)
    		nc[i] = -c[i].value();
    	return s.post(linearGreaterEqual(nc,p2,-p3));
    }
};

}
}



#endif /* CASPER_CP_FILTER_LINEAR_H_ */
