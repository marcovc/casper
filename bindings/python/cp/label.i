%module cp 

%{
#include <casper/kernel.h>
#include <casper/cp.h>
#include <bindings/python/kernel/expr.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>

%include <casper/cp/goal/heuristic.h>

%define VAR_SELECT(Eval)
%template(selectVarLex) 					Casper::CP::selectVarLex<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMinDom) 					Casper::CP::selectVarMinDom<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMaxDom) 					Casper::CP::selectVarMaxDom<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMinMinElem) 				Casper::CP::selectVarMinMinElem<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMinMaxElem) 				Casper::CP::selectVarMinMaxElem<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMaxMinElem) 				Casper::CP::selectVarMaxMinElem<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMaxMaxElem) 				Casper::CP::selectVarMaxMaxElem<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMinDegree) 				Casper::CP::selectVarMinDegree<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMaxDegree) 				Casper::CP::selectVarMaxDegree<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMinDomOverDegree) 		Casper::CP::selectVarMinDomOverDegree<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMaxDomOverDegree) 		Casper::CP::selectVarMaxDomOverDegree<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMinWeightedDegreetVarLex) Casper::CP::selectVarMinWeightedDegree<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMaxWeightedDegree) 		Casper::CP::selectVarMaxWeightedDegree<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMinDomOverWeightedDegree) Casper::CP::selectVarMinDomOverWeightedDegree<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectVarMaxDomOverWeightedDegree) Casper::CP::selectVarMaxDomOverWeightedDegree<Casper::Expr<Casper::Seq<Eval> > >;
%enddef

%define VAL_SELECT(Eval)
%template(selectValsMin)	Casper::CP::selectValsMin<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectValsMax)	Casper::CP::selectValsMax<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectValsRand)	Casper::CP::selectValsRand<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectValMin)		Casper::CP::selectValMin<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectValMax)		Casper::CP::selectValMax<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectHalfMin)	Casper::CP::selectHalfMin<Casper::Expr<Casper::Seq<Eval> > >;
%template(selectHalfMax)	Casper::CP::selectHalfMax<Casper::Expr<Casper::Seq<Eval> > >;
%enddef

VAR_SELECT(int)
//VAR_SELECT(bool)

VAL_SELECT(int)
//VAL_SELECT(bool)

%include <casper/cp/goal/labeling.h>

%template(label) Casper::CP::label<Casper::Expr<Casper::Seq<int> > >;
//%template(label) Casper::CP::label<Casper::Expr<Casper::Seq<bool> > >;

