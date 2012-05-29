
/*
 *  Adapted for Casper from skeleton code by Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *  	Guido Tack, 2007
 *  	Marco Correia, 2012
 */

#define CASPER_LOG_NRANGES_DEPTH

#ifndef CASPER_FLATZINC_HH__
#define CASPER_FLATZINC_HH__

#include <iostream>
#include <map>
#include <cassert>

#include <thirdparty/flatzinc_skeleton/conexpr.hh>
#include <thirdparty/flatzinc_skeleton/ast.hh>
#include <thirdparty/flatzinc_skeleton/varspec.hh>

#include <casper/kernel.h>
#include <casper/cp/int.h>
#include <casper/cp/set.h>

#include <bindings/fzn/options.h>

#define CASPER_FLATZINC_VERSION "1.5"

/**
 * \namespace FlatZinc
 * \brief Interpreter for the %FlatZinc language
 *
 * The FlatZinc namespace contains all functionality required
 * to parse and solve constraint models written in the %FlatZinc language.
 *
 */

namespace FlatZinc {

typedef Casper::CP::Solver	Solver;
typedef Casper::CP::IntVar IntVar;
typedef Casper::CP::BoolVar BoolVar;
typedef Casper::CP::IntSetVar SetVar;

typedef Casper::CP::IntVarArray IntVarArray;
typedef Casper::CP::BoolVarArray BoolVarArray;
typedef Casper::CP::IntSetVarArray SetVarArray;

  class Printer;

  /**
   * \brief A space that can be initialized with a %FlatZinc model
   *
   */
  class FlatZincModel {
  public:
    enum Meth {
      SAT, //< Solve as satisfaction problem
      MIN, //< Solve as minimization problem
      MAX  //< Solve as maximization problem
    };
  protected:
    /// Number of integer variables
    int intVarCount;
    /// Number of Boolean variables
    int boolVarCount;
    /// Number of set variables
    int setVarCount;

    /// Index of the integer variable to optimize
    int _optVar;
  
    /// Whether to solve as satisfaction or optimization problem
    Meth _method;
    
    /// Annotations on the solve item
    AST::Array* _solveAnnotations;

    /// Run SAT search
    void runSAT(std::ostream& out, const Printer& p, const FlatZinc::Options& opt);

    /// Show statistics
    void printStats(std::ostream& out);

  public:
    /// The Solver object
    Solver solver;
    /// The Solver current status
    bool valid;
    /// The search tree
    Casper::Goal search;

    /// The integer variables
    IntVarArray* piv;
    /// Indicates whether an integer variable is introduced by mzn2fzn
    std::vector<bool> iv_introduced;
    /// Indicates whether an integer variable aliases a Boolean variable
    std::vector<int> iv_boolalias;
    /// The Boolean variables
    BoolVarArray* pbv;
    /// Indicates whether a Boolean variable is introduced by mzn2fzn
    std::vector<bool> bv_introduced;
    /// The set variables
    SetVarArray* psv;
    /// Indicates whether a set variable is introduced by mzn2fzn
    std::vector<bool> sv_introduced;

    /// Construct empty space
    FlatZincModel(void);
  
    /// Destructor
    ~FlatZincModel(void);
  
    /// Initialize space with given number of variables
    void init(int intVars, int boolVars, int setVars);

    /// Create new integer variable from specification
    void newIntVar(IntVarSpec* vs);
    /// Link integer variable \a iv to Boolean variable \a bv
    void aliasBool2Int(int iv, int bv);
    /// Return linked Boolean variable for integer variable \a iv
    int aliasBool2Int(int iv);
    /// Create new Boolean variable from specification
    void newBoolVar(BoolVarSpec* vs);
    /// Create new set variable from specification
    void newSetVar(SetVarSpec* vs);
  
    /// Post a constraint specified by \a ce
    void postConstraint(const ConExpr& ce, AST::Node* annotation);
  
    /// Post the solve item
    void solve(AST::Array* annotation);
    /// Post that integer variable \a var should be minimized
    void minimize(int var, AST::Array* annotation);
    /// Post that integer variable \a var should be maximized
    void maximize(int var, AST::Array* annotation);

    /// Run the search
    void run(std::ostream& out, const Printer& p, const FlatZinc::Options& opt);
  
    /// Produce output on \a out using \a p
    void print(std::ostream& out, const Printer& p) const;

    /// Return whether to solve a satisfaction or optimization problem
    Meth method(void) const;

    /// Return index of variable used for optimization
    int optVar(void) const;

    /**
     * \brief Create branchers corresponding to the solve item annotations
     *
     * If \a ignoreUnknown is true, unknown solve item annotations will be
     * ignored, otherwise a warning is written to \a err.
     */
    void createBranchers(AST::Node* ann, bool ignoreUnknown,
                         std::ostream& err = std::cerr);

    /// Return the solve item annotations
    AST::Array* solveAnnotations(void) const;

  };

  /**
   * \brief Output support class for %FlatZinc interpreter
   *
   */
  class Printer {
  private:
    AST::Array* _output;
    void printElem(std::ostream& out,
                   AST::Node* ai, const FlatZincModel& m) const;
  public:
    Printer(void) : _output(NULL) {}
    void init(AST::Array* output);

    void print(std::ostream& out, const FlatZincModel& m) const;
  
    ~Printer(void);
    
  private:
    Printer(const Printer&);
    Printer& operator=(const Printer&);
  };

  /// %Exception class for %FlatZinc errors
  class Error {
  private:
    const std::string msg;
  public:
    Error(const std::string& where, const std::string& what)
    : msg(where+": "+what) {}
    const std::string& toString(void) const { return msg; }
  };

  /**
   * \brief Parse FlatZinc file \a fileName into \a fzs and return it.
   *
   * Creates a new empty FlatZincModel if \a fzs is NULL.
   */
  FlatZincModel* parse(const std::string& fileName,
                       Printer& p, std::ostream& err = std::cerr,
                       FlatZincModel* fzs=NULL);

  /**
   * \brief Parse FlatZinc from \a is into \a fzs and return it.
   *
   * Creates a new empty FlatZincModel if \a fzs is NULL.
   */
  FlatZincModel* parse(std::istream& is,
                       Printer& p, std::ostream& err = std::cerr,
                       FlatZincModel* fzs=NULL);

}

#endif

