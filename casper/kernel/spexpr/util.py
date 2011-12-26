
import sys
import os

libPath = os.path.abspath('pyutils')
sys.path.append(libPath)

import objdb

def printRef(h):
	header = h
	
	class RefCreator:
		def __init__(self,t,ev=None,m=None):
			self.t = t
			self.ev = ev
			self.m = m
		def print1(self,f):
			if header:
				print "extern",
			print "template struct Create<"+f+","+self.t+">;"
		def __call__(self,r,f):
			if r.properties["ev"]==self.ev and objdb.getCPModule(r)==self.m:
				self.print1(f)
	
	def refArg(ev):
		if ev!="bool" and ev!="int":
			return None
		return "Ref<"+ev+">"
	
	RefCreator("Ref<int>").print1("Ref<int>")
	RefCreator("Ref<bool>").print1("Ref<bool>")
	RefCreator("Ref<bool>").print1("Goal")
	
	objdb.forAllRelOper(RefCreator("Ref<int>","int","int"),refArg)
	objdb.forAllRelOper(RefCreator("Ref<bool>","bool","int"),refArg)
	
	objdb.forAllRelPred(RefCreator("Ref<int>","int","int"))
	objdb.forAllRelPred(RefCreator("Ref<bool>","bool","int"))


def printGoal(h):
	header = h
	
	class GoalCreator:
		def print1(self,f):
			if header:
				print "extern",
			print "template struct Create<"+f+",Goal>;"
		def __call__(self,r,f):
			if r.properties["ev"]=="bool":
				self.print1(f)
	
	def goalArg(ev):
		if ev!="bool":
			return None
		return "Goal"
	
	GoalCreator().print1("Goal")
	GoalCreator().print1("Ref<bool>")
	
	objdb.forAllRelOper(GoalCreator(),goalArg)
	objdb.forAllRelPred(GoalCreator())
