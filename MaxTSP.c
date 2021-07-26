/* GRR20166365 - Diogo Paris Kraut */

/*! \file MaxTSP.c
 * \brief Driver code and functions used for Maximum TSP solver
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "graph.h"
#include "MaxTSP.h"

/*! Number of places to visit */
int n;
graph_t *g;
list_t *q;
int bestCost;
int *bestPath;
int visited_node_count = 0;
list_t *connected_to_start;
int bound = 1;
/*! Allocates and initiates a traversal tree node
 * \param in Edge we took to this state
 * \param cost Cost of the path not including 'in'
 */
path_t *initPath(edge_t *in, int *path, int cost) {
    path_t *p = malloc(sizeof(path_t));
    if(!p) {
        perror("initPath failed to allocate p");
        exit(1);
    }
    p->in = in;
    p->cost = cost;
    p->path = malloc(sizeof(int) * n);
    if(!p->path) {
        perror("initPath failed to allocate p->path");
        exit(1);
    }
    memset(p->path, -1, sizeof(int) * n );

    if(path != NULL) {
        memcpy(p->path, path, n * sizeof(int));
        for(int i = 1; i < n; i++) {
            if(p->path[i] == -1) {
                p->path[i] = in->dest->id;
                break;
            }
        }
    } else
        p->path[0] = 0;
    return p;
}



/*! Finds an edge arriving at the initial vertex whose src has id == 'id'
 * \param id ID to be searched for
 */
edge_t *edgeToZero(int id) {
    vertex_t *zero = vertexSearch(g, 0);
    node_t *aux = zero->edge_in->head;
    while(aux != NULL) {
        if(((edge_t *)aux->data)->src->id == id) {
            return ((edge_t *)aux->data);
        }
        aux = aux->next;
    }
    return NULL;
}

/*! Searches an array for 'x'
 * \param x Number to be searched for
 */
int searchArray(int *A, int x) {
    for(int i = 0; i < n; i++) {
        if(A[i] == x)
            return 1;
    }
    return 0;
}

/*! Prints path*/
void printPath(int *A) {
    for (int i = 0; i < n-1; ++i) {
        printf("%d ", A[i]+1);
    }
    printf("%d\n", A[n-1]+1);
}

/*! Copies current path to bestPath
 * \param path Current path */
void saveCurrentBestPath(int *path) {
    for(int i = 0; i < n; i++) {
        if(path[i] != -1)
            bestPath[i] = path[i];
        else {
            bestPath[i] = 0;
            return;
        }
    }
}

/*! Determines if the subGraph starting at vertex v_in has a vertex
 * with an edge to vertex 0.
 * \param v_in starting vertex for search
 * \param path current path
 * \returns 1 if has return to vertex0
 * \return 0 otherwise
 */
int subGraphReturnsToZero(vertex_t *v_in, int *path) {
    /* Alocate array of vertexes that will serve as a visited list
     * and later will be searched for an edge to vertex 0 */
    vertex_t **visited = malloc(sizeof(vertex_t *) * n);

    for(int i = 0; i < n; i++)
        visited[i] = NULL;

    /* BFS through subgraph to generate a list of vertexes from v_in
     * and that aren't in path */
    list_t *dfs_stack = initList();
    push(dfs_stack, initNode(v_in));
    vertex_t *v = NULL;
    while(dfs_stack->size > 0) {
        v = (vertex_t *)pop(dfs_stack);
        if(visited[v->id] == NULL) {
            visited[v->id] = v; // add to lsit
        }
        node_t *aux = v->edge_out->head;
        while(aux != NULL) {
            vertex_t *v_aux = (vertex_t *)((edge_t *)aux->data)->dest;
            if(visited[v_aux->id] == NULL && searchArray(path, v_aux->id) == 0) {
                push(dfs_stack, initNode(v_aux));
            }

            aux = aux->next;
        }
    }

    /* Search list */
    for(int i = 0; i < n; i++) {
        if(visited[i] != NULL) {
            if(edgeToZero(i)) {
                free(visited);
                return 1;
            }
        }
    }
    free(visited);
    return 0;

}
/*! Recursively generates all permutations of possible valid paths.
 * A valid path in one that starts and ends on vertex 0, and doesn't
 * visit any other vertex more than once.
 * At the end of the execution, bestCost and bestPath will have, respectively,
 * the highest cost and its associated path.
 * \param p current path info
 * */
void MaxTSP(path_t *p) {
    vertex_t *currentVertex;
    /* If not entry node */
    if(p->in != NULL) {
        visited_node_count++;
        /* Update cost, look for edge from current vertex to start */
        currentVertex = vertexSearch(g, p->in->dest->id);
        p->cost += p->in->val;
        edge_t *e = edgeToZero(currentVertex->id);
        if(e != NULL) {
            /* If found edge, check if this is new best path */
            if(p->cost + e->val > bestCost){
                bestCost = p->cost + e->val;
                saveCurrentBestPath(p->path);
            }
        }
    } else {
        currentVertex = vertexSearch(g, 0);
    }
    // printf("Cost %d: ", p->cost);
    // printArr(p->path);

    // printf("%d\n", visited_node_count);
    /* Add children to queue for BFS */
    node_t *aux = currentVertex->edge_out->head;
    while(aux != NULL) {
        edge_t *e_aux = (edge_t *)aux->data;
        if(!searchArray(p->path, e_aux->dest->id)) {
            #ifdef NOBOUND
                bound = 1;
            #else
                bound = subGraphReturnsToZero(e_aux->dest, p->path);
            #endif
            if(bound)
                enqueue(q, initNode(initPath(e_aux, p->path, p->cost)));
        }
        aux = aux->next;
    }

    if(q->size > 0) {
        free(p->path);
        free(p);
        MaxTSP(dequeue(q));
    }


}

int main(int argc, char const *argv[]) {
	/* INPUT */
    FILE *fp;
    if(argc == 2)
        fp = fopen(argv[1], "r");
    else
        fp = stdin;

    if(!fp) {
        printf("fopen err\n");
        exit(1);
    }
    setbuf(stdout, NULL);
    // clock_t init_time;

    fscanf(fp, "%d", &n);

    int i,j,aux;
    g = initGraph();
    bestCost = -1;
    q = initList();
    connected_to_start = initList();
    bestPath = malloc(sizeof(int) * n);

    for(i = 0; i < n; i++) {
        enqueue(g->vertexes, initNode(initVertex(i)));
    }


	/* Read graph */
    for(i = 0; i < n; i++) {
    	for(j = i; j < n; j++) {
            if(i != j) {
                fscanf(fp, "%d", &aux);
                if(aux > 0) {
                    vertex_t *vi = vertexSearch(g, i);
                    vertex_t *vj = vertexSearch(g, j);
                    edge_t *e = initEdge(vi, vj, aux);
                    // printf("Edge from %d to %d. Cost = %d\n", e->src->id, e->dest->id, e->val);
                    enqueue(g->edges, initNode(e));
                    e = initEdge(vj, vi, aux);
                    enqueue(g->edges, initNode(e));
                }

            }
        }
    }
    fclose(fp);
    // init_time = clock();
    // printGraph(g);
    MaxTSP(initPath(NULL, NULL, 0));
    printf("%d\n", bestCost);
    // printf("Best path: ");
    printPath(bestPath);
    // printf("Total nodes visited: %d\n", visited_node_count);
    // printf("Run time: %ld\n", clock() - init_time);
	return 0;
}