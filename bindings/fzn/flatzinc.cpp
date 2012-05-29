/*
 *  Adapted for Casper from skeleton code by Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *  	Guido Tack, 2007
 *  	Marco Correia, 2012
 */


#include <bindings/fzn/flatzinc.hh>
#include <thirdparty/flatzinc_skeleton/registry.hh>

#include <vector>
#include <string>
using namespace std;

#ifdef CASPER_LOG_NRANGES_DEPTH
namespace Casper {
namespace CP {
namespace Detail {

Casper::Util::StdList<int> depthCount;
Casper::Util::StdList<int> rangesAcc; 

struct LogRangeCount : IGoal 
{
	LogRangeCount(IExplorer& explorer, const IntVarArray& vars) : explorer(explorer),vars(vars) {}
	Goal execute()
	{
		auto it1 = depthCount.begin();
		auto it2 = rangesAcc.begin();
		int curDepth = explorer.getStats().curDepth;
		while (it1 != depthCount.end() && curDepth>=0)
		{
			++it1;
			++it2;
			--curDepth;
		}
		while (curDepth>=0)
		{
			it1 = depthCount.pushBack(0);
			it2 = rangesAcc.pushBack(0);
			--curDepth;
		}
		
                for (uint i = 0; i < vars.size(); ++i)
                {
                        int nranges = 1;
                        auto it = vars[i].domain().begin();
                        if (!vars[i].domain().ground())
                        {
                                int lastVal = *it;
                                ++it;
                                for ( ; it != vars[i].domain().end(); ++it)
                                        if (*it > lastVal+1)
                                        {
                                                ++nranges;
                                                lastVal = *it;
                                        }
                        }
                        *it2 += nranges;
                }
                ++*it1;
		return succeed();	
	}
	IExplorer& explorer;
	IntVarArray vars;
};

Goal logRangeCount(IExplorer& explorer, const IntVarArray& vars) 
{	return new (explorer.state) LogRangeCount(explorer,vars);	}

}
}
}

#endif

namespace FlatZinc {
  
  FlatZincModel::FlatZincModel(void)
  : intVarCount(-1), boolVarCount(-1), setVarCount(-1), _optVar(-1),
    _solveAnnotations(NULL),
    valid(true),
    search(Casper::succeed()),
    piv(NULL),pbv(NULL),psv(NULL)
  {
	#ifdef CASPER_PROFILE
	Casper::Util::Alarm::start(100,true);
	#endif
  }

  void
  FlatZincModel::init(int intVars, int boolVars, int setVars) {
    intVarCount = 0;
    piv = new (solver) IntVarArray(solver,intVars);
    iv_introduced = std::vector<bool>(intVars);
    iv_boolalias = std::vector<int>(intVars);
    boolVarCount = 0;
    pbv = new (solver) BoolVarArray(solver,boolVars);
    bv_introduced = std::vector<bool>(boolVars);
    setVarCount = 0;
    psv = new (solver) SetVarArray(solver,setVars);
    sv_introduced = std::vector<bool>(setVars);
  }

void FlatZincModel::newIntVar(IntVarSpec* vs)
{
	if (vs->alias)
		(*piv)[intVarCount++] = (*piv)[vs->i];
	else
	{
		//std::cerr << "create new IntVar " << intVarCount << "\n";
		/// create actual integer variable from vs
		if (vs->assigned)
			(*piv)[intVarCount++] = IntVar(solver,vs->i,vs->i);
		else
		if (vs->domain())
		{
			AST::SetLit* sl = vs->domain.some();
			if (sl->interval)
				(*piv)[intVarCount++] = IntVar(solver,sl->min,sl->max);
			else
			{
				int* newdom = new int[sl->s.size()];
				for (int i=sl->s.size(); i--;)
					newdom[i] = sl->s[i];
				std::sort(newdom,&newdom[sl->s.size()]);
				(*piv)[intVarCount++] = IntVar(solver,newdom,&newdom[sl->s.size()]);
				delete newdom;
			}
		}
		else
			(*piv)[intVarCount++] = IntVar(solver,Casper::limits<int>::min(), Casper::limits<int>::max());
	}
	iv_introduced[intVarCount-1] = vs->introduced;
	iv_boolalias[intVarCount-1] = -1;
}

