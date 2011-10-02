/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2009-2010 - Marco Correia <marco.v.correia@gmail.com>           *
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

#ifndef CASPER_CP_H_
#define CASPER_CP_H_

#include <casper/cp/int.h>
#include <casper/cp/set.h>
#include <casper/cp/real.h>
#include <casper/cp/goal.h>

/** \defgroup IntegerDomain Integers
 *  Integer domain specific functionality.
 *	\ingroup CP
 */

/** \defgroup SetDomain Sets
 *  Set domain specific functionality.
 *	\ingroup CP
 */

/** \defgroup GenericDomain Generic
 * 	\ingroup CP
 * 	Functionality which is generic with regards to the reasoning domain.
 */


/** \defgroup ValViews Value
 * 	\ingroup CPViews
 * 	A ValView allows to instantiate the associated expression or
 * 	to access its value.
 */

/** \defgroup BndViews Bound
 * 	\ingroup CPViews
 * 	A BndView allows to access or update the bounds of an associated expression.
 */

/** \defgroup DomViews Domain
 * 	\ingroup CPViews
 * 	A DomView allows to access or update the domain of the
 * 	associated expression. In some cases some constraints
 * 	and auxiliary variables are created as a side effect.
 * 	DomViews over some specific expressions may provide better
 * 	alternatives.
 */

/** \defgroup ChkViews Check
 * 	\ingroup CPViews
 * 	A ChkView allows to access or update the value of an
 * 	associated boolean expression, mostly used for constraint
 * 	reification.
 * 	\note For many expressions the entailnment is currently
 * checked based on the bounds of the variables. This may not
 * be ideal, so it could be useful to further specialize to
 * BndChkView and DomChkView, for example.
 */

/** \defgroup CPSetDataStructures Data structures
 *  Data structures for working with set domain variables
 *	\ingroup SetDomain
 */

/** \defgroup CPIntegerDataStructures Data structures
 *  Data structures for working with integer domain variables
 *	\ingroup IntegerDomain
 */

/** \defgroup CPDataStructures Data Structures
 * 	\ingroup GenericDomain
 * 	CP data structures.
 */

/**
 * \defgroup SetFilters Filtering
 * Filters specific to set domain variables.
 * \ingroup SetDomain
 */

/**
 * \defgroup IntFilters Filtering
 * Filters specific to integer domain variables.
 * \ingroup IntegerDomain
 */

/** \defgroup Filtering Filtering
 *
 * 	Filtering is accomplished through Filter objects. Filters provide an
 *  (inference) mechanism to narrow domains of variables of a given relation.
 *	\sa Filter, FilterScheduler
 *	\ingroup GenericDomain
 */

/** \defgroup CPSearch Search
 * 	\ingroup GenericDomain
 *
 * 	Search primitives for CP.
 */


/** \defgroup CPViews Views
 * 	\ingroup GenericDomain
 * 	A View imposes a specific interface to an associated generic expression.
 * 	In the CP context that interface allows to access or update the domain
 * 	of the associated expression. There are currently three types of views
 * 	available: DomView, BndView, and ValView.
 */

/**
 * \defgroup SetViews Views
 * Views over set domain expressions.
 * \ingroup SetDomain
 */

/** \defgroup IntExamples Examples
 *	\ingroup IntegerDomain Examples
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
 *  \until }
 *  \until }
 *  \until }
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
 *  \until }
 *
 */

/**	\example int/latin.cpp  Latin Square example. */
/**	\example int/golomb.cpp  Golomb Ruler example. */
/**	\example int/nqueens.cpp  NQueens example. */



/** \defgroup SetExamples Examples
 *	\ingroup SetDomain Examples
 * 	Examples involving Set variables.
 *
 *  \par Example 1: Steiner Triples.
 *  This is problem 44 in http://www.csplib.org/.
 *  See examples/set/steiner.cpp for the complete source file.
 *  \dontinclude set/steiner.cpp
 *  \skip //
 *  \until }
 *
 *
 *  \par Example 2: Social Golfers.
 *  This is problem 10 in http://www.csplib.org/.
 *  See examples/set/golfer.cpp for the complete source file.
 *  \dontinclude set/golfer.cpp
 *  \skip  //
 *  \until }
 *  \until }
 *  \until }
 *
 */

/**	\example set/steiner.cpp  Steiner triples example. */
/**	\example set/golfer.cpp  Social Golfers example. */

#endif /* CASPER_CP_H_ */
