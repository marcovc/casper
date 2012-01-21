
%import "swigutil.i"

%module kernel 
%feature("autodoc", "1");
%{
#include <casper/kernel.h>
#include <bindings/python/kernel/swigtypes.h>
%}

// global definitions to remain hidden [hidden]
%import <casper/kernel/common.h>
%import <casper/kernel/traits.h>

%include <kernel/env.i>

%include <kernel/expr.i>

%include <kernel/goal.i>
%include <kernel/explorer.i>
%include <kernel/ref.i>

%{
    #include <casper/version.h>
%}

%include <casper/version.h>



%init %{
Casper::pIntSWIGKernelTypeInfo = new Casper::SWIGKernelTypeInfo<int>();
Casper::pBoolSWIGKernelTypeInfo  = new Casper::SWIGKernelTypeInfo<bool>();;
Casper::pIntSeqSWIGKernelTypeInfo = new Casper::SWIGKernelTypeInfo<Casper::Seq<int> >();
Casper::pBoolSeqSWIGKernelTypeInfo = new Casper::SWIGKernelTypeInfo<Casper::Seq<bool> >();
%}
