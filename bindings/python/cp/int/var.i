%module cp 

%{
#include <casper/kernel.h>
#include <casper/cp.h>
#include <casper/kernel/obj/expr.h>
#include <bindings/python/util/iterator.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>

%import <cp/var.i>
%import <kernel/expr.i>

%include <bindings/python/cp/int/fd.i>

%template(IntVar) Casper::CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type>;
%template(BoolVar) Casper::CP::Var<bool,Casper::CP::Traits::GetDefaultDom<bool>::Type>;

						
%extend Casper::CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type> {
	%template(IntVar) Var<int>;
	%template(IntVar) Var<int,int>;
	Casper::CP::IntVarDom domain() { return Casper::CP::IntVarDom(*$self); }
}

%extend Casper::CP::Var<bool,Casper::CP::Traits::GetDefaultDom<bool>::Type> {
	%template(BoolVar) Var<bool>;
	%template(BoolVar) Var<bool,bool>;
	Casper::CP::BoolVarDom domain() { return Casper::CP::BoolVarDom(*$self); }
}


%include <cp/int/intvar_operators.i>
%include <cp/int/boolvar_operators.i>
