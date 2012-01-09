
import sys
import os

libPath = os.path.abspath('pyutils')
sys.path.append(libPath)

import objdb


def printExprWrapper(h,m=None):
	header = h
	
	class ExprWrapper:
		def __init__(self,m=None):
			self.m = m
			
		def print1(self,ev,f):
			if ev in ["Casper::IntSeq","Casper::BoolSeq","Seq<int>","Seq<bool>"]:
				if header:
					print "extern",
				print "template struct ExprWrapper<"+ev+","+f+">;"
			else:
				l = [ ("CP::DomExpr<"+ev+">","toCPDomExpr(CP::Store&) const"),
					  ("CP::BndExpr<"+ev+">","toCPBndExpr(CP::Store&) const"),
					  ("CP::ValExpr<"+ev+">","toCPValExpr(CP::Store&) const"),
					   ("Ref<CP::Var<"+ev+"> >","toCPVarRef(State&) const"),
					   ]
				if ev=="bool":
					l.append(("CP::ChkExpr","toChkExpr(CP::Store&) const"))
					l.append(("bool","postDomFilter(CP::Store&) const"))
					l.append(("bool","postBndFilter(CP::Store&) const"))
					l.append(("bool","postValFilter(CP::Store&) const"))
				for r,n in l:
					if header:
						print "extern",
					print "template "+r+" ExprWrapper<"+ev+","+f+">::"+n+";"
		def __call__(self,r,f):
			if self.m==None or objdb.getCPModule(r)==self.m:
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
		objdb.forAllRelOper(ExprWrapper(m),arg)
	objdb.forAllRelPred(ExprWrapper(m))

	objdb.forAllInstObjs(ExprWrapper())

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
