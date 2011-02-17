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
#include <casper/thirdparty/saucy-1.1/saucy_int.h>
#include <iostream>
#include <iterator>

/***** Public begin and end functions *****/

saucy_t *
saucy_begin(int argc, char **argv)
{
  saucy_t *saucy;

  saucy = SAUCY_CALLOC(1, saucy_t);
  if (saucy == NULL) {
    //(void) fprintf(stderr, "saucy ran out of memory\n");
    throw std::bad_alloc();
  }
  saucy_data_init_options(saucy);
  if (argc && argv) {
    saucy_parse_options(saucy, argc, argv);
  }

  return saucy;
}

#if 0
static void
print_graph(const saucy_t *saucy)
{
	int i, j, k;
	printf("%d %d %d\n", saucy->vertex_num, saucy->edge_num, 1);
	for (i = 0; i < saucy->vertex_num; ++i) {
		for (j = saucy->adj[i]; j < saucy->adj[i+1]; ++j) {
			k = saucy->edg[j];
			if (i < k) printf("%d %d\n", i, k);
		}
	}
}

static void
print_color(const saucy_t *saucy)
{
	int i;
	const int n = saucy->vertex_num;
	printf("lab:");
	for (i = 0; i < n; ++i) printf(" %d", saucy->lab[i]);
	printf("\nptn:");
	for (i = 0; i < n; ++i) printf(" %d", saucy->ptn[i]);

	putchar('\n');
}
#endif

void
saucy_init(saucy_t *saucy)
{
  saucy_data_init(saucy);
  saucy_refine_init(saucy);
  saucy_search_init(saucy);
  saucy->grpsize_base = 1.0;
  saucy->grpsize_exp = 0;

  return;
}

void
saucy_end(saucy_t *saucy)
{
  saucy_search_cleanup(saucy);
  saucy_refine_cleanup(saucy);
  saucy_data_cleanup(saucy);
  if (saucy) {
    SAUCY_FREE(saucy);
  }
}

/***** Public search functions *****/

int
saucy_search(saucy_t *saucy)
{
  int i, timeout_flag, bads, nodes;
  double cpu_time, current_cpu_time;
  /*struct rusage ruse;*/

  /* Start timing */
  cpu_time = CPUTIME;
  timeout_flag = 0;
  bads = nodes = 0;

  /* Preprocessing after initial coloring */
  saucy_refine(saucy);

  /* Keep going while there are tree nodes to expand */
  while (!timeout_flag) {
    /* Find a target cell */
    ++nodes; 
    saucy_search_target_cell(saucy);
    if (saucy->target != saucy->vertex_num+1) { 
      saucy_search_descend_left(saucy); 
      continue; 
    }

    /* We don't have a target, so we're discrete */
    if (!saucy->have_zeta) {
      saucy->have_zeta = 1;
      for (i = 0; i < saucy->vertex_num; ++i) {
        saucy->zeta[saucy->lab[i]] = i;
      }
      if (saucy->zeta_mode) {        
        break;
      }
    } else {
      /* Prepare permutation and check */
      for (i = 0; i < saucy->vertex_num; ++i) {
        saucy->gamma[i] = saucy->lab[saucy->zeta[i]];
      }
      saucy_search_is_automorphism(saucy);

      /* Do some stuff if we actually got an automorphism */
      if (saucy->flag) {
        ++saucy->perm_num;
        saucy_search_update_theta(saucy);
      } else {
        /* Keep track of leaf nodes that are not automorphisms */
        ++bads;
      }
    }

    /* Since we're discrete, we need to backtrack */
    saucy_search_backtrack(saucy);
    if (!saucy->lev) {
      break;
    } else {
      saucy_search_descend(saucy);
    }
    if (saucy->timeout > 0) {
      current_cpu_time = CPUTIME;
      if ((current_cpu_time - cpu_time) > saucy->timeout) {
        timeout_flag = 1;
      }
    }
  }

  /* Finish timing */
  cpu_time =  CPUTIME - cpu_time;
  
  /* Normalize group size, if needed */
  if ((saucy->grpsize_exp > 0) || (saucy->grpsize_base >= 1000000.0)) {
	  while (saucy->grpsize_base >= 10.0) {
		saucy->grpsize_base /= 10;
		++saucy->grpsize_exp;
	  }
  }
  
  /* Print out stats if requested */
  if (saucy->stats_mode) {
    (void) fprintf(stderr, "saucy stats: symmetries=%g",
		saucy->grpsize_base);
	if (saucy->grpsize_exp > 0) {
		fprintf(stderr, "e%d", saucy->grpsize_exp);
	}
	(void) fprintf(stderr, " nodes=%d gens=%d bads=%d time=%.2fs\n",
                   nodes, saucy->perm_num, bads, cpu_time);
  }
  
  /* Let caller know about abnormal termination */
  return !timeout_flag;
}

/***** Public data functions *****/

/*
 * Init option flags
 */
void
saucy_data_init_options(saucy_t *saucy)
{
  saucy->quiet_mode = 0;
  saucy->gap_mode = 0;
  saucy->zeta_mode = 0;
  saucy->stats_mode = 0;
  saucy->max_saved_auto_num = 60;
  saucy->timeout = 0;
}

/*
 * Set options
 */
void
saucy_data_set_options(saucy_t *saucy, int quiet_mode, int gap_mode, 
                       int zeta_mode, int stats_mode, int max_saved_auto_num, 
                       int timeout)
{
  saucy->quiet_mode = quiet_mode;
  saucy->gap_mode = gap_mode;
  saucy->zeta_mode = zeta_mode;
  saucy->stats_mode = stats_mode;
  saucy->max_saved_auto_num = max_saved_auto_num;
  saucy->timeout = timeout;
}

/***** Public parse functions *****/

/*
 * parse_options(argc, argv)
 * Set option flags
 */
void
saucy_parse_options(saucy_t *saucy, int argc, char **argv)
{
  int c;

  /* Do some command line processing */
  do {
    
    /* Get the next option */
    c = getopt(argc, argv, "csqzgt:p:d:");
    if (c != -1) {
      /* Handle the option */
      switch (c) {
      case 's': 
        saucy->stats_mode = 1; 
        break;
      case 'q': 
        saucy->quiet_mode = 1; 
        break;
      case 'z': 
        saucy->zeta_mode = 1; 
        break;
      case 'g': 
        saucy->gap_mode = 1; 
        break;
      case 't': 
        saucy->timeout = saucy_parse_opt_int(1); 
        break;
      case 'p': 
        saucy->max_saved_auto_num = saucy_parse_opt_int(1); 
        break;
      default:
        saucy_parse_usage();
      }
    }
  } while (c != -1);
  
  /* There must be exactly one file */
  if (argc - optind != 1) {
    (void) fprintf(stderr, "saucy: must specify input file (see man page)\n");
    exit(EXIT_FAILURE);
  }
  
  /* Open the input file */
  if ((saucy->file = fopen(argv[optind], "r")) == NULL) {
    (void) fprintf(stderr, "saucy: unable to open file %s\n", argv[optind]);
    exit(EXIT_FAILURE);
  }

  return;
}

