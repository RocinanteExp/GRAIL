#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "labelling.h"
#include "graph.h"
#include "bitmap.h"
#include <stdint.h>
#define MAX_THREADS 4
#define DEBUG 0 
// MACRO for MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

uint32_t r;
uint32_t global_rank = 0;
//struct thread argument
struct thread_argument 
{
    Graph* graph;
    uint32_t idx;
    uint32_t rank;
};
/*struct thread_argument_roots 
{
    Graph* graph;
    uint32_t idx;
    uint32_t* rank;
    uint32_t root_id;
    pthread_spinlock_t* lock;
    Bitmap* visited_nodes;
};*/
static void *setting_intervals(void *thread_argument);
static void  graph_random_visit(Graph *graph,Bitmap* visited_nodes,uint32_t node_id, uint32_t idx, uint32_t* rank);
static void ramdom_shuffle(uint32_t* vec,uint32_t size);
//static void *setting_intervals_roots(void *thread_argument);
//static void  graph_random_visit_with_lock(Graph *graph,Bitmap* visited_nodes,uint32_t node_id, uint32_t idx, uint32_t* rank, pthread_spinlock_t* lock);
// function to get a random number for roots
static uint32_t get_ramdom_order_roots(Graph *graph, Bitmap *roots_map) 
{
#if !DEBUG
    uint32_t j = rand_r(pthread_self()) % graph->num_root_nodes;
#else
    uint32_t j = 0;
#endif
    while(bitmap_test_bit(roots_map, j))
    {
        j = (j+1) % graph->num_root_nodes;
    }    
    return j;
}

