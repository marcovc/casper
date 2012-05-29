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
		#ifdef CASPER_LOG_OLD
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

namespace Detail {

template<class Eval,class Elem>
struct LinearGreaterEqualHelper : IFilter
{
	LinearGreaterEqualHelper(Store& store,const Util::StdVector<Eval>& pc,
										  const Util::StdVector<Eval>& nc,
										  const Util::StdVector<Elem>& px,
										  const Util::StdVector<Elem>& nx,
										  const Eval& b) :
						IFilter(store),
						npc(pc.size()),
						nnc(nc.size()),
						pc(pc),
						nc(nc),
						px(store,px),
						nx(store,nx),
						b(b)
					#ifdef CASPER_LOG
					,store(store)
					#endif

	{}
	bool execute()
	{
		#ifdef CASPER_LOG_OLD
		store.getEnv().log(this, "LinearGreaterEqualHelper", Util::Logger::filterExecuteBegin);
		#endif

		Eval fmax = 0;
		for (uint i = 0; i < npc; ++i)
			fmax += pc[i]*px[i].max();
		for (uint i = 0; i < nnc; ++i)
			fmax += nc[i]*nx[i].min();

		if (b > fmax)
			return false;

		fmax = b-fmax;

		if (fmax>=0)
		{
			for (uint i = 0; i < npc; ++i)
				if (!px[i].updateMin(Casper::Util::divLbPP(fmax,pc[i])+px[i].max()))
					return false;

			for (uint i = 0; i < nnc; i++)
				if (!nx[i].updateMax(Casper::Util::divUbPN(fmax,nc[i])+nx[i].min()))
					return false;
		}
		else
		{
			for (uint i = 0; i < npc; ++i)
				if (!px[i].updateMin(Casper::Util::divLbNP(fmax,pc[i])+px[i].max()))
					return false;

			for (uint i = 0; i < nnc; i++)
				if (!nx[i].updateMax(Casper::Util::divUbNN(fmax,nc[i])+nx[i].min()))
					return false;
		}
		return true;
	}
	Cost cost() const {	return linearLo; }
	void attach(INotifiable* s)
	{
		px.attach(s);
		nx.attach(s);
	}
	void detach()
	{
		px.detach();
		nx.detach();
	}
	const uint npc;
	const uint nnc;

	const Util::StdVector<Eval> pc;
	const Util::StdVector<Eval> nc;

	BndArrayView<Eval,Util::StdVector<Elem> >	px;
	BndArrayView<Eval,Util::StdVector<Elem> >	nx;

	const Eval		 b;

	#ifdef CASPER_LOG
	Store& store;
	#endif
};


template<class Eval,class Elem>
struct LinearGreaterEqualRewriteHelper : IFilter
{
	struct CX {
		const int 			c;
		BndView<Eval,Elem>	x;
		CX(Store& s, const int& c, const Elem& e) : c(c),x(s,e) {}
	};

	LinearGreaterEqualRewriteHelper(Store& store,const Util::StdVector<Eval>& pc,
										  const Util::StdVector<Eval>& nc,
										  const Util::StdVector<Elem>& px,
										  const Util::StdVector<Elem>& nx,
										  const Eval& b) :
						IFilter(store),
						npc(store,pc.size()),
						nnc(store,nc.size()),
						pcx(store,pc.size()),
						ncx(store,nc.size()),
						b(store,b)
						#ifdef CASPER_LOG
						,eg(this,"Casper::CP::Detail::LinearGreaterEqualRewriteHelper")
						#endif

	{
		CASPER_AT_FILTER_CTOR_ENTER(eg);
		for (uint i = 0; i < npc; ++i)
			pcx.initialize(i,new (store) CX(store,pc[i],px[i]));
		for (uint i = 0; i < nnc; ++i)
			ncx.initialize(i,new (store) CX(store,nc[i],nx[i]));
		CASPER_AT_FILTER_CTOR_LEAVE(eg);
	}

