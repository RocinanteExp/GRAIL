#include "graph.h"
#include "label.h"
#include "query.h"
#include "menu.h"
#include "constants.h"

int main(int argc, char **argv) {
#if 0
    const char *graph_path = "test/input/large/graph3.gra";
    const char *queue_path = "test/input/large/graph3.que";
#elif 1
    const char *graph_path = "test/input/grafo20.gra";
    const char *queue_path = "test/input/grafo20_25.que";
#endif

#if 0
    Graph* graph = graph_create(graph_path, 2);
    label_generate_random_labels(graph);
    query_init(queue_path, graph);
    query_print_results("test/output/queries_out.txt");
    query_cleanup();
    graph_destroy(graph);
#else
    menu(argc, argv);
#endif

    return 0;
}
