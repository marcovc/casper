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
 

#ifndef CASPER_KERNEL_OBJ_ARRAY_H_
#define CASPER_KERNEL_OBJ_ARRAY_H_

#include <casper/kernel/spexpr/expr.h>

#include <typeinfo>

namespace Casper {
namespace Detail {

template<class T,class Eval,int dims>
struct Create<T,Util::StdArray<Eval,dims> >
{
	const Util::StdArray<Eval,dims>& operator()(const T& t)
	{	throw Casper::Exception::TypeCoercion(typeid(T).name(),typeid(Util::StdArray<Eval,dims>).name());	}
};

template<class Eval,int dims>
struct Create<Util::StdArray<Eval,dims>,Util::StdArray<Eval,dims> >
{
	const Util::StdArray<Eval,dims>& operator()(const Util::StdArray<Eval,dims>& t)
	{	return t;	}
};

template<class Eval>
struct Create<Util::StdList<Expr<Eval> >,Util::StdArray<Eval,1> >
{
	Util::StdArray<Eval,1> operator()(const Util::StdList<Expr<Eval> >& t)
	{
		Util::StdArray<Eval,1> r(std::distance(t.begin(),t.end()));
		int i = 0;
		for (auto it = t.begin(); it != t.end(); ++it)
			r[i++] = it->toLiteral();
		return r;
	}
};

template<class Eval,int dims>
struct Create<Util::StdArray<Expr<Eval>,dims>,Util::StdArray<Expr<Eval>,dims> >
{
	const Util::StdArray<Eval,dims>& operator()(const Util::StdArray<Expr<Eval>,dims>& t)
	{	return t; }
};

} // Detail

namespace CP {

// TMP: don't know why automatic conversion fails on this. Workaround:
template<class Eval,class View>
Filter gacSchema(Store& store,const View& v,const Expr<Seq<Eval> >& expr)
{	return gacSchema(store,v,expr.toLitStdArray());	}


} // CP

}

#endif /* CASPER_KERNEL_OBJ_BNDEXPR_H_ */
