%import "swigutil.i"

%module util
%feature("autodoc", "1") ;
%{
#include <casper/util/timer.h>
#include <sstream>
%}

 
// Timer
%ignore Casper::Util::ITimer::print;
%ignore Casper::Util::CPUTimer::print;
%ignore Casper::Util::WallTimer::print;
%include <casper/util/timer.h>
COUTWRAPPER(Casper::Util::CPUTimer)

%include <util/stdarray.i>
