%import "swigutil.i"

%module util
%feature("autodoc", "1") ;
%{
#include <casper/util/timer.h>
#include <sstream>
%}

 
// Timer
%ignore Casper::Util::ITimer;
%ignore Casper::Util::CPUTimer::print;
%ignore Casper::Util::WallTimer::print;
%include <casper/util/timer.h>
COUTWRAPPER(Casper::Util::CPUTimer)
%extend Casper::Util::CPUTimer {
    double getSecs() const { return $self->getSecs(); }
}
%include <util/stdarray.i>
%include <util/iterator.i>