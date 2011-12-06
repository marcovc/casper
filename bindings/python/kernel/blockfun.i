%module kernel 

%import <kernel/expr.i>

%pythoncode %{

class BlockFunHandler:
	def __init__(self,func,signatures):
		self.func = func
		self.signatures = signatures[:]
		self.args = []
	def __call__(self,*newargs):
		newargcount = len(newargs)
		found = False
		for i in range(len(self.signatures)):
			if self.signatures[i][0] == newargcount:
				self.signatures[i] = self.signatures[i][1:]
				found = True
			else: 
				self.signatures[i] = []
		if not found:
			raise TypeError("wrong signature for function "+str(self.func))
		self.signatures = [sig for sig in self.signatures if len(sig)>0]
		self.args += newargs
		if len(self.signatures)>0:
			return self
		return self.func(*self.args)

class BlockFun:
	def __init__(self,*args):
		self.args = args
	def __call__(self,*nargs):
		return BlockFunHandler(*self.args)(*nargs)
		

%}
