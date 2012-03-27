###########################################################################
#   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   #
#                                                                         #
#   Copyright:                                                            #
#   2012-2012 - Marco Correia <marco.v.correia@gmail.com>                 #
#                                                                         #
#   Licensed under the Apache License, Version 2.0 (the "License");       #
#   you may not use this file except in compliance with the License.      #
#   You may obtain a copy of the License at                               #
#            http://www.apache.org/licenses/LICENSE-2.0                   #
#   Unless required by applicable law or agreed to in writing, software   #
#   distributed under the License is distributed on an                    #
#   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,          #
#   either express or implied.                                            #
#   See the License for the specific language governing permissions and   #
#   limitations under the License.                                        #
###########################################################################

import itertools
import random

def log2(x):
	import math
	return math.log(x)/math.log(2)

def nbCombs(n,k):
	return reduce(lambda a,b: a*(n-b)/(b+1),xrange(k),1)

def getIdxsCombs(sol,arity,nbCons):
	if nbCons==None or nbCons >= nbCombs(len(sol),arity):
		combs = [c for c in itertools.combinations(range(len(sol)),arity)]
		random.shuffle(combs)
	else:
		# try to cover all variables
		combs = []
		l = set(range(0,len(sol)))
		for i in range(0,nbCons):
			# generate random set of arity indexes
			if len(l)<arity:
				l = set(range(0,len(sol)))
			idxs = random.sample(l,arity)
			l.difference_update(idxs)
			combs.append(idxs)
	return combs

def getFreeVars(sol,constraints):
	l = set(range(len(sol)))
	for c in constraints:
		l.difference_update(c)
	return l

class CIntDistinct:
	def __init__(self,sol,arity,nbCons=None):
		self.sol = sol
		self.constraints = []
		self.iconstraints = []
		combs = getIdxsCombs(sol,arity,nbCons)
		# create constraints
		for idxs in combs:
			if self.distinct(idxs):
				if nbCons==0:
					break
				elif nbCons!=None:
					nbCons-=1
				self.constraints.append(idxs)
		# if more constraints are required
		for idxs in combs:
			if not self.distinct(idxs):
				if nbCons==0:
					break
				elif nbCons!=None: 
					nbCons-=1
				self.iconstraints.append(idxs)

	def getFreeVars(self):
		return getFreeVars(self.sol,self.constraints+self.iconstraints)
	
	def distinct(self,idxs):
		for i in range(len(idxs)):
			for j in range(i+1,len(idxs)):
				if self.sol[idxs[i]]==self.sol[idxs[j]]:
					return False
		return True
			
	def printMzn(self,constraintAnn=None):
		for c in self.constraints:
			print "constraint alldifferent(["+",".join(["q["+str(i+1)+"]" for i in c])+"]);"
		for c in self.iconstraints:
			l = set(range(min(self.sol),max(self.sol)+1))
			d = set()
			# remove used values from l, and add direct idxs to d
			for i in c:
				if self.sol[i] in l:
					l.remove(self.sol[i])
					d.add(i)
			# associate a free element with the remaining variables
			q = []
			for i in c:
				if i in d:
					q.append("q["+str(i+1)+"]")
				else:
					e = l.pop()
					q.append("q["+str(i+1)+"]+"+str(e-self.sol[i]))
			if constraintAnn==None:
				print "constraint alldifferent(["+",".join(q)+"]);"
			else:
				print "constraint alldifferent(["+",".join(q)+"]) :: "+constraintAnn+";"

	def getNbSolsUb(self,nbValues):
		allcs = self.constraints + self.iconstraints
		prod = [nbValues]*len(self.sol)
		for c in allcs:
			curVal = nbValues
			for i in c:
				prod[i] = min(prod[i],curVal)
				if curVal>0:
					curVal -= 1
		r = 1
		for i in prod:
			r *= i
		return r


