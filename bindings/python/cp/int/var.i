%module cp 

%{
#include <casper/kernel.h>
#include <casper/cp.h>
#include <casper/kernel/obj/expr.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>

%import <cp/var.i>
%import <kernel/expr.i>

%template(IntVar) Casper::CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type>;
%template(BoolVar) Casper::CP::Var<bool,Casper::CP::Traits::GetDefaultDom<bool>::Type>;

%extend Casper::CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type> {
	%template(IntVar) Var<int>;
	%template(IntVar) Var<int,int>;
}

%extend Casper::CP::Var<bool,Casper::CP::Traits::GetDefaultDom<bool>::Type> {
	%template(BoolVar) Var<bool>;
	%template(BoolVar) Var<bool,bool>;
}

%inline %{
Casper::Expr<int> myMethod(const Casper::Expr<int>& s)
{	return s;	}
%}

%include <cp/int/intvar_operators.i>
%include <cp/int/boolvar_operators.i>
