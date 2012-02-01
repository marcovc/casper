 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_CP_FILTER_VAL
#define _H_CASPER_CP_FILTER_VAL

#include <casper/cp/filter/common.h>
#include <casper/cp/filter/common.h>
#include <casper/cp/filter/bnd.h>	//tmp
#include <casper/cp/view/val.h>
#include <casper/cp/view/bnd.h>
#include <casper/cp/int/fd.h>

namespace Casper {
namespace CP {

template<class,class,class>
struct ValFilterView1;

template<class,class,class,class,class>
struct ValFilterView2;

template<class,class,class,class,class,class,class>
struct ValFilterView3;

template<class,class,class,class,class,class,class,class,class>
struct ValFilterView4;

template<class,class,class,class,class,class,class,class,class,class,class>
struct ValFilterView5;

template<class Rel>
struct ValFilterView;


// happens for example when the negation for a relation is not defined
template<>
struct ValFilterView<Rel0<UnknownRel> > : UndefinedFilter
{
	ValFilterView(Store& s,const Rel0<UnknownRel>&) : UndefinedFilter(s,*this) {}
};

template<class F,class T1>
struct ValFilterView<Rel1<F,T1> > :
	public ValFilterView1<F,typename Casper::Traits::GetEval<T1>::Type,T1>
{
	ValFilterView(Store& s,const Rel1<F,T1>& r) :
		ValFilterView1<F,typename Casper::Traits::GetEval<T1>::Type,T1>(s,r.p1) {}
};

template<class F,class T1,class T2>
struct ValFilterView<Rel2<F,T1,T2> > :
	public ValFilterView2<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 typename Casper::Traits::GetEval<T2>::Type,T2>
{
	ValFilterView(Store& s,const Rel2<F,T1,T2>& r) :
		ValFilterView2<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  typename Casper::Traits::GetEval<T2>::Type,T2>(s,r.p1,r.p2) {}
};

template<class F,class T1,class T2,class T3>
struct ValFilterView<Rel3<F,T1,T2,T3> > :
	public ValFilterView3<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 typename Casper::Traits::GetEval<T2>::Type,T2,
						 typename Casper::Traits::GetEval<T3>::Type,T3>
{
	ValFilterView(Store& s,const Rel3<F,T1,T2,T3>& r) :
		ValFilterView3<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  typename Casper::Traits::GetEval<T2>::Type,T2,
					  typename Casper::Traits::GetEval<T3>::Type,T3>(s,r.p1,r.p2,r.p3) {}
};

template<class F,class T1,class T2,class T3,class T4>
struct ValFilterView<Rel4<F,T1,T2,T3,T4> > :
	public ValFilterView4<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 typename Casper::Traits::GetEval<T2>::Type,T2,
						 typename Casper::Traits::GetEval<T3>::Type,T3,
						 typename Casper::Traits::GetEval<T4>::Type,T4>
{
	ValFilterView(Store& s,const Rel4<F,T1,T2,T3,T4>& r) :
		ValFilterView4<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  typename Casper::Traits::GetEval<T2>::Type,T2,
					  typename Casper::Traits::GetEval<T3>::Type,T3,
					  typename Casper::Traits::GetEval<T4>::Type,T4>(s,r.p1,r.p2,r.p3,r.p4) {}
};

template<class F,class T1,class T2,class T3,class T4,class T5>
struct ValFilterView<Rel5<F,T1,T2,T3,T4,T5> > :
	public ValFilterView5<F,typename Casper::Traits::GetEval<T1>::Type,T1,
						 typename Casper::Traits::GetEval<T2>::Type,T2,
						 typename Casper::Traits::GetEval<T3>::Type,T3,
						 typename Casper::Traits::GetEval<T4>::Type,T4,
						 typename Casper::Traits::GetEval<T5>::Type,T5>
{
	ValFilterView(Store& s,const Rel5<F,T1,T2,T3,T4,T5>& r) :
		ValFilterView5<F,typename Casper::Traits::GetEval<T1>::Type,T1,
					  typename Casper::Traits::GetEval<T2>::Type,T2,
					  typename Casper::Traits::GetEval<T3>::Type,T3,
					  typename Casper::Traits::GetEval<T4>::Type,T4,
					  typename Casper::Traits::GetEval<T5>::Type,T5>(s,r.p1,r.p2,r.p3,r.p4,r.p5) {}
};

template<class Func,class Eval1,class Expr1>
struct PostValFilter1
{
	static bool post(Store& s,const Expr1& v1)
	{
		std::ostringstream os;
		::operator<<(os,rel<Func>(v1));
		throw Casper::Exception::UndefinedFilter(os.str().c_str(),"Value",
				Casper::Traits::getTypeStr<Rel1<Func,Expr1> >());
	}
};


template<class Func,class Eval1,class Expr1,class Eval2,class Expr2>
struct PostValFilter2
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2)
	{
		std::ostringstream os;
		::operator<<(os,rel<Func>(v1,v2));
		throw Casper::Exception::UndefinedFilter(os.str().c_str(),"Value",
				Casper::Traits::getTypeStr<Rel2<Func,Expr1,Expr2> >());
	}
};

