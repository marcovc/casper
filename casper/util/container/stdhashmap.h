 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#if defined(CASPER_HASH_MARCO)
#include <casper/util/container/hash/hashmap-marco.h>
#elif defined(CASPER_HASH_RUBEN)
#include <casper/util/container/hash/hashmap-ruben.h>
#elif defined(CASPER_HASH_STD)
#include <casper/util/container/hash/hashmap-std.h>
#elif defined(CASPER_HASH_MAP)
#include <casper/util/container/hash/hashmap-map.h>
#endif
