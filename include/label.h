#ifndef LABELING_H_GUARD
#define LABELING_H_GUARD
#include "bitmap.h"
#include "graph.h"
#include <stdint.h>
#define TEST 1
/* Create the randome labels for the given graph*/
void graph_randomize_labelling_sequential(Graph *graph, uint32_t num_intevals);
//parallel fuction for labelling of the graph
void graph_randomize_labelling(Graph *graph);

#if TEST
void graph_random_visit(Graph *graph,Bitmap* visited_nodes,uint32_t node_id, uint32_t idx, uint32_t* rank);
void ramdom_shuffle(uint32_t* vec, uint32_t size);
bool query_is_reachable_multi(uint32_t source_id, uint32_t dest_id, Graph* graph, Bitmap *vst_nodes);
#endif


typedef struct label_s
{
    uint32_t left,right;
}Label;
//Create a new label
Label label_init(uint32_t l,uint32_t r);
//see if l1 is included in l2
bool label_include(Label l1,Label l2); 

#endif
