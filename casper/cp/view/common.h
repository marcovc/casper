 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_CP_VIEW_COMMON
#define _H_CASPER_CP_VIEW_COMMON

#include <casper/kernel/notify/notifiable.h>
#include <casper/kernel/notify/notifier.h>
#include <casper/cp/store.h>
#include <casper/kernel/par/par.h>

//#include <casper/kernel/notify/susplist.h>
//#include <casper/kernel/relation.h>
//#include <casper/kernel/memory.h>

namespace Casper {
namespace CP {

template<class,class> class Var;
template<class T>	struct ValExpr;
template<class T>	struct BndExpr;
template<class E,class D>	struct DomExpr;
//template<class T>	struct MutExpr;

template<class,class>				struct ValView;
template<class,class,class>			struct ValViewRel1;
template<class,class,class,class>	struct ValViewRel2;

template<class,class>				struct BndView;
template<class,class,class>			struct BndViewRel1;
template<class,class,class,class>	struct BndViewRel2;

template<class,class,class>			struct DomView;
//template<class,class,class>			struct DomViewRel1;
//template<class,class,class,class>	struct DomViewRel2;

template<class,class,class>
struct ChkViewRel1;

template<class,class,class,class,class>
struct ChkViewRel2;

template<class,class,class,class,class,class,class>
struct ChkViewRel3;

template<class,class,class,class,class,class,class,class,class>
struct ChkViewRel4;

template<class,class,class,class,class,class,class,class,class,class,class>
struct ChkViewRel5;


namespace Traits {

namespace expression {
struct Domain;
struct Bound;
struct Value;
struct Par;
struct Relation;
struct Filter;
struct Ground;
};

template<class View,class Expr>
struct ExprMatch
{	static const bool value = false;	};

template<class Eval,class Dom>
struct ExprMatch<Var<Eval,Dom>,expression::Domain>
{	static const bool value = true;	};

template<class Eval,class Dom>
struct ExprMatch<DomExpr<Eval,Dom>,expression::Domain>
{	static const bool value = true;	};

template<class Eval,class View,class Dom>
struct ExprMatch<DomView<Eval,View,Dom>,expression::Domain>
{	static const bool value = true;	};

template<class Eval>
struct ExprMatch<BndExpr<Eval>,expression::Bound>
{	static const bool value = true;	};

template<class Eval,class View>
struct ExprMatch<BndView<Eval,View>,expression::Bound>
{	static const bool value = true;	};

template<class Eval>
struct ExprMatch<ValExpr<Eval>,expression::Value>
{	static const bool value = true;	};

template<class Eval,class View>
struct ExprMatch<ValView<Eval,View>,expression::Value>
{	static const bool value = true;	};

template<class Eval>
struct ExprMatch<Par<Eval>,expression::Par>
{	static const bool value = true;	};

template<class Eval,class View>
struct ExprMatch<ParView<Eval,View>,expression::Par>
{	static const bool value = true;	};

template<class Func,class View>
struct ExprMatch<Rel1<Func,View>,expression::Relation>
{	static const bool value = true;	};

template<class Func,class View1,class View2>
struct ExprMatch<Rel2<Func,View1,View2>,expression::Relation>
{	static const bool value = true;	};

template<>
struct ExprMatch<Filter,expression::Filter>
{	static const bool value = true;	};

/*
template<class Func,class View,class Test>
struct ExprMatch<Rel1<Func,View>,Test>
{	static const bool value = true;	};

template<class Func,class View1,class View2,class Test>
struct ExprMatch<Rel2<Func,View1,View2>,Test>
{	static const bool value = ExprMatch<View1,Test>::value or
							  ExprMatch<View2,Test>::value;	};
*/
template<class Func,class View>
struct ExprMatch<Rel1<Func,View>,expression::Ground>
{	static const bool value = ExprMatch<View,expression::Value>::value or
							  ExprMatch<View,expression::Par>::value;	};

template<class View>
bool isDomExpr(const View& v)
{	return ExprMatch<View,expression::Domain>::value;	}

template<class View>
bool isBndExpr(const View& v)
{	return ExprMatch<View,expression::Bound>::value;	}

template<class View>
bool isValExpr(const View& v)
{	return ExprMatch<View,expression::Value>::value;	}

template<class View>
bool isPar(const View& v)
{	return ExprMatch<View,expression::Par>::value;	}

template<class View>
bool isFilter(const View& v)
{	return ExprMatch<View,expression::Filter>::value;	}

template<class View>
bool isRelation(const View& v)
{	return ExprMatch<View,expression::Relation>::value;	}

template<class View>
bool isGround(const View& v)
{	return ExprMatch<View,expression::Ground>::value;	}

} // Traits
};
};

#endif // _H_CASPER_KERNEL_VIEW_COMMON