  void
  FlatZincModel::aliasBool2Int(int iv, int bv) {
    iv_boolalias[iv] = bv;
  }
  int
  FlatZincModel::aliasBool2Int(int iv) {
    return iv_boolalias[iv];
  }

void  FlatZincModel::newBoolVar(BoolVarSpec* vs)
{
	if (vs->alias)
		(*pbv)[boolVarCount++] = (*pbv)[vs->i];
	else
	{
		//std::cerr << "create new BoolVar " << boolVarCount << "\n";
		/// create actual Boolean variable from vs
		if (vs->assigned)
			(*pbv)[boolVarCount++] = BoolVar(solver,vs->i);
		else
		if (vs->domain())
		{
			AST::SetLit* sl = vs->domain.some();
			assert(sl->interval);
			(*pbv)[boolVarCount++] = BoolVar(solver,sl->min,sl->max);
		}
		else
			(*pbv)[boolVarCount++] = BoolVar(solver,false,true);
	}
	bv_introduced[boolVarCount-1] = vs->introduced;
}

void FlatZincModel::newSetVar(SetVarSpec* vs)
{
	if (vs->alias)
		(*psv)[setVarCount++] = (*psv)[vs->i];
	else
	{
		//std::cerr << "create new SetVar " << setVarCount << "\n";
		/// create actual set variable from vs

		if (vs->assigned)
		{
			assert(vs->upperBound());
			AST::SetLit* vsv = vs->upperBound.some();
			if (vsv->interval)
			{
				Casper::Util::StdRange<int> d(vsv->min, vsv->max);
				(*psv)[setVarCount++] = SetVar(solver, d, d);
			}
			else
			{
				int* is = new int[vsv->s.size()];
				for (int i=vsv->s.size(); i--; )
					is[i] = vsv->s[i];
				std::sort(is, &is[vsv->s.size()]);
				(*psv)[setVarCount++] = SetVar(solver, is, &is[vsv->s.size()],
												&is[vsv->s.size()], &is[vsv->s.size()]);
				delete is;
			}
		 }
		else
		if (vs->upperBound())
		{
			AST::SetLit* vsv = vs->upperBound.some();
			if (vsv->interval)
			{
				Casper::Util::StdRange<int> d(vsv->min, vsv->max);
				(*psv)[setVarCount++] = SetVar(solver, d);
			}
			else
			{
				int* is = new int[vsv->s.size()];
				for (int i=vsv->s.size(); i--; )
					is[i] = vsv->s[i];
				std::sort(is, &is[vsv->s.size()]);
				(*psv)[setVarCount++] = SetVar(solver, is, &is[vsv->s.size()]);
				delete is;
			}
		 }
		else
			(*psv)[setVarCount++] = SetVar(solver,
					Casper::Util::StdRange<int>(Casper::limits<int>::min(),
												Casper::limits<int>::max()));
	}
	sv_introduced[setVarCount-1] = vs->introduced;
}


  void
  FlatZincModel::postConstraint(const ConExpr& ce, AST::Node* ann) {
    try {
      registry().post(*this, ce, ann);
    } catch (AST::TypeError& e) {
      throw FlatZinc::Error("Type error", e.what());
    }
  }

  void flattenAnnotations(AST::Array* ann, std::vector<AST::Node*>& out) {
      for (unsigned int i=0; i<ann->a.size(); i++) {
        if (ann->a[i]->isCall("seq_search")) {
          AST::Call* c = ann->a[i]->getCall();
          if (c->args->isArray())
            flattenAnnotations(c->args->getArray(), out);
          else
            out.push_back(c->args);
        } else {
          out.push_back(ann->a[i]);
        }
      }
  }

