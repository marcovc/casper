

from casper import *
from casper.cp import *

n = 4

solver = Solver()

vars = IntVarArray(solver,n,1,n)

solver.post(distinct(vars))
solver.post(distinct([vars[i]+i for i in range(0,n)]))
solver.post(distinct([vars[i]-i for i in range(0,n)]))

x = IntVar(solver)
v = IntPar(solver)

search = whiledo(lambda:not vars.ground()) (
					assign(x,lambda:min(vars,key=domainSize)) &	
					assign(v,lambda:min(x)) &
					(post(solver,x==v) | post(solver,x!=v)) 
				)

found = solver.solve(search);

if found:
    print vars
else:
	print "no solution"
	    
print solver.getStats()
print solver.getCPUTimer()

#found = solver.solve(assign(p,2) | assign(p,lambda:8))
#
#while found:
#    print p
#    found = solver.next()
#
#
#p = IntPar(solver.getState(),lambda:8)
#e = IntExpr(lambda:8)
#print e
#print "bla"
        
             
