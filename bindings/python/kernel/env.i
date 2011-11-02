
%import "swigutil.i"

%module kernel 
%{
#include <casper/kernel.h>
%}

// Env [TODO]
%import <casper/kernel/state/env.h>

%include <casper/kernel/state/state.h>
