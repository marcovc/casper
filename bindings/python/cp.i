
%module cp 
%feature("autodoc", "1");

%{
#include <casper/kernel.h>
#include <casper/cp.h>
#include <casper/cp/spexpr/expr.h>
#include <bindings/python/cp/swigtypes.h>
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
%include <cp/goal.i>


%init %{
Casper::pIntSWIGCPTypeInfo = new Casper::SWIGCPTypeInfo<int>();
Casper::pBoolSWIGCPTypeInfo  = new Casper::SWIGCPTypeInfo<bool>();;
Casper::pIntSeqSWIGCPTypeInfo = new Casper::SWIGCPTypeInfo<Casper::Seq<int> >();
Casper::pBoolSeqSWIGCPTypeInfo = new Casper::SWIGCPTypeInfo<Casper::Seq<bool> >();
%}
