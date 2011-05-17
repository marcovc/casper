 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
 *                                                                         *
 *   Licensed under the Apache License, Version 2.0 (the "License");       *
 *   you may not use this file except in compliance with the License.      *
 *   You may obtain a copy of the License at                               *
 *            http://www.apache.org/licenses/LICENSE-2.0                   *
 *   Unless required by applicable law or agreed to in writing, software   *
 *   distributed under the License is distributed on an                    *
 *   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,          *
 *   either express or implied.                                            *
 *   See the License for the specific language governing permissions and   *
 *   limitations under the License.                                        *
 \*************************************************************************/

/**
 * \file
 * Bit array sets.
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_BITSETA
#define _H_CASPER_KERNEL_CONTAINER_BITSETA

#include <casper/util/container/sciterator.h>
#include <casper/util/container/stdpair.h>
#include <casper/kernel/reversible/array.h>


namespace Casper {
namespace Detail {


//using namespace std;

 /**
 	This class works as an adaptor to stl::vector<bool>, in order
	to make it a model of the Unique Sorted Associative Container
	and Simple Associative Container (as defined in the STL). In
	other words, implements the \a set<int> interface in an bit array:
	each element is unique and all elements are sorted according to the
	\a less<int> function. The \a Alloc template parameter is ignored.
	\arg size that denotes the max number of elements of the container.
	\arg type defines the element to cast elements of this container
	(must be an integer, eg. int,short,unsigned char,etc).
	\note requirement (outdated?) : sizeof( \a type ) > \a size + 1
 */
template<typename T, int maxSize, class Alloc = std::allocator<bool> >
class BitsetA : private Vector<bool>
{
	public:

//	typedef mybitset<Size>	Super;
	typedef Vector<bool> Super;

	// required from Container model
	typedef T 				Value;
	typedef int 			InternalValue;
	typedef const Value& 	Reference;
	typedef Reference 		ConstReference;
	typedef const Value* 	Pointer;
	typedef const Pointer 	ConstPointer;
	typedef long 			Difference;
	typedef unsigned long 	Size;

	Reversible<Size> 			count;
	State&						state;

	// required from Associative Container model
	typedef Value Key; // the key and the value are the same

	typedef	Util::SCIterator<InternalValue,Difference>	scit;

	class Iterator;
	//friend class Iterator;
	class Iterator : public std::iterator<std::bidirectional_iterator_tag,
									 	  const InternalValue,
									 	  Difference>
	{
		friend class BitsetA;
		public:
		typedef typename BitsetA::Value Value;
		typedef typename BitsetA::InternalValue InternalValue;
		//typedef typename BitsetA::Reference Reference;
		//typedef typename BitsetA::Pointer Pointer;

		bool operator==(const Iterator& s) const;
		bool operator!=(const Iterator& s) const;
		Value operator*() const
		{assert(curVal<maxSize); return static_cast<Value>(curVal);}
		Value* operator->() const
		{assert(curVal<maxSize); return static_cast<Value*>(&curVal);}
		Iterator& operator++(); // pre-increment
		Iterator operator++(int); //post-increment
		Iterator() {}
		Iterator(const Iterator& i) : pOwner(i.pOwner),curVal(i.curVal) {}
		Iterator(const BitsetA* pOwner, InternalValue i) :
												pOwner(pOwner),curVal(i) {}
		const Iterator& operator=(const Iterator& i)
			{pOwner = i.pOwner; curVal = i.curVal; return *this;}
		Iterator& operator--(); // pre-decrement
		Iterator operator--(int); //post-decrement
		private:
		const BitsetA*	pOwner;
		InternalValue	curVal;
	};

	typedef Iterator ConstIterator;

	BitsetA(State& state) : Super(state,maxSize,false),
							  count(state,0),state(state) { }
	BitsetA(State& state, int first, int last) :
							  Super(state,maxSize,false),
							  count(state,0),state(state)
	{ insert(scit(first),++scit(last)); }
	template <class InputIterator>
	BitsetA(State& state,InputIterator first, InputIterator last);
	BitsetA(const BitsetA& s) : Super(s),
								count(s.count),state(s.state) {}
	const BitsetA<T,maxSize,Alloc>& operator=(const BitsetA<T,maxSize,Alloc>& s)
	{	count = s.count; ((Super&)*this) = s; return *this;	}
	virtual ~BitsetA() {}

	inline Iterator begin() const;
	inline Iterator end() const;
	inline Size size() const;
	inline bool empty() const;
	inline void clear();

