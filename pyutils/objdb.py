
import itertools

attrsPriority = ["type","cl","ev","extType","func",	"nArgs",\
				"arg1Eval","arg2Eval","arg3Eval","arg4Eval","arg5Eval"]

class Property:
	def __init__(self,attr,value):
		self.attr = attr
		self.value = value
	def __eq__(self,other):
		return self.attr==other.attr and self.value==other.value
	def __ne__(self,other):
		return self.attr!=other.attr or self.value!=other.value
	def __hash__(self):
		return hash(self.attr)+hash(self.value)

class Rule:
	def __init__(self,**kwargs):
		self.properties = dict(kwargs)
	def covers(self,rule):
		for k,v in rule.properties.iteritems():
			if k not in self.properties.keys() or self.properties[k]!=v:
				return False
		return True 
	def isCoveredBy(self,rule):
		for k,v in self.properties.iteritems():
			if k not in rule.properties.keys() or rule.properties[k]!=v:
				return False
		return True 

class ActionMap:
	def __init__(self):
		self.rules = []	
	def add(self,rule,action):
		#self.rules += [(i,action) for i in itertools.permutations(rule,len(rule))]
		self.rules.append((rule,action))
	def runAll(self):
		for (rule,action) in self.rules:
			action(rule)
	def getCoveredBySet(self,rule):
		cover = []
		for (mrule,action) in self.rules:
			if mrule.isCoveredBy(rule):
				cover.append((mrule,action))
		return cover
	def getCoverSet(self,rule):
		cover = []
		for (mrule,action) in self.rules:
			if mrule.covers(rule):
				cover.append((mrule,action))
		return cover

objs = set()
seqTypes = ["Casper::BoolSeq", "Casper::IntSeq", "Casper::BoolSetSeq", "Casper::IntSetSeq"]
setTypes = ["Casper::IntSet","Casper::BoolSet"]

# literal types
for e in ["bool", "int"]:
	objs.add(Rule(type="tExt",cl="Common",ev=e,extType="Lit"))
for e in ["Casper::BoolSeq", "Casper::IntSeq"]:
	objs.add(Rule(type="tExt",cl="CP",ev=e,extType="LitArray"))
	
# CP types
for e in ["bool", "int"]:
	objs.add(Rule(type="tExt",cl="CP",ev=e,extType="Var"))
for e in seqTypes:
	objs.add(Rule(type="tExt",cl="CP",ev=e,extType="VarArray"))


# boolean unary operations between boolean types
objs.add(Rule(type="tRel",func="Not",ev="bool",nArgs=1,arg1Eval="bool"))
objs.add(Rule(type="tRel",func="Cast<bool>",ev="bool",nArgs=1,arg1Eval="int"))

# boolean unary operations between integer sequence types
objs.add(Rule(type="tRel",func="Distinct",ev="bool",nArgs=1,arg1Eval="Casper::IntSeq"))

# boolean unary operations between set types
for f in ["Disjoint","Partition"]:
	for t in ["Casper::BoolSetSeq", "Casper::IntSetSeq"]:
		objs.add(Rule(type="tRel",func=f,ev="bool",nArgs=1,arg1Eval=t))

# boolean binary operations between boolean types
for f in ["And","Or","XOr","Less","LessEqual","Greater","GreaterEqual"]:
	objs.add(Rule(type="tRel",func=f,ev="bool",nArgs=2,arg1Eval="bool",arg2Eval="bool"))

# boolean binary operations between integer types
for f in ["Less","LessEqual","Greater","GreaterEqual"]:
	objs.add(Rule(type="tRel",func=f,ev="bool",nArgs=2,arg1Eval="int",arg2Eval="int"))

# boolean binary operations between set types
for f in ["Disjoint","Contained"]:
	for t in ["Casper::BoolSet", "Casper::IntSet"]:
		objs.add(Rule(type="tRel",func=f,ev="bool",nArgs=2,arg1Eval=t,arg2Eval=t))

