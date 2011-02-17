/***************************************************************************
 *   Copyright (C) 2008 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/



#if defined(CASPER_IDXSETFD_HASH) or defined(CASPER_IDXSETFD_HASH1)  
#include <casper/set/idxsetfdfilter-hash.h>
#elif defined(CASPER_IDXSETFD_HASHPOSS) or defined(CASPER_IDXSETFD_HASHPOSSBITS)
#include <casper/set/idxsetfdfilter-hash+poss.h>
#endif
