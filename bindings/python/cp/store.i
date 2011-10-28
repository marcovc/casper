%module cp 

%{
#include <casper/kernel.h>
#include <casper/cp.h>
#include <casper/kernel/obj/expr.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>

%import <casper/kernel/reversible/reversible.h>

%import <kernel/env.i>

// Store,StoreStats
%ignore Casper::CP::Store::getStats();
%ignore Casper::CP::Store::operator Util::IHeap&;
%ignore Casper::CP::Store::operator const Util::IHeap&;
%ignore Casper::CP::Store::operator State&;
%ignore Casper::CP::Store::operator const State&;
%include <casper/cp/store.h>
COUTWRAPPER(Casper::CP::StoreStats)
	
%extend Casper::CP::Store {
	bool post(const Casper::Expr<bool>& expr, 
			  Casper::CP::Consistency consistency = Casper::CP::Bounds)
	{
		switch (consistency)
		{
			case Casper::CP::Domain:
				return expr.postDomFilter(*$self);
			case Casper::CP::Bounds:
				return expr.postBndFilter(*$self);
			case Casper::CP::Value:
				return expr.postValFilter(*$self);
		}
	}

}