class CLinearEq:
	def __init__(self,sol,arity,absCoeff=5,nbCons=None):
		self.sol = sol
		self.arity = arity
		self.constraints = getIdxsCombs(sol,arity,nbCons)
		self.minCoeff = -absCoeff;
		self.maxCoeff = absCoeff;
		
	def getFreeVars(self):
		return getFreeVars(self.sol,self.constraints)
			
	def printMzn(self,constraintAnn=None):
		for c in self.constraints:
			coeffs = []
			l = []
			r = 0
			for i in c:
				coeff = random.choice(xrange(self.minCoeff,self.maxCoeff+1))
				if coeff==0:	# FIXME
					coeff = 1
				l.append("q["+str(i+1)+"]*"+str(coeff))
				r += self.sol[i]*coeff
			if constraintAnn==None:
				print "constraint "+"+".join(l)+"=="+str(r)+";"
			else:
				print "constraint "+"+".join(l)+"=="+str(r)+" :: "+constraintAnn+";"

	def getNbSolsUb(self,nbValues):
		ep = (self.arity-1)*len(self.constraints)
		en = 0
		for c1 in range(len(self.constraints)):
			for c2 in range(c1+1,len(self.constraints)):
				en += len(set(self.constraints[c1]).intersection(set(self.constraints[c2])))
		return nbValues**(ep-en)

class CLinearIneq:
	def __init__(self,sol,arity,absCoeff=5,nbCons=None):
		self.sol = sol
		self.arity = arity
		self.constraints = getIdxsCombs(sol,arity,nbCons)
		self.minCoeff = -absCoeff;
		self.maxCoeff = absCoeff;
		
	def getFreeVars(self):
		return getFreeVars(self.sol,self.constraints)
			
	def printMzn(self,constraintAnn=None):
		for c in self.constraints:
			coeffs = []
			l = []
			r = 0
			for i in c:
				coeff = random.choice(xrange(self.minCoeff,self.maxCoeff+1))
				if coeff==0:	# FIXME
					coeff = 1
				l.append("q["+str(i+1)+"]*"+str(coeff))
				r += self.sol[i]*coeff
			ctr = random.choice([">="+str(r-random.choice(self.looseness)),
							     "<="+str(r+random.choice(self.looseness))])
			if constraintAnn==None:
				print "constraint "+"+".join(l)+ctr+";"
			else:
				print "constraint "+"+".join(l)+ctr+";"

#	def getNbSolsUb(self,nbValues):
#		ep = (self.arity-1)*len(self.constraints)
#		en = 0
#		for c1 in range(len(self.constraints)):
#			for c2 in range(c1+1,len(self.constraints)):
#				en += len(set(self.constraints[c1]).intersection(set(self.constraints[c2])))
#		return nbValues**(ep-en)
			
class StressCSP:
	def __init__(self,nbVars,nbVals):
		self.constraints = []
		self.nbVars = nbVars
		self.nbVals = nbVals
		self.minVal = 1
		self.maxVal = nbVals
		self.sol = []
		for i in range(0,nbVars):
			self.sol.append(random.choice(range(self.minVal,self.maxVal+1)))
	def addConstraint(self,c,*args):
		cons = c(self.sol,*args)
		self.constraints.append(cons)
	def getFreeVars(self):
		l = set(range(self.nbVars))
		for c in self.constraints:
			l.intersection_update(c.getFreeVars())
		return l
	def computeK(self):
		actualNbVars = self.nbVars-len(self.getFreeVars())
		for c in self.constraints:
			sols = log2(c.getNbSolsUb(self.nbVals))
			sp = log2(self.nbVals**actualNbVars)
			k = 1-sols/sp
			print k,sols,sp
	def printMzn(self,constraintAnn=None):
		print "include \"globals.mzn\";"
		print "array [1.."+str(self.nbVars)+"] of var "+str(self.minVal)+".."+str(self.maxVal)+": q;"
		for i in self.constraints:
			i.printMzn(constraintAnn)
		freeVars = self.getFreeVars()
		# fix free vars
		for v in freeVars:
			print "constraint q["+str(v+1)+"]=="+str(self.sol[v])+";"
		print "solve :: int_search(q, first_fail, indomain_min, complete) satisfy;"
		print "output [ show(q) ++ \"\\n\" ];"
		
		
if __name__ == "__main__":

	import argparse
	
	parser = argparse.ArgumentParser(description='Generate satisfiable CSP problem involving specific constraints')
	parser.add_argument('seed', nargs=1,
	                  help='random seed')
	
	args = parser.parse_args()
	
	random.seed(int(args.seed[0]))
	csp = StressCSP(150,10)
	#csp.addConstraint(CIntDistinct, 100, 43)
	csp.addConstraint(CLinearEq, 100, 10, 3)
	csp.printMzn()
	#csp.computeK()
	