template<class Func,class Eval1,class Expr1,class Eval2,class Expr2,
		class Eval3,class Expr3>
struct PostValFilter3
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2,const Expr3& v3)
	{
		std::ostringstream os;
		::operator<<(os,rel<Func>(v1,v2,v3));
		throw Casper::Exception::UndefinedFilter(os.str().c_str(),"Value",
				Casper::Traits::getTypeStr<Rel3<Func,Expr1,Expr2,Expr3> >());
	}
};

template<class Func,class Eval1,class Expr1,class Eval2,class Expr2,
		class Eval3,class Expr3,class Eval4,class Expr4>
struct PostValFilter4
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2,const Expr3& v3,const Expr4& v4)
	{
		std::ostringstream os;
		::operator<<(os,rel<Func>(v1,v2,v3,v4));
		throw Casper::Exception::UndefinedFilter(os.str().c_str(),"Value",
				Casper::Traits::getTypeStr<Rel4<Func,Expr1,Expr2,Expr3,Expr4> >());
	}
};

template<class Func,class Eval1,class Expr1,class Eval2,class Expr2,
		class Eval3,class Expr3,class Eval4,class Expr4,
		class Eval5,class Expr5>
struct PostValFilter5
{
	static bool post(Store& s,const Expr1& v1,const Expr2& v2,const Expr3& v3,const Expr4& v4,const Expr5& v5)
	{
		std::ostringstream os;
		::operator<<(os,rel<Func>(v1,v2,v3,v4,v5));
		throw Casper::Exception::UndefinedFilter(os.str().c_str(),"Value",
				Casper::Traits::getTypeStr<Rel5<Func,Expr1,Expr2,Expr3,Expr4,Expr5> >());
	}
};

