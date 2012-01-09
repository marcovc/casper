%module kernel 

%import <kernel/expr.i>

%pythoncode %{

class _FunPar:
	def __init__(self,**kwargs):
		self.name = kwargs['name']
		if 'kw' in kwargs.keys():
			self.kw = kwargs['kw']
		else:
			self.kw = False
		if 'block' in kwargs.keys():
			self.block = kwargs['block']
		else:
			self.block = 0
		if 'default' in kwargs.keys():
			self.default = kwargs['default']
		else:
			self.default = None
		
class _BlockFunHandler:
	def __init__(self,func,signatures):
		self.func = func
		self.signatures = signatures[:]
		self.args = []
		self.curBlock = 0
					
	def __call__(self,*newargs,**kwargs):
		for parg in newargs:
			if any([sig[0].block==self.curBlock and not sig[0].kw for sig in self.signatures]):
				self.args.append(parg)
			else:
				raise TypeError("unmatched signature for function "+str(self.func))
			self.signatures = [sig[1:] for sig in self.signatures if sig[0].block >= self.curBlock and not sig[0].kw]

		if any([len(sig)==0 for sig in self.signatures]):
			return self.func(*self.args)

		nextsigs = []
		sigsargs = []
		for sig in self.signatures:
			sigargs = []
			for r in range(len(kwargs.items())):		
				if sig[0].block==self.curBlock and sig[0].kw:
					if sig[0].name in kwargs.keys():
						sigargs.append(kwargs[sig[0].name])
						sig.pop(0)
					elif sig[0].default!=None: 
						sigargs.append(sig[0].default)
						sig.pop(0)
				else:
					sig=[]
					break
			if len(sig)>0:		
				nextsigs.append(sig)
				sigsargs.append(sigargs)
				
		self.signatures = nextsigs

		# each sigargs in sigsargs must be equal otherwise call is ambiguous
		for r in range(len(sigsargs)-1):
			if sigsargs[r]!=sigsargs[r+1]:
				raise TypeError("ambiguous signature for function "+str(self.func))			
		
		if len(sigsargs)>0:
			self.args += sigsargs[0]

		if len(self.signatures)==0:
			raise TypeError("unmatched signature for function "+str(self.func))			
		if any([len(sig)==0 for sig in self.signatures]):
			return self.func(*self.args)
		self.curBlock+=1
		return self
												

class _BlockFun:
	def __init__(self,*args):
		self.args = args
	def __call__(self,*nargs,**kwargs):
		return _BlockFunHandler(*self.args)(*nargs,**kwargs)
		

%}
