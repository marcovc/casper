

eBool = "eBool"
eInt = "eInt"
eReal = "eReal"
eIntSet = "eIntSet"
eBoolSet = "eBoolSet"
eIntRange = "eIntRange"
eRealRange = "eRealRange"
eIntArray = "eIntArray"
eRealArray = "eRealArray"
eBoolArray = "eBoolArray"
eBoolVar = "eBoolVar"
eIntVar = "eIntVar"
eRealVar = "eRealVar"
eIntSetVar = "eIntSetVar"
eBoolSetVar = "eBoolSetVar"
eIntVarArray = "eIntVarArray"
eRealVarArray = "eRealVarArray"
eBoolVarArray = "eBoolVarArray"
eIntSetVarArray = "eIntSetVarArray"
eBoolSetVarArray = "eBoolSetVarArray"
eBoolExpr = "eBoolExpr"
eIntExpr = "eIntExpr"
eRealExpr = "eRealExpr"
eIntSetExpr = "eIntSetExpr"
eBoolSetExpr = "eBoolSetExpr"
		    
bool_evals = [eBool,eBoolVar,eBoolExpr]
int_evals = [eInt,eIntVar,eIntExpr]
real_evals = [eReal,eRealVar,eRealExpr]
bool_set_evals = [eBoolSet,eBoolSetVar,eBoolSetExpr]
int_set_evals = [eIntSet,eIntSetVar,eIntSetExpr]
bool_array_evals = [eBoolArray,eBoolVarArray]
int_array_evals = [eIntArray,eIntVarArray]
real_array_evals = [eRealArray,eRealVarArray]
array_evals = int_array_evals + real_array_evals


valid_preds = set()

def valid_pred_1(p,set1):
	global valid_preds
	for i in set1:
		valid_preds.add( (p,i) )

def valid_pred_2(p,set1,set2):
	global valid_preds
	for i in set1:
		for j in set2:
			valid_preds.add( (p,i,j) )
	
############# valid predicates ###############

# temp:
real_evals = int_set_evals = bool_set_evals = real_array_evals =  bool_array_evals = []

valid_pred_1("pNot",bool_set_evals)
valid_pred_1("pDistinct",int_array_evals)
#valid_pred_1("pAnd",[eBoolVarArray])

for p in ["pEqual","pDistinct"]:
	for i in [bool_evals,int_evals,real_evals,int_set_evals,bool_set_evals]:
		valid_pred_2(p,i,i)

for p in ["pLess","pLessEqual","pGreater","pGreaterEqual"]:
	for i in [bool_evals,int_evals,real_evals]:
		valid_pred_2(p,i,i)

for i in ["pAnd","pOr","pXOr","pIff","pIf"]:
	valid_pred_2(i,bool_evals,bool_evals)

for i in [int_set_evals,bool_set_evals]:
	valid_pred_2("pContained",i,i)

valid_pred_2("pMember",int_evals,int_set_evals)
valid_pred_2("pMember",bool_evals,bool_set_evals)

valid_pred_2("pInTable",[eIntVarArray],[eIntArray])

#TODO: pNotInTable

############# valid expressions ###############

valid_int_exprs = set()
valid_real_exprs = set()
valid_int_set_exprs = set()
valid_bool_set_exprs = set()

def valid_expr_1(p,set1,valid_exprs):
	for i in set1:
		valid_exprs.add( (p,i) )

def valid_expr_2(p,set1,set2,valid_exprs):
	for i in set1:
		for j in set2:
			valid_exprs.add( (p,i,j) )

for i in ["eSym", "eAbs"]:
	valid_expr_1(i,int_evals,valid_int_exprs)
	valid_expr_1(i,real_evals,valid_real_exprs)
	
for i in ["eAdd","eSub","eMul","eDiv","eMod","eMin","eMax"]:
	valid_expr_2(i,int_evals,int_evals,valid_int_exprs)
	valid_expr_2(i,real_evals,real_evals,valid_real_exprs)
	
