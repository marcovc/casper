%module cp 

%{
#include <casper/kernel.h>
#include <casper/cp.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>

%import <cp/vararray.i>

// base classes, not explicitely instantiated, but methods must be known to derived classes
%ignore Casper::Util::StdArray< Casper::CP::Var< int,Casper::CP::Traits::GetDefaultDom< int >::Type >,1 >::operator==;
%template() Casper::Util::StdArray< Casper::CP::Var< int,Casper::CP::Traits::GetDefaultDom< int >::Type >,1 >;
%template() Casper::Util::StdArray< Casper::CP::Var< bool,Casper::CP::Traits::GetDefaultDom< bool >::Type >,1 >;


%template(IntVarArray) Casper::CP::VarArray<int,1,Casper::CP::Traits::GetDefaultDom<int>::Type>;
%template(BoolVarArray) Casper::CP::VarArray<bool,1,Casper::CP::Traits::GetDefaultDom<bool>::Type>;


%extend Casper::CP::VarArray<int,1,Casper::CP::Traits::GetDefaultDom<int>::Type> {
	%template(IntVarArray) VarArray<int>;
	%template(IntVarArray) VarArray<int,int>;
}
%extend Casper::CP::VarArray<bool,1,Casper::CP::Traits::GetDefaultDom<bool>::Type> {
	%template(BoolVarArray) VarArray<bool>;
	%template(BoolVarArray) VarArray<bool,bool>;
}
