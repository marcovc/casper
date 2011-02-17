#ifndef SAUCY_INT_H
#define SAUCY_INT_H
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
#include <vector>
#include <list>
#include <casper/thirdparty/saucy-1.1/saucy.h>

#define SAUCY_MALLOC(count_,type_) ((type_ *) malloc((count_)*sizeof(type_)))
#define SAUCY_CALLOC(count_,type_) ((type_ *) calloc((unsigned) (count_),sizeof(type_)))
#define SAUCY_REALLOC(ptr_,type_,num_) ((type_ *) realloc((ptr_),sizeof(type_)))
#define SAUCY_FREE(ptr_) free((ptr_))

#define CPUTIME (((double)clock())/((double)CLOCKS_PER_SEC))

#define MULTIPLY(s1_,s2_,i_)   \
  {                            \
    if ((s1_ *= i_) >= 1e10) { \
      s1_ /= 1e10; s2_ += 10;  \
    }                          \
  }

/* Bit manipulation routines */
#define BITS            ((int)(sizeof(int)*8))
#define BSIZE(n_)       (((n_-1)/BITS)+1)
#define BALLOC(b_,n_)   ((b_)=SAUCY_CALLOC(BSIZE((n_)),int))
#define BWIPE(b_,n_)    (memset((b_),0,(BSIZE((n_))*sizeof(int))))
#define BISSET(b_,i_)   ((b_)[(i_)/BITS]&(1<<((i_)%BITS)))
#define BSET(b_,i_)     ((b_)[(i_)/BITS]|=(1<<((i_)%BITS)))
#define BCLEAR(b_,i_)   ((b_)[(i_)/BITS]&=~(1<<((i_)%BITS)))
#define BERASE(b_,i_)   ((b_)[(i_)/BITS]=0)

/* Array-of-bit-vectors manipulation routines */
#define BARR_ALLOC(b_,n_,k_)  ((b_)=SAUCY_CALLOC((k_*BSIZE((n_))),int))
#define BARR(b_,i_,k_)        ((b_)+((i_)*BSIZE(k_)))
#define FIX(fix_,i_,n_)       (BARR((fix_),(i_),(n_)))
#define MCR(mcr_,i_,n_)       (BARR((mcr_),(i_),(n_)))

/* To add stuff to alpha/beta */
#define refine_add_induce(saucy_, single_, who_) \
  {                                                    \
    if (single_) {                                     \
      saucy->beta[saucy_->beta_num++] = who_;          \
    } else {                                           \
      saucy->alpha[saucy_->alpha_num++] = who_;        \
    }                                                  \
    saucy->abmark[who_] = 1;                           \
  }

