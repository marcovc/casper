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
 

#ifndef CASPER_KERNEL_OBJ_REF_H_
#define CASPER_KERNEL_OBJ_REF_H_

namespace Casper {
namespace Detail {


template<class T,class Eval>
struct Create<T,Ref<Eval> >
{
	Ref<Eval> operator()(Casper::State& state, const T& t);
};

template<class T,class Eval>
Ref<Eval> Create<T,Ref<Eval> >::operator()(Casper::State& state, const T& t)
{	return Ref<Eval>(state,t);	}

//template<class T,class Eval>
//struct Create<T,Ref<CP::Var<Eval> > >
//{
//	Ref<CP::Var<Eval> > operator()(State& state, const T& t)
//	{	throw Casper::Exception::TypeCoercion(typeid(T).name(),"CP::Var<Eval>");	}
//};

} // Detail

template<class Eval>
struct RefView<Eval,Expr<Eval> > : IRef<Eval>
{
	RefView(State& state, const Expr<Eval>& e) :
		par(e.toRef(state)),expr(e) {}
	const Expr<Eval>& getObj() const {	return expr; }
	Eval value() const { return par.value(); }
	void setValue(const Eval& v)	{	par.setValue(v);	}
	Ref<Eval> par;
	Expr<Eval> expr;
};

// Extracts a given element of any array type
template<class Eval>
struct RefArrayView<Expr<Seq<Eval> > >
{
	typedef Expr<Eval>	Elem;
	RefArrayView(State& s,const Expr<Seq<Eval> >& e) : a(e.toStdArray()) {}
	Elem& operator[](int idx)
	{	return a[idx];	}
	const Elem& operator[](int idx) const
	{	return a[idx];	}
	Util::StdArray<Expr<Eval> > a;
};


template<class Eval>
struct RefView<CP::Var<Eval>,Expr<Eval> > : IRef<CP::Var<Eval> >
{
	RefView(State& state, const Expr<Eval>& e) :
		par(e.toCPVarRef(state)),expr(e) {}
	const Expr<Eval>& getObj() const {	return expr; }
	CP::Var<Eval> value() const { return par.value(); }
	//void setValue(const Eval& v)	{	par.setValue(v);	}
	Ref<CP::Var<Eval> > par;
	Expr<Eval> expr;
};

}

#endif /* CASPER_KERNEL_OBJ_PAR_H_ */