# boolean binary operations between several type pairs
for f in ["Distinct","Equal"]:
	for t in ["bool", "int", "Casper::BoolSet", "Casper::IntSet"]:
		objs.add(Rule(type="tRel",func=f,ev="bool",nArgs=2,arg1Eval=t,arg2Eval=t))

# boolean binary operations between heterogeneous types
objs.add(Rule(type="tRel",func="SumEqual",ev="bool",nArgs=2,arg1Eval="Casper::IntSeq",arg2Eval="int"))
objs.add(Rule(type="tRel",func="Member",ev="bool",nArgs=2,arg1Eval="int",arg2Eval="Casper::IntSet"))
objs.add(Rule(type="tRel",func="NotMember",ev="bool",nArgs=2,arg1Eval="bool",arg2Eval="Casper::BoolSet"))
objs.add(Rule(type="tRel",func="Element",ev="bool",nArgs=2,arg1Eval="Casper::BoolSeq",arg2Eval="int"))
objs.add(Rule(type="tRel",func="InTable",ev="bool",nArgs=2,arg1Eval="Casper::IntSeq",arg2Eval="Casper::IntSeq"))
objs.add(Rule(type="tRel",func="NotInTable",ev="bool",nArgs=2,arg1Eval="Casper::IntSeq",arg2Eval="Casper::IntSeq"))

# boolean ternary operations
objs.add(Rule(type="tRel",func="SumProductEqual",ev="bool",nArgs=3,arg1Eval="Casper::IntSeq",arg2Eval="Casper::IntSeq",arg3Eval="int"))
objs.add(Rule(type="tRel",func="ElementEqual",ev="bool",nArgs=3,arg1Eval="Casper::IntSeq",arg2Eval="int",arg3Eval="int"))
objs.add(Rule(type="tRel",func="ElementEqual",ev="bool",nArgs=3,arg1Eval="Casper::BoolSeq",arg2Eval="int",arg3Eval="bool"))
for e in ["Casper::IntSet","Casper::BoolSet"]:
	objs.add(Rule(type="tRel",func="IntersectEqual",ev="bool",nArgs=3,arg1Eval=e,arg2Eval=e,arg3Eval=e))
	objs.add(Rule(type="tRel",func="UnionEqual",ev="bool",nArgs=3,arg1Eval=e,arg2Eval=e,arg3Eval=e))

# boolean five-ary (!?) operations
objs.add(Rule(type="tRel",func="Cumulative",ev="bool",nArgs=5,arg1Eval="Casper::IntSeq",arg2Eval="Casper::IntSeq",\
							arg3Eval="Casper::IntSeq",arg4Eval="Casper::IntSeq",arg5Eval="int"))

# integer unary operations between integer types
for f in ["Sym","Abs","Exp","Sqr","Log"]:
	objs.add(Rule(type="tRel",func=f,ev="int",nArgs=1,arg1Eval="int"))
objs.add(Rule(type="tRel",func="Cast<int>",ev="int",nArgs=1,arg1Eval="bool"))

# integer unary operations between integer sequence types
objs.add(Rule(type="tRel",func="Sum",ev="int",nArgs=1,arg1Eval="Casper::IntSeq"))

# integer unary operations between set types
objs.add(Rule(type="tRel",func="Cardinal",ev="int",nArgs=1,arg1Eval="Casper::IntSet"))

# integer binary operations between integer types
for f in ["Add","Sub","Mul","Div","Min","Max","Mod"]:
	objs.add(Rule(type="tRel",func=f,ev="int",nArgs=2,arg1Eval="int",arg2Eval="int"))

# integer binary operations between integer sequence types
objs.add(Rule(type="tRel",func="SumProduct",ev="int",nArgs=2,arg1Eval="Casper::IntSeq",arg2Eval="Casper::IntSeq"))

# integer binary operations between heterogeneous types
objs.add(Rule(type="tRel",func="Element",ev="int",nArgs=2,arg1Eval="Casper::IntSeq",arg2Eval="int"))


## Goals ##
for t in ["bool","int"]:
	objs.add(Rule(type="tRel",func="Assign",ev="bool",nArgs=2,arg1Eval=t,arg2Eval=t))
