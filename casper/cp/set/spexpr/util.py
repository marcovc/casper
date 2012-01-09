
import sys
import os

libPath = os.path.abspath('pyutils')
sys.path.append(libPath)

import objdb

def printViews(h):
	header = h
	
	class ViewCreator:
		def __init__(self,t,ev=None,m=None):
			self.header = header
			self.t = t
			self.ev = ev
			self.m = m
		def print1(self,f):
			if self.header:
				print "extern",
			print "template struct Create<"+f+","+self.t+">;"		
		def __call__(self,r,f):
			if r.properties["ev"]==self.ev and objdb.getCPModule(r)==self.m:
				self.print1(f)
	
	def varArg(ev):
		if ev!="Casper::IntSet" and ev!="Casper::BoolSet":
			return None
		return "CP::Var<"+ev+">"
		
	for arg in ["CP::Var<Casper::IntSet>"]:
		ViewCreator("CP::DomExpr<Casper::IntSet>").print1(arg)
	for arg in ["CP::Var<Casper::BoolSet>"]:
		ViewCreator("CP::DomExpr<Casper::BoolSet>").print1(arg)
		
	for arg in [varArg]:
		objdb.forAllRelOper(ViewCreator("CP::DomExpr<Casper::IntSet>","Casper::IntSet","set"),arg)
		objdb.forAllRelOper(ViewCreator("CP::DomExpr<Casper::BoolSet>","Casper::BoolSet","set"),arg)

		for ev in ["int","bool"]: 	
			objdb.forAllRelOper(ViewCreator("CP::DomExpr<"+ev+">",ev,"set"),arg)
			objdb.forAllRelOper(ViewCreator("CP::BndExpr<"+ev+">",ev,"set"),arg)
			objdb.forAllRelOper(ViewCreator("CP::ValExpr<"+ev+">",ev,"set"),arg)
		objdb.forAllRelOper(ViewCreator("CP::ChkExpr","bool","set"),arg)

	objdb.forAllRelPred(ViewCreator("CP::DomExpr<Casper::IntSet>","Casper::IntSet","set"))
	objdb.forAllRelPred(ViewCreator("CP::DomExpr<Casper::BoolSet>","Casper::BoolSet","set"))
	
	for ev in ["int","bool"]: 	
		objdb.forAllRelPred(ViewCreator("CP::DomExpr<"+ev+">",ev,"set"))
		objdb.forAllRelPred(ViewCreator("CP::BndExpr<"+ev+">",ev,"set"))
		objdb.forAllRelPred(ViewCreator("CP::ValExpr<"+ev+">",ev,"set"))
	
	objdb.forAllRelPred(ViewCreator("CP::ChkExpr","bool","set"))
				
def printPost2(h):
	header = h
	
	class PostCreator:
		def __init__(self,m=None):
			self.header = header
			self.m = m
		def print1(self,f):
			if self.header:
				print "extern",
			print "template struct PostFilter<"+f+">;"		
		def __call__(self,r,f):
			if r.properties["ev"]=="bool" and objdb.getCPModule(r)==self.m:
				self.print1(f)
	
	def varArg(ev):
		if ev!="Casper::IntSet" and ev!="Casper::BoolSet":
			return None
		return "CP::Var<"+ev+">"
	
#	def refArg(ev):
#		if ev!="bool" and ev!="int":
#			return None
#		return "Ref<"+ev+">"
#
#	def goalArg(ev):
#		if ev!="bool":
#			return None
#		return "Goal"
	
	
	for arg in [varArg]: #,refArg,goalArg]:
		objdb.forAllRelOper(PostCreator("set"),arg)	
	objdb.forAllRelPred(PostCreator("set"))

		 
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

	def varArg(ev):
		if ev not in ["Casper::BoolSet","Casper::IntSet"]:
			return None
		return "CP::Var<"+ev+">"

	if header:
		for arg in [varArg]:
			objdb.forAllRelOper(RefCreator("Ref<int>","int","set"),arg)
			objdb.forAllRelOper(RefCreator("Ref<bool>","bool","set"),arg)
		
	objdb.forAllRelPred(RefCreator("Ref<CP::Var<Casper::IntSet> >","Casper::IntSet","set"))
	objdb.forAllRelPred(RefCreator("Ref<CP::Var<Casper::BoolSet> >","Casper::BoolSet","set"))


def printExprWrapper(h):
	header = h
	
	class ExprWrapper:
		def print1(self,ev,f):
			if header:
				print "extern",
			print "template struct ExprWrapper<"+ev+","+f+">;"
		def __call__(self,r,f):
			if objdb.getCPModule(r)=="set":
				self.print1(r.properties["ev"],f)
	
	def varArg(ev):
		if ev not in ["Casper::BoolSet","Casper::IntSet"]:
			return None
		return "CP::Var<"+ev+">"

	for ev in ["Casper::BoolSet","Casper::IntSet"]:
		ExprWrapper().print1(ev,"CP::Var<"+ev+">")
		ExprWrapper().print1("Seq<"+ev+">","CP::VarArray<"+ev+",1>")
		ExprWrapper().print1("Seq<"+ev+">","CP::VarArray<"+ev+",2>")
	
	for arg in [varArg]:
		objdb.forAllRelOper(ExprWrapper(),arg)
	objdb.forAllRelPred(ExprWrapper())

def printRel(h):
	header = h
	
	class Rel:
		def print1(self,ev,f):
			if header:
				print "extern",
			print "template struct "+f+";"			
		def __call__(self,r,f):
			self.print1(r.properties["ev"],f)
	
	def varArg(ev):
		if ev not in ["Casper::BoolSet","Casper::IntSet"]:
			return None
		return "CP::Var<"+ev+">"
	
	for arg in [varArg]:
		objdb.forAllRelOper(Rel(),arg)
