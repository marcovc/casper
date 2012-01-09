%module cp 

%{
#include <casper/kernel.h>
#include <casper/cp.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>

// VarArray
%include <casper/cp/vararray.h>

%extend Casper::CP::VarArray 
{
	unsigned int size(unsigned int dim = 0) const
	{	return $self->size(dim);	}
	unsigned int count(unsigned int dim = 0) const
	{	return $self->count(dim);	}
	bool empty() const
	{	return $self->empty();	}
	
	int __len__() const { return $self->size(); }

	Casper::CP::VarArray 
	__getslice__(int i,int j) const 
	{
		const unsigned int mi = std::max(0,i);
		const unsigned int ma = std::min(static_cast<int>($self->size()),j)-1;
		const unsigned int nelems = ma-mi+1;
		Casper::CP::VarArray<T,dims,Dom1> r($self->getStore(),nelems);
		for (unsigned int i = 0; i < nelems; ++i)
			r[i] = $self->operator[](i+mi);
		return r; 
	}

	
}
COUTWRAPPER(Casper::CP::VarArray)
