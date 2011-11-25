
import sys
from casper import *
from casper.cp import *
import random

n = 100

solver = Solver()

vars = IntVarArray(solver,n,1,n)

solver.post(distinct(vars))
solver.post(distinct([vars[i]+i for i in range(0,n)]))
solver.post(distinct([vars[i]-i for i in range(0,n)]))

#x = IntVarRef(solver)
#v = IntRef(solver)
#search = whiledo(lambda:not vars.ground()) (
#					assign(x,min(vars,key=lambda x: len(x.domain()))) &
#					assignMin(x,vars,domainSize(x)*ground(x)))	
#					assign(v,lambda:min(x)) &
#					(post(solver,x==v) | post(solver,x!=v)) 
#				)


def search():
	builtin = __builtins__ 
	if vars.ground():
		return True

	idx = mmin(range(n),key=lambda idx: vars[idx].ground()*n*n+len(vars[idx].domain())*n+
								builtin.abs(idx-n/2))
	x = vars[idx]
#	x = __builtins__.min(nonground,key=lambda v: len(v.domain()))
	v = random.choice(list(x.domain()))
	return  (post(solver,x==v) | post(solver,x!=v)) & search

found = solver.solve(search)
if found:
    print vars
else:
	print "no solution"
	    
print solver.getStats()
print solver.getCPUTimer()

#search = whiledo(lambda:not vars.ground()) (
#					assign(x,lambda:min(vars,key=domainSize)) &	
#					assign(v,lambda:min(x)) &
#					(post(solver,x==v) | post(solver,x!=v))) 

#
#p = IntPar(solver.getState(),lambda:8)
#print p
#e = IntExpr(lambda:8)
#print e
        
             
