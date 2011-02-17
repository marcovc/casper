/*
 * saucy.c
 * Automorphism groups of graphs
 *
 * by Paul T. Darga <pdarga@umich.edu>
 * and Mark Liffiton <liffiton@umich.edu>
 *
 * Copyright (C) 2004, The Regents of the University of Michigan
 * See the LICENSE file for details.
 */
#include "saucy.h"

/*
 * main(argc, argv)
 * Program entry, option parsing, and algorithm execution.
 */

int
main(int argc, char **argv)
{
  saucy_t *saucy;

#ifdef Synopsys
  util_mem_start(&argc, argv);
#endif
	
  /* Initialization */
  saucy = saucy_begin(argc, argv);
  saucy_init(saucy);
  
  /* Run the search */
  if (!saucy_search(saucy)) {
    fprintf(stderr, "saucy: search timed out\n");
  }
  
  /* Cleanup */
  saucy_end(saucy);
  
  /* That's it, have a nice day */
  return EXIT_SUCCESS;
}
