%module util 

%{
#include <casper/kernel/common.h>
#include <casper/util/container/stdarray.h>
#include <casper/util/memory.h>
%}

%import <swigutil.i>

/*
%import <casper/kernel/common.h>
%import <casper/util/memory.h>

%ignore Casper::Util::StdArray::operator=;
%ignore Casper::Util::StdArray::operator[];
%ignore Casper::Util::StdArray::begin;
%ignore Casper::Util::StdArray::end;
%ignore Casper::Util::StdArray::getHeap;
%include <casper/util/container/stdarray.h>

%template(IntArray) Casper::Util::StdArray<int,1>;

COUTWRAPPER(Casper::Util::StdArray)
*/

