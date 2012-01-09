
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
		if ev!="bool" and ev!="int":
			return None
		return "CP::Var<"+ev+">"
	
	def refArg(ev):
		if ev!="bool" and ev!="int":
			return None
		return "Ref<"+ev+">"

	def goalArg(ev):
		if ev!="bool":
			return None
		return "Goal"
	
	for arg in ["CP::Var<int>","Ref<int>","int","bool"]:
		ViewCreator("CP::DomExpr<int>").print1(arg)
		ViewCreator("CP::BndExpr<int>").print1(arg)
		ViewCreator("CP::ValExpr<int>").print1(arg)
	
	for arg in ["Goal","Ref<bool>","CP::Var<bool>","bool","int"]:
		ViewCreator("CP::BndExpr<bool>").print1(arg)
		ViewCreator("CP::ValExpr<bool>").print1(arg)
		ViewCreator("CP::ChkExpr").print1(arg)
	
	for arg in ["Ref<bool>","CP::Var<bool>","bool"]:
		ViewCreator("CP::DomExpr<bool>").print1(arg)

	if header:
		args = [varArg,refArg,goalArg]
	else:
		args = [varArg]
		
	for arg in args:
		objdb.forAllRelOper(ViewCreator("CP::DomExpr<int>","int","int"),arg)
		objdb.forAllRelOper(ViewCreator("CP::BndExpr<int>","int","int"),arg)
		objdb.forAllRelOper(ViewCreator("CP::ValExpr<int>","int","int"),arg)
		
		objdb.forAllRelOper(ViewCreator("CP::DomExpr<bool>","bool","int"),arg)
		objdb.forAllRelOper(ViewCreator("CP::BndExpr<bool>","bool","int"),arg)
		objdb.forAllRelOper(ViewCreator("CP::ValExpr<bool>","bool","int"),arg)
		
		objdb.forAllRelOper(ViewCreator("CP::ChkExpr","bool","int"),arg)
	
	objdb.forAllRelPred(ViewCreator("CP::DomExpr<int>","int","int"))
	objdb.forAllRelPred(ViewCreator("CP::BndExpr<int>","int","int"))
	objdb.forAllRelPred(ViewCreator("CP::ValExpr<int>","int","int"))
	
	objdb.forAllRelPred(ViewCreator("CP::DomExpr<bool>","bool","int"))
	objdb.forAllRelPred(ViewCreator("CP::BndExpr<bool>","bool","int"))
	objdb.forAllRelPred(ViewCreator("CP::ValExpr<bool>","bool","int"))
	
	objdb.forAllRelPred(ViewCreator("CP::ChkExpr","bool","int"))
	
	if header:
		objdb.forAllInstObjs(ViewCreator("CP::DomExpr<bool>","bool"))
		objdb.forAllInstObjs(ViewCreator("CP::BndExpr<bool>","bool"))
		objdb.forAllInstObjs(ViewCreator("CP::ValExpr<bool>","bool"))	
		objdb.forAllInstObjs(ViewCreator("CP::ChkExpr","bool"))


				
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
		if ev!="bool" and ev!="int":
			return None
		return "CP::Var<"+ev+">"
	
	def refArg(ev):
		if ev!="bool" and ev!="int":
			return None
		return "Ref<"+ev+">"

	def goalArg(ev):
		if ev!="bool":
			return None
		return "Goal"
	
	PostCreator("int").print1("CP::Var<bool>")
	
	for arg in [varArg,refArg,goalArg]:
		objdb.forAllRelOper(PostCreator("int"),arg)	
	objdb.forAllRelPred(PostCreator("int"))

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
		if ev!="bool" and ev!="int":
			return None
		return "Ref<"+ev+">"
	
	RefCreator("Ref<int>").print1("CP::Var<int>")
	RefCreator("Ref<CP::Var<int> >").print1("CP::Var<int>")
	RefCreator("Ref<CP::Var<int> >").print1("Ref<int>")
	RefCreator("Ref<bool>").print1("CP::Var<bool>")
	RefCreator("Ref<CP::Var<bool> >").print1("CP::Var<bool>")
	RefCreator("Ref<CP::Var<bool> >").print1("Goal")
	RefCreator("Ref<CP::Var<bool> >").print1("Ref<bool>")
	
	objdb.forAllRelOper(RefCreator("Ref<CP::Var<int> >","int","int"),refArg)
	objdb.forAllRelOper(RefCreator("Ref<CP::Var<bool> >","bool","int"),refArg)
	
	objdb.forAllRelPred(RefCreator("Ref<CP::Var<int> >","int","int"))
	objdb.forAllRelPred(RefCreator("Ref<CP::Var<bool> >","bool","int"))
	

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
		if ev not in ["bool","int"]:
			return None
		return "CP::Var<"+ev+">"

	for ev in ["bool","int"]:
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
		if ev not in ["bool","int"]:
			return None
		return "CP::Var<"+ev+">"

	for arg in [varArg]:
		objdb.forAllRelOper(Rel(),arg)
