/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2009 - Marco Correia <marco.v.correia@gmail.com>           *
 *                                                                         *
 *   Licensed under the Apache License, Version 2.0 (the "License");       *
 *   you may not use this file except in compliance with the License.      *
 *   You may obtain a copy of the License at                               *
 *            http://www.apache.org/licenses/LICENSE-2.0                   *
 *   Unless required by applicable law or agreed to in writing, software   *
 *   distributed under the License is distributed on an                    *
 *   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,          *
 *   either express or implied.                                            *
 *   See the License for the specific language governing permissions and   *
 *   limitations under the License.                                        *
 \*************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <libxml++/libxml++.h>

#include <bindings/cpp/cpp.h>
#include <bindings/cpp/print.h> // tmp
#include <list>

#include <boost/algorithm/string.hpp>

#include <boost/xpressive/xpressive.hpp> // must be here
#include "exprparser.h"

// Parameterized predicates
struct ParPredicate
{
	std::string pars;
	std::string expr;
	ParPredicate(const std::string& pars, const std::string& expr) :
		pars(pars),expr(expr) {}
	casperbind::cpp::SharedSymbol generateConstraint(const casperbind::cpp::SymbolArray& pars);
};


casperbind::cpp::SharedSymbol ParPredicate::generateConstraint(const casperbind::cpp::SymbolArray& sympars)
{
	std::map<std::string,casperbind::cpp::SharedSymbol> mpars;
	// parse pars str to vector
	std::vector<std::string> strpars;
	boost::split(strpars, pars, boost::is_space());
	// create map from par name to current symbol instantiation
	if (sympars.getSize() != (int)strpars.size()/2)
		throw xmlpp::validity_error(std::string("predicate defined/called with diferent number of parameters: ")+expr);
	for (int i = 0; i < (int)strpars.size()/2; ++i)
		mpars.insert(make_pair(strpars[i*2+1],sympars[i]));
	// parse expression
	ExprParser e;
	casperbind::cpp::SharedSymbol s = e.parse(expr,mpars);
	if (s.getType(true) != casperbind::cpp::Symbol::sPredicate)
		throw xmlpp::parse_error(std::string("parsing expression: ")+expr);
	return s;
}

class XCSPParser : public xmlpp::SaxParser
{
public:
  XCSPParser();
  virtual ~XCSPParser();

  void presentationBegin(const AttributeList& atts);

  void domainBegin(const AttributeList& attributes);
  void domainContents(const Glib::ustring& text);

  void variableBegin(const AttributeList& attributes);

  void relationBegin(const AttributeList& attributes);
  void relationContents(const Glib::ustring& text);

  void predicateBegin(const AttributeList& attributes);
  void predicateEnd();
  void expressionParsContents(const Glib::ustring& text);
  void expressionContents(const Glib::ustring& text);

  void constraintBegin(const AttributeList& attributes);
  void constraintParsContents(const Glib::ustring& text);
  void constraintEnd();

  casperbind::cpp::Instance getInstance() const;

protected:
  //overrides:
  virtual void on_start_document();
  virtual void on_end_document();
  virtual void on_start_element(const Glib::ustring& name,
                                const AttributeList& properties);
  virtual void on_end_element(const Glib::ustring& name);
  virtual void on_characters(const Glib::ustring& characters);
  virtual void on_characters_buffered();
  virtual void on_comment(const Glib::ustring& text);
  virtual void on_warning(const Glib::ustring& text);
  virtual void on_error(const Glib::ustring& text);
  virtual void on_fatal_error(const Glib::ustring& text);

  void updatePathBegin(const Glib::ustring& text) { path.push_front(text);	}
  void updatePathEnd(const Glib::ustring& text);
  void assertParent(const Glib::ustring& text);
  const Glib::ustring& parent() const;
  const Glib::ustring& grandParent() const;

  casperbind::cpp::IntSet parseIntSet(const Glib::ustring& text);
  casperbind::cpp::IntRange parseIntRange(const Glib::ustring& text);
  casperbind::cpp::SharedSymbol parseIntDomain(const Glib::ustring& text);
  casperbind::cpp::IntArray parseIntTupleList(const Glib::ustring& text,
												int arity, int size);
  casperbind::cpp::SymbolArray parseParameters(const std::string& s) const;
  casperbind::cpp::SymbolArray parseScope(const std::string& s) const;

  std::list<Glib::ustring> path;

