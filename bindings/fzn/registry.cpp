/*
 *  Adapted for Casper from skeleton code by Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *  	Guido Tack, 2007
 *  	Marco Correia, 2012
 */

#include <thirdparty/flatzinc_skeleton/registry.hh>
#include <bindings/fzn/flatzinc.hh>


namespace FlatZinc {

  Registry& registry(void) {
    static Registry r;
    return r;
  }

  void
  Registry::post(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
    std::map<std::string,poster>::iterator i = r.find(ce.id);
    if (i == r.end()) {
      throw FlatZinc::Error("Registry",
        std::string("Constraint ")+ce.id+" not found");
    }
    i->second(s, ce, ann);
  }

  void
  Registry::add(const std::string& id, poster p) {
    r[id] = p;
  }

  namespace {

  	template<class> struct IsVar;
  	template<class> struct IsVarArray;
  	template<class> struct GetVar;
  	template<class> struct GetVarArray;
  	template<class> struct GetLit;
  	template<class> struct GetLitArray;


  	template<>
  	struct GetLit<int> {
  		int operator()(FlatZincModel&,AST::Node* pNode) const
  		{	return pNode->getInt();	}
  	};

  	template<>
  	struct GetLit<bool> {
  		bool operator()(FlatZincModel&,AST::Node* pNode) const
  		{	return pNode->getBool();	}
  	};

  	// FIXME: set literals are not available in Casper, currently this creates
  	// an instantiated auxiliary variable
  	template<>
  	struct GetLit<Casper::IntSet> {
  		SetVar operator()(FlatZincModel& s,AST::Node* pNode) const
  		{
  		   AST::SetLit* sl = pNode->getSet();
		   if (sl->interval)
			 return SetVar(s.solver,Casper::Util::StdRange<int>(sl->min, sl->max),
					 	 	 	   	  Casper::Util::StdRange<int>(sl->min, sl->max));
		   else
		   {
				int* is = new int[static_cast<unsigned long int>(sl->s.size())];
				for (int i=sl->s.size(); i--; )
				  is[i] = sl->s[i];
				std::sort(is,&is[sl->s.size()]);
				SetVar r(s.solver,is, &is[sl->s.size()],&is[sl->s.size()], &is[sl->s.size()]);
				delete[] is;
				return r;
		   }
  		}
  	};

  	template<class T>
  	struct GetLitArray {
  		Casper::Util::StdArray<T> operator()(FlatZincModel& s,AST::Node* pNode) const
  		{
  			AST::Array* a = pNode->getArray();
  			Casper::Util::StdArray<T> r(s.solver,a->a.size());
  			for (unsigned int i = 0; i < a->a.size(); ++i)
  				r[i] = GetLit<T>()(s,a->a[i]);
  			return r;
  		}
  	};

  	template<class T>
  	struct GetLitSet {
  		Casper::Detail::IntervalA<T> operator()(FlatZincModel& s,AST::Node* pNode) const
  		{
  			 AST::SetLit* sl = pNode->getSet();
			 if (sl->interval)
				 return Casper::Detail::IntervalA<T>(s.solver,Casper::Util::StdRange<T>(sl->min, sl->max));
			  else
				 return Casper::Detail::IntervalA<T>(s.solver,&sl->s[0],&sl->s[0] + sl->s.size());
  		}
  	};

  	template<class T>
  	struct GetLit<Casper::Seq<T> > : GetLitArray<T> {};

  	template<>
  	struct IsVar<int> {
  		bool operator()(AST::Node* pNode) const
  		{	return pNode->isIntVar();	}
  	};

  	template<>
  	struct IsVar<bool> {
  		bool operator()(AST::Node* pNode) const
  		{	return pNode->isBoolVar();	}
  	};

  	template<>
  	struct IsVar<Casper::IntSet> {
  		bool operator()(AST::Node* pNode) const
  		{	return pNode->isSetVar();	}
  	};

  	template<class T>
  	struct IsVarArray {
  		bool operator()(AST::Node* pNode) const
  		{	return pNode->isArray(); }
  	};

  	template<class T>
  	struct IsVar<Casper::Seq<T> > : IsVarArray<T>
  	{};

  	template<>
  	struct GetVar<int> {
  		IntVar operator()(FlatZincModel& s,AST::Node* pNode) const
  		{
  			if (IsVar<int>()(pNode))
  				return (*s.piv)[pNode->getIntVar()];
  			else
  			{
  				int value = pNode->getInt();
  				return IntVar(s.solver,value,value);
  			}
  		}
  	};