  namespace {
  template<class VarArray>
  Casper::CP::VarSelector getIntVarSelector(Casper::CP::Store& store,
		  	  	  	  	  	  	  	  	  	const VarArray& va,AST::Node* ann)
  {
	  if (AST::Atom* s = dynamic_cast<AST::Atom*>(ann)) {
	      if (s->id == "input_order")
	        return Casper::CP::selectVarLex(store,va,false);
	      if (s->id == "first_fail")
	        return Casper::CP::selectVarMinDom(store,va,false);
	      if (s->id == "anti_first_fail")
	        return Casper::CP::selectVarMaxDom(store,va,false);
	      if (s->id == "smallest")
	        return Casper::CP::selectVarMinMinElem(store,va,false);
	      if (s->id == "largest")
	        return Casper::CP::selectVarMaxMaxElem(store,va,false);
	      if (s->id == "occurrence")
	        return Casper::CP::selectVarMaxDegree(store,va,false);
	      //if (s->id == "max_regret")
	      //  return TieBreakVarBranch<IntVarBranch>(INT_VAR_REGRET_MIN_MAX);
	      //if (s->id == "most_constrained")
	      //  return TieBreakVarBranch<IntVarBranch>(INT_VAR_SIZE_MIN,
	      //    INT_VAR_DEGREE_MAX);
	      //if (s->id == "random")
	      //  return TieBreakVarBranch<IntVarBranch>(INT_VAR_RND);
	      if (s->id == "afc_min")
	        return Casper::CP::selectVarMinWeightedDegree(store,va,false);
	      if (s->id == "afc_max")
	        return Casper::CP::selectVarMaxWeightedDegree(store,va,false);
	      if (s->id == "size_afc_min")
	        return Casper::CP::selectVarMinDomOverWeightedDegree(store,va,false);
	      if (s->id == "size_afc_max")
	        return Casper::CP::selectVarMaxDomOverWeightedDegree(store,va,false);
	    }
	    std::cerr << "Warning, ignored search annotation: ";
	    ann->print(std::cerr);
	    std::cerr << std::endl;
	    return Casper::CP::selectVarLex(store,va,false);
  }

  template<class VarArray>
  Casper::CP::ValSelector getIntValSelector(Casper::CP::Store& store,
		  	  	  	  	  	  	  	  	  	const VarArray& va,AST::Node* ann)
  {
    if (AST::Atom* s = dynamic_cast<AST::Atom*>(ann)) {
      if (s->id == "indomain_min")
        return Casper::CP::selectValMin(store,va);
      if (s->id == "indomain_max")
        return Casper::CP::selectValMax(store,va);
      //if (s->id == "indomain_median")
      //  return INT_VAL_MED;
      if (s->id == "indomain_split")
        return Casper::CP::selectHalfMin(store,va);
      if (s->id == "indomain_reverse_split")
        return Casper::CP::selectHalfMax(store,va);
      //if (s->id == "indomain_random")
      //  return INT_VAL_RND;
      if (s->id == "indomain")
        return Casper::CP::selectValsMin(store,va);
      //if (s->id == "indomain_middle") {
      //  std::cerr << "Warning, replacing unsupported annotation "
      //            << "indomain_middle with indomain_median" << std::endl;
      //  return INT_VAL_MED;
      //}
      if (s->id == "indomain_interval") {
        std::cerr << "Warning, replacing unsupported annotation "
                  << "indomain_interval with indomain_split" << std::endl;
        return Casper::CP::selectHalfMin(store,va);
      }
    }
    std::cerr << "Warning, ignored search annotation: ";
    ann->print(std::cerr);
    std::cerr << std::endl;
    return  Casper::CP::selectValMin(store,va);
  }

  Casper::CP::VarSelector getSetVarSelector(Casper::CP::Store& store,
 		  	  	  	  	  	  	  	  	  	const SetVarArray& va,AST::Node* ann)
  {
	  if (AST::Atom* s = dynamic_cast<AST::Atom*>(ann)) {
		  if (s->id == "input_order")
			return Casper::CP::selectVarLex(store,va,false);
		  if (s->id == "first_fail")
			return Casper::CP::selectVarMinDom(store,va,false);
		  if (s->id == "anti_first_fail")
			return Casper::CP::selectVarMaxDom(store,va,false);
		  if (s->id == "smallest")
			return Casper::CP::selectVarMinMinElem(store,va,false);
		  if (s->id == "largest")
			return Casper::CP::selectVarMaxMaxElem(store,va,false);
	  }
    std::cerr << "Warning, ignored search annotation: ";
    ann->print(std::cerr);
    std::cerr << std::endl;
    return Casper::CP::selectVarLex(store,va,false);
  }

