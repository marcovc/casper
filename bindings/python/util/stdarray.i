%module util 

%{
#include <casper/kernel/common.h>
#include <casper/util/container/stdarray.h>
#include <casper/util/memory.h>
%}

%import <swigutil.i>

%import <casper/kernel/common.h>
%import <casper/util/memory.h>

%ignore Casper::Util::StdArray::operator=;
%ignore Casper::Util::StdArray::operator==;
%ignore Casper::Util::StdArray::operator!=;
//%ignore Casper::Util::StdArray::operator[];
%ignore Casper::Util::StdArray::begin;
%ignore Casper::Util::StdArray::end;
%ignore Casper::Util::StdArray::getHeap;


%import <casper/util/container/stdarray.h>

COUTWRAPPER(Casper::Util::StdArray)


// base classes, not explicitely instantiated, but methods must be known to derived classes
%ignore Casper::Util::StdArray< Casper::CP::Var< int,Casper::CP::Traits::GetDefaultDom< int >::Type >,1 >::operator==;
%template() Casper::Util::StdArray< Casper::CP::Var< int,Casper::CP::Traits::GetDefaultDom< int >::Type >,1 >;
%template() Casper::Util::StdArray< Casper::CP::Var< bool,Casper::CP::Traits::GetDefaultDom< bool >::Type >,1 >;
%template() Casper::Util::StdArray< Casper::CP::Var< int,Casper::CP::Traits::GetDefaultDom< int >::Type >,2 >;
%template() Casper::Util::StdArray< Casper::CP::Var< bool,Casper::CP::Traits::GetDefaultDom< bool >::Type >,2 >;
%template() Casper::Util::StdArray< Casper::CP::Var< int,Casper::CP::Traits::GetDefaultDom< int >::Type >,3 >;
%template() Casper::Util::StdArray< Casper::CP::Var< bool,Casper::CP::Traits::GetDefaultDom< bool >::Type >,3 >;

