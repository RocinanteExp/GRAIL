#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "graph.h"
#include "label.h"
#include "query.h"
#include "constants.h"
#include "menu.h"

int main(int argc, char **argv) {
#if 0
    const char *graph_path = "../test/input/large/graph3.gra";
    const char *queue_path = "../test/input/large/graph3.que";
#elseif 0
    const char *graph_path = "test/input/grafo20.gra";
    const char *queue_path = "test/input/grafo20_25.que";
#endif


#if 0
    Graph* graph = graph_create("test/input/v500000e1000.gra", 2);
    graph_randomize_labelling_sequential(graph, 2);
    query_init("test/input/v500000e1000.que", graph);
#else
    menu(argc, argv);
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
