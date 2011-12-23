
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
			if self.header:
				print "extern",
			print "template "+self.t+" Create<"+f+","+self.t+">::operator()(CP::Store&,const "+f+"&);"		
		def __call__(self,r,f):
			if r.properties["ev"]==self.ev and objdb.getCPModule(r)==self.m:
				self.print1(f)
	
	def varArg(ev):
		if ev!="bool" and ev!="int":
			return None
		return "CP::Var<"+ev+">"
	
	def refArg(ev):
		if ev!="bool" and ev!="int":
			return None
		return "Ref<"+ev+">"
	
	for arg in ["CP::Var<int>","Ref<int>"]:
		ViewCreator("CP::DomExpr<int>").print1(arg)
		ViewCreator("CP::BndExpr<int>").print1(arg)
		ViewCreator("CP::ValExpr<int>").print1(arg)
	
	for arg in ["Goal","Ref<bool>","CP::Var<bool>"]:
		ViewCreator("CP::BndExpr<bool>").print1(arg)
		ViewCreator("CP::ValExpr<bool>").print1(arg)
		ViewCreator("CP::ChkExpr").print1(arg)
	
	ViewCreator("CP::DomExpr<bool>").print1("Ref<bool>")
	
	for arg in [varArg]:
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
		if ev!="bool" and ev!="int":
			return None
		return "CP::Var<"+ev+" >"
	
	def refArg(ev):
		if ev!="bool" and ev!="int":
			return None
		return "Ref<"+ev+" >"
	
	def goalArg(ev):
		if ev!="bool":
			return None
		return "Goal"
	
	PostCreator("int").print1("CP::Var<bool>")
	
	objdb.forAllRelOperList(PostCreator("int"),varArg)	
	objdb.forAllRelPredList(PostCreator("int"))
	
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
	
	RefCreator("Ref<CP::Var<int> >").print1("CP::Var<int>")
	RefCreator("Ref<CP::Var<int> >").print1("Ref<int>")
	RefCreator("Ref<CP::Var<bool> >").print1("CP::Var<bool>")
	RefCreator("Ref<CP::Var<bool> >").print1("Goal")
	RefCreator("Ref<CP::Var<bool> >").print1("Ref<bool>")
	
	objdb.forAllRelOper(RefCreator("Ref<CP::Var<int> >","int","int"),refArg)
	objdb.forAllRelOper(RefCreator("Ref<CP::Var<bool> >","bool","int"),refArg)
	
	objdb.forAllRelPred(RefCreator("Ref<CP::Var<int> >","int","int"))
	objdb.forAllRelPred(RefCreator("Ref<CP::Var<bool> >","bool","int"))
