/*
 * FlatzincDriver.h
 *
 *  Created on: Apr 2, 2010
 *      Author: serge
 */

#ifndef _FLATZINC_DRIVER_H_
#define _FLATZINC_DRIVER_H_

#include <cpp.h>
#include <parser.hpp>
#include <list>

using namespace Casperbind::cpp;

class FlatzincDriver;
// Announce to Flex the prototype we want for lexing function, ...
# define YY_DECL \
  yy::Flatzinc_parser::token_type					 \
  yylex (yy::Flatzinc_parser::semantic_type* yylval,	\
		 yy::Flatzinc_parser::location_type* yylloc,	\
		 FlatzincDriver& driver)

// ... and declare it for the parser's sake.
YY_DECL;

class FlatzincDriver {
public:
	/*
	 * Specification of the upper and lower bound
	 * values of unconstrained ints and sets
	 * */
	FlatzincDriver(const int, const int, const float, const float);
	virtual ~FlatzincDriver();

	bool trace_scanning;
	bool trace_parsing;
	std::string file;


	// Handling the scanner
	void scan_begin();
	void scan_end();

	// Handling the parser
	void parse(const std::string& f);

	// Error handling

	void error(const yy::location& l, const std::string& m);
	void error(const std::string& m);

	void addSymbol(SharedSymbol s, const std::string& k);
	void addAnnotations(SharedSymbol s);
	void addInstanceAnnotations();
	void addVariable(SharedSymbol s);
	void addConstraint(SharedSymbol s);
	bool isAnnotation(const std::string& k);
	const std::string& getKey(const Symbol& s) const;
	const SharedSymbol& getSymbol(const std::string& s);
	Instance* newInstance();
	IntRange* getDefaultIntRange();
	RealRange* getDefaultRealRange();
	SymbolArray* variableSymbolArray(const Symbol*, const int);
	SymbolSet* rangeAsSet(SymbolRange*);

	void beginArgList();
	void endArgList();
	const Symbol* argListAsTypedArray() const;
	SymbolArray* argListAsArray() const;
	SymbolSet* argListAsSet();
	void addArg(SharedSymbol s);


	void beginAnnList();
	void addAnnArg(const Annotation &s);
	bool noArgs();

	Index index;
	Annotations annotations;
	Annotations instanceAnnotations;
	SymbolSet variables;
	SymbolSet constraints;
	int defaultIntLowerBound;
	int defaultIntUpperBound;
	double defaultRealLowerBound;
	double defaultRealUpperBound;
	std::list<SharedSymbol> argList;
	std::list<Annotation> annArgList;
	bool open;
	
};
#endif /* _FLATZINC_DRIVER_H_ */

