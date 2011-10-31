
import sys
import os

libPath = os.path.abspath('bindings/python/pyutils')
sys.path.append(libPath)

import objdb

obj = "Casper::Expr<bool>"
ev = "bool"

print "%module kernel"
print "%import <kernel/expr.i>" 
print "%extend "+obj+"{"

objdb.printExprOperators(obj,ev)

print "}"
