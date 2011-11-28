
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

Casper::Expr<bool> selectMin(const Casper::IntPar& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<int>& expr)
{	return selectMin<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool>,Casper::Expr<int> >(par,set,cond,expr); }

Casper::Expr<bool> selectMin(const Casper::IntPar& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<bool>& expr)
{	return selectMin<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool>,Casper::Expr<bool> >(par,set,cond,expr); }

Casper::Expr<bool> selectMin(const Casper::IntPar& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<int>& expr)
{	return selectMin<Casper::Expr<Casper::Seq<int> >,Casper::Expr<int> >(par,set,expr); }

Casper::Expr<bool> selectMin(const Casper::IntPar& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& expr)
{	return selectMin<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool> >(par,set,expr); }

Casper::Expr<bool> selectMax(const Casper::IntPar& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<int>& expr)
{	return selectMax<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool>,Casper::Expr<int> >(par,set,cond,expr); }

Casper::Expr<bool> selectMax(const Casper::IntPar& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond,
							 const Casper::Expr<bool>& expr)
{	return selectMax<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool>,Casper::Expr<bool> >(par,set,cond,expr); }

Casper::Expr<bool> selectMax(const Casper::IntPar& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<int>& expr)
{	return selectMax<Casper::Expr<Casper::Seq<int> >,Casper::Expr<int> >(par,set,expr); }

Casper::Expr<bool> selectMax(const Casper::IntPar& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& expr)
{	return selectMax<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool> >(par,set,expr); }

Casper::Expr<bool> selectRand(const Casper::IntPar& par, 
							 const Casper::Expr<Casper::Seq<int> >& set,
							 const Casper::Expr<bool>& cond)
{	return selectRand<Casper::Expr<Casper::Seq<int> >,Casper::Expr<bool> >(par,set,cond); }

Casper::Expr<bool> selectRand(const Casper::IntPar& par, 
							 const Casper::Expr<Casper::Seq<int> >& set)
{	return selectRand<Casper::Expr<Casper::Seq<int> > >(par,set); }

} // Casper

%}
