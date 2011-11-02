

from casper import *
from casper.cp import *

n = 4

solver = Solver()

vars = IntVarArray(solver,n,1,n)
#
#
#solver.post(distinct(vars))
#solver.post(distinct([vars[i]+i for i in range(0,n)]))
#solver.post(distinct([vars[i]-i for i in range(0,n)]))
#
#found = solver.solve(label(solver,vars,selectVarMinDom(solver,vars),selectValsRand(solver,vars)));
#
#if found:
#    print vars
#    found = solver.next()
#    
#print solver.getStats()
#print solver.getCPUTimer()

class Fail(Exception):
    def __str__(self):
        return "Fail"
    
def succeed():
    return True
def fail():
    return False

class display:
    def __init__(self,stri):
        self.stri = str(stri)
    def __call__(self):
        print self.stri
        return True

def forall(iterable):
    print "here"
    try:
        b = iterable.next()
    except StopIteration:
        return True
    for g in b:
        g()
        print iterable
        forall(iterable)
    

def branch(iterable):
    for g in iterable:
        print g
        yield g
 
        
            
for i in forall(branch(display((v,x)) for v in range(1,3)) for x in [-1,1]):
    print i

#for i in tryall(display(v) for v in range(1,3))():
#    print i

