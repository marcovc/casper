
%import "swigutil.i"

%module kernel 
%{
#include <casper/kernel.h>
%}

// Env [TODO]
%import <casper/kernel/state/env.h>