  casperbind::cpp::Index index;
  std::list<casperbind::cpp::SharedSymbol> variables;
  std::map<std::string,casperbind::cpp::SharedSymbol> posTables;
  std::map<std::string,casperbind::cpp::SharedSymbol> negTables;

  Glib::ustring curCharactersBuffer;

  std::string curDomainKey;

  std::string curRelationKey;
  int curRelationArity;
  int curRelationNbTuples;

  std::string curPredicateKey;
  std::string curExpressionPars;
  std::string curExpression;

  std::map<std::string,ParPredicate*> predicates;

  std::string curConstraintKey;
  std::string curConstraintScope;
  std::string curConstraintRef;
  std::string curConstraintPars;

  std::list<casperbind::cpp::SharedSymbol> constraints;

  enum { pos, neg } curRelationSemantics;
};

const Glib::ustring& XCSPParser::parent() const
{	return *path.begin();	}

const Glib::ustring& XCSPParser::grandParent() const
{	return *++path.begin();	}

void XCSPParser::updatePathEnd(const Glib::ustring& text)
{
	assertParent(text);
	path.pop_front();
}

void XCSPParser::assertParent(const Glib::ustring& text)
{
	if (parent() != text)
		throw xmlpp::parse_error(std::string("parsing element: ")+text);
}

struct StrToInt
{
	int operator()(const std::string& s) const
	{	return atoi(s.c_str());	}
};


casperbind::cpp::IntSet XCSPParser::parseIntSet(const Glib::ustring& text)
{
	std::vector<std::string> tokens;
	boost::split(tokens, text.raw(), boost::is_space());
	casperbind::cpp::IntSet r;
	std::transform(tokens.begin(),tokens.end(),
			casperbind::cpp::Detail::inserter(r),StrToInt());
	return r;
}

casperbind::cpp::IntRange XCSPParser::parseIntRange(const Glib::ustring& text)
{
	unsigned int pos = text.find("..");
	if (pos >= text.size())
		throw xmlpp::parse_error(std::string("parsing integer range: ")+text);
	int lb = atoi(text.substr(0,pos).c_str());
	int ub = atoi(text.substr(pos+2,text.size()).c_str());
	return casperbind::cpp::IntRange(lb,ub);
}

casperbind::cpp::SharedSymbol XCSPParser::parseIntDomain(const Glib::ustring& text)
{
	std::vector<Glib::ustring> tokens;
	boost::split(tokens, text.raw(), boost::is_any_of("\t \r\n"));

	// if we have only one range, store it and leave
	if (tokens.size()==1 and tokens[0].find("..") < tokens[0].size())
		return parseIntRange(text);

	casperbind::cpp::IntSet s;

	// else store list of values since there is no mixed sets (ranges+int) in casperbind
	for (std::vector<Glib::ustring>::iterator it = tokens.begin();
			it != tokens.end(); ++it)
		if (it->find("..") < tokens[0].size()) // is a range
		{
			casperbind::cpp::IntRange r(parseIntRange(*it));
			for (int i = r.getLower(); i <= r.getUpper(); ++i)
				s.add(i);
		}
		else	// is a set
		{
			casperbind::cpp::IntSet r(parseIntSet(*it));
			for (casperbind::cpp::IntSet::ConstIterator it2 = r.begin();
					it2 != r.end(); ++it2)
				s.add(*it2);
		}

	return s;
}

casperbind::cpp::IntArray XCSPParser::parseIntTupleList(const Glib::ustring& text,
				int arity, int size)
{
	int dims[2] = { size, arity };
	casperbind::cpp::IntArray r(2,dims);
	int c = 0;

	std::vector<std::string> tuples;
	boost::split(tuples, text.raw(), boost::is_any_of("|"));

	for (std::vector<std::string>::iterator it = tuples.begin();
			it != tuples.end(); ++it)
	{
		std::vector<std::string> elements;
		boost::split(elements, *it, boost::is_any_of(" \t\r\n"));
		std::transform(elements.begin(),elements.end(),&r[c],StrToInt());
		c += elements.size();
	}

	return r;
}