	bool execute()
	{
		CASPER_AT_FILTER_EXEC_ENTER(eg);

		Eval fmax = 0;
		Eval fmin = 0;
		for (uint i = 0; i < npc; )
		{
			const CX& cx = *pcx[i];
			Eval xmax,xmin;
			cx.x.range(xmin,xmax);
			if (xmax==xmin)
			{
				b = b-cx.c*xmax;
				npc = npc-1;
				pcx[i] = pcx[npc];
			}
			else
			{
				fmax += cx.c*xmax;
				fmin += cx.c*xmin;
				++i;
			}
		}
		for (uint i = 0; i < nnc; )
		{
			const CX& cx = *ncx[i];
			Eval xmax,xmin;
			cx.x.range(xmin,xmax);
			if (xmax==xmin)
			{
				b = b-cx.c*xmin;
				nnc = nnc-1;
				ncx[i] = ncx[nnc];
			}
			else
			{
				fmax += cx.c*xmin;
				fmin += cx.c*xmax;
				++i;
			}
		}

		if (b > fmax)
		{
			CASPER_AT_FILTER_EXEC_LEAVE(eg,false);
			return false;
		}

		if (fmin >= b)
		{
			detach();
			CASPER_AT_FILTER_EXEC_LEAVE(eg,true);
			return true;
		}

		fmax = b-fmax;

		if (fmax>=0)
		{
			for (uint i = 0; i < npc; ++i)
				if (!pcx[i]->x.updateMin(Casper::Util::divLbPP(fmax,pcx[i]->c)+pcx[i]->x.max()))
				{
					CASPER_AT_FILTER_EXEC_LEAVE(eg,false);
					return false;
				}

			for (uint i = 0; i < nnc; i++)
				if (!ncx[i]->x.updateMax(Casper::Util::divUbPN(fmax,ncx[i]->c)+ncx[i]->x.min()))
				{
					CASPER_AT_FILTER_EXEC_LEAVE(eg,false);
					return false;
				}
		}
		else
		{
			for (uint i = 0; i < npc; ++i)
				if (!pcx[i]->x.updateMin(Casper::Util::divLbNP(fmax,pcx[i]->c)+pcx[i]->x.max()))
				{
					CASPER_AT_FILTER_EXEC_LEAVE(eg,false);
					return false;
				}

			for (uint i = 0; i < nnc; i++)
				if (!ncx[i]->x.updateMax(Casper::Util::divUbNN(fmax,ncx[i]->c)+ncx[i]->x.min()))
				{
					CASPER_AT_FILTER_EXEC_LEAVE(eg,false);
					return false;
				}
		}
		CASPER_AT_FILTER_EXEC_LEAVE(eg,true);
		return true;
	}
	Cost cost() const
	{	return linearLo;	}
	void attach(INotifiable* s)
	{
		CASPER_AT_FILTER_ATTACH_ENTER(eg);
		for (uint i = 0; i < npc; ++i)
			pcx[i]->x.attach(s);
		for (uint i = 0; i < nnc; ++i)
			ncx[i]->x.attach(s);
		CASPER_AT_FILTER_ATTACH_LEAVE(eg);
	}
	void detach()
	{
		CASPER_AT_FILTER_DETACH_ENTER(eg);
		for (uint i = 0; i < npc; ++i)
			pcx[i]->x.detach();
		for (uint i = 0; i < nnc; ++i)
			ncx[i]->x.detach();
		CASPER_AT_FILTER_DETACH_LEAVE(eg);
	}

	Reversible<uint> npc;
	Reversible<uint> nnc;

	Vector<CX*>	pcx;
	Vector<CX*>	ncx;

	Reversible<Eval> b;

	#ifdef CASPER_LOG
	Casper::Util::FilterEG eg;
	#endif
};

}

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

//    	if (c.size()==3)
//   		return s.post(c[0].getObj()*x[0].getObj()+c[1].getObj()*x[1].getObj()>=p3-c[2].getObj()*x[2].getObj());

    	uint npc = 0;
		uint nnc = 0;

     	for (auto it = c.begin(); it != c.end(); ++it)
		{
			assert(it->value()!=0); // FIXME: throw exception instead
			if (it->value()>0)
				++npc;
			else
				++nnc;
		}

     	// fill positive and negative coefficients arrays
		Util::StdVector<Eval> pc(s,npc);
		Util::StdVector<Eval> nc(s,nnc);
     	// fill positive and negative bndviews arrays
       	typedef typename Casper::Traits::GetTermElem<Expr2>::Type	Elem;
    	Util::StdVector<Elem> px(s,npc);
		Util::StdVector<Elem> nx(s,nnc);
		npc = 0;
		nnc = 0;
		IterationView<Expr2>	itx(p2);
    	for (auto itc = c.begin(); itc != c.end(); ++itc, itx.iterate())
    		if (itc->value()>0)
    		{
    			pc[npc] = itc->value();
    			::new(&px[npc]) Elem(itx.value());
    			++npc;
    		}
    		else
    		{
    			nc[nnc] = itc->value();
    			::new(&nx[nnc]) Elem(itx.value());
    			++nnc;
    		}

    	ValView<Eval,Expr3> b(s,p3);
    	assert(b.ground());

    	// TODO: check for all unity coefficients (i.e. sum)
    	// and rewriting when number of terms is small

    	return s.post(new (s) Detail::LinearGreaterEqualRewriteHelper<Eval,Elem>(s,pc,nc,px,nx,b.value()));
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
