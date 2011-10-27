%module cp 

%{
#include <casper/kernel.h>
#include <casper/cp.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>

%import <casper/cp/traits.h>

// BndExpr [hidden]
%import <casper/cp/expr.h>