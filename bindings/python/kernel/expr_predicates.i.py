
import sys
import os

libPath = os.path.abspath('bindings/python/pyutils')
sys.path.append(libPath)

import objdb

print "%module kernel"
print "%import <kernel/expr.i>" 
print "%inline %{"
print "namespace CasperPreds {"
objdb.printExprPredicates()
print "}"
print "%}"
