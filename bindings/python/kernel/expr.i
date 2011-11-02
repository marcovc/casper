//%import "swigutil.i"

%module kernel 
%{
#include <casper/kernel.h>
#include <casper/cp.h>
#include <casper/kernel/obj/expr.h>
#include <bindings/python/kernel/expr.h>
%}

//%import <cp/view.i>

// Expr

// typemaps

// int

%typemap(in) Casper::Expr<int> const&
{
	std::cout << "in Expr<int> typemap\n";
	$1 = Casper::Detail::CreateFromPyObject<int>::create($input);
};

%typemap(typecheck,precedence=0) Casper::Expr<int> const&
{
	std::cout << "in Expr<int> typecheck\n";
	$1 = Casper::Detail::CreateFromPyObject<int>::check($input);
};

// bool

%typemap(in) Casper::Expr<bool> const&
{
	std::cout << "in Expr<bool> typemap\n";
	$1 = Casper::Detail::CreateFromPyObject<bool>::create($input);
};

%typemap(typecheck,precedence=0) Casper::Expr<bool> const&
{
	std::cout << "in Expr<bool> typecheck\n";
	$1 = Casper::Detail::CreateFromPyObject<bool>::check($input);
};

// Seq<int>

%typemap(in) Casper::Expr<Casper::Seq<int> > const&
{
	std::cout << "in Expr<Seq<int> > typemap\n";
	$1 = Casper::Detail::CreateFromPyObject<Casper::Seq<int> >::create($input);
};

%typemap(typecheck,precedence=0) Casper::Expr<Casper::Seq<int> > const&
{
	std::cout << "in Expr<Seq<int> > typecheck\n";
	$1 = Casper::Detail::CreateFromPyObject<Casper::Seq<int> >::check($input);
};

// Seq<bool>

%typemap(in) Casper::Expr<Casper::Seq<bool> > const&
{
	std::cout << "in Expr<Seq<bool> > typemap\n";
	$1 = Casper::Detail::CreateFromPyObject<Casper::Seq<bool> >::create($input);
};

%typemap(typecheck,precedence=0) Casper::Expr<Casper::Seq<bool> > const&
{
	std::cout << "in Expr<Seq<bool> > typecheck\n";
	$1 = Casper::Detail::CreateFromPyObject<Casper::Seq<bool> >::check($input);
};


%import <casper/util/pimpl.h>

%template() Casper::Util::SPImplIdiom< Casper::Detail::IExpr< bool > >;
%template() Casper::Util::SPImplIdiom< Casper::Detail::IExpr< int > >;
%template() Casper::Util::SPImplIdiom< Casper::Detail::IExpr< Casper::Seq<bool> > >;
%template() Casper::Util::SPImplIdiom< Casper::Detail::IExpr< Casper::Seq<int> > >;

%include <casper/kernel/obj/expr.h>

// This is needed since SWIG confuses partial specializations (bug #3431508)

%define DEFINE_EXPR(Eval)
template<>
struct Expr<Eval> : Casper::Util::SPImplIdiom<Detail::IExpr<Eval> >
{
	typedef Casper::Util::SPImplIdiom<Detail::IExpr<Eval> > Super;
	Expr(const Expr& expr) : Super(expr) {}
};
%enddef

%define DEFINE_SEQEXPR(Eval)
template<>
struct Expr<Casper::Seq<Eval> > : Casper::Util::SPImplIdiom<Detail::IExpr<Casper::Seq<Eval> > >
{
	typedef Casper::Util::SPImplIdiom<Detail::IExpr<Casper::Seq<Eval> > > Super;
	Expr(const Expr& expr) : Super(expr) {}
};
%enddef
namespace Casper {
DEFINE_EXPR(int)
DEFINE_SEQEXPR(int)
DEFINE_SEQEXPR(bool)
}

%template(IntExpr) Casper::Expr<int>;
%template(BoolExpr) Casper::Expr<bool>;
%template(IntSeqExpr) Casper::Expr<Casper::Seq<int> >;
%template(BoolSeqExpr) Casper::Expr<Casper::Seq<bool> >;
 

%include <kernel/boolexpr_operators.i>
%include <kernel/intexpr_operators.i>
%include <kernel/expr_predicates.i>


