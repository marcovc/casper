%skeleton "lalr1.cc"       
%require "2.3"
%defines
%define "parser_class_name" "Flatzinc_parser"

%{
	//#define YY_NO_UNPUT
#include <cpp.h>
#include <print.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <assert.h>

	//#include <driver.h>

	//using namespace std;
	using namespace Casperbind::cpp;
	using namespace Casperbind::cpp::detail;

	%}

%code requires {
	class FlatzincDriver;
	//using namespace std;
	using namespace Casperbind::cpp;
	using namespace Casperbind::cpp::detail;
}


%parse-param 	{ FlatzincDriver& driver }
%lex-param 	{ FlatzincDriver& driver }

%locations
%initial-action
{
	// Initialize the initial location.
	@$.begin.filename = @$.end.filename = &driver.file;
};

%debug
%error-verbose

%union{
	IntRange *range;
	RealRange *realrange;
	std::string  *string_val;
	Variable *variable;
	Constraint *constraint;
	const Symbol *symbol;
	SymbolSet *set;	
	SymbolArray *array;
	bool *bval;
	int int_val;
	double float_val;
	const SharedSymbol* ssymbol;
}

%{
#include <driver.h>
	class FlatzincDriver;
	%}

%token <int_val>    INT_LITERAL
<string_val> STRING_LITERAL IDENT
<float_val>  FLOAT_LITERAL
ARRAY BOOL CONSTRAINT FALSE FLOAT INT MAXIMIZE MINIMIZE OF
PREDICATE SATISFY SET SOLVE TRUE VAR DOTDOT COLONCOLON 
SEMICOLON COLON EQ COMMA RPAR LPAR LBR RBR RCUR LCUR


%token END 0 "end of file"

%type <symbol> scalar_ti_expr_tail bool_ti_expr_tail int_ti_expr_tail set_ti_expr_tail non_array_ti_expr_tail float_ti_expr_tail expr array_access_expr set_literal array_literal array_decl_tail2
%type <bval> bool_literal
%type <ssymbol> constraint_elem
%%

model          : pred_decl_items var_decl_items constraint_items model_end

pred_decl_items:
      /* empty */
    | pred_decl_items_head

pred_decl_items_head:
      pred_decl_item SEMICOLON
    | pred_decl_items_head pred_decl_item SEMICOLON

// pred_decl_items : pred_decl_items pred_decl_item SEMICOLON
// | pred_decl_items error SEMICOLON { yyerrok; }
// | /* empty */

var_decl_items : var_decl_items var_decl_item SEMICOLON  
| /* empty */

constraint_items: constraint_items constraint_item SEMICOLON
| /* empty */

model_end      : solve_item SEMICOLON


//---------------------------------------------------------------------------
// Items
//---------------------------------------------------------------------------

pred_decl_item:
PREDICATE IDENT LPAR pred_decl_args RPAR

var_decl_item:
VAR    non_array_ti_expr_tail COLON IDENT annotations EQ expr
{
	SharedSymbol v(*$7);
	driver.addSymbol(v, *($4));
	driver.addAnnotations(v);
	//if (v.getType(true) != casperbind::cpp::Symbol::sVariable)
	//	driver.addVariable(v);
}
| VAR    non_array_ti_expr_tail COLON IDENT annotations
{
	SharedSymbol v = new Variable($2);
	driver.addSymbol(v, *($4));
	driver.addAnnotations(v);
	driver.addVariable(v);
}
| non_array_ti_expr_tail COLON IDENT annotations EQ expr {assert(0);}
| ARRAY LBR INT_LITERAL DOTDOT INT_LITERAL RBR OF VAR non_array_ti_expr_tail COLON  IDENT annotations array_decl_tail2
{
	if(driver.noArgs()){
		int size = $5-$3;
		SharedSymbol s = driver.variableSymbolArray($9,size);
		driver.addSymbol(s, *($11));
		driver.addAnnotations(s);
		driver.addVariable(s);
	}
	else{
		SharedSymbol s = $13;
		driver.addSymbol(s, *($11));
		driver.addAnnotations(s);
		driver.addVariable(s);
	}

}
| ARRAY LBR INT_LITERAL DOTDOT INT_LITERAL RBR OF non_array_ti_expr_tail COLON  IDENT annotations array_decl_tail2
{
	SharedSymbol s = ($12)->clone();
	driver.addSymbol(s, *($10));
	driver.addAnnotations(s);
	//driver.addVariable(s);
}


