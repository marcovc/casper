

import util

print "/* THIS FILE WAS AUTOGENERATED FROM view.h.py */"
print "#include <casper/kernel/spexpr/expr.h>"
print "namespace Casper {"
util.printRel(True)
print "namespace Detail {"
util.printViews(True)
util.printExprWrapper(True)
util.printPost2(True)
util.printRef(True)
print "}"


print "}"
