%module cp 

%{
#include <casper/kernel.h>
#include <casper/cp.h>
#include <casper/kernel/obj/expr.h>
#include <bindings/python/util/iterator.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>


// real FD class is not exposed, instead the following is used
%inline %{
namespace Casper {
namespace CP {

struct IntVarDom 
{
	IntVarDom(const Casper::CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type>& v) : v(v) {}
	int __len__() {	return v.domain().size();	}
#ifndef SWIG
	Casper::CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type> v;
#endif	
};

struct BoolVarDom 
{
	BoolVarDom(const Casper::CP::Var<bool,Casper::CP::Traits::GetDefaultDom<bool>::Type>& v) : v(v) {}
	int __len__() {	return v.domain().size();	}
#ifndef SWIG
	Casper::CP::Var<bool,Casper::CP::Traits::GetDefaultDom<bool>::Type> v;
#endif	
};

}
}
%}

ITERATOR_WRAPPER(%arg(IntVarDomIterator),
				 %arg(Casper::CP::IntVarDom),
				 %arg(Casper::CP::Traits::GetDefaultDom<int>::Type),
				 %arg(Casper::CP::Traits::GetDefaultDom<int>::Type::Iterator),
				 %arg(int),
				 %arg($self->v.domain()))

ITERATOR_WRAPPER(%arg(BoolVarDomIterator),
				 %arg(Casper::CP::BoolVarDom),
				 %arg(Casper::CP::Traits::GetDefaultDom<bool>::Type),
				 %arg(Casper::CP::Traits::GetDefaultDom<bool>::Type::Iterator),
				 %arg(bool),
				 %arg($self->v.domain()))