struct saucy_struct {
  int vertex_num; /* Number of vertices searched over */
  int edge_num;   /* Number of edges */
  int cell_num;   /* Number of cells in partition */
  int lev;        /* Current search tree level */
  int flag;       /* Was the last leaf node an automorphism? */
  int target;     /* Front of cell about to be split */
  int target_min; /* Location of minimum element in target cell */
  int anc;        /* Level of greatest common ancestor with zeta */
  int have_zeta;  /* Have we hit the first leaf node yet? */
  int saved_auto_num;/* Number of auto's saved */
  int front;      /* Position of refining cell */
  int alpha_num;  /* Size of alpha stack */
  int beta_num;   /* Size of beta stack */
  int cell_list_num;/* Number of cells in cell list */
  int perm_num;   /* permutation count */
  int max_saved_auto_num;/* Number of saved auto's for backtracking from bads*/
  int timeout;    /* Seconds before quitting after refinement */
  int stats_mode; /* Print out stats when we're done */
  int zeta_mode;  /* Stop when we hit the first leaf node */
  int quiet_mode; /* Don't output automorphisms */
  int gap_mode;   /* Do GAP I/O (interface with Shatter) */
  int grpsize_indmin;/* used to compute size of automorphism group */
  int grpsize_exp;/* used to compute size of automorphism group */
  double grpsize_base;/* used to compute size of automorphism group */
  int *edg;       /* Connection info */
  int *adj;       /* Adjacency list sizes and positions */
  int *lab;       /* Labelling of objects (half of coloring) */
  int *ptn;       /* Where/what level cells split (other half) */
  int *nextnonsing;/* Forward next-nonsingleton pointers */ 
  int *prevnonsing;/* Backward next-nonsingleton pointers */
  int *cell_front;/* Pointer to front of cells */
  int *cell_len;  /* Length of cells (defined for cfront's) */
  int *gamma;     /* Container for permutations at leaf nodes */
  int *bit_contents;/* Workspace bit vector */
  int *start;     /* Location of target at each level */
  int *fixed;     /* Label fixed at each level */
  int *zeta;      /* The first leaf node */
  int *theta;     /* Running approximation of orbit partition */
  int *bit_fixed; /* Bit vector version of fixed */
  int *fix;       /* Labels left fixed by each saved auto */
  int *mcr;       /* Min cell representatives of each saved auto */
  int *alpha;     /* Nonsingletons that might induce refinement */
  int *beta;      /* Singletons that might induce refinement */
  int *bucket;    /* Workspace */
  int *vertex_count;/* Num vertices with same adj count to ref cell */
  int *cell_count;/* Number of connections to refining cell */
  int *cell_list; /* List of cells marked for refining */
  char *abmark;   /* Those things that are marked */
  char *stuff;    /* bit_contents replacement (faster) */
  char *cell_mark;/* Is this cell marked for refining? */
  saucy_permutation_t *permutations;/* saved permutation list */
  FILE *file;     /* File we're reading input from */
  
  // mvc: data structures for holding symmetry groups instead of print them
  typedef std::vector<int> 					Orbit;
  typedef std::vector<Orbit>				Generator;
  typedef std::vector<Generator>			Generators;
  Generators	generators;
};

struct saucy_cycle_struct {
  saucy_cycle_t *next_cycle; /* next cycle */
  int cycle_size;            /* cycle size */
  int *cycle;                /* cycle */
};

struct saucy_permutation_struct {
  saucy_permutation_t *next_permutation; /* next permutation */
  int cycle_num;                         /* number of cycles */
  saucy_cycle_t *cycles;                 /* cycles */
};

/* saucy.c - Private functions */
void saucy_search_init(saucy_t *saucy);
void saucy_search_cleanup(saucy_t *saucy);
void saucy_search_descend_left(saucy_t *saucy);
void saucy_search_update_theta(saucy_t *saucy);
void saucy_search_backtrack(saucy_t *saucy);
void saucy_search_theta_prune(saucy_t *saucy);
void saucy_search_orbit_prune(saucy_t *saucy);
void saucy_search_descend(saucy_t *saucy);
void saucy_search_min_target(saucy_t *saucy);
void saucy_search_target_cell(saucy_t *saucy);
void saucy_search_is_automorphism(saucy_t *saucy);
void saucy_data_init(saucy_t *saucy);
void saucy_data_cleanup(saucy_t *saucy);
void saucy_data_mark(saucy_t *saucy, int x);
void saucy_data_unmark(saucy_t *saucy, int x);
void saucy_data_count(saucy_t *saucy, int x);
void saucy_data_read_amorph(saucy_t *saucy); /*Read graph in amorph format*/
void saucy_data_read_gap(saucy_t *saucy);/*Read graph in GAP format*/
void saucy_data_vertex_complete(saucy_t *saucy);/*Fix vertices for adjacency array*/
void saucy_data_edge_complete(saucy_t *saucy);/*Fix edges for adjacency array*/
void saucy_refine_init(saucy_t *saucy);
void saucy_refine_cleanup(saucy_t *saucy);
void saucy_refine(saucy_t *saucy);
void saucy_refine_singleton(saucy_t *saucy);/*Refine with a single cell*/
void saucy_refine_nonsingle(saucy_t *saucy);/*Refine with cell (>=2 objs)*/
void saucy_refine_sort_cell_list(int *cell_list, int cell_size);/*Shell sort*/
int saucy_parse_opt_int(int min);
void saucy_parse_usage(void);

#endif
