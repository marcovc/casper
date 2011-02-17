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

#ifndef _H_CASPER_INT
#define _H_CASPER_INT

#include <casper/int/common.h>
#include <casper/int/fd.h>
#include <casper/int/variable.h>
#include <casper/int/view.h>
#include <casper/int/filter.h>
#include <casper/int/goal.h>
#include <casper/int/global.h>

#include <casper/int/distinct/gac.h>
#include <casper/int/distinct/bac.h>
#include <casper/int/distinct/val.h>

namespace casper {

/** \defgroup Integer Integer
 *  Integer domain specific functionality.
 *	\ingroup Domains
 */


/** \defgroup IntExamples Integer examples
 *	\ingroup Integer Examples
 * 	Examples involving integer variables.
 *
 *  \par Example 1: Latin Squares.
 *  All rows and columns are distinct. The problem
 *  is modelled with default propagator for all constraints, and default
 *  search heuristics.
 *  See examples/int/latin.cpp for the complete source file.
 *  \dontinclude int/latin.cpp
 *  \skip //
 *  \until }
 *  \until }
 *
 *
 *  \par Example 2: The Golomb Ruler problem.
 *  The problem is described at http://4c.ucc.ie/~tw/csplib/prob/prob006/index.html.
 *  This example shows parameterizing the type of propagator applied to
 *  some constraint, specified as the parameter \p g.
 *  See examples/int/golomb.cpp for the complete source file.
 *  \dontinclude int/golomb.cpp
 *  \skip  std;
 *  \line  *
 *  \until }
 *
 *
 *  \par Example 3: The Queens problem.
 *  Try to place n queens on a nXn chessboard, such that no two queens attack
 *  each other.
 *  This example describes how to create a new filter involving integer domain
 *  variables. We start by defining the filter:
 *  \dontinclude int/nqueens.cpp
 *  \skip std;
 *  \line *
 *  \until };
 *  Note that this filter is directional, i.e. only enforces consistency in one
 *  direction. We now define a function that creates and returns instances of
 *  this filter and its symmetric, which is obtained from the previous
 *  NoThreatX by swapping arguments:
 *  \until }
 *  At this point the noThreat filter is available for use as any other filter:
 *  \until }
 *
 */

/**	\example int/latin.cpp  Latin Square example. */
/**	\example int/golomb.cpp  Golomb Ruler example. */
/**	\example int/nqueens.cpp  NQueens example. */

};

#endif /*_H_CASPER_INT*/
