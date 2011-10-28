%module cp 

%{
#include <casper/kernel.h>
#include <casper/cp.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>


// Var
%ignore Casper::CP::Var::domain;
%ignore Casper::CP::Var::pDomain;
%ignore Casper::CP::Var::operator=;
%include <casper/cp/var.h>

COUTWRAPPER(Casper::CP::Var)
