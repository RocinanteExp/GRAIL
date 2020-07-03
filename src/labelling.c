#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "labelling.h"
#include "graph.h"
#include "bitmap.h"
#include <stdint.h>
#define DEBUG 1 
// MACRO for MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
uint32_t r;
uint32_t global_rank = 0;

// function to get a random number for roots
static uint32_t get_random_order_roots(Graph *graph, Bitmap *roots_map) 
{

#if DEBUG
    uint32_t j = 0;
#else
    uint32_t j = rand() % graph->num_root_nodes;
#endif

    while(bitmap_test_bit(roots_map, j))
    {
        j = ( j + 1 ) % graph->num_root_nodes;
    }    
    return j;
}

// function to get a random number for node childrens
static uint32_t get_random_order_children(Node *node, Bitmap * child_map) 
{
#if DEBUG
    uint32_t j = 0;
#else
    uint32_t j = rand() % node->num_children;
#endif
    while(bitmap_test_bit(child_map,j))
    {
        j = ( j + 1 ) % node->num_children;
    }    
    return j;
}

// Random visit fuction that implementi the get_ramdom_order_children fuction idx indicates the current label that is being set for the node
static void graph_random_visit_sequential(Graph *graph, Bitmap *visited_nodes, uint32_t node_id, uint32_t idx)
{
    // node already visited. Return.
    if(bitmap_test_bit(visited_nodes,node_id))
    {
        return;
    }

    bitmap_set_bit(visited_nodes, node_id);

    Node *node = graph->nodes[node_id];
    Bitmap *child_map = bitmap_create(node->num_children);
    if(child_map == NULL) {
        fprintf(stderr, "bitmap create failed at graph_random_visit_sequential\n");
        exit(1);
    }

    uint32_t num_childrens = 0;
    while(num_childrens < node->num_children)
    {
        num_childrens++;
        uint32_t j = get_random_order_children(node, child_map);
        bitmap_set_bit(child_map, j);
        graph_random_visit_sequential(graph,visited_nodes,node->children[j],idx);
    }
    
    bool first = true; 
    uint32_t rc = UINT32_MAX;
    for(uint32_t j = 0; j < node->num_children; j++)
    {
        if(first){
            first = false;
            rc = graph->nodes[node->children[j]]->intervals[idx].left;
            continue;
        }

        uint32_t tmp_rc = MIN(rc, graph->nodes[node->children[j]]->intervals[idx].left);
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
    Bitmap* roots_map = bitmap_create(graph->num_root_nodes);
    if(roots_map == NULL)
    {
        fprintf(stderr, "ERROR IN ALLOCATING BITMAP IN: graph_randomize_labelling\n");
        exit(-1);
    }

    Bitmap* nodes_map = bitmap_create(graph->num_nodes);
    if(nodes_map == NULL)
    {
        fprintf(stderr, "ERROR IN ALLOCATING BITMAP IN: graph_randomize_labelling\n");
        exit(-1);
    }
    
    // idx == current traversal / interval, e.g. d = 5, idx goes from 0 to 4
    for(uint32_t idx = 0; idx < num_intevals; idx++)
    {
        // reset values at the beginning of every traversal
        global_rank = 1;
        uint32_t num_roots = 0;
        bitmap_clear_all(roots_map);
        bitmap_clear_all(nodes_map);

        // visit all roots
        while(num_roots < graph->num_root_nodes)
        {
            uint32_t i = get_random_order_roots(graph, roots_map);
            bitmap_set_bit(roots_map, i);
            num_roots++;
#if DEBUG
            fprintf(stdout, "visiting root node %d\n", graph->root_nodes[i]);
#endif
            graph_random_visit_sequential(graph, nodes_map, graph->root_nodes[i], idx);
        }
    }

    bitmap_destroy(roots_map);
    bitmap_destroy(nodes_map);
}
