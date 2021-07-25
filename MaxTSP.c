/* GRR20166365 - Diogo Paris Kraut */

/*! \file MaxTSP.c
 * \brief Driver code and functions used for Maximum TSP solver
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
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
int bound = 0;
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

void printArr(int *A) {
    for (int i = 0; i < n-1; ++i) {
        printf("%d ", A[i]);
    }
    printf("%d\n", A[n-1]);
}

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

int subGraphReturnsToZero(vertex_t *v_in, int *path) {
    // int *visited = calloc(n, sizeof(int));

    vertex_t **visited = malloc(sizeof(vertex_t *) * n);
    for(int i = 0; i < n; i++)
        visited[i] = NULL;

    list_t *dfs_stack = initList();
    push(dfs_stack, initNode(v_in));
    vertex_t *v = NULL;
    while(dfs_stack->size > 0) {
        v = (vertex_t *)pop(dfs_stack);
        if(visited[v->id] == NULL) {
            visited[v->id] = v;
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
/*BOUND IDEA -----------------
    calculate average cost of all edges, check if traversing down current edge
    is better than average 
------------------------------
    make subgraph that starts on edge 'in'. Check if any of the nodes in 
    subgraph can go back to original graph through a node not in path.
    if not, cull all of it
    ---- 
    maybe mantain a list of vertexes with return to 0 and one without*/
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
            #ifdef BOUND
                bound = subGraphReturnsToZero(e_aux->dest, p->path);
            #else
                bound = 1;
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
    printGraph(g);
    MaxTSP(initPath(NULL, NULL, 0));
    printf("Best path: ");
    printArr(bestPath);
    printf("Total nodes visited: %d\n", visited_node_count);
	return 0;
}