array_decl_tail2:
EQ array_literal {$$ = $2;}
| /*empty*/ {driver.beginArgList();}


constraint_item:
CONSTRAINT constraint_elem annotations { driver.addAnnotations(*$2);}

constraint_elem:
IDENT LPAR exprs RPAR
{
	std::string *s = $1;
	SymbolArray array = *(driver.argListAsArray()); 
	SharedSymbol *terms = array.getData();

	if(*s=="array_bool_and"){
		const SymbolArray& term0 = terms[0];
		assert(term0.getSize()>1);
		const SymbolArray& arg = (term0.getSize()==2)?terms[0]:makeSymbolArray(terms[0]);
		SharedSymbol c = new Constraint(Predicate::pIff, 
				makeSymbolArray(new Constraint(Predicate::pAnd,arg),
						terms[1]));
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="array_bool_element" || *s=="array_float_element" || 
		    *s=="array_int_element" || *s=="array_set_element" ||
		    *s=="array_var_bool_element" || *s=="array_var_float_element" ||
		    *s=="array_var_int_element" || *s=="array_var_set_element") {
		SharedSymbol c = new Constraint(Predicate::pEqual, 
				makeSymbolArray(
						new Expression(Expression::eElem, terms[0], terms[1]),
						terms[2]));
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="array_bool_or"){
		SharedSymbol c = new Constraint(Predicate::pIff, 
				makeSymbolArray(new Constraint(Predicate::pOr,terms[0]),
						terms[1]));
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool2int"){
		SharedSymbol a = terms[0];
		SharedSymbol b = terms[1];
		SharedSymbol r = terms[2];

		SymbolArray feqArray(2);
		feqArray[0] = b;
		feqArray[1] = new int(1);

		SymbolArray fIffArray(2);

		fIffArray[0] = a;
		fIffArray[1] = new Constraint(Predicate::pEqual, feqArray);

		SymbolArray seqArray(2);
		seqArray[0] = b;
		seqArray[1] = new int(0);

		SymbolArray sIffArray(2);

		SymbolArray nArray(1);
		nArray[0] = a;

		sIffArray[0] = new Constraint(Predicate::pNot,nArray);
		sIffArray[1] = new Constraint(Predicate::pEqual, seqArray);

		SymbolArray finalArray(2);
		finalArray[0] = new Constraint(Predicate::pIff, fIffArray);
		finalArray[1] = new Constraint(Predicate::pIff, sIffArray);

		SharedSymbol c = new Constraint(Predicate::pAnd,finalArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_and"){
		SymbolArray andArray(2);
		andArray[0] = terms[0];
		andArray[1] = terms[1];

		SymbolArray iffArray(2);
		iffArray[0] = new Constraint(Predicate::pAnd,andArray);
		iffArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_clause"){
		SymbolArray orArray(2); 
		orArray[0] = new Constraint(Predicate::pOr,terms[0]); 
		orArray[1] = new Constraint(Predicate::pNot,makeSymbolArray(new Constraint(Predicate::pOr,terms[1])));
		SharedSymbol c = new Constraint(Predicate::pOr,orArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_clause_reif"){
		SymbolArray orArray(2); 
		orArray[0] = new Constraint(Predicate::pOr,terms[0]);
		orArray[1] = new Constraint(Predicate::pNot,makeSymbolArray(new Constraint(Predicate::pOr,terms[1])));

		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pOr,orArray);
		iffArray[1] = terms[2];
		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_eq" || *s=="float_eq" || *s=="int_eq" || *s=="set_eq"){
		SharedSymbol c = new Constraint(Predicate::pEqual,array);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_eq_reif" || *s=="float_eq_reif" || *s=="int_eq_reif" || *s == "set_eq_reif"){
		SymbolArray eqArray(2); 
		eqArray[0] = terms[0]; 
		eqArray[1] = terms[1]; 
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pEqual, eqArray); 
		iffArray[1] = terms[2];
		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_ge"){
		SymbolArray notArray(1); 
		notArray[0] = terms[1];
		SymbolArray orArray(2); 
		orArray[0] = terms[0]; 
		orArray[1] = new Constraint(Predicate::pNot,notArray);

		SharedSymbol c = new Constraint(Predicate::pOr,orArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_ge_reif"){
		SymbolArray notArray(1); 
		notArray[0] = terms[1];
		SymbolArray orArray(2); 
		orArray[0] = terms[0]; 
		orArray[1] = new Constraint(Predicate::pNot,notArray);
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pOr,orArray); 
		iffArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_gt"){
		SymbolArray notArray(1); 
		notArray[0] = terms[1];
		SymbolArray andArray(2); 
		andArray[0] = terms[0]; 
		andArray[1] = new Constraint(Predicate::pNot,notArray);
		SharedSymbol c = new Constraint(Predicate::pAnd,andArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_gt_reif"){
		SymbolArray notArray(1); 
		notArray[0] = terms[1];
		SymbolArray andArray(2); 
		andArray[0] = terms[0]; 
		andArray[1] = new Constraint(Predicate::pNot,notArray);
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pAnd,andArray); 
		iffArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_le"){
		SymbolArray ifArray(2); 
		ifArray[0] = terms[0]; 
		ifArray[1] = terms[1];
		SharedSymbol c = new Constraint(Predicate::pIf,ifArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_le_reif"){
		SymbolArray ifArray(2); 
		ifArray[0] = terms[0]; 
		ifArray[1] = terms[1];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pIf,ifArray); 
		iffArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_left_imp"){
		SymbolArray ifArray(2); 
		ifArray[0] = terms[1]; 
		ifArray[1] = terms[0];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pIf,ifArray); 
		iffArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_lt"){
		SharedSymbol c = new Constraint(Predicate::pLess,makeSymbolArray(terms[0],terms[1]));
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_lt_reif"){
		SymbolArray notArray(1); 
		notArray[0] = terms[0];
		SymbolArray andArray(2); 
		andArray[0] = terms[1]; 
		andArray[1] = new Constraint(Predicate::pNot,notArray);
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pAnd,andArray); 
		iffArray[0] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_ne" || *s=="float_ne" || *s=="int_ne" || *s=="set_ne"){
		SharedSymbol c = new Constraint(Predicate::pDistinct,makeSymbolArray(terms[0],terms[1]));
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_ne_reif" || *s=="float_ne_reif" || *s=="int_ne_reif" || *s=="set_ne_reif" || *s=="bool_xor"){
		SharedSymbol distinct = new Constraint(Predicate::pDistinct, makeSymbolArray(terms[0],terms[1]));

		SharedSymbol reif = new Constraint(Predicate::pIff, makeSymbolArray(distinct, terms[2]));
		driver.addConstraint(reif);
		$$ = new SharedSymbol(reif);
	}
	else if(*s=="bool_not"){
		SharedSymbol c = new Constraint(Predicate::pNot,array);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_or"){
		SymbolArray orArray(2); 
		orArray[0] = terms[0]; 
		orArray[1] = terms[1];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pOr, orArray); 
		iffArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="bool_right_imp"){
		SymbolArray ifArray(2); 
		ifArray[0] = terms[0]; 
		ifArray[1] = terms[1];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pIf,ifArray); 
		iffArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_abs" || *s=="int_abs"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eAbs, terms[0]);
		eqArray[1] = terms[1];

		SharedSymbol c = new Constraint(Predicate::pEqual,eqArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_ge" || *s=="int_ge"){
		SharedSymbol c = new Constraint(Predicate::pGreaterEqual,array);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_ge_reif" || *s=="int_ge_reif"){
		SymbolArray geArray(2); 
		geArray[0] = terms[0]; 
		geArray[1] = terms[1];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pGreaterEqual,geArray); 
		iffArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_gt" || *s=="int_gt"){
		SharedSymbol c = new Constraint(Predicate::pGreater,array);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_gt_reif" || *s=="int_gt_reif"){
		SymbolArray gtArray(2); 
		gtArray[0] = terms[0]; 
		gtArray[1] = terms[1];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pGreater,gtArray); 
		iffArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_le" || *s=="int_le"){
		SharedSymbol c = new Constraint(Predicate::pLessEqual,array);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_le_reif" || *s=="int_le_reif"){
		SymbolArray leArray(2); 
		leArray[0] = terms[0]; 
		leArray[1] = terms[1];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pLessEqual,leArray); 
		iffArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lin_eq" || *s=="int_lin_eq"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eSumProduct, terms[0], terms[1]); 
		eqArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pEqual,eqArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lin_eq_reif" || *s=="int_lin_eq_reif"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eSumProduct, terms[0], terms[1]); 
		eqArray[1] = terms[2];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pEqual,eqArray); 
		iffArray[1] = terms[3];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lin_ge" || *s=="int_lin_ge"){
		SymbolArray geArray(2); 
		geArray[0] = new Expression(Expression::eSumProduct, terms[0], terms[1]); 
		geArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pGreaterEqual,geArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lin_ge_reif" || *s=="int_lin_ge_reif"){
		SymbolArray geArray(2); 
		geArray[0] = new Expression(Expression::eSumProduct, terms[0], terms[1]); 
		geArray[1] = terms[2];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pGreaterEqual,geArray); 
		iffArray[1] = terms[3];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lin_gt" || *s=="int_lin_gt"){
		SymbolArray gtArray(2); 
		gtArray[0] = new Expression(Expression::eSumProduct, terms[0], terms[1]); 
		gtArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pGreater,gtArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lin_gt_reif" || *s=="int_lin_gt_reif"){
		SymbolArray gtArray(2); 
		gtArray[0] = new Expression(Expression::eSumProduct, terms[0], terms[1]); 
		gtArray[1] = terms[2];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pGreater,gtArray); 
		iffArray[1] = terms[3];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lin_le" || *s=="int_lin_le"){
		SymbolArray leArray(2); 
		leArray[0] = new Expression(Expression::eSumProduct, terms[0], terms[1]); 
		leArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pLessEqual,leArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lin_le_reif" || *s=="int_lin_le_reif"){
		SymbolArray leArray(2); 
		leArray[0] = new Expression(Expression::eSumProduct, terms[0], terms[1]); 
		leArray[1] = terms[2];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pLessEqual,leArray); 
		iffArray[1] = terms[3];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lin_lt" || *s=="int_lin_lt"){
		SymbolArray ltArray(2); 
		ltArray[0] = new Expression(Expression::eSumProduct, terms[0], terms[1]); 
		ltArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pLess,ltArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lin_lt_reif" || *s=="int_lin_lt_reif"){
		SymbolArray ltArray(2); 
		ltArray[0] = new Expression(Expression::eSumProduct, terms[0], terms[1]); 
		ltArray[1] = terms[2];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pLess,ltArray); 
		iffArray[1] = terms[3];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lin_ne" || *s=="int_lin_ne"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eSumProduct, terms[0], terms[1]); 
		eqArray[1] = terms[2];
		SharedSymbol  c = new Constraint(Predicate::pDistinct,eqArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lin_ne_reif" || *s=="int_lin_ne_reif"){
		SharedSymbol c = new Constraint(Predicate::pDistinct, makeArray(terms[0], terms[1]));
		SharedSymbol reif = new Constraint(Predicate::pIff, makeArray(c, terms[2]));
		driver.addConstraint(reif);
		$$ = new SharedSymbol(reif);
	}
	else if(*s=="float_lt" || *s=="int_lt"){
		SharedSymbol c = new Constraint(Predicate::pLess,array);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_lt_reif" || *s=="int_lt_reif"){
		SymbolArray ltArray(2); 
		ltArray[0] = terms[0]; 
		ltArray[1] = terms[1];
		SymbolArray iffArray(2); 
		iffArray[0] = new Constraint(Predicate::pLessEqual,ltArray); 
		iffArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pIff,iffArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_max" || *s=="int_max"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eMax, terms[0], terms[1]); 
		eqArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pEqual,eqArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_min" || *s=="int_min"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eMin, terms[0], terms[1]); 
		eqArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pEqual,eqArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_minus" || *s=="int_minus"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eSub, terms[0], terms[1]); 
		eqArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pEqual,eqArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_negate" || *s=="int_negate"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eSym, terms[0]); 
		eqArray[1] = terms[1];

		SharedSymbol c = new Constraint(Predicate::pEqual,eqArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="float_plus" || *s=="int_plus"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eAdd, terms[0], terms[1]); 
		eqArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pEqual,eqArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="int_div"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eDiv, terms[0], terms[1]); 
		eqArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pEqual,eqArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="int_mod"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eMod, terms[0], terms[1]); 
		eqArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pEqual,eqArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="int_times"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eMul, terms[0], terms[1]); 
		eqArray[1] = terms[2];

		SharedSymbol c = new Constraint(Predicate::pEqual,eqArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="int2float"){
		assert(0);
		//TODO: Saber como se faz esta?
		$$ = new SharedSymbol(new Constraint(Predicate::pEqual, array));
	}
	else if(*s=="set_card"){
		SymbolArray eqArray(2); 
		eqArray[0] = new Expression(Expression::eCard,terms[0]); 
		eqArray[1] = terms[1];

		SharedSymbol c = new Constraint(Predicate::pEqual,eqArray);
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="set_diff"){
		SharedSymbol c = new Expression(Expression::eSub, makeSymbolArray(terms[0], terms[1]));
		SharedSymbol eq = new Constraint(Predicate::pEqual, makeSymbolArray(c, terms[2]));
		driver.addConstraint(eq);
		$$ = new SharedSymbol(eq);
	}
	else if(*s=="set_ge"){
		SharedSymbol contain = new Constraint(Constraint::pContained, makeSymbolArray(terms[1], terms[0]));
		SharedSymbol min = new Expression(Expression::eMin, makeSymbolArray(new Expression(Expression::eSymDiff, makeSymbolArray(terms[0], terms[1]))));
		SharedSymbol member = new Constraint(Constraint::pMember,makeSymbolArray(min, terms[1]));

		SharedSymbol final = new Constraint(Constraint::pOr, makeSymbolArray(contain, member));

		driver.addConstraint(final);
		$$ = new SharedSymbol(final);
	}
	else if(*s=="set_gt"){
		SharedSymbol contain = new Constraint(Constraint::pContained, makeSymbolArray(terms[1], terms[0]));
		SharedSymbol andDiff = new Constraint(Constraint::pAnd, makeSymbolArray(contain, new Constraint(Constraint::pDistinct, makeSymbolArray(terms[0], terms[1]))));
		SharedSymbol min = new Expression(Expression::eMin, makeSymbolArray(new Expression(Expression::eSymDiff, makeSymbolArray(terms[0], terms[1]))));
		SharedSymbol member = new Constraint(Constraint::pMember,makeSymbolArray(min, terms[1]));

		SharedSymbol final = new Constraint(Constraint::pOr, makeSymbolArray(andDiff, member));

		driver.addConstraint(final);
		$$ = new SharedSymbol(final);
	}
	else if(*s=="set_in"){
		SharedSymbol c = new Constraint(Constraint::pMember, makeSymbolArray(terms[0], terms[1]));
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="set_in_reif"){
		SharedSymbol c = new Constraint(Constraint::pMember, makeSymbolArray(terms[0], terms[1]));
		SharedSymbol reif = new Constraint(Constraint::pIff, makeSymbolArray(c, terms[2]));

		driver.addConstraint(reif);
		$$ = new SharedSymbol(reif);
	}
	else if(*s=="set_intersect"){
		SharedSymbol c = new Expression(Expression::eIntersect, makeSymbolArray(terms[0], terms[1]));
		SharedSymbol eq = new Constraint(Constraint::pEqual, makeSymbolArray(c, terms[2]));

		driver.addConstraint(eq);
		$$ = new SharedSymbol(eq);
	}
	else if(*s=="set_le"){
		SharedSymbol contain = new Constraint(Constraint::pContained, makeSymbolArray(terms[0], terms[1]));
		SharedSymbol min = new Expression(Expression::eMin, makeSymbolArray(new Expression(Expression::eSymDiff, makeSymbolArray(terms[0], terms[1]))));
		SharedSymbol member = new Constraint(Constraint::pMember,makeSymbolArray(min, terms[0]));

		SharedSymbol final = new Constraint(Constraint::pOr, makeSymbolArray(contain, member));

		driver.addConstraint(final);
		$$ = new SharedSymbol(final);
	}
	else if(*s=="set_lt"){
		SharedSymbol contain = new Constraint(Constraint::pContained, makeSymbolArray(terms[0], terms[1]));
		SharedSymbol andDiff = new Constraint(Constraint::pAnd, makeSymbolArray(contain, new Constraint(Constraint::pDistinct, makeSymbolArray(terms[0], terms[1]))));
		SharedSymbol min = new Expression(Expression::eMin, makeSymbolArray(new Expression(Expression::eSymDiff, makeSymbolArray(terms[0], terms[1]))));
		SharedSymbol member = new Constraint(Constraint::pMember,makeSymbolArray(min, terms[0]));

		SharedSymbol final = new Constraint(Constraint::pOr, makeSymbolArray(andDiff, member));

		driver.addConstraint(final);
		$$ = new SharedSymbol(final);
	}
	else if(*s=="set_subset"){
		SharedSymbol c = new Constraint(Constraint::pContained, makeSymbolArray(terms[0], terms[1]));
		driver.addConstraint(c);

		$$ = new SharedSymbol(c);
	}
	else if(*s=="set_subset_reif"){
		SharedSymbol c = new Constraint(Constraint::pContained, makeSymbolArray(terms[0], terms[1]));
		SharedSymbol reif = new Constraint(Constraint::pIff, makeSymbolArray(c, terms[2]));

		driver.addConstraint(reif);
		$$ = new SharedSymbol(reif);
	}
	else if(*s=="set_superset"){
		SharedSymbol c = new Constraint(Constraint::pContained, makeSymbolArray(terms[1], terms[0]));
		driver.addConstraint(c);

		$$ = new SharedSymbol(c);
	}
	else if(*s=="set_superset_reif"){
		SharedSymbol c = new Constraint(Constraint::pContained, makeSymbolArray(terms[1], terms[0]));
		SharedSymbol reif = new Constraint(Constraint::pIff, makeSymbolArray(c, terms[2]));

		driver.addConstraint(reif);
		$$ = new SharedSymbol(reif);
	}
	else if(*s=="set_symdiff"){
		SharedSymbol sym = new Expression(Expression::eSymDiff, makeSymbolArray(terms[0], terms[1]));
		SharedSymbol eq = new Constraint(Constraint::pEqual, makeSymbolArray(sym, terms[2]));

		driver.addConstraint(eq);
		$$ = new SharedSymbol(eq);
	}
	else if(*s=="set_union"){
		SharedSymbol uni = new Expression(Expression::eUnion, makeSymbolArray(terms[0], terms[1]));
		SharedSymbol eq = new Constraint(Constraint::pEqual, makeSymbolArray(uni, terms[2]));

		driver.addConstraint(eq);
		$$ = new SharedSymbol(eq);
	}
	else if(*s=="all_different") {
		SharedSymbol c = new Constraint(Constraint::pDistinct,makeSymbolArray(terms));
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else if(*s=="table") {
		SharedSymbol c = new Constraint(Constraint::pInTable,makeSymbolArray(terms[0],terms[1]));
		driver.addConstraint(c);
		$$ = new SharedSymbol(c);
	}
	else 	
	{
		driver.error(@1,std::string("unsupported constraint: ")+std::string(*$1));
		exit(1);
	}
}	