  Casper::CP::ValSelector getSetValSelector(Casper::CP::Store& store,
		  	  	  	  	  	  	  	  	  	const SetVarArray& va,AST::Node* ann)
  {
    if (AST::Atom* s = dynamic_cast<AST::Atom*>(ann)) {
      if (s->id == "indomain_min")
        return Casper::CP::selectValMin(store,va);
      if (s->id == "indomain_max")
        return Casper::CP::selectValMax(store,va);
//      if (s->id == "outdomain_min")
//        return SET_VAL_MIN_EXC;
//      if (s->id == "outdomain_max")
//        return SET_VAL_MAX_EXC;
    }
    std::cerr << "Warning, ignored search annotation: ";
    ann->print(std::cerr);
    std::cerr << std::endl;
    return Casper::CP::selectValMin(store,va);
  }

  } // anonymous

  void  FlatZincModel::createBranchers(AST::Node* ann, bool ignoreUnknown,
                                 std::ostream& err)
  {
    if (ann) {
      std::vector<AST::Node*> flatAnn;
      if (ann->isArray()) {
        flattenAnnotations(ann->getArray()  , flatAnn);
      } else {
        flatAnn.push_back(ann);
      }

      for (unsigned int i=0; i<flatAnn.size(); i++) {
        try {
          AST::Call *call = flatAnn[i]->getCall("int_search");
          AST::Array *args = call->getArgs(4);
          AST::Array *vars = args->a[0]->getArray();

          // std::cerr << "int_search\n";
          // install search
          int k=vars->a.size();
          for (int i=vars->a.size(); i--;)
        	  if (vars->a[i]->isInt())
        		  k--;
          IntVarArray va(solver,k);
          k=0;
          for (unsigned int i=0; i<vars->a.size(); i++) {
        	  if (vars->a[i]->isInt())
        		  continue;
              va[k++] = (*piv)[vars->a[i]->getIntVar()];
          }

          search = Casper::CP::label(solver,va,/*Casper::CP::Detail::logRangeCount(*solver.getExplorer(),*piv),*/getIntVarSelector(solver,va,args->a[1]),
        		  	  	  	  	   	   	   	   getIntValSelector(solver,va,args->a[2]));
        } catch (AST::TypeError& e) {
          (void) e;
          try {
            AST::Call *call = flatAnn[i]->getCall("bool_search");
            AST::Array *args = call->getArgs(4);
            AST::Array *vars = args->a[0]->getArray();

            // std::cerr << "bool_search\n";
            // install search
            int k=vars->a.size();
            for (int i=vars->a.size(); i--;)
            	if (vars->a[i]->isBool())
            		k--;
            BoolVarArray va(solver,k);
            k=0;
            for (unsigned int i=0; i<vars->a.size(); i++) {
            	if (vars->a[i]->isBool())
            		continue;
            	va[k++] = (*pbv)[vars->a[i]->getBoolVar()];
            }
            search = Casper::CP::label(solver,va,getIntVarSelector(solver,va,args->a[1]),
          		  	  	  	  	   	   	   	   	 getIntValSelector(solver,va,args->a[2]));

          } catch (AST::TypeError& e) {
            (void) e;
            try {
              AST::Call *call = flatAnn[i]->getCall("set_search");
              AST::Array *args = call->getArgs(4);
              AST::Array *vars = args->a[0]->getArray();

              // std::cerr << "set_search\n";
              // install search
              int k=vars->a.size();
              for (int i=vars->a.size(); i--;)
            	  if (vars->a[i]->isSet())
            		  k--;
              SetVarArray va(solver,k);
              k=0;
              for (unsigned int i=0; i<vars->a.size(); i++) {
            	  if (vars->a[i]->isSet())
            		  continue;
                  va[k++] = (*psv)[vars->a[i]->getSetVar()];
               }
               search = Casper::CP::label(solver,va,getSetVarSelector(solver,va,args->a[1]),
             		  	  	  	  	   	   	   	   	getSetValSelector(solver,va,args->a[2]));
            } catch (AST::TypeError& e) {
              (void) e;
              if (!ignoreUnknown) {
                err << "Warning, ignored search annotation: ";
                flatAnn[i]->print(err);
                err << std::endl;
              }
            }
          }
        }
      }
    }

    // install search for all remaining variables
    int introduced = 0;
    for (int i=piv->size(); i--;)
      if (iv_introduced[i])
        introduced++;
    IntVarArray iv_sol(solver,piv->size()-introduced);
    IntVarArray iv_tmp(solver,introduced);
    for (int i=piv->size(), j=0, k=0; i--;)
      if (iv_introduced[i])
        iv_tmp[j++] = (*piv)[i];
      else
        iv_sol[k++] = (*piv)[i];

    introduced = 0;
    for (int i=pbv->size(); i--;)
      if (bv_introduced[i])
        introduced++;
    BoolVarArray bv_sol(solver,pbv->size()-introduced);
    BoolVarArray bv_tmp(solver,introduced);
    for (int i=pbv->size(), j=0, k=0; i--;)
      if (bv_introduced[i])
        bv_tmp[j++] = (*pbv)[i];
      else
        bv_sol[k++] = (*pbv)[i];

    introduced = 0;
    for (int i=psv->size(); i--;)
      if (sv_introduced[i])
        introduced++;
    SetVarArray sv_sol(solver,psv->size()-introduced);
    SetVarArray sv_tmp(solver,introduced);
    for (int i=psv->size(), j=0, k=0; i--;)
      if (sv_introduced[i])
        sv_tmp[j++] = (*psv)[i];
      else
        sv_sol[k++] = (*psv)[i];

    if (!iv_sol.empty())
		search = Casper::Goal(solver,search and
					Casper::CP::label(solver,iv_sol,
							Casper::CP::selectVarMinDomOverWeightedDegree(solver,iv_sol)));

    if (!bv_sol.empty())
		search = Casper::Goal(solver,search and
					Casper::CP::label(solver,bv_sol,
							Casper::CP::selectVarMaxWeightedDegree(solver,bv_sol)));

    if (!sv_sol.empty())
		search = Casper::Goal(solver,search and
					Casper::CP::label(solver,sv_sol,
							Casper::CP::selectVarMinDomOverWeightedDegree(solver,sv_sol)));

    if (!iv_tmp.empty())
		search = Casper::Goal(solver,search and
					Casper::CP::label(solver,iv_tmp,
							Casper::CP::selectVarMinDomOverWeightedDegree(solver,iv_tmp)));

    if (!bv_tmp.empty())
		search = Casper::Goal(solver,search and
					Casper::CP::label(solver,bv_tmp,
							Casper::CP::selectVarMaxWeightedDegree(solver,bv_tmp)));

    if (!sv_tmp.empty())
		search = Casper::Goal(solver,search and
					Casper::CP::label(solver,sv_tmp,
							Casper::CP::selectVarMinDomOverWeightedDegree(solver,sv_tmp)));
  }

