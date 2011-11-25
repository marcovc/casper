%module util 

%{
#include <bindings/python/util/iterator.h>
%}

%import <swigutil.i>

%pythonprepend Casper::Util::Iterator::next() 						
%{
if not self._valid():
	raise StopIteration						
%}


%import <bindings/python/util/iterator.h>