solve_item:
SOLVE annotations solve_kind { driver.addInstanceAnnotations();}

solve_kind:
SATISFY
| MINIMIZE expr
| MAXIMIZE expr

//---------------------------------------------------------------------------
// Predicate parameters
//---------------------------------------------------------------------------

pred_decl_args:
pred_decl_arg COMMA pred_decl_args
| pred_decl_arg

pred_decl_arg:
non_array_ti_expr_tail COLON IDENT
| VAR non_array_ti_expr_tail COLON IDENT
| ARRAY LBR pred_arg_array_index RBR OF  pred_arg_array_tail COLON IDENT

pred_arg_array_index:
INT
| INT_LITERAL DOTDOT INT_LITERAL
| pred_arg_array_index COMMA pred_arg_array_index	// mvc: was not in official parser

pred_arg_array_tail:
non_array_ti_expr_tail
| VAR non_array_ti_expr_tail

//---------------------------------------------------------------------------
// Type-Inst Expression Tails
//---------------------------------------------------------------------------

non_array_ti_expr_tail:
scalar_ti_expr_tail {$$ = $1;}
| set_ti_expr_tail {$$ = $1;}


scalar_ti_expr_tail:
bool_ti_expr_tail {$$ = $1;}
| int_ti_expr_tail {$$ = $1;}
| float_ti_expr_tail {$$ = $1;}

