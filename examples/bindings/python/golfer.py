
from casper import *
from casper.cp import *

w = 4
g = 4
s = 4
n = g*s

heuristic = 0

solver = Solver()

vars = IntVarArray3(solver,w,g,s,1,n)

# meet at most once
for wi in range(w):
	for wj in range(wi+1,w):
		for gi in range(g):
			for gj in range(g):
				solver.post(sum([asInt(vars[wi][gi][i]==vars[wj][gj][j]) 
								for i in range(s) for j in range(s)])<=1)
#solver.post(some([vars[wi][gi][i]==vars[wj][gj][j]) 
#				for i in range(s) for j in range(s)])

# all different in each week
for wi in range(w):
	solver.post(distinct(vars[wi]),Domain)

# order positions in groups
for wi in range(w):
	for gi in range(g):
		for pi in range(s-1):
			solver.post(vars[wi][gi][pi] < vars[wi][gi][pi+1])

# order groups in weeks
for wi in range(w):
	for gi in range(g-1):
		solver.post(vars[wi][gi][0] < vars[wi][gi+1][0])

# order weeks
for wi in range(w-1):
	solver.post(vars[wi][0][0] <= vars[wi+1][0][0])

# labeling
if heuristic==0:
	found = solver.solve(label(solver,vars,selectVarMinDom(solver,vars)))
else:
	pi,wi,gi,si = IntRef(solver),IntRef(solver),IntRef(solver),IntRef(solver)
	found = solver.solve(forAll(pi,range(1,n+1)) (
						   forAll(wi,range(w)) (
						   	 tryAll(gi,range(g)) (
							   tryAll(si,range(s)) (
								 post(solver,vars[wi][gi][si] == pi)
							)))))
	
if found:
    print vars
else:
	print "no solution"
	    
print solver.getStats()
print solver.getCPUTimer()

