%module util 

%{
#include <casper/kernel/common.h>
#include <casper/util/container/stdarray.h>
#include <casper/util/memory.h>
#include <bindings/python/kernel/expr.h>
%}

%import <swigutil.i>
//%import <kernel.i>

%import <casper/kernel/common.h>
%import <casper/util/memory.h>

%ignore Casper::Util::StdArray::operator=;
%ignore Casper::Util::StdArray::operator==;
%ignore Casper::Util::StdArray::operator!=;
//%ignore Casper::Util::StdArray::operator[];
%ignore Casper::Util::StdArray::begin;
%ignore Casper::Util::StdArray::end;
%ignore Casper::Util::StdArray::getHeap;

%rename(__len__) Casper::Util::StdArray::size;

%import <casper/util/container/stdarray.h>

COUTWRAPPER(Casper::Util::StdArray)

%define WRAP_STDARRAY_INDEX_OP(T,Dims,Elem)
%template() Casper::Rel2<Casper::Element,Casper::Util::StdArray<T,Dims>,Casper::Expr<int> >;
%extend Casper::Util::StdArray<T,Dims>
{
	Elem getElemInt(int i) const
	{	return $self->operator[](i);	}

	Casper::Rel2<Casper::Element,Casper::Util::StdArray<T,Dims>,Casper::Expr<int> >	getElemExpr(const Casper::Expr<int>& t) const
	{	return	Casper::Rel2<Casper::Element,Casper::Util::StdArray<T,Dims>,Casper::Expr<int> >(*$self,t);	}
	
	%pythoncode
	{
		def __getitem__(self,i):
			import numbers
			if not isinstance(i,numbers.Number):
				return self.getElemExpr(i)
			elif i>=self.__len__() or i<0:
				raise IndexError
			else:
				return self.getElemInt(i)
	}
	
	void __setitem__(int i,const T& v) 
	{ $self->operator[](i) = v; }		
}
%enddef

// base classes, not explicitely instantiated, but methods must be known to derived classes
%ignore Casper::Util::StdArray< Casper::CP::Var< int,Casper::CP::Traits::GetDefaultDom< int >::Type >,1 >::operator==;
%template() Casper::Util::StdArray< Casper::CP::Var< int,Casper::CP::Traits::GetDefaultDom< int >::Type >,1 >;
%template() Casper::Util::StdArray< Casper::CP::Var< bool,Casper::CP::Traits::GetDefaultDom< bool >::Type >,1 >;
%template() Casper::Util::StdArray< Casper::CP::Var< int,Casper::CP::Traits::GetDefaultDom< int >::Type >,2 >;
%template() Casper::Util::StdArray< Casper::CP::Var< bool,Casper::CP::Traits::GetDefaultDom< bool >::Type >,2 >;
%template() Casper::Util::StdArray< Casper::CP::Var< int,Casper::CP::Traits::GetDefaultDom< int >::Type >,3 >;
%template() Casper::Util::StdArray< Casper::CP::Var< bool,Casper::CP::Traits::GetDefaultDom< bool >::Type >,3 >;

// useful classes
%template(IntArray) Casper::Util::StdArray<int,1>;
%template(IntArray2) Casper::Util::StdArray<int,2>;
%template(IntArray3) Casper::Util::StdArray<int,3>;

%template(BoolArray) Casper::Util::StdArray<bool,1>;
%template(BoolArray2) Casper::Util::StdArray<bool,2>;
%template(BoolArray3) Casper::Util::StdArray<bool,3>;

//WRAP_STDARRAY_INDEX_OP(int,1,int)

