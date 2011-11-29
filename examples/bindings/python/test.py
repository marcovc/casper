
from casper import *
from casper.cp import *

n = 10
m = 55 
 
solver = Solver()

marks = IntVarArray(solver,n,0,m)

solver.post(distinct([marks[i]-marks[j] for i in range(n) for j in range(i)]))
	
for i in range(n-1):
	solver.post(marks[i] < marks[i+1])

solver.post(marks[0]==0)
solver.post(marks[n-1]==m)

idx = IntPar(solver)
v = IntPar(solver)
search = forAll(idx,range(n),
			tryAll(v,domain(marks[idx]),
				post(solver,marks[idx]==v)))
						
found = solver.solve(search)
if found:
    print marks
else:
	print "no solution"
	    
print solver.getStats()
print solver.getCPUTimer()
