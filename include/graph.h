#ifndef GRAPH_H
#define GRAPH_H
#include <stdint.h>
#include "bitmap.h"
#include "constants.h"
#include "label.h"

typedef struct node_s {
  uint32_t id;
  uint32_t *children;
  uint32_t num_children;
  Label *intervals;
  uint32_t num_intervals;
}Node;

typedef struct graph_s {
    uint32_t num_nodes;
    uint32_t num_intervals;
    uint32_t num_root_nodes;
    uint32_t *root_nodes;
    Node **nodes;
}Graph;

Graph* graph_create(const char *filepath, int num_intervals);
void graph_destroy(Graph *graph);
/* Print for each node its children node ids. If verbose is on it prints also the labels 
 * If index_node is > 0, it prints the node with id == index_node. 
 * If index_node is -1 it prints the entire graph.
 */
void graph_print_to_stdout(Graph *graph, bool verbose, uint32_t index_node);

// Print a graph to filepath with option "w"
bool graph_print_to_stream(char *graph_print_to_path, bool also_label, char *label_print_to_path, Graph *graph); 
Node* node_create_multiple(uint32_t num_intervals, const uint32_t* node_ids, uint32_t num_nodes);
#endif
