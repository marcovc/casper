 /*************************************************************************\
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

/**
 *	\file
 *
 *  Gerneralized Arc Consistent distinct constraint [\ref regin94].
 *
 *  Note: This file is just a wrapper for the Gecode library distinct filter,
 *  see \ref gac-gecode-h.
 */

#ifndef _H_CASPER_CP_INT_DISTINCT_GAC
#define _H_CASPER_CP_INT_DISTINCT_GAC


//#include <casper/cp/filter/dom.h>
//#include <casper/util/container/stdarray.h>
//#include <casper/cp/int/distinct/common.h>
//#include <casper/cp/int/distinct/val.h>
//#include <casper/cp/int/var.h>

#define	 USE_GECODE_DOM_ALLDIFF
//#define	 USE_CASPER_DOM_ALLDIFF_V1
//#define	 USE_CASPER_DOM_ALLDIFF_V2
//#define	 USE_CASPER_DOM_ALLDIFF_V2_DELTA
//#define	 USE_CASPER_DOM_ALLDIFF_V3

#include <casper/cp/int/distinct/gac-gecode.h>
#include <casper/cp/int/distinct/gac-slow.h>

#endif
