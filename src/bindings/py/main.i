
%include <pyabc.i>

%module casper 
%{
#include <bindings/cpp/cpp.h>
#include <bindings/cpp/print.h>

#include <unordered_map>
#include <utility>

typedef const casperbind::cpp::Symbol* Key;
typedef std::pair<PyObject*,int> Data;
typedef std::unordered_map<Key,Data> Map;
Map sharedProxies;
			  
void insertRef(const casperbind::cpp::Symbol* pSymbol, PyObject* pPyObject)
{
	Map::iterator it = sharedProxies.find(pSymbol);
	if (it != sharedProxies.end()) 	// increment refs
	  	++it->second.second;
	else 							// insert in dict
	{
	 	sharedProxies[pSymbol] = std::make_pair(pPyObject,1);
	  	std::cout << "inserted ref to pyobj " << pPyObject << std::endl;
	  	Py_INCREF(pPyObject);
	}
}

void removeRef(const casperbind::cpp::Symbol* pSymbol)
{
    std::cout << "in removeref of symbol " << pSymbol << std::endl; 
	Map::iterator it = sharedProxies.find(pSymbol);
	assert(it != sharedProxies.end());
	std::cout << "cur refs = " << (it->second.second-1) << std::endl;
	if (--it->second.second == 0)
	{
		Py_DECREF(it->second.first);
		sharedProxies.erase(it);
	}
}

PyObject* getRef(const casperbind::cpp::Symbol* pSymbol)
{
	Map::const_iterator it = sharedProxies.find(pSymbol);
	assert(it != sharedProxies.end());
	Py_INCREF(it->second.first);
	return it->second.first;
}

struct SharedSymbolDeleter 
{
	void operator()(const casperbind::cpp::Symbol* pSymbol) const
	{	removeRef(pSymbol);	}
};

#define ASSERT_NUMBER(p) \
{\
	if (!PyNumber_Check(p))\
	{\
		PyErr_SetString(PyExc_TypeError,"expecting a number");\
		return NULL;\
	}\
}

%}
	
%define COUTWRAPPER(c)
	%extend c 
	{
		const char* __str__ ()
    	{
    		std::ostringstream s;
        	s << *$self;
        	static char ret[1024];
        	strcpy(ret,s.str().c_str());
        	return ret;
    	}
	};
%enddef

