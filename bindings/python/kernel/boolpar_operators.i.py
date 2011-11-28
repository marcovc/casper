
import sys
import os

libPath = os.path.abspath('bindings/python/pyutils')
sys.path.append(libPath)

import objdb

obj = "Casper::Par<bool>"
ev = "bool"

print "%module kernel"
print "%import <kernel/par.i>" 
print "%extend "+obj+"{"

objdb.printVarOperators(obj,ev)

print "}"
