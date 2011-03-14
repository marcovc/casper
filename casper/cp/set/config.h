/***************************************************************************
 *   Copyright (C) 2008 by Marco Correia                                   *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CASPER_SET_CONFIG_H_
#define CASPER_SET_CONFIG_H_

/**************************************************
 * selects the hashset and hashmap implementation *
 *************************************************/
//#define CASPER_HASH_MARCO
//#define CASPER_HASH_RUBEN
#define CASPER_HASH_STD
//#define CASPER_HASH_MAP

#define CASPER_IDXSETFD_HASHRATIO 1.25

/******************************************
 * selects the default set implementation *
 ******************************************/
#define CASPER_DEFAULTSET_SETFD
//#define CASPER_DEFAULTSET_IDXSETFD

/***********************************************
 * selects the default idxsetfd implementation *
 **********************************************/
//#define CASPER_IDXSETFD_HASH
//#define CASPER_IDXSETFD_HASH1
#define CASPER_IDXSETFD_HASHPOSS

/**********************************************
 * selects the set card domain implementation *
 **********************************************/
#define CurSetFDCard	FD<Casper::Detail::List,Casper::Detail::Interval,int>
//#define	CurSetFDCard	FD<Util::StdList,Element,int>


/***************************************************
 * if defined, switches to old or no domain deltas *
 ***************************************************/
#ifdef CASPER_DEFAULTSET_SETFD
//#define CASPER_NODELTAS
//#define CASPER_DELTAS_SELECT
//#define CASPER_OLDDELTAS
#else
//#define CASPER_OLDDELTAS
#endif

/********************************************
 * if defined, switches to old filter sched *
 ********************************************/
//#define CASPER_OLD_COST_FILTER_SCHED

#ifdef CASPER_DEFAULTSET_IDXSETFD
#define CurSetFD IdxSetFD
#else
#define CurSetFD SetFD
#endif


#endif /*CASPER_SET_CONFIG_H_*/
