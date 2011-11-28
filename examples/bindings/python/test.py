
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

#idx = IntPar(solver.getState())
#v = IntPar(solver.getState())
#search = whileDo(lambda:not vars.ground(),
#					assign(idx,lambda:min(range(n),key=lambda idx: vars[idx].ground()*n*n+len(vars[idx].domain())*n+abs(idx-n/2))) &
#					assign(v,lambda:random.choice(list(vars[idx.value()].domain()))) &
#					(post(solver,vars[idx]==v) | post(solver,vars[idx]!=v))
#				)

idx = IntPar(solver.getState())
v = IntPar(solver.getState())
search = whileDo(~ground(vars),
					selectMin(idx,range(n),~ground(vars[idx]),domainSize(vars[idx])*n+abs(idx-n/2)) &
					selectRand(v,domain(vars[idx])) &
					(post(solver,vars[idx]==v) | post(solver,vars[idx]!=v))
				)
						
#def search():
#	if vars.ground():
#		return True
#	idx = min(range(n),key=lambda idx: vars[idx].ground()*n*n+len(vars[idx].domain())*n+abs(idx-n/2))
#	x = vars[idx]
#	v = random.choice(list(x.domain()))
#	#v = min(x.domain(),key=lambda v: abs(v-n/2))
#	return  (post(solver,x==v) | post(solver,x!=v)) & search

found = solver.solve(search)
if found:
    print vars
else:
	print "no solution"
	    
print solver.getStats()
print solver.getCPUTimer()

