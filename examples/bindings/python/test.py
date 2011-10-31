

from casper import *
from casper.cp import *

n = 5

solver = Solver()

vars = IntVarArray(solver,n,1,n)

solver.post(distinct(vars))
solver.post(distinct([vars[i]+i for i in range(0,n)]))
solver.post(distinct([vars[i]-i for i in range(0,n)]))

print "1"
s1 = selectVarMinDom(solver,vars)
print "2"
s2 =selectValMin(solver,vars)
print "3"
g = label(solver,vars,s1,s2)
print "4"
found = solver.solve(g);


while found:
    print vars
    found = solver.next()
    
print solver.getStats()
print solver.getCPUTimer()
