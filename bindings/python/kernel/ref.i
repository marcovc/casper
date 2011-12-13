%module kernel 

%{
#include <casper/kernel.h>
%}

%import <swigutil.i>
%import <util.i>

// Ref
%include <casper/kernel/ref/ref.h>
%extend Casper::Ref {
	//%template(assign) operator= <Casper::Expr<T> >;
	void assign(const Casper::Expr<T>& s)
	{	$self->operator=(s);	}
	%template(Ref) Ref<Casper::Expr<T> >;
}
COUTWRAPPER(Casper::Ref)

%template(IntRef) Casper::Ref<int>;

%include <kernel/intref_operators.i>
%include <kernel/boolref_operators.i>