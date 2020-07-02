#include "graph.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
START_TEST (test_graph_create)
{
    Graph* graph= graph_create("grafo.gra",2);
    ck_assert(graph->num_nodes==10);
    ck_assert(graph->nodes[0]->id==0);
    ck_assert(graph->nodes[0]->num_children==1);
    ck_assert(graph->num_root_nodes==6);
    ck_assert(graph->root_nodes[0]==0);
    ck_assert(graph->root_nodes[2]==3);
}
END_TEST
Suite* graph_suite(void)
{
    Suite* s;
    TCase* tc_core;
    s=suite_create("Graph");
    tc_core= tcase_create("Core");
    tcase_add_test(tc_core, test_graph_create);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int n_fail;
    Suite* s;
    SRunner *sr;
    Graph *graph = graph_create("grafo.gra", 2);
    graph_print(graph, false, -1);
    s=graph_suite();
    sr=srunner_create(s);
    srunner_run_all(sr,CK_NORMAL);
    n_fail=srunner_ntests_failed(sr);
    srunner_free(sr);
    return (n_fail==0)?EXIT_SUCCESS:EXIT_FAILURE;
}
