

from casper import *
from casper.cp import *

n = 4

solver = Solver()

#vars = IntVarArray(solver,n,1,n)
#
#solver.post(distinct(vars))
#solver.post(distinct([vars[i]+i for i in range(0,n)]))
#solver.post(distinct([vars[i]-i for i in range(0,n)]))
#
#found = solver.solve(label(solver,vars,selectVarMinDom(solver,vars),selectValsRand(solver,vars)));
#
#while found:
#    print vars
#    found = solver.next()
#    
#print solver.getStats()
#print solver.getCPUTimer()

p = IntPar(solver.getState(),lambda:8)
e = IntExpr(lambda:8)
print e
print "bla"
#d = IntPar(lambda:8)

#print f
#print gg
#found = solver.solve(assign(p,2) | assign(p,lambda:8))

#while found:
#    print p
#    found = solver.next()

#i << lambda:min(j for j in range(1,3)))
#j << 
#solver.post(distinct(aggregate(i,range(1,10)) (vars[i]+i)))



        
             