  AST::Array*
  FlatZincModel::solveAnnotations(void) const {
    return _solveAnnotations;
  }

  void
  FlatZincModel::solve(AST::Array* ann) {
    _method = SAT;
    _solveAnnotations = ann;
  }

  void
  FlatZincModel::minimize(int var, AST::Array* ann) {
    _method = MIN;
    _optVar = var;
    _solveAnnotations = ann;
    // Branch on optimization variable to ensure that it is given a value.
    AST::Array* args = new AST::Array(4);
    args->a[0] = new AST::Array(new AST::IntVar(_optVar));
    args->a[1] = new AST::Atom("input_order");
    args->a[2] = new AST::Atom("indomain_min");
    args->a[3] = new AST::Atom("complete");
    AST::Call* c = new AST::Call("int_search", args);
    if (!ann)
      ann = new AST::Array(c);
    else
      ann->a.push_back(c);
  }

  void
  FlatZincModel::maximize(int var, AST::Array* ann) {
    _method = MAX;
    _optVar = var;
    _solveAnnotations = ann;
    // Branch on optimization variable to ensure that it is given a value.
    AST::Array* args = new AST::Array(4);
    args->a[0] = new AST::Array(new AST::IntVar(_optVar));
    args->a[1] = new AST::Atom("input_order");
    args->a[2] = new AST::Atom("indomain_min");
    args->a[3] = new AST::Atom("complete");
    AST::Call* c = new AST::Call("int_search", args);
    if (!ann)
      ann = new AST::Array(c);
    else
      ann->a.push_back(c);
  }

  FlatZincModel::~FlatZincModel(void) {
    delete _solveAnnotations;
  }