objs.add(Rule(type="tRel",func="WhileDo",ev="bool",nArgs=2,arg1Eval="bool",arg2Eval="bool"))

objs.add(Rule(type="tRel",func="ForAll",ev="bool",nArgs=3,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval="bool"))
objs.add(Rule(type="tRel",func="ForAll",ev="bool",nArgs=4,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval="bool",arg4Eval="bool"))

objs.add(Rule(type="tRel",func="TryAll",ev="bool",nArgs=3,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval="bool"))
objs.add(Rule(type="tRel",func="TryAll",ev="bool",nArgs=4,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval="bool",arg4Eval="bool"))

for t in ["int"]:
	objs.add(Rule(type="tRel",func="ForAll",ev="bool",nArgs=5,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval="bool",arg4Eval=t,arg5Eval="bool"))
	objs.add(Rule(type="tRel",func="TryAll",ev="bool",nArgs=5,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval="bool",arg4Eval=t,arg5Eval="bool"))
#	objs.add(Rule(type="tRel",func="SelectMin",ev="bool",nArgs=4,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval="bool",arg4Eval=t))
#	objs.add(Rule(type="tRel",func="SelectMin",ev="bool",nArgs=3,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval=t))
#	objs.add(Rule(type="tRel",func="SelectMax",ev="bool",nArgs=4,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval="bool",arg4Eval=t))
#	objs.add(Rule(type="tRel",func="SelectMax",ev="bool",nArgs=3,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval=t))
#objs.add(Rule(type="tRel",func="SelectRand",ev="bool",nArgs=3,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval="bool"))
#objs.add(Rule(type="tRel",func="SelectRand",ev="bool",nArgs=2,arg1Eval="int",arg2Eval="Casper::IntSeq"))

## Ref expressions
for f in ["bool","int","Casper::IntSet","Casper::BoolSet","Casper::IntSeq","Casper::BoolSeq","Casper::BoolSetSeq","Casper::IntSetSeq"]:
	objs.add(Rule(type="tRel",cl="CP",func="Ground",ev="bool",nArgs=1,arg1Eval=f))
for f in ["bool","int"]:
	objs.add(Rule(type="tRel",cl="CP",func="DomainSize",ev="int",nArgs=1,arg1Eval=f))
objs.add(Rule(type="tRel",cl="CP",func="Domain",ev="Casper::IntSeq",nArgs=1,arg1Eval="int"))
objs.add(Rule(type="tRel",cl="CP",func="Domain",ev="Casper::BoolSeq",nArgs=1,arg1Eval="bool"))

for t in ["bool","int","float","Casper::IntSeq","Casper::BoolSeq","Casper::FloatSeq"]:
	objs.add(Rule(type="tRel",func="ArgMax",ev="int",nArgs=3,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval=t))
	objs.add(Rule(type="tRel",func="ArgMax",ev="int",nArgs=4,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval="bool",arg4Eval=t))
	objs.add(Rule(type="tRel",func="ArgMin",ev="int",nArgs=3,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval=t))
	objs.add(Rule(type="tRel",func="ArgMin",ev="int",nArgs=4,arg1Eval="int",arg2Eval="Casper::IntSeq",arg3Eval="bool",arg4Eval=t))


# TODO later
#				"fIfThen","fIfThenElse","fInRange",
#				"fAll","fMaxDiff",
#				"fForSome","fSelectFirst",,"fRandInRange",



cppObjType = ActionMap()

