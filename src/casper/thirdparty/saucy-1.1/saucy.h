#ifndef SAUCY_H
#define SAUCY_H
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

/* 
 * The alarm() call and SIGALRM are POSIX.  getopt() is
 * (apparently) an XSI extension.  Defining _XOPEN_SOURCE at 600
 * implies _POSIX_C_SOURCE at 200112L.
 */

#define _XOPEN_SOURCE 600
#include <unistd.h> /* getopt, alarm, SIGALRM */

/* We need this for strdup */
#define _XOPEN_SOURCE_EXTENDED 1

/* ANSI stuff */
#include <limits.h> /* CHAR_BIT */
#include <stdlib.h> /* malloc, calloc, free, exit, EXIT_* */
#include <string.h> /* memset */
#include <signal.h> /* signal, SIG_IGN */
#include <stdio.h>  /* FILE, fopen, fscanf, printf, fclose */
#include <time.h>/* clock, CLOCKS_PER_SEC */

typedef struct saucy_struct saucy_t;
typedef struct saucy_cycle_struct saucy_cycle_t;
typedef struct saucy_permutation_struct saucy_permutation_t;

/* symmetry computation pi */
extern saucy_t *saucy_begin(int argc, char **argv);
extern void saucy_init(saucy_t *saucy);
extern void saucy_end(saucy_t *saucy);
extern int saucy_search(saucy_t *saucy);

/* options pi */
extern void saucy_data_init_options(saucy_t *saucy);
extern void saucy_data_set_options(saucy_t *saucy, int quiet_mode,
                                   int gap_mode, int zeta_mode, 
                                   int stats_mode, int max_saved, int timeout);
extern void saucy_parse_options(saucy_t *saucy, int argc, char **argv);

/* graph pi */
extern void saucy_graph_create(saucy_t *saucy, int node_num, int edge_num);
extern void saucy_graph_vertex_edge_count(saucy_t *saucy, int node, 
                                          int edge_num);
extern void saucy_graph_vertex_complete(saucy_t *saucy);
extern void saucy_graph_insert_edge(saucy_t *saucy, int j, int k);
extern void saucy_graph_insert_partition_separator(saucy_t *saucy, int k);
extern void saucy_graph_insert_color(saucy_t *saucy, int i, int j);
extern void saucy_graph_edge_complete(saucy_t *saucy);

/* permutation pi */
extern saucy_permutation_t *saucy_permutations_get_first_permutation(saucy_t *saucy);
extern saucy_permutation_t *saucy_permutations_get_next_permutation(saucy_permutation_t *permutation);
extern int saucy_permutations_get_permutation_count(saucy_t *saucy);
extern int saucy_permutations_get_cycle_count(saucy_permutation_t *permutation);
extern saucy_cycle_t *saucy_permutations_get_first_cycle(saucy_permutation_t *permutation);
extern saucy_cycle_t *saucy_permutations_get_next_cycle(saucy_cycle_t *cycle);
extern int saucy_permutations_get_cycle_size(saucy_cycle_t *cycle);
extern int saucy_permutations_get_cycle_element(saucy_cycle_t *cycle, int index);

#endif
