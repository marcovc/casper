%module cp 

%{
#include <casper/kernel.h>
#include <casper/cp.h>
#include <bindings/python/kernel/expr.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>

// Post
%import <casper/kernel/goal/store.h>
%template(post) Casper::post<Casper::CP::Store,Casper::Expr<bool> >;

