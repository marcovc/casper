
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

# TODO later
#				"fWhileNotGround",
#				"fIfThen","fIfThenElse","fInRange",
#				"fAll","fMaxDiff","fArgMin","fArgMax",
#				"fForSome","fAssign","fSelectFirst",,"fRandInRange",
#				"fTryAll","fForAll","fWhileDo"]:



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
		"Cast<bool>" : "asBool"						
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
				'''
				# self with literal
				if ev2=="int" or ev2=="bool":
					arg2 = ev2
					printBinaryOperator(r.properties["func"],r.properties["ev"],arg1,arg2)
				# literal with self		
					printBinaryOperator(r.properties["func"],r.properties["ev"],arg1,arg2,True)
				# self with var
				arg2 = "Casper::CP::Var<"+ev2+",Casper::CP::Traits::GetDefaultDom<"+ev2+">::Type>"
				printBinaryOperator(r.properties["func"],r.properties["ev"],arg1,arg2)
				'''
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
				'''
				# expr with literal
				if ev2=="int" or ev2=="bool":
					arg2 = ev2
					printBinaryOperator(r.properties["func"],r.properties["ev"],arg1,arg2)
				# literal with expr		
					printBinaryOperator(r.properties["func"],r.properties["ev"],arg1,arg2,True)
				'''
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
			