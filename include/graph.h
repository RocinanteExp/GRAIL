#ifndef GRAPH_H_GUARD
#define GRAPH_H_GUARD
#include "bitmap.h"
#include "label.h"
#include <stdint.h>

typedef struct node_s{
  uint32_t id;
  uint32_t* children;
  uint32_t num_children;
  Label* intervals;
  Bitmap* interval_bitmap;
  uint32_t num_intervals;
}Node;

typedef struct graph_s{
    uint32_t num_nodes;
    uint32_t num_intervals;
    uint32_t num_root_nodes;
    uint32_t* root_nodes;
    Node** nodes;
}Graph;

/* Create a node with id set to node_id.  
 * If index_node is > 0, it prints the node with that id. 
 * If index_node is -1 the entire graph will be printed.
 */
//Create node, d is the numbers of intervals n is the number ot childrens of the node;
Node* node_create(uint32_t num_intervals, uint32_t node_id);
//Free memory and destroy node
void node_destroy(Node* node);

Graph* graph_create(char *filepath, int num_intervals);
void graph_destroy(Graph *graph);
/* Print a graph. If verbose is on, it will print for each node, besides the children ids, the labels 
 * If index_node is > 0, it prints the node with that id. 
 * If index_node is -1 the entire graph will be printed.
 */
void graph_print(Graph *graph, bool verbose, uint32_t index_node);
/* Print a node. In the verbose version, it will print, besides the children ids, the labels 
 */
void node_print(Node *node, bool verbose); 
int graph_print_to_file(char *filename, Graph *graph); 
void labels_print(Graph *graph);
#endif
