template<> struct SymbolToCasper<eBoolExpr,cBounds>
{
	typedef Casper::BndExpr<Casper::bool> Type;
	Type operator()(CPSolver& solver,const Symbol& s)
	{
		const Predicate& p = s;
		Eval e1,e2,e3;
		e1 = getEval(p[0]);
		if (p.getArity()>=2) e2 = getEval(p[1]);
		if (p.getArity()>=3) e3 = getEval(p[2]);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eBoolVar and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eBoolVar ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pAnd and e1==eBool and e2==eBool )
			return Pred2ToCasper<Predicate::pAnd ,eBool ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eBool and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pLessEqual ,eBool ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eBool and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pGreater ,eBool ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eBool and e2==eBool )
			return Pred2ToCasper<Predicate::pLessEqual ,eBool ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eBoolVar and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pDistinct ,eBoolVar ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pAnd and e1==eBoolVar and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pAnd ,eBoolVar ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eIntVar and e2==eIntVar )
			return Pred2ToCasper<Predicate::pEqual ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pAnd and e1==eBoolVar and e2==eBool )
			return Pred2ToCasper<Predicate::pAnd ,eBoolVar ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eBool and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pEqual ,eBool ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eIntExpr and e2==eIntVar )
			return Pred2ToCasper<Predicate::pLess ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIff and e1==eBoolExpr and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pIff ,eBoolExpr ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIf and e1==eBool and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pIf ,eBool ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eIntExpr and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pLess ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eBool and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pGreater ,eBool ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pXOr and e1==eBool and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pXOr ,eBool ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIf and e1==eBool and e2==eBool )
			return Pred2ToCasper<Predicate::pIf ,eBool ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eInt and e2==eIntVar )
			return Pred2ToCasper<Predicate::pLess ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eInt and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pLess ,eInt ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eIntVar and e2==eIntVar )
			return Pred2ToCasper<Predicate::pLessEqual ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIff and e1==eBoolVar and e2==eBool )
			return Pred2ToCasper<Predicate::pIff ,eBoolVar ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eBool and e2==eBool )
			return Pred2ToCasper<Predicate::pLess ,eBool ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eBool and e2==eBool )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eBool ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pXOr and e1==eBoolExpr and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pXOr ,eBoolExpr ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eBoolVar and e2==eBool )
			return Pred2ToCasper<Predicate::pLessEqual ,eBoolVar ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eBoolVar and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pDistinct ,eBoolVar ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eBoolExpr and e2==eBool )
			return Pred2ToCasper<Predicate::pEqual ,eBoolExpr ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eBoolExpr and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pLessEqual ,eBoolExpr ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIff and e1==eBoolExpr and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pIff ,eBoolExpr ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eInt and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pEqual ,eInt ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eBool and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pEqual ,eBool ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eIntVar and e2==eIntVar )
			return Pred2ToCasper<Predicate::pLess ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eInt and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pDistinct ,eInt ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==1 and p.getFunctor()==Predicate::pDistinct and e1==eIntVarArray )
			return Pred1ToCasper<Predicate::pDistinct ,eIntVarArray ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eBool and e2==eBool )
			return Pred2ToCasper<Predicate::pEqual ,eBool ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eIntVar and e2==eInt )
			return Pred2ToCasper<Predicate::pEqual ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pAnd and e1==eBoolExpr and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pAnd ,eBoolExpr ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIf and e1==eBool and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pIf ,eBool ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pOr and e1==eBoolVar and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pOr ,eBoolVar ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pXOr and e1==eBoolExpr and e2==eBool )
			return Pred2ToCasper<Predicate::pXOr ,eBoolExpr ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIff and e1==eBoolVar and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pIff ,eBoolVar ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eBool and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eBool ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pOr and e1==eBoolExpr and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pOr ,eBoolExpr ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pXOr and e1==eBool and e2==eBool )
			return Pred2ToCasper<Predicate::pXOr ,eBool ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eInt and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pLessEqual ,eInt ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIf and e1==eBoolVar and e2==eBool )
			return Pred2ToCasper<Predicate::pIf ,eBoolVar ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pOr and e1==eBool and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pOr ,eBool ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eBoolExpr and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pGreater ,eBoolExpr ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pOr and e1==eBoolExpr and e2==eBool )
			return Pred2ToCasper<Predicate::pOr ,eBoolExpr ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eIntVar and e2==eIntVar )
			return Pred2ToCasper<Predicate::pGreater ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eBoolVar and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pEqual ,eBoolVar ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pOr and e1==eBoolVar and e2==eBool )
			return Pred2ToCasper<Predicate::pOr ,eBoolVar ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eBool and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pLess ,eBool ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eIntExpr and e2==eInt )
			return Pred2ToCasper<Predicate::pLess ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eIntVar and e2==eInt )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIff and e1==eBoolExpr and e2==eBool )
			return Pred2ToCasper<Predicate::pIff ,eBoolExpr ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eBool and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eBool ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIf and e1==eBoolExpr and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pIf ,eBoolExpr ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eIntVar and e2==eInt )
			return Pred2ToCasper<Predicate::pLess ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pOr and e1==eBoolVar and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pOr ,eBoolVar ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eInt and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pGreater ,eInt ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eIntVar and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pDistinct ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eInt and e2==eInt )
			return Pred2ToCasper<Predicate::pLess ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eBoolExpr and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eBoolExpr ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eBoolVar and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pGreater ,eBoolVar ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eBoolExpr and e2==eBool )
			return Pred2ToCasper<Predicate::pLessEqual ,eBoolExpr ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pAnd and e1==eBoolVar and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pAnd ,eBoolVar ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pOr and e1==eBoolExpr and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pOr ,eBoolExpr ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eBool and e2==eBool )
			return Pred2ToCasper<Predicate::pDistinct ,eBool ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eIntExpr and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pLessEqual ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIff and e1==eBool and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pIff ,eBool ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eInt and e2==eInt )
			return Pred2ToCasper<Predicate::pEqual ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pInTable and e1==eIntVarArray and e2==eIntArray )
			return Pred2ToCasper<Predicate::pInTable ,eIntVarArray ,eIntArray ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eBoolExpr and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pEqual ,eBoolExpr ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eIntExpr and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pDistinct ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pXOr and e1==eBoolExpr and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pXOr ,eBoolExpr ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eBoolExpr and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pLess ,eBoolExpr ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eBoolExpr and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eBoolExpr ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==1 and p.getFunctor()==Predicate::pDistinct and e1==eIntArray )
			return Pred1ToCasper<Predicate::pDistinct ,eIntArray ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eBoolVar and e2==eBool )
			return Pred2ToCasper<Predicate::pLess ,eBoolVar ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eBoolExpr and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pLessEqual ,eBoolExpr ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eBoolVar and e2==eBool )
			return Pred2ToCasper<Predicate::pGreater ,eBoolVar ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eInt and e2==eInt )
			return Pred2ToCasper<Predicate::pLessEqual ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eBool and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pDistinct ,eBool ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eIntVar and e2==eInt )
			return Pred2ToCasper<Predicate::pGreater ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eBoolExpr and e2==eBool )
			return Pred2ToCasper<Predicate::pGreater ,eBoolExpr ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eBoolExpr and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pDistinct ,eBoolExpr ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pAnd and e1==eBoolExpr and e2==eBool )
			return Pred2ToCasper<Predicate::pAnd ,eBoolExpr ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pAnd and e1==eBool and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pAnd ,eBool ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pAnd and e1==eBoolExpr and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pAnd ,eBoolExpr ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eIntVar and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pLessEqual ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eInt and e2==eIntVar )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eIntVar and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pLess ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eIntVar and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIf and e1==eBoolVar and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pIf ,eBoolVar ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eIntExpr and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pGreater ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eInt and e2==eIntVar )
			return Pred2ToCasper<Predicate::pDistinct ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIff and e1==eBool and e2==eBool )
			return Pred2ToCasper<Predicate::pIff ,eBool ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pXOr and e1==eBoolVar and e2==eBool )
			return Pred2ToCasper<Predicate::pXOr ,eBoolVar ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eBoolExpr and e2==eBool )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eBoolExpr ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eIntExpr and e2==eInt )
			return Pred2ToCasper<Predicate::pEqual ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eBoolVar and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pLess ,eBoolVar ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eBool and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pDistinct ,eBool ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eInt and e2==eInt )
			return Pred2ToCasper<Predicate::pGreater ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eBoolExpr and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pDistinct ,eBoolExpr ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eIntVar and e2==eInt )
			return Pred2ToCasper<Predicate::pDistinct ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eBoolVar and e2==eBool )
			return Pred2ToCasper<Predicate::pEqual ,eBoolVar ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eBoolVar and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eBoolVar ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eBoolVar and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pEqual ,eBoolVar ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eIntExpr and e2==eInt )
			return Pred2ToCasper<Predicate::pLessEqual ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIf and e1==eBoolExpr and e2==eBool )
			return Pred2ToCasper<Predicate::pIf ,eBoolExpr ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIf and e1==eBoolExpr and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pIf ,eBoolExpr ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pAnd and e1==eBool and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pAnd ,eBool ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eIntExpr and e2==eInt )
			return Pred2ToCasper<Predicate::pDistinct ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eBoolVar and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pLess ,eBoolVar ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eBoolVar and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pGreater ,eBoolVar ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eIntExpr and e2==eIntVar )
			return Pred2ToCasper<Predicate::pGreater ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eIntExpr and e2==eIntVar )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIff and e1==eBool and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pIff ,eBool ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pXOr and e1==eBoolVar and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pXOr ,eBoolVar ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eIntExpr and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eBoolExpr and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pEqual ,eBoolExpr ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIf and e1==eBoolVar and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pIf ,eBoolVar ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eIntVar and e2==eIntVar )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eIntExpr and e2==eInt )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eBoolExpr and e2==eBool )
			return Pred2ToCasper<Predicate::pLess ,eBoolExpr ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eBoolExpr and e2==eBool )
			return Pred2ToCasper<Predicate::pDistinct ,eBoolExpr ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eBoolExpr and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pLess ,eBoolExpr ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eBoolVar and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pLessEqual ,eBoolVar ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eInt and e2==eIntVar )
			return Pred2ToCasper<Predicate::pLessEqual ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pOr and e1==eBool and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pOr ,eBool ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eIntExpr and e2==eInt )
			return Pred2ToCasper<Predicate::pGreater ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eInt and e2==eIntVar )
			return Pred2ToCasper<Predicate::pGreater ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eIntExpr and e2==eIntVar )
			return Pred2ToCasper<Predicate::pLessEqual ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eInt and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eInt ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pXOr and e1==eBoolVar and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pXOr ,eBoolVar ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eBoolVar and e2==eBool )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eBoolVar ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreaterEqual and e1==eInt and e2==eInt )
			return Pred2ToCasper<Predicate::pGreaterEqual ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eBool and e2==eBool )
			return Pred2ToCasper<Predicate::pGreater ,eBool ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eIntExpr and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pEqual ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eIntExpr and e2==eIntVar )
			return Pred2ToCasper<Predicate::pEqual ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eBoolVar and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pLessEqual ,eBoolVar ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eBoolVar and e2==eBool )
			return Pred2ToCasper<Predicate::pDistinct ,eBoolVar ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eIntVar and e2==eInt )
			return Pred2ToCasper<Predicate::pLessEqual ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eInt and e2==eInt )
			return Pred2ToCasper<Predicate::pDistinct ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pIff and e1==eBoolVar and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pIff ,eBoolVar ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eIntVar and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pEqual ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLessEqual and e1==eBool and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pLessEqual ,eBool ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pEqual and e1==eInt and e2==eIntVar )
			return Pred2ToCasper<Predicate::pEqual ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eIntVar and e2==eIntExpr )
			return Pred2ToCasper<Predicate::pGreater ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eIntExpr and e2==eIntVar )
			return Pred2ToCasper<Predicate::pDistinct ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pGreater and e1==eBoolExpr and e2==eBoolExpr )
			return Pred2ToCasper<Predicate::pGreater ,eBoolExpr ,eBoolExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pDistinct and e1==eIntVar and e2==eIntVar )
			return Pred2ToCasper<Predicate::pDistinct ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pOr and e1==eBool and e2==eBool )
			return Pred2ToCasper<Predicate::pOr ,eBool ,eBool ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pLess and e1==eBool and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pLess ,eBool ,eBoolVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getFunctor()==Predicate::pXOr and e1==eBool and e2==eBoolVar )
			return Pred2ToCasper<Predicate::pXOr ,eBool ,eBoolVar ,cBounds>()(solver,p);
		throw EImplement(s);
	}
};
template<> struct SymbolToCasper<eIntExpr,cBounds>
{
	typedef Casper::BndExpr<Casper::int> Type;
	Type operator()(CPSolver& solver,const Symbol& s)
	{
		const Expression& p = s;
		Eval e1,e2,e3;
		e1 = getEval(p[0]);
		if (p.getArity()>=2) e2 = getEval(p[1]);
		if (p.getArity()>=3) e3 = getEval(p[2]);
		if (p.getArity()==2 and p.getOperator()==Expression::eMax and e1==eInt and e2==eInt )
			return Expr2ToCasper<Type,Expression::eMax ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMax and e1==eIntVar and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eMax ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eElem and e1==eIntArray and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eElem ,eIntArray ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eDiv and e1==eIntExpr and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eDiv ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eDiv and e1==eIntExpr and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eDiv ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMin and e1==eInt and e2==eInt )
			return Expr2ToCasper<Type,Expression::eMin ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMax and e1==eInt and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eMax ,eInt ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMax and e1==eInt and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eMax ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSumProduct and e1==eIntVarArray and e2==eIntArray )
			return Expr2ToCasper<Type,Expression::eSumProduct ,eIntVarArray ,eIntArray ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMul and e1==eIntExpr and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eMul ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMin and e1==eInt and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eMin ,eInt ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eDiv and e1==eInt and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eDiv ,eInt ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMax and e1==eIntVar and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eMax ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMul and e1==eInt and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eMul ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMul and e1==eIntExpr and e2==eInt )
			return Expr2ToCasper<Type,Expression::eMul ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMod and e1==eInt and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eMod ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eDiv and e1==eInt and e2==eInt )
			return Expr2ToCasper<Type,Expression::eDiv ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eDiv and e1==eIntVar and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eDiv ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMul and e1==eIntVar and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eMul ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSub and e1==eInt and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eSub ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMod and e1==eInt and e2==eInt )
			return Expr2ToCasper<Type,Expression::eMod ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMin and e1==eIntExpr and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eMin ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSumProduct and e1==eIntVarArray and e2==eIntVarArray )
			return Expr2ToCasper<Type,Expression::eSumProduct ,eIntVarArray ,eIntVarArray ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eDiv and e1==eInt and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eDiv ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMin and e1==eInt and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eMin ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==1 and p.getOperator()==Expression::eSym and e1==eIntVar )
			return Expr1ToCasper<Type,Expression::eSym ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMin and e1==eIntVar and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eMin ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSumProduct and e1==eIntArray and e2==eIntArray )
			return Expr2ToCasper<Type,Expression::eSumProduct ,eIntArray ,eIntArray ,cBounds>()(solver,p);
		if (p.getArity()==1 and p.getOperator()==Expression::eSym and e1==eInt )
			return Expr1ToCasper<Type,Expression::eSym ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eDiv and e1==eIntVar and e2==eInt )
			return Expr2ToCasper<Type,Expression::eDiv ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMax and e1==eIntVar and e2==eInt )
			return Expr2ToCasper<Type,Expression::eMax ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSumProduct and e1==eIntArray and e2==eIntVarArray )
			return Expr2ToCasper<Type,Expression::eSumProduct ,eIntArray ,eIntVarArray ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMin and e1==eIntVar and e2==eInt )
			return Expr2ToCasper<Type,Expression::eMin ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMod and e1==eIntVar and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eMod ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==1 and p.getOperator()==Expression::eAbs and e1==eIntExpr )
			return Expr1ToCasper<Type,Expression::eAbs ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSub and e1==eInt and e2==eInt )
			return Expr2ToCasper<Type,Expression::eSub ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eAdd and e1==eIntVar and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eAdd ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMax and e1==eIntExpr and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eMax ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSub and e1==eInt and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eSub ,eInt ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSub and e1==eIntVar and e2==eInt )
			return Expr2ToCasper<Type,Expression::eSub ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMax and e1==eIntExpr and e2==eInt )
			return Expr2ToCasper<Type,Expression::eMax ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==1 and p.getOperator()==Expression::eSym and e1==eIntExpr )
			return Expr1ToCasper<Type,Expression::eSym ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMod and e1==eInt and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eMod ,eInt ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMod and e1==eIntVar and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eMod ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSub and e1==eIntVar and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eSub ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSub and e1==eIntExpr and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eSub ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eElem and e1==eIntVarArray and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eElem ,eIntVarArray ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==1 and p.getOperator()==Expression::eAbs and e1==eIntVar )
			return Expr1ToCasper<Type,Expression::eAbs ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMul and e1==eIntVar and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eMul ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eElem and e1==eIntArray and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eElem ,eIntArray ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMod and e1==eIntExpr and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eMod ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eDiv and e1==eIntVar and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eDiv ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSub and e1==eIntExpr and e2==eInt )
			return Expr2ToCasper<Type,Expression::eSub ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==1 and p.getOperator()==Expression::eAbs and e1==eInt )
			return Expr1ToCasper<Type,Expression::eAbs ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMod and e1==eIntExpr and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eMod ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eElem and e1==eIntArray and e2==eInt )
			return Expr2ToCasper<Type,Expression::eElem ,eIntArray ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eAdd and e1==eIntExpr and e2==eInt )
			return Expr2ToCasper<Type,Expression::eAdd ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSub and e1==eIntExpr and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eSub ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMax and e1==eIntExpr and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eMax ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMin and e1==eIntVar and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eMin ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMin and e1==eIntExpr and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eMin ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMul and e1==eIntVar and e2==eInt )
			return Expr2ToCasper<Type,Expression::eMul ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eAdd and e1==eIntExpr and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eAdd ,eIntExpr ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eAdd and e1==eIntExpr and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eAdd ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMin and e1==eIntExpr and e2==eInt )
			return Expr2ToCasper<Type,Expression::eMin ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eSub and e1==eIntVar and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eSub ,eIntVar ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eElem and e1==eIntVarArray and e2==eInt )
			return Expr2ToCasper<Type,Expression::eElem ,eIntVarArray ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMod and e1==eIntExpr and e2==eInt )
			return Expr2ToCasper<Type,Expression::eMod ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMod and e1==eIntVar and e2==eInt )
			return Expr2ToCasper<Type,Expression::eMod ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eAdd and e1==eInt and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eAdd ,eInt ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eAdd and e1==eInt and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eAdd ,eInt ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eElem and e1==eIntVarArray and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eElem ,eIntVarArray ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eAdd and e1==eInt and e2==eInt )
			return Expr2ToCasper<Type,Expression::eAdd ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eAdd and e1==eIntVar and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eAdd ,eIntVar ,eIntExpr ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMul and e1==eIntExpr and e2==eIntVar )
			return Expr2ToCasper<Type,Expression::eMul ,eIntExpr ,eIntVar ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMul and e1==eInt and e2==eInt )
			return Expr2ToCasper<Type,Expression::eMul ,eInt ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eDiv and e1==eIntExpr and e2==eInt )
			return Expr2ToCasper<Type,Expression::eDiv ,eIntExpr ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eAdd and e1==eIntVar and e2==eInt )
			return Expr2ToCasper<Type,Expression::eAdd ,eIntVar ,eInt ,cBounds>()(solver,p);
		if (p.getArity()==2 and p.getOperator()==Expression::eMul and e1==eInt and e2==eIntExpr )
			return Expr2ToCasper<Type,Expression::eMul ,eInt ,eIntExpr ,cBounds>()(solver,p);
		throw EImplement(s);
	}
};
template<> struct SymbolToCasper<eRealExpr,cBounds>
{
	typedef Casper::BndExpr<Casper::double> Type;
	Type operator()(CPSolver& solver,const Symbol& s)
	{
		const Expression& p = s;
		Eval e1,e2,e3;
		e1 = getEval(p[0]);
		if (p.getArity()>=2) e2 = getEval(p[1]);
		if (p.getArity()>=3) e3 = getEval(p[2]);
		throw EImplement(s);
	}
};
template<> struct SymbolToCasper<eIntSetExpr,cBounds>
{
	typedef Casper::DomExpr<Casper::IntSet> Type;
	Type operator()(CPSolver& solver,const Symbol& s)
	{
		const Expression& p = s;
		Eval e1,e2,e3;
		e1 = getEval(p[0]);
		if (p.getArity()>=2) e2 = getEval(p[1]);
		if (p.getArity()>=3) e3 = getEval(p[2]);
		throw EImplement(s);
	}
};
template<> struct SymbolToCasper<eBoolSetExpr,cBounds>
{
	typedef Casper::DomExpr<Casper::BoolSet> Type;
	Type operator()(CPSolver& solver,const Symbol& s)
	{
		const Expression& p = s;
		Eval e1,e2,e3;
		e1 = getEval(p[0]);
		if (p.getArity()>=2) e2 = getEval(p[1]);
		if (p.getArity()>=3) e3 = getEval(p[2]);
		throw EImplement(s);
	}
};
