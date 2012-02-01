
// global ignores
%ignore operator<<;
%ignore Casper::Detail;

%define COUTWRAPPER(c)
	%extend c 
	{
		const char* __str__ ()
    	{
    		std::ostringstream s;
        	s << *$self;
        	//static char* ret = new char[s.rdbuf()->str().size()];
        	char* ret = new char[s.str().size()+1];
        	strcpy(ret,s.str().c_str());
        	return ret;
    	}
	};
%enddef


%define ITERATOR_WRAPPER(ItName,Class,Container,It,Elem,Call)
%template(ItName) Casper::Util::Iterator<Container,It,Elem>;
%extend Class 
{
	Casper::Util::Iterator<Container,It,Elem> __iter__() 
	{	return Casper::Util::Iterator<Container,It,Elem>(Call);	}
}
%enddef
