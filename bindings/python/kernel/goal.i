
%import "swigutil.i"

%module kernel 
%{
#include <casper/kernel.h>
#include <casper/kernel/goal.h>
%}

// Goal [TODO]
%include <casper/kernel/goal/goal.h>

%include <kernel/goal_operators.i>

%inline %{

namespace Casper {

Casper::Expr<bool> _selectMin(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<int>& expr)
{	return selectMin<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool>,Casper::Expr<int> >(par,set,cond,expr); }

Casper::Expr<bool> _selectMin(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<bool>& expr)
{	return selectMin<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool>,Casper::Expr<bool> >(par,set,cond,expr); }

Casper::Expr<bool> _selectMin(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<int>& expr)
{	return selectMin<Casper::Expr<Casper::Seq<int> >,Casper::Expr<int> >(par,set,expr); }

Casper::Expr<bool> _selectMin(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& expr)
{	return selectMin<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool> >(par,set,expr); }

Casper::Expr<bool> _selectMin(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<Casper::Seq<int> >& expr)
{	return selectMin<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool>,Casper::Expr<Casper::Seq<int> > >(par,set,cond,expr); }

Casper::Expr<bool> _selectMin(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<Casper::Seq<bool> >& expr)
{	return selectMin<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool>,Casper::Expr<Casper::Seq<bool> > >(par,set,cond,expr); }

Casper::Expr<bool> _selectMin(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<Casper::Seq<int> >& expr)
{	return selectMin<Casper::Expr<Casper::Seq<int> >,Casper::Expr<Casper::Seq<int> > >(par,set,expr); }

Casper::Expr<bool> _selectMin(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<Casper::Seq<bool> >& expr)
{	return selectMin<Casper::Expr<Casper::Seq<int> >,Casper::Expr<Casper::Seq<bool> > >(par,set,expr); }

Casper::Expr<bool> _selectMax(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<int>& expr)
{	return selectMax<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool>,Casper::Expr<int> >(par,set,cond,expr); }

Casper::Expr<bool> _selectMax(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<bool>& expr)
{	return selectMax<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool>,Casper::Expr<bool> >(par,set,cond,expr); }

Casper::Expr<bool> _selectMax(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<int>& expr)
{	return selectMax<Casper::Expr<Casper::Seq<int> >,Casper::Expr<int> >(par,set,expr); }

Casper::Expr<bool> _selectMax(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& expr)
{	return selectMax<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool> >(par,set,expr); }

Casper::Expr<bool> _selectMax(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<Casper::Seq<int> >& expr)
{	return selectMax<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool>,Casper::Expr<Casper::Seq<int> > >(par,set,cond,expr); }

Casper::Expr<bool> _selectMax(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<Casper::Seq<bool> >& expr)
{	return selectMax<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool>,Casper::Expr<Casper::Seq<bool> > >(par,set,cond,expr); }

Casper::Expr<bool> _selectMax(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<Casper::Seq<int> >& expr)
{	return selectMax<Casper::Expr<Casper::Seq<int> >,Casper::Expr<Casper::Seq<int> > >(par,set,expr); }

Casper::Expr<bool> _selectMax(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<Casper::Seq<bool> >& expr)
{	return selectMax<Casper::Expr<Casper::Seq<int> >,Casper::Expr<Casper::Seq<bool> > >(par,set,expr); }

Casper::Expr<bool> selectRand(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond)
{	return selectRand<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool> >(par,set,cond); }

Casper::Expr<bool> selectRand(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set)
{	return selectRand<Casper::Expr<Casper::Seq<int> > >(par,set); }


Casper::Expr<bool> _forAll(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& goal)
{	return rel<ForAll>(par,set,goal); }

Casper::Expr<bool> _forAll(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<bool>& goal)
{	return rel<ForAll>(par,set,cond,goal); }

Casper::Expr<bool> _forAll(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<int>& sort,
							 const Casper::Expr<bool>& goal)
{	return rel<ForAll>(par,set,cond,sort,goal); }

Casper::Expr<bool> _tryAll(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& goal)
{	return rel<TryAll>(par,set,goal); }
 
Casper::Expr<bool> _tryAll(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<bool>& goal)
{	return rel<TryAll>(par,set,cond,goal); }

Casper::Expr<bool> _tryAll(const Casper::IntRef& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<int>& sort,
							 const Casper::Expr<bool>& goal)
{	return rel<TryAll>(par,set,cond,sort,goal); }

} // Casper

%}

%pythoncode %{


whileDo = _BlockFun(_kernel._whileDo,[[_FunPar(name='cond'),_FunPar(name='goal',block=1)]])


selectMin = _BlockFun(_kernel._selectMin,
	[[_FunPar(name='par'),_FunPar(name='set'),_FunPar(name='expr',block=1)],
	 [_FunPar(name='par'),_FunPar(name='set'),_FunPar(name='cond',kw=True),_FunPar(name='expr',block=1)]])

selectMax = _BlockFun(_kernel._selectMax,
	[[_FunPar(name='par'),_FunPar(name='set'),_FunPar(name='expr',block=1)],
	 [_FunPar(name='par'),_FunPar(name='set'),_FunPar(name='cond',kw=True),_FunPar(name='expr',block=1)]])

forAll = _BlockFun(_kernel._forAll,
	[[_FunPar(name='par'),_FunPar(name='set'),_FunPar(name='goal',block=1)],
	 [_FunPar(name='par'),_FunPar(name='set'),_FunPar(name='cond',kw=True),_FunPar(name='goal',block=1)],
	 [_FunPar(name='par'),_FunPar(name='set'),_FunPar(name='cond',kw=True,default=True),
	 						_FunPar(name='orderby',kw=True),_FunPar(name='goal',block=1)]])
	 
tryAll = _BlockFun(_kernel._tryAll,
	[[_FunPar(name='par'),_FunPar(name='set'),_FunPar(name='goal',block=1)],
	 [_FunPar(name='par'),_FunPar(name='set'),_FunPar(name='cond',kw=True),_FunPar(name='goal',block=1)],
	 [_FunPar(name='par'),_FunPar(name='set'),_FunPar(name='cond',kw=True,default=True),
	 						_FunPar(name='orderby',kw=True),_FunPar(name='goal',block=1)]])
%}


