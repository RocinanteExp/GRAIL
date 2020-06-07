#ifndef GRAPH_H_GUARD
#define GRAPH_H_GUARD
#include "bitmap.h"
#include "label.h"
#include <stdint.h>

typedef struct node_s
{
  uint32_t id;
  uint32_t* children;
  uint32_t num_children;
  Label* intervals;
  Bitmap* interval_bitmap;
}Node;

typedef struct graph_s{
    uint32_t num_nodes;
    uint32_t num_intervals;
    uint32_t num_root_nodes;
    uint32_t* root_nodes;
    Node** nodes;
}Graph;

//Create node, d is the numbers of intervals n is the number ot childrens of the node;
Node* node_create(int d, uint32_t node_id);
//Free memory and destroy node
void node_destroy(Node* node);
//Set the childrens of a node using the string red from the file
void node_set_children(Node* node, char* str);

Graph* graph_create(char *filepath, int num_intervals);
void graph_destroy(Graph *graph);
/*
 * print a graph or a single node of the graph. If index_node is < 0, it will print all the nodes of the graph. 
 * The last part still needs to be implemented*/
void graph_print(Graph *graph, bool verbose, uint32_t index_node);
/*
 * print a node. If verbose is true, it is a verbose version of the print. The last part is a TODO
 * */
void node_print(Node *node, bool verbose); 

#endif
