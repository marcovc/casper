
import os
import sys

libPath = os.path.abspath('casper/cp/spexpr')
sys.path.append(libPath)

import cputil
import util


print "/* THIS FILE WAS AUTOGENERATED FROM ref.h.py */"
print "#include <casper/kernel/spexpr/expr_wrapper.h>"
print "namespace Casper {"
util.printRel(True)
print "namespace Detail {"
cputil.printExprWrapper(True,"set")
util.printExprWrapper(True)
print "}"
print "}"
