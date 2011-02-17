/***************************************************************************
 *   Copyright (C) 2006/2007 by Marco Correia                               *
 *   mvc@di.fct.unl.pt															   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _H_CASPER_SET
#define _H_CASPER_SET

//#include <casper/set/common.h>
#include <casper/set/setfd.h>
#include <casper/set/setfdfilter.h>
#include <casper/set/variable.h>
//#include <casper/int/view.h>
#include <casper/set/filter.h>
#include <casper/set/goal.h>
//#include <casper/int/global.h>

#include <casper/set/idxsetfd.h>
#include <casper/set/idxsetfdfilter.h>


namespace casper {
	
/** \defgroup Set Set
 *  Set domain specific functionality.
 *	\ingroup Domains
 */

	
/** \defgroup SetExamples Set examples
 *	\ingroup Set Examples
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

};

#endif /*_H_CASPER_SET*/
