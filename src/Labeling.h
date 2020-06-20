#ifndef LABELING_H_GUARD
#define LABELING_H_GUARD
#include "bitmap.h"
#include "Label.h"
#include "Graph.h"
#include <stdint.h>
void graph_randomize_labelling_sequential(Graph *graph, int num_intevals);
#endif