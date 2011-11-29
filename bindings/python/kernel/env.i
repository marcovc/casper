
%import "swigutil.i"

%module kernel 
%{
#include <casper/kernel.h>
%}

// Env [TODO]
%import <casper/kernel/state/env.h>


%include <casper/kernel/state/state.h>

%typemap(typecheck,precedence=0) Casper::State&
{
	void * argp = NULL;
	$1 = SWIG_IsOK(SWIG_ConvertPtr($input, &argp, $descriptor(Casper::CP::Solver*), 0)) or
	 	 SWIG_IsOK(SWIG_ConvertPtr($input, &argp, $descriptor(Casper::CP::Store*), 0)) or
	 	 SWIG_IsOK(SWIG_ConvertPtr($input, &argp, $descriptor(Casper::State*), 0));
};

%typemap(in) Casper::State&
{
	void * argp = NULL;
	if (SWIG_IsOK(SWIG_ConvertPtr($input, &argp, $descriptor(Casper::CP::Solver*), 0)))
		$1 = &static_cast<Casper::State&>(*static_cast<Casper::CP::Solver*>(argp));
	else
	if (SWIG_IsOK(SWIG_ConvertPtr($input, &argp, $descriptor(Casper::CP::Store*), 0)))
		$1 = &static_cast<Casper::State&>(*static_cast<Casper::CP::Store*>(argp));
	else
	if (SWIG_IsOK(SWIG_ConvertPtr($input, &argp, $descriptor(Casper::State*), 0)))
		$1 = &static_cast<Casper::State&>(*static_cast<Casper::State*>(argp));
	else
	assert(0);
};
