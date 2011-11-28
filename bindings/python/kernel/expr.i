//%import "swigutil.i"

%module kernel 
%{
#include <casper/kernel.h>
#include <casper/cp.h>
#include <casper/kernel/obj/expr.h>
#include <casper/kernel/goal/whiledo.h>
#include <bindings/python/kernel/expr.h>
%}

//%import <cp/view.i>

// Expr

// typemaps

%define SET_EXPR_SWIG_TYPE_DESCR(Eval)
Casper::Detail::CreateFromPyObject<Eval>::isExpr = $descriptor(Casper::Expr<Eval>*);
Casper::Detail::CreateFromPyObject<Eval>::isCPVar = 
			$descriptor(Casper::CP::Var<Eval,Casper::CP::Traits::GetDefaultDom<Eval>::Type >*);
Casper::Detail::CreateFromPyObject<Eval>::isPar = $descriptor(Casper::Par<Eval>*);
Casper::Detail::CreateFromPyObject<Eval>::isGoal = $descriptor(Casper::Goal*);				
%enddef

%define SET_SEQEXPR_SWIG_TYPE_DESCR(Eval)
Casper::Detail::CreateFromPyObject<Casper::Seq<Eval> >::isSeqExpr = $descriptor(Casper::Expr<Casper::Seq<Eval> >*);
Casper::Detail::CreateFromPyObject<Casper::Seq<Eval> >::isCPVarArray1 = 
		$descriptor(Casper::CP::VarArray<Eval,1,Casper::CP::Traits::GetDefaultDom<Eval>::Type >*);
Casper::Detail::CreateFromPyObject<Casper::Seq<Eval> >::isCPVarArray2 = 
		$descriptor(Casper::CP::VarArray<Eval,2,Casper::CP::Traits::GetDefaultDom<Eval>::Type >*);
SET_EXPR_SWIG_TYPE_DESCR(Eval)
%enddef
	

// int


%typemap(in) Casper::Expr<int> const&
{
	//std::cout << "in Expr<int> typemap\n";
	
	SET_EXPR_SWIG_TYPE_DESCR(int)	
		
	$1 = Casper::Detail::CreateFromPyObject<int>::create($input);
};

%typemap(typecheck,precedence=0) Casper::Expr<int> const&
{
	//std::cout << "in Expr<int> typecheck\n";
	
	SET_EXPR_SWIG_TYPE_DESCR(int)
		
	$1 = Casper::Detail::CreateFromPyObject<int>::check($input);
};

// bool

%typemap(in) Casper::Expr<bool> const&
{
	//std::cout << "in Expr<bool> typemap\n";
	
	SET_EXPR_SWIG_TYPE_DESCR(bool)
	
	$1 = Casper::Detail::CreateFromPyObject<bool>::create($input);
};

%typemap(typecheck,precedence=0) Casper::Expr<bool> const&
{
	//std::cout << "in Expr<bool> typecheck\n";

	SET_EXPR_SWIG_TYPE_DESCR(bool)

	$1 = Casper::Detail::CreateFromPyObject<bool>::check($input);
};

// Seq<int>

%typemap(in) Casper::Expr<Casper::Seq<int> > const&
{
	//std::cout << "in Expr<Seq<int> > typemap\n";
	
	SET_SEQEXPR_SWIG_TYPE_DESCR(int)
	
	$1 = Casper::Detail::CreateFromPyObject<Casper::Seq<int> >::create($input);
};

%typemap(typecheck,precedence=0) Casper::Expr<Casper::Seq<int> > const&
{
	//std::cout << "in Expr<Seq<int> > typecheck\n";

	SET_SEQEXPR_SWIG_TYPE_DESCR(int)
	
	$1 = Casper::Detail::CreateFromPyObject<Casper::Seq<int> >::check($input);
};

// Seq<bool>

%typemap(in) Casper::Expr<Casper::Seq<bool> > const&
{
	//std::cout << "in Expr<Seq<bool> > typemap\n";
	
	SET_SEQEXPR_SWIG_TYPE_DESCR(bool)
	
	$1 = Casper::Detail::CreateFromPyObject<Casper::Seq<bool> >::create($input);
};

%typemap(typecheck,precedence=0) Casper::Expr<Casper::Seq<bool> > const&
{
	//std::cout << "in Expr<Seq<bool> > typecheck\n";

	SET_SEQEXPR_SWIG_TYPE_DESCR(bool)
	
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
//DEFINE_EXPR(int)
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

// Ambiguous predicates handlers (predicates that are both casper and python reserved words)

%pythoncode %{
def min(*args,**kargs):
	import numbers
	if len(kargs.keys())>0 or \
		len(args)>2 or \
		all([isinstance(arg,numbers.Number) for arg in args]):
		import __builtin__
		return __builtin__.min(*args,**kargs)
	else:
		return _min(*args,**kargs)

def max(*args,**kargs):
	import numbers
	if len(kargs.keys())>0 or \
		all([isinstance(arg,numbers.Number) for arg in args]):
		import __builtin__
		return __builtin__.max(*args,**kargs)
	else:
		return _max(*args,**kargs)

def abs(arg):
	import numbers
	if isinstance(arg,numbers.Number):
		import __builtin__
		return __builtin__.abs(arg)
	else:
		return _abs(arg)

def sum(*args,**kwargs):
	import collections
	if len(kargs.keys())>0 or \
		isinstance(args[0],collections.Iterable):
		import __builtin__
		return __builtin__.sum(*args,**kargs)
	else:
		return _sum(*args,**kargs)
	
def pow(*args):
	import numbers
	if all([isinstance(arg,numbers.Number) for arg in args]):
		import __builtin__
		return __builtin__.pow(*args)
	else:
		return _pow(*args)		
%}
