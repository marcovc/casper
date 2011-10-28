
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
        	static char* ret = new char[s.rdbuf()->str().size()];
        	strcpy(ret,s.str().c_str());
        	return ret;
    	}
	};
%enddef