struct PostValFilter
{
	template<class Func,class Expr1>
	bool operator()(Store& s,const Rel1<Func,Expr1>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef PostValFilter1<Func,Eval1,Expr1>		Post;
		return Post::post(s,v.p1);
	}
	template<class Func,class Expr1,class Expr2>
	bool operator()(Store& s,const Rel2<Func,Expr1,Expr2>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Expr2>::Type	Eval2;
		typedef PostValFilter2<Func,Eval1,Expr1,Eval2,Expr2>	Post;
		return Post::post(s,v.p1,v.p2);
	}
	template<class Func,class Expr1,class Expr2,class Expr3>
	bool operator()(Store& s,const Rel3<Func,Expr1,Expr2,Expr3>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Expr2>::Type	Eval2;
		typedef typename Casper::Traits::GetEval<Expr3>::Type	Eval3;
		typedef PostValFilter3<Func,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3> Post;
		return Post::post(s,v.p1,v.p2,v.p3);
	}
	template<class Func,class Expr1,class Expr2,class Expr3,class Expr4>
	bool operator()(Store& s,const Rel4<Func,Expr1,Expr2,Expr3,Expr4>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Expr2>::Type	Eval2;
		typedef typename Casper::Traits::GetEval<Expr3>::Type	Eval3;
		typedef typename Casper::Traits::GetEval<Expr4>::Type	Eval4;
		typedef PostValFilter4<Func,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3,Eval4,Expr4> Post;
		return Post::post(s,v.p1,v.p2,v.p3,v.p4);
	}
	template<class Func,class Expr1,class Expr2,class Expr3,class Expr4,class Expr5>
	bool operator()(Store& s,const Rel5<Func,Expr1,Expr2,Expr3,Expr4,Expr5>& v) const
	{
		typedef typename Casper::Traits::GetEval<Expr1>::Type	Eval1;
		typedef typename Casper::Traits::GetEval<Expr2>::Type	Eval2;
		typedef typename Casper::Traits::GetEval<Expr3>::Type	Eval3;
		typedef typename Casper::Traits::GetEval<Expr4>::Type	Eval4;
		typedef typename Casper::Traits::GetEval<Expr5>::Type	Eval5;
		typedef PostValFilter5<Func,Eval1,Expr1,Eval2,Expr2,Eval3,Expr3,Eval4,Expr4,Eval5,Expr5> Post;
		return Post::post(s,v.p1,v.p2,v.p3,v.p4,v.p5);
	}

	bool operator()(Store& s,const bool& b) const;
	template<class Dom>
	bool operator()(Store& s,const Var<bool,Dom>& v) const
	{	return s.post(
			new (s) BndFilterView<Rel2<Equal,Var<bool,Dom>,bool> >(s,rel<Equal>(v,true)));	}

	bool operator()(Store& s,const Casper::Expr<bool>& v) const;
};

extern PostValFilter postValFilter;

/**
 * FIXME: see same relation for bnd and dom
 * Enforces the conjunction of two constraints. It succeeds if both
 * constraints are satisfiable, fails otherwise.
 */
template<class Eval1,class Expr1,class Eval2,class Expr2>
struct ValFilterView2<And,Eval1,Expr1,Eval2,Expr2> : IFilter
{
	ValFilterView2(Store& s,const Expr1& p1, const Expr2& p2) :
		IFilter(s),store(s),v1(p1),v2(p2) {}

	bool execute()
	{	return postValFilter(store,v1) and postValFilter(store,v2);	}
/*
	bool entailed() const
	{	return p1.min()==p2.min() && p1.min()==true; }
	Filter operator!()
	{	return Val(!v1 or !v2); }
*/
	void attach(INotifiable* s) {}
	void detach(INotifiable* s) {}

	Store&				store;
	Expr1	v1;
	Expr2	v2;
};



// must update below to use Rels
#if 0
// AtMost constraint
template<class T,int Dim,class Dom>
struct AtMost : IFilter
{
	AtMost(Store& s, uint occ, VarArray<T,Dim,Dom> vars, const T& val) :
		IFilter(s),store(s),occ(s,occ),vars(vars),val(val) {}

	struct VarDemon : INotifiable
	{
		VarDemon(AtMost* pOwner, uint index) :
			rOwner(*pOwner),index(index) {}
		bool notify()
		{	return rOwner.notifyIndex(index);	}
		AtMost& rOwner;
		const uint index;
	};
	bool execute()	// this filter does everything in notification phase
	{	return true; }
	bool notifyIndex(uint index)
	{
		assert(vars[index].ground());
		if (vars[index].domain().value() == val)
		{
			if (occ == 0)
				return false;
			occ = occ-1;
		}
		return true;
	}
	void attach(INotifiable* s)
	{
		uint ss = vars.count();
		for (uint i = 0; i < ss; i++)
			vars(i).domain().attachOnGround(new (store) VarDemon(this,i));
	}
	void detach(INotifiable* s)
	{
		/* TODO: should detach all created demons */
	}
	Store&					store;
	Reversible<uint> 		occ;
	VarArray<T,Dim,Dom> 	vars;
	const T 		val;
};

template<class T,int Dim,class Dom>
Filter atMost(Store& store,uint occ, VarArray<T,Dim,Dom> vars, const T& val)
{	return new (store) AtMost<T,Dim,Dom>(store,occ,vars,val); }


