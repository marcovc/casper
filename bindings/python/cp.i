
%module cp 
%feature("autodoc", "1");

%{
#include <casper/kernel.h>
#include <casper/cp.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>

%include <cp/store.i>
%include <cp/solver.i>
%include <cp/var.i>
%include <cp/int/var.i>
%include <cp/vararray.i>
%include <cp/int/vararray.i>
%include <cp/label.i>

