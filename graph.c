/* Diogo Paris Kraut - GRR 20166365 */
/*!\file graph.c *
 * \brief Functions for manipulating a graph (and the list/stack)
 */
#include "graph.h"


/*! Allocates and initiates a transaction list */
list_t *initList() {
    list_t *l = malloc(sizeof(list_t));
    if(!l) {
        perror("initList failed to allocate l");
        exit(1);
    }

    l->head = NULL;
    l->size = 0;
    return l;
}

/*! Allocates and initiates a data node *
 *\param data Pointer to structure/var that holds data to be added to node
 */
node_t *initNode(void *data) {
    node_t *n = malloc(sizeof(node_t));
    if(!n) {
        perror("initNode failed to allocate n");
        exit(1);
    }

    n->data = data;
    n->next = NULL;

    return n;
}

/*! Allocates and initiates a vertex *
 *\param id Vertex 'id'
 */
vertex_t *initVertex(int id) {
    vertex_t *v = malloc(sizeof(vertex_t));
    if(!v) {
        perror("initVertex failed to allocate v");
        exit(1);
    }


    v->edge_in  = initList();
    v->edge_out = initList();
    v->visited = 0;
    v->id = id;

    return v;
}

/*! Allocates and initiates an edge     
 *\param src Source vertex of edge      
 *\param dest Destination vertex of edge
 * \param val Value of edge
 */
edge_t *initEdge(vertex_t *src, vertex_t *dest, int val) {
    edge_t *e = malloc(sizeof(edge_t));
    if(!e) {
        perror("initEdge failed to allocate e");
        exit(1);
    }


    e->src = src;
    e->dest = dest;
    e->val = val;
    enqueue(e->src->edge_out, initNode(e));
    enqueue(e->dest->edge_in, initNode(e));

    return e;
}

/*! Allocates and initiates a graph */
graph_t *initGraph() {
    graph_t *g = malloc(sizeof(graph_t));
    if(!g) {
        perror("initGraph failed to allocate g");
        exit(1);
    }

    g->vertexes = initList();
    g->edges  = initList();
    return g;
}

/*! Enqueues node 'n' to list 'l'
 *\param l Pointer to a list
 *\param n Pointer to a node 
 */
void enqueue(list_t *l, node_t *n) {
    if(!l || !n) 
        return;

    if(l->head == NULL) {
        l->head = n;
        l->tail = n;
    } else {
        l->tail->next = n;
        l->tail = n;
    }

    n->next = NULL;
    l->size++;
}

void printGraph(graph_t *g) {
    node_t *aux = g->vertexes->head;

    while(aux != NULL) {
        vertex_t *v = (vertex_t *)aux->data;
        node_t *aux2 = v->edge_out->head;
        while(aux2 != NULL) {
            edge_t *e = (edge_t *)aux2->data;
            printf("%d --%d--> %d\n", v->id, e->val, e->dest->id);
            aux2= aux2->next;
        }

        aux = aux->next;
    }
}

/*! Dequeues head from list 'l' *
 * \param l Pointer to list
 * \return Pointer to node removed
 * \return NULL if couldnt remove
 */
void *dequeue(list_t *l) {
    if(!l || !l->head)
        return NULL;

    node_t *aux;
    void *ret;
    aux = l->head;

    l->head = l->head->next;
    aux->next = NULL;
    ret = aux->data;
    free(aux);
    l->size--;
    return ret;
}

/*! Searches graph 'g' for vertex with id 'id'
 *\param g Pointer to graph                    
 *\param id Id of vertex to look for           
 */
vertex_t *vertexSearch(graph_t *g, int id) {
    node_t *n_aux;
    vertex_t *v_aux;

    n_aux = g->vertexes->head;
    while(n_aux != NULL) {
        v_aux = (vertex_t *)n_aux->data;
        if(v_aux->id == id)
            return v_aux;
        n_aux = n_aux->next;
    }

    return NULL;
}

/*! Searches graph 'g' for edge 'e'         
 *\param g Pointer to graph                 
 *\param e Pointer to edge to be looked for 
 */
int edgeSearch(graph_t *g, edge_t *e) {
    node_t *n_aux = g->edges->head;
    edge_t *e_aux;

    while(n_aux != NULL) {
        e_aux = (edge_t *)n_aux->data;
        if(e_aux->src == e->src && e_aux->dest == e->dest)
            return 1;
        n_aux = n_aux->next;
    }

    return 0;
}

/*! Stack operation push    
 *\param s Pointer to stack 
 *\param n Pointer to node  
 */
void push(list_t *s, node_t *n) {
    node_t *n_aux = s->head;
    if(n_aux != NULL) {
        n->next = n_aux;
        s->head = n;
    } else {
        s->head = n;
    }
    s->size++;
}

/*! Stack operation pop     
 *\param s Pointer to stack 
 */
node_t *pop(list_t *s) {
    void *ret = s->head->data;
    node_t *freeN = s->head;
    s->head = s->head->next;
    s->size --;
    free(freeN);
    return ret;
}