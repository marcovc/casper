%module cp 

%{
#include <casper/kernel.h>
#include <casper/cp.h>
#include <bindings/python/kernel/expr.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>

// Store
%import <cp/store.i>

// Solver,SolverStats
%ignore Casper::CP::Solver::getStats();
%ignore Casper::CP::Solver::operator Util::IHeap&;
%ignore Casper::CP::Solver::operator const Util::IHeap&;
%ignore Casper::CP::Solver::operator State&;
%ignore Casper::CP::Solver::operator const State&;
%include <casper/cp/solver.h>
COUTWRAPPER(Casper::CP::SolverStats)
