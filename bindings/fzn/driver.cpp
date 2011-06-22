/*
 * FlatzincDriver.cpp
 *
 *  Created on: Apr 2, 2010
 *      Author: serge
 */

#include "driver.h"
#include "parser.hpp"
#include <string>
#include <iostream>
#include "print.h"

using namespace std;

FlatzincDriver::FlatzincDriver(const int defaultIntLb, const int defaultIntUb, const float defaultRealLb, const float defaultRealUb):
trace_scanning(false),
trace_parsing(false),
defaultIntLowerBound(defaultIntLb),
defaultIntUpperBound(defaultIntUb),
defaultRealLowerBound(defaultRealLb),
defaultRealUpperBound(defaultRealUb),
argList(),
open(false)
{
}

FlatzincDriver::~FlatzincDriver() {

}

void FlatzincDriver::parse(const std::string &f){
	file = f;
	scan_begin ();
	yy::Flatzinc_parser parser (*this);
	parser.set_debug_level (trace_parsing);
	parser.parse ();
	scan_end ();
}

void FlatzincDriver::error (const yy::location& l, const std::string& m) {
	std::cerr << l << ": " << m << std::endl;
}

void FlatzincDriver::error (const std::string& m) {
	std::cerr << m << std::endl;
}

Instance* FlatzincDriver::newInstance(){
	return new Instance(this->index,this->annotations, this->variables, this->constraints);
}

void FlatzincDriver::addSymbol(SharedSymbol s, const std::string& k){
	this->index.add(s,k);
}

void FlatzincDriver::addAnnotations(SharedSymbol s){
	for (std::list<Annotation>::const_iterator it = annArgList.begin();
			it != annArgList.end(); ++it)
		this->annotations.add(s,*it);
}

void FlatzincDriver::addInstanceAnnotations(){

/*	for (std::list<Annotation>::const_iterator it = annArgList.begin();
			it != annArgList.end(); ++it)
		this->instanceAnnotations.add(*it);*/
}

void FlatzincDriver::addVariable(SharedSymbol s){
	this->variables.add(s);
}

void FlatzincDriver::addConstraint(SharedSymbol s){
	this->constraints.add(s);
}

bool FlatzincDriver::isAnnotation(const std::string& k) {
	return not this->index.hasSymbol(k);
}

const SharedSymbol& FlatzincDriver::getSymbol(const std::string& k){
	try{
		return this->index.getSymbol(k);
	}catch(ENoSymbolForKey e){
		std::cout << "Nothing for " + k + "." << std::endl;
	}
	return *new SharedSymbol(new int(0));
}

IntRange* FlatzincDriver::getDefaultIntRange(){
	return new IntRange(defaultIntLowerBound,defaultIntUpperBound);
}

RealRange* FlatzincDriver::getDefaultRealRange(){
	return new RealRange(defaultRealLowerBound,defaultRealUpperBound);
}

void FlatzincDriver::beginArgList(){
	this->argList.clear();
}

template<class T>
struct ArgListAsTypedArray
{
	typedef casperbind::cpp::Array<T> Ret;
	const Symbol* operator()(const FlatzincDriver& d) const
	{
		int sizes[1] = {d.argList.size()};
		Ret* r = new Ret(1,sizes);
		int i = 0;
		for(list<SharedSymbol>::const_iterator it = d.argList.begin();
				it != d.argList.end(); it++,i++){
			(*r)[i] = static_cast<const T&>(*it);
		}
		return r;
	}
};

template<template<class T> class Helper,class Arg>
const Symbol* dispatchScalarType(const Symbol::Type& t,const Arg& arg)
{
	switch (t)
	{
		case Symbol::sBool:
			return Helper<bool>()(arg);
		case Symbol::sInt:
			return Helper<int>()(arg);
		case Symbol::sDouble:
			return Helper<double>()(arg);
		default:
			return Helper<SharedSymbol>()(arg);
	}
}

const Symbol* FlatzincDriver::argListAsTypedArray() const
{
	SharedSymbol firstElem = *this->argList.begin();
	return dispatchScalarType<ArgListAsTypedArray,FlatzincDriver>(firstElem.getType(true),
																		*this);
}

SymbolArray* FlatzincDriver::argListAsArray() const
{
	int sizes[1] = {argList.size()};
	SymbolArray* r = new SymbolArray(1,sizes);
	int i = 0;
	for(list<SharedSymbol>::const_iterator it = argList.begin();
			it != argList.end(); it++,i++){
		(*r)[i] = *it;
	}
	return r;
}

SymbolSet* FlatzincDriver::argListAsSet(){
	SymbolSet *s = new SymbolSet();
	list<SharedSymbol>::iterator it;
	for(it=this->argList.begin(); it != argList.end(); it++){
		s->add(*it);
	}
	return s;
}

SymbolArray* FlatzincDriver::variableSymbolArray(const Symbol *domain, const int size){
	int sizes[1] = {size+1};
	SharedSymbol s[size+1];

	for(int i = 0; i <= size; ++i){
		Variable v(domain);
		s[i] = SharedSymbol(v);
	}

	return new SymbolArray(1,sizes,s);
}

void FlatzincDriver::addArg(SharedSymbol s){
	this->argList.push_front(s);
}

SymbolSet* FlatzincDriver::rangeAsSet(SymbolRange *i){
	int lower = i->lower;
	int upper = i->upper;

	for(; lower < upper; ++lower){

	}
	return NULL;
}

void FlatzincDriver::addAnnArg(const Annotation &s){
	this->annArgList.push_front(s);
}

void FlatzincDriver::beginAnnList(){
	this->annArgList.clear();
}

bool FlatzincDriver::noArgs(){
	return this->argList.empty();
}
