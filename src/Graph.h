#ifndef GRAPH_H_GUARD
#define GRAPH_H_GUARD
#include "bitmap.h"
#include "Label.h"
typedef struct node_s
{
  unsigned int id;
  Label* intervals;
  bitmap* interval_bitmap;
  unsigned int* childrens;
  unsigned int num_childrens;
}Node;
//Create node , d is the numbers of intervals n is the number ot childrens of the node;
Node* create_node(int d, unsigned int id);
//Free memory and destroy node
void destroy_node(Node* node);
//Set the childrens of a node using the string red from the file
void set_childrens(Node* node,char * str);
#endif