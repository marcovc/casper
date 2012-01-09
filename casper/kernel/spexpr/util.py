
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
			if r.properties["ev"]==self.ev and \
				(self.m == None or objdb.getCPModule(r)==self.m):
				self.print1(f)
	
	def refArg(ev):
		if ev!="bool" and ev!="int":
			return None
		return "Ref<"+ev+">"

	def setRefArg(ev):
		if ev!="Casper::IntSet" and ev!="Casper::BoolSet":
			return None
		return "Ref<"+ev+">"
	
	RefCreator("Ref<int>").print1("Ref<int>")
	RefCreator("Ref<bool>").print1("Ref<bool>")
	RefCreator("Ref<bool>").print1("Goal")
	RefCreator("Ref<int>").print1("CP::Var<int>")
	RefCreator("Ref<bool>").print1("CP::Var<bool>")
	
	objdb.forAllRelOper(RefCreator("Ref<int>","int","int"),refArg)
	objdb.forAllRelOper(RefCreator("Ref<bool>","bool","int"),refArg)
	objdb.forAllRelPred(RefCreator("Ref<int>","int","int"))
	objdb.forAllRelPred(RefCreator("Ref<bool>","bool","int"))

	objdb.forAllRelOper(RefCreator("Ref<int>","int","set"),setRefArg)
	objdb.forAllRelOper(RefCreator("Ref<bool>","bool","set"),setRefArg)
	objdb.forAllRelPred(RefCreator("Ref<int>","int","set"))
	objdb.forAllRelPred(RefCreator("Ref<bool>","bool","set"))

	objdb.forAllInstObjs(RefCreator("Ref<int>","int"))
	objdb.forAllInstObjs(RefCreator("Ref<bool>","bool"))
	
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

def printExprWrapper(h):
	header = h
	
	class ExprWrapper:
		def print1(self,ev,f):
			if ev in ["Casper::IntSeq","Casper::BoolSeq","Seq<int>","Seq<bool>"]:
				if header:
					print "extern",
				print "template struct ExprWrapper<"+ev+","+f+">;"
			else:
				l = [ (ev,"toLiteral() const"),
					   ("Ref<"+ev+">","toRef(State&) const"),
					   ("State* const","getPState() const"),
					   ("std::ostream&","print(std::ostream&) const"),
					   ]
				if ev=="bool":
					l.append(("Goal","toGoal(State&) const"))
				for r,n in l:
					if header:
						print "extern",
					print "template "+r+" ExprWrapper<"+ev+","+f+">::"+n+";"
		def __call__(self,r,f):
			self.print1(r.properties["ev"],f)
	
	def goalArg(ev):
		if ev!="bool":
			return None
		return "Goal"
	
	def refArg(ev):
		if ev!="bool" and ev!="int":
			return None
		return "Ref<"+ev+">"

	for ev in ["bool","int"]:
		ExprWrapper().print1(ev,"Ref<"+ev+">")
		ExprWrapper().print1(ev,ev)
		ExprWrapper().print1("Seq<"+ev+">","Casper::Util::StdList<Expr<"+ev+"> >")
	ExprWrapper().print1("bool","Goal")

	for arg in [goalArg,refArg]:
		objdb.forAllRelOper(ExprWrapper(),arg)
	objdb.forAllRelPred(ExprWrapper())

	objdb.forAllInstObjs(ExprWrapper())

def printExpr(h):
	header = h
	
	class Expr:
		def print1(self,ev,f):
			if header:
				print "extern",
			print "template Expr<"+ev+">::Expr<"+f+">(const "+f+"&);"
		def __call__(self,r,f):
			self.print1(r.properties["ev"],f)
	
	def goalArg(ev):
		if ev!="bool":
			return None
		return "Goal"
	
	def refArg(ev):
		if ev!="bool" and ev!="int":
			return None
		return "Ref<"+ev+">"

	for ev in ["bool","int"]:
		Expr().print1(ev,"Ref<"+ev+">")
		Expr().print1(ev,ev)
		Expr().print1("Seq<"+ev+">","Casper::Util::StdList<Expr<"+ev+"> >")
	Expr().print1("bool","Goal")

	for arg in [goalArg,refArg]:
		objdb.forAllRelOper(Expr(),arg)
	objdb.forAllRelPred(Expr())

	objdb.forAllInstObjs(Expr())

def printRel(h):
	header = h
	
	class Rel:
		def print1(self,ev,f):
			if header:
				print "extern",
			print "template struct "+f+";"			
		def __call__(self,r,f):
			self.print1(r.properties["ev"],f)
	
	def goalArg(ev):
		if ev!="bool":
			return None
		return "Goal"
	
	def refArg(ev):
		if ev!="bool" and ev!="int":
			return None
		return "Ref<"+ev+">"

	for arg in [goalArg,refArg]:
		objdb.forAllRelOper(Rel(),arg)
	objdb.forAllRelPred(Rel())

	objdb.forAllInstObjs(Rel())
