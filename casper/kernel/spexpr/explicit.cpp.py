

import util
		
print "/* THIS FILE WAS AUTOGENERATED FROM explicit_ref.cpp.py */"
print "#include <casper/kernel/spexpr/expr_wrapper.h>"
print "namespace Casper {"
util.printRel(False)
print "namespace Detail {"
util.printRef(False)
util.printGoal(False)
util.printExprWrapper(False)
print "}"
#util.printExpr(False)
print "}"