// AtLeast constraint
template<class T,int Dim,class Dom>
struct AtLeast : IFilter
{
	AtLeast(Store& s, uint occ, VarArray<T,Dim,Dom> vars, const T& val) :
		IFilter(s),store(store),occ(s,vars.size()-occ),vars(vars),val(val) {}

	struct VarDemon : INotifiable
	{
		VarDemon(AtLeast* pOwner, uint index) :
			rOwner(*pOwner),index(index) {}
		bool notify()
		{	return rOwner.notifyIndex(index);	}
		AtLeast& rOwner;
		const uint index;
	};
	bool execute()	// this filter does everything in notification phase
	{	return true; }
	bool notifyIndex(uint index)
	{
		assert(vars[index].ground());
		if (vars[index].domain().value() != val)
		{
			if (occ == 0)
				return false;
			occ = occ-1;
		}
		return true;
	}
	void attach(INotifiable* s)
	{
		uint ss = vars.count();
		for (uint i = 0; i < ss; i++)
			vars(i).domain().attachOnGround(new (store) VarDemon(this,i));
	}
	void detach(INotifiable* s)
	{
		/* TODO: should detach all created demons */
	}
	Store&					store;
	Reversible<uint> 		occ;
	VarArray<T,Dim,Dom> 	vars;
	const T 		val;
};

template<class T,int Dim,class Dom>
Filter atLeast(Store& store,uint occ, VarArray<T,Dim,Dom> vars, const T& val)
{	return new (store) AtLeast<T,Dim,Dom>(store,occ,vars,val); }

// Exactly constraint
template<class T,int Dim,class Dom>
struct Exactly : IFilter
{
	Exactly(Store& s, uint occ, VarArray<T,Dim,Dom> vars, const T& val) :
		IFilter(s),store(s),eqocc(s,occ),neqocc(s,vars.size()-occ),
		vars(vars),
		val(val) {}

	struct VarDemon : INotifiable
	{
		VarDemon(Exactly* pOwner, uint index) :
			rOwner(*pOwner),index(index) {}
		bool notify()
		{	return rOwner.notifyIndex(index);	}
		Exactly& rOwner;
		const uint index;
	};
	bool execute()	// this filter does everything in notification phase
	{	return true; }
	bool notifyIndex(uint index)
	{
		assert(vars[index].ground());
		if (vars[index].domain().value() == val)
		{
			if (eqocc == 0)
				return false;
			eqocc = eqocc-1;
		}
		else
		{
			if (neqocc == 0)
				return false;
			neqocc = neqocc-1;
		}

		return true;
	}
	void attach(INotifiable* s)
	{
		uint ss = vars.count();
		for (uint i = 0; i < ss; i++)
			vars(i).domain().attachOnGround(new (store) VarDemon(this,i));
	}
	void detach(INotifiable* s)
	{
		/* TODO: should detach all created demons */
	}
	Store&					store;
	Reversible<uint> 		eqocc;
	Reversible<uint> 		neqocc;
	VarArray<T,Dim,Dom> 	vars;
	const T 		val;
};

template<class T,int Dim,class Dom>
Filter exactly(Store& store,uint occ, VarArray<T,Dim,Dom> vars, const T& val)
{	return new (store) Exactly<T,Dim,Dom>(store,occ,vars,val); }


template<>
struct ValFilterView<ValExpr<bool> > : IFilter
{
	ValFilterView(Store& s,const ValExpr<bool>& p) :
		IFilter(s),p(p) {}

	// TODO: use setVal when ready
	bool execute()
	{	return p.setValue(true); }
//	bool entailed() const
//	{	return p.ground() and p.value()==true;	}
//	Filter operator!() const
//	{	return Val(p==false); }
	void attach(INotifiable* s) {	p.attach(s);	}
	void detach(INotifiable* s) {	p.detach(s);	}

	ValExpr<bool>	p;
};
#endif

} // CP
} // Casper

#endif /*_H_CASPER_CP_FILTER_VAL */