// function to get a random number for node childrens
static uint32_t get_ramdom_order_children(Node *node, Bitmap * child_map) 
{
#if !DEBUG
    uint32_t j = rand_r(pthread_self()) % node->num_children;
#else
    uint32_t j = 0;
#endif
    while(bitmap_test_bit(child_map,j))
    {
        j=(j+1)%node->num_children;
    }    
    return j;
}
// Random visit fuction that implementi the get_ramdom_order_children fuction idx indicates the current label that is being set for the node
static void  graph_random_visit_sequential(Graph *graph,Bitmap* visited_nodes,uint32_t node_id, uint32_t idx)
{
    int j = 0;
    uint32_t num_childrens = 0;
    uint32_t rc = UINT32_MAX;
    Bitmap* child_map;
    Node* node;
    if(bitmap_test_bit(visited_nodes,node_id))
    {
        return;
    }
    node = graph->nodes[node_id];
    bitmap_set_bit(visited_nodes, node_id);
    child_map = bitmap_create(node->num_children);
    while(num_childrens < node->num_children)
    {
        num_childrens++;
        j = get_ramdom_order_children(node, child_map);
        bitmap_set_bit(child_map, j);
        graph_random_visit_sequential(graph,visited_nodes,node->children[j],idx);
    }
    
    bool first = true; 
    for(j=0; j<node->num_children; j++)
    {
        if(first){
            first = false;
            rc = MIN(rc,graph->nodes[node->children[j]]->intervals[idx].left);
            continue;
        }

        int tmp_rc = MIN(rc,graph->nodes[node->children[j]]->intervals[idx].left);
        if(tmp_rc < rc)
            rc = tmp_rc;

    }

    node->intervals[idx].left = MIN(global_rank,rc);
    node->intervals[idx].right = global_rank;
    global_rank++;
    bitmap_destroy(child_map);
}
void graph_randomize_labelling_sequential(Graph *graph, uint32_t num_intevals)
{
    uint32_t idx = 0;
    uint32_t num_roots = 0;
    Bitmap* roots_map = bitmap_create(graph->num_root_nodes);
    Bitmap* nodes_map = bitmap_create(graph->num_nodes);
    if(roots_map == NULL)
    {
        fprintf(stderr, "ERROR IN ALLOCATING BITMAP IN: graph_randomize_labelling\n");
        exit(-1);
    }
    if(nodes_map == NULL)
    {
        fprintf(stderr, "ERROR IN ALLOCATING BITMAP IN: graph_randomize_labelling\n");
        exit(-1);
    }
    // cycle for  the number of labels for each node
    for(idx=0;idx<num_intevals;idx++)
    {
        //inizialize values
        r=1;
        num_roots=0;
        bitmap_clear_all(roots_map);
        bitmap_clear_all(nodes_map);
        uint32_t i=0;
        //visit all roots
        while(num_roots<graph->num_root_nodes)
        {
            i = get_ramdom_order_roots(graph,roots_map);
            bitmap_set_bit(roots_map,i);
            num_roots++;
            graph_random_visit_sequential(graph,nodes_map,graph->root_nodes[i],idx);
        }
    }
    bitmap_destroy(roots_map);
    bitmap_destroy(nodes_map);
}
void graph_randomize_labelling(Graph *graph)
{
    uint32_t idx=0;
    pthread_t *tids=malloc(graph->num_intervals*sizeof(pthread_t));
    struct thread_argument* args= malloc(graph->num_intervals*sizeof(struct thread_argument));
    
    for(idx=0;idx<graph->num_intervals;idx++)
    {
        // inizialize thread arguments
        args[idx].graph=graph; args[idx].idx=idx;
        args[idx].rank=1; 
       int err=pthread_create(&tids[idx],NULL,setting_intervals,(void*)&args[idx]);
        if(err!=0)
        {
            fprintf(stderr, "ERROR: pthread_create %d", idx);
            exit(-2);
        }
    }
    for(idx=0;idx<graph->num_intervals;idx++)
    {
       int err= pthread_join(tids[idx],NULL);
        if(err!=0)
        {
            fprintf(stderr, "ERROR: pthread_join %d", idx);
            exit(-2);
        }
    }
}
// thread fuction fo setting intervals of the labels of the given index
static void *setting_intervals(void *thread_argument)
{
    struct thread_argument *arg=(struct thread_argument *) thread_argument;
    Graph* graph= arg->graph;
    uint32_t idx=arg->idx;
    uint32_t rank = arg->rank;
    uint32_t num_roots=0;
    int i=0;
    uint32_t* roots = malloc(graph->num_root_nodes*sizeof(uint32_t));
    if(roots==NULL)
    {
        fprintf(stderr, "ERROR IN ALLOCATING roots IN: setting_intervals\n");
        pthread_exit((void*)-1);
    } 
    Bitmap* visited_nodes = bitmap_create(graph->num_nodes);
    if(visited_nodes == NULL)
    {
        fprintf(stderr, "ERROR IN ALLOCATING BITMAP IN: setting_intervals\n");
        pthread_exit((void*)-1);
    }
   /* if(graph->num_root_nodes!=0)
        i= rand_r(pthread_self())%graph->num_root_nodes;
    while(num_roots<graph->num_root_nodes)
    {
        num_roots++;
        graph_random_visit(graph,visited_nodes,graph->root_nodes[i],idx,&rank);
        i=(i+1)%graph->num_root_nodes;
    }*/
    //copy root array
    for(i=0;i<graph->num_root_nodes;i++)
        roots[i]=graph->root_nodes[i];
    ramdom_shuffle(roots,graph->num_root_nodes);
    for(i=0;i<graph->num_root_nodes;i++)
        graph_random_visit(graph,visited_nodes,roots[i],idx,&rank);
    bitmap_destroy(visited_nodes);
    pthread_exit((void*) 0);
}
//sequential random visit
static void  graph_random_visit(Graph *graph,Bitmap* visited_nodes,uint32_t node_id, uint32_t idx, uint32_t* rank)
{
    int j = 0;
    uint32_t num_childrens = 0;
    uint32_t rc = UINT32_MAX;
    Node* node;
    if(bitmap_test_bit(visited_nodes,node_id))
    {
        return;
    }
    node = graph->nodes[node_id];
    bitmap_set_bit(visited_nodes, node_id);
   if(node->num_children>0)
        j=rand_r(pthread_self())%node->num_children;
    
    while(num_childrens < node->num_children)
    {
        num_childrens++;
        graph_random_visit(graph,visited_nodes,node->children[j],idx,rank);
        j=(j+1)%node->num_children;
    }
    
    bool first = true; 
    for(j=0; j<node->num_children; j++)
    {
        if(first){
            first = false;
            rc = MIN(rc,graph->nodes[node->children[j]]->intervals[idx].left);
            continue;
        }

        int tmp_rc = MIN(rc,graph->nodes[node->children[j]]->intervals[idx].left);
        if(tmp_rc < rc)
            rc = tmp_rc;

    }

    node->intervals[idx].left = MIN(*rank,rc);
    node->intervals[idx].right = *rank;
    *rank=*rank+1;
}

static void ramdom_shuffle(uint32_t* vec,uint32_t size)
{
    uint32_t n=0,j=0,i=0,step=0;
    if(size>1000000)
        n=1000000;
    else
    {
        n=size;
    }
    for(step=0;step<n;step++)
    {
        i=rand_r(pthread_self())%size;
        j=rand_r(pthread_self())%size;
        uint32_t tmp=vec[i];
        vec[i]=vec[j];
        vec[j]=tmp;
    }
    return;
}