casperbind::cpp::SymbolArray XCSPParser::parseParameters(const std::string& s) const
{
	std::list<casperbind::cpp::SharedSymbol> l;
	std::vector<std::string> pars;
	boost::split(pars, s, boost::is_any_of("\t\r\n "));
	for (std::vector<std::string>::const_iterator it = pars.begin();
			it != pars.end(); ++it)
		if (index.hasKey(*it)) // parameter is a variable
			l.push_back(index.getSymbol(*it));
		else // parameter is an integral constant
			l.push_back(casperbind::cpp::int(atoi(it->c_str())));
	casperbind::cpp::SymbolArray r(l.size());
	std::copy(l.begin(),l.end(),r.getData());
	return r;
}

casperbind::cpp::SymbolArray XCSPParser::parseScope(const std::string& s) const
{
	std::list<casperbind::cpp::SharedSymbol> l;
	std::vector<std::string> pars;
	boost::split(pars, s, boost::is_any_of("\t\r\n "));
	for (std::vector<std::string>::const_iterator it = pars.begin();
			it != pars.end(); ++it)
		if (index.hasKey(*it)) // parameter is a variable
			l.push_back(index.getSymbol(*it));
		else // something is wrong
			throw xmlpp::parse_error(std::string("undeclared variable in constraint scope: ")+s);
	casperbind::cpp::SymbolArray r(l.size());
	std::copy(l.begin(),l.end(),r.getData());
	return r;
}

void XCSPParser::presentationBegin(const AttributeList& attributes)
{
	// test for valid XCSP format versions
	const int n = 2;
	Glib::ustring compatibleVersions[n] = { "XCSP 2.0","XCSP 2.1" };

	Glib::ustring name = "format";
	AttributeList::const_iterator versionIt = std::find_if(attributes.begin(),
										attributes.end(), AttributeHasName(name));

	if (versionIt == attributes.end() or
			std::find_if(compatibleVersions,
						 &compatibleVersions[n],
						 std::bind1st(std::equal_to<Glib::ustring>(),versionIt->value))==
							 &compatibleVersions[n])
	{
		std::ostringstream s;
		std::copy(compatibleVersions,&compatibleVersions[n],
					std::ostream_iterator<Glib::ustring>(s,", "));
		throw xmlpp::validity_error("incompatible XCSP version. Supported versions: "+s.str());
	}
}

void XCSPParser::domainBegin(const AttributeList& attributes)
{
	AttributeList::const_iterator nameIt = std::find_if(attributes.begin(),
										attributes.end(), AttributeHasName("name"));
	if (nameIt == attributes.end())
		throw xmlpp::validity_error("unnamed domain found");

	curDomainKey = nameIt->value;
}

void XCSPParser::variableBegin(const AttributeList& attributes)
{
	AttributeList::const_iterator nameIt = std::find_if(attributes.begin(),
										attributes.end(), AttributeHasName("name"));
	std::string curVariableKey = nameIt->value;
	if (nameIt == attributes.end())
		throw xmlpp::validity_error("unnamed variable");

	AttributeList::const_iterator domIt = std::find_if(attributes.begin(),
										attributes.end(), AttributeHasName("domain"));
	if (domIt == attributes.end())
		throw xmlpp::validity_error("variable with unspecified domain");

	const casperbind::cpp::SharedSymbol& dom  = index.getSymbol(domIt->value);

	if (dom.getType()!=casperbind::cpp::Symbol::sSymbol)
		throw xmlpp::validity_error("assertion failure in XCSPParser::variableBegin");

	casperbind::cpp::SharedSymbol var = casperbind::cpp::Variable(dom);
	variables.push_back(var);
	index.add(var,curVariableKey);
}

void XCSPParser::relationBegin(const AttributeList& attributes)
{
	AttributeList::const_iterator nameIt = std::find_if(attributes.begin(),
										attributes.end(), AttributeHasName("name"));
	if (nameIt == attributes.end())
		throw xmlpp::validity_error("unnamed relation");

	curRelationKey = nameIt->value;

	AttributeList::const_iterator arityIt = std::find_if(attributes.begin(),
										attributes.end(), AttributeHasName("arity"));
	if (arityIt == attributes.end())
		throw xmlpp::validity_error("relation with unspecified arity");

	curRelationArity = atoi(arityIt->value.c_str());

	AttributeList::const_iterator nbIt = std::find_if(attributes.begin(),
										attributes.end(), AttributeHasName("nbTuples"));
	if (nbIt == attributes.end())
		throw xmlpp::validity_error("relation with unspecified nbTuples");

	curRelationNbTuples = atoi(nbIt->value.c_str());

	AttributeList::const_iterator semIt = std::find_if(attributes.begin(),
										attributes.end(), AttributeHasName("semantics"));
	if (semIt == attributes.end())
		throw xmlpp::validity_error("relation with unspecified semantics");

	if (semIt->value == "supports")
		curRelationSemantics = pos;
	else
		curRelationSemantics = neg;
}


