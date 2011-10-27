%import "swigutil.i"

%module kernel 
%{
#include <casper/kernel.h>
#include <casper/kernel/obj/expr.h>
%}

%import <cp/view.i>

// Expr
%include <casper/kernel/obj/expr.h>
%template(IntExpr) Casper::Expr<int>;
%template(BoolExpr) Casper::Expr<bool>;

%include <kernel/boolexpr_operators.i>
%include <kernel/intexpr_operators.i>
%include <kernel/expr_predicates.i>
