
%import "swigutil.i"

%module kernel 
%feature("autodoc", "1");
%{
#include <casper/kernel.h>
%}

// global definitions to remain hidden [hidden]
%import <casper/kernel/common.h>
%import <casper/kernel/traits.h>

// State [hidden?]
%ignore Casper::State::operator Util::IHeap&;
%ignore Casper::State::operator const Util::IHeap&;
%import <casper/kernel/state/state.h>

%include <kernel/expr.i>

%include <kernel/goal.i>
%include <kernel/explorer.i>
