
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
	
	objdb.forAllRelPred(ViewCreator("CP::DomExpr<Casper::IntSet>","Casper::IntSet","set"))
	objdb.forAllRelPred(ViewCreator("CP::DomExpr<Casper::BoolSet>","Casper::BoolSet","set"))
	
	

def printPost(h,kind):
	header = h
	class PostCreator:
		def __init__(self,m=None):
			self.m = m
		def printRel(self,func,args,argevs):
			if header:
				print "extern",
			print "template struct Post"+kind+"Filter"+str(len(args))+"<Casper::"+func+","+",".join([argevs[i]+","+args[i] for i in range(len(args))])+" >;" 
#			if header:
#				print "extern",
#			print "template struct "+kind+"FilterView"+str(len(args))+"<Casper::"+func+","+",".join([argevs[i]+","+args[i] for i in range(len(args))])+" >;" 
		def print1(self,f):
			if header:
				print "extern",
			print "template bool Post"+kind+"Filter::operator()(Store&,const "+f+"&) const;"
		def __call__(self,r,func,args,argevs):
			if r.properties["ev"]=="bool" and objdb.getCPModule(r)==self.m:
				if r.properties["type"]=="tRel":
					self.printRel(func,args,argevs)
	
	def varArg(ev):
		if ev!="Casper::IntSet" and ev!="Casper::BoolSet":
			return None
		return "CP::Var<"+ev+">"
		
	objdb.forAllRelOperList(PostCreator("set"),varArg)	
	objdb.forAllRelPredList(PostCreator("set"))
	 
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
		
	objdb.forAllRelPred(RefCreator("Ref<CP::Var<Casper::IntSet> >","Casper::IntSet","set"))
	objdb.forAllRelPred(RefCreator("Ref<CP::Var<Casper::BoolSet> >","Casper::BoolSet","set"))