void XCSPParser::predicateBegin(const AttributeList& attributes)
{
	AttributeList::const_iterator nameIt = std::find_if(attributes.begin(),
										attributes.end(), AttributeHasName("name"));
	if (nameIt == attributes.end())
		throw xmlpp::validity_error("unnamed predicate found");

	curPredicateKey = nameIt->value;
}

void XCSPParser::domainContents(const Glib::ustring& text)
{
	index.add(parseIntDomain(text),curDomainKey);
}

void XCSPParser::relationContents(const Glib::ustring& text)
{
	casperbind::cpp::IntArray a = parseIntTupleList(text,curRelationArity,
														 curRelationNbTuples);
	index.add(a,curRelationKey);
	if (curRelationSemantics == pos)
		posTables.insert(make_pair(curRelationKey,index.getSymbol(curRelationKey)));
	else
		negTables.insert(make_pair(curRelationKey,index.getSymbol(curRelationKey)));
}

void XCSPParser::expressionParsContents(const Glib::ustring& text)
{
	curExpressionPars = text;
}

void XCSPParser::expressionContents(const Glib::ustring& text)
{
	curExpression = text;
}

void XCSPParser::predicateEnd()
{
	if (predicates.find(curPredicateKey) != predicates.end())
		throw xmlpp::validity_error("multiple definitions of predicate with same name");
	predicates.insert(std::make_pair(curPredicateKey,new ParPredicate(curExpressionPars,curExpression)));
}

void XCSPParser::constraintBegin(const AttributeList& attributes)
{
	AttributeList::const_iterator nameIt = std::find_if(attributes.begin(),
										attributes.end(), AttributeHasName("name"));
	if (nameIt == attributes.end())
		throw xmlpp::validity_error("unnamed constraint");

	curConstraintKey = nameIt->value;

	AttributeList::const_iterator scopeIt = std::find_if(attributes.begin(),
										attributes.end(), AttributeHasName("scope"));
	if (scopeIt == attributes.end())
		throw xmlpp::validity_error("constraint with unspecified scope");

	curConstraintScope = scopeIt->value;

	AttributeList::const_iterator refIt = std::find_if(attributes.begin(),
										attributes.end(), AttributeHasName("reference"));
	if (refIt == attributes.end())
		throw xmlpp::validity_error("constraint with unspecified reference");

	curConstraintRef = refIt->value;

}

void XCSPParser::constraintParsContents(const Glib::ustring& text)
{
	curConstraintPars = text;
}

void XCSPParser::constraintEnd()
{
	// if cur constraint is a predicate
	if (predicates.find(curConstraintRef)!=predicates.end())
	{
		casperbind::cpp::SymbolArray pars = parseParameters(curConstraintPars);
		index.add(predicates[curConstraintRef]->generateConstraint(pars),curConstraintKey);
		constraints.push_back(index.getSymbol(curConstraintKey));
	}
	else // if cur constraint is a positive table
	if (posTables.find(curConstraintRef) != posTables.end())
	{
		casperbind::cpp::SymbolArray vars = parseScope(curConstraintScope);
		casperbind::cpp::SymbolArray pars(2);
		pars[0] = vars;
		pars[1] = posTables[curConstraintRef];
		casperbind::cpp::SharedSymbol s =
				casperbind::cpp::Predicate(casperbind::cpp::Predicate::pInTable,pars);
		index.add(s,curConstraintKey);
		constraints.push_back(s);
	}
	else
	if (negTables.find(curConstraintRef) != negTables.end())
	{
		casperbind::cpp::SymbolArray vars = parseScope(curConstraintScope);
		casperbind::cpp::SymbolArray pars(2);
		pars[0] = vars;
		pars[1] = negTables[curConstraintRef];
		casperbind::cpp::SharedSymbol s =
				casperbind::cpp::Predicate(casperbind::cpp::Predicate::pNotInTable,pars);
		index.add(s,curConstraintKey);
		constraints.push_back(s);
	}
}

XCSPParser::XCSPParser()
  : xmlpp::SaxParser()
{
}