/***** Public graph functions *****/

void
saucy_graph_create(saucy_t *saucy, int vertex_num, int edge_num)
{
  int i;

  saucy->vertex_num = vertex_num;
  saucy->edge_num = edge_num;

  /* Allocate basic arrays */
  saucy->adj = SAUCY_CALLOC((vertex_num+1), int);
  if (saucy->adj == NULL) {
    //(void) fprintf(stderr, "saucy->adj ran out of memory\n");
    throw std::bad_alloc();
    //exit(EXIT_FAILURE);
  }
  saucy->edg = SAUCY_MALLOC((2 * edge_num), int);
  if (saucy->edg == NULL) {
    //(void) fprintf(stderr, "saucy->edg ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->lab = SAUCY_MALLOC((vertex_num+1), int);
  if (saucy->lab == NULL) {
    //(void) fprintf(stderr, "saucy->lab ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->ptn = SAUCY_MALLOC(vertex_num, int);
  if (saucy->ptn == NULL) {
    //(void) fprintf(stderr, "saucy->ptn ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }

  saucy->lab[vertex_num] = vertex_num;
  
  /* Initialize the partition nest */
  for (i = 0; i < vertex_num; ++i) {
    saucy->lab[i] = i;
    saucy->ptn[i] = vertex_num+1;
  }
  /*saucy_graph_insert_partition_separator(saucy, vertex_num);*/
}

void
saucy_graph_vertex_edge_count(saucy_t *saucy, int node, int edge_num)
{
  saucy->adj[node] = edge_num; 
}

void
saucy_graph_vertex_complete(saucy_t *saucy)
{
  saucy_data_vertex_complete(saucy);
}

void
saucy_graph_insert_color(saucy_t *saucy, int i, int j)
{
  saucy->lab[i] = j-1;
  saucy->ptn[i] = saucy->vertex_num+1;
}

void
saucy_graph_insert_edge(saucy_t *saucy, int j, int k)
{
  saucy->edg[saucy->adj[j]++] = k; 
  saucy->edg[saucy->adj[k]++] = j;
}

void
saucy_graph_insert_partition_separator(saucy_t *saucy, int k)
{
  saucy->ptn[k-1] = 0;
}

void
saucy_graph_edge_complete(saucy_t *saucy)
{
  saucy_data_edge_complete(saucy);
}

/***** Public permutation functions *****/

/* get first permutation */
saucy_permutation_t *
saucy_permutations_get_first_permutation(saucy_t *saucy)
{
  return saucy->permutations;
}

/* get next permutation */
saucy_permutation_t *
saucy_permutations_get_next_permutation(saucy_permutation_t *permutation)
{
  return permutation->next_permutation;
}

/* get number of permutations */
int
saucy_permutations_get_permutation_count(saucy_t *saucy)
{
  return saucy->perm_num;
}

/* get number of cycles */
int
saucy_permutations_get_cycle_count(saucy_permutation_t *permutation)
{
  return permutation->cycle_num;
}

/* get first permutation cycle*/
saucy_cycle_t *
saucy_permutations_get_first_cycle(saucy_permutation_t *permutation)
{
  return permutation->cycles;
}

/* get next permutation cycle*/
saucy_cycle_t *
saucy_permutations_get_next_cycle(saucy_cycle_t *cycle)
{
  return cycle->next_cycle;
}

/* get cycle size */
int
saucy_permutations_get_cycle_size(saucy_cycle_t *cycle)
{
  return cycle->cycle_size;
}

/* get cycle element */
int
saucy_permutations_get_cycle_element(saucy_cycle_t *cycle, int index)
{
  int *cycle_data;

  cycle_data = (int *) &cycle->cycle;
  return cycle_data[index];
}

/***** Private search functions *****/

 void
saucy_search_init(saucy_t *saucy)
{
  int i;
  
  /* Initialize scalars */
  saucy->lev = saucy->anc = 1;
  saucy->have_zeta = saucy->flag = saucy->saved_auto_num = 0;
  saucy->target = saucy->target_min = 0;

  /* Allocate vectors (assuming vertex_num defined) */
  saucy->start = SAUCY_MALLOC(saucy->vertex_num, int);
  if (saucy->start == NULL) {
    //(void) fprintf(stderr, "saucy->start ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->fixed = SAUCY_MALLOC(saucy->vertex_num, int);
  if (saucy->fixed == NULL) {
    //(void) fprintf(stderr, "saucy->fixed ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->zeta = SAUCY_MALLOC(saucy->vertex_num, int);
  if (saucy->zeta == NULL) {
    //(void) fprintf(stderr, "saucy->zeta ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->gamma = SAUCY_MALLOC(saucy->vertex_num, int);
  if (saucy->gamma == NULL) {
    //(void) fprintf(stderr, "saucy->gamma ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->theta = SAUCY_MALLOC(saucy->vertex_num, int);
  if (saucy->theta == NULL) {
    //(void) fprintf(stderr, "saucy->theta ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }

  /* Allocate bit vectors */
  BALLOC(saucy->bit_fixed, saucy->vertex_num);
  if (saucy->bit_fixed == NULL) {
    //(void) fprintf(stderr, "saucy->bit_fixed ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  BALLOC(saucy->bit_contents, saucy->vertex_num);
  if (saucy->bit_contents == NULL) {
    //(void) fprintf(stderr, "saucy->bit_contents ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  BARR_ALLOC(saucy->fix, saucy->vertex_num, saucy->max_saved_auto_num);
  if (saucy->fix == NULL) {
    //(void) fprintf(stderr, "saucy->fix ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  BARR_ALLOC(saucy->mcr, saucy->vertex_num, saucy->max_saved_auto_num);
  if (saucy->mcr == NULL) {
    //(void) fprintf(stderr, "saucy->mcr ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  
  /* The initial orbit partition is discrete */
  for (i = 0; i < saucy->vertex_num; ++i) {
    saucy->theta[i] = i;
  }
}

 void
saucy_search_cleanup(saucy_t *saucy)
{
  /* Free all this memory */
  if (saucy->start) {
    SAUCY_FREE(saucy->start);
  }
  if (saucy->fixed) {
    SAUCY_FREE(saucy->fixed);
  }
  if (saucy->theta) {
    SAUCY_FREE(saucy->theta);
  }
  if (saucy->gamma) {
    SAUCY_FREE(saucy->gamma);
  }
  if (saucy->zeta) {
    SAUCY_FREE(saucy->zeta);
  }
  if (saucy->bit_fixed) {
    SAUCY_FREE(saucy->bit_fixed);
  }
  if (saucy->bit_contents) {
    SAUCY_FREE(saucy->bit_contents);
  }
  if (saucy->fix) {
    SAUCY_FREE(saucy->fix);
  }
  if (saucy->mcr) {
    SAUCY_FREE(saucy->mcr);
  }
}

 void
saucy_search_descend_left(saucy_t *saucy)
{
  int temp;

  /* Remember that this cell is the target at this level */
  saucy->start[saucy->lev] = saucy->target;
  
  /* Update greatest common ancestor with zeta */
  if (!saucy->have_zeta) {
    ++saucy->anc;
  }

  /* Move the minimum label to the front */
  temp = saucy->lab[saucy->target_min];
  saucy->lab[saucy->target_min] = saucy->lab[saucy->target];
  saucy->lab[saucy->target] = temp;
  
  /* Remember that we fixed this element at this level */
  saucy->fixed[saucy->lev] = temp;
  BSET(saucy->bit_fixed, temp);
  
  /* Split the cell */
  saucy->ptn[saucy->target] = ++saucy->lev; 
  ++saucy->cell_num;
  
  /* Update cell fronts */
  for (temp = saucy->target+1; 
       temp <= saucy->target + saucy->cell_len[saucy->target]; ++temp) {
    saucy->cell_front[saucy->lab[temp]] = saucy->target + 1;
  }

  /* If |target| = 2, then skip target completely */
  if (saucy->cell_len[saucy->target] == 1) {
    saucy->nextnonsing[saucy->prevnonsing[saucy->target]] = 
      saucy->nextnonsing[saucy->target];
    saucy->prevnonsing[saucy->nextnonsing[saucy->target]] = 
      saucy->prevnonsing[saucy->target];
  } else {
    /* Otherwise, fix pointers to deal with nonsingle after target */
    saucy->nextnonsing[saucy->target+1] = saucy->nextnonsing[saucy->target];
    saucy->prevnonsing[saucy->target+1] = saucy->prevnonsing[saucy->target];
    saucy->nextnonsing[saucy->prevnonsing[saucy->target]] = saucy->target+1;
    saucy->prevnonsing[saucy->nextnonsing[saucy->target]] = saucy->target+1;
  }
  
  /* Update cell lengths */
  saucy->cell_len[saucy->target+1] = saucy->cell_len[saucy->target] - 1;
  saucy->cell_len[saucy->target] = 0;
  
  /* Now go and do some work */
  saucy_refine(saucy);
}

 void
saucy_search_update_theta(saucy_t *saucy)
{
  int i, j, x, y, z;

  /* Update all cell representatives */
  for (i = 0; i < saucy->vertex_num; ++i) {
    
    /* Look for the start of an orbit */
    if (i >= saucy->gamma[i]) {
      continue;
    }

    /* Find the cell rep for the two elements */
    for (x = i; x != saucy->theta[x]; x = saucy->theta[x]);
    for (y = saucy->gamma[i]; y != saucy->theta[y]; y = saucy->theta[y]);

    /* Pick the cell with the smaller representative */
    z = (x < y) ? x : y;

    /* Update the old root's cell representatives */
    saucy->theta[x] = saucy->theta[y] = z;
  }

  /* Clear out last automorphism if we've already saved max */
  if (saucy->saved_auto_num == saucy->max_saved_auto_num) {
    --saucy->saved_auto_num;
    BWIPE(FIX(saucy->fix, saucy->saved_auto_num, saucy->vertex_num), 
          saucy->vertex_num);
    BWIPE(MCR(saucy->mcr, saucy->saved_auto_num, saucy->vertex_num), 
          saucy->vertex_num);
  }

  /* Look for orbits in gamma and update fix and mcr */
  for (i = 0; i < saucy->vertex_num; ++i) {

    /* Find the start of an orbit, and save it as an mcr */
    if (BISSET(saucy->bit_contents, i)) {
      continue;
    }
    BSET(saucy->bit_contents, i);
    BSET(MCR(saucy->mcr, saucy->saved_auto_num, saucy->vertex_num), i);
    
    /* If the element is fixed, save as fixed and continue */
    if (saucy->gamma[i] == i) {
      BSET(FIX(saucy->fix, saucy->saved_auto_num, saucy->vertex_num), i);
      continue;
    }

    /* Find all other elements in the orbit */
    for (j = saucy->gamma[i]; j != i; j = saucy->gamma[j]) {
      BSET(saucy->bit_contents, j);
    }
  }

  /* Clean up */
  BWIPE(saucy->bit_contents, saucy->vertex_num);
  ++saucy->saved_auto_num;
}

 void
saucy_search_backtrack(saucy_t *saucy)
{
  int i, j, k, t, indx, lastnonsing;
  
  /* Keep backtracking for a while */
  do {
    /* Back up from this spot to our ancestor with zeta */
    for (--saucy->lev; saucy->lev > 0; --saucy->lev) {
      BCLEAR(saucy->bit_fixed, saucy->fixed[saucy->lev]);
      if (saucy->flag && (saucy->lev > saucy->anc)) {
        continue;
      }
      break;
    }
    
    /* If we're at the top, quit */
    if (!saucy->lev) {
      return;
    }

    /* Update ancestor with zeta if we've rewound more */
    if (saucy->anc > saucy->lev) {
      saucy->anc = saucy->lev;
      saucy->grpsize_indmin = saucy->fixed[saucy->lev];
    }

    /* Recover the partition nest to this level */
    for (i = j = saucy->cell_num = 0, lastnonsing = -1; i < saucy->vertex_num; 
         j = ++i) {
      
      /* Rewind ptn */
      while (saucy->ptn[i] > saucy->lev) {
        saucy->ptn[i] = saucy->vertex_num+1;
        saucy->cell_front[saucy->lab[i++]] = j;
      }

      /* We're at the end of a cell */
      ++saucy->cell_num;
      if (i == j) {
        continue;
      }

      /* Update lengths and fronts */
      saucy->cell_len[j] = i-j;
      saucy->cell_front[saucy->lab[i]] = j;
      
      /* Update the nonsingleton list */
      saucy->prevnonsing[j] = lastnonsing;
      saucy->nextnonsing[lastnonsing] = j;
      lastnonsing = j;
    }
    
    /* Fix the end of the nonsingleton list */
    saucy->prevnonsing[saucy->vertex_num] = lastnonsing;
    saucy->nextnonsing[lastnonsing] = saucy->vertex_num;
    
    /* If we're at our gca with zeta, orbit prune with theta */
    if (saucy->lev == saucy->anc) {
      saucy_search_theta_prune(saucy); 
    } else {
      saucy_search_orbit_prune(saucy);
    }

    /* Compute |Aut(G)| found so far */
    if ((saucy->target_min == saucy->vertex_num+1) && 
        (saucy->anc == saucy->lev)) {
      indx = 0; 
      i = saucy->start[saucy->lev] - 1;

      /* Find factor */
      do {
        /* Find lab[i]'s minimum cell representative */
        for (j = saucy->lab[++i]; j != saucy->theta[j]; j = saucy->theta[j]);
        k = saucy->lab[i]; 
        while (saucy->theta[k] != j) {
          t = saucy->theta[k]; 
          saucy->theta[k] = j; 
          k = t;
        }

        /* If it is the fixed value, then increment */
        if (j == saucy->grpsize_indmin) {
          ++indx;
        }
      } while (saucy->ptn[i] > saucy->lev);

      /* Update size */
      MULTIPLY(saucy->grpsize_base, saucy->grpsize_exp, indx);
    }
    /* Quit when there's something left in the target */
  } while (saucy->target_min == saucy->vertex_num+1);
}

 void
saucy_search_theta_prune(saucy_t *saucy)
{
  int i = saucy->start[saucy->lev], j, k, t;

  /* Find the start of the target cell at this level */
  saucy->target = i;
  saucy->target_min = saucy->vertex_num+1;

  /* Find the next-biggest minimum cell representative */
  do {
    ++i;
    
    /* If we have a new candidate for minimum... */
    if ((saucy->lab[i] > saucy->fixed[saucy->lev]) && 
        (saucy->lab[i] < saucy->target_min)) {
      
      /* Find and update the minimum cell rep for this element */
      for (j = saucy->lab[i]; j != saucy->theta[j]; j = saucy->theta[j]);
      k = saucy->lab[i]; 
      while (saucy->theta[k] != j) {
        t = saucy->theta[k]; 
        saucy->theta[k] = j; 
        k = t;
      }

      /* Update if we are in fact a min cell rep */
      if (saucy->theta[saucy->lab[i]] == saucy->lab[i]) {
        saucy->target = i; 
        saucy->target_min = saucy->lab[i];
      }
    }
  }
  /* Keep going until the end of the cell */
  while (saucy->ptn[i] > saucy->lev);
}

 void
saucy_search_orbit_prune(saucy_t *saucy)
{
  int i, j, good;

  /* Start with a completely set bit vector */
  for (j = 0; j < BSIZE(saucy->vertex_num); ++j) {
    saucy->bit_contents[j] = ~0;
  }

  /* Find the mcr's of permutations that fix this node */
  for (i = 0; i < saucy->saved_auto_num; ++i) {

    /* Make sure we're a superset */
    for (j = 0, good = 1; good && j < BSIZE(saucy->vertex_num); ++j) {
      if (saucy->bit_fixed[j] & ~FIX(saucy->fix, i, saucy->vertex_num)[j]) {
        good = 0;
      }
    }
    if (!good) {
      continue;
    }

    /* The automorphism fixes this node, so update mcr */
    for (j = 0; j < BSIZE(saucy->vertex_num); ++j) {
      saucy->bit_contents[j] &= MCR(saucy->mcr, i, saucy->vertex_num)[j];
    }
  }
  
  /* Point to the start of the target cell */
  saucy->target = i = saucy->start[saucy->lev];
  saucy->target_min = saucy->vertex_num+1;
  
  /* Do some orbit pruning */
  do {
    ++i;
    
    /* If we have a new candidate for minimum... */
    if ((saucy->lab[i] > saucy->fixed[saucy->lev]) && 
        (saucy->lab[i] < saucy->target_min)) {
      
      /* Make it minimum if it is an mcr */
      if (BISSET(saucy->bit_contents, saucy->lab[i])) {
        saucy->target = i; 
        saucy->target_min = saucy->lab[i];
      }
		}
    /* Keep going until the end of the cell */
  } while (saucy->ptn[i] > saucy->lev);
  
  /* Clear the bit vector */
  BWIPE(saucy->bit_contents, saucy->vertex_num);
}

 void
saucy_search_descend(saucy_t *saucy)
{
  int temp;
  
  /* Put the new minimum value into the singleton cell */
  saucy->lab[saucy->start[saucy->lev]] = saucy->lab[saucy->target];
  saucy->lab[saucy->target] = saucy->fixed[saucy->lev];
  saucy->target = saucy->start[saucy->lev];

  /* Mark the new element as fixed */
  saucy->fixed[saucy->lev] = saucy->lab[saucy->target];
  BSET(saucy->bit_fixed, saucy->fixed[saucy->lev]);
  
  /* Split the singleton cell again, and refine */
  saucy->ptn[saucy->target] = saucy->lev + 1; 
  ++saucy->lev; 
  ++saucy->cell_num;
  
  /* Update cell fronts */
  for (temp = saucy->target+1; 
       temp <= saucy->target + saucy->cell_len[saucy->target]; ++temp) {
    saucy->cell_front[saucy->lab[temp]] = saucy->target + 1;
  }

  /* Update the nonsingleton list */
  if (saucy->cell_len[saucy->target] == 1) {
    saucy->nextnonsing[saucy->prevnonsing[saucy->target]] = 
      saucy->nextnonsing[saucy->target];
    saucy->prevnonsing[saucy->nextnonsing[saucy->target]] = 
      saucy->prevnonsing[saucy->target];
  } else {
    saucy->nextnonsing[saucy->target+1] = saucy->nextnonsing[saucy->target];
    saucy->prevnonsing[saucy->target+1] = saucy->prevnonsing[saucy->target];
    saucy->nextnonsing[saucy->prevnonsing[saucy->target]] = saucy->target+1;
    saucy->prevnonsing[saucy->nextnonsing[saucy->target]] = saucy->target+1;
  }

  /* Update cell lengths */
  saucy->cell_len[saucy->target+1] = saucy->cell_len[saucy->target] - 1;
  saucy->cell_len[saucy->target] = 0;
  
  /* Descend once again */
  saucy_refine(saucy);
}

 void
saucy_search_min_target(saucy_t *saucy)
{
  int i, size, min, color, next, target_size = saucy->vertex_num+1;
  
  /* Start with a bogus value */
  saucy->target = saucy->vertex_num+1;
  
  /* Search for the smallest non-trivial cell */
  for (i = saucy->nextnonsing[-1]; i < saucy->vertex_num; i = next) {
    
    /* Find our size and the next thing in line */
    next = saucy->nextnonsing[i];
    size = saucy->cell_len[i] + 1;

    
    /* Skip singletons (in the front, anyway) */
    if (size == 1) {
      continue;
    }
    
    /* Skip anything that isn't an improvement */
    if (size >= target_size) {
      continue;
    }
    
    /* Find the minimum value in this cell */
    color = min = i;
    do { 
      if (saucy->lab[++i] < saucy->lab[min]) {
        min = i; 
      }
    } while (saucy->ptn[i] > saucy->lev);

    /* Update target */
    target_size = size;
    saucy->target_min = min;
    saucy->target = color;

    /* Shortcut length 2 */
    if (target_size == 2) {
      break;
    }
  }
}

 void
saucy_search_target_cell(saucy_t *saucy)
{
  saucy_search_min_target(saucy);
}

 void
saucy_search_is_automorphism(saucy_t *saucy)
{
  int i, j, l, *cycle_data, length;
  saucy_permutation_t *permutation;
  saucy_cycle_t *cycle;

  /* Iterate through the vertices */
  for (i = 0, saucy->flag = 1; saucy->flag && (i < saucy->vertex_num); ++i) {
    
    /* Skip equivalent mappings */
    if (i == saucy->gamma[i]) {
      continue;
    }
    
    /* Construct bit vector for this adjacency list */
    for (j = saucy->adj[i]; j < saucy->adj[i+1]; ++j) {
      BSET(saucy->bit_contents, saucy->gamma[saucy->edg[j]]);
    }
    
    /* Check mapping */
    for (j = saucy->adj[saucy->gamma[i]]; j < saucy->adj[saucy->gamma[i]+1]; 
         ++j) {
      if (!BISSET(saucy->bit_contents, saucy->edg[j])) {
        saucy->flag = 0; 
        break;
      }
    }
    
    /* Clear the bit vector */
    for (j = saucy->adj[i]; j < saucy->adj[i+1]; ++j) {
      BERASE(saucy->bit_contents, saucy->gamma[saucy->edg[j]]);
    }
  }
  
  /* Skip out if we're not valid */
  if (!saucy->flag) {
    return;
  }
  
  /* Print gap mode prefix */
  if (saucy->gap_mode && !saucy->quiet_mode) {
    (void) fprintf(stdout, (saucy->permutations == 0) ? "\n" : ",\n");
  }

  permutation = SAUCY_MALLOC(1, saucy_permutation_t);
  if (permutation == NULL) {
    //(void) fprintf(stderr, "permutation ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  permutation->next_permutation = saucy->permutations;
  permutation->cycle_num = 0;
  permutation->cycles = 0;
  saucy->permutations = permutation;
  
  /* Print the automorphism */
  saucy_t::Generator generator;
  for (i = 0; i < saucy->vertex_num; ++i) {
    
    /* Mark elements we've already seen */
    if (BISSET(saucy->bit_contents, i)) {
      continue;
    }
    BSET(saucy->bit_contents, i);
    
    /* Skip fixed elements */
    if (saucy->gamma[i] == i) {
      continue;
    }

    saucy_t::Orbit	orbit;
    orbit.push_back(i);
    /*if (!saucy->quiet_mode) {
      (void) fprintf(stdout, "(%d", saucy->gap_mode ? i+1 : i);
    }*/
    
    /* Mark and print elements in this orbit */
    for (j = saucy->gamma[i], l = 1; j != i; j = saucy->gamma[j], ++l) {
      BSET(saucy->bit_contents, j);
      orbit.push_back(j);/*
      if (!saucy->quiet_mode) {
        (void) putchar(saucy->gap_mode ? ',' : ' ');
        (void) fprintf(stdout, "%d", saucy->gap_mode ? j+1 : j);
      }*/
    }

    /* Create saved cycle info */
    length = sizeof(saucy_cycle_t) + (sizeof(int) * (l - 1));
    cycle = (saucy_cycle_t *) SAUCY_MALLOC(length, char);
    if (cycle == NULL) {
      //(void) fprintf(stderr, "cycle ran out of memory\n");
      //exit(EXIT_FAILURE);
    	throw std::bad_alloc();
    }
    permutation->cycle_num++;
    cycle->next_cycle = permutation->cycles;
    permutation->cycles = cycle;
    cycle->cycle_size = l;
    
    l = 0;
    cycle_data = (int *) &cycle->cycle;
    cycle_data[l++] = i;
    for (j = saucy->gamma[i]; j != i; j = saucy->gamma[j]) {
      cycle_data[l++] = j;
    }
    
    /* Finish off the orbit */
    generator.push_back(orbit);
    /*if (!saucy->quiet_mode) {
      (void) fprintf(stdout, ")");
    }*/
  }
  
  /* Clean up after ourselves */
 /* if (!saucy->gap_mode && !saucy->quiet_mode) {
    (void) fprintf(stdout, "\n");
  }*/
  saucy->generators.push_back(generator);
  BWIPE(saucy->bit_contents, saucy->vertex_num);

#if 0
  (void) fflush(stdout);
#endif
}

/***** Private data functions *****/

 void
saucy_data_init(saucy_t *saucy)
{

  if (saucy->file != NULL) {
    /* Initialize graph structure */
    if (saucy->gap_mode) {
      saucy_data_read_gap(saucy);
      if (!saucy->quiet_mode) {
        (void) putchar('[');
      }
    } else {
      saucy_data_read_amorph(saucy);
    }
    (void) fclose(saucy->file);
    saucy->file = NULL;
  } else {
    if (!saucy->quiet_mode) {
      (void) putchar('[');
    }
  }
}

 void
saucy_data_cleanup(saucy_t *saucy)
{
  saucy_permutation_t *permutation, *tmp_permutation;
  saucy_cycle_t *cycle, *tmp_cycle;

  if (saucy->gap_mode && !saucy->quiet_mode) {
    (void) fprintf(stdout, "\n]\n");
  }
  
  /* Free everything */
  if (saucy->adj) {
    SAUCY_FREE(saucy->adj);
  }
  if (saucy->edg) {
    SAUCY_FREE(saucy->edg);
  }
  if (saucy->lab) {
    SAUCY_FREE(saucy->lab);
  }
  if (saucy->ptn) {
    SAUCY_FREE(saucy->ptn);
  }
  if (saucy->permutations) {
    permutation = saucy->permutations;
    while(permutation) {
      tmp_permutation = permutation;
      cycle = permutation->cycles;
      while(cycle) {
        tmp_cycle = cycle;
        cycle = cycle->next_cycle;
        SAUCY_FREE(tmp_cycle);
      }
      permutation = permutation->next_permutation;
      SAUCY_FREE(tmp_permutation);
    }
  }
}

 void
saucy_data_mark(saucy_t *saucy, int x)
{
  int i, cf;
  
  /* Mark connections */
  for (i = saucy->adj[x]; i < saucy->adj[x+1]; ++i) {
    saucy->stuff[saucy->edg[i]] = 1;
    cf = saucy->cell_front[saucy->edg[i]];
    
    /* Add nonsingleton cell fronts to the list */
    if (saucy->cell_len[cf] && !saucy->cell_mark[cf]) {
      saucy->cell_mark[cf] = 1;
      saucy->cell_list[saucy->cell_list_num++] = cf;
    }
  }
}

 void
saucy_data_unmark(saucy_t *saucy, int x)
{
  int i;
  
  /* Clear the cell contents */
  for (i = saucy->adj[x]; i < saucy->adj[x+1]; ++i) {
    saucy->stuff[saucy->edg[i]] = 0;
  }
}

 void
saucy_data_count(saucy_t *saucy, int x)
{
  int i, cf;
  
  /* Count connections */
  for (i = saucy->adj[x]; i < saucy->adj[x+1]; ++i) {
    
    /* Find the cell front */
    cf = saucy->cell_front[saucy->edg[i]];
    
    /* Skip singletons */
    if (!saucy->cell_len[cf]) {
      continue;
    }
    
    /* Increment the count for this vertex */
    ++saucy->cell_count[saucy->edg[i]];
    
    /* Add this cell to the refine list */
    if (!saucy->cell_mark[cf]) {
      saucy->cell_mark[cf] = 1;
      saucy->cell_list[saucy->cell_list_num++] = cf;
    }
  }
}

 void
saucy_data_read_amorph(saucy_t *saucy)
{
  fpos_t fpos;
  int i, j, k, partition_num, vertex_num, edge_num;
  FILE *file;
  
  file = saucy->file;

  /* Read the sizes */
  (void) fscanf(file, "%d %d %d", &vertex_num, &edge_num, &partition_num);
  saucy_graph_create(saucy, vertex_num, edge_num);
  
  /* Insert separators in partition nest */
  for (i = 1, j = 0; i < partition_num; ++i, j = k) {
    (void) fscanf(file, "%d", &k);
    saucy_graph_insert_partition_separator(saucy, k);
  }

  /* Um, hello... this is implicit in the amorph syntax */
  saucy_graph_insert_partition_separator(saucy, vertex_num);
  
  /* Remember this spot in the file */
  (void) fgetpos(file, &fpos);
  
  /* Count the size of each adjacency list */
  for (i = 0; i < saucy->edge_num; ++i) {
    (void) fscanf(file, "%d %d", &j, &k);
    ++saucy->adj[j]; 
    ++saucy->adj[k];
  }
  
  /* Fix that */
  saucy_graph_vertex_complete(saucy);
  
  /* Go back to the front of the edges */
  (void) fsetpos(file, &fpos);
  
  /* Insert adjacencies */
  for (i = 0; i < saucy->edge_num; ++i) {
    (void) fscanf(file, "%d %d", &j, &k);
    saucy_graph_insert_edge(saucy, j, k);
  }
  
  /* Fix that too */
  saucy_graph_edge_complete(saucy);
}

 void
saucy_data_read_gap(saucy_t *saucy)
{
  fpos_t fpos;
  int i, j, k, edge_num, vertex_num;
  char c;
  FILE *file;

  file = saucy->file;

  /* Skip leading chaff */
  do {
    while (fgetc(file) != '[');
  } while (fgetc(file) != '[');
  (void) ungetc('[', file);

  /* Remember this spot in the file */
  (void) fgetpos(file, &fpos);
  
  /* First pass: count the edges */
  edge_num = 0; 
  do {
    ++edge_num; 
    (void) fscanf(file, "[%d,%d]", &j, &k);
  } while (fgetc(file) == ',');
  
  /* Read the number of vertices */
  (void) fscanf(file, ", %d)), [", &vertex_num);
  
  /* Do the allocation */
  saucy_graph_create(saucy, vertex_num, edge_num);

  /* Go back to the front of the edges */
  (void) fsetpos(file, &fpos);
  
  /* Second pass: count adjacencies for each vertex */
  do {
    (void) fscanf(file, "[%d,%d]", &j, &k);
    ++saucy->adj[j-1]; 
    ++saucy->adj[k-1];
  } while (fgetc(file) == ',');

  /* Fix that */
  saucy_graph_vertex_complete(saucy);
  
  /* Go back again */
  (void) fsetpos(file, &fpos);

  /* Third pass: actually insert the edges */
  do {
    (void) fscanf(file, "[%d,%d]", &j, &k);
    saucy_graph_insert_edge(saucy, j-1, k-1);
  } while (fgetc(file) == ',');

  /* Fix that too */
  saucy_graph_edge_complete(saucy);

  /* Read the number of vertices (again) */
  (void) fscanf(file, ", %d)), [", &vertex_num);
  
  /* We've read the edges; now read the coloring */
  i = -1; 
  do {

    /* Eat a left bracket */
    (void) fgetc(file);
    
    /* Try eating a right bracket; otherwise keep going */
    c = fgetc(file);
    if (c == ']') {
      continue;
    }
    (void) ungetc(c, file);
    
    /* Read in the entire color */
    do {
      (void) fscanf(file, "%d", &j);
      saucy_graph_insert_color(saucy, ++i, j);
    } while (fgetc(file) == ',');

    /* Mark the end of the cell and add to alpha */
    saucy_graph_insert_partition_separator(saucy, i+1);
  } while (fgetc(file) == ',');
}

 void
saucy_data_vertex_complete(saucy_t *saucy)
{
  int val, sum, i;

  /* Translate adj values to real locations */
  val = saucy->adj[0]; 
  sum = 0; 
  saucy->adj[0] = 0;
  for (i = 1; i < saucy->vertex_num; ++i) {
    sum += val;
    val = saucy->adj[i];
    saucy->adj[i] = sum;
  }
}

 void
saucy_data_edge_complete(saucy_t *saucy)
{
  int i;
  
  /* Translate again-broken sizes to adj values */
  for (i = saucy->vertex_num-1; i > 0; --i) {
    saucy->adj[i] = saucy->adj[i-1];
  }
  saucy->adj[0] = 0;
  saucy->adj[saucy->vertex_num] = 2 * saucy->edge_num;
}

/***** Private refinement functions *****/

 void
saucy_refine_init(saucy_t *saucy)
{
  int i, j;

  /* Allocate refinement stuff */
  saucy->alpha = SAUCY_MALLOC(saucy->vertex_num, int);
  if (saucy->alpha == NULL) {
    //(void) fprintf(stderr, "saucy->alpha ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->beta = SAUCY_MALLOC(saucy->vertex_num, int);
  if (saucy->beta == NULL) {
    //(void) fprintf(stderr, "saucy->beta ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->abmark = SAUCY_CALLOC(saucy->vertex_num, char);
  if (saucy->abmark == NULL) {
    //(void) fprintf(stderr, "saucy->abmark ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->cell_front = SAUCY_CALLOC(saucy->vertex_num, int);
  if (saucy->cell_front == NULL) {
    //(void) fprintf(stderr, "saucy->front ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->cell_len = SAUCY_MALLOC(saucy->vertex_num, int);
  if (saucy->cell_len == NULL) {
    //(void) fprintf(stderr, "saucy->cell_len ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->alpha_num = saucy->beta_num = 0;
  
  /* Other allocations */
  saucy->stuff = SAUCY_CALLOC((saucy->vertex_num+1), char);
  if (saucy->stuff == NULL) {
    //(void) fprintf(stderr, "saucy->stuff ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->bucket = SAUCY_MALLOC((saucy->vertex_num+2), int);
  if (saucy->bucket == NULL) {
    //(void) fprintf(stderr, "saucy->bucket ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->vertex_count = SAUCY_MALLOC((saucy->vertex_num+1), int);
  if (saucy->vertex_count == NULL) {
    //(void) fprintf(stderr, "saucy->vertex_count ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->cell_count = SAUCY_CALLOC(saucy->vertex_num, int);
  if (saucy->cell_count == NULL) {
    //(void) fprintf(stderr, "saucy->cell_count ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->cell_mark = SAUCY_CALLOC(saucy->vertex_num, char);
  if (saucy->cell_mark == NULL) {
    //(void) fprintf(stderr, "saucy->cell_mark ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->cell_list = SAUCY_MALLOC(saucy->vertex_num, int);
  if (saucy->cell_list == NULL) {
    //(void) fprintf(stderr, "saucy->cell_list ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->cell_list_num = 0;
  
  /* Allocate nonsingleton lists */
  saucy->nextnonsing = SAUCY_MALLOC((saucy->vertex_num+1), int);
  if (saucy->nextnonsing == NULL) {
    //(void) fprintf(stderr, "saucy->nextnonsing ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  saucy->prevnonsing = SAUCY_MALLOC((saucy->vertex_num+1), int);
  if (saucy->prevnonsing == NULL) {
    //(void) fprintf(stderr, "saucy->prevnonsing ran out of memory\n");
    //exit(EXIT_FAILURE);
	  throw std::bad_alloc();
  }
  
  /* Update refinement stuff based on initial partition */
  for (i = j = saucy->cell_num = 0; i < saucy->vertex_num; 
       j = i, ++saucy->cell_num) {
    do { 
      saucy->cell_front[saucy->lab[i]] = j; 
    } while (saucy->ptn[i++]);
    saucy->cell_len[j] = i - j - 1;
    refine_add_induce(saucy, !saucy->cell_len[j], j);
  }
  
  /* nextnonsing[-1] is the first nonnegative cell, prevnonsing[n] is last */
  ++saucy->nextnonsing;
  
  /* Prepare lists based on cell lengths */
  for (i = 0, j = -1; i < saucy->vertex_num; i += saucy->cell_len[i] + 1) {
    if (!saucy->cell_len[i]) {
      continue;
    }
    saucy->prevnonsing[i] = j;
    saucy->nextnonsing[j] = i;
    j = i;
  }

  /* Fix the end */
  saucy->prevnonsing[saucy->vertex_num] = j;
  saucy->nextnonsing[j] = saucy->vertex_num;
}

 void
saucy_refine_cleanup(saucy_t *saucy)
{
  /* Free everything */
  if (saucy->alpha) {
    SAUCY_FREE(saucy->alpha); 
  }
  if (saucy->beta) {
    SAUCY_FREE(saucy->beta); 
  }
  if (saucy->abmark) {
    SAUCY_FREE(saucy->abmark);
  }
  if (saucy->stuff) {
    SAUCY_FREE(saucy->stuff); 
  }
  if (saucy->bucket) {
    SAUCY_FREE(saucy->bucket); 
  }
  if (saucy->vertex_count) {
    SAUCY_FREE(saucy->vertex_count);
  }
  if (saucy->cell_front) {
    SAUCY_FREE(saucy->cell_front); 
  }
  if (saucy->cell_len) {
    SAUCY_FREE(saucy->cell_len);
  }
  if (saucy->cell_count) {
    SAUCY_FREE(saucy->cell_count); 
  }
  if (saucy->cell_mark) {
    SAUCY_FREE(saucy->cell_mark); 
  }
  if (saucy->cell_list) {
    SAUCY_FREE(saucy->cell_list);
  }
  if (saucy->nextnonsing) {
    SAUCY_FREE(--saucy->nextnonsing); 
  }
  if (saucy->prevnonsing) {
    SAUCY_FREE(saucy->prevnonsing);
  }
}

 void
saucy_refine(saucy_t *saucy)
{
  /* Add target to beta */
  saucy->beta[saucy->beta_num++] = saucy->target;
  saucy->abmark[saucy->target] = 1;
  
  /* Keep going until refinement stops */
  do {

    /* If discrete, clear alpha and bail */
    if (saucy->cell_num == saucy->vertex_num) {
      while (saucy->alpha_num) { 
        saucy->abmark[saucy->alpha[--saucy->alpha_num]] = 0; 
      }
      while (saucy->beta_num) { 
        saucy->abmark[saucy->beta[--saucy->beta_num]] = 0; 
      }
      break;
    }

    /* Look for something else to refine on */
    if (saucy->beta_num) {
      saucy->front = saucy->beta[--saucy->beta_num];
    } else if (saucy->alpha_num) {
      saucy->front = saucy->alpha[--saucy->alpha_num];
    } else {
      break;
    }
    saucy->abmark[saucy->front] = 0;
    
    /* Perform the appropriate refinement operation */
    if (saucy->ptn[saucy->front] <= saucy->lev) {
      saucy_refine_singleton(saucy);
    } else {
      saucy_refine_nonsingle(saucy);
    }
  } while(saucy->cell_num <= saucy->vertex_num);
}

 void
saucy_refine_singleton(saucy_t *saucy)
{
  int cf, cb, ff, fb, tmp, save_stuff;
  
  /* Mark connections */
  saucy_data_mark(saucy, saucy->lab[saucy->front]);
  saucy_refine_sort_cell_list(saucy->cell_list, saucy->cell_list_num);

  /* Now iterate over the marked cells */
  while (saucy->cell_list_num) {

    /* Get the cell boundaries */
    cf = saucy->cell_list[--saucy->cell_list_num];
    cb = cf + saucy->cell_len[cf];
    ff = cf; 
    fb = cb;

    /* Cheat: set stuff[lab[cb+1]] so we can tighten this loop */
    save_stuff = saucy->stuff[saucy->lab[cb+1]];
    saucy->stuff[saucy->lab[cb+1]] = 1;
    
    /* Move from opposite ends, sorting notconn->conn */
    while (ff <= fb) {
      if (!saucy->stuff[saucy->lab[ff]]) {
        ++ff;
        while (!saucy->stuff[saucy->lab[ff]]) ++ff;
      } else {
        tmp = saucy->lab[ff];
        saucy->lab[ff] = saucy->lab[fb];
        saucy->lab[fb] = tmp;
        --fb;
			}
    }
    
    /* Uncheat */
    saucy->stuff[saucy->lab[cb+1]] = save_stuff;
    
    /* Check if we split */
    if ((fb >= cf) && (ff <= cb)) {
      
      /* We did, so mark the new level */
      saucy->ptn[fb] = saucy->lev; 
      ++saucy->cell_num;
      
      /* Add the new guy if we were already in alpha */
      if (saucy->abmark[cf] || fb-cf >= cb-ff) {
        refine_add_induce(saucy, (ff == cb), ff);
      } else {
        /* Otherwise, add the smaller */
        refine_add_induce(saucy, (fb == cf), cf);
      }

      /* Update cell lengths */
      saucy->cell_len[cf] = fb - cf;
      saucy->cell_len[ff] = cb - ff;
      
      /* Update cell front pointers */
      while (cb >= ff) {
        saucy->cell_front[saucy->lab[cb--]] = ff;
      }
      
      /* Update nonsingleton list */
      if (saucy->cell_len[ff]) {
        saucy->nextnonsing[ff] = saucy->nextnonsing[cf];
        saucy->prevnonsing[saucy->nextnonsing[cf]] = ff;
        saucy->prevnonsing[ff] = cf;
        saucy->nextnonsing[cf] = ff;
      }
      if (!saucy->cell_len[cf]) {
        saucy->nextnonsing[saucy->prevnonsing[cf]] = saucy->nextnonsing[cf];
        saucy->prevnonsing[saucy->nextnonsing[cf]] = saucy->prevnonsing[cf];
      }
    }
    
    /* Clear the mark */
    saucy->cell_mark[cf] = 0;
  }

  /* Clear the cell contents */
  saucy_data_unmark(saucy, saucy->lab[saucy->front]);
}

 void
saucy_refine_nonsingle(saucy_t *saucy)
{
  int back, cnt, i, cf, cb, ff, fb, max_size, max_pos, bmin, bmax;
  int max_site, lastnonsing, endnonsing;

  /* Find the end of this cell */
  back = saucy->front + saucy->cell_len[saucy->front];

  /* Iterate through all our adjacency lists */
  for (i = saucy->front; i <= back; ++i) {
    saucy_data_count(saucy, saucy->lab[i]);
  }
  saucy_refine_sort_cell_list(saucy->cell_list, saucy->cell_list_num);

  /* Iterate through all the cells we're connected to */
  while (saucy->cell_list_num) {
    
    /* Find the front and back */
    ff = cf = saucy->cell_list[--saucy->cell_list_num];
    cb = cf + saucy->cell_len[cf];
    
    /* Clear the mark */
    saucy->cell_mark[cf] = 0;
    
    /* Remember some stuff about the nonsingleton list */
    lastnonsing = saucy->prevnonsing[cf];
    endnonsing = saucy->nextnonsing[cf];
    
    /* Prepare the buckets */
    cnt = saucy->cell_count[saucy->lab[ff]];
    saucy->cell_count[saucy->lab[ff]] = 0;
    saucy->vertex_count[ff] = bmin = bmax = cnt;
    saucy->bucket[cnt] = 1;
    
    /* Iterate through the rest of the vertices */
    while (++ff <= cb) {
      cnt = saucy->cell_count[saucy->lab[ff]];
      saucy->cell_count[saucy->lab[ff]] = 0;
      
      /* Initialize intermediate buckets */
      while (bmin > cnt) {
        saucy->bucket[--bmin] = 0;
      }
      while (bmax < cnt) {
        saucy->bucket[++bmax] = 0;
      }
      
      /* Mark this count */
      ++saucy->bucket[cnt];
      saucy->vertex_count[ff] = cnt;
    }
    
    /* If they all had the same count, bail */
    if (bmin == bmax) {
      continue;
    }
    ff = fb = cf; 
    max_size = max_pos = max_site = -1;

    /* Calculate bucket locations, sizes */
    for (i = bmin; i <= bmax; ++i, ff = fb) {
      
      /* Skip empty buckets */
      if (!saucy->bucket[i]) {
        continue;
      }
      fb = ff + saucy->bucket[i];
      saucy->bucket[i] = ff;

      /* Fix max-sized bucket */
      if (fb - ff > max_size) {
        max_size = fb - ff;
        max_pos = ff;
      }
      
      /* Add everybody except first */
      if (ff != cf) {
        if (fb - ff == 1) {
          if (ff == max_pos) {
            max_site = saucy->beta_num;
          }
          saucy->beta[saucy->beta_num++] = ff;
        } else {
          if (ff == max_pos) {
            max_site = saucy->alpha_num;
          }
          saucy->alpha[saucy->alpha_num++] = ff;
        }
        saucy->abmark[ff] = 1;
      }

      /* Create new cell for everybody but last */
      if (fb <= cb) {
        saucy->ptn[fb-1] = saucy->lev; 
        ++saucy->cell_num;
      }
      
      /* Fix cell lengths */
      saucy->cell_len[ff] = fb - ff - 1;
      
      /* Update nonsingleton list */
      if (saucy->cell_len[ff]) {
        saucy->nextnonsing[lastnonsing] = ff;
        saucy->prevnonsing[ff] = lastnonsing;
        lastnonsing = ff;
      }
    }

    /* Finish the nonsingleton list */
    saucy->nextnonsing[lastnonsing] = endnonsing;
    saucy->prevnonsing[endnonsing] = lastnonsing;
    
    /* Repair the partition nest */
    for (i = cf; i <= cb; ++i) {
      saucy->gamma[saucy->bucket[saucy->vertex_count[i]]++] = saucy->lab[i];
    }
    for (i = cf; i <= cb; ++i) {
      saucy->lab[i] = saucy->gamma[i];
    }
    
    /* Clear the counts */
    for (i = cf; i <= cb; ++i) {
      saucy->cell_count[saucy->lab[i]] = 0;
    }
    
    /* Fix cell beginnings */
    for (i = ff = cf + saucy->cell_len[cf] + 1; i <= cb; ++i) {
      saucy->cell_front[saucy->lab[i]] = ff;
      if (saucy->ptn[i] <= saucy->lev) {
        ff = i + 1;
      }
    }

    /* If the front isn't marked and it isn't the largest... */
    if (!saucy->abmark[cf] && (cf != max_pos)) {
      
      /* Remove the max from its stack */
      if (saucy->ptn[max_pos] <= saucy->lev) {
        saucy->abmark[saucy->beta[max_site]] = 0;
        saucy->beta[max_site] = saucy->beta[--saucy->beta_num];
      } else {
        saucy->abmark[saucy->alpha[max_site]] = 0;
        saucy->alpha[max_site] = saucy->alpha[--saucy->alpha_num];
      }
      
      /* Add the front to its stack */
      refine_add_induce(saucy, (saucy->ptn[cf] <= saucy->lev), cf);
    }
  }
}

 void
saucy_refine_sort_cell_list(int *cell_list, int cell_list_num)
{
  int h, i, j, k;
  
  /* Shell sort, as implemented in nauty */
  j = cell_list_num / 3;
  h = 1;
  do { 
    h = 3 * h + 1; 
  } while (h < j);

  do {
    for (i = h; i < cell_list_num; ++i) {
      k = cell_list[i];
      for (j = i; cell_list[j-h] > k; ) {
        cell_list[j] = cell_list[j-h];
        if ((j -= h) < h) {
          break;
        }
      }
      cell_list[j] = k;
    }
    h /= 3;
  } while (h > 0);
}

/***** Private parse functions *****/

/*
 * parse_opt_int(min)
 * Return the integer value sitting in optarg if it is greater than or
 * equal to min; otherwise, exit with an error status.
 */

 int
saucy_parse_opt_int(int min)
{
  int val = atoi(optarg);

  if (val < min) {
    (void) fprintf(stderr, "saucy: invalid numeric argument\n");
    saucy_parse_usage();
  }
  return val;
}

/*
 * pusage()
 * Print a usage statement to standard error and exit with error.
 * This function does not return.
 */

 void
saucy_parse_usage(void)
{
  (void) fprintf(stderr, "usage: saucy [-sqzg] [-t secs] [-p count] FILE\n");
  exit(EXIT_FAILURE);
}