	inline Iterator find(const Key& k) const;
	inline Util::StdPair<Iterator, Iterator> equal_range(const Key& k) const;
	inline Iterator lowerBound(const Key& k) const;
	inline Iterator upperBound(const Key& k) const;
	inline Util::StdPair<Iterator,bool> insert(ConstReference);
	inline Iterator insert(Iterator hint, ConstReference value);
	inline Size erase(const Key& k);
	inline virtual bool erase(Iterator p, Iterator q);
	inline virtual bool erase(Iterator p);
	template <class InputIterator>
	inline void insert(InputIterator first, InputIterator last);
	inline void insert(scit first, scit last);
	void updateMin(Value v);
	void updateMax(Value v);
	void updateMin(Iterator it) {	updateMin(*it);	}
	void updateMax(Iterator it) {	updateMax(*it);	}

//	inline void insert(Iterator first, Iterator last);
	typedef std::reverse_iterator<Iterator> ReverseIterator;
	typedef ReverseIterator ConstReverseIterator;
	ReverseIterator rbegin() const {return ReverseIterator(end());}
	ReverseIterator rend() const {return ReverseIterator(begin());}
	State&	getState() const	{ return state; }

	bool testInternRep() const
	{
		unsigned int i = 0;
		for (int n = 0; n < maxSize; n++)
			i += (*this)[n];
		return count == i;
	}
};

// *********** ITERATOR **************

/// \todo maybe a check for curval < begin is missing in the 2 following methods
template<class T, int maxSize,class AllocT>
bool BitsetA<T,maxSize,AllocT>::Iterator::operator==(const Iterator& s) const
{
	return curVal == s.curVal;
}

template<class T, int maxSize,class AllocT>
bool BitsetA<T,maxSize,AllocT>::Iterator::operator!=(const Iterator& s) const
{
	return curVal != s.curVal;
}

template<class T, int maxSize,class AllocT>
typename BitsetA<T,maxSize,AllocT>::Iterator&
BitsetA<T,maxSize,AllocT>::Iterator::operator++() // pre-increment
{
	//while (!(*pOwner)[++curVal] && curVal<maxSize)
	while (++curVal<maxSize and !(*pOwner)[curVal])
		;
	return *this;
}

template<class T, int maxSize,class AllocT>
typename BitsetA<T,maxSize,AllocT>::Iterator
BitsetA<T,maxSize,AllocT>::Iterator::operator++(int) //post-increment
{
	Iterator ret(*this);
	while (!(*pOwner)[++curVal] && curVal<maxSize)
	;
	return ret;
}

template<class T, int maxSize,class AllocT>
typename BitsetA<T,maxSize,AllocT>::Iterator&
BitsetA<T,maxSize,AllocT>::Iterator::operator--() // pre-decrement
{
	while (!(*pOwner)[--(curVal)])
	;
	return *this;
}

template<class T, int maxSize,class AllocT>
typename BitsetA<T,maxSize,AllocT>::Iterator
BitsetA<T,maxSize,AllocT>::Iterator::operator--(int) //post-decrement
{
	Iterator ret(*this);
	while (!(*pOwner)[--curVal])
	;
	return ret;
}

// *********** CONTAINER **************

template<class T, int maxSize,class AllocT>
template <class InputIterator>
BitsetA<T,maxSize,AllocT>::BitsetA(State& state,
	InputIterator f, InputIterator l) :
	 Super(state,maxSize,false),count(state,0),state(state)
{ insert(f,l); }

template<class T, int maxSize,class AllocT>
typename BitsetA<T,maxSize,AllocT>::Iterator
BitsetA<T,maxSize,AllocT>::find(const Key& k) const
{
	if (k<maxSize && (*this)[k])
		return Iterator(this,k);
	else
		return end();
}

template<class T, int maxSize,class AllocT>
typename BitsetA<T,maxSize,AllocT>::Iterator
BitsetA<T,maxSize,AllocT>::lowerBound(const Key& k) const
{
	Iterator ret(this,k);
	while (ret.curVal<maxSize && !(*this)[ret.curVal])
		++ret.curVal;
	return ret;
}

template<class T, int maxSize,class AllocT>
Util::StdPair<typename BitsetA<T,maxSize,AllocT>::Iterator,
	 typename BitsetA<T,maxSize,AllocT>::Iterator>
BitsetA<T,maxSize,AllocT>::equal_range(const Key& k) const
{
	typename BitsetA<T,maxSize,AllocT>::Iterator it(find(k));
	return Util::StdPair<typename BitsetA<T,maxSize,AllocT>::Iterator,
	 			typename BitsetA<T,maxSize,AllocT>::Iterator>(it,it);
}

template<class T, int maxSize,class AllocT>
typename BitsetA<T,maxSize,AllocT>::Iterator
BitsetA<T,maxSize,AllocT>::upperBound(const Key& k) const
{
	Iterator ret(this,k+1);
	while (ret.curVal<maxSize && !(*this)[ret.curVal])
		++ret.curVal;
	return ret;
}

template<class T, int maxSize,class AllocT>
Util::StdPair<typename BitsetA<T,maxSize,AllocT>::Iterator,bool>
BitsetA<T,maxSize,AllocT>::insert(ConstReference k)
{
	if ((*this)[k])
		return Util::StdPair<typename BitsetA<T,maxSize,AllocT>::Iterator,
					bool>(Iterator(this,k),false);
	else
	{
		count = count + 1;
		((*this)[k]) = true;
		return Util::StdPair<typename BitsetA<T,maxSize,AllocT>::Iterator,
					bool>(Iterator(this,k),true);
	}
}

