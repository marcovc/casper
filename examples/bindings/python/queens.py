
from casper import *
from casper.cp import *
#from casper.util import *

n = 4

solver = Solver()
vars = IntVarArray(solver,n,1,n)

solver.post(distinct(vars))
solver.post(distinct([vars[i]+i for i in range(n)]))
solver.post(distinct([vars[i]-i for i in range(n)]))
		
idx = IntRef(solver)
v = IntRef(solver)


search = whileDo(~ground(vars)) (
					selectMin(idx,range(n),cond=~ground(vars[idx])) ([domainSize(vars[idx]),abs(idx-n/2)]) &
					selectRand(v,domain(vars[idx])) &
					(post(solver,vars[idx]==v) | post(solver,vars[idx]!=v))
				)

'''
def search():
	import random
	if vars.ground():
		return True
	idx = min(range(n),key=lambda idx: vars[idx].ground()*n*n+len(vars[idx].domain())*n+abs(idx-n/2))
	x = vars[idx]
	v = random.choice(list(x.domain()))
	#v = min(x.domain(),key=lambda v: abs(v-n/2))
	return  (post(solver,x==v) | post(solver,x!=v)) & search
'''

#search = \
#whileDo(~vars.ground()) (
#	selectMin(idx,range(n),cond=~vars[idx].ground()) 
#				([vars[idx].domain().size(),abs(idx-n/2)]) &
#	selectRand(v,vars[idx].domain()) &
#	(solver.post(vars[idx]==v) | solver.post(vars[idx]!=v))
#)
						
found = solver.solve(search)
#found=solver.solve(label(solver,vars))
if found:
    print vars
else:
	print "no solution"
	    
print solver.getStats()
print solver.getCPUTimer()