%{
casperbind::cpp::SharedSymbol	createSharedSymbol(PyObject* pObj)
{
	static swig_type_info * swig_SharedSymbol_ptr = SWIG_TypeQuery("casperbind::cpp::SharedSymbol*");
	static swig_type_info * swig_Symbol_ptr = SWIG_TypeQuery("casperbind::cpp::Symbol*");
	static swig_type_info * swig_PSymbol_ptr = SWIG_TypeQuery("casperbind::cpp::Symbol**");

	static swig_type_info * swig_Bool_ptr = SWIG_TypeQuery("casperbind::cpp::Bool*");
	static swig_type_info * swig_Int_ptr = SWIG_TypeQuery("casperbind::cpp::Int*");
	static swig_type_info * swig_BoolSet_ptr = SWIG_TypeQuery("casperbind::cpp::detail::Set<bool>*");
	static swig_type_info * swig_IntSet_ptr = SWIG_TypeQuery("casperbind::cpp::detail::Set<int>*");
	static swig_type_info * swig_IntRange_ptr = SWIG_TypeQuery("casperbind::cpp::detail::Range<int>*");
	static swig_type_info * swig_SymbolSet_ptr = SWIG_TypeQuery("casperbind::cpp::detail::Set<casperbind::cpp::SharedSymbol,casperbind::cpp::detail::LtSharedSymbol>*");
	
	void * argp = NULL;
	
	if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_SharedSymbol_ptr, 0))) 
		return casperbind::cpp::SharedSymbol(*static_cast<casperbind::cpp::SharedSymbol*>(argp));
	else
	if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_Symbol_ptr, 0))) 
	{
		insertRef(static_cast<casperbind::cpp::Symbol*>(argp),pObj);
		return casperbind::cpp::SharedSymbol(static_cast<casperbind::cpp::Symbol*>(argp),
											SharedSymbolDeleter());
	}
	else
	if (PyBool_Check(pObj))
	{
		casperbind::cpp::Bool* r = new casperbind::cpp::Bool(static_cast<bool>(PyInt_AsLong(pObj)));
		PyObject* pPyObj = SWIG_NewPointerObj(r, swig_Bool_ptr, 1);
		insertRef(r,pPyObj);
		return casperbind::cpp::SharedSymbol(r,SharedSymbolDeleter());							
	}
	else
	if (PyInt_Check(pObj))
	{
		casperbind::cpp::Int* r = new casperbind::cpp::Int(static_cast<int>(PyInt_AsLong(pObj)));
		PyObject* pPyObj = SWIG_NewPointerObj(r, swig_Int_ptr, 1);
		insertRef(r,pPyObj);
		return casperbind::cpp::SharedSymbol(r,SharedSymbolDeleter());							
	}
	else
	if (PySequence_Check(pObj))
	{
		int size = PySequence_Size(pObj);
		int i;
		
		// check for list of bools
		for (i = 0; i < size; i++)
			if (!PyBool_Check(PySequence_GetItem(pObj, i)))
				break;
		if (i==size)
		{
			casperbind::cpp::detail::Set<bool>* r = new casperbind::cpp::detail::Set<bool>();
			for (int i = 0; i < size; i++)
				r->add(static_cast<bool>(PyInt_AsLong(PySequence_GetItem(pObj, i))));
			PyObject* pPyObj = SWIG_NewPointerObj(r, swig_BoolSet_ptr, 1);
			insertRef(r,pPyObj);
			return casperbind::cpp::SharedSymbol(r,SharedSymbolDeleter());
		}

		// check for list of integers
		for (i = 0; i < size; i++)
			if (!PyInt_Check(PySequence_GetItem(pObj, i)))
				break;
		if (i==size)				
		{
			// check for contiguous range
			int last = static_cast<int>(PyInt_AsLong(PySequence_GetItem(pObj, 0)));
			for (i = 1; i < size; i++)
				if (last != static_cast<int>(PyInt_AsLong(PySequence_GetItem(pObj, i)))-1)
					break;
				else
					last = static_cast<int>(PyInt_AsLong(PySequence_GetItem(pObj, i)));
			if (i==size)	// store it as Range
			{
				casperbind::cpp::detail::Range<int>* r = 
						new casperbind::cpp::detail::Range<int>(
								static_cast<int>(PyInt_AsLong(PySequence_GetItem(pObj, 0))),
								static_cast<int>(PyInt_AsLong(PySequence_GetItem(pObj, size-1))));
				PyObject* pPyObj = SWIG_NewPointerObj(r, swig_IntRange_ptr, 1);
				insertRef(r,pPyObj);
				return casperbind::cpp::SharedSymbol(r,SharedSymbolDeleter());
			}
			else			// store it as Set
			{		
				casperbind::cpp::detail::Set<int>* r = new casperbind::cpp::detail::Set<int>();
				for (int i = 0; i < size; i++)
					r->add(static_cast<int>(PyInt_AsLong(PySequence_GetItem(pObj, i))));
				PyObject* pPyObj = SWIG_NewPointerObj(r, swig_IntSet_ptr, 1);
				insertRef(r,pPyObj);
				return casperbind::cpp::SharedSymbol(r,SharedSymbolDeleter());
			}
		}	

		// build heterogeneous set
		{
			casperbind::cpp::detail::Set<casperbind::cpp::SharedSymbol,casperbind::cpp::detail::LtSharedSymbol>* r =
				new casperbind::cpp::detail::Set<casperbind::cpp::SharedSymbol,casperbind::cpp::detail::LtSharedSymbol>();
			for (int i = 0; i < size; i++)
				r->add(createSharedSymbol(PySequence_GetItem(pObj, i)));
			PyObject* pPyObj = SWIG_NewPointerObj(r, swig_SymbolSet_ptr, 1);
			insertRef(r,pPyObj);
			return casperbind::cpp::SharedSymbol(r,SharedSymbolDeleter());			
		}
		std::cout << "bronca\n";
	}
//	else
//	if (SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_PSymbol_ptr, 0))) 
//		return casperbind::cpp::SharedSymbol(*static_cast<casperbind::cpp::Symbol**>(argp));
	else
	{
		std::cout << "unkown conversion in typemap\n";
		assert(0);
	}
}


int	checkSharedSymbol(PyObject* pObj)
{
	static swig_type_info * swig_SharedSymbol_ptr = SWIG_TypeQuery("casperbind::cpp::SharedSymbol*");
	static swig_type_info * swig_Symbol_ptr = SWIG_TypeQuery("casperbind::cpp::Symbol*");
	static swig_type_info * swig_PSymbol_ptr = SWIG_TypeQuery("casperbind::cpp::Symbol**");

	void * argp = NULL;
	
	return  PyBool_Check(pObj) or
			PyInt_Check(pObj) or
			PySequence_Check(pObj) or
			SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_SharedSymbol_ptr, 0)) or 
			SWIG_IsOK(SWIG_ConvertPtr(pObj, &argp, swig_Symbol_ptr, 0));
}

%}

%typemap(in) casperbind::cpp::SharedSymbol
{
	std::cout << "in in typemap\n";
	$1 = createSharedSymbol($input);
};

