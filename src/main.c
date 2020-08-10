#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "graph.h"
#include "label.h"
#include "query.h"
#include "constants.h"

int main(int argc, char **argv) {

#if 0
    Graph* graph = graph_create("test/input/v500000e1000.gra", 2);
    graph_randomize_labelling_sequential(graph, 2);
    query_init("test/input/v500000e1000.que", graph);
#else
    Graph* graph = graph_create("test/input/grafo20.gra", 2);
    graph_randomize_labelling(graph);
    graph_print(graph, true, -1); 
    query_init("test/input/grafo20_25.que", graph);
#endif

#if 0
    clock_t start = clock();
    graph_print_to_file("test/input/out.gra", graph); 
    clock_t end = clock();
    fprintf(stdout, "FINISCHED WRITING FILE\n");
    printf("It took %fs\n", (double)(end - start) / CLOCKS_PER_SEC);
#endif
    return 0;

}

