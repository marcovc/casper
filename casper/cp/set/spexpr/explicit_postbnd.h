/* THIS FILE WAS AUTOGENERATED FROM post.h.py */
namespace Casper {
template<class> struct Expr;
namespace CP {
extern template struct PostBndFilter2<Casper::Disjoint,Casper::BoolSet,CP::Var<Casper::BoolSet>,Casper::BoolSet,CP::Var<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Disjoint,Casper::BoolSet,CP::Var<Casper::BoolSet>,Casper::BoolSet,Expr<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Disjoint,Casper::BoolSet,Expr<Casper::BoolSet>,Casper::BoolSet,CP::Var<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Disjoint,Casper::IntSet,CP::Var<Casper::IntSet>,Casper::IntSet,CP::Var<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Disjoint,Casper::IntSet,CP::Var<Casper::IntSet>,Casper::IntSet,Expr<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Disjoint,Casper::IntSet,Expr<Casper::IntSet>,Casper::IntSet,CP::Var<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Contained,Casper::BoolSet,CP::Var<Casper::BoolSet>,Casper::BoolSet,CP::Var<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Contained,Casper::BoolSet,CP::Var<Casper::BoolSet>,Casper::BoolSet,Expr<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Contained,Casper::BoolSet,Expr<Casper::BoolSet>,Casper::BoolSet,CP::Var<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Contained,Casper::IntSet,CP::Var<Casper::IntSet>,Casper::IntSet,CP::Var<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Contained,Casper::IntSet,CP::Var<Casper::IntSet>,Casper::IntSet,Expr<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Contained,Casper::IntSet,Expr<Casper::IntSet>,Casper::IntSet,CP::Var<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Distinct,Casper::BoolSet,CP::Var<Casper::BoolSet>,Casper::BoolSet,CP::Var<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Distinct,Casper::BoolSet,CP::Var<Casper::BoolSet>,Casper::BoolSet,Expr<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Distinct,Casper::BoolSet,Expr<Casper::BoolSet>,Casper::BoolSet,CP::Var<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Distinct,Casper::IntSet,CP::Var<Casper::IntSet>,Casper::IntSet,CP::Var<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Distinct,Casper::IntSet,CP::Var<Casper::IntSet>,Casper::IntSet,Expr<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Distinct,Casper::IntSet,Expr<Casper::IntSet>,Casper::IntSet,CP::Var<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Equal,Casper::BoolSet,CP::Var<Casper::BoolSet>,Casper::BoolSet,CP::Var<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Equal,Casper::BoolSet,CP::Var<Casper::BoolSet>,Casper::BoolSet,Expr<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Equal,Casper::BoolSet,Expr<Casper::BoolSet>,Casper::BoolSet,CP::Var<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Equal,Casper::IntSet,CP::Var<Casper::IntSet>,Casper::IntSet,CP::Var<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Equal,Casper::IntSet,CP::Var<Casper::IntSet>,Casper::IntSet,Expr<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Equal,Casper::IntSet,Expr<Casper::IntSet>,Casper::IntSet,CP::Var<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Member,int,Expr<int>,Casper::IntSet,CP::Var<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::NotMember,bool,Expr<bool>,Casper::BoolSet,CP::Var<Casper::BoolSet> >;
extern template struct PostBndFilter1<Casper::Ground,Casper::IntSet,CP::Var<Casper::IntSet> >;
extern template struct PostBndFilter1<Casper::Ground,Casper::BoolSet,CP::Var<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Disjoint,Casper::BoolSet,Expr<Casper::BoolSet>,Casper::BoolSet,Expr<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Disjoint,Casper::IntSet,Expr<Casper::IntSet>,Casper::IntSet,Expr<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Contained,Casper::BoolSet,Expr<Casper::BoolSet>,Casper::BoolSet,Expr<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Contained,Casper::IntSet,Expr<Casper::IntSet>,Casper::IntSet,Expr<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Distinct,Casper::BoolSet,Expr<Casper::BoolSet>,Casper::BoolSet,Expr<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Distinct,Casper::IntSet,Expr<Casper::IntSet>,Casper::IntSet,Expr<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Equal,Casper::BoolSet,Expr<Casper::BoolSet>,Casper::BoolSet,Expr<Casper::BoolSet> >;
extern template struct PostBndFilter2<Casper::Equal,Casper::IntSet,Expr<Casper::IntSet>,Casper::IntSet,Expr<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::Member,int,Expr<int>,Casper::IntSet,Expr<Casper::IntSet> >;
extern template struct PostBndFilter2<Casper::NotMember,bool,Expr<bool>,Casper::BoolSet,Expr<Casper::BoolSet> >;
extern template struct PostBndFilter3<Casper::IntersectEqual,Casper::IntSet,Expr<Casper::IntSet>,Casper::IntSet,Expr<Casper::IntSet>,Casper::IntSet,Expr<Casper::IntSet> >;
extern template struct PostBndFilter3<Casper::UnionEqual,Casper::IntSet,Expr<Casper::IntSet>,Casper::IntSet,Expr<Casper::IntSet>,Casper::IntSet,Expr<Casper::IntSet> >;
extern template struct PostBndFilter3<Casper::IntersectEqual,Casper::BoolSet,Expr<Casper::BoolSet>,Casper::BoolSet,Expr<Casper::BoolSet>,Casper::BoolSet,Expr<Casper::BoolSet> >;
extern template struct PostBndFilter3<Casper::UnionEqual,Casper::BoolSet,Expr<Casper::BoolSet>,Casper::BoolSet,Expr<Casper::BoolSet>,Casper::BoolSet,Expr<Casper::BoolSet> >;
extern template struct PostBndFilter1<Casper::Ground,Casper::IntSet,Expr<Casper::IntSet> >;
extern template struct PostBndFilter1<Casper::Ground,Casper::BoolSet,Expr<Casper::BoolSet> >;
extern template struct PostBndFilter1<Casper::Ground,Casper::BoolSetSeq,Expr<Casper::BoolSetSeq> >;
extern template struct PostBndFilter1<Casper::Ground,Casper::IntSetSeq,Expr<Casper::IntSetSeq> >;
extern template struct PostBndFilter1<Casper::Disjoint,Casper::BoolSetSeq,Expr<Casper::BoolSetSeq> >;
extern template struct PostBndFilter1<Casper::Disjoint,Casper::IntSetSeq,Expr<Casper::IntSetSeq> >;
extern template struct PostBndFilter1<Casper::Partition,Casper::BoolSetSeq,Expr<Casper::BoolSetSeq> >;
extern template struct PostBndFilter1<Casper::Partition,Casper::IntSetSeq,Expr<Casper::IntSetSeq> >;
}
}