XCSPParser::~XCSPParser()
{
}

void XCSPParser::on_start_document()
{
  std::cout << "on_start_document()" << std::endl;
}

void XCSPParser::on_end_document()
{
  std::cout << "on_end_document()" << std::endl;
}

void XCSPParser::on_start_element(const Glib::ustring& name,
                                   const AttributeList& attributes)
{
	if (name == "presentation")
	{
		assertParent("instance");
		presentationBegin(attributes);
	}
	else
	if (name == "domains")
		assertParent("instance");
	else
	if (name == "domain")
	{
		assertParent("domains");
		domainBegin(attributes);
	}
	else
	if (name == "variables")
		assertParent("instance");
	else
	if (name == "variable")
	{
		assertParent("variables");
		variableBegin(attributes);
	}
	else
	if (name == "relations")
		assertParent("instance");
	else
	if (name == "relation")
	{
		assertParent("relations");
		relationBegin(attributes);
	}
	else
	if (name == "predicates")
		assertParent("instance");
	else
	if (name == "predicate")
	{
		assertParent("predicates");
		predicateBegin(attributes);
	}
	else
	if (name == "constraint")
	{
		assertParent("constraints");
		constraintBegin(attributes);
	}

	updatePathBegin(name);

	curCharactersBuffer = "";
}

void XCSPParser::on_end_element(const Glib::ustring& name)
{
	if (curCharactersBuffer.size()>0)
	{
		on_characters_buffered();
		curCharactersBuffer = "";
	}

	if (parent()=="predicate")
		predicateEnd();
	else
	if (parent()=="constraint")
		constraintEnd();

	updatePathEnd(name);

//  std::cout << "on_end_element()" << std::endl;
}

void XCSPParser::on_characters(const Glib::ustring& text)
{
	curCharactersBuffer += text;
}

void XCSPParser::on_characters_buffered()
{
	if (parent()=="domain")
		domainContents(curCharactersBuffer);
	else
	if (parent()=="relation")
		relationContents(curCharactersBuffer);
	else
	if (parent()=="parameters" and grandParent()=="predicate")
		expressionParsContents(curCharactersBuffer);
	else
	if (parent()=="functional" and grandParent()=="expression")
		expressionContents(curCharactersBuffer);
	else
	if (parent()=="parameters" and grandParent()=="constraint")
		constraintParsContents(curCharactersBuffer);

 // std::cout << "on_characters(): " << text << std::endl;
}

void XCSPParser::on_comment(const Glib::ustring& text)
{
  std::cout << "on_comment(): " << text << std::endl;
}

void XCSPParser::on_warning(const Glib::ustring& text)
{
  std::cout << "on_warning(): " << text << std::endl;
}

void XCSPParser::on_error(const Glib::ustring& text)
{
  std::cout << "on_error(): " << text << std::endl;
}

void XCSPParser::on_fatal_error(const Glib::ustring& text)
{
  std::cout << "on_fatal_error(): " << text << std::endl;
}

casperbind::cpp::Instance XCSPParser::getInstance() const
{
	casperbind::cpp::SymbolArray vars(variables.size());
	int c = 0;
	// FIXME this should be only labeling variables (the remaining are refd indirectly)
	for (std::list<casperbind::cpp::SharedSymbol>::const_iterator it = variables.begin();
			it != variables.end(); ++it)
		vars[c++] = *it;
	casperbind::cpp::SymbolArray cons(constraints.size());
	c = 0;
    for (std::list<casperbind::cpp::SharedSymbol>::const_iterator it = constraints.begin();
			it != constraints.end(); ++it)
    	cons[c++] = *it;
    return casperbind::cpp::Instance(index,vars,cons);
}

int
main(int argc, char* argv[])
{
  std::string filepath;
  if(argc > 1 )
    filepath = argv[1]; //Allow the user to specify a different XML file to parse.
  else
  {
	  std::cerr << "usage: " << argv[0] << " xcspfile" << std::endl;
	  return 1;
  }
  // Parse the entire document in one go:
  try
  {
    XCSPParser parser;
    parser.set_substitute_entities(true); //
    parser.parse_file(filepath);
    SymStream scout(std::cout,parser.getInstance().getIndex());
    scout << parser.getInstance() << std::endl;

  }
  catch(const xmlpp::exception& ex)
  {
    std::cout << "exception: " << ex.what() << std::endl;
  }


  return 0;
}

