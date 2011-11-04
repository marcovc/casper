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
	%template(IntVarArray2) VarArray<uint,int>;
	%template(IntVarArray2) VarArray<uint,int,int>;
	
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
	%template(BoolVarArray2) VarArray<uint,bool>;
	%template(BoolVarArray2) VarArray<uint,bool,bool>;
	
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