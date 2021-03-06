/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _M_CASPER_KERNEL_GOAL_OP_GOAL
#define _M_CASPER_KERNEL_GOAL_OP_GOAL
 
#include <casper/kernel/rel/macros.h>

#define AT_Goal					Goal
#define AD_Goal			

#define AT_GoalFunObj			std::function<Goal()>
#define AD_GoalFunObj			

namespace Casper {

REL_FROM_LOGICAL_OPERATORS(Goal,T)
REL_FROM_LOGICAL_OPERATORS(bool,Goal)

REL_FROM_LOGICAL_OPERATORS(Goal,GoalFunObj)
REL_FROM_LOGICAL_OPERATORS(GoalFunObj,Goal)

}
 
#endif // _M_CASPER_KERNEL_GOAL_OP_GOAL
