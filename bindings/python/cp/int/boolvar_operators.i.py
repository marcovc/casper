
import sys
import os

libPath = os.path.abspath('pyutils')
sys.path.append(libPath)

import objdb

obj = "Casper::CP::Var<bool,Casper::CP::Traits::GetDefaultDom<bool>::Type>"
ev = "bool"

print "%module cp"
print "%import <cp/int/var.i>" 
print "%extend "+obj+"{"

objdb.printVarOperators(obj,ev)

print "}"