%typemap(typecheck) casperbind::cpp::SharedSymbol
{
	std::cout << "in in typecheck\n";
	$1 = checkSharedSymbol($input);
};

%typemap(out) (const casperbind::cpp::SharedSymbol&)
{
	std::cout << "in out typemap\n";
	$result = getRef(&$1->get());
};

%ignore Symbol;
%ignore LtSharedSymbol;
%include "bindings/cpp/symbol.h"
COUTWRAPPER(casperbind::cpp::SharedSymbol)

%ignore Container;
%include "bindings/cpp/container.h"

%include "bindings/cpp/detail.h"
COUTWRAPPER(casperbind::cpp::detail::Range)
COUTWRAPPER(casperbind::cpp::detail::Array)
COUTWRAPPER(casperbind::cpp::detail::Set)
%template(IntRange) 	casperbind::cpp::detail::Range<int>;
%template(SymbolRange) 	casperbind::cpp::detail::Range<casperbind::cpp::SharedSymbol>;
%template(IntArray) 	casperbind::cpp::detail::Array<int>;
%template(SymbolArray) 	casperbind::cpp::detail::Array<casperbind::cpp::SharedSymbol>;
%template(BoolArray) 	casperbind::cpp::detail::Array<bool>;
%template(IntSet) 		casperbind::cpp::detail::Set<int>;
%template(BoolSet) 		casperbind::cpp::detail::Set<bool>;
%template(SymbolSet) 	casperbind::cpp::detail::Set<casperbind::cpp::SharedSymbol,casperbind::cpp::detail::LtSharedSymbol>;

%include "bindings/cpp/variable.h"
COUTWRAPPER(casperbind::cpp::Variable)

%include "bindings/cpp/expression.h"
COUTWRAPPER(casperbind::cpp::Expression)
%template(Int) 			casperbind::cpp::detail::Constant<int,casperbind::cpp::Symbol::sInt>;
%template(Bool) 		casperbind::cpp::detail::Constant<bool,casperbind::cpp::Symbol::sBool>;
%template(Double) 		casperbind::cpp::detail::Constant<double,casperbind::cpp::Symbol::sDouble>;
%template(String) 		casperbind::cpp::detail::Constant<const char*,casperbind::cpp::Symbol::sString>;

