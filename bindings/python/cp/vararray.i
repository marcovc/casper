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
	unsigned int count(unsigned int dim = 0) const
	{	return $self->count(dim);	}
	unsigned int size(unsigned int dim = 0) const
	{	return $self->size(dim);	}
	bool empty() const
	{	return $self->empty();	}
}
COUTWRAPPER(Casper::CP::VarArray)
