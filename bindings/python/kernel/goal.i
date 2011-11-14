
%import "swigutil.i"

%module kernel 
%{
#include <casper/kernel.h>
%}

// Goal [TODO]
%include <casper/kernel/goal/goal.h>

%include <kernel/goal_operators.i>