template<class T, int maxSize,class AllocT>
typename BitsetA<T,maxSize,AllocT>::Iterator
BitsetA<T,maxSize,AllocT>::insert(Iterator hint, ConstReference value)
{
	if (!(*this)[value])
	{
		count = count + 1;
		((*this)[value]) = true;
	}
	return typename BitsetA<T,maxSize,AllocT>::Iterator(this,value);
}

template<class T, int maxSize,class AllocT>
typename BitsetA<T,maxSize,AllocT>::Size
BitsetA<T,maxSize,AllocT>::erase(const Key& k)
{
	if ((*this)[k])
	{
		count = count - 1;
		((*this)[k]) = false;
		return 1;
	}
	else
		return 0;
}

template<class T, int maxSize,class AllocT>
bool BitsetA<T,maxSize,AllocT>::erase(Iterator p, Iterator q)
{
	InternalValue diff(0);
	for (InternalValue n = p.curVal; n < q.curVal; n++)
		if ((*this)[n])
		{
			diff++;
			((*this)[n]) = false;
		}
	count = count - static_cast<Size>(diff);
	return !empty();
}

template<class T, int maxSize,class AllocT>
void BitsetA<T,maxSize,AllocT>::updateMin(Value v)
{
	InternalValue diff(0);
	for (InternalValue n = 0; n < v; n++)
		if ((*this)[n])
		{
			diff++;
			((*this)[n]) = false;
		}
	count = count - diff;
	return;
}

template<class T, int maxSize,class AllocT>
void BitsetA<T,maxSize,AllocT>::updateMax(Value v)
{
	InternalValue diff(0);
	for (InternalValue n = maxSize-1; n > v; n--)
		if ((*this)[n])
		{
			diff++;
			((*this)[n]) = false;
		}
	count = count - diff;
	return;
}

template<class T, int maxSize,class AllocT>
bool BitsetA<T,maxSize,AllocT>::erase(Iterator p)
{
	count = count - static_cast<Size>(1);
	((*this)[p.curVal]) = false;
	return !empty();
}

template<class T, int maxSize,class AllocT>
template <class InputIterator>
void BitsetA<T,maxSize,AllocT>::insert(InputIterator first, InputIterator last)
{
	InternalValue diff(0);
	while (first != last)
	{
		if (!(*this)[*first])
		{
			((*this)[*first]) = true;
			diff++;
		}
		++first;
	}
	count = count + static_cast<Size>(diff);
}

template<class T, int maxSize,class AllocT>
void BitsetA<T,maxSize,AllocT>::insert(scit first, scit last)
{
	InternalValue diff(0);
	while (first != last)
	{
		if (!(*this)[*first])
		{
			((*this)[*first]) = true;
			++diff;
		}
		++first;
	}
	count = count + diff;
}

template<class T, int maxSize,class AllocT>
typename BitsetA<T,maxSize,AllocT>::Iterator
BitsetA<T,maxSize,AllocT>::begin() const
{
	uint i = 0;
	while (i < maxSize && !(*this)[i])
		++i;
	return Iterator(this,i);
}

template<class T, int maxSize,class AllocT>
typename BitsetA<T,maxSize,AllocT>::Iterator
BitsetA<T,maxSize,AllocT>::end() const
{
	return Iterator(this,maxSize);
}

template<class T, int maxSize,class AllocT>
typename BitsetA<T,maxSize,AllocT>::Size
BitsetA<T,maxSize,AllocT>::size() const
{
	return count;
}

template<class T, int maxSize,class AllocT>
bool BitsetA<T,maxSize,AllocT>::empty() const
{ return count == static_cast<Size>(0);}

template<class T, int maxSize,class AllocT>
void BitsetA<T,maxSize,AllocT>::clear()
{
	for (T n = 0; n < maxSize; n++)
		(*this)[n] = false;
	count = 0;
}

template<int>
struct Bitset;

template<class T,int Size,class Container,class CompareT>
struct SelectElement<Bitset<Size>,T,Container,CompareT>
{ typedef BitsetA<T,Size> Type; };

} // Detail
} // Casper


/**
	Writes the contents of the bitset to an ostream.
*/
template<class T, int maxSize,class AllocT>
std::ostream& operator<<(std::ostream& o, const Casper::Detail::BitsetA<T,maxSize,AllocT>& a)
{
	o << "{";
	for (typename Casper::Detail::BitsetA<T,maxSize,AllocT>::Iterator it = a.begin();
		it != a.end(); ++it)
			o << " " << (int)*it;
#ifdef C_DEBUG_ALL
	o << ",#" << a.count;
#endif
	o << " }";
	return o;
}


#endif // _H_CASPER_KERNEL_CONTAINER_BITSETA