cppObjType.add(Rule(ev="bool",extType="Lit"),lambda r: "bool")
cppObjType.add(Rule(ev="int",extType="Lit"),lambda r: "int")
cppObjType.add(Rule(ev="Casper::BoolSeq",extType="LitArray"),lambda r: "Util::StdBoolArray")
cppObjType.add(Rule(ev="Casper::IntSeq",extType="LitArray"),lambda r: "Util::StdIntArray")
cppObjType.add(Rule(cl="CP",ev="bool",extType="Var"),lambda r: "CP::Var<bool,Casper::CP::Traits::GetDefaultDom<bool>::Type>")
cppObjType.add(Rule(cl="CP",ev="int",extType="Var"),lambda r: "CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type>")
cppObjType.add(Rule(cl="CP",ev="Casper::BoolSet",extType="Var"),lambda r: "CP::Casper::BoolSetVar")
cppObjType.add(Rule(cl="CP",ev="Casper::IntSet",extType="Var"),lambda r: "CP::Casper::IntSetVar")
cppObjType.add(Rule(cl="CP",ev="Casper::BoolSeq",extType="VarArray"),lambda r: "CP::BoolVarArray")
cppObjType.add(Rule(cl="CP",ev="Casper::IntSeq",extType="VarArray"),lambda r: "CP::IntVarArray")
cppObjType.add(Rule(cl="CP",ev="Casper::BoolSetSeq",extType="VarArray"),lambda r: "CP::Casper::BoolSetVarArray")
cppObjType.add(Rule(cl="CP",ev="Casper::IntSetSeq",extType="VarArray"),lambda r: "CP::Casper::IntSetVarArray")

for i in range(1,6):
	cppObjType.add(Rule(type="tRel",nArgs=i),
				lambda r,i=i: "Rel"+str(i)+"<"+r.properties["func"][1:]+
					"".join([","+cppEval[r.properties["arg"+str(j)+"Eval"]] for j in range(1,i+1)])
					+">")
 #			
#intBndExpr = ActionMap()
#
#def printTest(rule):
#	strs = []
#	for attr in attrsPriority:
#		if attr in rule.properties.keys():
#			value = rule.properties[attr]
#			if attr in cppAttrPrefix.keys():
#				value = cppAttrPrefix[attr]+str(value)
#			s = (cppTest[attr]+"=="+str(value))
#			strs += [s]
#	print "if (",strs[0],
#	for i in range(1,len(strs)):
#		print "and",strs[i],
#	print ")"
#
#def printAction(rule,target):
#	sources = cppObjType.getCoveredBySet(rule)
#	if len(sources)==0:
#		print "\tthrow NotImplemented();"
#		return 
#	assert(len(sources)==1)
#	source = sources[0][1](rule)
#	print "\treturn Create<"+source+","+target+">()(store,obj);"
#		
#def printIntBndExpr(rule):
#	printTest(rule)
#	printAction(rule,"CP::IntBndExpr")
#		
#for r in objs:
#	if r.properties["ev"]=="int": 
#		intBndExpr.add(r,printIntBndExpr)
#
#intBndExpr.runAll()

#python unary and binary operators
casperOp2PythonOp = {
		("Equal",2) : "eq",
		("Distinct",2) : "ne",
		("And",2) : "and",
		("XOr",2) : "xor",
		("Or",2) : "or",
		("Not",1) : "invert",
		("Mod",2) : "mod",
		("Less",2) : "lt",
		("LessEqual",2) : "le",
		("Greater",2) : "gt",
		("GreaterEqual",2) : "ge",
		("Add",2) : "add",
		("Sym",1) : "neg",
		("Sub",2) : "sub",
		("Mul",2) : "mul", 
		("Div",2) : "div",
		("Pow",2) : "pow",
}

customCasperFn2PythonFn = {
		"Cast<int>" : "asInt",
		"Cast<bool>" : "asBool",
		"Abs" : "_abs",
		"Min" : "_min",
		"Max" : "_max",
		"Sum" : "_sum",
		"Pow" : "_pow",
		"WhileDo" : "_whileDo",
		"ForAll" : "_forAll",
		"TryAll" : "_tryAll",
		"SelectMin" : "_selectMin",
		"SelectMax" : "_selectMax",
		"SelectRand" : "_selectRand",						
}


def casperFn2PythonFn(casperFn):
	if casperFn in customCasperFn2PythonFn.keys():
		return customCasperFn2PythonFn[casperFn]
	return casperFn[0].lower()+casperFn[1:] 

# binary operators

