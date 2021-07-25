/* Diogo Paris Kraut - GRR 20166365 */
/*! \file MaxTSP.h
 * \brief Structures and prototypes for working with the max TSP
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*! Structure to hold path info */
typedef struct path_t {
	edge_t *in; 		 /*!< Edge we took to get to this vertex */
	int *path;			 /*!< Array representing path including current vertex */
	int cost;			 /*!< Cost of the path so far */
} path_t;

path_t *initPath(edge_t *in, int *path, int cost);
edge_t *edgeToZero(int id);
int searchArray(int *A, int x);
void MaxTSP(path_t *p);