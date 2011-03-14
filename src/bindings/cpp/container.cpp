/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2009 - Sergio Silva <email>           				   		   *
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

#include "container.h"
#include <iostream>
#include <algorithm>

using namespace std;

namespace Casperbind {
namespace cpp {

namespace Detail {

const Container::ElemType ElemTypeTraits<bool>::type = Container::sBool;

const Container::ElemType ElemTypeTraits<int>::type = Container::sInt;

const Container::ElemType ElemTypeTraits<double>::type = Container::sDouble;

const Container::ElemType ElemTypeTraits<const char*>::type = Container::sString;

const Container::ElemType ElemTypeTraits<SharedSymbol>::type = Container::sSymbol;


}

}
}

