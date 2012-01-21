//%import "swigutil.i"

%module kernel 
%{
#include <casper/kernel.h>
#include <casper/cp.h>
#include <casper/kernel/spexpr/expr.h>
#include <casper/kernel/goal/whiledo.h>
#include <bindings/python/kernel/expr.h>
%}


%import <std_string.i>

//%import <cp/view.i>

// Expr

// typemaps

// int


%typemap(in) Casper::Expr<int> const&
{
	$1 = Casper::Detail::CreateFromPyObject<int>::create($input);
};

%typemap(typecheck,precedence=0) Casper::Expr<int> const&
{
	$1 = Casper::Detail::CreateFromPyObject<int>::check($input);
};

// bool

%typemap(in) Casper::Expr<bool> const&
{
	$1 = Casper::Detail::CreateFromPyObject<bool>::create($input);
};

%typemap(typecheck,precedence=0) Casper::Expr<bool> const&
{
	$1 = Casper::Detail::CreateFromPyObject<bool>::check($input);
};

// Seq<int>

%typemap(in) Casper::Expr<Casper::Seq<int> > const&
{
	$1 = Casper::Detail::CreateFromPyObject<Casper::Seq<int> >::create($input);
};

%typemap(typecheck,precedence=0) Casper::Expr<Casper::Seq<int> > const&
{
	$1 = Casper::Detail::CreateFromPyObject<Casper::Seq<int> >::check($input);
};

// Seq<bool>

%typemap(in) Casper::Expr<Casper::Seq<bool> > const&
{
	$1 = Casper::Detail::CreateFromPyObject<Casper::Seq<bool> >::create($input);
};

%typemap(typecheck,precedence=0) Casper::Expr<Casper::Seq<bool> > const&
{
	$1 = Casper::Detail::CreateFromPyObject<Casper::Seq<bool> >::check($input);
};

%import <casper/util/pimpl.h>

%template() Casper::Util::SPImplIdiom< Casper::Detail::IExpr< bool > >;
%template() Casper::Util::SPImplIdiom< Casper::Detail::IExpr< int > >;
%template() Casper::Util::SPImplIdiom< Casper::Detail::IExpr< Casper::Seq<bool> > >;
%template() Casper::Util::SPImplIdiom< Casper::Detail::IExpr< Casper::Seq<int> > >;

%include <casper/kernel/spexpr/expr.h>

// This is needed since SWIG confuses partial specializations (bug #3431508)

%define DEFINE_EXPR(Eval)
template<>
struct Expr<Eval> : Casper::Util::SPImplIdiom<Detail::IExpr<Eval> >
{
	typedef Casper::Util::SPImplIdiom<Detail::IExpr<Eval> > Super;
	Expr(const Expr& expr) : Super(expr) {}
	std::string getTypeStr() const
	{	return this->getImpl().getTypeStr();	}
};
%enddef

%define DEFINE_SEQEXPR(Eval)
template<>
struct Expr<Casper::Seq<Eval> > : Casper::Util::SPImplIdiom<Detail::IExpr<Casper::Seq<Eval> > >
{
	typedef Casper::Util::SPImplIdiom<Detail::IExpr<Casper::Seq<Eval> > > Super;
	Expr(const Expr& expr) : Super(expr) {}
	std::string getTypeStr() const
	{	return this->getImpl().getTypeStr();	}
	bool hasSeqElement() const
	{	return this->getImpl().hasSeqElement();	}	
};
%enddef
namespace Casper {
//DEFINE_EXPR(int)
DEFINE_SEQEXPR(int)
DEFINE_SEQEXPR(bool)
}


%template(IntExpr) Casper::Expr<int>;
%template(BoolExpr) Casper::Expr<bool>;
%template(IntSeqExpr) Casper::Expr<Casper::Seq<int> >;
%template(BoolSeqExpr) Casper::Expr<Casper::Seq<bool> >;
 
COUTWRAPPER(Casper::Expr<int>)
COUTWRAPPER(Casper::Expr<bool>)
COUTWRAPPER(Casper::Expr<Casper::Seq<int> >)
COUTWRAPPER(Casper::Expr<Casper::Seq<bool> >)

%define EXTEND_SEQ_EXPR(Eval)
%extend Casper::Expr<Casper::Seq<Eval> > 
{
	Casper::Expr<Eval> getItem1(int i) const { return $self->getElement(i); }
	Casper::Expr<Casper::Seq<Eval> > getItem2(int i) const { return $self->getSeqElement(i); }
	Casper::Expr<Casper::Seq<Eval> > getElem2(const Casper::Expr<int>& i) const { return Casper::rel<Casper::Element>(*$self,i); }
	
	%pythoncode
	{
		def __getitem__(self,i):
			import numbers
			if not isinstance(i,numbers.Number):
				if self.hasSeqElement():
					return self.getElem2(i)
				else:
					return element(self,i)		
			elif i>=self.size() or i<0:
				raise IndexError
			elif self.hasSeqElement():
				return self.getItem2(i)
			else:
				return self.getItem1(i)		
	}
		
//	void 
//	__setitem__(int i,const Casper::CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type>& v) 
//	{ $self->operator[](i) = v; }		
}
%enddef

EXTEND_SEQ_EXPR(int)

%include <kernel/boolexpr_operators.i>
%include <kernel/intexpr_operators.i>
%include <kernel/expr_predicates.i>
%include <kernel/blockfun.i>

// Ambiguous predicates handlers (predicates that are both casper and python reserved words)

%pythoncode %{
def min(*args,**kargs):
	import numbers
	if len(kargs.keys())>0 or \
		len(args)>2 or \
		all([isinstance(arg,numbers.Number) for arg in args]):
		import __builtin__
		return __builtin__.min(*args,**kargs)
	else:
		return _min(*args,**kargs)

def max(*args,**kargs):
	import numbers
	if len(kargs.keys())>0 or \
		all([isinstance(arg,numbers.Number) for arg in args]):
		import __builtin__
		return __builtin__.max(*args,**kargs)
	else:
		return _max(*args,**kargs)

def abs(arg):
	import numbers
	if isinstance(arg,numbers.Number):
		import __builtin__
		return __builtin__.abs(arg)
	else:
		return _abs(arg)

def sum(*args,**kwargs):
	import numbers
	if len(kwargs.keys())>0 or \
	 	all([isinstance(arg,numbers.Number) for arg in args]):
		import __builtin__
		return __builtin__.sum(*args,**kwargs)
	else:
		return _sum(*args,**kwargs)

'''
	import collections
	if len(kwargs.keys())>0 or \
		isinstance(args[0],collections.Iterable):
		import __builtin__
		return __builtin__.sum(*args,**kwargs)
	else:
		return _sum(*args,**kwargs)
'''
	
def pow(*args):
	import numbers
	if all([isinstance(arg,numbers.Number) for arg in args]):
		import __builtin__
		return __builtin__.pow(*args)
	else:
		return _pow(*args)		
%}
