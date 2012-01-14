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

/*
%template()	Casper::Rel2<Casper::Element,Casper::Util::StdArray<Casper::CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type>,3>,Casper::Expr<int> >;
%extend Casper::Rel2<Casper::Element,Casper::Util::StdArray<Casper::CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type>,3>,Casper::Expr<int> >
{
	%rename(__getitem__) operator[](const Casper::Expr<int>&);
}
*/

%template(IntVarArray) Casper::CP::VarArray<int,1,Casper::CP::Traits::GetDefaultDom<int>::Type>;
%template(BoolVarArray) Casper::CP::VarArray<bool,1,Casper::CP::Traits::GetDefaultDom<bool>::Type>;
%template(IntVarArray2) Casper::CP::VarArray<int,2,Casper::CP::Traits::GetDefaultDom<int>::Type>;
%template(BoolVarArray2) Casper::CP::VarArray<bool,2,Casper::CP::Traits::GetDefaultDom<bool>::Type>;
%template(IntVarArray3) Casper::CP::VarArray<int,3,Casper::CP::Traits::GetDefaultDom<int>::Type>;
%template(BoolVarArray3) Casper::CP::VarArray<bool,3,Casper::CP::Traits::GetDefaultDom<bool>::Type>;


%extend Casper::CP::VarArray<int,1,Casper::CP::Traits::GetDefaultDom<int>::Type> {
	%template(IntVarArray) VarArray<int>;
	%template(IntVarArray) VarArray<int,int>;
/*	
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
*/		
	void 
	__setitem__(int i,const Casper::CP::Var<int,Casper::CP::Traits::GetDefaultDom<int>::Type>& v) 
	{ $self->operator[](i) = v; }
	
	
}


%extend Casper::CP::VarArray<bool,1,Casper::CP::Traits::GetDefaultDom<bool>::Type> {
	%template(BoolVarArray) VarArray<bool>;
	%template(BoolVarArray) VarArray<bool,bool>;
	
	const Casper::CP::Var<bool,Casper::CP::Traits::GetDefaultDom<bool>::Type>& 
	getitemi(int i) const { return $self->operator[](i); }

	Casper::Expr<bool>
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
	__setitem__(int i,const Casper::CP::Var<bool,Casper::CP::Traits::GetDefaultDom<bool>::Type>& v) 
	{ $self->operator[](i) = v; }
}

%extend Casper::CP::VarArray<int,2,Casper::CP::Traits::GetDefaultDom<int>::Type> {
	%template(IntVarArray2) VarArray<uint,int>;
	%template(IntVarArray2) VarArray<uint,int,int>;

	Casper::CP::VarArray<int,1,Casper::CP::Traits::GetDefaultDom<int>::Type> 
	getitemi(int i) const 
	{ 
		Casper::CP::VarArray<int,1,Casper::CP::Traits::GetDefaultDom<int>::Type> r($self->getStore(),$self->operator[](i));
		return r; 
	}

	Casper::Expr<Casper::Seq<int> >
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
	__setitem__(int i,const Casper::CP::VarArray<int,1,Casper::CP::Traits::GetDefaultDom<int>::Type>& v) 
	{ $self->operator[](i) = v; }
	
}
%extend Casper::CP::VarArray<bool,2,Casper::CP::Traits::GetDefaultDom<bool>::Type> {
	%template(BoolVarArray2) VarArray<uint,bool>;
	%template(BoolVarArray2) VarArray<uint,bool,bool>;
	
	Casper::CP::VarArray<bool,1,Casper::CP::Traits::GetDefaultDom<bool>::Type> 
	getitemi(int i) const 
	{ 
		Casper::CP::VarArray<bool,1,Casper::CP::Traits::GetDefaultDom<bool>::Type> r($self->getStore(),$self->operator[](i));
		return r; 
	}

	Casper::Expr<Casper::Seq<bool> >
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
	__setitem__(int i,const Casper::CP::VarArray<bool,1,Casper::CP::Traits::GetDefaultDom<bool>::Type>& v) 
	{ $self->operator[](i) = v; }
	
}

%extend Casper::CP::VarArray<int,3,Casper::CP::Traits::GetDefaultDom<int>::Type> {
	%template(IntVarArray3) VarArray<uint,uint,int>;
	%template(IntVarArray3) VarArray<uint,uint,int,int>;
	
	Casper::CP::VarArray<int,2,Casper::CP::Traits::GetDefaultDom<int>::Type> 
	getitemi(int i) const 
	{ 
		Casper::CP::VarArray<int,2,Casper::CP::Traits::GetDefaultDom<int>::Type> r($self->getStore(),$self->operator[](i));
		return r; 
	}

	Casper::Expr<Casper::Seq<int> >
	getiteme(const Casper::Expr<int>& e) const 
	{ 
		return $self->operator[](e); 
	}
	
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
	__setitem__(int i,const Casper::CP::VarArray<int,2,Casper::CP::Traits::GetDefaultDom<int>::Type>& v) 
	{ $self->operator[](i) = v; }
	
}
%extend Casper::CP::VarArray<bool,3,Casper::CP::Traits::GetDefaultDom<bool>::Type> {
	%template(BoolVarArray3) VarArray<uint,uint,bool>;
	%template(BoolVarArray3) VarArray<uint,uint,bool,bool>;
	
	Casper::CP::VarArray<bool,2,Casper::CP::Traits::GetDefaultDom<bool>::Type> 
	getitemi(int i) const 
	{ 
		Casper::CP::VarArray<bool,2,Casper::CP::Traits::GetDefaultDom<bool>::Type> r($self->getStore(),$self->operator[](i));
		return r; 
	}

	Casper::Expr<Casper::Seq<bool> >
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
	__setitem__(int i,const Casper::CP::VarArray<bool,2,Casper::CP::Traits::GetDefaultDom<bool>::Type>& v) 
	{ $self->operator[](i) = v; }
	
}