valid_expr_1("eCard",int_set_evals,valid_int_set_exprs)
valid_expr_1("eCard",bool_set_evals,valid_bool_set_exprs)

for i in ["eIntersect","eUnion","eSymDiff","eSub"]:
	valid_expr_2(i,int_set_evals,int_set_evals,valid_int_set_exprs)
	valid_expr_2(i,bool_set_evals,bool_set_evals,valid_bool_set_exprs)

# TODO: eIfThenElse

valid_expr_2("eElem",int_array_evals,int_evals,valid_int_exprs)
valid_expr_2("eElem",real_array_evals,int_evals,valid_real_exprs)
#valid_expr_2("eElem",bool_array_evals,int_evals,valid_bool_exprs)

valid_expr_2("eSumProduct",int_array_evals,int_array_evals,valid_int_exprs)
valid_expr_2("eSumProduct",real_array_evals,real_array_evals,valid_real_exprs)
 
			
###############################################

def print_pred_cpp_code(ret_eval,ret_type):
	print "template<> struct SymbolToCasper<"+ret_eval+",cBounds>"
	print "{"
	print "\ttypedef "+ret_type+" Type;"
	print "\tType operator()(ICPSolver& solver,const Symbol& s)"
	print "\t{"
	print "\t\tconst Predicate& p = s;"
	print "\t\tEval e1,e2,e3;"
	print "\t\te1 = getEval(p[0]);"
	print "\t\tif (p.getArity()>=2) e2 = getEval(p[1]);"
	print "\t\tif (p.getArity()>=3) e3 = getEval(p[2]);"
	for p in valid_preds:
		print "\t\tif (p.getArity()=="+str(len(p)-1)+" and p.getFunctor()==Predicate::"+p[0],
		for i in range(1,len(p)):
			print "and e"+str(i)+"=="+p[i],
		print ")"		
		print "\t\t\treturn Pred"+str(len(p)-1)+"ToCasper<Predicate::"+p[0],
		for i in range(1,len(p)):
			print ","+p[i],
		print ",cBounds>()(solver,p);"
	print "\t\tthrow EImplement(s);"
	print "\t}"
	print "};"

def print_expr_cpp_code(ret_eval,ret_type,valid_exprs):
	print "template<> struct SymbolToCasper<"+ret_eval+",cBounds>"
	print "{"
	print "\ttypedef "+ret_type+" Type;"
	print "\tType operator()(ICPSolver& solver,const Symbol& s)"
	print "\t{"
	print "\t\tconst Expression& p = s;"
	print "\t\tEval e1,e2,e3;"
	print "\t\te1 = getEval(p[0]);"
	print "\t\tif (p.getArity()>=2) e2 = getEval(p[1]);"
	print "\t\tif (p.getArity()>=3) e3 = getEval(p[2]);"
	for p in valid_exprs:
		print "\t\tif (p.getArity()=="+str(len(p)-1)+" and p.getOperator()==Expression::"+p[0],
		for i in range(1,len(p)):
			print "and e"+str(i)+"=="+p[i],
		print ")"		
		print "\t\t\treturn Expr"+str(len(p)-1)+"ToCasper<Type,Expression::"+p[0],
		for i in range(1,len(p)):
			print ","+p[i],
		print ",cBounds>()(solver,p);"
	print "\t\tthrow EImplement(s);"
	print "\t}"
	print "};"

print_pred_cpp_code("eBoolExpr","casper::BndExpr<casper::Bool>")
print_expr_cpp_code("eIntExpr","casper::BndExpr<casper::Int>",valid_int_exprs)
print_expr_cpp_code("eRealExpr","casper::BndExpr<casper::Double>",valid_real_exprs)
print_expr_cpp_code("eIntSetExpr","casper::DomExpr<casper::IntSet>",valid_int_set_exprs)
print_expr_cpp_code("eBoolSetExpr","casper::DomExpr<casper::BoolSet>",valid_bool_set_exprs)

