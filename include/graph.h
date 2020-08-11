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
  Bitmap *interval_bitmap;
  uint32_t num_intervals;
}Node;

typedef struct graph_s {
    uint32_t num_nodes;
    uint32_t num_intervals;
    uint32_t num_root_nodes;
    uint32_t *root_nodes;
    Node **nodes;
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
void graph_print_to_stdout(Graph* graph, bool verbose, uint32_t index_node);
/* Print a node. In the verbose version, it will print, besides the children ids, the labels 
 */
void node_print_to_stdout(Node* node, bool verbose);

// Print a graph to filepath with option "w"
bool graph_print_to_stream(bool to_stdout, char* graph_path_to, bool with_label, char* label_path_to, Graph* graph); 
// Print the labels to filepath with option "w"
bool label_print_to_file(char *filename, Graph *graph); 

#endif