def printUnaryOperator(func,ev,arg1):
	print "Casper::Expr<"+ev+">"
	print  "__"+casperOp2PythonOp[(func,1)]+"__()"
	print "{"
	print "\tCasper::Rel1<Casper::"+func+","+arg1+"> r(*$self);"
	print "\treturn r;"
	print "}" 

def printBinaryOperator(func,ev,arg1,arg2,reverse=False):
	print "Casper::Expr<"+ev+">"
	if not reverse:
		print  "__"+casperOp2PythonOp[(func,2)]+"__("+arg2+" const& v)"
	else:
		print  "__r"+casperOp2PythonOp[(func,2)]+"__("+arg2+" const& v)"
	print "{"
	if not reverse:	
		print "\tCasper::Rel2<Casper::"+func+","+arg1+","+arg2+"> r(*$self,v);"
	else:
		print "\tCasper::Rel2<Casper::"+func+","+arg2+","+arg1+"> r(v,*$self);"
	print "\treturn r;"
	print "}" 

def printVarOperators(arg1,ev1):
	for r in objs:
		if r.properties["type"]=="tRel" and \
			(r.properties["func"], r.properties["nArgs"]) in casperOp2PythonOp.keys() and \
			r.properties["arg1Eval"]==ev1:
			if r.properties["nArgs"]==1:
				printUnaryOperator(r.properties["func"],r.properties["ev"],arg1)
			if r.properties["nArgs"]==2:
				ev2 = r.properties["arg2Eval"]
				# self with expression
				arg2 = "Casper::Expr<"+ev2+">"
				printBinaryOperator(r.properties["func"],r.properties["ev"],arg1,arg2)
				# expression with self
				printBinaryOperator(r.properties["func"],r.properties["ev"],arg1,arg2,True)

def printExprOperators(arg1,ev1):
	for r in objs:
		if r.properties["type"]=="tRel" and \
			(r.properties["func"], r.properties["nArgs"]) in casperOp2PythonOp.keys() and \
			r.properties["arg1Eval"]==ev1:
			if r.properties["nArgs"]==1:
				printUnaryOperator(r.properties["func"],r.properties["ev"],arg1)
			if r.properties["nArgs"]==2:
				ev2 = r.properties["arg2Eval"]
				# expr with expr	
				arg2 = "Casper::Expr<"+ev2+">"
				printBinaryOperator(r.properties["func"],r.properties["ev"],arg1,arg2)


def printPredicate(func,ev,argevs):

#	print "%template("+casperFn2PythonFn(func)+") Casper::"+func[0].lower()+func[1:]+"<",
#	print "Casper::Expr<"+argevs[0]+">",
#	for i in range(1,len(argevs)):
#		print  ",Casper::Expr<"+argevs[i]+">",
#	print ">;"
	
	print "Casper::Expr<"+ev+">"
	print casperFn2PythonFn(func)+"(Casper::Expr<"+argevs[0]+"> const& a0",
	for i in range(1,len(argevs)):
		print  ",Casper::Expr<"+argevs[i]+"> const& a"+str(i),
	print ")"
	print "{"
	print "\treturn Casper::rel<Casper::"+func+">(a0",
	for i in range(1,len(argevs)):
		print ",a"+str(i),
	print ");"	
	print "}" 

def printExprPredicates():
	for r in objs:
		if r.properties["type"]=="tRel" and \
			(r.properties["func"], r.properties["nArgs"]) not in casperOp2PythonOp.keys():			
			argevs = [r.properties["arg"+str(i)+"Eval"] for i in range(1,r.properties["nArgs"]+1)]
			printPredicate(r.properties["func"],r.properties["ev"],argevs)
	
