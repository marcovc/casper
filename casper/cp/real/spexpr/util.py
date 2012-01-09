
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
			if r.properties["ev"]==self.ev and (self.m == None or objdb.getCPModule(r)==self.m):
				self.print1(f)
	
	def varArg(ev):
		if ev!="float":
			return None
		return "CP::Var<"+ev+">"
	
	def refArg(ev):
		if ev!="float":
			return None
		return "Ref<"+ev+">"
	
	for arg in ["CP::Var<float>","Ref<float>","float"]:
		ViewCreator("CP::BndExpr<float>").print1(arg)
		
	if header:
		args = [varArg,refArg]
	else:
		args = [varArg]
		
	for arg in args:
		objdb.forAllRelOper(ViewCreator("CP::BndExpr<float>","float","real"),arg)
		
		objdb.forAllRelOper(ViewCreator("CP::DomExpr<bool>","bool","real"),arg)
		objdb.forAllRelOper(ViewCreator("CP::BndExpr<bool>","bool","real"),arg)
		objdb.forAllRelOper(ViewCreator("CP::ValExpr<bool>","bool","real"),arg)
		
		objdb.forAllRelOper(ViewCreator("CP::ChkExpr","bool","real"),arg)
	
	objdb.forAllRelPred(ViewCreator("CP::BndExpr<float>","float","real"))
	
	objdb.forAllRelPred(ViewCreator("CP::DomExpr<bool>","bool","real"))
	objdb.forAllRelPred(ViewCreator("CP::BndExpr<bool>","bool","real"))
	objdb.forAllRelPred(ViewCreator("CP::ValExpr<bool>","bool","real"))
	
	objdb.forAllRelPred(ViewCreator("CP::ChkExpr","bool","real"))
	
	if header:
		objdb.forAllInstObjs(ViewCreator("CP::DomExpr<bool>","real"))
		objdb.forAllInstObjs(ViewCreator("CP::BndExpr<bool>","real"))
		objdb.forAllInstObjs(ViewCreator("CP::ValExpr<bool>","real"))	
		objdb.forAllInstObjs(ViewCreator("CP::ChkExpr","real"))
				
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
			if r.properties["ev"]=="bool" and (self.m==None or objdb.getCPModule(r)==self.m):
				self.print1(f)
	
	def varArg(ev):
		if ev!="float":
			return None
		return "CP::Var<"+ev+">"
	
	def refArg(ev):
		if ev!="float":
			return None
		return "Ref<"+ev+">"
	
	for arg in [varArg,refArg]:
		objdb.forAllRelOper(PostCreator("real"),arg)	
	objdb.forAllRelPred(PostCreator("real"))

	if header:
		objdb.forAllInstObjs(PostCreator())
	
	
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
		if ev!="float":
			return None
		return "Ref<"+ev+">"
	
	RefCreator("Ref<float>").print1("CP::Var<float>")
	RefCreator("Ref<CP::Var<float> >").print1("CP::Var<float>")
	RefCreator("Ref<CP::Var<float> >").print1("Ref<float>")
	
	objdb.forAllRelOper(RefCreator("Ref<CP::Var<float> >","float","real"),refArg)
	objdb.forAllRelPred(RefCreator("Ref<CP::Var<float> >","float","real"))
	

def printExprWrapper(h):
	header = h
	
	class ExprWrapper:
		def print1(self,ev,f):
			if header:
				print "extern",
			print "template struct ExprWrapper<"+ev+","+f+">;"
		def __call__(self,r,f):
			self.print1(r.properties["ev"],f)
	
	def varArg(ev):
		if ev not in ["float"]:
			return None
		return "CP::Var<"+ev+">"

	for ev in ["float"]:
		ExprWrapper().print1(ev,"CP::Var<"+ev+">")
		ExprWrapper().print1("Seq<"+ev+">","CP::VarArray<"+ev+",1>")
		ExprWrapper().print1("Seq<"+ev+">","CP::VarArray<"+ev+",2>")
	
	for arg in [varArg]:
		objdb.forAllRelOper(ExprWrapper(),arg)

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
		if ev not in ["float"]:
			return None
		return "CP::Var<"+ev+">"

	for arg in [varArg]:
		objdb.forAllRelOper(Rel(),arg)
