
%module cp 
%feature("autodoc", "1");

%{
#include <casper/kernel.h>
#include <casper/cp.h>
#include <casper/cp/spexpr/expr.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>

%include <cp/store.i>
%include <cp/solver.i>
%include <cp/var.i>
%include <cp/int/var.i>
%include <cp/vararray.i>
%include <cp/int/vararray.i>
%include <cp/label.i>
%include <cp/goal.i>


%{
namespace Casper {
namespace CP {
bool Store::postExpr(const Casper::Expr<bool>& expr,
		  Casper::CP::Consistency consistency)
{
	switch (consistency)
	{
        std::cout << "posting\n";
		case Casper::CP::Domain:
			return expr.postDomFilter(*this);
		case Casper::CP::Bounds:
			return expr.postBndFilter(*this);
		case Casper::CP::Value:
			return expr.postValFilter(*this);
		default:
			throw Casper::Exception::InvalidOperation("invalid value for consistency parameter");
	}
}
}
namespace Detail {
    template<class Store,class T>
    struct Post<Store,Casper::Expr<T> > : IGoal
    {
        Post(Store& store,const Casper::Expr<T>& p, bool validate) :
		store(store),p(p),validate(validate) {}
        Goal execute()
        {
            if (store.postExpr(p))
			return Goal(!validate or store.valid());
            else
			return fail();
        }
        Store&	store;
        const Expr<T>	p;
        const bool	validate;
    };

}
}
%}

