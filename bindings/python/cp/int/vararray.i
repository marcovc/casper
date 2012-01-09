%module cp 

%{
#include <casper/kernel.h>
#include <casper/cp.h>
%}

%import <swigutil.i>
%import <kernel.i>
%import <util.i>

%import <cp/vararray.i>

%import <util/stdarray.i>

%template(IntVarArray) Casper::CP::VarArray<int,1,Casper::CP::Traits::GetDefaultDom<int>::Type>;
%template(BoolVarArray) Casper::CP::VarArray<bool,1,Casper::CP::Traits::GetDefaultDom<bool>::Type>;
%template(IntVarArray2) Casper::CP::VarArray<int,2,Casper::CP::Traits::GetDefaultDom<int>::Type>;
%template(BoolVarArray2) Casper::CP::VarArray<bool,2,Casper::CP::Traits::GetDefaultDom<bool>::Type>;


%extend Casper::CP::VarArray<int,1,Casper::CP::Traits::GetDefaultDom<int>::Type> {
	%template(IntVarArray) VarArray<int>;
	%template(IntVarArray) VarArray<int,int>;
	
	const Casper::CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type>& 
	getitemi(int i) const { return $self->operator[](i); }

	Casper::Expr<int>
	getiteme(const Casper::Expr<int>& e) const { return $self->operator[](e); }
	
	%pythoncode
	{
		def __getitem__(self,i):
			import numbers
			if not isinstance(i,numbers.Number):
				return self.getiteme(i)
			elif i>=self.size() or i<0:
				raise IndexError
			else:
				return self.getitemi(i)
	}
		
	void 
	__setitem__(int i,const Casper::CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type>& v) 
	{ $self->operator[](i) = v; }
	
	
}
%extend Casper::CP::VarArray<bool,1,Casper::CP::Traits::GetDefaultDom<bool>::Type> {
	%template(BoolVarArray) VarArray<bool>;
	%template(BoolVarArray) VarArray<bool,bool>;
	
	const Casper::CP::Var<bool,Casper::CP::Traits::GetDefaultDom<bool>::Type>& 
	getitem(int i) const { return $self->operator[](i); }
	
	%pythoncode
	{
		def __getitem__(self,i):
			if i>=self.size() or i<0:
				raise IndexError
			else:
				return self.getitem(i)
	}
	
	void 
	__setitem__(int i,const Casper::CP::Var<bool,Casper::CP::Traits::GetDefaultDom<bool>::Type>& v) 
	{ $self->operator[](i) = v; }
}

%extend Casper::CP::VarArray<int,2,Casper::CP::Traits::GetDefaultDom<int>::Type> {
	%template(IntVarArray2) VarArray<unsigned int,int>;
	%template(IntVarArray2) VarArray<unsigned int,int,int>;
	
	Casper::CP::VarArray<int,1,Casper::CP::Traits::GetDefaultDom<int>::Type> 
	getitem(int i) const 
	{ return Casper::CP::VarArray<int,1,Casper::CP::Traits::GetDefaultDom<int>::Type>($self->getStore(),$self->operator[](i)); }
	
	%pythoncode
	{
		def __getitem__(self,i):
			if i>=self.size() or i<0:
				raise IndexError
			else:
				return self.getitem(i)
	}
	void 
	__setitem__(int i,const Casper::CP::VarArray<int,1,Casper::CP::Traits::GetDefaultDom<int>::Type>& v) 
	{ $self->operator[](i) = v; }
	
}
%extend Casper::CP::VarArray<bool,2,Casper::CP::Traits::GetDefaultDom<bool>::Type> {
	%template(BoolVarArray2) VarArray<unsigned int,bool>;
	%template(BoolVarArray2) VarArray<unsigned int,bool,bool>;
	
	Casper::CP::VarArray<bool,1,Casper::CP::Traits::GetDefaultDom<bool>::Type> 
	getitem(int i) const 
	{ return Casper::CP::VarArray<bool,1,Casper::CP::Traits::GetDefaultDom<bool>::Type>($self->getStore(),$self->operator[](i)); }
	
	%pythoncode
	{
		def __getitem__(self,i):
			if i>=self.size() or i<0:
				raise IndexError
			else:
				return self.getitem(i)
	}
	
	void 
	__setitem__(int i,const Casper::CP::VarArray<bool,1,Casper::CP::Traits::GetDefaultDom<bool>::Type>& v) 
	{ $self->operator[](i) = v; }
	
}
