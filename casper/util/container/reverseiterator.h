/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#ifndef CASPER_UTIL_CONTAINER_REVERSEITERATOR_H_
#define CASPER_UTIL_CONTAINER_REVERSEITERATOR_H_

namespace Casper {
namespace Util  {


template<typename Iterator>
struct  ReverseIteratorAdaptor
{
protected:
  Iterator current;


public:
  typedef typename Iterator::Value	Value;
  typedef typename Iterator::Difference	Difference;


//  typedef Iterator					iterator_type;
//  typedef typename Iterator::difference_type	difference_type;
//  typedef typename Iterator::pointer		pointer;
//  typedef typename __traits_type::reference		reference;

  ReverseIteratorAdaptor() : current() { }

  explicit
  ReverseIteratorAdaptor(Iterator it) : current(it) {}

  ReverseIteratorAdaptor(const ReverseIteratorAdaptor& it) : current(it.current) {}

  template<typename Iter>
  ReverseIteratorAdaptor(const ReverseIteratorAdaptor<Iter>& it) : current(it.base()) {}

  Iterator  base() const  { return current; }

  bool operator==(const ReverseIteratorAdaptor& s) const
  {	return current==s.current;}
  bool operator!=(const ReverseIteratorAdaptor& s) const
  {	return current!=s.current;}

  Value  operator*() const
  {
	  Iterator tmp = current;
	  return *--tmp;
  }

  ReverseIteratorAdaptor&  operator++()
  {
	  --current;
	  return *this;
  }

  ReverseIteratorAdaptor  operator++(int)
  {
	  ReverseIteratorAdaptor tmp = *this;
	  --current;
	  return tmp;
  }

  ReverseIteratorAdaptor&  operator--()
  {
	  ++current;
	  return *this;
  }

  ReverseIteratorAdaptor  operator--(int)
  {
	  ReverseIteratorAdaptor tmp = *this;
	  ++current;
	  return tmp;
  }

  ReverseIteratorAdaptor  operator+(Difference n) const
  {		return ReverseIteratorAdaptor(current - n); }

  ReverseIteratorAdaptor&  operator+=(Difference n)
  {
	  current -= n;
	  return *this;
  }

  ReverseIteratorAdaptor  operator-(Difference n) const
  { 	return ReverseIteratorAdaptor(current + n); }

  ReverseIteratorAdaptor&  operator-=(Difference n)
  {
		current += n;
		return *this;
  }

  Value  operator[](Difference n) const
  { return *(*this + n); }
};

}
}

#endif /* CASPER_UTIL_CONTAINER_REVERSEITERATOR_H_ */
