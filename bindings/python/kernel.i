
%import "swigutil.i"

%module kernel 
%feature("autodoc", "1");
%{
#include <casper/kernel.h>
%}

// global definitions to remain hidden [hidden]
%import <casper/kernel/common.h>
%import <casper/kernel/traits.h>

%include <kernel/env.i>

%include <kernel/expr.i>

%include <kernel/goal.i>
%include <kernel/explorer.i>
%include <kernel/ref.i>