  void FlatZincModel::runSAT(std::ostream& out, const Printer& p, const FlatZinc::Options& opt)
  {
	  bool nbSols = 0;
	  valid = valid and solver.solve(search);
	  while (valid)
	  {
		  print(out,p);
		  out << "----------" << std::endl;
		  if (++nbSols == opt.nbSolutions() and !opt.allSolutions())
			  break;
		  valid = solver.next();
	  }
	  if (!valid)
	  {
		  if (nbSols > 0)
			  out << "==========" << std::endl;
		  else
			  out << "=====UNSATISFIABLE=====" << endl;
	  }
  }

  void  FlatZincModel::printStats(std::ostream& out)
  {
	  const int lw=32;
	  const int rw=10;
		out  << left << setw(lw) << "% Number of Boolean domains" << ":" <<  std::setw(rw) << std::right
			  << static_cast<Casper::CP::Store&>(solver).getStats().getNbBoolDomains() << std::endl
		   << left << setw(lw) << "% Number of integer domains" << ":" <<  std::setw(rw) << std::right
		      << static_cast<Casper::CP::Store&>(solver).getStats().getNbIntDomains() << std::endl
		   << left << setw(lw) << "% Number of set domains" << ":" <<  std::setw(rw) << std::right
			  << static_cast<Casper::CP::Store&>(solver).getStats().getNSetDomains() << std::endl
		   << left << setw(lw) << "% Number of range domains" << ":" <<  std::setw(rw) << std::right
			  << static_cast<Casper::CP::Store&>(solver).getStats().getNbRangeDomains() << std::endl
		   << left << setw(lw) << "% Number of filters" << ":" <<  std::setw(rw) << std::right
			  << static_cast<Casper::CP::Store&>(solver).getStats().getNbFilters() << std::endl
		   << left << setw(lw) << "% Number of filter executions" << ":" <<  std::setw(rw) << std::right
			  << static_cast<Casper::CP::Store&>(solver).getStats().getNbPropagations() << std::endl;
#ifdef CASPER_EXTRA_STATS
	   out << left << setw(lw) << "% Number of effective filter executions" << ":" <<  std::setw (rw) << std::right
			  << static_cast<Casper::CP::Store&>(solver).getStats().getNbEffectivePropagations() << " ("
			  << (static_cast<Casper::CP::Store&>(solver).getStats().getNbEffectivePropagations()*100.0/
				  static_cast<Casper::CP::Store&>(solver).getStats().getNbPropagations()) << "%)" << std::endl;
	   out << left << setw(lw) << "% Number of Boolean domain updates" << ":" <<  std::setw(rw) << std::right
			  << static_cast<Casper::CP::Store&>(solver).getStats().getNbBoolDomainUpdates() << endl
	      << left << setw(lw) << "% Number of integer domain updates" << ":" <<  std::setw(rw) << std::right
			  << static_cast<Casper::CP::Store&>(solver).getStats().getNbIntDomainUpdates() << endl
	      << left << setw(lw) << "% Number of set domain updates" << ":" <<  std::setw(rw) << std::right
			  << static_cast<Casper::CP::Store&>(solver).getStats().getNbSetDomainUpdates() << endl
	      << left << setw(lw) << "% Number of range domain updates" << ":" <<  std::setw(rw) << std::right
			  << static_cast<Casper::CP::Store&>(solver).getStats().getNbRangeDomainUpdates() << endl;
#endif
		out << left << setw(lw) << "% Number of choice points" << ":" <<  std::setw(rw) << std::right
			   << solver.getExplorer()->getStats().getNbChoicePoints() << std::endl
		   << left << setw(lw) << "% Number of fixpoints" << ":" <<  std::setw(rw) << std::right
			  << static_cast<Casper::CP::Store&>(solver).getStats().getNbFPComputations() << std::endl
		   << left << setw(lw) << "% Number of fails" << ":" <<  std::setw(rw) << std::right
		   	   << solver.getExplorer()->getStats().getNbFails() << std::endl
		   << left << setw(lw) << "% Maximum depth" << ":" <<  std::setw(rw) << std::right
		   	   << solver.getExplorer()->getStats().getMaxDepth() << std::endl
		   << left << setw(lw) << "% Number of solutions" << ":" <<  std::setw(rw) << std::right
		   	   << solver.getExplorer()->getStats().getNbSolutions() << std::endl
		   << left << setw(lw) << "% Peak of reversible heap (KiB)" << ":" <<  std::setw(rw) << std::right
			   << (static_cast<const Casper::Util::IHeap&>(solver).maxAllocated()/1024) << std::endl;

		#ifdef CASPER_PROFILE
		Casper::Util::profiler.print(out);
		#endif

		#ifdef CASPER_LOG_NRANGES_DEPTH
           	out << "% NbRangesDist: ";
		for (auto it = Casper::CP::Detail::depthCount.begin(); it != Casper::CP::Detail::depthCount.end(); ++it )
		{
			out << *it << " ";
		}
		out << std::endl;
		#endif


  }
  void  FlatZincModel::run(std::ostream& out, const Printer& p, const FlatZinc::Options& opt)
  {
	  switch (_method)
	  {
	  	  case MIN:
	  	  case MAX:
	  		  std::cerr << "start optimization search\n";
	  		  // TODO: perform actual search
	  		  break;
	  	  case SAT:
	  		  //std::cerr << "start satisfiability search\n";
	  		  // perform actual search
	  		  runSAT(out,p,opt);
	  		  break;
	  }
	#ifdef CASPER_PROFILE
	Casper::Util::Alarm::stop();
	#endif

	  if (opt.showStats())
		  printStats(out);
  }

