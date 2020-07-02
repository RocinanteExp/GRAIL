#include "labelling.h"
#include "query.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>

START_TEST (graph_labelling_test)
{
    Graph *graph=graph_create("grafo.gra",2);
    graph_randomize_labelling_sequential(graph,graph->num_intervals);
    labels_print(graph);
    
    ck_assert(graph->nodes[0]->intervals[0].left!=UINT32_MAX);
    ck_assert(graph->nodes[0]->intervals[1].left!=UINT32_MAX);
    ck_assert(graph->nodes[0]->intervals[1].right!=UINT32_MAX);
}
END_TEST
START_TEST(graph_query_test)
{
    Graph *graph=graph_create("grafo.gra",2);
    graph_randomize_labelling_sequential(graph,graph->num_intervals);
    labels_print(graph);
    query_init("query.que",graph);
}
END_TEST
Suite* labelling_suite(void)
{
    Suite* s;
    TCase* tc_core;
    s=suite_create("Labelling and Query");
    tc_core= tcase_create("Core");
    tcase_add_test(tc_core, graph_labelling_test);
    tcase_add_test(tc_core, graph_query_test);
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
    s=labelling_suite();
    sr=srunner_create(s);
    srunner_run_all(sr,CK_NORMAL);
    n_fail=srunner_ntests_failed(sr);
    srunner_free(sr);
    return (n_fail==0)?EXIT_SUCCESS:EXIT_FAILURE;
}
