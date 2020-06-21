#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Labeling.h"
#include "Graph.h"
#include "bitmap.h"
#include <stdint.h>
#define DEBUG 1 
// MACRO for MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

uint32_t global_rank = 0;
// function to get a random number for roots
static uint32_t get_ramdom_order_roots(Graph *graph, Bitmap *roots_map) 
{

#if !DEBUG
    uint32_t j = rand() % graph->num_root_nodes;
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
    uint32_t j = rand() % node->num_children;
#else
    uint32_t j = 0;
#endif
    while(bitmap_test_bit(child_map,j))
    {
        j=(j+1)%node->num_children;
    }    
    return j;
}

static void graph_random_visit_sequential(Graph* graph, Bitmap* visited_nodes, uint32_t node_id, uint32_t idx)
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
void graph_randomize_labelling_sequential(Graph *graph, int num_intevals)
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
    for(idx = 0; idx < num_intevals; idx++)
    {
        global_rank = 1;
        bitmap_clear_all(roots_map);
        bitmap_clear_all(nodes_map);
        uint32_t i = 0;
        while(num_roots < graph->num_root_nodes)
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