bool_ti_expr_tail:
BOOL {
	BoolSet *b = new BoolSet(); b->add(bool(false)); b->add(bool(true));
	$$ = b;}

int_ti_expr_tail:
INT {$$ = driver.getDefaultIntRange();}
| INT_LITERAL DOTDOT INT_LITERAL { $$ = new IntRange($1,$3);}
| LBR int_literals RBR {assert(0); $$ = driver.getDefaultIntRange();}

int_literals:
INT_LITERAL COMMA int_literals
| INT_LITERAL

float_ti_expr_tail:
FLOAT {$$ = driver.getDefaultRealRange();}
| FLOAT_LITERAL DOTDOT FLOAT_LITERAL {$$ = new RealRange($1,$3);}

set_ti_expr_tail:
SET OF int_ti_expr_tail
{
	$$ = $3;
}

//---------------------------------------------------------------------------
// Expressions
//---------------------------------------------------------------------------

exprs:
expr COMMA exprs {driver.addArg(($1)->clone());}
| expr {driver.beginArgList(); driver.addArg(($1)->clone());}

expr:
bool_literal {$$ = $1;}
| INT_LITERAL {$$ = new int($1);}
| FLOAT_LITERAL {$$ = new double($1);}
| STRING_LITERAL {$$ = new String($1->c_str());}
| array_literal {$$ = $1;}
| array_access_expr {$$ = $1;}
| IDENT
{
	const std::string a(*$1);
	if (driver.isAnnotation(a))
		$$ = new Annotation(String(a.c_str()));
	else
		$$ = &(driver.getSymbol(a));
}
| IDENT LPAR exprs RPAR { $$ = new Annotation(String($1->c_str()),*driver.argListAsArray());}	/* An annotation value with > 0 arguments. */
| set_literal {$$ = $1;}

bool_literal: 
FALSE {$$ = new bool(false);}
| 	TRUE {$$ = new bool(true);}

set_literal:
LCUR exprs RCUR { $$ = driver.argListAsSet();}
| LCUR RCUR {assert(0); $$ = new SymbolSet();}
| INT_LITERAL DOTDOT INT_LITERAL {$$ = new IntRange($1,$3);}

array_literal:
LBR exprs RBR { $$ = driver.argListAsTypedArray(); }
| LBR RBR {assert(0); $$ = new SymbolArray(0);}

array_access_expr: IDENT LBR expr RBR {
	const std::string a(*$1); 
	const SymbolArray &s = driver.getSymbol(a); 
	const int i = *($3); 
	$$ = &(s[i-1]);
}

//---------------------------------------------------------------------------
// Annotations
//---------------------------------------------------------------------------

annotations:
COLONCOLON expr annotations { driver.addAnnArg(*($2)->clone()); }
| {driver.beginAnnList(); }
%%
void yy::Flatzinc_parser::error(const yy::Flatzinc_parser::location_type& l, const std::string& m){
	driver.error (l, m);
}