  	template<>
  	struct GetVar<bool> {
  		BoolVar operator()(FlatZincModel& s,AST::Node* pNode) const
  		{
  			if (IsVar<bool>()(pNode))
  				return (*s.pbv)[pNode->getBoolVar()];
  			else
  			{
  				bool value = pNode->getBool();
  				return BoolVar(s.solver,value,value);
  			}
  		}
  	};

  	template<>
  	struct GetVar<Casper::IntSet> {
  		SetVar operator()(FlatZincModel& s,AST::Node* pNode) const
  		{
  			if (IsVar<Casper::IntSet>()(pNode))
  				return (*s.psv)[pNode->getSetVar()];
  			else
  				return GetLit<Casper::IntSet>()(s,pNode); // FIXME
  		}
  	};

  	template<class T>
  	struct GetVarArray {
  		Casper::CP::VarArray<T> operator()(FlatZincModel& s,AST::Node* pNode) const
  		{
  			AST::Array* a = pNode->getArray();
  			Casper::CP::VarArray<T> r(s.solver,a->a.size());
  			for (unsigned int i = 0; i < a->a.size(); ++i)
  				r[i] = GetVar<T>()(s,a->a[i]);
  			return r;
  		}
  	};

  	template<class T>
  	struct GetVar<Casper::Seq<T> > : GetVarArray<T>
  	{};

	// unary predicates
  	template<class PredTree,class Eval0,class Post>
  	void p_rel1_gen(FlatZincModel& s, const ConExpr& ce, AST::Node* ann, const Post& post)
  	{
  		PredTree p(s);
  	  	if (IsVar<Eval0>()(ce[0]))
			p(GetVar<Eval0>()(s,ce[0]),post);
		else
			p(GetLit<Eval0>()(s,ce[0]),post);
	}

