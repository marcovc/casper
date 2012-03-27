HEADERS=ast.hh conexpr.hh flatzinc.hh option.hh parser.hh parser.tab.hh registry.hh symboltable.hh varspec.hh
SOURCES=flatzinc.cpp fz.cpp lexer.yy.cpp parser.tab.cpp registry.cpp

CXXFLAGS=-O2 -g

all: fz

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

fz: $(SOURCES:%.cpp=%.o)
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES:%.cpp=%.o)

lexer.yy.cpp: lexer.lxx parser.tab.hh
	flex -olexer.yy.cpp lexer.lxx

parser.tab.hh parser.tab.cpp: parser.yxx
	bison -t -o parser.tab.cpp -d $<
	mv parser.tab.hpp parser.tab.hh

.PHONY: clean distclean
clean:
	rm -f $(SOURCES:%.cpp=%.o)
	rm -f fz

distclean: clean
	rm -f lexer.yy.cpp parser.tab.hh parser.tab.cpp