def getCPModule(r):
	if r.properties["type"]=="tRel":
		argevs = [r.properties["arg"+str(i)+"Eval"] for i in range(1,r.properties["nArgs"]+1)]
		# if ev=set or any arg is a set, then put in set module
		if  r.properties["ev"]=="Casper::IntSet" or r.properties["ev"]=="Casper::BoolSet" or \
			r.properties["ev"]=="Casper::IntSetSeq" or r.properties["ev"]=="Casper::BoolSetSeq" or \
			any([ev=="Casper::IntSet" or ev=="Casper::BoolSet" for ev in argevs]) or \
			any([ev=="Casper::IntSetSeq" or ev=="Casper::BoolSetSeq" for ev in argevs]):
			return "set"
		# if is not a bool expression then put in evaluation module
		if r.properties["ev"]!="bool":
			return r.properties["ev"]
		# if any arg has double or float then put in real module
		if  r.properties["ev"]=="double" or r.properties["ev"]=="float" or \
			r.properties["ev"]=="DoubleSeq" or r.properties["ev"]=="FloatSeq" or \
			any([ev=="double" or ev=="float" for ev in argevs]) or \
			any([ev=="DoubleSeq" or ev=="FloatSeq" for ev in argevs]):
			return "real"
		#otherwise put it int module
		return "int"
	assert(0)
	

def forAllRelOper(fn,argfn):
	for r in objs:
		if r.properties["type"]=="tRel":			
			argevs = [r.properties["arg"+str(i)+"Eval"] for i in range(1,r.properties["nArgs"]+1)]
			func = r.properties["func"]
			if len(argevs)==1 and argfn(argevs[0])!=None:
				fn(r,"Rel1<Casper::"+func+","+argfn(argevs[0])+" >")
			elif len(argevs)==2:	
				if argfn(argevs[0])!=None:
					if argfn(argevs[1])!=None:
						fn(r,"Rel2<Casper::"+func+","+argfn(argevs[0])+","+argfn(argevs[1])+" >")
					fn(r,"Rel2<Casper::"+func+","+argfn(argevs[0])+",Expr<"+argevs[1]+"> >")
				if argfn(argevs[1])!=None:
					fn(r,"Rel2<Casper::"+func+",Expr<"+argevs[0]+">,"+argfn(argevs[1])+" >")

def forAllRelPred(fn):
	for r in objs:
		if r.properties["type"]=="tRel":			
			argevs = [r.properties["arg"+str(i)+"Eval"] for i in range(1,r.properties["nArgs"]+1)]
			func = r.properties["func"]
			s = "Rel"+str(len(argevs))+"<Casper::"+func
			for ev in argevs:
				s += ",Expr<"+ev+">"
			s += ">"
			fn(r,s)

def forAllRelOperList(fn,argfn):
	for r in objs:
		if r.properties["type"]=="tRel":			
			argevs = [r.properties["arg"+str(i)+"Eval"] for i in range(1,r.properties["nArgs"]+1)]
			func = r.properties["func"]
			if len(argevs)==1 and argfn(argevs[0])!=None:
				fn(r,func,[argfn(argevs[0])],argevs)
			elif len(argevs)==2:	
				if argfn(argevs[0])!=None:
					if argfn(argevs[1])!=None:
						fn(r,func,[argfn(argevs[0]),argfn(argevs[1])],argevs)
					fn(r,func,[argfn(argevs[0]),"Expr<"+argevs[1]+">"],argevs)
				if argfn(argevs[1])!=None:
					fn(r,func,["Expr<"+argevs[0]+">",argfn(argevs[1])],argevs)

def forAllRelPredList(fn):
	for r in objs:
		if r.properties["type"]=="tRel":			
			argevs = [r.properties["arg"+str(i)+"Eval"] for i in range(1,r.properties["nArgs"]+1)]
			func = r.properties["func"]
			s = []
			for ev in argevs:
				s.append("Expr<"+ev+">")
			fn(r,func,s,argevs)

def args(ev):
	s = []
	if ev=="bool":
		s.append('Goal')
	if ev not in seqTypes:
		s.append('CP::Var<'+ev+'>')
	if ev not in seqTypes and ev not in setTypes:	
		s.append("Ref<"+ev+">")
	s.append("Expr<"+ev+">")
	return s 

	# CP::Var<->Expr
	# CP::Ref<->Expr
	# Expr<->Expr
	# Goal<->Expr
	
			