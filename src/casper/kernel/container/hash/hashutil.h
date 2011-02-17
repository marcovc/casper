#ifndef _H_CASPER_HASHUTIL_
#define _H_CASPER_HASHUTIL_

#include <casper/kernel.h>

#include <string>
#include <list>

struct HashUtil
{
public:
	static const unsigned int SEED = 23;
	
	static unsigned int hash(unsigned int aSeed, bool aBoolean)
	{
		return aSeed + (aBoolean? 1 : 0);
	}
	
	static unsigned int hash(unsigned int aSeed, char aChar)
	{
		return aSeed + (int)aChar;
	}
	
	static unsigned int hash(unsigned int aSeed, int aInt)
	{
		return aSeed + aInt;
	}
	
	static unsigned int hash(unsigned int aSeed, long aLong)
	{
		return aSeed + (int)(aLong xor (aLong >> 16));
	}
	
	static unsigned int hash(unsigned int aSeed, float aFloat)
	{
		return hash(aSeed, (int)aFloat);
	}
	
	static unsigned int hash(unsigned int aSeed, double aDouble)
	{
		return hash(aSeed, (long)aDouble);
	}
	
	static unsigned int hash(unsigned int aSeed, string aString)
	{
		unsigned int result = aSeed;

		int length = aString.length();
		
		for (int i = 0; i < length; ++i)
		{
			result = hash(result, aString.at(i));
		}
		
		return result;
	}
	
	template <class T1, class T2> static unsigned int hash(unsigned int aSeed, std::pair< T1, T2 > aPair)
	{
		unsigned int result = aSeed;
		
		result = hash(result, aPair.first);
		result = hash(result, aPair.second);
		
		return result;
	}
	
	template <class T1, class T2> static unsigned int hash(unsigned int aSeed, casper::detail::Pair< T1, T2 > aPair)
	{
		unsigned int result = aSeed;
			
		result = hash(result, aPair.first);
		result = hash(result, aPair.second);
			
		return result;
	}
	
	template <class T> static unsigned int hash(unsigned int aSeed, std::list<T> aList)
	{
		return hash(aSeed, aList.begin(), aList.end());
	}
	
	template <class InputIterator> static unsigned int hash(unsigned int aSeed, InputIterator begin, InputIterator end)
	{	
		unsigned int result = aSeed;
				
		for (InputIterator it = begin; it != end; ++it)
		{
			result = hash(result, *it);
		}
				
		return result;
	}
	
private:
	static const unsigned int PRIME_NUMBER = 37;

	static unsigned int firstTerm(int aSeed)
	{
		return PRIME_NUMBER * aSeed;
	}
};

#endif /*_H_CASPER_HASHUTIL_*/
