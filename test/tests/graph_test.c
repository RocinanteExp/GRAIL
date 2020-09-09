#include "graph.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
START_TEST (test_graph_create)
{
    Graph* graph= graph_create("test/input/grafo20.gra",5);
    ck_assert(graph!=NULL);
    ck_assert_msg(graph->num_nodes==20,"value should have been 20 but it is %d",graph->num_nodes);
    ck_assert_msg(graph->nodes[0]->id==0,"value should have been 0 but it is %d",graph->nodes[0]->id);
    ck_assert_msg(graph->nodes[0]->num_children==1,"value should have been 1 but it is %d",graph->nodes[0]->num_children);
    ck_assert_msg(graph->num_root_nodes==9,"value should have been 9 but it is %d",graph->num_root_nodes);
    ck_assert_msg(graph->root_nodes[0]==0,"value should have been 0 but it is %d",graph->root_nodes[0]);
    ck_assert_msg(graph->root_nodes[2]==2,"value should have been 2 but it is %d",graph->root_nodes[2]);
    ck_assert_msg(graph->nodes[18]->id==18,"value should have been 18 but it is %d",graph->nodes[18]->id);
    ck_assert_msg(graph->nodes[18]->num_children==0,"value should have been 0 but it is %d",graph->nodes[18]->num_children);
    ck_assert_msg(graph->num_intervals==5,"value should have been 5 but it is %d",graph->num_intervals);
    
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
    Graph *graph = graph_create("test/input/grafo20.gra", 2);
    graph_print(graph, false, -1);
    s=graph_suite();
    sr=srunner_create(s);
    srunner_run_all(sr,CK_NORMAL);
    n_fail=srunner_ntests_failed(sr);
    srunner_free(sr);
    return (n_fail==0)?EXIT_SUCCESS:EXIT_FAILURE;
}