 	// binary predicates
  	template<class PredTree,class Eval0,class Eval1>
  	void p_rel2_gen(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
  	{
  		PredTree p(s);
  	  	if (IsVar<Eval0>()(ce[0]))
			if (IsVar<Eval1>()(ce[1]))
				p(GetVar<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]));
			else
				p(GetVar<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]));
		else
			if (IsVar<Eval1>()(ce[1]))
				p(GetLit<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]));
			else
				p(GetLit<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]));
  	}

 	// ternary predicates
  	template<class PredTree,class Eval0,class Eval1,class Eval2>
  	void p_rel3_gen(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
  	{
  		PredTree p(s);
    	if (IsVar<Eval0>()(ce[0]))
    		if (IsVar<Eval1>()(ce[1]))
    			if (IsVar<Eval2>()(ce[2]))
    				p(GetVar<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]),GetVar<Eval2>()(s,ce[2]));
				else
					p(GetVar<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]),GetLit<Eval2>()(s,ce[2]));
    		else
    			if (IsVar<Eval2>()(ce[2]))
    				p(GetVar<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]),GetVar<Eval2>()(s,ce[2]));
				else
					p(GetVar<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]),GetLit<Eval2>()(s,ce[2]));
    	else
    		if (IsVar<Eval1>()(ce[1]))
    			if (IsVar<Eval2>()(ce[2]))
    				p(GetLit<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]),GetVar<Eval2>()(s,ce[2]));
				else
					p(GetLit<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]),GetLit<Eval2>()(s,ce[2]));
    		else
    			if (IsVar<Eval2>()(ce[2]))
    				p(GetLit<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]),GetVar<Eval2>()(s,ce[2]));
				else
					p(GetLit<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]),GetLit<Eval2>()(s,ce[2]));
  	}

 	// 4-ary predicates
  	template<class PredTree,class Eval0,class Eval1,class Eval2,class Eval3>
  	void p_rel4_gen(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
  	{
  		PredTree p(s);
    	if (IsVar<Eval0>()(ce[0]))
    		if (IsVar<Eval1>()(ce[1]))
    			if (IsVar<Eval2>()(ce[2]))
    				if (IsVar<Eval3>()(ce[3]))
    					p(GetVar<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]),
    					  GetVar<Eval2>()(s,ce[2]),GetVar<Eval3>()(s,ce[3]));
    				else
    					p(GetVar<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]),
    					  GetVar<Eval2>()(s,ce[2]),GetLit<Eval3>()(s,ce[3]));
    			else
    				if (IsVar<Eval3>()(ce[3]))
    					p(GetVar<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]),
    					  GetVar<Eval2>()(s,ce[2]),GetVar<Eval3>()(s,ce[3]));
    				else
    					p(GetVar<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]),
    					  GetLit<Eval2>()(s,ce[2]),GetLit<Eval3>()(s,ce[3]));
    		else
    			if (IsVar<Eval2>()(ce[2]))
    				if (IsVar<Eval3>()(ce[3]))
    					p(GetVar<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]),
    					  GetVar<Eval2>()(s,ce[2]),GetVar<Eval3>()(s,ce[3]));
    				else
    					p(GetVar<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]),
    					  GetVar<Eval2>()(s,ce[2]),GetLit<Eval3>()(s,ce[3]));
    			else
    				if (IsVar<Eval3>()(ce[3]))
    					p(GetVar<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]),
    					  GetVar<Eval2>()(s,ce[2]),GetVar<Eval3>()(s,ce[3]));
    				else
    					p(GetVar<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]),
    					  GetLit<Eval2>()(s,ce[2]),GetLit<Eval3>()(s,ce[3]));
    	else
    		if (IsVar<Eval1>()(ce[1]))
    			if (IsVar<Eval2>()(ce[2]))
    				if (IsVar<Eval3>()(ce[3]))
    					p(GetLit<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]),
    					  GetVar<Eval2>()(s,ce[2]),GetVar<Eval3>()(s,ce[3]));
    				else
    					p(GetLit<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]),
    					  GetVar<Eval2>()(s,ce[2]),GetLit<Eval3>()(s,ce[3]));
    			else
    				if (IsVar<Eval3>()(ce[3]))
    					p(GetLit<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]),
    					  GetVar<Eval2>()(s,ce[2]),GetVar<Eval3>()(s,ce[3]));
    				else
    					p(GetLit<Eval0>()(s,ce[0]),GetVar<Eval1>()(s,ce[1]),
    					  GetLit<Eval2>()(s,ce[2]),GetLit<Eval3>()(s,ce[3]));
    		else
    			if (IsVar<Eval2>()(ce[2]))
    				if (IsVar<Eval3>()(ce[3]))
    					p(GetLit<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]),
    					  GetVar<Eval2>()(s,ce[2]),GetVar<Eval3>()(s,ce[3]));
    				else
    					p(GetLit<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]),
    					  GetVar<Eval2>()(s,ce[2]),GetLit<Eval3>()(s,ce[3]));
    			else
    				if (IsVar<Eval3>()(ce[3]))
    					p(GetLit<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]),
    					  GetVar<Eval2>()(s,ce[2]),GetVar<Eval3>()(s,ce[3]));
    				else
    					p(GetLit<Eval0>()(s,ce[0]),GetLit<Eval1>()(s,ce[1]),
    					  GetLit<Eval2>()(s,ce[2]),GetLit<Eval3>()(s,ce[3]));
    }

 	// unay predicates
  	template<class Pred>
  	struct PredTree1
  	{
  		PredTree1(FlatZincModel& s) : s(s) {}
  		template<class T1,class Post>
  		void operator()(const T1& t1, const Post& p)
  		{	s.valid &= s.solver.post(Casper::rel<Pred>(t1),p); }
  		FlatZincModel& s;
  	};
  	template<class Pred,class Eval0,class Post = Casper::CP::PostBndFilter>
  	void p_rel1(FlatZincModel& s, const ConExpr& ce, AST::Node* ann,
  				const Post& p = Casper::CP::postBndFilter)
  	{	p_rel1_gen<PredTree1<Pred>,Eval0>(s,ce,ann,p); }

  	// binary predicates
  	template<class Pred>
  	struct PredTree2
  	{
  		PredTree2(FlatZincModel& s) : s(s) {}
  		template<class T1,class T2>
  		void operator()(const T1& t1, const T2& t2)
  		{	s.valid &= s.solver.post(Casper::rel<Pred>(t1,t2)); }
  		FlatZincModel& s;
  	};
  	template<class Pred,class Eval0,class Eval1>
  	void p_rel2(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
  	{	p_rel2_gen<PredTree2<Pred>,Eval0,Eval1>(s,ce,ann); }

  	// binary predicates with switched args
  	template<class Pred>
  	struct PredTree2Sw
  	{
  		PredTree2Sw(FlatZincModel& s) : s(s) {}
  		template<class T1,class T2>
  		void operator()(const T1& t1, const T2& t2)
  		{	s.valid &= s.solver.post(Casper::rel<Pred>(t2,t1)); }
  		FlatZincModel& s;
  	};
  	template<class Pred,class Eval0,class Eval1>
  	void p_rel2_sw(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
  	{	p_rel2_gen<PredTree2Sw<Pred>,Eval0,Eval1>(s,ce,ann); }

  	// binary predicates specified by a unary operator and a binary predicate
  	template<class Op,class Pred>
  	struct PredTree2_1_2
  	{
  		PredTree2_1_2(FlatZincModel& s) : s(s) {}
  		template<class T1,class T2>
  		void operator()(const T1& t1, const T2& t2)
  		{	s.valid &= s.solver.post(Casper::rel<Pred>(Casper::rel<Op>(t1),t2)); }
  		FlatZincModel& s;
  	};
  	template<class Op, class Pred,class Eval0,class Eval1>
  	void p_rel2_1_2(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
  	{	p_rel2_gen<PredTree2_1_2<Op,Pred>,Eval0,Eval1>(s,ce,ann); }

  	// ternary predicates
  	template<class Pred>
  	struct PredTree3
  	{
  		PredTree3(FlatZincModel& s) : s(s) {}
  		template<class T1,class T2,class T3>
  		void operator()(const T1& t1, const T2& t2, const T3& t3)
  		{	s.valid &= s.solver.post(Casper::rel<Pred>(t1,t2,t3)); }
  		FlatZincModel& s;
  	};
  	template<class Pred,class Eval0,class Eval1,class Eval2>
  	void p_rel3(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
  	{	p_rel3_gen<PredTree3<Pred>,Eval0,Eval1,Eval2>(s,ce,ann); }

  	// ternary predicates with the two first elementes swapped
  	template<class Pred>
  	struct PredTree3_Sw2
  	{
  		PredTree3_Sw2(FlatZincModel& s) : s(s) {}
  		template<class T1,class T2,class T3>
  		void operator()(const T1& t1, const T2& t2, const T3& t3)
  		{	s.valid &= s.solver.post(Casper::rel<Pred>(t2,t1,t3)); }
  		FlatZincModel& s;
  	};
  	template<class Pred,class Eval0,class Eval1,class Eval2>
  	void p_rel3_sw2(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
  	{	p_rel3_gen<PredTree3_Sw2<Pred>,Eval0,Eval1,Eval2>(s,ce,ann); }

  	// ternary predicates specified by a binary operator and a binary predicate
  	template<class Op,class Pred>
  	struct PredTree3_2_2
  	{
  		PredTree3_2_2(FlatZincModel& s) : s(s) {}
  		template<class T1,class T2,class T3>
  		void operator()(const T1& t1, const T2& t2, const T3& t3)
  		{	s.valid &= s.solver.post(Casper::rel<Pred>(Casper::rel<Op>(t1,t2),t3)); }
  		FlatZincModel& s;
  	};
  	template<class Op, class Pred,class Eval0,class Eval1,class Eval2>
  	void p_rel3_2_2(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
  	{	p_rel3_gen<PredTree3_2_2<Op,Pred>,Eval0,Eval1,Eval2>(s,ce,ann); }

  	// ternary predicates specified by a binary operator and a binary predicate,
  	// where the elements to the binary operator are swapped
  	template<class Op,class Pred>
  	struct PredTree3_2Sw_2
  	{
  		PredTree3_2Sw_2(FlatZincModel& s) : s(s) {}
  		template<class T1,class T2,class T3>
  		void operator()(const T1& t1, const T2& t2, const T3& t3)
  		{	s.valid &= s.solver.post(Casper::rel<Pred>(Casper::rel<Op>(t2,t1),t3)); }
  		FlatZincModel& s;
  	};
  	template<class Op, class Pred,class Eval0,class Eval1,class Eval2>
  	void p_rel3_2Sw_2(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
  	{	p_rel3_gen<PredTree3_2Sw_2<Op,Pred>,Eval0,Eval1,Eval2>(s,ce,ann); }

 	// 4-ary predicates specified by a ternary operator and a binary predicate
  	template<class Op,class Pred>
  	struct PredTree4_3_2
  	{
  		PredTree4_3_2(FlatZincModel& s) : s(s) {}
  		template<class T1,class T2,class T3,class T4>
  		void operator()(const T1& t1, const T2& t2, const T3& t3, const T4& t4)
  		{	s.valid &= s.solver.post(Casper::rel<Pred>(Casper::rel<Op>(t1,t2,t3),t4)); }
  		FlatZincModel& s;
  	};
  	template<class Op, class Pred,class Eval0,class Eval1,class Eval2,class Eval3>
  	void p_rel4_3_2(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
  	{	p_rel4_gen<PredTree4_3_2<Op,Pred>,Eval0,Eval1,Eval2,Eval3>(s,ce,ann); }

 	// 4-ary predicates specified by two binary operators and a binary predicate
  	template<class Op1,class Op2,class Pred>
  	struct PredTree4_2_2_2
  	{
  		PredTree4_2_2_2(FlatZincModel& s) : s(s) {}
  		template<class T1,class T2,class T3,class T4>
  		void operator()(const T1& t1, const T2& t2, const T3& t3, const T4& t4)
  		{	s.valid &= s.solver.post(Casper::rel<Pred>(Casper::rel<Op2>(Casper::rel<Op1>(t1,t2),t3),t4)); }
  		FlatZincModel& s;
  	};
  	template<class Op1, class Op2,class Pred,class Eval0,class Eval1,class Eval2,class Eval3>
  	void p_rel4_2_2_2(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
  	{	p_rel4_gen<PredTree4_2_2_2<Op1,Op2,Pred>,Eval0,Eval1,Eval2,Eval3>(s,ce,ann); }

  	// -------------------------------- // -------------------------------- //

  	bool isEmptyArray(AST::Node* pn)
  	{	return pn->getArray()->a.size()==0;	}

  	void p_int_eq(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      p_rel2<Casper::Equal,int,int>(s,ce,ann);
    }
    void p_int_ne(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      p_rel2<Casper::Distinct,int,int>(s,ce,ann);
    }
    void p_int_le(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      p_rel2<Casper::LessEqual,int,int>(s,ce,ann);
    }
    void p_int_lt(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      p_rel2<Casper::Less,int,int>(s,ce,ann);
    }

    /* Comparisons */
    void p_int_eq_reif(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel3_2_2<Casper::Equal,Casper::Equal,int,int,bool>(s,ce,ann);
    }
    void p_int_ne_reif(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel3_2_2<Casper::Distinct,Casper::Equal,int,int,bool>(s,ce,ann);
    }
    void p_int_le_reif(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel3_2_2<Casper::LessEqual,Casper::Equal,int,int,bool>(s,ce,ann);
    }
    void p_int_lt_reif(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel3_2_2<Casper::Less,Casper::Equal,int,int,bool>(s,ce,ann);
    }

    void p_int_lin_eq(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
  	  if (isEmptyArray(ce[0]) or isEmptyArray(ce[1]))
  	    return;
      p_rel3<Casper::LinearEqual,Casper::IntSeq,Casper::IntSeq,int>(s,ce,ann);
    }
    void p_int_lin_eq_reif(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
  	  if (isEmptyArray(ce[0]) or isEmptyArray(ce[1]))
  	    return;
      p_rel4_3_2<Casper::LinearEqual,Casper::Equal,
      	  	  	 Casper::IntSeq,Casper::IntSeq,int,bool>(s,ce,ann);
    }
    void p_int_lin_ne(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
	  if (isEmptyArray(ce[0]) or isEmptyArray(ce[1]))
		return;
      p_rel3_2_2<Casper::SumProduct,Casper::Distinct,
      	  	  	 Casper::IntSeq,Casper::IntSeq,int>(s,ce,ann);
    }
    void p_int_lin_ne_reif(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
//      std::cerr << "int_lin_ne_reif("<<(*ce[0])<<","<<(*ce[1])<<","<<(*ce[2])
//        <<","<<(*ce[3])<<")::"<<(*ann)<<"\n";
	  if (isEmptyArray(ce[0]) or isEmptyArray(ce[1]))
		return;
      p_rel4_2_2_2<Casper::SumProduct,Casper::Distinct,Casper::Equal,
      	  	  	   Casper::IntSeq,Casper::IntSeq,int,bool>(s,ce,ann);
    }
    void p_int_lin_le(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
  	  if (isEmptyArray(ce[0]) or isEmptyArray(ce[1]))
  	    return;
      p_rel3<Casper::LinearLessEqual,Casper::IntSeq,Casper::IntSeq,int>(s,ce,ann);
    }
    void p_int_lin_le_reif(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
  	  if (isEmptyArray(ce[0]) or isEmptyArray(ce[1]))
  	    return;
      p_rel4_3_2<Casper::LinearLessEqual,Casper::Equal,Casper::IntSeq,
      	  	  	  	  	  	  	  Casper::IntSeq,int,bool>(s,ce,ann);
    }

    /* arithmetic constraints */
  
    void p_int_plus(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      p_rel3_2_2<Casper::Add,Casper::Equal,int,int,int>(s,ce,ann);
    }

    void p_int_times(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      p_rel3_2_2<Casper::Mul,Casper::Equal,int,int,int>(s,ce,ann);
    }
    void p_int_div(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      p_rel3_2_2<Casper::Div,Casper::Equal,int,int,int>(s,ce,ann);
    }
    void p_int_mod(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      p_rel3_2_2<Casper::Mod,Casper::Equal,int,int,int>(s,ce,ann);
    }

    void p_int_min(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      p_rel3_2_2<Casper::Min,Casper::Equal,int,int,int>(s,ce,ann);
    }
    void p_int_max(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      p_rel3_2_2<Casper::Max,Casper::Equal,int,int,int>(s,ce,ann);
    }

    void p_bool_eq(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel2<Casper::Equal,bool,bool>(s,ce,ann);
    }
    void p_bool_eq_reif(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel3_2_2<Casper::Equal,Casper::Equal,bool,bool,bool>(s,ce,ann);
    }
    void p_bool_le(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel2<Casper::LessEqual,bool,bool>(s,ce,ann);
    }
    void p_bool_le_reif(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel3_2_2<Casper::LessEqual,Casper::Equal,bool,bool,bool>(s,ce,ann);
    }
    void p_bool_lt(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel2<Casper::Less,bool,bool>(s,ce,ann);
    }
    void p_bool_lt_reif(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel3_2_2<Casper::Less,Casper::Equal,bool,bool,bool>(s,ce,ann);
    }

    void p_bool_or(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel3_2_2<Casper::Or,Casper::Equal,bool,bool,bool>(s,ce,ann);
    }
    void p_bool_and(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel3_2_2<Casper::And,Casper::Equal,bool,bool,bool>(s,ce,ann);
    }
    void p_array_bool_and(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
    {
      std::cerr << "array_bool_and("<<(*ce[0])<<","<<(*ce[1])
        <<")::"<<(*ann)<<"\n";
    }
    void p_array_bool_or(FlatZincModel& s, const ConExpr& ce, AST::Node* ann)
    {
      std::cerr << "array_bool_or("<<(*ce[0])<<","<<(*ce[1])
        <<")::"<<(*ann)<<"\n";
    }
    void p_array_bool_clause(FlatZincModel& s, const ConExpr& ce,
                             AST::Node* ann) {
      std::cerr << "array_bool_clause("<<(*ce[0])<<","<<(*ce[1])
        <<")::"<<(*ann)<<"\n";
    }
    void p_bool_xor(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel3_2_2<Casper::XOr,Casper::Equal,bool,bool,bool>(s,ce,ann);
    }
    void p_bool_not(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
        p_rel2_1_2<Casper::Not,Casper::Equal,bool,bool>(s,ce,ann);
    }
  
    /* element constraints */
    void p_array_int_element(FlatZincModel& s, const ConExpr& ce, 
                                 AST::Node* ann) {
      p_rel3_sw2<Casper::ElementEqual,int,Casper::IntSeq,int>(s,ce,ann);
    }
    void p_array_bool_element(FlatZincModel& s, const ConExpr& ce, 
                                  AST::Node* ann) {
      p_rel3_sw2<Casper::ElementEqual,int,Casper::BoolSeq,bool>(s,ce,ann);
    }
  
    /* coercion constraints */
    void p_bool2int(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      p_rel2_1_2<Casper::Cast<int>,Casper::Equal,bool,int>(s,ce,ann);
    }

    void p_int_in(FlatZincModel& s, const ConExpr& ce, AST::Node *ann) {
    	if (IsVar<int>()(ce[0]))
    		s.valid = s.solver.post(Casper::member(GetVar<int>()(s,ce[0]),GetLitSet<int>()(s,ce[1])));
    	else
		if (IsVar<bool>()(ce[0]))
			s.valid = s.solver.post(Casper::member(GetVar<bool>()(s,ce[0]),GetLitSet<bool>()(s,ce[1])));
		else
			assert(0);
        }

    void p_abs(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      p_rel2_1_2<Casper::Abs,Casper::Equal,int,int>(s,ce,ann);
    }

    void p_int_distinct(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      if (ann and ann->hasAtom("val"))
        p_rel1<Casper::Distinct,Casper::IntSeq>(s,ce,ann,Casper::CP::postValFilter);
      else
	  if (ann and ann->hasAtom("domain"))
		p_rel1<Casper::Distinct,Casper::IntSeq>(s,ce,ann,Casper::CP::postDomFilter);
	  else
	    p_rel1<Casper::Distinct,Casper::IntSeq>(s,ce,ann,Casper::CP::postBndFilter);
    }

    void p_int_lex_lesseq(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
		p_rel2<Casper::LessEqual,Casper::IntSeq,Casper::IntSeq>(s,ce,ann);
    }

    void p_int_table(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
    	if (IsVarArray<int>()(ce[0]))
    		s.valid = s.solver.post(Casper::inTable(GetVarArray<int>()(s,ce[0]),
    												GetLitArray<int>()(s,ce[1])));
    	else
    		s.valid = s.solver.post(Casper::inTable(GetLitArray<int>()(s,ce[0]),
    												GetLitArray<int>()(s,ce[1])));
    }

    void p_bool_table(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
    	if (IsVarArray<bool>()(ce[0]))
    		s.valid = s.solver.post(Casper::inTable(GetVarArray<bool>()(s,ce[0]),
    												GetLitArray<bool>()(s,ce[1])));
    	else
    		s.valid = s.solver.post(Casper::inTable(GetLitArray<bool>()(s,ce[0]),
    												GetLitArray<bool>()(s,ce[1])));
    }

    class IntPoster {
    public:
      IntPoster(void) {
        registry().add("int_eq", &p_int_eq);
        registry().add("int_ne", &p_int_ne);
        registry().add("int_le", &p_int_le);
        registry().add("int_lt", &p_int_lt);
        registry().add("int_eq_reif", &p_int_eq_reif);
        registry().add("int_ne_reif", &p_int_ne_reif);
        registry().add("int_le_reif", &p_int_le_reif);
        registry().add("int_lt_reif", &p_int_lt_reif);
        registry().add("int_lin_eq", &p_int_lin_eq);
        registry().add("int_lin_eq_reif", &p_int_lin_eq_reif);
        registry().add("int_lin_ne", &p_int_lin_ne);
        registry().add("int_lin_ne_reif", &p_int_lin_ne_reif);
        registry().add("int_lin_le", &p_int_lin_le);
        registry().add("int_lin_le_reif", &p_int_lin_le_reif);
        registry().add("int_plus", &p_int_plus);
        registry().add("int_times", &p_int_times);
        registry().add("int_div", &p_int_div);
        registry().add("int_mod", &p_int_mod);
        registry().add("int_min", &p_int_min);
        registry().add("int_max", &p_int_max);
        registry().add("int_abs", &p_abs);
        registry().add("bool_eq", &p_bool_eq);
        registry().add("bool_eq_reif", &p_bool_eq_reif);
        registry().add("bool_le", &p_bool_le);
        registry().add("bool_le_reif", &p_bool_le_reif);
        registry().add("bool_lt", &p_bool_lt);
        registry().add("bool_lt_reif", &p_bool_lt_reif);
        registry().add("bool_or", &p_bool_or);
        registry().add("bool_and", &p_bool_and);
        registry().add("bool_xor", &p_bool_xor);
        //registry().add("array_bool_and", &p_array_bool_and);
        //registry().add("array_bool_or", &p_array_bool_or);
        //registry().add("bool_clause", &p_array_bool_clause);
        registry().add("bool_not", &p_bool_not);
        registry().add("array_int_element", &p_array_int_element);
        registry().add("array_var_int_element", &p_array_int_element);
        registry().add("array_bool_element", &p_array_bool_element);
        registry().add("array_var_bool_element", &p_array_bool_element);
        registry().add("bool2int", &p_bool2int);

        registry().add("int_in", &p_int_in);
        //registry().add("bool_lin_eq", &p_bool_lin_eq);
        //registry().add("bool_lin_le", &p_bool_lin_le);

        // ---- std library: ----

        registry().add("all_different_int", &p_int_distinct);
        registry().add("lex_lesseq_int", &p_int_lex_lesseq);
        registry().add("table_int", &p_int_table);
        registry().add("table_bool", &p_bool_table);
      }
    };
    IntPoster __int_poster;

    void p_set_union(FlatZincModel& s, const ConExpr& ce, AST::Node *ann) {
      p_rel3<Casper::UnionEqual,Casper::IntSet,Casper::IntSet,Casper::IntSet>(s,ce,ann);
    }
    void p_set_intersect(FlatZincModel& s, const ConExpr& ce, AST::Node *ann) {
      p_rel3<Casper::IntersectEqual,Casper::IntSet,Casper::IntSet,Casper::IntSet>(s,ce,ann);
    }
    void p_set_diff(FlatZincModel& s, const ConExpr& ce, AST::Node *ann) {
      std::cerr << "set_diff("<<(*ce[0])<<","<<(*ce[1])<<","<<(*ce[2])
        <<")::"<<(*ann)<<"\n";
    }

    void p_set_symdiff(FlatZincModel& s, const ConExpr& ce, AST::Node* ann) {
      std::cerr << "set_symdiff("<<(*ce[0])<<","<<(*ce[1])<<","<<(*ce[2])
        <<")::"<<(*ann)<<"\n";
    }

    void p_set_eq(FlatZincModel& s, const ConExpr& ce, AST::Node * ann) {
      p_rel2<Casper::Equal,Casper::IntSet,Casper::IntSet>(s,ce,ann);
    }
    void p_set_ne(FlatZincModel& s, const ConExpr& ce, AST::Node * ann) {
      p_rel2<Casper::Distinct,Casper::IntSet,Casper::IntSet>(s,ce,ann);
    }
    void p_set_subset(FlatZincModel& s, const ConExpr& ce, AST::Node * ann) {
      p_rel2<Casper::Contained,Casper::IntSet,Casper::IntSet>(s,ce,ann);
    }
    void p_set_card(FlatZincModel& s, const ConExpr& ce, AST::Node * ann) {
      p_rel2_1_2<Casper::Cardinal,Casper::Equal,Casper::IntSet,int>(s,ce,ann);
    }
    void p_set_in(FlatZincModel& s, const ConExpr& ce, AST::Node * ann) {
      p_rel2<Casper::Member,int,Casper::IntSet>(s,ce,ann);
    }
    void p_set_eq_reif(FlatZincModel& s, const ConExpr& ce, AST::Node * ann) {
      p_rel3_2_2<Casper::Equal,Casper::Equal,Casper::IntSet,Casper::IntSet,bool>(s,ce,ann);
    }
    void p_set_ne_reif(FlatZincModel& s, const ConExpr& ce, AST::Node * ann) {
      p_rel3_2_2<Casper::Distinct,Casper::Equal,Casper::IntSet,Casper::IntSet,bool>(s,ce,ann);
    }
    void p_set_subset_reif(FlatZincModel& s, const ConExpr& ce,
                           AST::Node *ann) {
      p_rel3_2_2<Casper::Contained,Casper::Equal,Casper::IntSet,Casper::IntSet,bool>(s,ce,ann);
    }
    void p_set_in_reif(FlatZincModel& s, const ConExpr& ce, AST::Node *ann) {
      p_rel3_2_2<Casper::Member,Casper::Equal,int,Casper::IntSet,bool>(s,ce,ann);
    }
    void p_set_disjoint(FlatZincModel& s, const ConExpr& ce, AST::Node *ann) {
      p_rel2<Casper::Disjoint,Casper::IntSet,Casper::IntSet>(s,ce,ann);
    }

    void p_array_set_element(FlatZincModel& s, const ConExpr& ce,
                             AST::Node* ann) {
      std::cerr << "array_set_element("<<(*ce[0])<<","<<(*ce[1])<<","<<(*ce[2])
        <<")::"<<(*ann)<<"\n";
    }


    class SetPoster {
    public:
      SetPoster(void) {
        registry().add("set_eq", &p_set_eq);
        registry().add("set_ne", &p_set_ne);
        registry().add("set_union", &p_set_union);
        //registry().add("array_set_element", &p_array_set_element);
        //registry().add("array_var_set_element", &p_array_set_element);
        registry().add("set_intersect", &p_set_intersect);
        //registry().add("set_diff", &p_set_diff);
        //registry().add("set_symdiff", &p_set_symdiff);
        registry().add("set_subset", &p_set_subset);
        registry().add("set_card", &p_set_card);
        registry().add("set_in", &p_set_in);
        registry().add("set_eq_reif", &p_set_eq_reif);
        registry().add("equal_reif", &p_set_eq_reif);
        registry().add("set_ne_reif", &p_set_ne_reif);
        registry().add("set_subset_reif", &p_set_subset_reif);
        registry().add("set_in_reif", &p_set_in_reif);
        registry().add("disjoint", &p_set_disjoint);

        // ---- std library: ----
        //registry().add("partition_set", &p_set_partition);

      }
    };
    SetPoster __set_poster;

  }
}

// STATISTICS: flatzinc-any