%extend casperbind::cpp::detail::Constant<bool,casperbind::cpp::Symbol::sBool> 
{
	int __int__() { return $self->data; }
	%pythoncode
	{
		def __str__(self): return str(bool(self));
		def __bool__(self): return int(self) != 0
		def __lshift__(self, other): return int(self) << other
		def __rlshift__(self, other): return other << int(self)
		def __rshift__(self, other): return int(self) >> other
		def __rrshift__(self, other): return other >> int(self)
		def __and__(self, other): return bool(self) & other
		def __rand__(self, other): return other & bool(self)
		def __xor__(self, other): return bool(self) ^ other
		def __rxor__(self, other): return other ^ bool(self)
		def __or__(self, other): return bool(self) | other
		def __ror__(self, other): return other | bool(self)
		def __invert__(self): return ~bool(self)
		def __index__(self): return int(self)
		def __float__(self): return float(bool(self))
		def numerator(self): return +self
		def denominator(self): return 1

		def __trunc__(self): return int(self)
		def __floor__(self): return int(self)
		def __ceil__(self): return int(self)
		def __round__(self, ndigits): return int(self)
		def __divmod__(self, other): return (int(self) // other, int(self) % other)
		def __rdivmod__(self, other): return (other // int(self), other % int(self))
		def __mod__(self, other): return int(self) % other
		def __rmod__(self, other): return other % int(self)

		def __lt__(self, other): return int(self)<other
		def __le__(self, other): return int(self)<=other
		def __gt__(self, other): return int(self)>other
		def __ge__(self, other): return int(self)>=other

		def __complex__(self): return complex(float(self))

		def __add__(self, other): return int(self)+other
		def __radd__(self, other): return other+int(self)
		def __neg__(self): return -int(self)
		def __pos__(self): return +int(self)
		def __sub__(self, other): return int(self)-other
		def __rsub__(self, other): return other-int(self)
		def __mul__(self, other): return int(self)+other
		def __rmul__(self, other): return other+int(self)
		def __div__(self, other): return int(self)/other
		def __rdiv__(self, other): return other/int(self)
		def __pow__(self, exponent): return int(self)**exponent
		def __rpow__(self, base): return base ** int(self)
		def __abs__(self): return abs(int(self))		
	}
};

%extend casperbind::cpp::detail::Constant<int,casperbind::cpp::Symbol::sInt> 
{
	int __int__() { return $self->data; }
	
	%pythoncode
	{
		def __str__(self): return str(int(self));
	
		def __lshift__(self, other): return int(self) << other
		def __rlshift__(self, other): return other << int(self)
		def __rshift__(self, other): return int(self) >> other
		def __rrshift__(self, other): return other >> int(self)
		def __and__(self, other): return int(self) & other
		def __rand__(self, other): return other & int(self)
		def __xor__(self, other): return int(self) ^ other
		def __rxor__(self, other): return other ^ int(self)
		def __or__(self, other): return int(self) | other
		def __ror__(self, other): return other | int(self)
		def __invert__(self): return ~int(self)
		def __index__(self): return int(self)
		def __float__(self): return float(int(self))
		def numerator(self): return +self
		def denominator(self): return 1

		def __trunc__(self): return int(self)
		def __floor__(self): return int(self)
		def __ceil__(self): return int(self)
		def __round__(self, ndigits): return int(self)
		def __divmod__(self, other): return (int(self) // other, int(self) % other)
		def __rdivmod__(self, other): return (other // int(self), other % int(self))
		def __mod__(self, other): return int(self) % other
		def __rmod__(self, other): return other % int(self)

		def __lt__(self, other): return int(self)<other
		def __le__(self, other): return int(self)<=other
		def __gt__(self, other): return int(self)>other
		def __ge__(self, other): return int(self)>=other

		def __complex__(self): return complex(float(self))
		def __bool__(self): return int(self) != 0

		def __add__(self, other): return int(self)+other
		def __radd__(self, other): return other+int(self)
		def __neg__(self): return -int(self)
		def __pos__(self): return +int(self)
		def __sub__(self, other): return int(self)-other
		def __rsub__(self, other): return other-int(self)
		def __mul__(self, other): return int(self)+other
		def __rmul__(self, other): return other+int(self)
		def __div__(self, other): return int(self)/other
		def __rdiv__(self, other): return other/int(self)
		def __pow__(self, exponent): return int(self)**exponent
		def __rpow__(self, base): return base ** int(self)
		def __abs__(self): return abs(int(self))
	}
};

%extend casperbind::cpp::detail::Constant<double,casperbind::cpp::Symbol::sDouble> 
{
	double __float__() { return $self->data; }
	int __int__() { return static_cast<int>($self->data); }
	bool __bool__() { return static_cast<bool>($self->data); }
	
	%pythoncode
	{
		def __str__(self): return str(float(self));
		
		def __lshift__(self, other): return float(self) << other
		def __rlshift__(self, other): return other << float(self)
		def __rshift__(self, other): return float(self) >> other
		def __rrshift__(self, other): return other >> float(self)
		def __and__(self, other): return bool(self) & other
		def __rand__(self, other): return other & bool(self)
		def __xor__(self, other): return bool(self) ^ other
		def __rxor__(self, other): return other ^ bool(self)
		def __or__(self, other): return bool(self) | other
		def __ror__(self, other): return other | bool(self)
		def __invert__(self): return ~float(self)
		def __index__(self): return int(self)
		def numerator(self): return +self
		def denominator(self): return 1

		def __trunc__(self): return trunc(float(self))
		def __floor__(self): return floor(float(self))
		def __ceil__(self): return ceil(float(self))
		def __round__(self, ndigits): return round(float(self),ndigits)
		def __divmod__(self, other): return (float(self) // other, float(self) % other)
		def __rdivmod__(self, other): return (other // float(self), other % float(self))
		def __mod__(self, other): return float(self) % other
		def __rmod__(self, other): return other % float(self)

		def __lt__(self, other): return float(self)<other
		def __le__(self, other): return float(self)<=other
		def __gt__(self, other): return float(self)>other
		def __ge__(self, other): return float(self)>=other

		def __complex__(self): return complex(float(self))		

		def __add__(self, other): return float(self)+other
		def __radd__(self, other): return other+float(self)
		def __neg__(self): return -float(self)
		def __pos__(self): return +float(self)
		def __sub__(self, other): return float(self)-other
		def __rsub__(self, other): return other-float(self)
		def __mul__(self, other): return float(self)+other
		def __rmul__(self, other): return other+float(self)
		def __div__(self, other): return float(self)/other
		def __rdiv__(self, other): return other/float(self)
		def __pow__(self, exponent): return float(self)**exponent
		def __rpow__(self, base): return base ** float(self)
		def __abs__(self): return abs(float(self))
	}
};

%extend casperbind::cpp::detail::Range<int> 
{
	bool __contains__(int i) const { return i >= $self->getLower() and i <= $self->getUpper(); }
	%pythoncode
	{
		def __getitem__(self,i):
			if i+self.getLower()>self.getUpper():
				raise IndexError
			else:
				return self.getLower()+i
				
		def __len__(self):
			return self.getUpper()-self.getLower()+1
	}
};