  FlatZincModel::Meth
  FlatZincModel::method(void) const {
    return _method;
  }

  int
  FlatZincModel::optVar(void) const {
    return _optVar;
  }

  void
  FlatZincModel::print(std::ostream& out, const Printer& p) const {
    p.print(out, *this);
  }

  void
  Printer::init(AST::Array* output) {
    _output = output;
  }

  void
  Printer::printElem(std::ostream& out, AST::Node* ai,
                     const FlatZincModel& m) const {
	  assert(m.valid);
    int k;
    if (ai->isInt(k)) {
      out << k;
    } else if (ai->isIntVar()) {
      // output actual variable
      out << (*m.piv)[ai->getIntVar()];
    } else if (ai->isBoolVar()) {
      // output actual variable
      if (!(*m.pbv)[ai->getBoolVar()].ground())
    	  out << "false..true";
      else
      if ((*m.pbv)[ai->getBoolVar()].domain().value())
    	  out << "true";
      else
    	  out << "false";
    } else if (ai->isSetVar()) {
      // output actual variable
    	if ((*m.psv)[ai->getSetVar()].ground())
    	{
    		out << "{";
    		auto it = (*m.psv)[ai->getSetVar()].domain().beginIn();
    		if (it != (*m.psv)[ai->getSetVar()].domain().endIn())
    		{
    			out << *it;
    			++it;
    		}
    		for ( ; it != (*m.psv)[ai->getSetVar()].domain().endIn(); ++it)
    			out << ", " << *it;
    		out << "}";
    	}
    	else
    		out << (*m.psv)[ai->getSetVar()]; // FIXME: ?

    } else if (ai->isBool()) {
      out << (ai->getBool() ? "true" : "false");
    } else if (ai->isSet()) {
      AST::SetLit* s = ai->getSet();
      if (s->interval) {
        out << s->min << ".." << s->max;
      } else {
        out << "{";
        for (unsigned int i=0; i<s->s.size(); i++) {
          out << s->s[i] << (i < s->s.size()-1 ? ", " : "}");
        }
      }
    } else if (ai->isString()) {
      std::string s = ai->getString();
      for (unsigned int i=0; i<s.size(); i++) {
        if (s[i] == '\\' && i<s.size()-1) {
          switch (s[i+1]) {
          case 'n': out << "\n"; break;
          case '\\': out << "\\"; break;
          case 't': out << "\t"; break;
          default: out << "\\" << s[i+1];
          }
          i++;
        } else {
          out << s[i];
        }
      }
    }
  }

  void
  Printer::print(std::ostream& out, const FlatZincModel& m) const {
    if (_output == NULL)
      return;
    for (unsigned int i=0; i< _output->a.size(); i++) {
      AST::Node* ai = _output->a[i];
      if (ai->isArray()) {
        AST::Array* aia = ai->getArray();
        int size = aia->a.size();
        out << "[";
        for (int j=0; j<size; j++) {
          printElem(out,aia->a[j],m);
          if (j<size-1)
            out << ", ";
        }
        out << "]";
      } else {
        printElem(out,ai,m);
      }
    }
  }

  Printer::~Printer(void) {
    delete _output;
  }

}

// STATISTICS: flatzinc-any
