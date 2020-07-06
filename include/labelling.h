#ifndef LABELING_H_GUARD
#define LABELING_H_GUARD
#include "bitmap.h"
#include "label.h"
#include "graph.h"
#include <stdint.h>
/* Create the randome labels for the given graph*/
void graph_randomize_labelling_sequential(Graph *graph, uint32_t num_intevals);
//parallel fuction for labelling of the graph
void graph_randomize_labelling(Graph *graph);
#endif
