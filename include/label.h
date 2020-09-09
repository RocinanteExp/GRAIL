#ifndef LABEL_H
#define LABEL_H
#include <stdint.h>
#include "bitmap.h"
#include "constants.h"

typedef struct graph_s Graph;
typedef struct node_s Node; 

typedef struct label_s {
    uint32_t left;
    uint32_t right;
} Label;

// parallel fuction for labelling of the graph
void label_generate_random_labels(Graph* graph);
// print the labels to filepath with option "w"
bool label_print_to_file(char *filename, Graph *graph); 
// check whether l1 is included in l2
bool label_include(Label l1, Label l2); 

#if TEST
void randomized_visit(Graph* graph, Bitmap* visited_nodes,uint32_t node_id, uint32_t idx, uint32_t* rank);
void vec_random_shuffle(uint32_t* vec, uint32_t size);
#endif

#endif
