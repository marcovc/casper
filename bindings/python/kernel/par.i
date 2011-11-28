%module kernel 

%{
#include <casper/kernel.h>
%}

%import <swigutil.i>
%import <util.i>

// Par
%include <casper/kernel/par/par.h>
%extend Casper::Par {
	//%template(assign) operator= <Casper::Expr<T> >;
	void assign(const Casper::Expr<T>& s)
	{	$self->operator=(s);	}
	%template(Par) Par<Casper::Expr<T> >;
}
COUTWRAPPER(Casper::Par)

%template(IntPar) Casper::Par<int>;

%include <kernel/intpar_operators.i>
%include <kernel/boolpar_operators.i>